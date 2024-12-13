#pragma once
#include <cstdint>
#ifndef NDEBUG
#include <iostream>
#include <print>
#else
#include <utility>
#endif
#include <string_view>

// _F floating point alternative opcode
enum Opcode : uint8_t {
    LOG,
    RET,   // return
    ADD,
    ADDF,
    SUB,
    SUBF,
    MUL,
    MULF,
    DIV,
    DIVF,
    MOD,     // modulus
    MODF,
    CMP,     // compare
    CMPF,
    CMPE,    // compare equal
    CMPEF,
    LOAD,    // load primitive types(ints and floats)
    LOADS,   // load string
    NEG,     // negate
    NEGF,
    NOT,     // logical not
};

namespace util::opcode {
auto to_string(Opcode opcode) -> std::string_view
{
    switch (opcode) {
        using enum Opcode;
        case ADD: return "ADD";
        case ADDF: return "ADDF";
        case SUB: return "SUB";
        case SUBF: return "SUBF";
        case MUL: return "MUL";
        case MULF: return "MULF";
        case DIV: return "DIV";
        case DIVF: return "DIVF";
        case MOD: return "MOD";
        case MODF: return "MODF";
        case CMP: return "CMP";
        case CMPF: return "CMPF";
        case CMPE: return "CMPE";
        case CMPEF: return "CMPEF";
        case LOAD: return "LOAD";
        case LOADS: return "LOADS";
        case NEG: return "NEG";
        case NEGF: return "NEGF";
        case NOT: return "NOT";
        case RET: return "RET";
        case LOG: return "LOG";
#ifndef NDEBUG
            std::println(std::cerr, "[DEBUG] Unknown opcode");
            return "UNKNOWN";
#else
        default:
            std::unreachable();
#endif
    }
}
}