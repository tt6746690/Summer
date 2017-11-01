#ifndef ROUTER_H
#define ROUTER_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "Constants.h"
#include "Request.h"
#include "Response.h"
#include "Trie.h"

namespace Summer {

struct Context {
  Request &req_;
  Response &res_;
  ssmap &param_;
  ssmap &query_;

  Context(Request &req, Response &res)
      : req_(req), res_(res), param_(req.param_), query_(req.query_){};
};

/**
 * @brief   Defines rules for giving handler unique ids
 *          template arguments to Handler during instantiation
 */
class HandlerCounter {
private:
  int handler_count_;

public:
  HandlerCounter(int start_from = 1) : handler_count_(start_from){};
  int operator()() { return handler_count_++; }
};

// global counter as default Counter to Handler_
static HandlerCounter global_handler_counter = HandlerCounter();

/**
 * @brief   A Wrapper around a Callable,
 */
template <typename Counter = HandlerCounter> 
class Handler_ {
public:
  using HandlerFunc = std::function<void(Context &)>;
  explicit Handler_() : handler_(nullptr), handler_id_(0){};
  explicit Handler_(HandlerFunc handler, Counter& count_up = global_handler_counter)
      : handler_(handler), handler_id_(count_up()){};

  operator bool() const { return handler_ != nullptr; }
  void operator()(Context &ctx) { handler_(ctx); }
  bool operator< (const Handler_<> &rhs) { return handler_id_ < rhs.handler_id_; }
  bool operator<=(const Handler_<> &rhs) { return !(rhs.handler_id_ < handler_id_); }
  bool operator==(const Handler_<> &rhs) { return !(rhs.handler_id_ < handler_id_) && !(handler_id_ < rhs.handler_id_); }  
  bool operator!=(const Handler_<> &rhs) { return (rhs.handler_id_ < handler_id_) || (handler_id_ < rhs.handler_id_); }  
  bool operator> (const Handler_<> &rhs) { return rhs.handler_id_ < handler_id_; }
  bool operator>=(const Handler_<> &rhs) { return !(handler_id_ < rhs.handler_id_); }

  void handle(HandlerFunc handle, Counter& count_up = global_handler_counter) {
    handler_ = handle;
    if (!handler_id_) handler_id_ = count_up();
  }

public:
  HandlerFunc handler_;
  int handler_id_;

  friend inline std::ostream & operator<<(std::ostream &strm, Handler_<> &handler) {
    return strm << "(" << handler.handler_id_ << ")";
  }
};

// use default template specialization
using Handler = Handler_<>;

/**
 * @brief   A class for constructing routes at compile-time, and
 *          resolving request to a sequence of handlers at run-time
 */
template <typename T> 
class Router {
public:
  explicit Router() : routes_(method_count){};
  /**
   * @brief   Registers handler for provided method + path
   *
   *  Rules
   *      -- specify named url parameter in angle brackets
   *          -- /books/<id>
   *
   * @precond path starts with /
   */
  void handle(RequestMethod method, std::string path, T handler) {
    assert(path.front() == '/');
    auto &route = routes_[etoint(method)];
    route.insert({path, handler});
  }

  template <typename Container = std::vector<RequestMethod>>
  void handle(Container methods, std::string path, T handler) {
    for (const auto &method : methods)
      handle(method, path, handler);
  }

  /**
   * @brief   Handle wrapper functions
   */
  void get(std::string path, T handler = T()) {
    handle(RequestMethod::GET, path, handler);
  }
  void post(std::string path, T handler = T()) {
    handle(RequestMethod::POST, path, handler);
  }
  void put(std::string path, T handler = T()) {
    handle(RequestMethod::PUT, path, handler);
  }
  void use(std::string path, T handler = T()) {
    for (int method = static_cast<int>(RequestMethod::GET);
         method != static_cast<int>(RequestMethod::UNDETERMINED); ++method) {
      handle(static_cast<RequestMethod>(method), path, handler);
    }
  }
  /**
   * @brief   Resolve path to a sequence of handler calls
   *          If no matching path is found, the sequence is empty
   */
  std::vector<T> resolve(RequestMethod method, std::string path) {
    auto &route = routes_[etoint(method)];
    auto found = route.find(path);
    if (found == route.end())
      return {};
    std::vector<T> handle_sequence;

    while (found != route.end()) {
      if (*found)
        handle_sequence.push_back(*found);
      --found;
    }

    std::reverse(handle_sequence.begin(), handle_sequence.end());
    return handle_sequence;
  }

  std::vector<T> resolve(Request &req) {
    auto method = req.method_;
    auto path = req.uri_.abs_path_;
    auto &route = routes_[etoint(method)];

    std::string param_key, param_value;
    auto found = route.find(path, param_key, param_value);

    if (found == route.end())
      return {};

    if (!param_key.empty() && !param_value.empty())
      req.param_.insert({param_key, param_value});

    std::vector<T> handle_sequence;

    while (found != route.end()) {
      if (*found)
        handle_sequence.push_back(*found);
      --found;
    }

    std::reverse(handle_sequence.begin(), handle_sequence.end());
    return handle_sequence;
  }

public:
  std::vector<Trie<T>> routes_;

public:
  friend std::ostream & operator<<(std::ostream &strm, Router r) {
    for (int i = 0; i < method_count; ++i) {
      auto &trie = r.routes_[i];
      if (trie.size_ != 0) {
        strm << Request::request_method_to_string(static_cast<RequestMethod>(i))
             << std::endl;
        strm << trie << std::endl;
      }
    }
    return strm;
  }
};
}

#endif