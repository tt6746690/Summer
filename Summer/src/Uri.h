#ifndef URI_H
#define URI_H

#include <ostream>
#include <string>

#include "Constants.h"

namespace Http {

/**
 * @brief   Converts 1 utf8 byte to its hex value
 */
auto ctohex(unsigned int c) -> std::string;

class Uri {
public:
  /* Assume only accepting Url for now*/
  std::string scheme_;
  std::string host_;
  std::string port_;
  std::string abs_path_;
  std::string query_;
  std::string fragment_;

  UriState state_ = UriState::uri_start;

  /**
   * @brief   Advance state for parsing uri given char
   *
   * @precondition  c is valid uri char
   */
  auto consume(char c) -> ParseStatus;

  /**
   * @brief   Decodes fields in uri
   */
  auto decode() -> void;
  /**
   * @brief   encode url
   *
   * @precond assumes utf8 encoded string
   *          assumes url consists of uri allowed charset
   *
   * -- No need to encode unreserved charset
   * -- Percent encode reserved charset,
   *  --  convert each char (ASCII or non-ASCII) to utf-8
   *  --  Represenet byte value with hex digits, preceded by %
   */
  auto static urlencode(const std::string &url) -> std::string;
  /**
   * @brief   decode url
   *
   * @precond assumes url consists of uri allowed charset
   */
  auto static urldecode(const std::string &url) -> std::string;

  /**
   * @brief   Convert a query string to a map of key-value pairs
   */
  auto static make_query(const std::string &query) -> ssmap;

public:
  friend auto operator<<(std::ostream &strm, Uri uri) -> std::ostream &;
};

/**
 * static strings
 */

static constexpr char unreserved_charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";
static constexpr char reserved_charset[] = "!*'();:@&=+$,/?#[]";
static constexpr char uri_charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmn"
                                      "opqrstuvwxyz0123456789-_.~!*'();:@&=+$,/"
                                      "?#[]";

constexpr bool is_uri_unreserved(char c) {
  for (auto b = std::begin(unreserved_charset),
            e = std::end(unreserved_charset);
       b != e; b++) {
    if (c == *b)
      return true;
  }
  return false;
}

constexpr bool is_uri(char c) {
  for (auto b = std::begin(uri_charset), e = std::end(uri_charset); b != e;
       b++) {
    if (c == *b)
      return true;
  }
  return false;
}
}

#endif