#include "catch.hpp"
#include "asio.hpp"
#include <utility>
#include <stdexcept>

#include "Constants.h"
#include "Server.h"

using namespace std;
using namespace asio;
using namespace Http;

TEST_CASE("Construct routes", "[Server]")
{

    io_service io;
    ServerAddr server_address = make_pair("127.0.0.1", 8888);
    auto app = make_unique<HttpServer>(server_address);

    SECTION("serverroute")
    {
        app->router_.get("/",
                         Handler([](Context &ctx) {
                             cout << "GET /" << endl;
                         }));

        app->router_.get("/home",
                         Handler([](Context &ctx) {
                             cout << "GET /home" << endl;
                         }));

        app->router_.get("/home/level2",
                         Handler([](Context &ctx) {
                             cout << "GET /home" << endl;
                         }));

        app->router_.get("/home/level2/level3",
                         Handler([](Context &ctx) {
                             cout << "GET /home/level2/level3" << endl;
                         }));

        app->router_.get("/account/bar/baz",
                         Handler([](Context &ctx) {
                             cout << "GET /acocunt/bar" << endl;
                         }));

        app->router_.get("/account/bar",
                         Handler([](Context &ctx) {
                             cout << "GET /account/bar" << endl;
                         }));

        app->router_.get("/account/foo",
                         Handler([](Context &ctx) {
                             cout << "GET /account/foo" << endl;
                         }));

        app->router_.post("/account/bar",
                          Handler([](Context &ctx) {
                              cout << "POST /account/bar" << endl;
                          }));

        // std::cout << app->router_ << std::endl;

        // app->run();
    }
}