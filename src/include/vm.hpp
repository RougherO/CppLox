#pragma once

#include <variant>
#include <array>

#include "code_segment.hpp"

class Stack {
public:
    using value_type = std::variant<bool,
                                    StringPtr,
                                    int64_t, uint64_t,
                                    double>;

    [[nodiscard]] auto top() const noexcept -> value_type
    {
        return m_stack[m_sptr - 1];
    }

    auto push(value_type value) noexcept -> std::size_t
    {
        m_stack[m_sptr] = value;
        return m_sptr++;
    }

    auto pop() noexcept -> value_type
    {
        return m_stack[--m_sptr];
    }

private:
    static constexpr uint16_t max_size = 8'192;
    std::size_t m_sptr {};
    std::array<value_type, max_size> m_stack {};
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

    Stack m_stack {};
    StringTable m_pool;
    ByteCode m_bc {};
    std::size_t m_iptr {};
};