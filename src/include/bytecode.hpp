#pragma once
#include <vector>
#include <cstdint>
#include <algorithm>

namespace util {
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
    [[nodiscard]] auto lines() const noexcept -> std::vector<std::pair<std::size_t, std::size_t>> const&
    {
        return m_lines;
    }

    void write_line_number(std::size_t offset, std::size_t line_nr) noexcept
    {
        if (m_lines.empty() || m_lines.back().second != line_nr) {
            m_lines.emplace_back(offset, line_nr);
        }
    }
    /**
     * .first => byte offset
     * .second => line number
     * Uses binary search over .first to calculate where the offset lies
     */
    [[nodiscard]] auto read_line_number(std::size_t byte_offset) const noexcept -> std::size_t
    {
        /**
         * We do a binary search over the encoding, .first represents byte offset till the current or less offset.
         * Using these we find out the current line where the byte offset possibly might be.
         * Then we simply return
         */
        auto it = std::ranges::upper_bound(m_lines.begin(), m_lines.end(),
                                           byte_offset, {},
                                           [](auto const& encoding) { return encoding.first; });

        return std::prev(it)->second;
    }

private:
    std::vector<std::pair<std::size_t, std::size_t>> m_lines;
};
}

class ByteCode {
public:
    [[nodiscard]] auto code() const noexcept -> std::vector<uint8_t> const&
    {
        return m_code;
    }
    [[nodiscard]] auto lines() const noexcept -> decltype(auto)
    {
        return m_line_info.lines();
    }
    void write_byte(uint8_t b, std::size_t line_nr) /* Used to write a single byte `opcode` `operand` `line information` etc. */
    {
        m_line_info.write_line_number(m_code.size(), line_nr);
        m_code.push_back(b);
    }
    [[nodiscard]] auto read_line_number(std::size_t offset) const noexcept -> std::size_t
    {
        return m_line_info.read_line_number(offset);
    }

private:
    std::vector<uint8_t> m_code;
    util::RLE m_line_info {};
};