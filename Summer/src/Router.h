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




// Aggregate types on request-response data structures
struct Context
{
    using MapType = std::unordered_map<std::string, std::string>;
    Request &req_;
    Response &res_;
    MapType &param_;
    MapType &query_;
    Context(Request &req, Response &res) : req_(req), res_(res), param_(req.param_), query_(req.query_){};
};


template <typename F>
constexpr bool is_handler_v = callable_with<F, Context&>() || callable_with<F>();
template <typename... Fs>
constexpr bool are_handlers_v = satisfies_all< is_handler_v<Fs>... >;

// A wrapper around a callable that consumes Context
static int handler_id_counter = 0;
class Handler 
{
public:
    using HandleFunc    = std::function<void(Context &)>;
    using ValueT        = std::vector<HandleFunc>;
protected:
    ValueT          handler_;
    int             handler_id_;
public:
    explicit Handler() {};
    template <typename... Fs>
    explicit Handler(Fs... fs) : handler_id_(++handler_id_counter) {
        static_assert(are_handlers_v<Fs...>, "Incorrect function arguments to Handler constructor");
        handler_ = { wrap(fs, callable_with<Fs, Context&>())...  };
    }

    template <typename F>
    inline HandleFunc wrap(F f, std::false_type) { return [f](Context& ctx){ f(); }; };
    template <typename F>
    inline HandleFunc wrap(F f, std::true_type) { return [f](Context& ctx){ f(ctx); }; };

public:
    // Invoke on context 
    void operator()(Context& ctx) { for(auto& f: handler_) { f(ctx);} }
    operator bool() const { return handler_.size() != 0; }
    friend inline bool operator< (const Handler& lhs, const Handler& rhs) { return lhs.handler_id_ < rhs.handler_id_; }
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
    template <typename... Fs> 
    void handle(RequestMethod method, const std::string& path, Fs&&... handlers);

    // Wrapper functions over handle
    template <typename... Fs> 
    void  get(const std::string& path, Fs&&... fs);
    template <typename... Fs>
    void post(const std::string& path, Fs&&... fs);
    template <typename... Fs> 
    void  put(const std::string& path, Fs&&... fs);
    template <typename... Fs>
    void  use(const std::string& path, Fs&&... fs);

    // Looks up path to yield a sequence of handlers, empty if no matching path found
    RouteType resolve(RequestMethod method, std::string& path);
    RouteType resolve(Request& request);

    friend std::ostream& operator<<(std::ostream& os, const Router& r);
public:
    RoutingTables routing_tables;
};


// impls 

template <typename... Fs> 
void Router::handle(RequestMethod method, const std::string& path, Fs&&... fs)
{
    auto &t = routing_tables[to_underlying_t(method)];
    auto h = Handler(std::forward<Fs>(fs)...);
    t.insert({path, h});
}

template <typename... Fs> 
void  Router::get(const std::string& path, Fs&&... fs) { handle(RequestMethod::GET, path, std::forward<Fs>(fs)...); }
template <typename... Fs>
void Router::post(const std::string& path, Fs&&... fs) { handle(RequestMethod::POST, path, std::forward<Fs>(fs)...); }
template <typename... Fs> 
void  Router::put(const std::string& path, Fs&&... fs) { handle(RequestMethod::PUT, path, std::forward<Fs>(fs)...); }
template <typename... Fs>
void  Router::use(const std::string& path, Fs&&... fs) 
{
    for(auto i = to_underlying_t(RequestMethod::GET); i != to_underlying_t(RequestMethod::UNDETERMINED); ++i)
        handle(static_cast<RequestMethod>(i), path, std::forward<Fs>(fs)...);
}


} // namespace Summer
#endif // __ROUTER_H__