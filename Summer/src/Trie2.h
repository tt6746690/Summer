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
#include <iostream>

#include "StrUtils.h"

using std::cout;
using std::endl; 
using std::ostream;

#define eol '\n'

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

    template<typename X>
    friend std::ostream& operator<<(std::ostream& os, const TrieNodeEdge<X>& e);
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
    using EdgesIterator         = typename std::vector<EdgeT>::iterator;

    PointerT                      parent;
    T                             value;
    EdgesT                        edges;

  public:
    explicit TrieNode() : parent(nullptr), value() {}
    explicit TrieNode(PointerT parent) : parent(parent) {}    
    explicit TrieNode(PointerT parent, T value) : parent(parent), value(value) {}

    std::size_t edge_size() const { return edges.size(); }
    void sort_edges() { std::sort(edges.begin(), edges.end()); }

    T& child_value(size_t i) { return (edges[i].child)->value;};
    bool has_child(PointerT node_ptr);
    
    // Store pointer, pointing to newly allocated node, into edges
    PointerT add_edge(KeyT key);
    PointerT add_edge(KeyT key, PointerT node_ptr);
    PointerT add_edge(KeyT key, T value);

    // Finds a range of edge (as idx to edges) with longest matching prefix as query string 
    std::pair<EdgesIterator, EdgesIterator> find_lmp_edges(KeyT query);

  public:
    friend bool operator< (const TrieNode &rhs, const TrieNode &lhs);
    friend inline bool operator<=(const TrieNode &rhs, const TrieNode &lhs) { return !(lhs < rhs); }
    friend inline bool operator> (const TrieNode &rhs, const TrieNode &lhs) { return  (lhs < rhs); }
    friend inline bool operator>=(const TrieNode &rhs, const TrieNode &lhs) { return !(rhs < lhs); }
    friend inline bool operator==(const TrieNode &rhs, const TrieNode &lhs) { return !(rhs < lhs) && !(lhs < rhs); }
    friend inline bool operator!=(const TrieNode &rhs, const TrieNode &lhs) { return  (rhs < lhs) ||  (lhs < rhs); }

    template<typename X, typename Y>
    friend std::ostream& operator<<(std::ostream& os, const TrieNode<X, Y>& node);
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
auto TrieNode<T, CharT>::has_child(PointerT node_ptr) -> bool 
{
    for(const auto& edge : edges) {
        if(edge.child == node_ptr) return true;
    }
    return false;
}

