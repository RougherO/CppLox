#include "vm.hpp"
#include "../instr/instr.hpp"
#include "../util/util.hpp"

// #define DEBUG_EXEC_TRACE
// #define DEBUG_STACK_TRACE
#ifdef DEBUG_EXEC_TRACE
#include <iostream>
#include <string>
#include "../debugger/debugger.hpp"
inline static Debugger debugger;
#endif

VM::VM(ByteCode const& bc)
    : byte_code { &bc }
{
}

void VM::execute() noexcept
{
#ifdef DEBUG_EXEC_TRACE
    debugger.set_vm(*this);
    std::string disassembled_str {};
#endif
    for (; m_ip != byte_code->code().size();) {
#ifdef DEBUG_EXEC_TRACE
        debugger.disassemble_instruction(m_ip, disassembled_str);
        std::cout << disassembled_str;
#ifdef DEBUG_STACK_TRACE
        std::cout << debugger.curr_stack_view();
#endif
#endif
        execute_next();
    }
}

void VM::execute_next() noexcept
{
    switch (byte_code->code()[m_ip]) {
    case Opcode::RETURN:
        m_ip += 1;
        break;
    case Opcode::LOAD: {
        uint8_t first_byte   = byte_code->code()[m_ip + 1];
        uint16_t second_byte = byte_code->code()[m_ip + 2];
        uint16_t index       = second_byte << 8 | first_byte;

        auto value_variant = m_const_pool.get_elem(index);
        auto value         = std::visit(Util::to_string,
                                        value_variant);

        m_ip += 3;
    } break;
    default:
        break;
    }
}

auto VM::get_constant(uint16_t index) const noexcept -> TypeVariant const&
{
    return m_const_pool.get_elem(index);
}

auto VM::get_stack() const noexcept -> Stack const&
{
    return m_stack;
}