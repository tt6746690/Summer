
#ifndef __TRAITS_H__
#define __TRAITS_H__

#include <typeinfo>
#include <utility>
#include <type_traits>

namespace Summer {


// conjunctions and disjunctions on constexpr bools


template <bool Head, bool... Tail>
struct satisfies_all_impl : std::integral_constant<bool, Head && satisfies_all_impl<Tail...>() > {};

template <bool T>
struct satisfies_all_impl<T> : std::integral_constant<bool, T> {};

template <bool... Ts>
using SatisfiesAll = satisfies_all_impl<Ts...>;

template <bool... Ts>
constexpr bool satisfies_all = SatisfiesAll<Ts...>();


template <bool Head, bool... Tail>
struct satisfies_some_impl : std::integral_constant<bool, Head || satisfies_some_impl<Tail...>() > {};

template <bool T>
struct satisfies_some_impl<T> : std::integral_constant<bool, T> {};

template <bool... Ts>
using SatisfiesSome = satisfies_some_impl<Ts...>;

template <bool... Ts>
constexpr bool satisfies_some = SatisfiesSome<Ts...>();

template <bool Head, bool... Tail>
struct satisfies_none_impl : std::integral_constant<bool, !Head && satisfies_none_impl<Tail...>() > {};

template <bool T>
struct satisfies_none_impl<T> : std::integral_constant<bool, !T> {};

template <bool... Ts>
using SatisfiesNone = satisfies_none_impl<Ts...>;

template <bool... Ts>
constexpr bool satisfies_none = SatisfiesNone<Ts...>();



// Is callable with 

struct callable_with_test
{
    template <typename F, typename... Args> 
    static decltype(std::declval<F>()( std::declval<Args>()... ), std::true_type() )
    f(int);

    template <typename F, typename... Args> 
    static std::false_type f(...);
};

template <typename F, typename... Args> 
struct callable_with : decltype( callable_with_test::f<F, Args...>(0) ) {};

template <typename F, typename... Args>
struct callable_with<F(Args...)> : callable_with<F, Args...> { };

template <typename... Args, typename F>
constexpr auto is_callable_with(F &&) -> callable_with<F, Args...> {
    return callable_with<F(Args...)>{};
}




} // namespace Summer 
#endif // __TRAITS_H__

