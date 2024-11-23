#pragma once
#include <vector>

#include "tokens.hpp"

class Lexer {
public:
    Lexer(std::string_view source)
        : m_source { source }
        , m_start { source.begin() }
        , m_curr { source.begin() }
    {
    }

    [[nodiscard]] auto scan() && -> std::vector<Token>&&;
    auto scan_token() -> Token;

private:
    [[nodiscard]] auto m_create_token(TokenType type) const noexcept -> Token;
    [[nodiscard]] auto m_create_errtok(std::string_view err_msg) const noexcept -> Token;
    [[nodiscard]] auto m_create_strtok() -> Token;
    void m_create_intrpltok();
    [[nodiscard]] auto m_create_numtok() noexcept -> Token;
    [[nodiscard]] auto m_create_idtok() noexcept -> Token;

    [[nodiscard]] auto m_peek() const noexcept -> char;
    [[nodiscard]] auto m_peek_next() const noexcept -> char;
    // return current character and advance current pointer position
    auto m_advance() noexcept -> char;
    // Try to match current character. If matched advance pointer position
    [[nodiscard]] auto m_match(char expected) noexcept -> bool;
    // Try to match series of characters `offset` distance from m_start pointer.
    // If matched emit a token of the given type
    [[nodiscard]] auto m_match_kwd(std::size_t offset, std::string_view expected, TokenType type) noexcept -> Token;
    void m_skip_whitespace() noexcept;
    [[nodiscard]] auto m_is_end() const noexcept -> bool;

    std::string_view m_source;
    std::string_view::const_iterator m_start { nullptr };
    std::string_view::const_iterator m_curr { nullptr };
    std::vector<Token> m_tokens;
    std::size_t m_line { 1 };
};