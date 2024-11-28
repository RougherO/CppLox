#pragma once

#ifdef NDEBUG
#include <utility>
#endif
#include "ast.hpp"

class TypeChecker {
public:
    TypeChecker(StmtType stmt)
        : m_stmt { std::move(stmt) }
    {
    }

    auto check() && -> std::optional<StmtType>;

private:
    StmtType m_stmt;
    ExprType m_expr;
};