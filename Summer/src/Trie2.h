#ifndef __TRIE2_H__
#define __TRIE2_H__

#include <cassert>
#include <memory> // smart pointers
#include <string>
#include <utility> // pair
#include <vector>
#include <array>
#include <map>
#include <iostream>
#include <algorithm>

#include "StrUtils.h"

using std::cout;
using std::endl; 
using std::ostream;

constexpr char eol() { return '\n'; }

namespace Summer
{


template<typename CharT>
auto lex_cmp = [](const std::basic_string<CharT>& x, const std::basic_string<CharT>& y) -> bool {
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
};


template <typename NodeType>
struct TrieNodeEdge
{
    typename NodeType::KeyT             prefix;
    typename NodeType::PointerT         child;

    explicit TrieNodeEdge() : prefix(""), child(nullptr) {}
    explicit TrieNodeEdge(typename NodeType::KeyT k, typename NodeType::PointerT p) : prefix(k), child(p) {}
    friend inline bool operator< (const TrieNodeEdge& lhs, const TrieNodeEdge& rhs) { return lex_cmp<typename NodeType::KeyCharT>(lhs.prefix, rhs.prefix); }
};





template <typename T, typename CharT>
class TrieNode
{
  public:
    using KeyCharT              = CharT;
    using KeyT                  = std::basic_string<CharT>;

    using PointerT              = TrieNode *;
    using UPointerT             = std::unique_ptr<TrieNode<T, CharT>>;
    using SPointerT             = std::shared_ptr<TrieNode<T, CharT>>;
    using ReferenceT            = TrieNode &;
    using ConstReferenceT       = const TrieNode &;

    using EdgeT                 = TrieNodeEdge<TrieNode<T, CharT>>;
    using EdgesT                = std::vector<EdgeT>;

    PointerT                      parent;
    T                             value;
    EdgesT                        edges;

  public:
    explicit TrieNode() : parent(nullptr), value() {}
    explicit TrieNode(PointerT parent) : parent(parent) {}    
    explicit TrieNode(PointerT parent, T value) : parent(parent), value(value) {}

    std::size_t edge_size() const { return edges.size(); }
    void sort_edges() { std::sort(edges.begin(), edges.end()); }
    
    // Store pointer, pointing to newly allocated node, into edges
    PointerT add_edge(KeyT key);
    PointerT add_edge(KeyT key, T value);
    
    // Transfers ownership of child node while removing corresponding entry from edges 
    PointerT yield_child(size_t i);

    // Returns value for  1) this  2) this->edges[i] 
    T& value();
    T& child_value(size_t i);


    friend bool operator< (const TrieNode &rhs, const TrieNode &lhs);
    friend inline bool operator<=(const TrieNode &rhs, const TrieNode &lhs) { return !(lhs < rhs); }
    friend inline bool operator> (const TrieNode &rhs, const TrieNode &lhs) { return  (lhs < rhs); }
    friend inline bool operator>=(const TrieNode &rhs, const TrieNode &lhs) { return !(rhs < lhs); }
    friend inline bool operator==(const TrieNode &rhs, const TrieNode &lhs) { return !(rhs < lhs) && !(lhs < rhs); }
    friend inline bool operator!=(const TrieNode &rhs, const TrieNode &lhs) { return  (rhs < lhs) ||  (lhs < rhs); }
};

template <typename T, typename CharT>
bool operator<(const TrieNode<T, CharT> &rhs, const TrieNode<T, CharT> &lhs)
{
    if (rhs.value != lhs.value) return rhs.value < lhs.value;
    else {
        if (rhs.parent != lhs.parent) return rhs.parent < lhs.parent;
        else return rhs.edges < lhs.edges;
    }
}


template<typename T, typename CharT> 
auto TrieNode<T, CharT>::add_edge(KeyT key) -> PointerT 
{
    PointerT new_node = new TrieNode(this);
    edges.emplace_back(key, new_node);
    return new_node;
}

template<typename T, typename CharT> 
auto TrieNode<T, CharT>::add_edge(KeyT key, T value) -> PointerT 
{
    PointerT new_node = new TrieNode(this, value);
    edges.emplace_back(key, new_node);
    return new_node;
}

template<typename T, typename CharT> 
auto TrieNode<T, CharT>::yield_child(size_t i) -> PointerT
{
    return edges[i];
}

template<typename T, typename CharT>
auto TrieNode<T, CharT>::value() -> T& 
{
    return &value;
}

template<typename T, typename CharT> 
auto TrieNode<T, CharT>::child_value(size_t i) -> T&
{
    return (edges[i].child)->value();
}




template<typename T, typename CharT>
class TrieIterator {    
    using ValueT            = T;
    using PointerT          = T *;
    using ReferenceT        = ValueT &;
    using ConstReferenceT   = const ValueT &; 
    using NodeT             = TrieNode<T, CharT>;
    using NodePointerT      = typename NodeT::PointerT;

    NodePointerT              ptr_;
public:

    explicit TrieIterator() : ptr_(nullptr) { }
    explicit TrieIterator(NodePointerT ptr) : ptr_(ptr) { }

