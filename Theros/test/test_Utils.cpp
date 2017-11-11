#include "catch.hpp"

#include <iostream>
#include <list>

#include "Utils.h"
#include "StrUtils.h"
#include "Url.h"
#include "Router.h"
#include "Traits.h"


using namespace std;
using namespace Theros;

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




template <typename F>
using IsHandlerType = std::enable_if_t<callable_with<F, Context&>() || callable_with<F>()>;


// Define SFINAE checks
DEFINE_SFINAE_CHECK_ONE_TARG(IsRandomAccessIterator);
DEFINE_SFINAE_CHECK_ONE_TARG(IsForwardIterator);
DEFINE_SFINAE_CHECK_TWO_TARG(IsSameOnDecay);
DEFINE_SFINAE_CHECK_ONE_TARG(IsHandlerType);



TEST_CASE("Utils")
{
    SECTION("Traits")
    {
        SECTION("satisfies_{all, some, none}")
        {
            REQUIRE(satisfies_all<true, true, true> == true);
            REQUIRE(satisfies_all<true, true, false> == false);
            REQUIRE(satisfies_all<false, true, true> == false);
            REQUIRE(satisfies_all<false, false, false> == false);

            REQUIRE(satisfies_some<true, true, true> == true);
            REQUIRE(satisfies_some<true, true, false> == true);
            REQUIRE(satisfies_some<false, true, true> == true);
            REQUIRE(satisfies_some<false, false, false> == false);

            REQUIRE(satisfies_none<true, true, true> == false);
            REQUIRE(satisfies_none<true, true, false> == false);
            REQUIRE(satisfies_none<false, true, true> == false);
            REQUIRE(satisfies_none<false, false, false> == true);
        }

        SECTION("is_callable_with")
        {
            auto add1 = [](int x) { return x + 1; };
            REQUIRE(is_callable_with<int>(add1) == true);
            REQUIRE(is_callable_with<std::ostream>(add1) == false);

            auto h1 = [](const Context& ctx) { return; };
            REQUIRE(is_callable_with<Context>(h1) == true);
            REQUIRE(is_callable_with<Context&>(h1) == true);
            REQUIRE(is_callable_with<const Context&>(h1) == true);
            REQUIRE(is_callable_with<Request>(h1) == false);
            REQUIRE(is_callable_with<>(h1) == false);


            auto h2 = []() { return 1; };
            REQUIRE(is_callable_with<Context>(h2) == false);
            REQUIRE(is_callable_with<>(h2) == true);

            REQUIRE(callable_with<decltype(h2)>() == true);
            REQUIRE(callable_with<decltype(h1)(Context)>() == true);
        }
    }

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

        SECTION("IsHandlerType")
        {

            auto IsHandleFunc1 = [](){ int x = 1; return x; };
            struct IsHandleFunc2 {
                bool operator()(const Context&) { return 1; }
            };
            auto IsHandleFunc3 = [](Context& ) { return 1; };

            SFINAE_CHECK_IS_T(IsHandlerType, decltype(IsHandleFunc1));
            SFINAE_CHECK_IS_T(IsHandlerType, decltype(IsHandleFunc2()));
            SFINAE_CHECK_IS_T(IsHandlerType, decltype(IsHandleFunc3));

        }

        SECTION("!IsHandlerType")
        {
            auto f1 = [](int x) { return x; };
            auto f2 = [f1](Request req) { return 1; };
            SFINAE_CHECK_IS_F(IsHandlerType, decltype(f1));
            SFINAE_CHECK_IS_F(IsHandlerType, decltype(f2));
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


TEST_CASE("url encoding/decoding") {

    std::string url;

    SECTION("ctohex") {
        REQUIRE(ctohex('#') == "23");
        REQUIRE(ctohex('%') == "25");
    }

    SECTION("encoding") {
        url = u8"François";
        REQUIRE(urlencode(url) == "Fran%C3%A7ois");
    }

    SECTION("decoding") {
        url = "a%20space";
        REQUIRE(urldecode(url) == u8"a space");

        url = "Fran%C3%A7ois";
        REQUIRE(urldecode(url) == u8"François");
    }


    SECTION("make query") {
        auto query = make_query("foo=bar&a=d,s,d");
        REQUIRE_NOTHROW(query.at("foo"));
        REQUIRE(query["foo"] == "bar");
        REQUIRE_NOTHROW(query.at("a"));
        REQUIRE(query["a"] == "d,s,d");
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