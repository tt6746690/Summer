#include "catch.hpp"
#include <utility>
#include <stdexcept>
#include <type_traits>

#include <cstring> 
#include <string> 
#include <iostream>

#include "Utils.h"
#include "StrUtils.h"


using namespace std;
using namespace Summer;





TEST_CASE("Utils")
{


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