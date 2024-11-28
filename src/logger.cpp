#include <print>

#include "instr.hpp"
#include "types.hpp"
#include "ast.hpp"
#include "logger.hpp"

void Logger::log(CodeSegment const& code_pair)
{
    auto const& [bc, _] = code_pair;

    constexpr std::size_t field_width { 20 };
    std::println("{:^{}} {:^{}} {:^{}} {:^{}}",
                 "OFFSET", field_width,
                 "LINE", field_width,
                 "OPCODE", field_width,
                 "VALUE", field_width);
    for (auto it = bc.lines().begin(); it != bc.lines().end(); it++) {

        auto const& line_info = std::format("[{}]", it->second);

        std::size_t next_offset = std::next(it) != bc.lines().end() ? std::next(it)->first : bc.code().size();

        for (std::size_t offset { it->first }; offset < bc.code().size() && offset < next_offset;) {

            auto opcode = static_cast<Opcode>(bc.code()[offset]);
            switch (opcode) {
                using enum Opcode;
                case LOG:
                case RET:
                case ADD:
                case SUB:
                case MUL:
                case DIV:
                case MOD:
                case NEG:
                case NOT:
                case CMP:
                case CMPE:
                    std::println("{:^#{}x} {:^{}} {:^{}}", offset, field_width, line_info, field_width, util::opcode::to_string(opcode), field_width);
                    offset++;
                    break;
                case LOAD: {
                    // we first get the type of the value we are loading
                    auto type_index = bc.code()[offset + 1];

                    auto log_val = [=, &offset]<typename T>(T) mutable {
                        T value = *std::bit_cast<T*>(&bc.code()[offset + 2]);
                        // For string values we are extracting from our interned table and for all other values
                        // we simply read them off of the bytecode
                        if constexpr (std::is_same_v<T, std::unordered_set<std::string>::const_iterator>) {
                            std::println("{:^#{}x} {:^{}} {:^{}} {}",
                                         offset, field_width,
                                         line_info, field_width,
                                         "LOAD", field_width,
                                         std::format("({:#x})\"{}\"", std::bit_cast<uintptr_t>(value), *value), field_width);
                        } else {
                            std::println("{:^#{}x} {:^{}} {:^{}} {:^{}}",
                                         offset, field_width,
                                         line_info, field_width,
                                         "LOAD", field_width,
                                         util::literal::to_string(value), field_width);
                        }
                        offset += 2 + sizeof(T);
                    };

                    switch (util::type::get_type(type_index)) {
                        using enum TypeIndex;
                        case BOOL: log_val(bool {}); break;
                        case INT8: log_val(int8_t {}); break;
                        case INT16: log_val(int16_t {}); break;
                        case INT32: log_val(int32_t {}); break;
                        case INT64: log_val(int64_t {}); break;
                        case UINT8: log_val(uint8_t {}); break;
                        case UINT16: log_val(uint16_t {}); break;
                        case UINT32: log_val(uint32_t {}); break;
                        case UINT64: log_val(uint64_t {}); break;
                        case FLOAT32: log_val(float {}); break;
                        case FLOAT64: log_val(double {}); break;
                        case STRING:
                            log_val(std::unordered_set<std::string>::const_iterator {});
                            break;
                        default:
                            std::println("{:^#{}x} {:^{}} {:^{}} {:^{}}", offset, field_width, line_info, field_width, "LOAD", field_width, "<UNKNOWN>", field_width);
                            offset += 2;
                    }
                } break;
                default:
                    std::println("{:^#{}x} {:^{}} {:^{}}",
                                 offset, field_width,
                                 line_info, field_width,
                                 "<UNKNOWN>", field_width);
                    offset++;
                    break;
            }
        }
    }
}