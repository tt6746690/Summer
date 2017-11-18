#ifndef __REQUESTPARSER_H__
#define __REQUESTPARSER_H__

#include <iosfwd>

namespace Theros
{

// foward declaration
class Request;

enum class ParseStatus
{
    accept = 1,
    reject,
    in_progress
};

class RequestParser
{
public:
  enum class State;
  explicit RequestParser();
  /**
   * @brief Populate Request object given a Range of chars
   */
  template <typename In>
  auto parse(Request &request, In begin, In end) -> std::tuple<In, ParseStatus>;
  /**
   * @brief   Advance parser state given input char
   */
  auto consume(Request &request, char c) -> ParseStatus;
  // Helper functions
  static void view_state(RequestParser::State state, ParseStatus status, char c);
  friend auto operator<<(std::ostream &strm, ParseStatus &status) -> std::ostream &;

public:
  // member public
  State state_;
};

/*
    OCTET          = <any 8-bit sequence of data>
    CHAR           = <any US-ASCII character (octets 0 - 127)>
    UPALPHA        = <any US-ASCII uppercase letter "A".."Z">
    LOALPHA        = <any US-ASCII lowercase letter "a".."z">
    ALPHA          = UPALPHA | LOALPHA
    DIGIT          = <any US-ASCII digit "0".."9">
    CTL            = <any US-ASCII control character
                        (octets 0 - 31) and DEL (127)>
    CR             = <US-ASCII CR, carriage return (13)>
    LF             = <US-ASCII LF, linefeed (10)>
    SP             = <US-ASCII SP, space (32)>
    HT             = <US-ASCII HT, horizontal-tab (9)>
    <">            = <US-ASCII double-quote mark (34)>

    CRLF           = CR LF
    LWS            = [CRLF] 1*( SP | HT )
    TEXT           = <any OCTET except CTLs,
                    but including LWS>

    token          = 1*<any CHAR except CTLs or separators>
    separators     = "(" | ")" | "<" | ">" | "@"
                    | "," | ";" | ":" | "\" | <">
                    | "/" | "[" | "]" | "?" | "="
                    | "{" | "}" | SP | HT
*/

/**
 * @brief   Helper functions for parsers
 */
constexpr bool is_char(char c) { return c >= 0 && c <= 127; }
constexpr bool is_upperalpha(char c) { return c >= 65 && c <= 90; }
constexpr bool is_loweralpha(char c) { return c >= 97 && c <= 122; }
constexpr bool is_alpha(char c) { return is_loweralpha(c) || is_upperalpha(c); }
constexpr bool is_digit(char c) { return c >= 48 && c <= 57; }
constexpr bool is_ctl(char c) { return (c >= 0 && c <= 31) || c == 127; }
constexpr bool is_cr(char c) { return c == 13; }
constexpr bool is_lf(char c) { return c == 10; }
constexpr bool is_crlf(char c) { return c == 13 || c == 10; }
constexpr bool is_sp(char c) { return c == 32; }
constexpr bool is_ht(char c) { return c == 9; }
constexpr bool is_separator(char c)
{
  switch (c)
  {
  case '(':
  case ')':
  case '<':
  case '>':
  case '@':
  case ',':
  case ';':
  case ':':
  case '\\':
  case '"':
  case '/':
  case '[':
  case ']':
  case '?':
  case '=':
  case '{':
  case '}':
  case ' ':
  case '\t':
    return true;
  default:
    return false;
  }
}
constexpr bool is_token(char c) { return !is_ctl(c) && !is_separator(c) && is_char(c); }

// Template definition

template <typename In>
inline std::tuple<In, ParseStatus> RequestParser::parse(Request &request, In begin, In end)
{
  ParseStatus status;
  while (begin != end)
  {
    status = consume(request, *begin++);
    if (status == ParseStatus::accept || status == ParseStatus::reject)
      break;
  }
  return {begin, status};
}

} // namespace Theros
#endif // __REQUESTPARSER_H__