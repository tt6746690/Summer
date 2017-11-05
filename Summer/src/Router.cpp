
#include "Router.h"


void Router::handle(RequestMethod method, std::string& path, HandlerType handler)
{
    auto &t = routing_tables[to_underlying_t(method)];
    t.insert({path, handler});
}


template <typename RequestMethods = std::vector<RequestMethod>>
void Router::handle(RequestMethods methods, std::string& path, HandlerType handler) 
{
    for (const auto &method : methods) handle(method, path, handler);
}

void  get(std::string& path, HandlerType handler) { handle(RequestMethod::GET, path, handler); }
void post(std::string& path, HandlerType handler) { handle(RequestMethod::POST, path, handler); }
void  put(std::string& path, HandlerType handler) { handle(RequestMethod::PUT, path, handler); }
void  use(std::string& path, HandlerType handler) 
{
    for(auto i = to_underlying_t(RequestMethod::GET); i != to_underlying_t(RequestMethod::UNDETERMINED); ++i)
        handle(static_cast<RequestMethod>(i), path, handler);
}


RouteType Router::resolve(RequestMethod method, std::string& path)
{
    auto &t = routing_tables[to_underlying_t(method)];
    auto found = t.find(path);

    RouteType route;
    while(found != t.end()) {
        route.push_back(*found);
        --found;
    }

    std::reverse(route.begin(), route.end());
    return route;
}


RouteType Router::resolve(Request& request)
{
    auto method = request.method_;
    auto path = request.uri_.abs_path_;
    auto &table = routing_tables[to_underlying_t(method)];


}



// std::vector<T> resolve(Request &req)
// {
//   auto method = req.method_;
//   auto path = req.uri_.abs_path_;
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
