#pragma once
#include <cstdint>
#ifndef NDEBUG
#include <iostream>
#include <print>
#else
#include <utility>
#endif
#include <string_view>

enum class Opcode : uint8_t {
    LOG,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    CMP,
    CMPE,
    LOAD,
    NEGATE,
    NOT,
    RETURN,
};

namespace util::opcode {
auto to_string(Opcode opcode) -> std::string_view
{
    switch (opcode) {
        using enum Opcode;
        case ADD: return "ADD";
        case SUB: return "SUB";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case MOD: return "MOD";
        case CMP: return "CMP";
        case CMPE: return "CMPE";
        case LOAD: return "LOAD";
        case NEGATE: return "NEGATE";
        case NOT: return "NOT";
        case RETURN: return "RETURN";
        case LOG: return "LOG";
    }
#ifndef NDEBUG
    std::println(std::cerr, "Unknown opcode");
#else
    std::unreachable();
#endif
}
}