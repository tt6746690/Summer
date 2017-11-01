#ifndef CORS_H
#define CORS_H

#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "Constants.h"
#include "Response.h"
#include "Router.h"


namespace Http {

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
class Cors : public Handler {

public:
  Cors(const std::vector<std::string> origins, 
        const std::vector<RequestMethod> methods,
        const int max_age = 51840000) 
        : Handler(), 
        origins_(origins),
        methods_(methods),
        max_age_(max_age) {
            handle_cors(); 
        };

private:
  void handle_cors() {
    handler_ = [origins_ = origins_, 
                methods_ = methods_, 
                max_age_ = max_age_](Context &ctx) {

        Response::HeaderValueType origin;
        bool valid;
        std::tie(origin, valid) = ctx.req_.get_header("Origin");

        if(!valid)
            return;
        
        if(ctx.req_.method_ != RequestMethod::OPTIONS){
            // simple CORS request,
            if(std::find(origins_.begin(), origins_.end(), "*") != origins_.end()){
                ctx.res_.set_header({"Access-Control-Allow-Origin", "*"});
                return;
            } 
            if(std::find(origins_.begin(), origins_.end(), origin) != origins_.end())
                ctx.res_.set_header({"Access-Control-Allow-Origin", origin});
        } else {
            // preflight request
            Response::HeaderValueType method;
            std::tie(method, valid) = ctx.req_.get_header("Access-Control-Request-Method");

            if(!valid)
                return;

            Response::HeaderValueType header;
            std::tie(method, valid) = ctx.req_.get_header("Access-Control-Request-Method");
            

            if(!valid)
                header = "";

            if(std::find(methods_.begin(), methods_.end(), 
                Request::string_to_request_method(method)) != methods_.end()){
                std::string methods;
                std::for_each(methods_.begin(), methods_.end() - 1, [&methods](auto m){
                    methods += std::string(Request::request_method_to_string(m)) + ", ";
                });
                methods += std::string(Request::request_method_to_string(methods_.back()));
                ctx.res_.set_header({"Access-Control-Allow-Methods", methods});
            } else {
                return;
            }
            
            // not handling allowed headers, simply propagate to allowed
            std::string headers;
            std::for_each(ctx.req_.headers_.begin(), ctx.req_.headers_.end(), [&headers](auto p){
                headers += std::get<0>(p) + ", ";
            });
            if(!headers.empty())
                headers.erase(headers.end() - 2, headers.end());
            ctx.res_.set_header({"Access-Control-Allow-Headers", headers});
            ctx.res_.set_header({"Access-Control-Max-Age", std::to_string(max_age_)});
        }
    };
  }

private:
    std::vector<std::string> origins_;
    std::vector<RequestMethod> methods_;
    int max_age_;
};


}
#endif