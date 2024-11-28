#pragma once
#include <memory>
#include "common.hpp"
#include "tokens.hpp"
#include "types.hpp"

/* binary expr types */
struct Add;
struct Subtract;
struct Multiply;
struct Divide;
struct Modulus;
enum class Order : uint8_t {
    LESS,
    EQUAL,
    GREATER,
};
template <Order>
struct Compare;
/* unary expr types */
struct Negate;
struct Not;
struct Literal;

using ExprType = std::variant<std::unique_ptr<Add>,
                              std::unique_ptr<Subtract>,
                              std::unique_ptr<Multiply>,
                              std::unique_ptr<Divide>,
                              std::unique_ptr<Modulus>,
                              std::unique_ptr<Compare<Order::LESS>>,
                              std::unique_ptr<Compare<Order::EQUAL>>,
                              std::unique_ptr<Compare<Order::GREATER>>,
                              std::unique_ptr<Negate>,
                              std::unique_ptr<Not>,
                              std::unique_ptr<Literal>>;

struct Expr {
    std::string word;
    std::size_t line;                     // store line in source code
    TypeIndex type { TypeIndex::NONE };   // actual cpplox type
};

struct Literal : Expr {
    TokenType pseudo_type;   // temporary token type
};

struct Binary : Expr {
    ExprType left;
    ExprType right;
};

struct Unary : Expr {
    ExprType right;
};

/* Binary */
struct Add : Binary { };
struct Subtract : Binary { };
struct Multiply : Binary { };
struct Divide : Binary { };
struct Modulus : Binary { };
template <Order>
struct Compare : Binary { };
/* Unary */
struct Negate : Unary { };
struct Not : Unary { };

/* stmt types */
struct Log;

using StmtType = std::variant<std::unique_ptr<Log>>;

struct Stmt {
    std::size_t line;
};

struct Log : Stmt {
    ExprType expr;
};

namespace util {
namespace literal {
    namespace {
        template <typename Type>
        struct LiteralValueToStrVisitor {
            auto operator()(Type const& elem) const -> std::string
            {
                return std::format("{}", elem);
            }
        };
    }

    // inline constexpr Visitor to_string = std::apply([]<typename... Ts>(Ts...) { return Visitor { LiteralValueToStrVisitor<Ts> {}... }; }, TypeList {});
}
namespace ast {
    namespace {
        template <typename Statement>
        struct StmtToStrVisitor {
            template <typename Derived>
            auto operator()(this Derived const& self, std::unique_ptr<Statement> const& stmt) -> std::string;
        };

        template <typename Expression>
        struct BinaryExprToStrVisitor {
            template <typename Derived>
            auto operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> std::string;
        };

        template <typename Expression>
        struct UnaryExprToStrVisitor {
            template <typename Derived>
            auto operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> std::string;
        };

        struct LiteralExprToStrVisitor {
            auto operator()(std::unique_ptr<Literal> const& expr) const -> std::string const&;
        };

        template <>
        template <typename Derived>
        auto StmtToStrVisitor<Log>::operator()(this Derived const& self, std::unique_ptr<Log> const& stmt) -> std::string
        {
            std::string expr = std::visit(self, stmt->expr);
            return std::format("[[Log]]\v>{}", std::move(expr));
        }

        template <typename Expression>
        template <typename Derived>
        auto BinaryExprToStrVisitor<Expression>::operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> std::string
        {
            std::string left  = std::visit(self, expr->left);
            std::string right = std::visit(self, expr->right);
            return std::format("[{}]\v>{} {}", expr->word, std::move(left), std::move(right));
        }

        template <typename Expression>
        template <typename Derived>
        auto UnaryExprToStrVisitor<Expression>::operator()(this Derived const& self, std::unique_ptr<Expression> const& expr) -> std::string
        {
            std::string right = std::visit(self, expr->right);
            return std::format("[{}]\v>{}", expr->word, std::move(right));
        }

        auto LiteralExprToStrVisitor::operator()(std::unique_ptr<Literal> const& expr) const -> std::string const&
        {
            return expr->word;
        }
    }

    inline constexpr Visitor to_string {
        StmtToStrVisitor<Log> {},
        BinaryExprToStrVisitor<Add> {},
        BinaryExprToStrVisitor<Subtract> {},
        BinaryExprToStrVisitor<Multiply> {},
        BinaryExprToStrVisitor<Divide> {},
        BinaryExprToStrVisitor<Modulus> {},
        BinaryExprToStrVisitor<Compare<Order::LESS>> {},
        BinaryExprToStrVisitor<Compare<Order::EQUAL>> {},
        BinaryExprToStrVisitor<Compare<Order::GREATER>> {},
        UnaryExprToStrVisitor<Negate> {},
        UnaryExprToStrVisitor<Not> {},
        LiteralExprToStrVisitor {},
    };
}
namespace type {
    // auto get_type(ExprType const& expr_type) -> TypeIndex
    // {
    //     return std::visit(util::Visitor {
    //                           []<typename T>(std::unique_ptr<T> const& expr) { return expr->type; } },
    //                       expr_type);
    // }
    // auto get_type(Expr const& expr) -> TypeIndex
    // {
    //     return expr.type;
    // }
}
}
