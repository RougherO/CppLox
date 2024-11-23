#pragma once
#include <string>
#include <unordered_set>

using StringTable = std::unordered_set<std::string>;
using StringPtr   = std::unordered_set<std::string>::const_iterator;