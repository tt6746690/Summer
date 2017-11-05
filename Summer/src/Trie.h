#ifndef __TRIE_H__
#define __TRIE_H__

#include <string>
#include <utility>
#include <cstddef>
#include <unordered_map>
#include <memory>
#include <ostream>
#include <algorithm>
#include <cassert>

#include "Utils.h"

namespace Summer
{

template <typename T>
class Trie
{
public:
  struct TrieNode;
  class TrieIterator;

  using key_type = std::string;
  using mapped_type = T;
  using value_type = std::pair<const key_type, T>;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = TrieIterator;
  using const_iterator = const TrieIterator;

  using node_ptr = TrieNode *;
  using trie_ptr = Trie *;
  using uniq_node_ptr = std::unique_ptr<TrieNode>;
  using shared_node_ptr = std::shared_ptr<TrieNode>;

public:
  /**
   * @brief   Node of a Trie
   * 
   * @param   data_       auxiliary data 
   *                      internal node:  may or may not contain data_
   *                      leaf node: must has valid data_
   * @param   parent_     pointer to parent node, nullptr for root_
   * @param   child_      a mapping of prefixes and pointers to associated nodes 
   */
  struct TrieNode
  {
    T data_;
    node_ptr parent_ = nullptr;
    std::unordered_map<std::string, uniq_node_ptr> child_;

  public:
    explicit TrieNode(){};
    explicit TrieNode(node_ptr parent, T data = T())
        : data_(data), parent_(parent){};

    bool operator==(const TrieNode &rhs) const
    {
      return (data_ == rhs.data_) &&
             (parent_ == rhs.parent_) &&
             (child_ == rhs.child_);
    }
  };

  /**
   * @brief   Iterator of a Trie
   * 
   * @param   trie_       points to the trie iterator belongs to
   * @param   node_       the current TrieNode iterator points to
   */
  class TrieIterator
  {

  public:
    Trie *trie_;
    node_ptr node_;

  public:
    explicit TrieIterator(){};
    explicit TrieIterator(trie_ptr trie, node_ptr node = nullptr)
        : trie_(trie), node_(node){};

    bool operator==(const TrieIterator &rhs) const
    {
      return (*node_ == *(rhs.node_));
    }
    bool operator!=(const TrieIterator &rhs) const
    {
      return !(operator==(rhs));
    }

    mapped_type &operator*() const
    {
      return node_->data_;
    }

    iterator &operator--() // predecrement
    {
      if (node_->parent_)
        node_ = node_->parent_;
      return *this;
    }

    iterator operator--(int) // postdecrement
    {
      iterator result = *this;
      --(*this);
      return result;
    }
    iterator &operator++() // predecrement
    {
      if (node_->child_)
        node_ = node_->child_.begin();
      return *this;
    }

    iterator operator++(int) // postdecrement
    {
      iterator result = *this;
      ++(*this);
      return result;
    }

  public:
    friend inline auto operator<<(std::ostream &strm, TrieIterator &itr) -> std::ostream &
    {
      strm << "[itr]" << itr.node_ << std::endl;
      return strm;
    }
  };

  Trie()
      : root_(std::make_shared<TrieNode>(nullptr)), size_(0){};

  /**
   * @brief   Iterator related functions
   */
  auto begin() -> iterator
  {
    node_ptr curr = root_.get();
    while (!curr->data_)
    {
      curr = curr->child_.begin().second.get();
    }
    return iterator(this, curr);
  }
  auto end() -> iterator
  {
    return iterator(this, root_.get());
  }

