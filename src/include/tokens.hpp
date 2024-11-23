#pragma once
#include <string_view>
#include <cstdint>

enum class TokenType : uint8_t {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    SEMICOLON,
    COLON,
    INTRPL,   // string interpolation "My name is ${name}"

    // operator
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    AS,
    DOT,

    // logical ops
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    AND,
    OR,

    // types
    CHAR,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
    BOOL,
    STRING,

    IDENTIFIER,

    CLASS,
    ELSE,
    FALSE,
    FOR,
    FUN,
    IF,
    LET,
    NIL,
    LOG,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    WHILE,

    ERROR,   // extra error token which we use in parser to catch errors while lexing
    END,
};

struct Token {
    TokenType type;
    std::string_view word;
    std::size_t line;
};