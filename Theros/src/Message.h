#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <iostream>
#include <algorithm>

#include <string>
#include <vector>
#include <unordered_map>

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



} // namespace Theros
#endif // __MESSAGE_H__
