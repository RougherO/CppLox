#include "vm.hpp"
#include "debugger.hpp"
#include <iostream>

int main()
{
    ByteCode bc;
    // bc.write_byte(Opcode::RETURN, 0);
    // bc.write_byte(Opcode::LOAD, 1);
    bc.write_byte(0, 1);
    bc.write_byte(0, 1);
    bc.write_byte(0b1000, 2);
    bc.write_byte(0b1001, 2);
    bc.write_byte(0b1100, 2);

    bc.read_line_number(0);
    bc.read_line_number(1);
    bc.read_line_number(2);
    bc.read_line_number(3);
    bc.read_line_number(4);

    VM vm { bc };

    Debugger dbg;
    dbg.set_vm(vm);
    dbg.disassemble_byte_code(std::cout, true);
}