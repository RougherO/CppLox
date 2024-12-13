#pragma once

#include "ast.hpp"

namespace SemanticChecker {
auto check(std::shared_ptr<Scope> const& program) -> bool;
}