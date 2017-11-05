#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstddef>
#include <iosfwd>
#include <string>
#include <typeinfo>
#include <utility>
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


// Enumerations
template <typename T,
          typename = std::enable_if_t<std::is_enum_v<T>>>
auto constexpr to_underlying_t(const T value) -> std::underlying_type_t<T>
{
  return static_cast<std::underlying_type_t<T>>(value);
}

template <typename T,
          typename = std::enable_if_t<std::is_enum_v<T>>>
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
          typename = std::enable_if_t<std::is_array_v<A> && std::is_enum_v<T>> >
auto constexpr enum_map(const A &array, T e) -> decltype(auto)
{
  auto idx = to_underlying_t(std::move(e));
  return array[idx];
}





} // namespace Summer
#endif // __UTILS_H__
