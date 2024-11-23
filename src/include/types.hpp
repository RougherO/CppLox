#pragma once
#include <variant>
#ifndef NDEBUG
#include <print>
#include <iostream>
#else
#include <utility>
#endif

namespace util {
template <template <typename...> typename R, typename List, typename... Us>
struct from;

template <template <typename...> typename R, template <typename...> typename List, typename... Us>
struct from<R, List<Us...>> {
    using type = R<Us...>;
};
}

// Order matters we use this order to infer types at runtime while compiling
enum class TypeIndex : uint8_t {
    BOOL,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
    STRING,
};

using TypeList = std::tuple<bool,
                            int8_t, int16_t, int32_t, int64_t,
                            uint8_t, uint16_t, uint32_t, uint64_t,
                            float, double,
                            std::string>;

using TypeVariant = util::from<std::variant, TypeList>::type;

namespace util::type {
auto to_string(TypeIndex index) -> std::string_view
{
    switch (index) {
        using namespace std::string_view_literals;
        using enum TypeIndex;
        case BOOL: return "bool";
        case INT8: return "i8";
        case INT16: return "i16";
        case INT32: return "i32";
        case INT64: return "i64";
        case UINT8: return "u8";
        case UINT16: return "u16";
        case UINT32: return "u32";
        case UINT64: return "u64";
        case FLOAT32: return "f32";
        case FLOAT64: return "f64";
        case STRING: return "str";
    }
#ifndef NDEBUG
    std::println(std::cerr, "[DEBUG] Unknown type");
#else
    std::unreachable();
#endif
};

auto get_type(TypeVariant const& type) noexcept -> TypeIndex   // used in vm when popping out pushed value from stack
{
    return static_cast<TypeIndex>(type.index());
}

auto get_type(uint8_t type_index) noexcept -> TypeIndex   // used in logger and vm when converting from byte to index
{
    return static_cast<TypeIndex>(type_index);
}
}