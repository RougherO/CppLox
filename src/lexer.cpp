#include <cctype>
#include <iterator>
#include <algorithm>
#ifndef NDEBUG
#include <print>
#include <iostream>
#else
#include <utility>
#endif

#include "lexer.hpp"

using namespace std::string_view_literals;

auto Lexer::scan() && -> std::vector<Token>&&
{
    while (!m_is_end()) {
        m_tokens.emplace_back(scan_token());
    }
    if (m_tokens.back().type != TokenType::END) {
        m_start = m_curr;
        m_tokens.emplace_back(m_create_token(TokenType::END));
    }

    return std::move(m_tokens);
}

auto Lexer::scan_token() -> Token
{
    m_skip_whitespace();

    m_start = m_curr;   // start scanning every token with m_curr and m_start pointing to the start of the token

    if (m_is_end()) {
        return m_create_token(TokenType::END);
    }

    char c = m_advance();

    if (std::isdigit(c)) {
        return m_create_numtok();
    }
    if (std::isalpha(c) || c == '_') {
        return m_create_idtok();
    }

    switch (c) {
        using enum TokenType;
        case '(': return m_create_token(LEFT_PAREN);
        case ')': return m_create_token(RIGHT_PAREN);
        case '{': return m_create_token(LEFT_BRACE);
        case '}': return m_create_token(RIGHT_BRACE);
        case ';': return m_create_token(SEMICOLON);
        case ',': return m_create_token(COMMA);
        case '.': return m_create_token(DOT);
        case '-': return m_create_token(MINUS);
        case '+': return m_create_token(PLUS);
        case '/': return m_create_token(SLASH);
        case '%': return m_create_token(PERCENT);
        case '*': return m_create_token(STAR);
        case ':': return m_create_token(COLON);

        case '!': return m_match('=')
                           ? m_create_token(BANG_EQUAL)
                           : m_create_token(BANG);
        case '=': return m_match('=')
                           ? m_create_token(EQUAL_EQUAL)
                           : m_create_token(EQUAL);
        case '<': return m_match('=')
                           ? m_create_token(LESS_EQUAL)
                           : m_create_token(LESS);
        case '>': return m_match('=')
                           ? m_create_token(GREATER_EQUAL)
                           : m_create_token(GREATER);
        case '"': m_start++; return m_create_strtok();
        default: return m_create_errtok("Unexpected character token");
    }
#ifndef NDEBUG
    std::println(std::cerr, "[DEBUG] No tokens matched");
#else
    std::unreachable();
#endif
}

auto Lexer::m_create_token(TokenType type) const noexcept -> Token
{
    return Token {
        type,
        std::string_view { m_start, m_curr },
        m_line,
    };
}

auto Lexer::m_create_errtok(std::string_view err_msg) const noexcept -> Token
{
    return Token {
        TokenType::ERROR,
        err_msg,
        m_line,
    };
}

auto Lexer::m_create_strtok() -> Token
{
    for (char c {}; !m_is_end() && (c = m_peek()) != '"';) {
        switch (c) {
            case '\n':
                m_line++;
                break;
            case '$':
                if (m_peek_next() == '{') {
                    m_create_intrpltok();
                    m_start = m_curr;   // after '}' collect rest of the characters in `string` token
                    continue;           // do not advance continue from current character
                }
                break;
            default:
                m_advance();
        }
    }

    if (m_is_end()) {
        return m_create_errtok("Unterminated string");
    }

    Token token = m_create_token(TokenType::STRING);
    m_advance();   // consume '"' character
    return token;
}

void Lexer::m_create_intrpltok()
{
    m_tokens.emplace_back(m_create_token(TokenType::INTRPL));
    m_advance();   // skip '$'
    m_advance();   // skip `{`

    while (!m_is_end() && m_tokens.back().type != TokenType::RIGHT_BRACE) {
        m_tokens.emplace_back(scan_token());
    }
}

auto Lexer::m_create_numtok() noexcept -> Token
{
    TokenType type { TokenType::INT32 };

    while (std::isdigit(m_peek())) {
        m_advance();
    }
    if (m_peek() == '.' && std::isdigit(m_peek_next())) {
        type = TokenType::FLOAT64;
        m_advance();
        while (std::isdigit(m_peek())) {
            m_advance();
        }
        if (m_peek() == 'f') {
            type = TokenType::FLOAT32;
            m_advance();
        }
    }

    return m_create_token(type);
}

