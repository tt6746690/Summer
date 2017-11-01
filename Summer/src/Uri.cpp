#include <cstdio>
#include <ostream>

#include "Constants.h"
#include "RequestParser.h"
#include "Uri.h"

namespace Http {

auto ctohex(unsigned int c) -> std::string {
  std::ostringstream os;
  os << std::hex << std::uppercase << c;
  return os.str();
}

auto Uri::decode() -> void {
  scheme_ = urldecode(scheme_);
  host_ = urldecode(host_);
  abs_path_ = urldecode(abs_path_);
  query_ = urldecode(query_);
  fragment_ = urldecode(fragment_);
}

auto Uri::urlencode(const std::string &url) -> std::string {
  if (url.empty())
    return {};

  std::string encoded{};
  for (auto c : url) {
    if (is_uri_unreserved(c))
      encoded += c;
    else
      encoded.append("%" + ctohex(static_cast<unsigned char>(c)));
  }
  return encoded;
}

auto Uri::urldecode(const std::string &url) -> std::string {
  if (url.empty())
    return {};

  std::string decoded{};
  char c;
  int cvt;

  for (auto itr = url.cbegin(); itr != url.cend(); ++itr) {
    c = *itr;
    if (c != '%') {
      decoded += c;
    } else {
      std::string hexhex{itr + 1, itr + 3};
      sscanf(hexhex.c_str(), "%x", &cvt);
      decoded += static_cast<unsigned char>(cvt);
      itr += 2;
    }
  }
  return decoded;
}

auto Uri::make_query(const std::string &qstr) -> ssmap {
  constexpr char tok_and = '&';
  constexpr char tok_equal = '=';

  std::string query = qstr;
  query += tok_and;

  std::size_t pos = 0;
  std::string token, key, value;
  ssmap query_map;

  while ((pos = query.find(tok_and)) != std::string::npos) {
    token = query.substr(0, pos);
    std::tie(key, value) = split(token, tok_equal);
    query_map.insert({key, value});
    query.erase(0, pos + 1);
  }
  return query_map;
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

auto Uri::consume(char c) -> ParseStatus {
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
auto operator<<(std::ostream &strm, Uri uri) -> std::ostream & {
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
}