#pragma once
#include <cstdint>
#include <variant>
#include <tuple>

struct Int {
    int32_t value;
};

struct Float {
    float value;
};

using TypeTuple = std::tuple<Int, Float>;

template <typename T>
concept LoxType = requires { std::get<T>(TypeTuple {}); };

template <template <typename...> typename R, typename TupleType>
    requires requires { std::tuple_size_v<TypeTuple>; }
using to = decltype(std::apply([]<typename... Ts>(Ts...) { return R<Ts...> {}; }, std::declval<TupleType>()));

using TypeVariant = to<std::variant, TypeTuple>;

template <typename... Vs>
struct Visitor : Vs... {
    using Vs::operator()...;
};

// template <uint8_t index>
// struct type_element : std::tuple_element<index, TypeTuple> { };

// template <uint8_t index>
// using type_element_t = typename type_element<index>::type;

// namespace Util {
// template <typename T, typename TupleType>
// struct tuple_index;

// template <typename T, typename... Ts>
// struct tuple_index<T, std::tuple<T, Ts...>> {
//     inline static uint8_t constexpr value = 0;
// };

// template <typename T, typename U, typename... Ts>
// struct tuple_index<T, std::tuple<U, Ts...>> {
//     inline static uint8_t constexpr value = 1 + tuple_index<T, std::tuple<Ts...>>::value;
// };

// template <typename T, typename TupleType>
// inline constexpr uint8_t tuple_index_v = tuple_index<T, TupleType>::value;
// }

// template <LoxType T>
// struct type_index : Util::tuple_index<T, TypeTuple> { };
