#include "bytecode.hpp"

auto ByteCode::code() const noexcept -> std::vector<uint8_t> const&
{
    return m_code;
}

void ByteCode::write_byte(uint8_t b, std::size_t line_nr)
{
    m_code.push_back(b);
    m_line_info.write_line_number(line_nr);
}

auto ByteCode::read_line_number(std::size_t offset) const noexcept -> std::size_t
{
    return m_line_info.read_line_number(offset);
}