#include "util.hpp"
#include <algorithm>

namespace Util {
void RLE::write_line_number(std::size_t line_nr) noexcept
{
    if (m_lines.back().first == line_nr) { /* current encoding's line number is the same as the line being written to */
        m_lines.back().second++;
        return;
    }
    /* Else new line is being written on to */
    m_lines.emplace_back(line_nr, 1);
}

auto RLE::read_line_number(std::size_t byte_offset) const noexcept -> std::size_t
{
    /**
     * We do a binary search over the encoding and .first + .second represents byte offset till the current line.
     * Using these we find out the current line where the byte offset possibly might be.
     * Then we simply return
     */
    auto encoding_it = std::ranges::upper_bound(m_lines, byte_offset, {}, [](auto const& encoding) { return encoding.first + encoding.second; });
    return encoding_it->first;
}
}