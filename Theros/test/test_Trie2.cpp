#include "catch.hpp"
#include <utility>
#include <stdexcept>

#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
#include <ostream>

#include "Utils.h"
#include "Trie2.h"

using std::pair;
using std::make_pair;
using std::string;
using std::strcmp;
using std::vector;
using std::cout;
using std::endl;
using std::unordered_map;

using ssumap = unordered_map<string, int>;


using namespace Theros;


auto make_trie = [](const ssumap& insertee) {
    auto t = Trie<int>();
    for(const auto& e : insertee) {
        t.insert({ e.first, e.second });
    }
    return t;
};


// Using / as deliminators, find common prefix of x, the route path, and y, the query path
// /< name >[/] matches with any string /value/ -- (name, value) pair is stored in kvs, common prefix returned
// Assumptions: y cannot contain '<'; x has balanced brackets
std::string find_route_prefix_unstrict(const char* x,   // route
                                       const char* y,   // query path 
                                       std::vector<std::pair<std::string, std::string>>& kvs) 
{
    if (*x == '\0' || *y == '\0') return "";

    const char* prefix = x;
    const char* name;
    const char* value;
    
    while(*x != '\0' && *y != '\0') 
    {
        if (*x == *y) { ++x; ++y; continue; }
        
        if (*x == '<') {
            name  = x+1;
            value = y++;

            while (*x != '>') { ++x; }
            while (*y != '\0' && *y != '/') { ++y; }    // check for end of c_string

            std::string k(name, x-name);
            std::string v(value, y-value);
            kvs.push_back({k, v});

            // x == '>' and y == '/', advance x by 1
            ++x;
        } else {
            return std::string(prefix, x-prefix);
        }
    }

    // cases when either one is exhausted
    if(*x ^ *y)
        return std::string(prefix, x-prefix);

    size_t prefix_len =
            (x-prefix+1 > strlen(prefix)) ? strlen(prefix) : x-prefix+1;
    return std::string(prefix, prefix_len);
} 



TEST_CASE("prefix pattern ", "Trie2")
{

    SECTION("find_route_prefix_unstrict")
    {
        using kvpairs = std::vector<std::pair<std::string, std::string>>;
        auto test_find_route_prefix_unstrict = [](const char* x,
                                                  const char* y,
                                                  const string& expected_prefix,
                                                  kvpairs expected_kvs) {
            kvpairs kvs;
            std::string prefix = find_route_prefix_unstrict(x, y, kvs);
            REQUIRE(kvs == expected_kvs);
            REQUIRE(prefix == expected_prefix);
        };


        // normal prefix matching
        test_find_route_prefix_unstrict("", "", "", {});
        test_find_route_prefix_unstrict("abc", "abcde", "abc", {});
        test_find_route_prefix_unstrict("apple", "banana", "", {});
        test_find_route_prefix_unstrict("coffeecup", "coffee", "coffee", {});

        // route path containing placeholders
        test_find_route_prefix_unstrict("/textbook/<author>", "/textbook/Shakespear",
                                        "/textbook/<author>", {{"author", "Shakespear"}});
        test_find_route_prefix_unstrict("/<id>/data", "/123456/data",
                                        "/<id>/data", {{"id", "123456"}});
        test_find_route_prefix_unstrict("/<a>/<b>/<c>", "/1/2/3",
                                        "/<a>/<b>/<c>", {{"a", "1"}, {"b", "2"}, {"c", "3"}});
        test_find_route_prefix_unstrict("<a>", "1",
                                        "<a>", {{"a", "1"}});
        test_find_route_prefix_unstrict("<a>", "1/thingelse",
                                        "<a>", {{"a", "1"}});
    }

    SECTION("routing_example")
    {

        auto t = make_trie({
            {"/", 1},
            {"/textbook/<author>", 2},
            {"/textbook/public_date/<date>", 3},
            {"/user/<id>", 4},
            {"/user/<id>/books/<book_id>", 5}
        });

        /**
         \	( 0 )
            |-/ 	( 1 )
                |-textbook/<author> 	( 2 )
                |-textbook/public_date/<date> 	( 3 )
                |-user/<id> 	( 4 )
                    |-/books/<book_id> 	( 5 )
        */


    }
}


