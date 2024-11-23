/**
    declaration -> classDecl | funcDecl | varDecl | statement
    statement   -> exprStmt | forStmt | ifStmt | logStmt | returnStmt | whileStmt | block
 */

#ifndef NDEBUG
#include <iostream>
#include <print>
#endif
#include <format>
#include <charconv>
#include <string_view>

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
    while (m_curr->type != TokenType::END) {
        m_advance();   // consume current token
        m_statement();
    }
}

void Parser::m_statement()
{
    switch (m_prev->type) {
        using enum TokenType;
        case LOG:
            m_log_statement();
            break;
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

    auto left = std::move(m_stack.back());
    m_stack.pop_back();

    auto type_index = util::type::get_type(left);

    if (type_index != util::type::get_type(m_expr)) {
        m_report(op, "Expect expressions of same type");
        return;
    }

    auto make_binary_expr = [this, op, &left, type_index]<typename ExprType>(ExprType, std::string_view c, TypeIndex new_type_index) {
        switch (type_index) {
            using enum TypeIndex;
            case INT8:
            case INT16:
            case INT32:
            case INT64:
            case UINT8:
            case UINT16:
            case UINT32:
            case UINT64:
            case FLOAT32:
            case FLOAT64:
                m_expr = std::make_unique<ExprType>(Binary {
                    Expr { .line = op->line, .type = new_type_index },
                    std::move(left), std::move(m_expr)
                });
                break;
            default:
                m_report(m_prev, std::format("Cannot perform '{}' operation on value of type: {}",
                                             c, util::type::to_string(type_index)));
                return;
        }
    };

    switch (op->type) {
        using enum TokenType;
        case PLUS:
            if (type_index == TypeIndex::STRING) {
                m_expr = std::make_unique<Add>(Binary {
                    Expr { .line = op->line, .type = TypeIndex::STRING },
                    std::move(left), std::move(m_expr)
                });
            } else {
                make_binary_expr(Add {}, "+", type_index);
            }
            break;
        case MINUS:
            make_binary_expr(Subtract {}, "-", type_index);
            break;
        case STAR:
            make_binary_expr(Multiply {}, "*", type_index);
            break;
        case SLASH:
            make_binary_expr(Divide {}, "/", type_index);
            break;
        case PERCENT:
            make_binary_expr(Modulus {}, "%", type_index);
        case LESS:
            make_binary_expr(Compare<Order::LESS> {}, "<", TypeIndex::BOOL);
            break;
        case LESS_EQUAL:
            make_binary_expr(Compare<Order::GREATER> {}, "<=", TypeIndex::BOOL);
            m_expr = std::make_unique<Not>(Unary {
                Expr { op->line, TypeIndex::BOOL },
                std::move(m_expr),
            });
            break;
        case GREATER:
            make_binary_expr(Compare<Order::GREATER> {}, ">", TypeIndex::BOOL);
            break;
        case GREATER_EQUAL:
            make_binary_expr(Compare<Order::LESS> {}, ">=", TypeIndex::BOOL);
            m_expr = std::make_unique<Not>(Unary {
                Expr { op->line, TypeIndex::BOOL },
                std::move(m_expr),
            });
            break;
        case EQUAL_EQUAL:
            if (type_index == TypeIndex::BOOL) {
                m_expr = std::make_unique<Compare<Order::EQUAL>>(Binary {
                    Expr { .line = op->line, .type = TypeIndex::BOOL },
                    std::move(left), std::move(m_expr)
                });
            } else {
                make_binary_expr(Compare<Order::EQUAL> {}, "==", TypeIndex::BOOL);
            }
            break;
        case BANG_EQUAL:
            if (type_index == TypeIndex::BOOL) {
                m_expr = std::make_unique<Compare<Order::EQUAL>>(Binary {
                    Expr { .line = op->line, .type = TypeIndex::BOOL },
                    std::move(left), std::move(m_expr)
                });
            } else {
                make_binary_expr(Compare<Order::EQUAL> {}, "!=", TypeIndex::BOOL);
            }
            m_expr = std::make_unique<Not>(Unary {
                Expr { op->line, TypeIndex::BOOL },
                std::move(m_expr),
            });
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

    auto type_index = util::type::get_type(m_expr);

    switch (op->type) {
        case TokenType::MINUS:
            switch (type_index) {
                using enum TypeIndex;
                case INT8:
                case INT16:
                case INT32:
                case INT64:
                case FLOAT32:
                case FLOAT64:
                    m_expr = std::make_unique<Negate>(Unary {
                        Expr { .line = op->line, .type = type_index },
                        std::move(m_expr)
                    });
                    break;
                case UINT8:
                case UINT16:
                case UINT32:
                case UINT64:
                    m_report(op, "Cannot negate an unsigned type");
                    return;
                default:
                    m_report(op, "Expect a numeric expression here");
                    return;
            }
        case TokenType::BANG:
            m_expr = std::make_unique<Not>(Unary {
                Expr { .line = op->line, .type = TypeIndex::BOOL },
                std::move(m_expr)
            });
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
    // push the left sub-expression into the stack and make ast point to primary expression
    m_stack.emplace_back(std::move(m_expr));

    TokenType type = m_prev->type;

    auto make_number = [this]<typename T>(T, TypeIndex type_index) {
        T value {};
        std::from_chars(m_prev->word.data(), m_prev->word.data() + m_prev->word.size(), value);
        m_expr = std::make_unique<Literal>(Expr { .line = m_prev->line, .type = type_index }, value);
    };
    switch (type) {
        using enum TokenType;
        case INT8: make_number(int8_t {}, TypeIndex::INT8); break;
        case INT16: make_number(int16_t {}, TypeIndex::INT16); break;
        case INT32: make_number(int32_t {}, TypeIndex::INT32); break;
        case INT64: make_number(int64_t {}, TypeIndex::INT64); break;
        case UINT8: make_number(uint8_t {}, TypeIndex::UINT8); break;
        case UINT16: make_number(uint16_t {}, TypeIndex::UINT16); break;
        case UINT32: make_number(uint32_t {}, TypeIndex::UINT32); break;
        case UINT64: make_number(uint64_t {}, TypeIndex::UINT64); break;
        case FLOAT32: make_number(float {}, TypeIndex::FLOAT32); break;
        case FLOAT64: make_number(double {}, TypeIndex::FLOAT64); break;
        default:
#ifndef NDEBUG
            std::println(std::cerr, "[DEBUG] Unknown number types");
#else
            std::unreachable();
#endif
    }
}

void Parser::m_literal()
{
    // push the left sub-expression into the stack and make ast point to primary expression
    m_stack.emplace_back(std::move(m_expr));

    switch (m_prev->type) {
        using enum TokenType;
        case TRUE:
            m_expr = std::make_unique<Literal>(Expr { .line = m_prev->line, .type = TypeIndex::BOOL }, true);
            break;
        case FALSE:
            m_expr = std::make_unique<Literal>(Expr { .line = m_prev->line, .type = TypeIndex::BOOL }, false);
            break;
        case STRING:
            m_expr = std::make_unique<Literal>(
                Expr { .line = m_prev->line, .type = TypeIndex::STRING },
                std::string { m_prev->word });
            break;
        case INTRPL: {
            // left string
            std::size_t line = m_prev->line;
            m_expr           = std::make_unique<Literal>(Expr { .line = line, .type = TypeIndex::STRING }, std::string { m_prev->word });

            // middle expression
            m_expression();

            auto left = std::move(m_stack.back());
            m_stack.pop_back();
            m_expr = std::make_unique<Add>(Binary {
                Expr { .line = line, .type = TypeIndex::STRING },
                std::move(left),
                std::move(m_expr),
            });

            m_match(TokenType::RIGHT_BRACE, "Expect '}' after interpolation");
            m_advance();

            // right string
            line = m_prev->line;
            m_literal();   // consume the rest of the string

            left = std::move(m_stack.back());
            m_stack.pop_back();
            m_expr = std::make_unique<Add>(Binary {
                Expr { .line = line, .type = TypeIndex::STRING },
                std::move(left),
                std::move(m_expr)
            });
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

void Parser::m_match(TokenType type, std::string_view err_msg)
{
    if (type == m_curr->type) {
        m_advance();
        return;
    }

    m_report(m_curr, err_msg);
}

void Parser::m_report(std::vector<Token>::const_iterator token, std::string_view err_msg)
{
    m_is_parsed   = false;
    m_is_panicked = true;

    std::cerr << std::format("[line: {}] error ", token->line);

    if (token->type == TokenType::END) {
        std::cerr << "at end: ";
    } else if (token->type == TokenType::ERROR) {

    } else {
        std::cerr << std::format("at '{}': ", token->word);
    }

    std::cerr << err_msg << std::endl;   // explicitly flush each error
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