#include <utility>

#include "compiler.hpp"
#include "instr.hpp"

namespace {
ByteCode byte_code;
StringTable string_table;
}

void emit_bytes(std::initializer_list<uint8_t> const& bytes, std::size_t line_nr)
{
    for (uint8_t byte : bytes) {
        byte_code.write_byte(byte, line_nr);
    }
}

struct ScopeVisitor {
    template <typename Derived>
    void operator()(this Derived const& self, std::shared_ptr<Scope> const& scope);
};

struct LogStmtVisitor {
    template <typename Derived>
    void operator()(this Derived const& self, std::unique_ptr<Log> const& stmt);
};

struct VarDeclVisitor {
    template <typename Derived>
    void operator()(this Derived const& self, std::unique_ptr<VarDecl> const& decl);
};

template <typename Expression, Opcode opcode>
struct BinaryExprOpcodeVisitor {
    template <typename Derived>
    void operator()(this Derived const& self, std::unique_ptr<Expression> const& expr);
};

template <typename Expression, Opcode opcode>
struct UnaryExprOpcodeVisitor {
    template <typename Derived>
    void operator()(this Derived const& self, std::unique_ptr<Expression> const& expr);
};

struct LiteralExprVisitor {
    void operator()(std::unique_ptr<Literal> const& expr) const;
};

auto Compiler::compile(std::shared_ptr<Scope> program) -> std::optional<std::pair<ByteCode, StringTable>>
{
    constexpr auto opcode_emitter = util::Visitor {
        ScopeVisitor {},
        LogStmtVisitor {},
        VarDeclVisitor {},
        BinaryExprOpcodeVisitor<Add, Opcode::ADD> {},
        BinaryExprOpcodeVisitor<Subtract, Opcode::SUB> {},
        BinaryExprOpcodeVisitor<Multiply, Opcode::MUL> {},
        BinaryExprOpcodeVisitor<Divide, Opcode::DIV> {},
        BinaryExprOpcodeVisitor<Modulus, Opcode::MOD> {},
        BinaryExprOpcodeVisitor<Compare<Order::LESS>, Opcode::CMP> {},
        BinaryExprOpcodeVisitor<Compare<Order::EQUAL>, Opcode::CMPE> {},
        BinaryExprOpcodeVisitor<Compare<Order::GREATER>, Opcode::CMP> {},
        UnaryExprOpcodeVisitor<Negate, Opcode::NEG> {},
        UnaryExprOpcodeVisitor<Not, Opcode::NOT> {},
        LiteralExprVisitor {},
    };

    opcode_emitter(program);
    // m_emit_bytes({ std::to_underlying(Opcode::RET) }, m_bc.lines().rbegin()->second);   // use the last byte's line number as return code's line number
    return std::pair { std::move(byte_code), std::move(string_table) };
}

// void Compiler::m_add_constant(std::unique_ptr<Literal> const& expr)
// {
//     auto type_index = expr->type;
//     m_emit_bytes({
//                      std::to_underlying(Opcode::LOAD),
//                      static_cast<uint8_t>(type_index),
//                  },
//                  expr->line);

//     auto emit_val = [this, line_nr = expr->line]<typename T>(T value) {
//         for (auto byte : std::bit_cast<std::array<uint8_t, sizeof(T)>>(value)) {
//             m_bc.write_byte(byte, line_nr);
//         }
//     };

//     switch (type_index) {
//         using enum TypeIndex;
//         case BOOL: emit_val(std::get<bool>(expr->value)); break;
//         case INT8: emit_val(std::get<int8_t>(expr->value)); break;
//         case INT16: emit_val(std::get<int16_t>(expr->value)); break;
//         case INT32: emit_val(std::get<int32_t>(expr->value)); break;
//         case INT64: emit_val(std::get<int64_t>(expr->value)); break;
//         case UINT8: emit_val(std::get<uint8_t>(expr->value)); break;
//         case UINT16: emit_val(std::get<uint16_t>(expr->value)); break;
//         case UINT32: emit_val(std::get<uint32_t>(expr->value)); break;
//         case UINT64: emit_val(std::get<uint64_t>(expr->value)); break;
//         case FLOAT32: emit_val(std::get<float>(expr->value)); break;
//         case FLOAT64: emit_val(std::get<double>(expr->value)); break;
//         case STRING:
//             emit_val(m_pool.emplace(
//                                std::move(std::get<std::string>(expr->value)))
//                          .first);
//             break;
//     }
// }
template <typename Derived>
void ScopeVisitor::operator()(this Derived const& self, std::shared_ptr<Scope> const& scope)
{
    for (auto const& s : scope->statements) {
        std::visit(self, s);
    }
}

