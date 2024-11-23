#pragma once

#include "ast.hpp"
#include "code_segment.hpp"

class Compiler {
public:
    Compiler(StmtType ast)
        : m_ast { std::move(ast) }
    {
    }

    [[nodiscard]] auto compile() && -> CodeSegment;

private:
    void m_add_constant(std::unique_ptr<Literal> const& expr);
    void m_emit_bytes(std::initializer_list<uint8_t> opcodes, std::size_t line_nr);

    StringTable m_pool;
    ByteCode m_bc;
    StmtType m_ast;
};