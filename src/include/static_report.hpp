#pragma once

#include <iostream>
#include <print>
#include <vector>
#include <memory>
#include <string_view>

#include "tokens.hpp"

namespace static_report {
void report(std::string_view msg)
{
    std::println(std::cerr, "{}", msg);
}

template <typename T>
void report(T const& val)
    requires requires {
        T::word;
        T::line;
    }
{
    std::print(std::cerr, "[line: {}] error at '{}'", val.line, val.word);
}

void report(std::vector<Token>::const_iterator token, std::string_view err_msg)
{
    report(*token);

    std::println("{}", err_msg);
}

template <typename Expression>
void report(std::unique_ptr<Expression> const& expr, std::string_view err_msg)
{
    report(*expr);

    std::println("{}", err_msg);
}
};