/* Implementation of above template declarations */
template <typename Derived>
void LogStmtVisitor::operator()(this Derived const& self, std::unique_ptr<Log> const& stmt)
{
    std::visit(self, stmt->expr);

    emit_bytes({ Opcode::LOG }, stmt->line);
}

template <typename Expression, Opcode opcode>
template <typename Derived>
void BinaryExprOpcodeVisitor<Expression, opcode>::operator()(this Derived const& self, std::unique_ptr<Expression> const& expr)
{
    std::visit(self, expr->left);    // traverse left child
    std::visit(self, expr->right);   // traverse right child

    uint8_t op = opcode;
    if (expr->type == TypeIndex::FLOAT32 || expr->type == TypeIndex::FLOAT64) {
        op = op + 1;
    }

    emit_bytes({ op }, expr->line);   // emit the current binary expr opcode
}

template <>
template <typename Derived>
void BinaryExprOpcodeVisitor<Compare<Order::LESS>, Opcode::CMP>::operator()(this Derived const& self, std::unique_ptr<Compare<Order::LESS>> const& expr)
{
    std::visit(self, expr->left);    // traverse left child
    std::visit(self, expr->right);   // traverse right child

    Opcode cmp = expr->type == TypeIndex::FLOAT32 || expr->type == TypeIndex::FLOAT64 ? Opcode::CMPF : Opcode::CMP;
    emit_bytes({
                   cmp,
                   Opcode::LOAD,
                   std::to_underlying(TypeIndex::INT8),
                   static_cast<uint8_t>(-1),
                   Opcode::CMPE,
               },
               expr->line);   // emit `cmp` instruction
}

template <>
template <typename Derived>
void BinaryExprOpcodeVisitor<Compare<Order::GREATER>, Opcode::CMP>::operator()(this Derived const& self, std::unique_ptr<Compare<Order::GREATER>> const& expr)
{
    std::visit(self, expr->left);    // traverse left child
    std::visit(self, expr->right);   // traverse right child

    Opcode cmp = expr->type == TypeIndex::FLOAT32 || expr->type == TypeIndex::FLOAT64 ? Opcode::CMPF : Opcode::CMP;
    emit_bytes({
                   cmp,
                   Opcode::LOAD,
                   std::to_underlying(TypeIndex::INT8),
                   1,
                   Opcode::CMPE,
               },
               expr->line);   // emit `cmp` instruction
}

template <typename Expression, Opcode opcode>
template <typename Derived>
void UnaryExprOpcodeVisitor<Expression, opcode>::operator()(this Derived const& self, std::unique_ptr<Expression> const& expr)
{
    std::visit(self, expr->right);        // traverse only child

    emit_bytes({ opcode }, expr->line);   // emit the current unary expr opcode
}

void LiteralExprVisitor::operator()(std::unique_ptr<Literal> const& expr) const
{
    auto expr_type = expr->type;

    auto encoder = [&expr]<typename T>(T value) {
        for (auto byte : std::bit_cast<std::array<uint8_t, sizeof(T)>>(value)) {
            emit_bytes({ byte }, expr->line);
        }
    };

    if (expr_type == TypeIndex::STRING) {
        emit_bytes({ Opcode::LOADS }, expr->line);
        encoder(string_table.emplace(std::move(expr->word)).first);
        return;
    }

    uint64_t value {};
    std::from_chars(expr->word.c_str(), expr->word.c_str() + expr->word.size(), );
    switch (expr_type) {
        using enum TypeIndex;
        case INT8:
            emit_bytes({ Opcode::LOAD, 8 }, expr->line);
            break;
        case INT16:
            width = 16;
            break;
        case INT32:
            width = 32;
            break;
        case INT64:
            width = 64;
            break;
        case UINT8:
            width = 8;
            break;
        case UINT16:
            width = 16;
            break;
        case UINT32:
            width = 32;
            break;
        case UINT64:
            width = 64;
            break;
        default: break;
    }
}