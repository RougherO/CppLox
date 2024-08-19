#include "debugger.hpp"
#include <format>
#include <string>
#include "../util/util.hpp"
#include "../instr/instr.hpp"
#include "../vm/vm.hpp"

void Debugger::set_vm(const VM& vm) noexcept
{
    m_vm = &vm;
}

void Debugger::disassemble_byte_code(std::ostream& os, bool print_stack) const noexcept
{
    os << "+============================ Disassembled View ============================+\n";
    os << std::format("|   {:6} {:>10} {:>20} {:>20} {:>10}  |\n", "offset", "instr", "op", "op", "line_nr");
    os << "+---------------------------------------------------------------------------+\n";
    std::string disassembled_str {};
    for (size_t offset = 0; offset != m_vm->byte_code->code().size();) {
        offset = disassemble_instruction(offset, disassembled_str);
        os << disassembled_str;
        if (print_stack) {
            os << curr_stack_view();
        }
    }
    os << "+---------------------------------------------------------------------------+\n";
}

auto Debugger::disassemble_instruction(std::size_t offset, std::string& disassembled_str) const noexcept -> std::size_t
{
    switch (m_vm->byte_code->code()[offset]) {
    case Opcode::RETURN:
        disassembled_str  = std::format("|   {:06} {:>10} {:>20} {:>20} {:>10}  |\n",
                                        offset, "RETURN",
                                        "-", "-",
                                        m_vm->byte_code->read_line_number(offset));
        offset           += 1;
        break;
    case Opcode::LOAD: {
        uint8_t first_byte    = m_vm->byte_code->code()[offset + 1];   // Little Endian LSB
        uint16_t second_byte  = m_vm->byte_code->code()[offset + 2];   // Little Endian
        uint16_t index        = second_byte << 8 | first_byte;
        auto value_variant    = m_vm->get_constant(index);
        auto value            = std::visit(Util::to_string,
                                           value_variant);
        disassembled_str      = std::format("|   {:06} {:>10} {:>20} {:>20} {:>10}  |\n",
                                            offset, "LOAD",
                                            index, value,
                                            m_vm->byte_code->read_line_number(offset));
        offset               += 3;
    } break;
    default:
        disassembled_str = std::format("Invalid Opcode {} at offset {}", m_vm->byte_code->code()[offset], offset);
    }

    return offset;
}

auto Debugger::curr_stack_view() const noexcept -> std::string
{
    std::string stack_view_str;
    for (std::size_t sp {}; sp != m_vm->get_stack().curr_stack_ptr(); ++sp) {
        stack_view_str = std::format("[ {} ]\n", std::visit(Util::to_string, m_vm->get_stack()[sp]));
    }
    return stack_view_str;
}