#pragma once
#include <string>
#include <vector>
#include "../types/types.hpp"

namespace Util {
inline auto to_string = [](LoxType auto elem) {
    return std::to_string(elem.value);
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
    void write_line_number(std::size_t line_nr) noexcept;
    /**
     * .first => line number
     * .second => count of bytes belong on that line number.
     * Uses binary search over (.first + .second) to calculate which line has the byte_offset
     */
    auto read_line_number(std::size_t byte_offset) const noexcept -> std::size_t;

private:
    std::vector<std::pair<std::size_t, std::size_t>> m_lines {
        { 0, 0 }
    };
};
}