
#include "Message.h"
#include "RequestParser.h"
#include "Url.h"

#include <cassert>
#include <utility>
#include <iostream> 


namespace Theros
{

//////////////////////////////////////////////////

std::string Message::version(int major, int minor)
{
    return "HTTP/" + std::to_string(major) + "." + std::to_string(minor);
}

void Message::build_header_name(char c)
{
    assert(headers_.size() != 0);
    header_name(headers_.back()).push_back(c);
}
void Message::build_header_value(char c)
{
    assert(headers_.size() != 0);
    header_value(headers_.back()).push_back(c);
}

Message::HeaderNameType& Message::header_name(const HeaderType &header)
{
    return std::get<0>(const_cast<HeaderType&>(header));
}
Message::HeaderValueType& Message::header_value(const HeaderType &header)
{
    return std::get<1>(const_cast<HeaderType&>(header));
}

std::pair<Message::HeaderValueType, bool> Message::get_header(HeaderNameType name)
{
    HeaderValueType val{};
    bool valid = false;

    for (auto &header : headers_)
    {
        if (header_name(header) == name)
            val = header_value(header), valid = true;
    }
    return std::make_pair(val, valid);
}

std::string Message::flatten_header() const
{
    std::string headers_flat;
    for (auto &header : headers_)
        headers_flat += header.first + ": " + header.second + CRLF;
    return headers_flat + CRLF;
}

void Message::set_header(HeaderType header)
{
    auto found = find_if(headers_.begin(), headers_.end(),
                         [&](auto &h) {
                             return h.first == header_name(header);
                         });
    if (found != headers_.end())
        *found = header;
    else
        headers_.push_back(header);
}

void Message::unset_header(HeaderNameType name)
{
    auto end = std::remove_if(headers_.begin(), headers_.end(),
                              [&](auto &header) {
                                  return header.first == name;
                              });
    headers_.erase(end, headers_.end());
}

int Message::content_length()
{
    HeaderValueType val = "";
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

Message::HeaderValueType Message::content_type()
{
    HeaderValueType val = "";
    bool found;
    std::tie(val, found) = get_header("Content-Type");

    if (found)
        return val;
    else
        set_header({"Content-Type", ""});
    return "";
}

void Message::content_type(HeaderValueType value)
{
    set_header({"Content-Type", value});
}

std::ostream& operator<<(std::ostream& os, const Message::HeaderType& header)
{
    return os << Message::header_name(header) << ": " << Message::header_value(header) << std::endl;
}

//////////////////////////////////////////////////

void Uri::decode() {
  scheme_ = urldecode(scheme_);
  host_ = urldecode(host_);
  abs_path_ = urldecode(abs_path_);
  query_ = urldecode(query_);
  fragment_ = urldecode(fragment_);
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
      abs_path_.push_back(c);
      return ParseStatus::in_progress;
    }
    if (is_alpha(c)) {
      scheme_.push_back(c);
      state_ = UriState::uri_scheme;
      return ParseStatus::in_progress;
    }
    break;
  case UriState::uri_scheme:
    if (is_alpha(c)) {
      scheme_.push_back(c);
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
    host_.push_back(c);
    return ParseStatus::in_progress;
  case UriState::uri_port:
    if (is_digit(c)) {
      port_.push_back(c);
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
    abs_path_.push_back(c);
    return ParseStatus::in_progress;
  case UriState::uri_query:
    if (c == '#') {
      state_ = UriState::uri_fragment;
      return ParseStatus::in_progress;
    }
    query_.push_back(c);
    return ParseStatus::in_progress;
  case UriState::uri_fragment:
    fragment_.push_back(c);
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
  if (uri.scheme_.size())
    strm << uri.scheme_ + "://" + uri.host_;
  if (uri.port_.size())
    strm << ":" << uri.port_;
  if (uri.abs_path_.size())
    strm << uri.abs_path_;
  if (uri.query_.size())
    strm << "?" << uri.query_;
  if (uri.fragment_.size())
    strm << "#" << uri.fragment_;
  return strm;
}


//////////////////////////////////////////////////


std::string Response::to_payload() const 
{
  std::string payloads = status_line() + flatten_header() + body_;
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
  return to_status_line(status_code_, version_major_, version_minor_);
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
  body_ += data;
}

void Response::write_range(char* data, int start, int end, int total) 
{
  set_header({"Content-Range", "bytes " + std::to_string(start) + "-" + std::to_string(end) + "/" + std::to_string(total)});
  content_length(content_length() + end - start);
  status_code_ = StatusCode::Partial_Content;

  body_ += std::string(data, end-start);
}

void Response::write_json(JsonType data) {
  std::string dump = data.dump();

  content_type("application/json");
  content_length(content_length() + dump.size());

  body_ += dump;
}

void Response::clear_body() {
  body_.clear();
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
  for (auto &header : response.headers_) {
    os << "< " << header << std::endl;
  }
  os << "< " << response.body_ << std::endl;
  return os;
}




} // namespace Theros
