#pragma once

namespace util {
template <typename... Vs>
struct Visitor : Vs... {
    using Vs::operator()...;
};
}