TEST_CASE("TrieNode2", "Trie2") 
{

    SECTION("operator<<") {

        auto edge = TrieNodeEdge<TrieNode<int, char>>();
        // cout << edge;

        auto node = TrieNode<int, char>();
        node.add_edge("awt", 123);
        node.add_edge("wtf", 123);
        node.add_edge("asf", 123);        
        // cout << node;
    }

    SECTION("add_edge + find_lmp_edges") {

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


   
    SECTION("Initialization") {
        auto t = Trie<int>();
        REQUIRE(t.size() == 0);    
        REQUIRE(t.begin() == t.end());
    }


    SECTION("find") {

        auto t = make_trie({ 
            {"smile", 1}, 
            {"smiles", 2}, 
            {"smiling", 3},
            {"smiled", 4},
            {"smil", 5}
        });


        SECTION("found") {
            auto test_found = [](Trie<int>& t, const ssumap& insertee) {
                for(const auto& e : insertee) {
                    auto it = t.find(e.first);
                    REQUIRE(it != t.end());
                    REQUIRE(*it == e.second);
                }
            };
            test_found(t, {
                {"smile", 1}, 
                {"smiles", 2}, 
                {"smiling", 3},
                {"smiled", 4},
                {"smil", 5}
            });
        }

        SECTION("not found") {
            auto test_not_found = [](Trie<int>& t, const vector<string>& keys) {
                for(const auto& k : keys) {
                    auto it = t.find(k);
                    REQUIRE(it == t.end());
                }
            };

            test_not_found(t, {"s", "sm", "smi", "", "irrelevant"});
        }
    }


    SECTION("Insertion") {


        SECTION("cannot insert elements with duplicate keys") {
            auto test_no_dup = [](Trie<int>& t, const ssumap& insertee) {
                for(const auto& e : insertee) {
                    auto it = t.insert({e.first, e.second});
                    REQUIRE(it == t.end());
                }
            };

            auto t = make_trie({ {"pre", 1} });
            test_no_dup(t, { {"pre", 2} });
        }

        SECTION("Insert at root if there is no prefix match or neither prefix/query exhausted") {
            auto test_insert_at_root = [](Trie<int>& t, const ssumap& insertee) {
                for(const auto& e : insertee) {
                    auto it = t.insert({e.first, e.second});
                    REQUIRE(it != t.end());
                    REQUIRE(--it == t.end());
                }
            };

            auto t = make_trie({ {"pre", 1} });
            test_insert_at_root(t, { {"no_shared_prefix_at_all", 2},  {"pr_neither_exhausted", 3} });
        }

        SECTION("Traverse down a level, if prefix is exhausted, while query string has some chars left") {
            auto test_go_down = [](Trie<int>& t, const ssumap& insertee) {
                for(const auto& e : insertee) {
                    auto it = t.insert({e.first, e.second});
                    REQUIRE(--it != t.end());
                    REQUIRE(--(--it) == t.end());
                }
            };

            auto t = make_trie({ {"banana", 1}, {"apple", 2}, {"application", 3} });
            test_go_down(t, { {"banananana", 4}, {"apples", 5}, {"applications", 6} });
        }

        SECTION("Reorganization of nodes if there is a range (len>=1) for which query exhausted") {
            auto test_reorganize = [](Trie<int>& t, const ssumap& insertee) {
                for(const auto& e : insertee) {
                    auto it = t.insert({e.first, e.second});
                    REQUIRE(it != t.end());
                    REQUIRE(--it == t.end());
                }
            };

            auto t = make_trie({ {"happy", 1}, {"happiness", 2}, {"happening", 3} });
            test_reorganize(t, { {"happ", 4} });
        }


        SECTION("Comprehensive example") {

            auto t = make_trie({ 
                {"smile", 1}, 
                {"smiles", 2}, 
                {"smiling", 3},
                {"smiled", 4},
                {"smil", 5}
            });

            // cout << t;
        }



    }

}