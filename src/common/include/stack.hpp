#pragma once
#include "types.hpp"
#include <array>
#include <limits>

struct Stack {
    inline static uint16_t constexpr max_stack_size = std::numeric_limits<uint16_t>::max();
    void push_value(TypeVariant value) noexcept;
    TypeVariant pop_value() noexcept;
    auto get_stack() const noexcept -> std::array<TypeVariant, max_stack_size> const&;
    auto curr_stack_ptr() const noexcept -> std::uint16_t;

    /**
     * Using c++23 `deducing this` and returning reference based on context
     * See https://www.modernescpp.com/index.php/c23-deducing-this/ "Deduplicating Function Overloading"
     *
     * Something to think about is why auto&& and not decltype(auto)
     * for return type as decltype(auto) returns prvalue as types T
     * and not reference. Otherwise both are similar.
     *
     * Also providing funciton definition in class definition itself as
     * return type is deduced type so depends on return value. Without return
     * type we cannot use this function.
     */
    auto&& operator[](this auto&& self, std::size_t index) noexcept
    {
        return self.m_frames[index];
    }

private:
    uint16_t m_stack_pointer {};
    std::array<TypeVariant, max_stack_size> m_frames {};
};