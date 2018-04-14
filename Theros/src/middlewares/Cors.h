#ifndef __CORS_H__
#define __CORS_H__

#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "../Constants.h"
#include "../Message.h"
#include "../Router.h"

namespace Theros
{

/**
 * @brief   Partial impl of Cors https://www.w3.org/TR/cors/
 *          Disregard credential
 * 
 *          Options may expand to node's cors https://www.npmjs.com/package/cors
 * 
 * 
 *          curl --http1.1 -v -X GET -H "Origin: localhost" '127.0.0.1:8888/r'
 *          curl --http1.1 -v -X OPTIONS -H " Access-Control-Request-Method: GET" '127.0.0.1:8888/r'
 *          curl --http1.1 -v -X OPTIONS -H "Origin: localhost" -H "Access-Control-Request-Method: GET" '127.0.0.1:8888/'
 */
class Cors : public Handler
{
  private:
    std::vector<std::string> origins_;
    std::vector<RequestMethod> methods_;
    int max_age_;
  public:
    Cors(const std::vector<std::string>& origins,
         const std::vector<RequestMethod>& methods,
         const int max_age = 51840000)
        : Handler(), origins_(origins), methods_(methods), max_age_(max_age)
    { handle_cors(); };

  private:
    void handle_cors()
    {
       auto cors_handler = [  origins_ = origins_,
                              methods_ = methods_,
                              max_age_ = max_age_ ](Context & ctx)
           {

           std::string origin = ctx.req.FindHeader("Origin");
           if(origin == "") return;
           
           if(ctx.req.method != RequestMethod::OPTIONS)
           {
                // simple CORS request,
               if (std::find(origins_.begin(), origins_.end(), "*") != origins_.end()) {
                   ctx.res.SetHeader({"Access-Control-Allow-Origin", "*"});
                   return;
               }
               if (std::find(origins_.begin(), origins_.end(), origin) != origins_.end())
                   ctx.res.SetHeader({"Access-Control-Allow-Origin", origin});
           } else {
               // preflight request
               std::string method = ctx.req.FindHeader("Access-Control-Request-Method");
               if (origin == "") return;
               std::string header = ctx.req.FindHeader("Access-Control-Request-Method");
               if (header == "") return;

               if (std::find(methods_.begin(), methods_.end(), request_method_from_cstr(method.c_str())) != methods_.end()) {
                   std::string methods;
                   std::for_each(methods_.begin(), methods_.end() - 1, [&methods](auto m) {
                       methods += request_method_as_string(m) + ", ";
                   });
                   methods += std::string(request_method_as_string(methods_.back()));
                   ctx.res.SetHeader({"Access-Control-Allow-Methods", methods});
               } else {
                   return;
               }

               // not handling allowed headers, simply propagate to allowed
               std::string headers;
               std::for_each(ctx.req.headers.begin(), ctx.req.headers.end(), [&headers](auto p) {
                   headers += p.name + ", ";
               });
               if (!headers.empty())
                   headers.erase(headers.end() - 2, headers.end());
               ctx.res.SetHeader({"Access-Control-Allow-Headers", headers});
               ctx.res.SetHeader({"Access-Control-Max-Age", std::to_string(max_age_)});
           }
       };

       append(cors_handler);
    }
};

} // namespace Theros
#endif // __CORS_H__