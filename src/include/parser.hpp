#pragma once

#include <utility>
#include <vector>

#include "ast.hpp"
#include "tokens.hpp"

// This was absolutely impossible for me to come up with.
// More than 70% of the class definition is from craftinginterpreters
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
    [[nodiscard]] auto parse() -> std::optional<std::shared_ptr<Scope>>
    {
        m_declaration();
        if (m_is_parsed) {
            return std::move(m_curr_scope);
        }
        return {};
    }

private:
    // root function which starts the parser
    void m_declaration();
    // parent function for parsing all kinds of statements
    void m_statement();
    // create block statements
    void m_block_statement();
    // parse log statements
    void m_log_statement();
    // parse variable declarations
    void m_variable_declaration();
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

    // calm down the parser and start parsing again
    void m_relax();
    // consume current character and advance
    // if encountering an error token then keep consuming all error tokens
    void m_advance();
    auto m_match(TokenType type) -> bool;
    void m_match(TokenType type, std::string_view err_msg);
    template <typename Expression>
    void m_make_binary_expression();
    template <typename Expression>
    void m_make_unary_expression();
    void m_make_literal_expression(TypeIndex type);
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
    // keeps track of the current scope
    std::shared_ptr<Scope> m_curr_scope { std::make_shared<Scope>() };
    // holds all statement types
    StmtType m_stmt;
    // holds all expression types
    ExprType m_expr;
    // enables panic mode and sets is parsed to false to indicate program is incorrect
    bool m_is_panicked {};
    bool m_is_parsed { true };
};