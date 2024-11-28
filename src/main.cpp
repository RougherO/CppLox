#include "lexer.hpp"
#include "parser.hpp"
#include "compiler.hpp"
#include "logger.hpp"
#include "type_check.hpp"
#include "vm.hpp"

#include "static_report.hpp"

auto main(int argc, char** argv) -> int
{
    Lexer lexer = Lexer(R"(log(1 < 2);)");

    Parser parser { std::move(lexer).scan() };

    auto ast = std::move(parser).parse();
    if (!ast.has_value()) {
        static_report::report("Parsing", "Check for syntax errors");
        return 1;
    }

    TypeChecker type_checker { std::move(ast.value()) };
    ast = std::move(type_checker).check();
    if (!ast.has_value()) {
        static_report::report("Type checking", "Check for type correctnesss");
        return 1;
    }

    std::println("{}", std::visit(util::ast::to_string, ast.value()));

    Compiler compiler { std::move(ast.value()) };

    auto code_segment = std::move(compiler).compile();

    Logger::log(code_segment);

    VM vm { code_segment };
    vm.execute();
}
