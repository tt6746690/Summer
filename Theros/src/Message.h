#ifndef __HEADER_H__
#define __HEADER_H__

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "Defines.h"
#include "Utils.h"  // enum_map
#include "Constants.h"


namespace Theros
{


////////////////////////////////////////////////////////////////////////
// Message 
////////////////////////////////////////////////////////////////////////

struct DefaultBody  {
public:
  using value_type = std::string;
public:

};

enum class HttpVersion : uint8_t {
  zero_nine,
  one_zero,
  one_one,
  two_zero,
  undetermined
};

/** Stringify Http version */
std::string version_as_string(HttpVersion v);


template<typename BodyType = DefaultBody>
class Message {
public:
  struct Header {
    std::string name;
    std::string value;
  };
  using Headers         = std::vector<Header>;
  using HeadersIterator = typename Headers::iterator;
  using Body            = typename BodyType::value_type;
public:
  HttpVersion   version;
  Headers       headers;
  Body          body;
public:
  Message() : version(HttpVersion::undetermined) { }
  /** 
   * Methods for manipulating headers 
   */
  /** Finds header value given a header name */
  std::string FindHeader(const std::string& name);
  /** Setting a header either modifies an existing header in place or insert a new one */
  void SetHeader(const Header& header); 
  /** Removing a header either removes an existing header or is an no-op */
  void RemoveHeader(const std::string& name);

  /** Convenience methods for Getting/Setting certain headers
   *    -- Content Length 
   *    -- Content Type
   *  Lookup func inserts entry with default values if header not found
   */
  int  ContentLength();
  void ContentLength(int length);
  std::string ContentType();
  void ContentType(const std::string& cont_type);

private:
  /** Returns an iterator to a header with name equivalent to `name` */
  HeadersIterator find_header_by_name(const std::string& name);
public:
  friend inline std::ostream& operator<<(std::ostream& os, const Header& h) { return os << h.name + ": " + h.value; }
}; 



////////////////////////////////////////////////////////////////////////
// Request 
////////////////////////////////////////////////////////////////////////

struct Uri
{
  std::string scheme;
  std::string host;
  std::string port;
  std::string abs_path;
  std::string query;
  std::string fragment;
};


/** Stringify in this format : "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]] */
std::string uri_as_string(const Uri& uri);


class Request : public Message<> {
public:
  using Method  = RequestMethod;
  using MapType = std::unordered_map<std::string, std::string>;
public:
  Method    method;
  Uri       uri;
  MapType   uri_param;
  MapType   uri_query;
public:
  Request() : method(RequestMethod::UNDETERMINED) { }
  friend std::ostream& operator<<(std::ostream& os, const Request& req);
};

std::string   request_method_as_string(RequestMethod method);
RequestMethod request_method_from_cstr(const char* method);


////////////////////////////////////////////////////////////////////////
// Response 
////////////////////////////////////////////////////////////////////////


class Response : public Message<> {
public:
  StatusCode status_code;
public:
  Response() : status_code(StatusCode::OK) { }

