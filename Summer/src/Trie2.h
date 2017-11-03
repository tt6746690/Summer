#ifndef __TRIE2_H__
#define __TRIE2_H__

#include <cassert>
#include <memory> // smart pointers
#include <string>
#include <utility> // pair
#include <vector>
#include <map>

namespace Summer
{

template <typename NodeType>
struct TrieNodeEdge
{
    using UNodePointerT = typename NodeType::UPointerT;
    std::string         prefix;
    UNodePointerT       child;
};

template <typename T>
class TrieNode
{
  public:
    using ValueT                = T;
    using PointerT              = TrieNode *;
    using UPointerT             = std::unique_ptr<TrieNode<T>>;
    using SPointerT             = std::shared_ptr<TrieNode<T>>;
    using ReferenceT            = TrieNode &;
    using ConstReferenceT       = const TrieNode &;
    using EdgeT                 = TrieNodeEdge<TrieNode>;

    T                             value;
    PointerT                      parent;
    std::vector<EdgeT>            edges;

    explicit TrieNode() {}
    explicit TrieNode(PointerT parent, ValueT data) : parent(parent), value(value) {}

    friend bool operator< (const TrieNode &rhs, const TrieNode &lhs);
    friend inline bool operator<=(const TrieNode &rhs, const TrieNode &lhs) { return !(lhs < rhs); }
    friend inline bool operator> (const TrieNode &rhs, const TrieNode &lhs) { return  (lhs < rhs); }
    friend inline bool operator>=(const TrieNode &rhs, const TrieNode &lhs) { return !(rhs < lhs); }
    friend inline bool operator==(const TrieNode &rhs, const TrieNode &lhs) { return !(rhs < lhs) && !(lhs < rhs); }
    friend inline bool operator!=(const TrieNode &rhs, const TrieNode &lhs) { return  (rhs < lhs) ||  (lhs < rhs); }
};

template <typename T>
bool operator<(const TrieNode<T> &rhs, const TrieNode<T> &lhs)
{
    if (rhs.value != lhs.value) return rhs.value < lhs.value;
    else {
        if (rhs.parent != lhs.parent) return rhs.parent < lhs.parent;
        else return rhs.edges < lhs.edges;
    }
}

template<typename T>
class TrieIterator {
    using NodeT             = TrieNode<T>;
    using NodePointerT      = typename NodeT::PointerT;

    NodePointerT              ptr_;
public:
    using ValueT            = T;
    using PointerT          = T *;
    using ReferenceT        = ValueT &;
    using ConstReferenceT   = const ValueT &; 

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
          typename AllocatorTrait = std::allocator_traits<std::pair<const std::basic_string<CharT>, T>>> 
class Trie {
public:
    using KeyT              = std::basic_string<CharT>;
    using KeyCompare        = Compare;

    using AllocatorT        = AllocatorTrait;
    using ValueT            = typename AllocatorT::value_type;
    using ReferenceT        = typename AllocatorT::value_type &;
    using ConstReferenceT   = const typename AllocatorT::value_type &;
    using PointerT          = typename AllocatorT::pointer;
    using ConstPointerT     = typename AllocatorT::const_pointer;
    using SizeT             = typename AllocatorT::size_type;
    using DifferenceT       = typename AllocatorT::difference_type;
public: 
    using NodeT             = TrieNode<T>;
    using NodePointerT      = typename NodeT::PointerT; 

    using IteratorT         = TrieIterator<T>;
    using ConstIteratorT    = const TrieIterator<T>;
private:
    IteratorT                 end_;
};


} // namespace Summer
#endif // __TRIE2_H__