  /**
   * @brief   Inserts data to Trie given key 
   */
  auto insert(const value_type &value) -> iterator
  {
    node_ptr node;
    std::string input_key;
    std::tie(node, input_key) = find_to_insert(value.first);

    /**
     * @note  keys in map has unique char at index 0
     *        if such previous key exists, we have a common prefix, 
     *        a duplicate first char indicates a need to split the edge
     *  -- determine a common prefix 
     *  -- removes previous key->node from child_ map
     *  -- Either the branching node 
     *      -- holds data, 
     *      -- does not hold data
     *  -- transfer ownership of previous node to the branching node
     * 
     *        Otherwise we simply insert the new value to node's child map
     */
    for (auto &child : node->child_)
    {
      auto prev_key = child.first;
      if (prev_key.front() == input_key.front())
      {
        std::string::iterator prev_mismatch, input_mismatch;

        std::tie(prev_mismatch, input_mismatch) =
            mismatch(prev_key.begin(), prev_key.end(), input_key.begin());

        std::string common_prefix{prev_key.begin(), prev_mismatch};
        std::string prev_suffix{prev_mismatch, prev_key.end()};
        std::string input_suffix{input_mismatch, input_key.end()};

        node_ptr prev_node = child.second.release();
        node->child_.erase(prev_key);
        assert(prev_key != common_prefix);
        /**
         *  {common_prefix == input_suffix}\
         *                                |- prev_suffix
         */
        if (input_mismatch == input_key.end())
        {
          node->child_.emplace(
              std::make_pair(common_prefix, newNode(node, value)));
          auto branch_node = node->child_.at(common_prefix).get();

          prev_node->parent_ = branch_node;
          branch_node->child_.emplace(
              std::make_pair(prev_suffix, std::unique_ptr<TrieNode>(prev_node)));
          return iterator(this, branch_node);
        }
        /**
         *  common_prefix\
         *                |- input_suffix
         *                |- prev_suffix
         */
        else
        {
          node->child_.emplace(
              std::make_pair(common_prefix, newNode(node)));
          auto branch_node = get_child(node, common_prefix);

          prev_node->parent_ = branch_node;
          branch_node->child_.emplace(
              std::make_pair(prev_suffix, std::unique_ptr<TrieNode>(prev_node)));

          branch_node->child_.emplace(
              std::make_pair(input_suffix, newNode(node, value)));

          auto inserted_node = get_child(branch_node, input_suffix);
          return iterator(this, inserted_node);
        }
      }
    }
    /**
     *  {common_prefix == prev_suffix}\
     *                                |- input_suffix
     */
    node->child_.emplace(
        std::make_pair(input_key, newNode(node, value)));
    auto inserted_node = get_child(node, input_key);
    return iterator(this, inserted_node);
  }

  /**
   * @brief   Given a key, find a node with identical prefix
   *          Return pointer to such node if exists, otherwise to root_
   */
  auto find(const std::string &key) -> iterator
  {
    std::string prefix;
    auto suffix_size = key.size();
    auto curr = root_.get();
    /**
     * Check all prefixes of key in child's map, 
     * if a prefix is found in a child's map, then implies either 
     * -- found the exact matching node, given the prefix exhaust the key 
     * -- found the a prefix node, 
     * ---- advance the current node and 
     * ---- search the remaining suffix key excluding the common prefix 
     * if we cant always find such prefix, then implies key not in trie
     */
    for (int len = 1, pos = 0; len <= suffix_size; len++)
    {
      prefix = key.substr(pos, len);
      if (curr->child_.count(prefix))
      {
        if (len == suffix_size)
        {
          return iterator(this, get_child(curr, prefix));
        }
        else
        {
          pos += len;
          suffix_size -= len;
          len = 0;
          curr = get_child(curr, prefix);
        }
      }
    }

    return end();
  }
  auto find(const std::string &key, std::string &param_key, std::string &param_value) -> iterator
  {
    std::string prefix;
    auto suffix_size = key.size();
    auto curr = root_.get();
    int len, pos;

    for (len = 1, pos = 0; len <= suffix_size; len++)
    {
      prefix = key.substr(pos, len);
      if (curr->child_.count(prefix))
      {
        if (len == suffix_size)
        {
          return iterator(this, get_child(curr, prefix));
        }
        else
        {
          pos += len;
          suffix_size -= len;
          len = 0;
          curr = get_child(curr, prefix);
        }
      }
    }

    /* test if its a match against <param> */
    for (const auto &value : curr->child_)
    {
      auto k = value.first;
      if (k.front() == '<')
      {
        /* Assuming url parameter matching is unique, 
          prefix string will be in one node */
        param_key = std::string{k.begin() + 1, k.end() - 1};
        param_value = key.substr(pos);

        return iterator(this, get_child(curr, k));
      }
    }
    return end();
  }

  /**
   * @brief   Returns the prefix held by an iterator 
   */
  auto prefix_of(iterator curr) -> std::string
  {
    std::string prefix;
    iterator parent = iterator(this, curr.node_->parent_);

    while (curr != end())
    {
      auto found = std::find_if(
          (parent.node_)->child_.begin(), (parent.node_)->child_.end(),
          [&](auto const &p) {
            return p.second.get() == curr.node_;
          });
      assert(found != (parent.node_)->child_.end());

      prefix = (*found).first + prefix;

      --curr;
      if (!(parent == end()))
        --parent;
    }

    return prefix;
  }

