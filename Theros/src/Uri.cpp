#include <cstdio>
#include <ostream>

#include "Uri.h"
#include "Url.h"
#include "Constants.h"
#include "RequestParser.h"
#include "Utils.h"

namespace Theros {

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


}