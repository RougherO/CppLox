#pragma once
#include "types.hpp"
#include <cstdint>
#include <limits>
#include <array>
#include <memory>

class ConstPool {
public:
    [[nodiscard]] auto get_elem(uint16_t index) const noexcept -> TypeVariant const& { return *m_elems[index]; }

private:
    inline static uint16_t constexpr max_size = std::numeric_limits<uint16_t>::max();
    std::array<std::unique_ptr<TypeVariant>, max_size> m_elems { std::make_unique<TypeVariant>(Int { 1 }) };
};