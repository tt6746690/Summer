
#include "asio.hpp"
#include "asio/ssl.hpp"

#include <vector>     // vector<pair<string, string>>    
#include <string>
#include <iostream>

#include "Connection.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"

using namespace std;

namespace Theros {


template<typename SocketType> 
void Connection<SocketType>::stop(){
  read_deadline_.cancel();
}

template<typename SocketType>
void Connection<SocketType>::terminate() {};

template<>
void Connection<TcpSocket>::terminate(){
  stop();
  socket_.shutdown(asio::ip::tcp::socket::shutdown_both);
  socket_.close();
}

template<>
void Connection<SslSocket>::terminate(){
  stop();
  socket_.async_shutdown(
    [this, self=this->shared_from_this()](std::error_code ec) { 
    socket_.lowest_layer().close(); 
  });
}


template<typename SocketType>
void Connection<SocketType>::start() {}

template<>
void Connection<TcpSocket>::start() { 
  read(); 

  read_deadline_.async_wait(
    [this, self=this->shared_from_this()]
      (std::error_code ec){
      check_read_deadline();
  });
}

template<>
void Connection<SslSocket>::start(){

  socket_.async_handshake(asio::ssl::stream_base::server,
    [this, self=this->shared_from_this()]
      (std::error_code ec){
        if(!ec){
          read();
          read_deadline_.async_wait(
            [this, self=this->shared_from_this()]
              (std::error_code ecc){
              check_read_deadline();
          });
        }
      });
}

template<typename SocketType>
void Connection<SocketType>::send_read_timeout(){
  response_.status_code = StatusCode::Request_Timeout;
  write();
}

template<typename SocketType>
void Connection<SocketType>::check_read_deadline(){
  if(read_deadline_.expires_at() <= ClockType::now()){
    send_read_timeout();
  } else {
    read_deadline_.async_wait(
      [this, self=this->shared_from_this()]
        (std::error_code ec){
        check_read_deadline();
    });
  }
}


template<typename SocketType>
void Connection<SocketType>::read() {

  read_deadline_.expires_from_now(read_timeout);

  asio::async_read(
    socket_, 
    asio::buffer(buffer_), 
    asio::transfer_at_least(1),
    [ this, self = this->shared_from_this() ]
      (std::error_code ec, std::size_t bytes_read) {

      assert(this == self.get());
      if (!ec) {
        decltype(buffer_.begin()) begin;
        ParseStatus parse_status;

        std::tie(begin, parse_status) =
            request_parser_.parse(request_, buffer_.begin(), buffer_.begin() + bytes_read);

        /**
        * Current buffer is fully read, branch on ParseStatus
        *    -- in_progress,
        *        so continue do async read
        *    -- accept,
        *        request header parsing finished
        *    -- reject,
        *        request has malformed syntax, send 400
        */
        switch (parse_status) {
            case ParseStatus::in_progress: {
              read();
              break;
            }
            case ParseStatus::accept: {
              response_.status_code = StatusCode::OK;
              response_.version = request_.version;

              // Resolves route and populate request.uri_param 
              std::vector<std::pair<std::string, std::string>> kv;
              auto handlers = router_.resolve(request_, kv);
              request_.uri_param.insert(kv.begin(), kv.end());

              for (auto &handler : handlers) {
               handler(context_);
              }

              write();
              break;
            }

            case ParseStatus::reject: {
              response_.status_code = StatusCode::Bad_Request;
              write();
              break;
            }
        }
      } 
    });
}

template<typename SocketType>
void Connection<SocketType>::write() {


  asio::async_write(
    socket_, 
    asio::buffer(response_.ToPayload()),
    asio::transfer_all(),
    [ this, self = this->shared_from_this() ](
        std::error_code ec, std::size_t bytes_written) {

      if (!ec) {
        terminate();
      }
    });
}



}

#pragma clang diagnostic pop