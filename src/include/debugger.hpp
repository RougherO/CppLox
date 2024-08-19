#pragma once
#include <ostream>
#include <string>
#include <cstddef>
#include "vm.hpp"

struct Debugger {
    void set_vm(VM const& vm) noexcept;
    void disassemble_byte_code(std::ostream& os, bool print_stack = false) const noexcept;
    auto disassemble_instruction(std::size_t offset, std::string& disassembled_str) const noexcept -> std::size_t;
    auto curr_stack_view() const noexcept -> std::string;

private:
    VM const* m_vm { nullptr };
};
