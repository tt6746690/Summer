#include "json.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <thread>


// #define NDEBUG
#include "summer.h"


#include <cstdio>
#include <cstdlib> 

using namespace asio;
using namespace Summer;
using nlohmann::json;

template<typename ServerType>
void start_server(int port){
    ServerAddr server_address =
        std::make_pair("127.0.0.2", port);
    auto app = std::make_unique<ServerType>(server_address);

    app->router_.handle(RequestMethod::GET, "/r", [](Context &ctx) {
                       // url query parser
                       ctx.req_.query_ = Uri::make_query(ctx.req_.uri_.query_);

                       JsonType urlparse = {
                           {"query", ctx.query_}, {"param", ctx.param_},
                       };
                       std::cout << std::setw(4) << urlparse << std::endl;
                     });

    std::cout << "app starts running on " << 
      app->host() << std::to_string(app->port()) << std::endl;
    std::cout << app->router_ << std::endl;
                     
    app->run();
}


int main(int argc, char**argv) {

  try {

    std::thread httpserver_task(start_server<HttpServer>, 8888);
//    std::thread httpsserver_task(start_server<HttpsServer>, 8889);

    httpserver_task.join();
//    httpsserver_task.join();

  } catch (const std::exception e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
