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
    using MappedT               = T;
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

    // Store pointer, pointing to newly allocated node, into edges
    PointerT add_child(KeyT key);
    PointerT add_child(KeyT key, MappedT value);



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
auto TrieNode<T, CharT>::add_child(KeyT key) -> PointerT 
{
    PointerT new_node = new TrieNode(this);
    edges.emplace_back(key, new_node);
    std::sort(edges.begin(), edges.end());
    return new_node;
}

template<typename T, typename CharT> 
auto TrieNode<T, CharT>::add_child(KeyT key, MappedT value) -> PointerT 
{
    PointerT new_node = new TrieNode(this, value);
    edges.emplace_back(key, new_node);
    std::sort(edges.begin(), edges.end());
    return new_node;
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
    using NodePointerT      = typename NodeT::PointerT; 

    using IteratorT         = TrieIterator<T, CharT>;
    using ConstIteratorT    = const TrieIterator<T, CharT>;
private:
    NodePointerT              begin_node_ = end_node();
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
Trie<T, CharT, Compare, Allocator>::Trie() { }


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
    if(!size_) {
        end_node_.add_child(KeyT());
    }

    // NodePointerT curr_node = root_node();

    
    return IteratorT();
}





} // namespace Summer
#endif // __TRIE2_H__