#ifndef CONNECTION_H
#define CONNECTION_H

#include "asio.hpp"
#include "asio/ssl.hpp"
#include "asio/basic_waitable_timer.hpp"


#include <type_traits>
#include <chrono>
#include <utility>
#include <iostream>

#include "Request.h"
#include "RequestParser.h"
#include "Response.h"
#include "Router.h"

namespace Http {

using TcpSocket = asio::ip::tcp::socket;
using SslSocket = asio::ssl::stream<asio::ip::tcp::socket>;
using ClockType = std::chrono::steady_clock;


template <typename SocketType>
class Connection : public std::enable_shared_from_this<Connection<SocketType>> {

public:
  using DeadlineTimer = asio::basic_waitable_timer<ClockType>;
  static constexpr auto max_time = ClockType::duration::max();
  static constexpr auto read_timeout = std::chrono::seconds(2);

public:
  explicit Connection(asio::io_service& io_service, Router<Handler> &router)
      : socket_(io_service),
        read_deadline_(io_service),
        router_(router){
          read_deadline_.expires_from_now(max_time);
        };

  explicit Connection(
    asio::io_service& io_service, asio::ssl::context& context, Router<Handler> &router)
      : socket_(io_service, context),
        read_deadline_(io_service),
        router_(router){
          read_deadline_.expires_from_now(max_time);
        };


public:
  /**
   * @brief   Starts reading asynchronously
   *          For TLS, do handshake first
   */
  void start();

  /**
   * @brief   Stops timer 
   */
  void stop();

  /**
   * @brief   Shuts down socket 
   */
  void terminate();


  /**
   * @brief   Read some from socket and save to buffer
   *          Parses request and executes handlers
   */
  void read();

  /**
   * @brief   Write buffer to socket 
   *          Call terminate()
   */
  void write();

  /**
   * @brief   Checks deadline expiration, 
   *          If expired, terminates connection 
   *          Otherwise, put timer on async wait for 
   *            -# on expiration 
   *            -# on cancel
   */
  void check_read_deadline();
  void send_read_timeout();

public:
  SocketType socket_;
private:
  std::array<char, 4096> buffer_;
  DeadlineTimer read_deadline_;
  Request request_;
  Response response_;
  Context context_{request_, response_};
  RequestParser request_parser_;
  Router<Handler> &router_;
};



}

#endif