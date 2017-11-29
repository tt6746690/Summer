// #define NDEBUG
#include "json.hpp"
#include "theros.h"

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using namespace std;
// using namespace asio;
using namespace Theros;
using nlohmann::json;

template <typename ServerType = HttpServer>
void make_server(int port)
{
    ServerAddr addr = make_pair("127.0.0.1", port);
    auto app = ServerType(addr);
    auto &r = app.router_;

    r.get("/<user>/home", [](Context &ctx) {
        // url query parser
        ctx.req.uri_query = make_query(ctx.req.uri.query);

        JsonType urlparse = {
            {"query", ctx.query},
            {"param", ctx.param},
        };
        cout << setw(4) << urlparse << eol;
    });

    string msg =
      "app starts running on " + app.host() + ":" + to_string(app.port()) + eol;
    cout << msg;

    app.run();
}

int main(int argc, char *argv[]) {
    try {
    thread server_task(make_server<>, 8888);
    server_task.join();
    } catch (const exception e) {
    cerr << e.what() << eol;
    }

    return 0;
}
