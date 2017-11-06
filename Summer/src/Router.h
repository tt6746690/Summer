#ifndef __ROUTER_H__
#define __ROUTER_H__


#include <functional>
#include <iosfwd>
#include <string>
#include <vector>


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
    friend inline bool operator< (const Handler& lhs, const Handler& rhs) { return lhs.handler_id_ < rhs.handler_id_; }
    friend inline bool operator<=(const Handler &rhs, const Handler &lhs) { return !(lhs < rhs); }
    friend inline bool operator> (const Handler &rhs, const Handler &lhs) { return  (lhs < rhs); }
    friend inline bool operator>=(const Handler &rhs, const Handler &lhs) { return !(rhs < lhs); }
    friend inline bool operator==(const Handler &rhs, const Handler &lhs) { return !(rhs < lhs) && !(lhs < rhs); }
    friend inline bool operator!=(const Handler &rhs, const Handler &lhs) { return  (rhs < lhs) ||  (lhs < rhs); }
    friend std::ostream &operator<<(std::ostream &os, const Handler &handler);
};



class Router 
{
public:
    using HandlerType       = Handler;
    using RouteType         = std::vector<HandlerType>;
    using RoutingTable      = Trie<HandlerType>;
    using RoutingTables     = std::vector<RoutingTable>;
public:
    explicit Router() : routing_tables(method_count) {}

    // Register handler for provided (path, handler_callable)
    void handle(RequestMethod method, const std::string& path, HandlerType handler);
    template <typename RequestMethods = std::vector<RequestMethod>>
    void handle(RequestMethods methods, const std::string& path, const HandlerType& handler);

    // Wrapper functions over handle
    void  get(const std::string& path, const HandlerType& handler);
    void post(const std::string& path, const HandlerType& handler);
    void  put(const std::string& path, const HandlerType& handler);
    void  use(const std::string& path, const HandlerType& handler);

    // Looks up path to yield a sequence of handlers, empty if no matching path found
    RouteType resolve(RequestMethod method, std::string& path);
    RouteType resolve(Request& request);

    friend std::ostream& operator<<(std::ostream& os, const Router& r);
public:
    RoutingTables routing_tables;
};


} // namespace Summer
#endif // __ROUTER_H__