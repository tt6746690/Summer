
#ifndef __TRAITS_H__
#define __TRAITS_H__

#include <typeinfo>
#include <utility>
#include <type_traits>

namespace Theros {


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

//// SFINAE aliases

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



} // namespace Theros
#endif // __TRAITS_H__

