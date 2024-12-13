#include "semantic_check.hpp"
#include "static_report.hpp"

namespace {
bool m_valid { true };
}

struct ScopeSemanticVisitor {
    template <typename Derived>
    auto operator()(this Derived const& self, std::shared_ptr<Scope> const& scope) -> TypeIndex;
};

template <typename Statement>
struct StmtSemanticVisitor {
    template <typename Derived>
    auto operator()(this Derived const& self, std::unique_ptr<Statement> const& stmt) -> TypeIndex;
};

template <typename Expression>
struct BinarySemanticVisitor {
    template <typename Derived>
    auto operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> TypeIndex;
};

template <typename Expression>
struct UnarySemanticVisitor {
    template <typename Derived>
    auto operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> TypeIndex;
};

struct LiteralSemanticVisitor {
    auto operator()(std::unique_ptr<Literal> const& expr) const -> TypeIndex;
};

auto SemanticChecker::check(std::shared_ptr<Scope> const& program) -> bool
{
    constexpr auto semantic_check_visitor = util::Visitor {
        ScopeSemanticVisitor {},
        StmtSemanticVisitor<VarDecl> {},
        StmtSemanticVisitor<Log> {},
        BinarySemanticVisitor<Add> {},
        BinarySemanticVisitor<Subtract> {},
        BinarySemanticVisitor<Multiply> {},
        BinarySemanticVisitor<Divide> {},
        BinarySemanticVisitor<Modulus> {},
        BinarySemanticVisitor<Compare<Order::LESS>> {},
        BinarySemanticVisitor<Compare<Order::EQUAL>> {},
        BinarySemanticVisitor<Compare<Order::GREATER>> {},
        UnarySemanticVisitor<Negate> {},
        UnarySemanticVisitor<Not> {},
        LiteralSemanticVisitor {},
    };

    semantic_check_visitor(program);
    return m_valid;
}

/**
 * At every statement we will see if its corresponding expr was semantically correct
 * If not, we report it at every statement node and again start checking as if everything's fine
 * This way we report as many errors as we can. Every statement is like a synchronization point for
 * the semantic analyzer after it panics when program has errors.
 */
template <typename Derived>
auto ScopeSemanticVisitor::operator()(this Derived const& self, std::shared_ptr<Scope> const& scope) -> TypeIndex
{
    for (auto const& s : scope->statements) {
        std::visit(self, s);
    }

    return TypeIndex::NONE;
}

template <>
template <typename Derived>
auto StmtSemanticVisitor<VarDecl>::operator()(this Derived const& self, std::unique_ptr<VarDecl> const& stmt) -> TypeIndex
{
    auto curr_scope = stmt->curr_scope.lock();
    auto& table     = curr_scope->table;

    if (auto entry = table.find(stmt->name); entry != table.end()) {
        static_report::report(
            stmt,
            std::format("Cannot declare variable '{}' as it already exists at [line: {}]",
                        stmt->name, entry->second->line));
        m_valid = false;
        return TypeIndex::NONE;
    }

    auto expr_type = std::visit(self, stmt->expr);
    if (expr_type == TypeIndex::NONE) {   // failed to infer assigned expression type
        static_report::report(stmt, std::format("Could not infer type for assigned expression"));
        m_valid = false;
        return TypeIndex::NONE;
    }

    if (auto var_type = stmt->type; var_type == TypeIndex::NONE) {   // means we need to infer types
        stmt->type = expr_type;
    } else if (var_type != expr_type) {
        static_report::report(
            stmt,
            std::format("Type mismatch, expected '{}' but got '{}'",
                        util::type::to_string(var_type), util::type::to_string(expr_type)));
        m_valid = false;
        return TypeIndex::NONE;
    }

    table.emplace(stmt->name, &stmt);   // add entry of this variable to the table of current scope

    return TypeIndex::NONE;
}

template <>
template <typename Derived>
auto StmtSemanticVisitor<Log>::operator()(this Derived const& self, std::unique_ptr<Log> const& stmt) -> TypeIndex
{
    auto expr_type = std::visit(self, stmt->expr);

    if (expr_type == TypeIndex::NONE) {
        static_report::report(stmt, "Cannot log this expression as it is incorrect");
    }
    return TypeIndex::NONE;
}

template <typename Expression>
template <typename Derived>
auto BinarySemanticVisitor<Expression>::operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> TypeIndex
{
    // Traverse left and right expressions
    auto left_type  = std::visit(self, expr->left);
    auto right_type = std::visit(self, expr->right);

    // If either expression fails this expression fails as well
    if (left_type == TypeIndex::NONE || right_type == TypeIndex::NONE) {
        return TypeIndex::NONE;
    }

    // Add expressions allow different types only if one of them is a string
    if constexpr (std::is_same_v<Expression, Add>) {
        if (left_type != right_type) {
            if (left_type == TypeIndex::STRING || right_type == TypeIndex::STRING) {
                return TypeIndex::STRING;
            }
            static_report::report(
                expr,
                "Cannot perform '+' operation due to type mismatch, '{}' & '{}'."
                " Concatanation needs at least one of them to be string");
            return TypeIndex::NONE;
        }
    } else {
        // For any other expressions performing binary operation on different types is an error
        if (left_type != right_type) {
            static_report::report(
                expr,
                std::format("Cannot perform '{}' operation due to type mismatch, '{}' & '{}'. Expect expressions of same type",
                            expr->word, util::type::to_string(left_type), util::type::to_string(right_type)));
            return TypeIndex::NONE;
        }
    }

    return expr->type = left_type;
}

template <typename Expression>
template <typename Derived>
auto UnarySemanticVisitor<Expression>::operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> TypeIndex
{
    auto expr_type = std::visit(self, expr->right);

    // Negate operation is not allowed on unsigned types
    if constexpr (std::is_same_v<Expression, Negate>) {
        using enum TypeIndex;
        if (expr_type == UINT8
            || expr_type == UINT16
            || expr_type == UINT32
            || expr_type == UINT64) {
            static_report::report(expr, "Cannot perform unary '-' on unsigned numeric expression");
            return NONE;
        }
    }

    return expr->type = expr_type;
}

auto LiteralSemanticVisitor::operator()(std::unique_ptr<Literal> const& expr) const -> TypeIndex
{
    return expr->type;
}
