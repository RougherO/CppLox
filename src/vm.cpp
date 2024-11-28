#include <cmath>
#include <print>
#ifndef NDEBUG
#include <iostream>
#else
#include <utility>
#endif

#include "vm.hpp"
#include "common.hpp"
#include "instr.hpp"
#include "types.hpp"

template <typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

void VM::execute()
{
    for (; !m_is_end();) {
        execute_next();
    }
}

void VM::execute_next()
{
    auto offset = std::next(m_bc.code().begin(), m_iptr);
    switch (static_cast<Opcode>(*offset)) {
        case Opcode::LOG: {
            auto val1 = m_stack.pop();
            std::visit(util::Visitor {
                           []<typename T>(T val) {
                               std::println("{}", val);
                           },
                           [](std::unordered_set<std::string>::const_iterator val) {
                               std::println("{}", *val);
                           },
                       },
                       val1);
            m_iptr++;
        } break;
        case Opcode::RET:
            std::exit(0);
            break;
        case Opcode::LOAD: {
            auto load_func = [this, &offset]<typename T>(T) {
                T value = *std::bit_cast<T*>(&offset[2]);
                if constexpr (std::is_unsigned_v<T>) {
                    m_stack.push(static_cast<uint64_t>(value));
                } else {
                    m_stack.push(value);
                }
                m_iptr += 2 + sizeof(T);
            };
            switch (static_cast<TypeIndex>(offset[1])) {
                using enum TypeIndex;
                case BOOL: load_func(bool {}); break;
                case INT8: load_func(int8_t {}); break;
                case INT16: load_func(int16_t {}); break;
                case INT32: load_func(int32_t {}); break;
                case INT64: load_func(int64_t {}); break;
                case UINT8: load_func(uint8_t {}); break;
                case UINT16: load_func(uint16_t {}); break;
                case UINT32: load_func(uint32_t {}); break;
                case UINT64: load_func(uint64_t {}); break;
                case FLOAT32: load_func(float {}); break;
                case FLOAT64: load_func(double {}); break;
                case STRING: load_func(std::unordered_set<std::string>::const_iterator {}); break;
            }
        } break;
        case Opcode::ADD: {
            auto val2 = m_stack.pop();
            auto val1 = m_stack.pop();
            std::visit(util::Visitor {
                           [this]<Arithmetic T>(T v1, T v2) {
                               m_stack.push(v1 + v2);
                           },
                           [this](std::unordered_set<std::string>::const_iterator v1, std::unordered_set<std::string>::const_iterator v2) {
                               m_stack.push(m_pool.emplace(*v1 + *v2).first);
                           },
                           [this]<typename T>(std::unordered_set<std::string>::const_iterator v1, T v2) {
                               m_stack.push(m_pool.emplace(*v1 + std::format("{}", v2)).first);
                           },
                           [this]<typename T>(T v1, std::unordered_set<std::string>::const_iterator v2) {
                               m_stack.push(m_pool.emplace(std::format("{}", v1) + *v2).first);
                           },
                           [this]<typename T1, typename T2>(T1, T2) {
#ifndef NDEBUG
                               std::println(std::cerr, "[DEBUG] Reached ADD instruction with two different types");
#else
                               std::unreachable();
#endif
                           } },
                       val1, val2);
            m_iptr++;
        } break;
        case Opcode::SUB: {
            auto val2 = m_stack.pop();
            auto val1 = m_stack.pop();
            std::visit(util::Visitor {
                           [this]<Arithmetic T>(T v1, T v2) {
                               m_stack.push(v1 - v2);
                           },
                           []<typename T1, typename T2>(T1, T2) {
#ifndef NDEBUG
                               std::println(std::cerr, "[DEBUG] Reached SUB instruction with two different types");
#else
                               std::unreachable();
#endif
                           },
                       },
                       val1, val2);
            m_iptr++;
        } break;
        case Opcode::MUL: {
            auto val2 = m_stack.pop();
            auto val1 = m_stack.pop();
            std::visit(util::Visitor {
                           [this]<Arithmetic T>(T v1, T v2) {
                               m_stack.push(v1 * v2);
                           },
                           []<typename T1, typename T2>(T1, T2) {
#ifndef NDEBUG
                               std::println(std::cerr, "[DEBUG] Reached MUL instruction with two different types");
#else
                               std::unreachable();
#endif
                           },
                       },
                       val1, val2);
            m_iptr++;
        } break;
        case Opcode::DIV: {
            auto val2 = m_stack.pop();
            auto val1 = m_stack.pop();
            std::visit(util::Visitor {
                           [this]<Arithmetic T>(T v1, T v2) {
                               m_stack.push(v1 / v2);
                           },
                           []<typename T1, typename T2>(T1, T2) {
#ifndef NDEBUG
                               std::println(std::cerr, "[DEBUG] Reached DIV instruction with two different types");
#else
                               std::unreachable();
#endif
                           },
                       },
                       val1, val2);
            m_iptr++;
        } break;
        case Opcode::MOD: {
            auto val2 = m_stack.pop();
            auto val1 = m_stack.pop();
            std::visit(util::Visitor {
                           [this]<Arithmetic T>(T v1, T v2) {
                               if constexpr (std::is_integral_v<T>) {
                                   m_stack.push(v1 % v2);
                               } else if constexpr (std::is_floating_point_v<T>) {
                                   m_stack.push(std::fmod(v1, v2));
                               }
                           },
                           []<typename T1, typename T2>(T1, T2) {
#ifndef NDEBUG
                               std::println(std::cerr, "[DEBUG] Reached MOD instruction with two different types");
#else
                               std::unreachable();
#endif
                           },
                       },
                       val1, val2);
            m_iptr++;
        } break;
        case Opcode::CMP: {
            auto val2 = m_stack.pop();
            auto val1 = m_stack.pop();
            std::visit(util::Visitor {
                           [this]<typename T>(T v1, T v2) {
                               if constexpr (Arithmetic<T>) {
                                   auto cmp = v1 <=> v2;
                                   m_stack.push(static_cast<int64_t>(
                                       cmp < 0
                                           ? -1
                                           : (cmp > 0
                                                  ? 1
                                                  : 0)));
                               } else {
                                   auto cmp = *v1 <=> *v2;
                                   m_stack.push(static_cast<int64_t>(
                                       cmp < 0
                                           ? -1
                                           : (cmp > 0
                                                  ? 1
                                                  : 0)));
                               }
                           },
                           []<typename T1, typename T2>(T1, T2) {
#ifndef NDEBUG
                               std::println(std::cerr, "[DEBUG] Reached CMP instruction with two different types");
#else
                               std::unreachable();
#endif
                           },
                       },
                       val1, val2);
            m_iptr++;
        } break;
        case Opcode::CMPE: {
            auto val2 = m_stack.pop();
            auto val1 = m_stack.pop();
            std::visit(util::Visitor {
                           [this]<typename T>(T v1, T v2) {
                               auto cmp = v1 == v2;
                               m_stack.push(static_cast<bool>(
                                   cmp));
                           },
                           []<typename T1, typename T2>(T1, T2) {
#ifndef NDEBUG
                               std::println(std::cerr, "[DEBUG] Reached CMPE instruction with two different types");
#else
                               std::unreachable();
#endif
                           },
                       },
                       val1, val2);
            m_iptr++;
        } break;
        case Opcode::NEG: {
            auto val1 = m_stack.pop();
            std::visit(util::Visitor {
                           [this]<Arithmetic T>(T v1) {
                               m_stack.push(-v1);
                           },
                           [this](bool) {
#ifndef NDEBUG
                               std::println(std::cerr, "[DEBUG] Reached NEGATE instruction with bool type");
#else
                               std::unreachable();
#endif
                           },
                           [this](std::unordered_set<std::string>::const_iterator) {
#ifndef NDEBUG
                               std::println(std::cerr, "[DEBUG] Reached NEGATE instruction with str types");
#else
                               std::unreachable();
#endif
                           },
                       },
                       val1);
            m_iptr++;
        } break;
        case Opcode::NOT: {
            auto val1 = m_stack.pop();
            std::visit(util::Visitor {
                           [this]<typename T>(T v1) {
                               m_stack.push(!v1);
                           },
                           [this](std::unordered_set<std::string>::const_iterator iter_to_str) {
                               m_stack.push(!iter_to_str->empty());
                           },
                       },
                       val1);
            m_iptr++;
        } break;
            // default:
            //     break;
    }
}