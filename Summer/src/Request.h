#ifndef REQUEST_H
#define REQUEST_H

#include <ostream>
#include <string>
#include <unordered_map>

#include "Constants.h" // RequestMetho
#include "Message.h"   // base class
#include "Uri.h"       // Uri
#include "Utilities.h" // enum_map

namespace Http {

class Request : public Message {

public:
  RequestMethod method_ = RequestMethod::UNDETERMINED;

  Uri uri_;
  ssmap param_;
  ssmap query_;

public:
  constexpr static const char *request_method_to_string(RequestMethod method) {
    return enum_map(request_methods, method);
  };
  constexpr static RequestMethod string_to_request_method(std::string& method){
    switch(method.front()){
      case 'G':
        return static_cast<RequestMethod>(0);
      case 'H':
        return static_cast<RequestMethod>(1);
      case 'P':{
        switch(method[1]){
          case 'O':
            return static_cast<RequestMethod>(2);
          case 'U':
            return static_cast<RequestMethod>(3);
          case 'A':
            return static_cast<RequestMethod>(4);
        }
      }
      case 'D':
        return static_cast<RequestMethod>(5);
      case 'C':
        return static_cast<RequestMethod>(6);
      case 'O':
        return static_cast<RequestMethod>(7);
      case 'T':
        return static_cast<RequestMethod>(8);
      default:
        return static_cast<RequestMethod>(9);
    }
  }

  friend auto inline operator<<(std::ostream &strm, const Request &request)
      -> std::ostream & {
    strm << "> " << Request::request_method_to_string(request.method_) << " "
         << request.uri_ << " "
         << Message::version(request.version_major_, request.version_minor_)
         << std::endl;
    for (auto header : request.headers_) {
      strm << "> " << header << std::endl;
    }
    strm << "> " << request.body_ << std::endl;

    return strm;
  }
};
}

#endif