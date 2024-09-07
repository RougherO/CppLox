#include "lexer.hpp"
#include "gtest/gtest.h"
#include <utility>
#include <tuple>
#include <ostream>

struct LexerTest : testing::Test {
protected:
    void SetUp() override
    {
        lexer_with_int    = new Lexer(int_s);
        lexer_with_float  = new Lexer(float_s);
        lexer_with_string = new Lexer(string_s);
        lexer_with_kwd    = new Lexer(keyword_s);

        lexer_with_single_quote_s = new Lexer(single_quote_s);

        lexer_with_interpolated_s              = new Lexer(interpolated_s);
        lexer_with_nested_interpolated_s       = new Lexer(nested_interpolated_s);
        lexer_with_no_end_brace_interpolated_s = new Lexer(no_end_brace_interpolated_s);
        lexer_with_no_end_quote_interpolated_s = new Lexer(no_end_quote_interpolated_s);
        lexer_with_simple_string_with_dollar_s = new Lexer(simple_string_with_dollar_s);
    }

    virtual void TearDown() override
    {
        delete lexer_with_int;
        delete lexer_with_float;
        delete lexer_with_string;
        delete lexer_with_kwd;
        delete lexer_with_single_quote_s;
        delete lexer_with_interpolated_s;
        delete lexer_with_nested_interpolated_s;
        delete lexer_with_no_end_brace_interpolated_s;
        delete lexer_with_no_end_quote_interpolated_s;
        delete lexer_with_simple_string_with_dollar_s;
    }

    std::string_view int_s = "1 2 3 4 \n"
                             "5 6 7 \n"
                             "8";

    std::string_view float_s = "1.0 2.0 3.0\n"
                               "1.333 4.56 8.89";

    std::string_view string_s = "\"Sample string\" \"Sample string part two\" \n"
                                "\"Sample string part 3\" \"Non terminating string test";

    std::string_view keyword_s = " and "
                                 " class "
                                 " else "
                                 " false "
                                 " for "
                                 " if "
                                 " let "
                                 " nil "
                                 " or "
                                 " print "
                                 " return "
                                 " super "
                                 " this "
                                 " true "
                                 " while "
                                 " keyword ";

    std::string_view single_quote_s = "\"";

    std::string_view interpolated_s = " \"Initial msg ${Hello + 1}\" ";

    std::string_view nested_interpolated_s = " \"${Hello \"${1 + 2}\" - 3}\" ";

    std::string_view no_end_brace_interpolated_s = " \"${Hello\" ";

    std::string_view no_end_quote_interpolated_s = " \"${Hello} ";

    std::string_view simple_string_with_dollar_s = " \"$Hello\" ";

    Lexer* lexer_with_float;
    Lexer* lexer_with_int;
    Lexer* lexer_with_string;
    Lexer* lexer_with_kwd;
    Lexer* lexer_with_single_quote_s;
    Lexer* lexer_with_interpolated_s;
    Lexer* lexer_with_nested_interpolated_s;
    Lexer* lexer_with_no_end_brace_interpolated_s;
    Lexer* lexer_with_no_end_quote_interpolated_s;
    Lexer* lexer_with_simple_string_with_dollar_s;
};

auto operator==(Token const& t1, Token const& t2)
{
    return std::tie(t1.type, t1.word, t1.line) == std::tie(t2.type, t2.word, t2.line);
}

auto operator<<(std::ostream& os, Token const& t) -> std::ostream&
{
    return os << std::format("Token {{type: {}, word: '{}', line: {}}}", (int)t.type, t.word, t.line);
}

TEST_F(LexerTest, IntLexTest)
{
    using enum TokenType;
    Token result;
    result.type = INT;

    result.word = "1";
    result.line = 1;
    EXPECT_EQ(lexer_with_int->scan_token(), result);
    result.word = "2";
    EXPECT_EQ(lexer_with_int->scan_token(), result);
    result.word = "3";
    EXPECT_EQ(lexer_with_int->scan_token(), result);
    result.word = "4";
    EXPECT_EQ(lexer_with_int->scan_token(), result);
    result.word = "5";
    result.line = 2;
    EXPECT_EQ(lexer_with_int->scan_token(), result);
    result.word = "6";
    EXPECT_EQ(lexer_with_int->scan_token(), result);
    result.word = "7";
    EXPECT_EQ(lexer_with_int->scan_token(), result);
    result.word = "8";
    result.line = 3;
    EXPECT_EQ(lexer_with_int->scan_token(), result);
}

TEST_F(LexerTest, FloatLexTest)
{
    using enum TokenType;
    Token result;
    result.type = FLOAT;

    result.word = "1.0";
    result.line = 1;
    EXPECT_EQ(lexer_with_float->scan_token(), result);
    result.word = "2.0";
    EXPECT_EQ(lexer_with_float->scan_token(), result);
    result.word = "3.0";
    EXPECT_EQ(lexer_with_float->scan_token(), result);
    result.word = "1.333";
    result.line = 2;
    EXPECT_EQ(lexer_with_float->scan_token(), result);
    result.word = "4.56";
    EXPECT_EQ(lexer_with_float->scan_token(), result);
    result.word = "8.89";
    EXPECT_EQ(lexer_with_float->scan_token(), result);
}

TEST_F(LexerTest, StringLexTest)
{
    using enum TokenType;
    Token result;
    result.type = STRING;

    result.word = "\"Sample string\"";
    result.line = 1;
    EXPECT_EQ(lexer_with_string->scan_token(), result);
    result.word = "\"Sample string part two\"";
    EXPECT_EQ(lexer_with_string->scan_token(), result);
    result.word = "\"Sample string part 3\"";
    result.line = 2;
    EXPECT_EQ(lexer_with_string->scan_token(), result);
    result.type = ERROR;
    result.word = "Unterminated string";
    EXPECT_EQ(lexer_with_string->scan_token(), result);
}

