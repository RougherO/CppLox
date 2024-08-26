#include "lexer.hpp"
#include <utility>
#include <cctype>
#include <iterator>
#include <algorithm>

Lexer::Lexer(std::string const& source)
    : m_source { source }
{
}

auto Lexer::scan() -> std::vector<Token>
{
    std::vector<Token> tokens {};
    for (m_start = m_curr = m_source.cbegin(); m_curr != m_source.cend();) {
    }
    tokens.emplace_back(TokenType::EOF, "", m_line);
    return tokens;
}

auto Lexer::m_scan_token() -> Token
{
    m_skip_whitespace();

    m_start = m_curr;

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
        default: return m_create_token(ERROR);
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

auto Lexer::m_create_strtok() noexcept -> Token
{
    for (char c {}; (c = m_advance()) != '"';) {
        if (m_is_end()) return m_create_errtok("Unterminated string");
        if (c == '\n') m_line++;
    }

    return m_create_token(TokenType::STRING);
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
    while (std::isalnum(m_peek()) || m_peek() == '_') m_advance();

    switch (*m_start) {
        using enum TokenType;
        case 'a': return m_match_kwd("nd", AND);
        case 'c': return m_match_kwd("lass", CLASS);
        case 'e': return m_match_kwd("lse", ELSE);
        case 'f':
            if (m_curr - m_start > 1) {
                switch (*std::next(m_start, 1)) {
                    case 'a': return m_match_kwd("lse", FALSE);
                    case 'o': return m_match_kwd("r", FOR);
                    case 'u': return m_match_kwd("n", FUN);
                }
            }
            break;
        case 'i': return m_match_kwd("f", IF);
        case 'n': return m_match_kwd("il", NIL);
        case 'o': return m_match_kwd("r", OR);
        case 'p': return m_match_kwd("rint", PRINT);
        case 'r': return m_match_kwd("eturn", RETURN);
        case 's': return m_match_kwd("uper", SUPER);
        case 't':
            if (m_curr - m_start > 1) {
                switch (*std::next(m_start, 1)) {
                    case 'h': return m_match_kwd("is", THIS);
                    case 'r': return m_match_kwd("ue", TRUE);
                }
            }
        case 'l': return m_match_kwd("et", LET);
        case 'w': return m_match_kwd("hile", WHILE);
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

auto Lexer::m_match_kwd(std::string_view expected, TokenType type) noexcept -> Token
{
    return std::ranges::equal(m_start, m_curr, expected.begin(), expected.end())
             ? m_create_token(type)
             : m_create_token(TokenType::IDENTIFIER);
}

void Lexer::m_skip_whitespace() noexcept
{
    while (true) {
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