template<typename T, typename CharT>
auto TrieNode<T, CharT>::add_edge(KeyT key, PointerT node_ptr) -> PointerT 
{
    edges.emplace_back(key, node_ptr);
    return node_ptr;
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
auto TrieNode<T, CharT>::find_lmp_edges(KeyT query) -> std::pair<EdgesIterator, EdgesIterator>
{
    sort_edges();
    
    int longest_prefix_len = 0;
    int lower_bound = -1, upper_bound = -1;

    for(int i = 0; i < edges.size(); ++i) {
        const auto& edge = edges[i];
        int len = find_common_prefix_len(edge.prefix.c_str(), query.c_str());
        if(len == query.size() && len == edge.prefix.size()) 
            return std::make_pair(edges.begin() + i, edges.begin() + i);
        // Found an edge with a longer prefix, start of a range of possibly equally long prefixes 
        if(len > longest_prefix_len) {
            longest_prefix_len = len;
            lower_bound = i;
        }         
        // [lower_bound, i) holds a prefix match
        if(len < longest_prefix_len) {
            upper_bound = i;
            break;
        }
    }

    if((lower_bound == -1 && upper_bound == -1) || 
        (query.size() > longest_prefix_len && edges[lower_bound].prefix.size() > longest_prefix_len))
        return std::make_pair(edges.end(), edges.end());

    upper_bound = (upper_bound >= lower_bound) ? upper_bound : edges.size();
    return std::make_pair(edges.begin() + lower_bound, edges.begin() + upper_bound);
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
    TrieIterator        &operator--()        { ptr_ = (ptr_->parent) ? ptr_->parent : ptr_; return *this; } 
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
    using EdgesT            = typename NodeT::EdgesT;    
    using EdgesIterator     = typename NodeT::EdgesIterator;

    using IteratorT         = TrieIterator<T, CharT>;
    using ConstIteratorT    = const TrieIterator<T, CharT>;
private:
    NodePointerT              begin_node_;
    NodeT                     end_node_;
    SizeT                     size_ = 0;
public: 
    NodePointerT  end_node()        { return static_cast<NodePointerT>(&end_node_); }
    NodePointerT  end_node() const  { return const_cast<const NodePointerT>(&end_node_); }    
    NodePointerT &begin_node()      { return begin_node_; }
    NodePointerT  root_node()       { return end_node(); }
    NodePointerT  root_node() const { return end_node(); }
    SizeT         size() const      { return size_; }

    explicit Trie();
    ~Trie();

    void destroy(NodePointerT nptr);
    auto begin() -> IteratorT { return IteratorT(begin_node()); }
    auto end()   -> IteratorT { return IteratorT(end_node());   }
    auto begin() const -> ConstIteratorT { return IteratorT(begin_node()); } 
    auto end()   const -> ConstIteratorT { return IteratorT(end_node()); }

    auto insert(const ValueT& value) -> IteratorT;
    auto find(const KeyT& key) -> IteratorT;

public:
    template<typename T1, typename T2, typename T3, typename T4>
    friend std::ostream& operator<<(const std::ostream& os, const Trie<T1, T2, T3, T4>& t);
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
        auto range = cur_node->find_lmp_edges(kstr);
        const EdgesT& edges = cur_node->edges;

        // std::cout << "end=" << edges.end() - edges.begin()
        //     << " [" << range.first-edges.begin() << ", " << range.second-edges.begin() << ")"  << eol;

        if(range.first == range.second) {
            if(range.first == edges.end()) {
                // No prefix match, or neither prefix/query exhausted
                cur_node = cur_node->add_edge(kstr, value.second);
                ++size_;
                return IteratorT(cur_node);
            } else {
                // Exact match, duplicate key not allowed
                return end();
            }
        } else {
            EdgesIterator& lower_bound = range.first;
            int match_len = find_common_prefix_len(lower_bound->prefix.c_str(), kstr);
            if(match_len == lower_bound->prefix.size()) {
                // prefix exhausted, go down next level
                kstr += match_len;
                cur_node = lower_bound->child;
                continue;
            } else {
                // query exhausted, transfer range of node to newly added node's edges                                        
                assert(match_len == strlen(kstr));
                NodePointerT new_node = new NodeT(cur_node, value.second);

                std::for_each(range.first, range.second, [&new_node, match_len](const auto& edge){
                    new_node->add_edge(edge.prefix.substr(match_len, edge.prefix.size()), edge.child);
                });
                
                cur_node->edges.erase(range.first, range.second);
                cur_node->add_edge(kstr, new_node);
                ++size_;
                return IteratorT(new_node);
            }
        }

    }
    return end();
}

template<typename T, typename CharT, typename Compare, typename Allocator> 
auto  Trie<T, CharT, Compare, Allocator>::find(const KeyT& key) -> IteratorT 
{
    NodePointerT cur_node = root_node();
    const CharT* kstr = key.data();

    while(cur_node != nullptr) 
    {
        auto range = cur_node->find_lmp_edges(kstr);
        const EdgesT& edges = cur_node->edges;

        if(range.first == range.second) {
            return (range.first != edges.end()) ? IteratorT((range.first)->child) : end();
        } else {
            EdgesIterator& lower_bound = range.first;
            int match_len = find_common_prefix_len(lower_bound->prefix.c_str(), kstr);
            if(match_len == lower_bound->prefix.size()) {
                kstr += match_len;
                cur_node = lower_bound->child;
                continue;
            } else return end();
        }
    }
    return end();
}


template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p) {
    return os << "(" << p.first << ", " << p.second << ")" << '\n';
}


template<typename T> 
std::ostream& operator<<(std::ostream& os, const std::vector<T>& c) {
    for(const auto& e: c) os << e << eol;
    return os;
}

template <typename NodeType> 
std::ostream& operator<<(std::ostream& os, const TrieNodeEdge<NodeType>& e)
{
    return os <<  "|-" << e.prefix << " " << *e.child;
}

template <typename T, typename CharT>
std::ostream& operator<<(std::ostream& os, const TrieNode<T, CharT>& node)
{

    static size_t depth = 0;
    os << '\t' << "( " << node.value << " )" << eol;
    if(node.edges.size()) {
        depth += 2;
        for(const auto& e: node.edges) {
            os << std::string(depth, ' ') << e;
        }
        depth -= 2;
    }
    return os;
}

template <typename T, typename CharT, typename Compare, typename Allocator>
std::ostream& operator<<(std::ostream& os, const Trie<T, CharT, Compare, Allocator>& t)
{
    return os << "\\" << *t.root_node() << eol << "size = (" << t.size() << ")" << eol;
}




} // namespace Summer
#endif // __TRIE2_H__