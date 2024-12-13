#include "lexer.hpp"
#include "parser.hpp"
#include "compiler.hpp"
#include "logger.hpp"
#include "semantic_check.hpp"
#include "vm.hpp"

#include "static_report.hpp"

auto main(int argc, char** argv) -> int
{
    Lexer lexer = Lexer(R"(log(1 < 2);)");

    Parser parser { std::move(lexer).scan() };

    auto program = std::move(parser).parse();
    if (!program.has_value()) {
        static_report::report("Parsing failed");
        return 1;
    }

    if (!SemanticChecker::check(program.value())) {
        static_report::report("Semantic analysis failed");
        return 1;
    }

    std::println("{}", util::ast::to_string(program.value()));

    Compiler compiler { std::move(ast.value()) };

    auto code_segment = std::move(compiler).compile();

    Logger::log(code_segment);

    VM vm { code_segment };
    vm.execute();
}
