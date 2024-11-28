#include "type_check.hpp"
#include "static_report.hpp"

template <typename Statement>
struct StmtTypeCheckVisitor {
    template <typename Derived>
    auto operator()(this Derived const& self, std::unique_ptr<Statement> const& stmt) -> TypeIndex;
};

template <typename Expression>
struct BinaryTypeCheckVisitor {
    template <typename Derived>
    auto operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> TypeIndex;
};

template <typename Expression>
struct UnaryTypeCheckVisitor {
    template <typename Derived>
    auto operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> TypeIndex;
};

struct LiteralTypeCheckVisitor {
    auto operator()(std::unique_ptr<Literal> const& expr) const -> TypeIndex;
};

auto TypeChecker::check() && -> std::optional<StmtType>
{
    constexpr auto type_check_visitor = util::Visitor {
        StmtTypeCheckVisitor<Log> {},
        BinaryTypeCheckVisitor<Add> {},
        BinaryTypeCheckVisitor<Subtract> {},
        BinaryTypeCheckVisitor<Multiply> {},
        BinaryTypeCheckVisitor<Divide> {},
        BinaryTypeCheckVisitor<Modulus> {},
        BinaryTypeCheckVisitor<Compare<Order::LESS>> {},
        BinaryTypeCheckVisitor<Compare<Order::EQUAL>> {},
        BinaryTypeCheckVisitor<Compare<Order::GREATER>> {},
        UnaryTypeCheckVisitor<Negate> {},
        UnaryTypeCheckVisitor<Not> {},
        LiteralTypeCheckVisitor {},
    };

    auto program_type = std::visit(type_check_visitor, m_stmt);
    if (program_type == TypeIndex::NONE) {
        return std::move(m_stmt);
    }

    return {};
}

/**
 * Statements should always return NONE type, since statements cannot have types.
 * Any other type than NONE type means something went wrong so type checking failed.
 * We should report failure in such case
 */
template <>
template <typename Derived>
auto StmtTypeCheckVisitor<Log>::operator()(this Derived const& self, std::unique_ptr<Log> const& stmt) -> TypeIndex
{
    auto expr_type = std::visit(self, stmt->expr);

    if (expr_type == TypeIndex::NONE) {   // expression should have some valid type
        static_report::report(stmt, "Could not infer type");
        return TypeIndex::STRING;         // returing string type instead of `none` type => Error occured
    }

    return TypeIndex::NONE;
}

template <typename Expression>
template <typename Derived>
auto BinaryTypeCheckVisitor<Expression>::operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> TypeIndex
{
    // Traverse left and right expressions
    auto left_type  = std::visit(self, expr->left);
    auto right_type = std::visit(self, expr->right);

    if constexpr (std::is_same_v<Expression, Add>) {
        // For add operation we check if either of them are strings. If they are we allow conatanation
        if (!(left_type == TypeIndex::STRING || right_type == TypeIndex::STRING) || left_type != right_type) {
            static_report::report(expr, "Cannot perform '+' on expressions of different types. Concatanation needs at least one of them to be string");
            return TypeIndex::NONE;
        }
    } else {
        // If both expression are of same type we allow binary operation
        if (left_type != right_type) {
            static_report::report(expr, std::format("Cannot perform '{}' on expressions of different types", expr->word));
            return TypeIndex::NONE;
        }
    }

    if (left_type == TypeIndex::NONE) {
        static_report::report(expr, "Could not infer type");
        return TypeIndex::NONE;
    }

    // Update expression type
    return expr->type = left_type;
}

template <typename Expression>
template <typename Derived>
auto UnaryTypeCheckVisitor<Expression>::operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> TypeIndex
{
    // Traverse right expression
    auto right_type = std::visit(self, expr->right);

    if constexpr (std::is_same_v<Expression, Negate>) {
        using enum TypeIndex;
        if (right_type == UINT8
            || right_type == UINT16
            || right_type == UINT32
            || right_type == UINT64) {
            static_report::report(expr, "Cannot perform unary '-' on unsigned numeric expression");
            return NONE;
        }
    }

    if (right_type == TypeIndex::NONE) {
        static_report::report(expr, "Could not infer type");
        return TypeIndex::NONE;
    }

    // Update expression type
    return expr->type = right_type;
}

auto LiteralTypeCheckVisitor::operator()(std::unique_ptr<Literal> const& expr) const -> TypeIndex
{
    switch (expr->pseudo_type) {
        using enum TokenType;
        case BOOL:
        case TRUE:
        case FALSE: expr->type = TypeIndex::BOOL; return TypeIndex::BOOL;
        // case INT8: expr->type = TypeIndex::INT8; return;
        // case INT16: expr->type = TypeIndex::INT16; return;
        case INT32: expr->type = TypeIndex::INT32; return TypeIndex::INT32;
        // case INT64: expr->type = TypeIndex::INT64; return;
        // case UINT8: expr->type = TypeIndex::UINT8; return;
        // case UINT16: expr->type = TypeIndex::UINT16; return;
        case UINT32: expr->type = TypeIndex::UINT32; return TypeIndex::UINT32;
        // case UINT64: expr->type = TypeIndex::UINT64; return;
        // case FLOAT32: expr->type = TypeIndex::FLOAT32; return;
        case FLOAT64: expr->type = TypeIndex::FLOAT64; return TypeIndex::FLOAT64;
        case STRING: expr->type = TypeIndex::STRING; return TypeIndex::STRING;
        default:
            expr->type = TypeIndex::NONE;
            static_report::report(expr, "Couldn't infer literal type");
            return TypeIndex::NONE;
    }
}