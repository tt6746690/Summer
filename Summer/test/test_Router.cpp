#include "catch.hpp"
#include <utility>
#include <stdexcept>

#include "Trie.h"
#include "Router.h"
#include "Constants.h"

using namespace Http;

TEST_CASE("handle, resolves", "[Router]")
{

    Response res;
    Request req;
    Context ctx(req, res);

    Router<Handler> r{};
    REQUIRE(r.routes_.size() == method_count);
    REQUIRE(r.routes_[0].size() == 0);
    REQUIRE(!r.routes_.front().root_->data_);

    SECTION("handle")
    {

        r.handle(RequestMethod::GET, "/home", Handler([](Context &ctx) {
                     std::cout << "Handler: GET/home" << std::endl;
                 }));

        auto home_foo_handle = Handler([](Context &ctx) {
            std::cout << "Handler: GET/home/foo" << std::endl;
        });
        r.handle(RequestMethod::GET, "/home/foo", home_foo_handle);

        auto home_bar_handle = Handler([](Context &ctx) {
            std::cout << "Handler: GET/home/bar" << std::endl;
        });
        r.handle(RequestMethod::GET, "/home/bar", home_bar_handle);

        SECTION("Simple handle")
        {
            REQUIRE(r.routes_[0].size() == 3);
            REQUIRE(home_foo_handle.handler_id_ == 2);
            REQUIRE(home_bar_handle.handler_id_ == 3);
        }

        SECTION("handles multiple methods")
        {
            r.handle({RequestMethod::GET, RequestMethod::POST}, "/in_both_methods", home_bar_handle);

            auto in_both_methods_get = r.resolve(RequestMethod::GET, "/in_both_methods");
            auto in_both_methods_post = r.resolve(RequestMethod::POST, "/in_both_methods");
            REQUIRE(!in_both_methods_get.empty());
            REQUIRE(!in_both_methods_post.empty());
        }

        SECTION("resolve handles")
        {
            auto handles = r.resolve(RequestMethod::GET, "/home");
            REQUIRE(handles.size() == 1);

            auto foo_handles = r.resolve(RequestMethod::GET, "/home/foo");
            auto bar_handles = r.resolve(RequestMethod::GET, "/home/bar");
            REQUIRE(foo_handles.size() == 2);
            REQUIRE(bar_handles.size() == 2);
            REQUIRE(foo_handles.front() == bar_handles.front());
            REQUIRE(foo_handles.back() != bar_handles.back());

            handles = r.resolve(RequestMethod::POST, "/home/bar");
            REQUIRE(handles.size() == 0);
        }

        SECTION("resolve url with parameter")
        {
            r.handle(RequestMethod::GET, "/home/<id>", Handler([](Context &ctx) {
                         std::cout << "Handler: GET /reads/<id>" << std::endl;
                     }));

            // std::cout << r << std::endl;

            req.uri_.abs_path_ = "/home/102938";
            req.method_ = RequestMethod::GET;

            auto handles = r.resolve(req);
            REQUIRE(handles.size() == 2);
            REQUIRE_NOTHROW(req.param_.find("id"));
            REQUIRE(req.param_["id"] == "102938");
        }
    }
}