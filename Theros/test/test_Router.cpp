#include "catch.hpp"
#include <utility>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "Constants.h"
#include "Trie.h"
#include "Router.h"

using namespace std;
using namespace Theros;

#define GET RequestMethod::GET
#define POST RequestMethod::POST

TEST_CASE("Router") 
{

    Response res;
    Request req; 
    Context ctx(req, res);

    Router r; 

    SECTION("initialization")
    {
        REQUIRE(r.routing_tables.size() == method_count);
        for(auto& rt : r.routing_tables) {
            REQUIRE(rt.size() == 0);
        }
    }


    SECTION("handle") 
    {
        r.handle(GET, "/home",
                 [](){ cout << "handler 1, lambda 1" << endl; },
                 [](){ cout << "handler 1, lambda 2" << endl; });
        r.get("/home/index.html",
              [](){ cout << "handler 2, lambda 1" << endl; });
        r.get("/hello",
              [](){ cout << "handler 3, lambda 1" << endl; });

        REQUIRE(r.table(GET).size() == 3);

        SECTION("resolve")
        {
            auto test_resolve = [&r](RequestMethod method, const string& path, vector<int> expected_ids)
            {
                auto handles = r.resolve(method, path);

                vector<int> handles_ids;
                transform(handles.begin(), handles.end(), back_inserter(handles_ids),
                    [](auto h){ return h.id(); });
                REQUIRE(handles.size() == expected_ids.size());
                REQUIRE(handles_ids == expected_ids);
            };

            // good paths
            test_resolve(GET, "/home", {1});
            test_resolve(GET, "/home/index.html", {1, 2});
            test_resolve(GET, "/hello", {3});

            // bad paths
            test_resolve(GET, "", {});
            test_resolve(GET, "/", {});
            test_resolve(GET, "/h", {});
            test_resolve(GET, "/home/", {});
        }
    }



}



// TEST_CASE("handle, resolves", "[Router]")
// {

//     Response res;
//     Request req;
//     Context ctx(req, res);

//     Router<Handler> r{};
//     REQUIRE(r.routes_.size() == method_count);
//     REQUIRE(r.routes_[0].size() == 0);
//     REQUIRE(!r.routes_.front().root_->data_);

//     SECTION("handle")
//     {


//         SECTION("handles multiple methods")
//         {
//             r.handle({RequestMethod::GET, RequestMethod::POST}, "/in_both_methods", home_bar_handle);

//             auto in_both_methods_get = r.resolve(RequestMethod::GET, "/in_both_methods");
//             auto in_both_methods_post = r.resolve(RequestMethod::POST, "/in_both_methods");
//             REQUIRE(!in_both_methods_get.empty());
//             REQUIRE(!in_both_methods_post.empty());
//         }

//         SECTION("resolve handles")
//         {
//             auto handles = r.resolve(RequestMethod::GET, "/home");
//             REQUIRE(handles.size() == 1);

//             auto foo_handles = r.resolve(RequestMethod::GET, "/home/foo");
//             auto bar_handles = r.resolve(RequestMethod::GET, "/home/bar");
//             REQUIRE(foo_handles.size() == 2);
//             REQUIRE(bar_handles.size() == 2);
//             REQUIRE(foo_handles.front() == bar_handles.front());
//             REQUIRE(foo_handles.back() != bar_handles.back());

//             handles = r.resolve(RequestMethod::POST, "/home/bar");
//             REQUIRE(handles.size() == 0);
//         }

//         SECTION("resolve url with parameter")
//         {
//             r.handle(RequestMethod::GET, "/home/<id>", Handler([](Context &ctx) {
//                          std::cout << "Handler: GET /reads/<id>" << std::endl;
//                      }));

//             // std::cout << r << std::endl;

//             req.uri_.abs_path_ = "/home/102938";
//             req.method_ = RequestMethod::GET;

//             auto handles = r.resolve(req);
//             REQUIRE(handles.size() == 2);
//             REQUIRE_NOTHROW(req.param_.find("id"));
//             REQUIRE(req.param_["id"] == "102938");
//         }
//     }
// }