  /** Serialize and concatenate status line, headers, and body */
  std::string ToPayload() const;
  /** Serialize status line, and headers */
  std::string StatusLine() const;
  std::string HeaderLine() const;

public:
  friend std::ostream& operator<<(std::ostream& os, const Response& res);
};

int status_code_as_int(StatusCode status_code);
const char* status_code_as_reason(StatusCode status_code);
StatusCode status_code_from_int(int status_code);


////////////////////////////////////////////////////////////////////////
// impls 
////////////////////////////////////////////////////////////////////////

template<typename BodyType>
auto Message<BodyType>::find_header_by_name(const std::string& name) -> HeadersIterator
{
  return std::find_if(headers.begin(), headers.end(), 
    [&name](Header& h){ return h.name == name; });
}

template<typename BodyType>
std::string Message<BodyType>::FindHeader(const std::string& name) 
{
  std::string header_value;
  HeadersIterator found = find_header_by_name(name);
  if (found != headers.end()) header_value = found->value;
  return header_value;
}

template<typename BodyType> 
void Message<BodyType>::SetHeader(const Header& header) 
{
  HeadersIterator found = find_header_by_name(header.name);
  if (found != headers.end())
    *found = header;
  else 
    headers.push_back(header);
}

template<typename BodyType>
void Message<BodyType>::RemoveHeader(const std::string& name)
{
  auto end = std::remove_if(headers.begin(), headers.end(),
    [&name](Header& h) { return h.name == name; });
  headers.erase(end, headers.end());
}

template<typename BodyType>
int  Message<BodyType>::ContentLength() {
  HeadersIterator it = find_header_by_name("Content-Length");
  if (it != headers.end())
    return std::atoi(it->value.c_str());
  else
    SetHeader({"Content-Length", "0"}); return 0;
}


template<typename BodyType>
std::string Message<BodyType>::ContentType() 
{
  HeadersIterator it = find_header_by_name("Content-Type");
  if (it != headers.end()) {
    return it->value;
  } else {
    SetHeader({"Content-Type", ""}); return "";
  }
}

template<typename BodyType>
void Message<BodyType>::ContentLength(int length) { SetHeader({"Content-Length", std::to_string(length)}); }

template<typename BodyType>
void Message<BodyType>::ContentType(const std::string& cont_type) { SetHeader({"Content-Type", cont_type}); }




// class Message
// {
// public:
//   using HeaderType = std::pair<std::string, std::string>;

//   /**
//    * @brief   Gets header with given name
//    */
//   auto get_header(std::string name) -> std::pair<std::string, bool>;
//   /**
//    * @brief   Concatenates header key:value pair
//    */
//   auto flatten_header() const -> std::string;
//   /**
//    * @brief   Sets header with given name and value
//    *
//    * Overwrites existing header if name matches,
//    * otherwise appends header to end of headers_
//    */
//   void set_header(HeaderType);
//   /**
//    * @brief   Removes header with given name
//    */
//   void unset_header(std::string name);

//   /**
//      * @brief   Gets/Sets commonly used headers
//      */
//   auto content_length() -> int;
//   void content_length(int length);
//   auto content_type() -> std::string;
//   void content_type(std::string value);

//   int version_major;
//   int version_minor;
//   std::vector<HeaderType> headers;
//   std::string body;

// public:
//   /**
//    * @brief   Return HTTP version
//    */
//   static auto version(int major, int minor) -> std::string;

//   /**
//    * @brief   Given a header, return its name/value
//    */
//   static std::string& header_name(HeaderType &header) { return std::get<0>(header); }
//   static std::string& header_name(const HeaderType &header) { return std::get<0>(const_cast<HeaderType&>(header)); }
//   static std::string& header_value(HeaderType &header) { return std::get<1>(header); }
//   static std::string& header_value(const HeaderType &header) { return std::get<1>(const_cast<HeaderType&>(header)); }

//   friend std::ostream& operator<<(std::ostream &strm, const HeaderType&);
// };


 

// class Response : public Message
// {
// public:
//   /**
//    * @brief   Generates response string
//    */
//   auto to_payload() const -> std::string;
//   /**
//    * @brief   gets/Sets status code for response
//    */
//   auto status_code() -> StatusCode;
//   void status_code(StatusCode status_code);
//   /**
//    * @brief   Convert Status code from int to enum type
//    */
//   auto static to_status_code(int status_code) -> StatusCode;
//   /**
//    * @brief   Gets reason phrase for this instance
//    */
//   auto reason_phrase() -> std::string;
//   /**
//    * @brief   Gets status line
//    *
//    * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
//    */
//   auto status_line() const -> std::string;
//   auto static to_status_line(StatusCode status_code, int http_version_major = 1, int http_version_minor = 1) -> std::string;
//   auto static to_status_line(int status_code, std::string reason, int http_version_major = 1, int http_version_minor = 1) -> std::string;

//   /**
//    * @brief   Append to body and set corresponding content-{type, length}
//    *
//    * Types:
//    *    string
//    *        sets content-type to text/html or text/plain
//    *        sets content-length
//    *    buffer,
//    *        sets content-type to application/octet-stream
//    *        sets content-length
//    *    stream,
//    *        sets content-type to application/octet-stream
//    *    json,
//    *        sets content-type to application/json
//    */
//   void write_text(std::string data);
//   void write_range(char *data, int start, int end, int total);
//   void write_json(JsonType data);

//   /**
//    * @brief   Clears body and resets size
//    */
//   void clear_body();

// private:
//   StatusCode status_code_ = StatusCode::OK; // defaults to 200 OK

// public:
//   /**
//    * @brief   Gets numeric status code / reason phrase given StatusCode
//    */
//   static int status_code_to_int(StatusCode status_code);
//   static char *status_code_to_reason(StatusCode status_code);
//   friend std::ostream &operator<<(std::ostream &os, const Response &response);
// };





// class Request : public Message
// {

// public:
//   RequestMethod method = RequestMethod::UNDETERMINED;

//   Uri uri;
//   std::unordered_map<std::string, std::string> param;
//   std::unordered_map<std::string, std::string> query;

// public:
//   constexpr static const char *request_method_to_string(RequestMethod method)
//   {
//     return enum_map(request_methods, method);
//   };
//   constexpr static RequestMethod string_to_request_method(std::string &method)
//   {
//     switch (method.front())
//     {
//     case 'G':
//       return static_cast<RequestMethod>(0);
//     case 'H':
//       return static_cast<RequestMethod>(1);
//     case 'P':
//     {
//       switch (method[1])
//       {
//       case 'O':
//         return static_cast<RequestMethod>(2);
//       case 'U':
//         return static_cast<RequestMethod>(3);
//       case 'A':
//         return static_cast<RequestMethod>(4);
//       }
//     }
//     case 'D':
//       return static_cast<RequestMethod>(5);
//     case 'C':
//       return static_cast<RequestMethod>(6);
//     case 'O':
//       return static_cast<RequestMethod>(7);
//     case 'T':
//       return static_cast<RequestMethod>(8);
//     default:
//       return static_cast<RequestMethod>(9);
//     }
//   }

//   friend auto inline operator<<(std::ostream &strm, const Request &request)
//       -> std::ostream &
//   {
//     strm << "> " << Request::request_method_to_string(request.method) << " "
//          << uri_as_string(request.uri) << " "
//          << Message::version(request.version_major, request.version_minor)
//          << std::endl;
//     for (auto header : request.headers)
//     {
//       strm << "> " << header << std::endl;
//     }
//     strm << "> " << request.body << std::endl;

//     return strm;
//   }
// };



} // namespace Theros
#endif // __CONSTANTS_H__
