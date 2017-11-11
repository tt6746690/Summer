#ifndef __URI_H__
#define __URI_H__

#include <iosfwd>
#include <string>

#include "Defines.h"
#include "StrUtils.h"
#include "Constants.h"

namespace Theros
{

// forward declare
enum class ParseStatus;

// interface function
auto ctohex(unsigned int c) -> std::string;
auto is_uri(char c) -> bool;
auto is_uri_unreserved(char c) -> bool;

struct Uri
{
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
  auto static make_query(const std::string &query) -> ssumap;

  // print
  friend auto operator<<(std::ostream &strm, Uri uri) -> std::ostream &;
};

} // namespace Theros
#endif // __URI_H__