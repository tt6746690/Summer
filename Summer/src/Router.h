#ifndef __ROUTER_H__
#define __ROUTER_H__

#include <algorithm>
#include <functional>
#include <iostream>
#include <utility>
#include <string>
#include <vector>

#include "Constants.h"
#include "Request.h"
#include "Response.h"
#include "Trie2.h"

namespace Summer {

struct Context
{
  Request &req_;
  Response &res_;
  ssumap &param_;
  ssumap &query_;

  Context(Request &req, Response &res) : req_(req), res_(res), param_(req.param_), query_(req.query_){};
};


// A wrapper around a callable that consumes Context
static int handler_id_counter = 0;
class Handler 
{
public:
    using HandlerType = std::function<void(Context &)>;

    explicit Handler() : handler_(nullptr), handler_id_(handler_id_counter++) { } 
    explicit Handler(HandlerType h) : handler_(h), handler_id_(handler_id_counter++) { }
protected:
    HandlerType     handler_;
    int             handler_id_;
public:
    // Invoke on context 
    void operator()(Context& ctx) { handler_(ctx); }
    operator bool() const { return handler_ != nullptr; }
    friend inline bool operator< (const Handler& lhs, const Handler& rhs) { return lhs.handler_id_ != rhs.handler_id_; }
    friend inline bool operator<=(const Handler &rhs, const Handler &lhs) { return !(lhs < rhs); }
    friend inline bool operator> (const Handler &rhs, const Handler &lhs) { return  (lhs < rhs); }
    friend inline bool operator>=(const Handler &rhs, const Handler &lhs) { return !(rhs < lhs); }
    friend inline bool operator==(const Handler &rhs, const Handler &lhs) { return !(rhs < lhs) && !(lhs < rhs); }
    friend inline bool operator!=(const Handler &rhs, const Handler &lhs) { return  (rhs < lhs) ||  (lhs < rhs); }
    friend inline std::ostream &operator<<(std::ostream &os, Handler &handler) { return os << "(" << handler.handler_id_ << ")"; }
};



class Router 
{
public:
    using HandlerType       = Handler;
    using RouteType         = std::vector<HandlerType>;
    using RoutingTable      = Trie<HandlerType>;
    using RoutingTables     = std::vector<RoutingTable>;
public:
    explicit Router() : routes(method_count) {}

    // Register handler for provided (path, handler_callable)
    void handle(RequestMethod method, std::string& path, HandlerType handler);
    template <typename RequestMethods = std::vector<RequestMethod>>
    void handle(RequestMethods methods, std::string& path, HandlerType handler);

    // Wrapper functions over handle
    void  get(std::string& path, HandlerType handler);
    void post(std::string& path, HandlerType handler);
    void  put(std::string& path, HandlerType handler);
    void  use(std::string& path, HandlerType handler);

    // Looks up path to yield a sequence of handlers, empty if no matching path found
    RouteType resolve(RequestMethod method, std::string& path);
    RouteType resolve(Request& request);

private:
    RoutingTables routing_tables;
};

// // A Router for construction and lookup of routing paths
// template <typename T>
// class Router
// {
  
// public:
//   explicit Router() : routes_(method_count){};
//   /**
//    * @brief   Registers handler for provided method + path
//    *
//    *  Rules
//    *      -- specify named url parameter in angle brackets
//    *          -- /books/<id>
//    *
//    * @precond path starts with /
//    */
//   void handle(RequestMethod method, std::string path, T handler)
//   {
//     assert(path.front() == '/');
//     auto &route = routes_[to_underlying_t(method)];
//     route.insert({path, handler});
//   }

//   template <typename Container = std::vector<RequestMethod>>
//   void handle(Container methods, std::string path, T handler)
//   {
//     for (const auto &method : methods)
//       handle(method, path, handler);
//   }

//   /**
//    * @brief   Handle wrapper functions
//    */
//   void get(std::string path, T handler = T())
//   {
//     handle(RequestMethod::GET, path, handler);
//   }
//   void post(std::string path, T handler = T())
//   {
//     handle(RequestMethod::POST, path, handler);
//   }
//   void put(std::string path, T handler = T())
//   {
//     handle(RequestMethod::PUT, path, handler);
//   }
//   void use(std::string path, T handler = T())
//   {
//     for (int method = static_cast<int>(RequestMethod::GET);
//          method != static_cast<int>(RequestMethod::UNDETERMINED); ++method)
//     {
//       handle(static_cast<RequestMethod>(method), path, handler);
//     }
//   }
//   /**
//    * @brief   Resolve path to a sequence of handler calls
//    *          If no matching path is found, the sequence is empty
//    */
//   std::vector<T> resolve(RequestMethod method, std::string path)
//   {
//     auto &route = routes_[to_underlying_t(method)];
//     auto found = route.find(path);
//     if (found == route.end())
//       return {};
//     std::vector<T> handle_sequence;

//     while (found != route.end())
//     {
//       if (*found)
//         handle_sequence.push_back(*found);
//       --found;
//     }

//     std::reverse(handle_sequence.begin(), handle_sequence.end());
//     return handle_sequence;
//   }

//   std::vector<T> resolve(Request &req)
//   {
//     auto method = req.method_;
//     auto path = req.uri_.abs_path_;
//     auto &route = routes_[to_underlying_t(method)];

//     std::string param_key, param_value;
//     auto found = route.find(path, param_key, param_value);

//     if (found == route.end())
//       return {};

//     if (!param_key.empty() && !param_value.empty())
//       req.param_.insert({param_key, param_value});

//     std::vector<T> handle_sequence;

//     while (found != route.end())
//     {
//       if (*found)
//         handle_sequence.push_back(*found);
//       --found;
//     }

//     std::reverse(handle_sequence.begin(), handle_sequence.end());
//     return handle_sequence;
//   }

// public:
//   std::vector<Trie<T>> routing_tables;

// public:
//   friend std::ostream &operator<<(std::ostream &strm, Router r)
//   {
//     for (int i = 0; i < method_count; ++i)
//     {
//       auto &trie = r.routes_[i];
//       if (trie.size_ != 0)
//       {
//         strm << Request::request_method_to_string(static_cast<RequestMethod>(i))
//              << std::endl;
//         strm << trie << std::endl;
//       }
//     }
//     return strm;
//   }
// };

} // namespace Summer
#endif // __ROUTER_H__