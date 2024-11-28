#include <utility>

#include "compiler.hpp"
#include "instr.hpp"

template <typename Statement, Opcode opcode>
struct StmtOpcodeVisitor {
    template <typename Derived>
    void operator()(this Derived const& self, std::unique_ptr<Statement> const& stmt);
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

auto Compiler::compile() && -> std::pair<ByteCode, std::unordered_set<std::string>>
{
    auto opcode_emitter = util::Visitor {
        StmtOpcodeVisitor<Log, Opcode::LOG> {},
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
        [this](std::unique_ptr<Literal> const& expr) { m_add_constant(expr); },                                  // for literal expression we simply pass the hardwork on to m_add_constant
        [this](Opcode opcode, std::size_t line_nr) { m_emit_bytes({ std::to_underlying(opcode) }, line_nr); },   // adding an opcode overload to simply call the visitor and emit an opcode after visiting all child nodes
    };

    std::visit(opcode_emitter, m_ast);
    m_emit_bytes({ std::to_underlying(Opcode::RET) }, m_bc.lines().rbegin()->second);   // use the last byte's line number as return code's line number

    return std::pair { std::move(m_bc), std::move(m_pool) };
}

void Compiler::m_add_constant(std::unique_ptr<Literal> const& expr)
{
    auto type_index = expr->type;
    m_emit_bytes({
                     std::to_underlying(Opcode::LOAD),
                     static_cast<uint8_t>(type_index),
                 },
                 expr->line);

    auto emit_val = [this, line_nr = expr->line]<typename T>(T value) {
        for (auto byte : std::bit_cast<std::array<uint8_t, sizeof(T)>>(value)) {
            m_bc.write_byte(byte, line_nr);
        }
    };

    switch (type_index) {
        using enum TypeIndex;
        case BOOL: emit_val(std::get<bool>(expr->value)); break;
        case INT8: emit_val(std::get<int8_t>(expr->value)); break;
        case INT16: emit_val(std::get<int16_t>(expr->value)); break;
        case INT32: emit_val(std::get<int32_t>(expr->value)); break;
        case INT64: emit_val(std::get<int64_t>(expr->value)); break;
        case UINT8: emit_val(std::get<uint8_t>(expr->value)); break;
        case UINT16: emit_val(std::get<uint16_t>(expr->value)); break;
        case UINT32: emit_val(std::get<uint32_t>(expr->value)); break;
        case UINT64: emit_val(std::get<uint64_t>(expr->value)); break;
        case FLOAT32: emit_val(std::get<float>(expr->value)); break;
        case FLOAT64: emit_val(std::get<double>(expr->value)); break;
        case STRING:
            emit_val(m_pool.emplace(
                               std::move(std::get<std::string>(expr->value)))
                         .first);
            break;
    }
}

void Compiler::m_emit_bytes(std::initializer_list<uint8_t> opcodes, std::size_t line_nr)
{
    for (uint8_t opcode : opcodes) {
        m_bc.write_byte(opcode, line_nr);
    }
}

/* Implementation of above template declarations */
template <>
template <typename Derived>
void StmtOpcodeVisitor<Log, Opcode::LOG>::operator()(this Derived const& self, std::unique_ptr<Log> const& stmt)
{
    std::visit(self, stmt->expr);

    self(Opcode::LOG, stmt->line);
}

template <typename Expression, Opcode opcode>
template <typename Derived>
void BinaryExprOpcodeVisitor<Expression, opcode>::operator()(this Derived const& self, std::unique_ptr<Expression> const& expr)
{
    std::visit(self, expr->left);    // traverse left child
    std::visit(self, expr->right);   // traverse right child

    self(opcode, expr->line);        // emit the current binary expr opcode
}

template <>
template <typename Derived>
void BinaryExprOpcodeVisitor<Compare<Order::LESS>, Opcode::CMP>::operator()(this Derived const& self, std::unique_ptr<Compare<Order::LESS>> const& expr)
{
    std::visit(self, expr->left);    // traverse left child
    std::visit(self, expr->right);   // traverse right child

    self(Opcode::CMP, expr->line);   // emit `cmp` instruction
    self(Opcode::LOAD, expr->line);
    self(static_cast<Opcode>(TypeIndex::INT8), expr->line);
    self(static_cast<Opcode>(-1), expr->line);   // LOAD -1 and compare whether its true
    self(Opcode::CMPE, expr->line);
}

template <>
template <typename Derived>
void BinaryExprOpcodeVisitor<Compare<Order::GREATER>, Opcode::CMP>::operator()(this Derived const& self, std::unique_ptr<Compare<Order::GREATER>> const& expr)
{
    std::visit(self, expr->left);    // traverse left child
    std::visit(self, expr->right);   // traverse right child

    self(Opcode::CMP, expr->line);   // emit `cmp` instruction
    self(Opcode::LOAD, expr->line);
    self(static_cast<Opcode>(TypeIndex::INT8), expr->line);
    self(static_cast<Opcode>(1), expr->line);   // LOAD -1 and compare whether its true
    self(Opcode::CMPE, expr->line);
}

template <typename Expression, Opcode opcode>
template <typename Derived>
void UnaryExprOpcodeVisitor<Expression, opcode>::operator()(this Derived const& self, std::unique_ptr<Expression> const& expr)
{
    std::visit(self, expr->right);   // traverse only child

    self(opcode, expr->line);        // emit the current unary expr opcode
}
