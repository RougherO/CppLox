#include "util.hpp"
#include <algorithm>

namespace Util {
void RLE::write_line_number(std::size_t offset, std::size_t line_nr) noexcept
{
    if (m_lines.back().second != line_nr) {
        m_lines.emplace_back(offset, line_nr);
    }
}

auto RLE::read_line_number(std::size_t byte_offset) const noexcept -> std::size_t
{
    /**
     * We do a binary search over the encoding and .first + .second represents byte offset till the current line.
     * Using these we find out the current line where the byte offset possibly might be.
     * Then we simply return
     */
    auto encoding_it = std::ranges::upper_bound(m_lines.begin() + 1, m_lines.end(),
                                                byte_offset, {},
                                                [](std::pair<std::size_t, std::size_t> const& encoding) { return encoding.first; });
    encoding_it--;
    return encoding_it->second;
}
}