#pragma once

#include <utility>
#include <vector>

#include "ast.hpp"
#include "tokens.hpp"

// This was absolutely impossible for me to come up with.
// More than 80% of the class definition is from craftinginterpreters
class Parser {
    // Precedence table for the parser
    enum class Precedence : uint8_t {
        NONE,
        ASSIGNMENT,   // =
        OR,           // or
        AND,          // and
        EQUALITY,     // == !=
        COMPARISON,   // < > <= >=
        TERM,         // + -
        FACTOR,       // * /
        UNARY,        // ! -
        CALL,         // . ()
        PRIMARY
    };

    using ParserFunc = void (Parser::*)();

    struct PrattEntry {
        ParserFunc prefix;
        ParserFunc infix;
        Precedence precedence;
    };

public:
    Parser(std::vector<Token> tokens);

    // optional return type: when no value is returned means parsing failed
    auto parse() -> std::optional<StmtType>
    {
        m_declaration();
        if (m_is_parsed) {
            return std::move(m_stmt);
        }
        return {};
    }

private:
    // root function which starts the parser
    void m_declaration();
    // parent function for parsing all kinds of statements
    void m_statement();
    // parse log statements
    void m_log_statement();
    // grouping -> ( expression )
    void m_grouping();
    // parent function for all kinds of expressions
    void m_expression();
    // parse all binary expressions:
    // `+`, `-`, `*`, `/`,
    // `<`, `<=`, `>`, `>=`,
    // `!=`, `==`
    void m_binary();
    // parse all unary expressions - `!`, `-`
    void m_unary();
    // parse numbers like floats and ints
    void m_number();
    // parse literal values - string, interpolations, true and false
    void m_literal();

    // consume current character and advance
    // if encountering an error token then keep consuming all error tokens
    void m_advance();
    // auto m_match(TokenType type) -> bool;
    void m_match(TokenType type, std::string_view err_msg);
    // we report maximum errors in the parsing phase itself
    void m_report(std::vector<Token>::const_iterator token, std::string_view err_msg);
    // get the table entry for the token `type` in the pratt table
    [[nodiscard]] auto m_get_entry(TokenType type) const noexcept -> PrattEntry const&;
    // actual pratt parsing takes place here based on precedence passed
    void m_parse_precedence(Precedence prec);

    std::array<PrattEntry, std::to_underlying(TokenType::END) + 1> m_table {};
    std::vector<Token> m_tokens;
    std::vector<ExprType> m_stack;
    std::vector<Token>::const_iterator m_curr;
    std::vector<Token>::const_iterator m_prev;
    // holds all statement types
    StmtType m_stmt;
    // holds all expression types
    ExprType m_expr;
    // enables panic mode and sets is parsed to false to indicate program is incorrect
    bool m_is_panicked {};
    bool m_is_parsed { true };
};