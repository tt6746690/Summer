#include "catch.hpp"
#include <utility>
#include <stdexcept>
#include <type_traits>

#include <cstring> 
#include <string> 
#include <iostream>
#include <list>
#include <vector>
#include <functional>

#include "Utils.h"
#include "StrUtils.h"


using namespace std;
using namespace Summer;

// Overloading macros for prepending template argument types
#define PREPEND_TYPENAMES1(T)               typename T
#define PREPEND_TYPENAMES2(X, Y)            typename X, typename Y
#define PREPEND_TYPENAMES3(X, Y, Z)         typename X, typename Y, typename Z
#define GET_PREPEND_TYPENAMES_MACRO(_1, _2, _3, NAME, ...) NAME
#define PREPEND_TYPENAMES(...)  GET_PREPEND_TYPENAMES_MACRO(__VA_ARGS__, PREPEND_TYPENAMES3, PREPEND_TYPENAMES2, PREPEND_TYPENAMES1)(__VA_ARGS__)


// Macros for defining SFINAE checks for a given enable_if_t instance IsT
#define __DEFINE_SFINAE_CHECK(IsT, ...) \
    template < PREPEND_TYPENAMES(__VA_ARGS__) , typename = void> \
    struct sfinae_check_##IsT : std::false_type {}; \
    template < PREPEND_TYPENAMES(__VA_ARGS__) > \
    struct sfinae_check_##IsT<__VA_ARGS__, IsT<__VA_ARGS__>> : std::true_type {}

#define DEFINE_SFINAE_CHECK(IsSomething, ...)           __DEFINE_SFINAE_CHECK(IsSomething, __VA_ARGS__)
#define DEFINE_SFINAE_CHECK_ONE_TARG(IsSomething)       DEFINE_SFINAE_CHECK(IsSomething, T)
#define DEFINE_SFINAE_CHECK_TWO_TARG(IsSomething)       DEFINE_SFINAE_CHECK(IsSomething, X, Y)
#define DEFINE_SFINAE_CHECK_THREE_TARG(IsSomething)     DEFINE_SFINAE_CHECK(IsSomething, X, Y, Z)


#define SFINAE_CHECK_IS_T(IsSomething, ...)             REQUIRE(sfinae_check_##IsSomething<__VA_ARGS__>() == true)
#define SFINAE_CHECK_IS_F(IsSomething, ...)             REQUIRE(sfinae_check_##IsSomething<__VA_ARGS__>() == false)


// Callable types 

template <typename F, typename ...Args> 
using CallableReturnVoid = std::is_same<std::invoke_result<F, Args...>::type, void>;

// template<typename T> 
// using EmtpyArgCallable = std::enable_if_t< decltype() >


DEFINE_SFINAE_CHECK_ONE_TARG(IsRandomAccessIterator);
DEFINE_SFINAE_CHECK_ONE_TARG(IsForwardIterator);
DEFINE_SFINAE_CHECK_TWO_TARG(IsSameOnDecay);



TEST_CASE("Utils")
{
    SECTION("SFINAE shorthand") 
    {

        SECTION("iterator")
        {
            SFINAE_CHECK_IS_T(IsRandomAccessIterator, vector<int>::iterator);
            SFINAE_CHECK_IS_F(IsRandomAccessIterator, list<int>::iterator);        
            SFINAE_CHECK_IS_F(IsForwardIterator, list<int>::iterator);
        }


        SECTION("decay") 
        {
             SFINAE_CHECK_IS_T(IsSameOnDecay, int[2], int*);
             SFINAE_CHECK_IS_T(IsSameOnDecay, const int&, int);
             SFINAE_CHECK_IS_T(IsSameOnDecay, int&, int);
        }
    }

    



    SECTION("Enumerations")
    {

#define CHECK_ENUM(underlying_type) \
    do { \
        enum class Alphabet : underlying_type { \
            A = 0, \
            B, \
            C, \
        }; \
        const char* AlphaArray[] = {"A", "B", "C"}; \
        REQUIRE(is_same_v<decltype(to_underlying_t(Alphabet::A)), underlying_type>); \
        REQUIRE(to_underlying_t(Alphabet::B) == (underlying_type)1); \
        REQUIRE(enum_map(AlphaArray, Alphabet::C) == "C"); \
    } while(0)

        CHECK_ENUM(uint8_t);
        CHECK_ENUM(uint16_t);
        CHECK_ENUM(uint32_t);
        CHECK_ENUM(int8_t);
        CHECK_ENUM(int16_t);
        CHECK_ENUM(int32_t);
    }



    SECTION("irange")
    {


#define TEST_IRANGE(b, e, s) \
    do { \
        vector<int> out; vector<int> expect; \
        for(auto i : irange<b, e, s>()) { \
            out.push_back(i); \
        } \
        for(int i = b; i < e;) { \
            expect.push_back(i); \
            i += s; \
        } \
        REQUIRE(sizeof(irange<b,e,s>()) == 4 * expect.size()); \
        REQUIRE(is_same_v<decay_t<decltype(declval<decltype(irange<b,e,s>())>()[0])>, int> == true); \
        REQUIRE(out == expect); \
    } while(0)


        TEST_IRANGE(1, 20, 1);
        TEST_IRANGE(2, 20, 2);
        TEST_IRANGE(-13, -1, 3);

    }
}


TEST_CASE("StrUtils")
{
    SECTION("common prefix") {
        
        auto check_common_prefix = [](const char* x, const char* y, const char* expect) {

            auto cpp_x = string(x, strlen(x));
            auto cpp_y = string(y, strlen(y));
            auto cpp_expect = string(expect, strlen(expect));

            auto cpp_prefix = find_common_prefix(cpp_x, cpp_y);
            REQUIRE(cpp_prefix == cpp_expect);
            
            auto prefix = find_common_prefix(x, y);
            REQUIRE(!strcmp(prefix.c_str(), expect));

        };
        check_common_prefix("abc", "abe", "ab");
        check_common_prefix("apple", "banana", "");
        check_common_prefix("coffeecup", "coffee", "coffee");
        check_common_prefix("", "", "");
    }


    SECTION("split") {

        auto check_split = [](const char* s, int at, pair<string, string> expect) {
            auto cpp_s = string(s, strlen(s));

            string first, second;
            split_in_half(s, at, first, second);
            REQUIRE(make_pair(first, second) == expect);
        };


        check_split("12345", 2, make_pair("12", "345"));
        check_split("12345", 0, make_pair("", "12345"));
        check_split("12345", 5, make_pair("12345", ""));
    }
        
}