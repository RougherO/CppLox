#pragma once
#include <cstddef>
#include "../stack/stack.hpp"
#include "../types/pool.hpp"
#include "../types/types.hpp"
#include "../bytecode/bytecode.hpp"
// #include <expected>

class VM {
public:
    VM(ByteCode const& byte_code);
    void execute() noexcept;
    void execute_next() noexcept;
    auto get_constant(uint16_t index) const noexcept -> TypeVariant const&;
    auto get_stack() const noexcept -> Stack const&; /* Returns an immutable view of the stack */
    ByteCode const* byte_code { nullptr };

private:
    ConstPool m_const_pool {};
    size_t m_ip {};
    Stack m_stack {};
};