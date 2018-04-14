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
using namespace Theros;
using nlohmann::json;

template <typename ServerType = HttpServer>
void make_server(int port)
{
    ServerAddr addr = make_pair("127.0.0.1", port);
    auto app = ServerType(addr);
    auto &r = app.router_;

    r.use("/", [](Context & ctx) {
        constexpr char tok_and = '&';
        constexpr char tok_equal = '=';

        std::string query = ctx.req.uri.query;
        query += tok_and;

        std::size_t pos = 0;
        std::string token, key, value;

        while ((pos = query.find(tok_and)) != std::string::npos) {
            token = query.substr(0, pos);
            std::tie(key, value) = split(token, tok_equal);
            ctx.req.uri_query.insert({key, value});
            query.erase(0, pos + 1);
        }
    });

    r.get("/<user>/home", [](Context &ctx) {
        JsonType urlparse = {
            {"query", ctx.query},
            {"param", ctx.param},
        };
        cout << setw(4) << urlparse << eol;
    });

    string msg = "app starts running on " + app.host() + ":" + to_string(app.port()) + eol;
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
