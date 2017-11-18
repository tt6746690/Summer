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

