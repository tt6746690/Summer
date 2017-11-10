
#ifndef __TRAITS_H__
#define __TRAITS_H__

#include <typeinfo>
#include <utility>
#include <type_traits>

namespace Summer {



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

