#include <algorithm>    // reverse

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
            os << request_method_as_string(static_cast<RequestMethod>(i)) << eol << table << eol;
    }
    return os;
}



} // namespace Theros
