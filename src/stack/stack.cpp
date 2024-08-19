#include "stack.hpp"

void Stack::push_value(TypeVariant value) noexcept
{
    m_frames[m_stack_pointer++] = value;
}

TypeVariant Stack::pop_value() noexcept
{
    return m_frames[--m_stack_pointer];
}

auto Stack::curr_stack_ptr() const noexcept -> uint16_t
{
    return m_stack_pointer;
}