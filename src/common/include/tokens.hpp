#pragma once
#include <string_view>
#include <cstddef>

enum class TokenType {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    IDENTIFIER,
    STRING,
    NUMBER,
    INT,
    FLOAT,

    AND,
    CLASS,
    ELSE,
    FALSE,
    FOR,
    FUN,
    IF,
    LET,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    WHILE,

    ERROR,
#undef EOF   // EOF is #defined somewhere causing problems with Token definition
    EOF,
};

struct Token {
    TokenType type;
    std::string_view word;
    std::size_t line;
};