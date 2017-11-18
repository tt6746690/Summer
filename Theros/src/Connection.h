#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "asio.hpp"
#include "asio/ssl.hpp"
#include "asio/basic_waitable_timer.hpp"

#include <chrono>
#include <utility> // enable_shared_from_this, move

#include "RequestParser.h"
#include "Message.h"
#include "Router.h"

namespace Theros
{

// forward declaration
enum class ParseStatus;

using TcpSocket = asio::ip::tcp::socket;
using SslSocket = asio::ssl::stream<asio::ip::tcp::socket>;
using ClockType = std::chrono::steady_clock;

template <typename SocketType>
class Connection
    : public std::enable_shared_from_this<Connection<SocketType>>
{

public:
  using DeadlineTimer = asio::basic_waitable_timer<ClockType>;
  static constexpr auto max_time = ClockType::duration::max();
  static constexpr auto read_timeout = std::chrono::seconds(2);

  explicit Connection(asio::io_service &io_service, Router& router);
  explicit Connection(asio::io_service &io_service, asio::ssl::context &context, Router& router);

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
  Context context_;
  RequestParser request_parser_;
  Router &router_;
};

template <typename SocketType>
Connection<SocketType>::Connection(asio::io_service &io_service, Router &router)
    : socket_(io_service),
      read_deadline_(io_service),
      context_{request_, response_},
      router_(router)
{
  read_deadline_.expires_from_now(max_time);
};

template <typename SocketType>
Connection<SocketType>::Connection(asio::io_service &io_service, asio::ssl::context &context, Router &router)
    : socket_(io_service, context),
      read_deadline_(io_service),
      context_{request_, response_},
      router_(router)
{
  read_deadline_.expires_from_now(max_time);
};

} // namespace Theros
#endif // __CONNECTION_H__