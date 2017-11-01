#include "catch.hpp"
#include <utility>
#include <stdexcept>

#include "Trie.h"

using namespace Http;

TEST_CASE("TrieNode", "[Trie]")
{
    Trie<std::string>::TrieNode n{};

    REQUIRE(n.data_ == "");
    REQUIRE(n.parent_ == nullptr);
    REQUIRE(n.child_.size() == 0);
}

TEST_CASE("Trie Full example", "[Trie]")
{
    Trie<std::string> t{};
    // using node_ptr = Trie<std::string>::TrieNode *;
    using iterator = Trie<std::string>::TrieIterator;

    t.insert({"smile", "smile"});
    t.insert({"smiled", "smiled"});
    t.insert({"smiles", "smiles"});

    /*
    |-smile\          smile
        |-s\          smiles
        |-d\          smiled
    */
    t.insert({"smiling", "smiling"});
    /*
    |-smil\                         // branch node 
        |-ing\          smiling     // input node
        |-e\            smile       // prev node
            |-s\        smiles
            |-d\        smiled
    */
    // std::cout << t << std::endl;

    SECTION("prefix_of")
    {
        iterator found;
        std::string prefix;

        found = t.find("smil");
        prefix = t.prefix_of(found);
        REQUIRE(prefix == "smil");

        found = t.find("smile");
        prefix = t.prefix_of(found);
        REQUIRE(prefix == "smile");

        found = t.find("smiled");
        prefix = t.prefix_of(found);
        REQUIRE(prefix == "smiled");
    }

    SECTION("find")
    {
        auto smil = get_child(t.root_, "smil");
        auto smiling = get_child(smil, "ing");
        auto smile = get_child(smil, "e");
        auto smiled = get_child(smile, "d");
        auto smiles = get_child(smile, "s");

        REQUIRE_NOTHROW(get_child(smile, "d"));
        REQUIRE_NOTHROW(get_child(smile, "s"));
        REQUIRE_THROWS_AS(get_child(smil, "d"), std::out_of_range);
        REQUIRE_THROWS_AS(get_child(smil, "s"), std::out_of_range);

        REQUIRE(smil->data_ == "");
        REQUIRE(smile->data_ == "smile");
        REQUIRE(smiled->data_ == "smiled");
        REQUIRE(smiles->data_ == "smiles");
        REQUIRE(smiling->data_ == "smiling");

        iterator found;
        found = t.find("smil");
        REQUIRE(found.node_ == smil);
        REQUIRE(*found == "");
        found = t.find("smile");
        REQUIRE(found.node_ == smile);
        REQUIRE(*found == "smile");
        found = t.find("smiled");
        REQUIRE(found.node_ == smiled);
        REQUIRE(*found == "smiled");
        found = t.find("smiles");
        REQUIRE(found.node_ == smiles);
        REQUIRE(*found == "smiles");
        found = t.find("smiling");
        REQUIRE(found.node_ == smiling);
        REQUIRE(*found == "smiling");
    }
}

TEST_CASE("Trie::{find_to_insert, insert}", "[Trie]")
{
    Trie<std::string> t{};
    using node_ptr = Trie<std::string>::TrieNode *;

    REQUIRE(t.size_ == 0);
    REQUIRE(t.root_->data_ == "");
    REQUIRE(t.root_->parent_ == nullptr);
    REQUIRE(t.root_->child_.size() == 0);

    SECTION("different strings")
    {
        t.insert(std::make_pair("a", "value1"));
        t.insert(std::make_pair("b", "value2"));

        REQUIRE(get_child(t.root_, "a")->data_ == "value1");
        REQUIRE(get_child(t.root_, "b")->data_ == "value2");
        REQUIRE(t.root_->child_.size() == 2);
    }

    SECTION("previous key is substring")
    {
        /*
            pre         
            presuffix   

            should append new key 

            pre\
                |- suffix
        */
        auto prev_key = std::make_pair("pre", "shortstr");
        auto new_key = std::make_pair("presuffix", "longstr");

        t.insert(prev_key);
        REQUIRE(t.root_->child_["pre"]->data_ == prev_key.second);
        REQUIRE(t.root_->child_.size() == 1);

        SECTION("find_to_insert")
        {
            node_ptr found;
            std::string suffix;
            std::tie(found, suffix) = t.find_to_insert(new_key.first);

            REQUIRE(get_child(t.root_, "pre") == found);
            REQUIRE(suffix == "suffix");
        }

        SECTION("insert")
        {
            t.insert(new_key);

            auto node = t.root_->child_["pre"].get();
            REQUIRE(t.root_->child_.size() == 1);
            REQUIRE(node->child_.size() == 1);
            REQUIRE(node->data_ == "shortstr");
            REQUIRE(node->child_.at("suffix")->data_ == "longstr");
        }
    }

    SECTION("new key is substring")
    {
        /*
            presuffix         
            pre

            pre\
                |- suffix
        */
        auto prev_key = std::make_pair("presuffix", "prev_key");
        auto new_key = std::make_pair("pre", "new_key");

        t.insert(prev_key);

        REQUIRE(t.root_->child_["presuffix"]->data_ == prev_key.second);
        REQUIRE(t.root_->child_.size() == 1);

        SECTION("find_to_insert")
        {
            node_ptr found;
            std::string suffix;
            std::tie(found, suffix) = t.find_to_insert(new_key.first);

            auto node = t.root_.get();
            REQUIRE(node == found);
            REQUIRE(suffix == "pre");
        }

        SECTION("insert")
        {
            t.insert(new_key);
            REQUIRE_THROWS_AS(t.root_->child_.at("presuffix"), std::out_of_range);
            REQUIRE_NOTHROW(t.root_->child_.at("pre"));

            auto branch = t.root_->child_.at("pre").get();
            REQUIRE(branch->data_ == new_key.second);
            REQUIRE(branch->child_.size() == 1);
            REQUIRE_NOTHROW(branch->child_.at("suffix"));
            REQUIRE(branch->child_.at("suffix").get()->data_ == prev_key.second);
        }
    }

    SECTION("keys share common prefix")
    {
        /*
            should split 
            pre1         
            pre2  

            pre\
                |- 1        str1
                |- 2        str2
        */
        auto str1 = std::make_pair("pre1", "str1");
        auto str2 = std::make_pair("pre2", "str2");

        t.insert(str1);
        REQUIRE(t.root_->child_["pre1"]->data_ == str1.second);
        REQUIRE(t.root_->child_.size() == 1);

        SECTION("find_to_insert")
        {
            node_ptr found;
            std::string suffix;
            std::tie(found, suffix) = t.find_to_insert(str2.first);

            auto node = t.root_.get();
            REQUIRE(node == found);
            REQUIRE(suffix == "pre2");
        }

        SECTION("insert")
        {
            t.insert(str2);

            REQUIRE_THROWS_AS(t.root_->child_.at("pre1"), std::out_of_range);
            REQUIRE_THROWS_AS(t.root_->child_.at("pre2"), std::out_of_range);
            REQUIRE_NOTHROW(t.root_->child_.at("pre"));

            auto branch = t.root_->child_.at("pre").get();
            REQUIRE(branch->data_ == "");
            REQUIRE(branch->child_.size() == 2);
            REQUIRE_NOTHROW(branch->child_.at("1"));
            REQUIRE_NOTHROW(branch->child_.at("2"));
            REQUIRE(branch->child_.at("1").get()->data_ == "str1");
            REQUIRE(branch->child_.at("2").get()->data_ == "str2");
        }
    }
}