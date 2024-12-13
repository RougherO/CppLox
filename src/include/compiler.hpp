#pragma once

#include <optional>

#include "string.hpp"
#include "bytecode.hpp"
#include "ast.hpp"

namespace Compiler {
[[nodiscard]] auto compile(std::shared_ptr<Scope> program) -> std::optional<std::pair<ByteCode, StringTable>>;
}