    ReferenceT           operator* () const  { return   ptr_->value; }
    PointerT             operator->() const  { return &(ptr_->value); }
    TrieIterator        &operator++()        { return *this; } // TODO
    TrieIterator        &operator++(int)     { TrieIterator it(*this); ++(*this); return it; } 
    TrieIterator        &operator--()        { return *this; } // TODO 
    TrieIterator        &operator--(int)     { TrieIterator it(*this); --(*this); return it; }
    friend inline bool   operator==(const TrieIterator& lhs, const TrieIterator& rhs)    { return lhs.ptr_ == rhs.ptr_; }
    friend inline bool   operator!=(const TrieIterator& lhs, const TrieIterator& rhs)    { return !(lhs == rhs); }
};



template <typename T, typename CharT = char, typename Compare = std::less<CharT>,
          typename Allocator = std::allocator<std::pair<const std::basic_string<CharT>, T>>> 
class Trie {
public:
    using KeyT              = std::basic_string<CharT>;
    using MappedT           = T;
    using KeyCompare        = Compare;

    using AllocatorT        = std::allocator_traits<Allocator>;
    using ValueT            = typename AllocatorT::value_type;
    using ReferenceT        = typename AllocatorT::value_type &;
    using ConstReferenceT   = const typename AllocatorT::value_type &;
    using PointerT          = typename AllocatorT::pointer;
    using ConstPointerT     = typename AllocatorT::const_pointer;
    using SizeT             = typename AllocatorT::size_type;
    using DifferenceT       = typename AllocatorT::difference_type;
public: 
    using NodeT             = TrieNode<T, CharT>;
    using NodePointerT      = TrieNode<T, CharT> *; 
    using EdgeT             = typename NodeT::EdgeT;

    using IteratorT         = TrieIterator<T, CharT>;
    using ConstIteratorT    = const TrieIterator<T, CharT>;
private:
    NodePointerT              begin_node_;
    NodeT                     end_node_;
    SizeT                     size_ = 0;
public: 
    NodePointerT  end_node()   { return static_cast<NodePointerT>(&end_node_); }
    NodePointerT &begin_node() { return begin_node_; }
    NodePointerT  root_node()  { return end_node(); }
    SizeT         size() const { return size_; }

    explicit Trie();
    ~Trie();

    void destroy(NodePointerT nptr);
    auto begin() -> IteratorT { return IteratorT(begin_node()); }
    auto end()   -> IteratorT { return IteratorT(end_node());   }
    auto begin() const -> ConstIteratorT { return IteratorT(begin_node()); } 
    auto end()   const -> ConstIteratorT { return IteratorT(end_node()); }
    auto insert(const ValueT& value) -> IteratorT;

    
};



template<typename T, typename CharT, typename Compare, typename Allocator> 
Trie<T, CharT, Compare, Allocator>::Trie() 
{ 
    begin_node_ = end_node();
}


template<typename T, typename CharT, typename Compare, typename Allocator> 
Trie<T, CharT, Compare, Allocator>::~Trie()
{
    destroy(root_node());
}

template<typename T, typename CharT, typename Compare, typename Allocator> 
void Trie<T, CharT, Compare, Allocator>::destroy(NodePointerT nptr) 
{
    if(nptr != nullptr) {
        for(typename Trie::NodeT::EdgeT& edge : nptr->edges) {
            destroy(edge.child);
        }
    }
}

template<typename T, typename CharT, typename Compare, typename Allocator> 
auto Trie<T, CharT, Compare, Allocator>::insert(const ValueT& value) -> IteratorT
{
    NodePointerT cur_node = root_node();
    const CharT* kstr = value.first.data();

    while(cur_node != nullptr) 
    {
        int longest_prefix_len = 0;
        int lower_bound = 0;

        for(int i = 0; i < cur_node->edges.size(); ++i) {
            const auto& edge = cur_node->edges[i];

            int len = find_common_prefix_len(edge.prefix.c_str(), kstr);
            if(len == strlen(kstr) && len == edge.prefix.size()) return end();  // insertion with duplicate key failed

            // Found a match s.t. edge.prefix is exhausted, go down the tree. 
            // (There cannot be another edge whose prefix is a longer match)
            if(len == edge.prefix.size()) {
                kstr += i;
                cur_node = edge.child;
                break;
            }
            
            // Found a match s.t. key is exhausted, 
            // Create a new node, and move cur_node to be child of the newly added node with updated key
            if(len == strlen(kstr)) {
                std::string new_k, updated_k;
                split_in_half(edges.prefix, len, new_k, updated_k);
                NodePointerT new_node = cur_node->add_edge(new_k, value.second);
                new_node->add_edge(updated_k, cur_node->child_value(i));
                
                // Erase last...
                cur_node.erase(cur_node.begin() + i);
                return;
            }

            // Found an edge with a longer prefix, start of a range of possibly equally long prefixes 
            if(len > longest_prefix_len) {
                longest_prefix_len = len;
                lower_bound = i;
            } 
            
            // [lower_bound, i] holds a prefix match
            if(len < longest_prefix_len) {

            }
        }

    }

    // cur_node is nullptr 

    
    return IteratorT();
}





} // namespace Summer
#endif // __TRIE2_H__