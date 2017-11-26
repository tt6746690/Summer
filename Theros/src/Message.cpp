
#include "Message.h"
#include "RequestParser.h"
#include "Url.h"

#include <cassert>
#include <utility>
#include <iostream> 
#include <algorithm>


namespace Theros
{




//////////////////////////////////////////////////

std::string Message::version(int major, int minor)
{
    return "HTTP/" + std::to_string(major) + "." + std::to_string(minor);
}



std::pair<std::string, bool> Message::get_header(std::string name)
{
    std::string val{};
    bool valid = false;

    for (auto &header : headers)
    {
        if (header_name(header) == name)
            val = header_value(header), valid = true;
    }
    return std::make_pair(val, valid);
}

std::string Message::flatten_header() const
{
    std::string headersflat;
    for (auto &header : headers)
        headersflat += header.first + ": " + header.second + CRLF;
    return headersflat + CRLF;
}

void Message::set_header(HeaderType header)
{
    auto found = find_if(headers.begin(), headers.end(),
                         [&](auto &h) {
                             return h.first == header_name(header);
                         });
    if (found != headers.end())
        *found = header;
    else
        headers.push_back(header);
}

void Message::unset_header(std::string name)
{
    auto end = std::remove_if(headers.begin(), headers.end(),
                              [&](auto &header) {
                                  return header.first == name;
                              });
    headers.erase(end, headers.end());
}

int Message::content_length()
{
    std::string val = "";
    bool found;
    std::tie(val, found) = get_header("Content-Length");

    if (found)
        return std::atoi(val.c_str());

    set_header({"Content-Length", "0"});
    return 0;
}

void Message::content_length(int length)
{
    set_header({"Content-Length", std::to_string(length)});
}

std::string Message::content_type()
{
    std::string val = "";
    bool found;
    std::tie(val, found) = get_header("Content-Type");

    if (found)
        return val;
    else
        set_header({"Content-Type", ""});
    return "";
}

void Message::content_type(std::string value)
{
    set_header({"Content-Type", value});
}

std::ostream& operator<<(std::ostream& os, const Message::HeaderType& header)
{
    return os << Message::header_name(header) << ": " << Message::header_value(header) << std::endl;
}

//////////////////////////////////////////////////

void Uri::decode() {
  scheme = urldecode(scheme);
  host = urldecode(host);
  abs_path = urldecode(abs_path);
  query = urldecode(query);
  fragment = urldecode(fragment);
}


/*
    Request-URI    = "*" | absoluteURI | abs_path | authority

    http_URL (absoluteURI) = "http:" "//" host [ ":" port ] [ abs_path [ "?"
   query ]]

    Note
      -- port=80 by default
      -- host, scheme are case insensitive, rest case sensitive
      -- abs_path=/ by default


    Caveates:
      ignore "*", and authority format

    TODO:
      -- decode url
      -- transmit error code that is appropriate
      -- return 414 request-uri too long ...
  */

ParseStatus Uri::consume(char c) {
  switch (state_) {
  case UriState::uri_start:
    if (c == '/') {
      state_ = UriState::uri_abs_path;
      abs_path.push_back(c);
      return ParseStatus::in_progress;
    }
    if (is_alpha(c)) {
      scheme.push_back(c);
      state_ = UriState::uri_scheme;
      return ParseStatus::in_progress;
    }
    break;
  case UriState::uri_scheme:
    if (is_alpha(c)) {
      scheme.push_back(c);
      return ParseStatus::in_progress;
    }
    if (c == ':') {
      state_ = UriState::uri_slash;
      return ParseStatus::in_progress;
    }
    break;
  case UriState::uri_slash:
    if (c == '/') {
      state_ = UriState::uri_slash_shash;
      return ParseStatus::in_progress;
    }
    break;
  case UriState::uri_slash_shash:
    if (c == '/') {
      state_ = UriState::uri_host;
      return ParseStatus::in_progress;
    }
    break;
  case UriState::uri_host:
    if (c == '/') {
      state_ = UriState::uri_abs_path;
      return ParseStatus::in_progress;
    }
    if (c == ':') {
      state_ = UriState::uri_port;
      return ParseStatus::in_progress;
    }
    host.push_back(c);
    return ParseStatus::in_progress;
  case UriState::uri_port:
    if (is_digit(c)) {
      port.push_back(c);
      return ParseStatus::in_progress;
    }
    if (c == '/') {
      state_ = UriState::uri_abs_path;
      return ParseStatus::in_progress;
    }
    break;
  case UriState::uri_abs_path:
    if (c == '?') {
      state_ = UriState::uri_query;
      return ParseStatus::in_progress;
    }
    if (c == '#') {
      state_ = UriState::uri_fragment;
      return ParseStatus::in_progress;
    }
    abs_path.push_back(c);
    return ParseStatus::in_progress;
  case UriState::uri_query:
    if (c == '#') {
      state_ = UriState::uri_fragment;
      return ParseStatus::in_progress;
    }
    query.push_back(c);
    return ParseStatus::in_progress;
  case UriState::uri_fragment:
    fragment.push_back(c);
    return ParseStatus::in_progress;
  default:
    break;
  }
  return ParseStatus::reject;
};

/*
    "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
*/
std::ostream& operator<<(std::ostream &strm, Uri uri) {
  if (uri.scheme.size())
    strm << uri.scheme + "://" + uri.host;
  if (uri.port.size())
    strm << ":" << uri.port;
  if (uri.abs_path.size())
    strm << uri.abs_path;
  if (uri.query.size())
    strm << "?" << uri.query;
  if (uri.fragment.size())
    strm << "#" << uri.fragment;
  return strm;
}


//////////////////////////////////////////////////


std::string Response::to_payload() const 
{
  std::string payloads = status_line() + flatten_header() + body;
  return payloads;
}

StatusCode Response::status_code() { return status_code_; }

void Response::status_code(StatusCode status_code) 
{
  status_code_ = status_code;
}

StatusCode Response::to_status_code(int status_code) 
{
  for (int i = 0; i < status_code_count; ++i) {
    if (status_codes[i] == status_code)
      return static_cast<StatusCode>(i);
  }
  return StatusCode::Not_Found;
}

std::string Response::reason_phrase() 
{
  return status_code_to_reason(status_code_);
}

std::string Response::status_line() const 
{
  return to_status_line(status_code_, version_major, version_minor);
};

std::string Response::to_status_line(StatusCode status_code, int http_version_major, int http_version_minor) 
{
  return version(http_version_major, http_version_minor) + " " +
         std::to_string(status_code_to_int(status_code)) + " " +
         status_code_to_reason(status_code) + CRLF;
}

std::string Response::to_status_line(int status_code, std::string reason, int http_version_major, int http_version_minor)
{
  return version(http_version_major, http_version_minor) + " " +
         std::to_string(status_code) + " " + reason + CRLF;
}

void Response::write_text(std::string data) 
{
  if (data.find("<!doctype html>") == 0)
    content_type("text/html; charset=utf-8");
  else
    content_type("text/plain");

  content_length(content_length() + data.size());
  body += data;
}

void Response::write_range(char* data, int start, int end, int total) 
{
  set_header({"Content-Range", "bytes " + std::to_string(start) + "-" + std::to_string(end) + "/" + std::to_string(total)});
  content_length(content_length() + end - start);
  status_code_ = StatusCode::Partial_Content;

  body += std::string(data, end-start);
}

void Response::write_json(JsonType data) {
  std::string dump = data.dump();

  content_type("application/json");
  content_length(content_length() + dump.size());

  body += dump;
}

void Response::clear_body() {
  body.clear();
  content_length(0);
}


int Response::status_code_to_int(StatusCode status_code) 
{ 
  return enum_map(status_codes, status_code); 
}
char *Response::status_code_to_reason(StatusCode status_code) 
{ 
  return enum_map(reason_phrases, status_code); 
}

std::ostream &operator<<(std::ostream &os, const Response &response) {
  os << "< " << response.status_line();
  for (auto &header : response.headers) {
    os << "< " << header << std::endl;
  }
  os << "< " << response.body << std::endl;
  return os;
}




} // namespace Theros
