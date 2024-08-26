#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <limits>
#include <array>
#include <memory>
#include <type_traits>
#include <ranges>
#include "types.hpp"

namespace Util {
inline auto to_string = [](LoxType auto elem) {
    auto val = std::to_string(elem.value);

    if constexpr (std::is_same_v<decltype(elem), Float>) {
        auto trimmed_zero_view = val
                               | std::views::reverse
                               | std::views::drop_while([](char c) { return c == '0'; })
                               | std::views::reverse;
        val.assign(trimmed_zero_view.begin(), trimmed_zero_view.end());
        if (val.back() == '.') {
            val.push_back('0');
        }
        return val;
    }

    return val;
};

class RLE {
public:
    /**
     * Writes each instruction's corresponding source line number.
     *
     * Uses RLE(run-length encoding to write line info) into a seperate line array;
     *
     * If any runtime error occurs. This line information can be
     * used to extract where the error occured in source.
     */
    void write_line_number(std::size_t offset, std::size_t line_nr) noexcept;
    /**
     * .first => line number
     * .second => first byte offset for that line, i.e., which byte's offset starts at that line.
     * Uses binary search over (.first + .second) to calculate which line has the offset just
     * less than or equal to `byte_offset`.
     */
    auto read_line_number(std::size_t byte_offset) const noexcept -> std::size_t;

private:
    std::vector<std::pair<std::size_t, std::size_t>> m_lines {
        { 0, 0 }
    };
};

class ConstPool {
public:
    [[nodiscard]] auto get_elem(uint16_t index) const noexcept -> TypeVariant const& { return *m_elems[index]; }

private:
    inline static uint16_t constexpr max_size = std::numeric_limits<uint16_t>::max();
    std::array<std::unique_ptr<TypeVariant>, max_size> m_elems { std::make_unique<TypeVariant>(Int { 1 }) };
};
}