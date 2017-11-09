#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstddef>
#include <iosfwd>
#include <string>
#include <typeinfo>
#include <utility>
#include <algorithm>
#include <type_traits>

#include "Defines.h"

namespace Summer
{

// operator<<

template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p) {
    return os << "(" << p.first << ", " << p.second << ")" << eol;
}
template<typename T> 
std::ostream& operator<<(std::ostream& os, const std::vector<T>& c) {
    for(const auto& e: c) os << e << eol;
    return os;
}


// Iterator 

template <typename It> 
using IteratorCategory = typename std::iterator_traits<It>::iterator_category;

template <typename It> 
using SameAsRandomAccessIterator = std::is_same<IteratorCategory<It>, std::random_access_iterator_tag>;
template <typename It> 
using IsRandomAccessIterator = std::enable_if_t<SameAsRandomAccessIterator<It>::value>;

template <typename It> 
using SameAsForwardIterator = std::is_same<IteratorCategory<It>, std::forward_iterator_tag>;
template <typename It> 
using IsForwardIterator = std::enable_if_t<SameAsForwardIterator<It>::value>;

// decay 

// strips cv qualifier, function pointer and array pointer decay
template <typename X, typename Y> 
using SameOnDecay = std::is_same<std::decay_t<X>, std::decay_t<Y>>;

template <typename X, typename Y> 
using IsSameOnDecay = std::enable_if_t<SameOnDecay<X, Y>::value>;

template <typename X, typename Y> 
using IsNotSameOnDecay = std::enable_if_t<!SameOnDecay<X, Y>::value>;


// Composing predicates 

template <template <typename ...> class... Preds>
struct Compose {
    template <typename T>
    static constexpr auto eval() {
        auto results = { Preds<T>::value ...};
        auto result = true;
        for(auto e: results) result &= e;
        return result;
    }
};

template <typename T, template <typename> class... Ts>
inline constexpr auto satisfies_all = (... && Ts<T>::value);

template <typename T, template <typename> class... Ts>
inline constexpr auto satisfies_some = (... || Ts<T>::value);

template <typename T, template <typename> class... Ts>
inline constexpr auto satisfies_none = (... && !Ts<T>::value);

// Compile-time Integer Range

template<int ...Ns>
struct sequence {
    static constexpr int data[sizeof...(Ns)]=  { Ns... };
};

template<int ...Ns>
struct gen_seq {};

template<int Step, int Start, int End, int ...Ns>
struct gen_seq<Step, Start, End, Ns...> {
    using type = typename gen_seq<Step, Start, End-Step, End-Step, Ns...>::type;
};

template<int Step, int Start, int ...Ns>
struct gen_seq<Step, Start, Start, Ns...> {
    using type = sequence<Ns...>;
};

template<int Start, int End, int Step>
using sequence_t = typename gen_seq<Step, Start, End>::type;


template<int Start, int End, int Step>
constexpr auto irange() -> std::add_lvalue_reference_t<decltype(sequence_t<Start, End + (End-Start)%Step, Step>::data)>
{
    static_assert(Start < End, "Start >= End not allowed");
    return sequence_t<Start, End + (End-Start)%Step, Step>::data;
}

template<int End>
constexpr auto irange() -> std::add_lvalue_reference_t<decltype(sequence_t<0, End, 1>::data)>
{
    return irange<0, End, 1>();
}

template<int Start, int End>
constexpr auto irange() -> std::add_lvalue_reference_t<decltype(sequence_t<Start, End, 1>::data)>
{
    return irange<Start, End, 1>();
};


// Enumerations

template <typename T> 
using IsEnum = typename std::enable_if_t<std::is_enum_v<T>>;

template <typename T> 
using IsArray = typename std::enable_if_t<std::is_array_v<T>>;

template <typename T, typename = IsEnum<T>>
auto constexpr to_underlying_t(const T value) -> std::underlying_type_t<T>
{
  return static_cast<std::underlying_type_t<T>>(value);
}

template <typename T, typename = IsEnum<T>>
std::ostream & operator<<(std::ostream &os, const T &e) { return os << std::to_string(to_underlying_t(e)); }

/**
 * @brief   Use enum to query arrays
 *
 * Use enums to keep track of a fixed immutable set of types
 * And use enum_map to retrieve its associated information at compile-time
 *
 * @precondition    enum index starting from 0
 */
template <typename A, typename T, 
          typename = IsArray<A>, typename = IsEnum<T>>
auto constexpr enum_map(const A &array, T e) -> decltype(auto)
{
  auto idx = to_underlying_t(std::move(e));
  return array[idx];
}







} // namespace Summer
#endif // __UTILS_H__
