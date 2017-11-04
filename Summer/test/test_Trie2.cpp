#include "catch.hpp"
#include <utility>
#include <stdexcept>

#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
#include <ostream>

#include "Trie2.h"

using std::pair;
using std::make_pair;
using std::string;
using std::strcmp;
using std::vector;
using std::cout;
using std::unordered_map;

using namespace Summer;



template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p) {
    return os << "(" << p.first << ", " << p.second << ")" << '\n';
}



TEST_CASE("TrieNode2", "Trie2") 
{

    SECTION("operator<<") {
        auto node = TrieNode<int, char>();
        node.add_edge("awt", 123);
        node.add_edge("wtf", 123);
        node.add_edge("asf", 123);        
        cout << node;
    }

    SECTION("add_edge + find_lmp_edges") {

        using ssumap = unordered_map<string, int>;
        struct lmp_expect {
            string query;
            int lower_bound;
            int upper_bound;
        };
        using spvec = vector<lmp_expect>;

        
        auto test_find_lmp = [](ssumap edges, spvec query_result) {
            auto node = TrieNode<int, char>();
            for(const auto& e : edges) {
                node.add_edge(e.first, e.second);
            }
            REQUIRE(node.edges.size() == edges.size());

            for(const auto& e: query_result) {
                auto range = node.find_lmp_edges(e.query);
                REQUIRE(range.first <= range.second);              
                REQUIRE(range.first == node.edges.begin() + e.lower_bound);
                REQUIRE(range.second == node.edges.begin() + e.upper_bound);  
            }
        };
    
        test_find_lmp({
            {"preMACH", 0},
            {"preUNIX", 1},
            {"preXEN", 2},
            {"preXodic", 3},            
            {"zoo1", 4},
            {"zoo2", 5}
        }, {
            // exact match, prefix and query exhausted : return (i, i), throw error
            {"preUNIX", 1, 1},
            {"zoo1", 4, 4},
            // no prefix match : return (end, end), add new node
            {"hyper", 6, 6},
            {"beast", 6, 6},
            // prefix not exhausted and query not exhausted during match, return (end, end), add new node
            {"preN", 6, 6},
            {"preXYZ", 6, 6},
            // 1 prefix match, prefix exhausted, go down next level
            {"preUNIXpost", 1, 2},
            // 1 prefix match, query exhausted, add new node, transfer range of node to newly added node's edges
            {"preM", 0, 1},
            {"preXo", 3, 4},
            // >1 prefix matches, add new node, transfer range of node to newly added node's edges
            {"pre", 0, 4},
            {"preX", 2, 4},
            {"zoo", 4, 6}
        });
    }
    
    
}

TEST_CASE("Trie2", "Trie2")
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

        cout << t;

    }
    
}