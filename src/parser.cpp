/**
    declaration -> classDecl | funcDecl | varDecl | statement
    statement   -> exprStmt | forStmt | ifStmt | logStmt | returnStmt | whileStmt | block
 */

#include <format>
#include <string_view>

#include "static_report.hpp"
#include "parser.hpp"

Parser::Parser(std::vector<Token> tokens)
    : m_tokens { std::move(tokens) }
    , m_curr { m_tokens.cbegin() }
    , m_prev { nullptr }
{
    m_table[std::to_underlying(TokenType::LEFT_PAREN)]    = { &Parser::m_grouping, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::RIGHT_PAREN)]   = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::LEFT_BRACE)]    = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::RIGHT_BRACE)]   = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::INTRPL)]        = { &Parser::m_literal, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::COMMA)]         = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::DOT)]           = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::MINUS)]         = { &Parser::m_unary, &Parser::m_binary, Precedence::TERM };
    m_table[std::to_underlying(TokenType::PLUS)]          = { nullptr, &Parser::m_binary, Precedence::TERM };
    m_table[std::to_underlying(TokenType::SEMICOLON)]     = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::SLASH)]         = { nullptr, &Parser::m_binary, Precedence::FACTOR };
    m_table[std::to_underlying(TokenType::PERCENT)]       = { nullptr, &Parser::m_binary, Precedence::FACTOR };
    m_table[std::to_underlying(TokenType::STAR)]          = { nullptr, &Parser::m_binary, Precedence::FACTOR };
    m_table[std::to_underlying(TokenType::BANG)]          = { &Parser::m_unary, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::EQUAL)]         = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::BANG_EQUAL)]    = { nullptr, &Parser::m_binary, Precedence::EQUALITY };
    m_table[std::to_underlying(TokenType::EQUAL_EQUAL)]   = { nullptr, &Parser::m_binary, Precedence::EQUALITY };
    m_table[std::to_underlying(TokenType::GREATER)]       = { nullptr, &Parser::m_binary, Precedence::COMPARISON };
    m_table[std::to_underlying(TokenType::GREATER_EQUAL)] = { nullptr, &Parser::m_binary, Precedence::COMPARISON };
    m_table[std::to_underlying(TokenType::LESS)]          = { nullptr, &Parser::m_binary, Precedence::COMPARISON };
    m_table[std::to_underlying(TokenType::LESS_EQUAL)]    = { nullptr, &Parser::m_binary, Precedence::COMPARISON };
    m_table[std::to_underlying(TokenType::IDENTIFIER)]    = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::STRING)]        = { &Parser::m_literal, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::INT8)]          = { &Parser::m_number, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::INT16)]         = { &Parser::m_number, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::INT64)]         = { &Parser::m_number, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::INT32)]         = { &Parser::m_number, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::UINT8)]         = { &Parser::m_number, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::UINT16)]        = { &Parser::m_number, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::UINT32)]        = { &Parser::m_number, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::UINT64)]        = { &Parser::m_number, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::FLOAT32)]       = { &Parser::m_number, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::FLOAT64)]       = { &Parser::m_number, nullptr, Precedence::PRIMARY };
    m_table[std::to_underlying(TokenType::AND)]           = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::CLASS)]         = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::ELSE)]          = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::FALSE)]         = { &Parser::m_literal, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::FOR)]           = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::FUN)]           = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::IF)]            = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::NIL)]           = { &Parser::m_literal, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::OR)]            = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::LOG)]           = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::RETURN)]        = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::SUPER)]         = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::THIS)]          = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::TRUE)]          = { &Parser::m_literal, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::LET)]           = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::WHILE)]         = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::ERROR)]         = { nullptr, nullptr, Precedence::NONE };
    m_table[std::to_underlying(TokenType::END)]           = { nullptr, nullptr, Precedence::NONE };
}

void Parser::m_declaration()
{
    while (!m_match(TokenType::END)) {
        m_statement();
    }
}

void Parser::m_statement()
{
    using enum TokenType;
    if (m_match(LOG)) {
        m_log_statement();
    }
}

void Parser::m_log_statement()
{
    std::size_t line = m_prev->line;
    m_advance();    // consume 'log' token
    m_grouping();   // parse the expression inside log(...)
    m_match(TokenType::SEMICOLON, "Expect ';' after statement");
    m_stmt = std::make_unique<Log>(Stmt { .line = line }, std::move(m_expr));
}

void Parser::m_grouping()
{
    m_expression();
    m_match(TokenType::RIGHT_PAREN, "Expect ')' after expression");
}

void Parser::m_expression()
{
    m_parse_precedence(Precedence::ASSIGNMENT);
}

