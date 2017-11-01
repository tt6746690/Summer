#ifndef UTILITIES_H
#define UTILITIES_H

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace Http {


template<typename InputIter>
auto concat(InputIter begin, InputIter end) -> std::string{
  std::string s;
  std::for_each(begin, end, [&s](auto e){
    s += e + ", ";
  });
  if(!s.empty())
    s.erase(s.end() - 2, s.end());
  return s;
}

static inline auto split(std::string s, char delim)
    -> std::pair<std::string, std::string> {
  auto pos = s.find(delim);
  if (pos != std::string::npos)
    return std::make_pair(s.substr(0, pos), s.substr(pos + 1));
  else
    return std::make_pair("", "");
}

auto constexpr constexpr_streq(const char *x, const char *y) -> bool {
  while (*x || *y) {
    if (*x++ != *y++)
      return false;
  }
  return false;
}

auto constexpr constexpr_strlen(const char *s) -> size_t {
  size_t length = 0;
  if (s) {
    while (*s++)
      ++length;
  }
  return length;
}

template <typename T, typename = void>
struct is_callable : std::is_function<T> {};

template <typename T>
struct is_callable<T, typename std::enable_if<std::is_same<
                          decltype(void(&T::operator())), void>::value>::type>
    : std::true_type {};

template <typename T, typename Key>
auto has_key(const T &container, const Key &key) -> bool {
  return (container.find(key) != std::end(container));
}

template <typename T1, typename T2>
auto operator<<(std::ostream &strm, const std::pair<T1, T2> &p)
    -> std::ostream & {
  return strm << p.first << ": " << p.second;
}

/**
 * @brief   converts enum type to its underlying integer type
 */
template <typename T,
          typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
auto constexpr etoint(const T value) -> typename std::underlying_type<T>::type {
  return static_cast<typename std::underlying_type<T>::type>(value);
}

template <typename T,
          typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
auto etostr(const T &value) -> typename std::string {
  return std::to_string(etoint(value));
}

template <typename T,
          typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
auto operator<<(std::ostream &strm, const T &value) -> std::ostream & {
  return strm << etostr(value);
}

/**
 * @brief   Use enum to query arrays
 *
 * Use enums to keep track of a fixed immutable set of types
 * And use enum_map to retrieve its associated information at compile-time
 *
 * @precondition    enum index starting from 0
 */
template <typename A, typename T,
          typename std::enable_if<std::is_array<A>::value>::type * = nullptr,
          typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
auto constexpr enum_map(A &array, const T &value)
    -> decltype(std::declval<A>()[0]) {
  return array[etoint(value)];
}
}

#endif
