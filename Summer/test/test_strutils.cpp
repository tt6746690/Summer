#include "catch.hpp"
#include <utility>
#include <stdexcept>

#include <cstring> 
#include <string> 

#include "StrUtils.h"


using std::string;
using std::strcmp;
using namespace Summer;


TEST_CASE("strutils")
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
        
}