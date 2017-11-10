#ifndef __ROUTER_H__
#define __ROUTER_H__

#include <functional>
#include <iosfwd>
#include <string>
#include <vector>

#include "Traits.h"
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



template <typename F>
using IsHandlerType = std::enable_if_t<callable_with<F, Context>() || callable_with<F>()>;

template <typename F>
using IsHandlerWithNoArgs = std::enable_if_t<!callable_with<F, Context>() || callable_with<F>()>;

template <typename F>
using IsHandlerWithContext = std::enable_if_t<callable_with<F(Context)>::value>;  // most vexing parse !



// A wrapper around a callable that consumes Context
static int handler_id_counter = 0;
class Handler 
{
public:
    using HandlerType = std::function<void(Context &)>;
    explicit Handler(HandlerType f) : handler_(f), handler_id_(handler_id_counter++) {}


//    template <typename F, typename = IsHandlerWithNoArgs<F>>
//    explicit Handler(F f) : handler_id_(handler_id_counter++) { handler_ = [](const Context&) { f(); }; }
//
//
//    template <typename... Fs>
//    explicit Handler(Fs... fs)
//

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

    template <typename... Fs> 
    void handle2(RequestMethod method, const std::string& path, Fs... handlers);
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



template <typename... Fs> 
void Router::handle2(RequestMethod method, const std::string& path, Fs... handlers)
{
    auto &t = routing_tables[to_underlying_t(method)];
    std::array<HandlerType, sizeof...(Fs)> Handlers = {handlers...};

    for(const auto& h : Handlers)
    {
        t.insert({path, h});
    }
}




} // namespace Summer
#endif // __ROUTER_H__