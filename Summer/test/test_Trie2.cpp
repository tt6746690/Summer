#include "catch.hpp"
#include <utility>
#include <stdexcept>

#include <cstring>
#include <string>

#include "Trie2.h"

using std::make_pair;
using std::string;
using std::strcmp;

using namespace Summer;

TEST_CASE("TrieNode2", "[Trie2]")
{

    struct foo { 
        int a;
        int b;
    };


   
    SECTION("Initialization") {
        auto t = Trie<foo>();
        REQUIRE(t.size() == 0);    
        REQUIRE(t.begin() == t.end());
    }

    SECTION("Insertion") {
        auto t = Trie<int>();
        t.insert(make_pair("home/", 1));
    }
    
}