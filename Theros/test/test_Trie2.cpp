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



//TEST_CASE("prefix pattern ", "Trie2")
//{
//
//
//    SECTION("routing_example")
//    {
//
//
//
//    }
//}


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

            for(auto& e: query_result) {
                {
                    auto range = node.find_lmp_edges(e.query);
                    REQUIRE(range.first <= range.second);
                    REQUIRE(range.first == node.edges.begin() + e.lower_bound);
                    REQUIRE(range.second == node.edges.begin() + e.upper_bound);
                }

                {
                    std::vector<std::pair<std::string, std::string>> kvs;
                    auto range = node.find_lmp_edges(e.query, kvs);
                    REQUIRE(range.first <= range.second);
                    REQUIRE(range.first - node.edges.begin() == e.lower_bound);
                    REQUIRE(range.second - node.edges.begin() == e.upper_bound);
                }
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

        struct lmp_expect2 {
            string query;
            int lower_bound;
            int upper_bound;
            vector<pair<string, string>> kvs;
        };
        using spvec2 = vector<lmp_expect2>;


        // tests kvs are extracted properly 
        auto test_find_lmp2 = [](ssumap edges, spvec2 query_result) {
            auto node = TrieNode<int, char>();
            for(const auto& e : edges) {
                node.add_edge(e.first, e.second);
            }
            REQUIRE(node.edges.size() == edges.size());

            for(auto& e: query_result) 
            {
                std::vector<std::pair<std::string, std::string>> kvs;
                kvs.clear();
                auto range = node.find_lmp_edges(e.query, kvs);
//
//                cout <<  e.query << " " << e.lower_bound << " - " << e.upper_bound << endl;
//                cout << "range " << range.first - node.edges.begin() << " - " << range.second - node.edges.begin() << endl;
//                cout << "expected kvs" << e.kvs << endl;
//                cout << "kvs   " << kvs << endl;
                     
                REQUIRE(kvs == e.kvs);
                REQUIRE(range.first <= range.second);
                REQUIRE(range.first - node.edges.begin() == e.lower_bound);
                REQUIRE(range.second - node.edges.begin() == e.upper_bound);
            }
        };

        test_find_lmp2({
            {"/<id>/data", 0},
            {"/<id>/ok", 1},
            {"/home", 2}
        }, {
            // exact match, prefix and query exhausted : return (i, i), throw error
            {"/12345/ok", 1, 1, { {"id", "12345"} }},
            {"/12345/data", 0, 0, { {"id", "12345"} }},
            {"/home", 2, 2, {}}
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
                    {
                        auto it = t.find(e.first);
                        REQUIRE(it != t.end());
                        REQUIRE(*it == e.second);
                    }
                    {
                        vector<pair<string, string>> kvs;
                        auto it = t.find(e.first, kvs);
                        REQUIRE(it != t.end());
                        REQUIRE(*it == e.second);
                    }
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
                    {
                        auto it = t.find(k);
                        REQUIRE(it == t.end());
                    }
                    {
                        vector<pair<string, string>> kvs;
                        auto it = t.find(k, kvs);
                        REQUIRE(it == t.end());
                    }
                }
            };

            test_not_found(t, {"s", "sm", "smi", "", "irrelevant"});
        }

        auto t2 = make_trie({
            {"/", 1},
            {"/textbook/<author>", 2},
            {"/textbook/publish_date/<date>", 3},
            {"/user/<id>", 4},
            {"/user/<id>/books/<book_id>", 5}
        });


        /**
         \	( 0 )
            |-/ 	( 1 )
                |-textbook/<author> 	( 2 )
                |-textbook/publish_date/<date> 	( 3 )
                |-user/<id> 	( 4 )
                    |-/books/<book_id> 	( 5 )
        */

        SECTION("found_with_kvs")
        {

            struct exp_found {
                std::string query;
                int handle_id;
                vector<pair<string, string>> kvs;
            };
            using expect_t = vector<exp_found>;

            auto test_found = [](Trie<int>& t, const expect_t& expect) {
                for(const auto& e : expect) {
                    vector<pair<string, string>> kvs_r;
                    auto it = t.find(e.query, kvs_r);
                    REQUIRE(it != t.end());
                    REQUIRE(*it == e.handle_id);
                    REQUIRE(kvs_r == e.kvs);
                }
            };

            test_found(t2, {
                {"/", 1, {}},
                {"/textbook/mrs_bar", 2, {{"author", "mrs_bar"}}},
                {"/textbook/publish_date/2004", 3, {{"date", "2004"}}},
                {"/user/mr_foo", 4, {{"id", "mr_foo"}}}, 
                {"/user/mr_foo/books/foos_grand_journey", 5, {{"id", "mr_foo"}, {"book_id", "foos_grand_journey"}}}
            });
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