TEST_F(LexerTest, KeywordLexTest)
{
    using enum TokenType;
    Token result;

    result.type = AND;
    result.word = "and";
    result.line = 1;
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = CLASS;
    result.word = "class";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = ELSE;
    result.word = "else";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = FALSE;
    result.word = "false";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = FOR;
    result.word = "for";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = IF;
    result.word = "if";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = LET;
    result.word = "let";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = NIL;
    result.word = "nil";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = OR;
    result.word = "or";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = PRINT;
    result.word = "print";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = RETURN;
    result.word = "return";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = SUPER;
    result.word = "super";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = THIS;
    result.word = "this";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = TRUE;
    result.word = "true";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = WHILE;
    result.word = "while";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
    result.type = IDENTIFIER;
    result.word = "keyword";
    EXPECT_EQ(lexer_with_kwd->scan_token(), result);
}

TEST_F(LexerTest, SingleQuoteString)
{
    using enum TokenType;
    Token result;

    result.type = ERROR;
    result.line = 1;
    result.word = "Unterminated string";
    EXPECT_EQ(lexer_with_single_quote_s->scan_token(), result);
}

TEST_F(LexerTest, StringInterPolationTest)
{
    using enum TokenType;
    Token result;

    auto tokens = std::move(*lexer_with_interpolated_s).scan();

    /* Exclusively use bound checking access to catch as many errors as possible */
    result.type = INTRPL;
    result.word = "\"Initial msg ";
    result.line = 1;
    EXPECT_EQ(tokens.at(0), result);
    result.type = IDENTIFIER;
    result.word = "Hello";
    EXPECT_EQ(tokens.at(1), result);
    result.type = PLUS;
    result.word = "+";
    EXPECT_EQ(tokens.at(2), result);
    result.type = INT;
    result.word = "1";
    EXPECT_EQ(tokens.at(3), result);
    result.type = RIGHT_BRACE;
    result.word = "}";
    EXPECT_EQ(tokens.at(4), result);
    result.type = STRING;
    result.word = "\"";
    EXPECT_EQ(tokens.at(5), result);
}

TEST_F(LexerTest, NestedStringInterPolationTest)
{
    using enum TokenType;
    Token result;

    auto tokens = std::move(*lexer_with_nested_interpolated_s).scan();

    /* Exclusively use bound checking access to catch as many errors as possible */
    result.type = INTRPL;
    result.word = "\"";
    result.line = 1;
    EXPECT_EQ(tokens.at(0), result);
    result.type = IDENTIFIER;
    result.word = "Hello";
    EXPECT_EQ(tokens.at(1), result);
    result.type = INTRPL;
    result.word = "\"";
    EXPECT_EQ(tokens.at(2), result);
    result.type = INT;
    result.word = "1";
    EXPECT_EQ(tokens.at(3), result);
    result.type = PLUS;
    result.word = "+";
    EXPECT_EQ(tokens.at(4), result);
    result.type = INT;
    result.word = "2";
    EXPECT_EQ(tokens.at(5), result);
    result.type = RIGHT_BRACE;
    result.word = "}";
    EXPECT_EQ(tokens.at(6), result);
    result.type = STRING;
    result.word = "\"";
    EXPECT_EQ(tokens.at(7), result);
    result.type = MINUS;
    result.word = "-";
    EXPECT_EQ(tokens.at(8), result);
    result.type = INT;
    result.word = "3";
    EXPECT_EQ(tokens.at(9), result);
    result.type = RIGHT_BRACE;
    result.word = "}";
    EXPECT_EQ(tokens.at(10), result);
    result.type = STRING;
    result.word = "\"";
    EXPECT_EQ(tokens.at(11), result);
    result.type = END;
    result.word = "";
    EXPECT_EQ(tokens.at(12), result);
}

TEST_F(LexerTest, NoEndBraceInterPolatedStringTest)
{
    using enum TokenType;
    Token result;

    auto tokens = std::move(*lexer_with_no_end_brace_interpolated_s).scan();

    /* Exclusively use bound checking access to catch as many errors as possible */
    result.type = INTRPL;
    result.word = "\"";
    result.line = 1;
    EXPECT_EQ(tokens.at(0), result);
    result.type = IDENTIFIER;
    result.word = "Hello";
    EXPECT_EQ(tokens.at(1), result);
    result.type = ERROR;
    result.word = "Unterminated string";
    EXPECT_EQ(tokens.at(2), result);
    result.type = ERROR;
    result.word = "Expected closing braces '}'";
    EXPECT_EQ(tokens.at(3), result);
}

TEST_F(LexerTest, NoEndQuoteInterPolatedStringTest)
{
    using enum TokenType;
    Token result;

    auto tokens = std::move(*lexer_with_no_end_quote_interpolated_s).scan();

    /* Exclusively use bound checking access to catch as many errors as possible */
    result.type = INTRPL;
    result.word = "\"";
    result.line = 1;
    EXPECT_EQ(tokens.at(0), result);
    result.type = IDENTIFIER;
    result.word = "Hello";
    EXPECT_EQ(tokens.at(1), result);
    result.type = RIGHT_BRACE;
    result.word = "}";
    EXPECT_EQ(tokens.at(2), result);
    result.type = ERROR;
    result.word = "Unterminated string";
    EXPECT_EQ(tokens.at(3), result);
}