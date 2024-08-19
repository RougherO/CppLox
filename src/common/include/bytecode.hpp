#pragma once
#include <cstdint>
#include <vector>
#include "util.hpp"

/**
 * Byte Code is in Little Endian format
 */
class ByteCode {
public:
    auto code() const noexcept -> std::vector<uint8_t> const&;
    void write_byte(uint8_t b, std::size_t line_nr); /* Used to write a single byte `opcode` `operand` `line information` etc. */
    auto read_line_number(std::size_t offset) const noexcept -> std::size_t;

private:
    std::vector<uint8_t> m_code {};
    Util::RLE m_line_info {};
};