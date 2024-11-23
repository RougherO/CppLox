#include "lexer.hpp"
#include "parser.hpp"
#include "compiler.hpp"
#include "logger.hpp"
#include "vm.hpp"

#include <iostream>
#include <print>

auto main(int argc, char** argv) -> int
{
    Lexer lexer = Lexer(R"(log(1 < 2);)");

    Parser parser { std::move(lexer).scan() };

    auto ast = std::move(parser).parse();
    if (!ast.has_value()) {
        std::println("Could not parse the program!");
        return 1;
    }

    std::println("{}", std::visit(util::ast::to_string, ast.value()));

    Compiler compiler { std::move(ast.value()) };

    auto code_segment = std::move(compiler).compile();

    Logger::log(code_segment);

    VM vm { code_segment };
    vm.execute();
}
