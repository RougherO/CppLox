#pragma once
#include <string_view>
#include <vector>
#include <string>
#include "tokens.hpp"

class Lexer {
public:
    Lexer(std::string const& source);
    auto scan() -> std::vector<Token>;

private:
    auto m_scan_token() -> Token;
    auto m_create_token(TokenType type) const noexcept -> Token;
    auto m_create_errtok(std::string_view err_msg) const noexcept -> Token;
    auto m_create_strtok() noexcept -> Token;
    auto m_create_numtok() noexcept -> Token;
    auto m_create_idtok() noexcept -> Token;

    auto m_peek() const noexcept -> char;
    auto m_peek_next() const noexcept -> char;
    auto m_advance() noexcept -> char;                                               // return current character and advance current pointer position
    auto m_match(char expected) noexcept -> bool;                                    // Try to match current character. If matched advance pointer position
    auto m_match_kwd(std::string_view expected, TokenType type) noexcept -> Token;   // Try to match series of characters. If matched emit a token of the given type
    void m_skip_whitespace() noexcept;
    auto m_is_end() const noexcept -> bool;

    std::string_view m_source {};
    std::string_view::const_iterator m_start { nullptr };
    std::string_view::const_iterator m_curr { nullptr };
    std::size_t m_line { 1 };
};