  /** 
   * @brief   Given a key, find the node to insert to
   * 
   * invariant 
   *  suffix.substr(0, i) is not contained in node's map
   *  i.e. suffix must be inserted in node's map in some way
   */
  auto find_to_insert(const std::string &key) -> std::pair<node_ptr, std::string>
  {
    std::string prefix;
    auto suffix_size = key.size();
    auto curr = root_.get();

    int len, pos;

    for (len = 1, pos = 0; len <= suffix_size; len++)
    {
      prefix = key.substr(pos, len);
      if (curr->child_.count(prefix))
      {
        if (len == suffix_size)
        {
          return std::make_pair(get_child(curr, prefix), std::string{});
        }
        else
        {
          pos += len;
          suffix_size -= len;
          len = 0;
          curr = get_child(curr, prefix);
        }
      }
    }
    return std::make_pair(curr, std::string{key.substr(pos, len)});
  }

  size_t size() const
  {
    return size_;
  }

public:
  shared_node_ptr root_;
  size_t size_ = 0;

private:
  /**
   * @brief   Test if a node is a leaf / internal node
   * A node is a leaf if does not have children, otherwise is an internal node
   */
  auto static is_leaf(node_ptr node) -> bool
  {
    return node->child_.size() == 0;
  }
  auto static is_internal(node_ptr node) -> bool
  {
    return node->child_.size() != 0;
  }

  /**
   * @brief   Test if a node is root / non-root
   * A node is root if its parent_ is nullptr, and nonroot otherwise
   */
  auto static is_root(node_ptr node) -> bool
  {
    return node->parent_ == nullptr;
  }
  auto static is_nonroot(node_ptr node) -> bool
  {
    return node->parent_ != nullptr;
  }

  /**
   * @brief   Helper function for creating node node
   */

  auto newNode(node_ptr parent, const value_type &value) -> uniq_node_ptr
  {
    size_ += 1;
    return std::make_unique<TrieNode>(parent, value.second);
  }
  auto newNode(node_ptr parent) -> uniq_node_ptr
  {
    return std::make_unique<TrieNode>(parent);
  }

  /**
   * @brief   Find a TrieNode in Trie rooted at node 
   *          with key equivalent to key, If no exact match, 
   *          Return the last node with a common prefix
   */
  auto static find_in(node_ptr node, std::string &key) -> node_ptr
  {
    if (!key.size())
      return node;

    if (is_leaf(node))
      return node;

    for (int i = 1; i < key.size(); i++)
    {
      auto found = node->child_.find(key.substr(0, i));
      if (found != node->child_.end())
      {
        key = key.substr(i);
        return find_in((found->second).get(), key);
      }
    }
    return node;
  }

public:
  /**
   * @brief   Finds the child of given node with corresponding prefix
   */
  friend inline auto get_child(const node_ptr &node, std::string prefix) -> node_ptr
  {
    return node->child_.at(prefix).get();
  }
  friend inline auto get_child(const uniq_node_ptr &node, std::string prefix) -> node_ptr
  {
    return node.get()->child_.at(prefix).get();
  }
  friend inline auto get_child(const shared_node_ptr &node, std::string prefix) -> node_ptr
  {
    return node.get()->child_.at(prefix).get();
  }

  friend inline auto operator<<(std::ostream &strm, const Trie &t) -> std::ostream &
  {
    strm << "\\" << *t.root_ << std::endl;
    strm << "size: " << t.size_ << std::endl;
    return strm;
  }

  friend inline auto operator<<(std::ostream &strm, const TrieNode &node) -> std::ostream &
  {
    static size_t depth = 0;
    strm << "\t" << node.data_ << std::endl;
    if (node.child_.size())
    {
      depth += 2;
      for (auto &item : node.child_)
        strm << std::string(depth, ' ') << depth / 2 << "|-" << item.first
             << *item.second.get();
      depth -= 2;
    }
    return strm;
  }

  template <typename X, typename Y = uniq_node_ptr>
  friend inline auto operator<<(std::ostream &strm, const std::unordered_map<X, Y> &m) -> std::ostream &
  {
    for (auto &item : m)
      strm << "{ " << item.first << "\n\t" << *item.second.get() << " }" << std::endl;
    return strm;
  }
};

} // namespace Summer
#endif // __TRIE_H__