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
    RET,   // return
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,    // modulus
    CMP,    // compare
    CMPE,   // compare equal
    LOAD,
    NEG,    // negate
    NOT,    // logical not
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
        case NEG: return "NEG";
        case NOT: return "NOT";
        case RET: return "RET";
        case LOG: return "LOG";
    }
#ifndef NDEBUG
    std::println(std::cerr, "[DEBUG] Unknown opcode");
#else
    std::unreachable();
#endif
}
}