#include "lexer.hpp"
#include <utility>
#include <cctype>
#include <iterator>
#include <algorithm>

Lexer::Lexer(std::string_view source)
    : m_source { source }
    , m_start { m_source.begin() }
    , m_curr { m_source.begin() }
{
}

auto Lexer::scan() && -> std::vector<Token>&&
{
    for (m_start = m_curr = m_source.cbegin(); !m_is_end();) {
        m_tokens.emplace_back(scan_token());
    }
    return std::move(m_tokens);
}

auto Lexer::scan_token() -> Token
{
    m_skip_whitespace();

    m_start = m_curr;

    if (m_is_end()) {
        return m_create_token(TokenType::EOF);
    }

    char c = m_advance();

    if (std::isdigit(c)) return m_create_numtok();
    if (std::isalpha(c) || c == '_') return m_create_idtok();

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
        case '*': return m_create_token(STAR);

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
        case '"': return m_create_strtok();
        default: return m_create_errtok("Unexpected character token");
    }
    std::unreachable();
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
                if (m_peek() == '{') {
                    m_create_intrpltok();
                }
                break;
        }
        m_advance();
    }

    if (m_is_end()) {
        return m_create_errtok("Unterminated string");
    }

    m_advance();   // consume '"' character
    return m_create_token(TokenType::STRING);
}

void Lexer::m_create_intrpltok()
{
    while (!m_is_end() && m_advance() != '}') {
        m_tokens.emplace_back(std::move(scan_token()));
    }

    if (m_is_end()) {
        m_tokens.emplace_back(std::move(m_create_errtok("Expected closing braces '}' for interpolation")));
    }

    m_tokens.emplace_back(std::move(m_create_token(TokenType::RIGHT_BRACE)));
}

auto Lexer::m_create_numtok() noexcept -> Token
{
    bool is_float {};
    while (std::isdigit(m_peek())) m_advance();
    if (m_peek() == '.' && std::isdigit(m_peek_next())) {
        is_float = true;
        m_advance();
        while (std::isdigit(m_peek())) m_advance();
    }

    return m_create_token(is_float ? TokenType::FLOAT : TokenType::INT);
}

auto Lexer::m_create_idtok() noexcept -> Token
{
    while (!m_is_end() && (std::isalnum(m_peek()) || m_peek() == '_')) m_advance();

    switch (*m_start) {
        using enum TokenType;
        case 'a': return m_match_kwd(1, "nd", AND);
        case 'c': return m_match_kwd(1, "lass", CLASS);
        case 'e': return m_match_kwd(1, "lse", ELSE);
        case 'f':
            if (m_curr - m_start > 1) {
                switch (*std::next(m_start, 1)) {
                    case 'a': return m_match_kwd(2, "lse", FALSE);
                    case 'o': return m_match_kwd(2, "r", FOR);
                    case 'u': return m_match_kwd(2, "n", FUN);
                }
            }
            break;
        case 'i': return m_match_kwd(1, "f", IF);
        case 'l': return m_match_kwd(1, "et", LET);
        case 'n': return m_match_kwd(1, "il", NIL);
        case 'o': return m_match_kwd(1, "r", OR);
        case 'p': return m_match_kwd(1, "rint", PRINT);
        case 'r': return m_match_kwd(1, "eturn", RETURN);
        case 's': return m_match_kwd(1, "uper", SUPER);
        case 't':
            if (m_curr - m_start > 1) {
                switch (*std::next(m_start, 1)) {
                    case 'h': return m_match_kwd(2, "is", THIS);
                    case 'r': return m_match_kwd(2, "ue", TRUE);
                }
            }
        case 'w': return m_match_kwd(1, "hile", WHILE);
    }

    return m_create_token(TokenType::IDENTIFIER);
}

auto Lexer::m_peek() const noexcept -> char
{
    return *m_curr;
}

auto Lexer::m_peek_next() const noexcept -> char
{
    auto it = std::next(m_curr, 1);
    [[unlikely]] if (it == m_source.end())
        return '\0';
    return *it;
}

auto Lexer::m_advance() noexcept -> char
{
    return *m_curr++;
}

auto Lexer::m_match(char expected) noexcept -> bool
{
    if (m_is_end()) return false;
    if (*m_curr != expected) return false;
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
                    while (!m_is_end() || m_peek() != '\n') m_advance();
                }
                break;
            default:
                return;
        }
    }
}

auto Lexer::m_is_end() const noexcept -> bool
{
    return m_curr == m_source.end();
}