void Parser::m_binary()
{
    auto op           = m_prev;
    auto const& entry = m_get_entry(op->type);

    m_parse_precedence(entry.precedence);

    switch (op->type) {
        using enum TokenType;
        case PLUS:
            m_make_binary_expression<Add>();
            break;
        case MINUS:
            m_make_binary_expression<Subtract>();
            break;
        case STAR:
            m_make_binary_expression<Multiply>();
            break;
        case SLASH:
            m_make_binary_expression<Divide>();
            break;
        case PERCENT:
            m_make_binary_expression<Modulus>();
            break;
        case LESS:
            m_make_binary_expression<Compare<Order::LESS>>();
            break;
        case LESS_EQUAL:
            m_make_binary_expression<Compare<Order::GREATER>>();
            m_make_unary_expression<Not>();
            break;
        case GREATER:
            m_make_binary_expression<Compare<Order::GREATER>>();
            break;
        case GREATER_EQUAL:
            m_make_binary_expression<Compare<Order::LESS>>();
            m_make_unary_expression<Not>();
            break;
        case EQUAL_EQUAL:
            m_make_binary_expression<Compare<Order::EQUAL>>();
            break;
        case BANG_EQUAL:
            m_make_binary_expression<Compare<Order::EQUAL>>();
            m_make_unary_expression<Not>();
            break;
        default:
#ifndef NDEBUG
            std::println(std::cerr, "[DEBUG] Unknown binary operator");
#else
            std::unreachable();
#endif
    }
}

void Parser::m_unary()
{
    auto op = m_prev;

    m_parse_precedence(Precedence::UNARY);

    switch (op->type) {
        case TokenType::MINUS:
            m_make_unary_expression<Negate>();
            break;
        case TokenType::BANG:
            m_make_unary_expression<Not>();
            break;
        default:
#ifndef NDEBUG
            std::println(std::cerr, "[DEBUG] Unknown unary operator");
#else
            std::unreachable();
#endif
    }
}

void Parser::m_number()
{
    m_make_literal_expression(m_prev->type);
}

void Parser::m_literal()
{
    // push the left sub-expression into the stack and make ast point to primary expression
    m_stack.emplace_back(std::move(m_expr));

    switch (m_prev->type) {
        using enum TokenType;
        case TRUE:
        case FALSE:
        case STRING:
            m_make_literal_expression(m_prev->type);
            break;
        case INTRPL: {
            // left string
            m_make_literal_expression(TokenType::STRING);

            // middle expression
            m_expression();
            m_make_binary_expression<Add>();   // add this with the left string

            // m_match(TokenType::RIGHT_BRACE, "Expect '}' after interpolation");
            m_advance();   // consume closing braces

            // right string
            m_literal();                       // consume the rest of the string
            m_make_binary_expression<Add>();   // add the right string with the left + middle
        } break;
        default:
#ifndef NDEBUG
            std::println(std::cerr, "Unknown literal type");
#else
            std::unreachable();
#endif
    }
}

void Parser::m_advance()
{
    m_prev = m_curr;

    ++m_curr;   // consume all error tokens until we get a non error token or reach the end
    if (m_curr->type == TokenType::ERROR) {
        m_report(m_curr, m_curr->word);
        while (m_curr != m_tokens.cend() && m_curr->type != TokenType::ERROR) {
            ++m_curr;
        }
    }
}

auto Parser::m_match(TokenType type) -> bool
{
    if (type == m_curr->type) {
        m_advance();
        return true;
    }

    return false;
}

void Parser::m_match(TokenType type, std::string_view err_msg)
{
    if (type == m_curr->type) {
        m_advance();
        return;
    }

    m_report(m_curr, err_msg);
}

template <typename Expression>
void Parser::m_make_binary_expression()
{
    auto left = std::move(m_stack.back());
    m_stack.pop_back();

    m_expr = std::make_unique<Expression>(Binary {
        Expr { .word = std::string { m_prev->word }, .line = m_prev->line },
        std::move(left), std::move(m_expr)
    });
}

template <typename Expression>
void Parser::m_make_unary_expression()
{
    m_expr = std::make_unique<Expression>(Unary {
        Expr { .word = std::string { m_prev->word }, .line = m_prev->line },
        std::move(m_expr)
    });
}

void Parser::m_make_literal_expression(TokenType type)
{
    // push the left sub-expression into the stack and make ast point to primary expression
    m_stack.emplace_back(std::move(m_expr));

    m_expr = std::make_unique<Literal>(
        Expr { .word = std::string { m_prev->word }, .line = m_prev->line },
        type);
}

void Parser::m_report(std::vector<Token>::const_iterator token, std::string_view err_msg)
{
    m_is_parsed   = false;
    m_is_panicked = true;

    static_report::report(token, err_msg);
}

auto Parser::m_get_entry(TokenType type) const noexcept -> PrattEntry const&
{
    return m_table[std::to_underlying(type)];
}

void Parser::m_parse_precedence(Precedence prec)
{
    m_advance();   // consume current token (should be a prefix operator as every expression always starts with a prefix value)

    auto prefix_func = m_get_entry(m_prev->type).prefix;

    if (prefix_func == nullptr) {   // check if current token has a prefix function tied to it from the table
        m_report(m_prev, "Expect expression here");
        return;
    }

    (this->*prefix_func)();   // call the prefix function and form the prefix expression / 1st operand

    while (prec <= m_get_entry(m_curr->type).precedence) {
        m_advance();
        auto infix_func = m_get_entry(m_prev->type).infix;
        if (infix_func == nullptr) {
            m_report(m_prev, "Expect operator here");
            return;
        }
        (this->*infix_func)();
    }
}