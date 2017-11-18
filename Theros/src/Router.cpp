#include <algorithm>    // reverse

#include "Utils.h"
#include "Router.h"

namespace Theros {


int Handler::handler_id_counter = 0;

std::ostream& operator<<(std::ostream& os, const Handler& handler)
{
    return os << "(" << handler.handler_id_ << ")";
}


auto Router::resolve(RequestMethod method, const std::string& path) -> RouteType
{
    RoutingTable& t = routing_tables[to_underlying_t(method)];
    auto found = t.find(path);

    RouteType route;

    while(found != t.end()) {
        route.push_back(*found);
        --found;
    }

    std::reverse(route.begin(), route.end());
    return route;
}


auto Router::resolve(const Request& request) -> RouteType
{
    auto method = request.method;
    auto path = request.uri.abs_path;
    return resolve(method, path);
}


auto Router::resolve(RequestMethod method, 
                     const std::string& path,
                     std::vector<std::pair<std::string, std::string>>& kvs) -> RouteType
{
    RoutingTable& t = routing_tables[to_underlying_t(method)];
    auto found = t.find(path, kvs);

    RouteType route;
    while(found != t.end()) {
        route.push_back(*found);
        --found;
    }
    std::reverse(route.begin(), route.end());
    return route;
}

auto Router::resolve(const Request& request,
                     std::vector<std::pair<std::string, std::string>>& kvs) -> RouteType
{
    auto method = request.method;
    auto path = request.uri.abs_path;
    return resolve(method, path, kvs);
}


auto Router::table(RequestMethod method) -> RoutingTable&
{
    return routing_tables[to_underlying_t(method)];
}


std::ostream& operator<<(std::ostream& os, const Router& r)
{
    for(int i = 0; i < method_count; ++i) {
        auto& table =  r.routing_tables[i];
        if(table.size())
            os << Request::request_method_to_string(static_cast<RequestMethod>(i)) << eol << table << eol;
    }
    return os;
}



} // namespace Theros

// std::vector<T> resolve(Request &req)
// {
//   auto method = req.method_;
//   auto path = req.uri_.abs_path;
//   auto &route = routes_[to_underlying_t(method)];

//   std::string param_key, param_value;
//   auto found = route.find(path, param_key, param_value);

//   if (found == route.end())
//     return {};

//   if (!param_key.empty() && !param_value.empty())
//     req.param_.insert({param_key, param_value});

//   std::vector<T> handle_sequence;

//   while (found != route.end())
//   {
//     if (*found)
//       handle_sequence.push_back(*found);
//     --found;
//   }

//   std::reverse(handle_sequence.begin(), handle_sequence.end());
//   return handle_sequence;
// }
