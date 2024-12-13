#pragma once

#include <array>

#include "code_segment.hpp"

class ExecutionStack {
public:
    static constexpr uint16_t max_size = 8 * 1'024;
    using iterator                     = std::array<uint64_t, max_size>::iterator;

    auto push(uint64_t value) noexcept -> std::size_t
    {
        *m_sptr = value;
        m_sptr++;
        return std::distance(m_stack.begin(), m_sptr) - 1;
    }

    auto pop() noexcept -> iterator
    {
        return --m_sptr;
    }

private:
    std::array<uint64_t, max_size> m_stack {};   // 8K elements of 8B size => 8 * 8KB stack
    iterator m_sptr {};
};

class VM {
public:
    VM(CodeSegment seg)
        : m_pool { std::move(seg.second) }
        , m_bc { std::move(seg.first) }
    {
    }
    void execute();
    void execute_next();

private:
    auto m_is_end() noexcept -> bool
    {
        return m_iptr == m_bc.code().size();
    }

    ExecutionStack m_stack {};
    StringTable m_pool;
    ByteCode m_bc {};
    std::size_t m_iptr {};
};