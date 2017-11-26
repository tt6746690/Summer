#include "catch.hpp"
#include <iostream>

#include "Utils.h"
#include "Constants.h"
#include "Trie.h"
#include "Router.h"

using namespace std;
using namespace Theros;

#define GET RequestMethod::GET
#define POST RequestMethod::POST


template<typename T>
auto require_same = [](const vector<T>& x, const vector<T>& y) {
    REQUIRE(x.size() == y.size());
    for(int i = 0; i < x.size(); ++i) {
        REQUIRE(x[i] == y[i]);
    }
};


TEST_CASE("Router") 
{

    SECTION("initialization")
    {
        Router r; 
        REQUIRE(r.routing_tables.size() == method_count);
        for(auto& rt : r.routing_tables) {
            REQUIRE(rt.size() == 0);
        }
    }


    SECTION("handle") 
    {
        Router r; 
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


    SECTION("path match to route path yield query")
    {
        Handler::handler_id_counter = 0;
        Router r; 

        r.get("/", [](){ cout << "handler 1" << endl; });
        r.get("/home",
            [](){ cout << "handler 2" << endl; });
        r.get("/user/<id>/info",
            [](){ cout << "handler 3" << endl; });
        r.get("/user/<id>/ideas",
            [](){ cout << "handler 4" << endl; });


        auto test_resolve = [&r](RequestMethod method, 
                                 const string& path, 
                                 vector<int> expected_ids, 
                                 vector<pair<string, string>> expected_kvs)
        {
            auto req = Request();
            req.method = method;
            req.uri.abs_path = path;

            vector<pair<string, string>> kvs;
            auto handles = r.resolve(req, kvs);

            vector<int> handles_ids;
            transform(handles.begin(), handles.end(), back_inserter(handles_ids),
                [] (auto h){ return h.id(); });
            REQUIRE(handles.size() == expected_ids.size());
            REQUIRE(handles_ids == expected_ids);

//            cout << "expected " << expected_kvs << endl;
//            cout << "result   " << kvs << endl;

            REQUIRE(kvs == expected_kvs);
        };
        
        // good paths
        vector<pair<string, string>> kvs;
        test_resolve(GET, "/", {1}, kvs);
        test_resolve(GET, "/home", {1, 2}, kvs);
        test_resolve(GET, "/user/foo/info", {1, 3}, {{"id", "foo"}});
    }

}