auto Lexer::m_create_idtok() noexcept -> Token
{
    while (!m_is_end() && (std::isalnum(m_peek()) || m_peek() == '_')) {
        m_advance();
    }

    switch (*m_start) {
        using enum TokenType;
        case 'a': return m_match_kwd(1, "nd", AND);
        case 'c':
            if (std::distance(m_start, m_curr) > 1) {
                switch (*std::next(m_start)) {
                    case 'h': return m_match_kwd(2, "ar", CHAR);
                    case 'l': return m_match_kwd(2, "ass", CLASS);
                    default: break;
                }
            }
            break;
        case 'e': return m_match_kwd(1, "lse", ELSE);
        case 'f':
            if (std::distance(m_start, m_curr) > 1) {
                switch (*std::next(m_start)) {
                    case '3': return m_match_kwd(2, "oat", FLOAT32);
                    case '6': return m_match_kwd(2, "oat", FLOAT64);
                    case 'a': return m_match_kwd(2, "lse", FALSE);
                    case 'o': return m_match_kwd(2, "r", FOR);
                    case 'u': return m_match_kwd(2, "n", FUN);
                    default: break;
                }
            }
            break;
        case 'i':
            if (std::distance(m_start, m_curr) > 1) {
                switch (*std::next(m_start)) {
                    case '1': return m_match_kwd(2, "6", INT16);
                    case '3': return m_match_kwd(2, "2", INT32);
                    case '6': return m_match_kwd(2, "4", INT64);
                    case '8': return m_match_kwd(1, "8", INT8);
                    case 'f': return m_match_kwd(1, "f", IF);
                    default: break;
                }
            }
            break;
        case 'l':
            if (std::distance(m_start, m_curr) > 1) {
                switch (*std::next(m_start)) {
                    case 'e': return m_match_kwd(2, "t", LET);
                    case 'o': return m_match_kwd(2, "g", LOG);
                    default: break;
                }
            }
            break;
        case 'n': return m_match_kwd(1, "il", NIL);
        case 'o': return m_match_kwd(1, "r", OR);
        case 'r': return m_match_kwd(1, "eturn", RETURN);
        case 's':
            if (std::distance(m_start, m_curr) > 1) {
                switch (*std::next(m_start)) {
                    case 't': return m_match_kwd(2, "ring", STRING);
                    case 'u': return m_match_kwd(2, "per", SUPER);
                    default: break;
                }
            }
        case 't':
            if (std::distance(m_start, m_curr) > 1) {
                switch (*std::next(m_start)) {
                    case 'h': return m_match_kwd(2, "is", THIS);
                    case 'r': return m_match_kwd(2, "ue", TRUE);
                    default: break;
                }
            }
            break;
        case 'u':
            if (std::distance(m_start, m_curr) > 1) {
                switch (*std::next(m_start)) {
                    case '1': return m_match_kwd(2, "6", UINT16);
                    case '3': return m_match_kwd(2, "2", UINT32);
                    case '6': return m_match_kwd(2, "4", UINT64);
                    case '8': return m_match_kwd(1, "8", UINT8);
                    default: break;
                }
            }
            break;
        case 'w': return m_match_kwd(1, "hile", WHILE);
        default:
            break;
    }

    return m_create_token(TokenType::IDENTIFIER);
}

auto Lexer::m_peek() const noexcept -> char
{
    return *m_curr;
}

auto Lexer::m_peek_next() const noexcept -> char
{
    auto const* it = std::next(m_curr);
    [[unlikely]] if (it == m_source.end()) {
        return '\0';
    }
    return *it;
}

auto Lexer::m_advance() noexcept -> char
{
    return *m_curr++;
}

auto Lexer::m_match(char expected) noexcept -> bool
{
    if (m_is_end()) {
        return false;
    }
    if (*m_curr != expected) {
        return false;
    }
    m_advance();
    return true;
}

auto Lexer::m_match_kwd(std::size_t offset, std::string_view expected, TokenType type) noexcept -> Token
{
    return std::ranges::equal(std::next(m_start, offset), m_curr, expected.begin(), expected.end())
             ? m_create_token(type)
             : m_create_token(TokenType::IDENTIFIER);
}

void Lexer::m_skip_whitespace() noexcept
{
    while (!m_is_end()) {
        char c = m_peek();
        switch (c) {
            case '\n':
                m_line++;
            case ' ':
            case '\r':
            case '\t':
                m_advance();
                break;
            case '/':
                if (m_peek_next() == '/') {
                    while (!m_is_end() && m_peek() != '\n') {
                        m_advance();
                    }
                } else {
                    return;
                }
                break;
            default: return;
        }
    }
}

auto Lexer::m_is_end() const noexcept -> bool
{
    return m_curr == m_source.end();
}