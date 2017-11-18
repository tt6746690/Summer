#ifndef __HEADER_H__
#define __HEADER_H__

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include "Defines.h"
#include "Utils.h"  // enum_map
#include "Constants.h"


namespace Theros
{

class Message
{
public:
  using HeaderNameType = std::string;
  using HeaderValueType = std::string;
  using HeaderType = std::pair<HeaderNameType, HeaderValueType>;

  /**
   * @brief   appends a char to name/value of last header in headers
   *
   * @pre headers_ must be nonempty
   */
  void build_header_name(char c);
  void build_header_value(char c);

  /**
   * @brief   Gets header with given name
   */
  auto get_header(HeaderNameType name) -> std::pair<HeaderValueType, bool>;
  /**
   * @brief   Concatenates header key:value pair
   */
  auto flatten_header() const -> std::string;
  /**
   * @brief   Sets header with given name and value
   *
   * Overwrites existing header if name matches,
   * otherwise appends header to end of headers_
   */
  void set_header(HeaderType);
  /**
   * @brief   Removes header with given name
   */
  void unset_header(HeaderNameType name);

  /**
     * @brief   Gets/Sets commonly used headers
     */
  auto content_length() -> int;
  void content_length(int length);
  auto content_type() -> HeaderValueType;
  void content_type(HeaderValueType value);

  int version_major_;
  int version_minor_;
  std::vector<HeaderType> headers_;
  std::string body_;

public:
  /**
   * @brief   Return HTTP version
   */
  static auto version(int major, int minor) -> std::string;

  /**
   * @brief   Given a header, return its name/value
   */
  static auto header_name(const HeaderType &header) -> HeaderNameType &;
  static auto header_value(const HeaderType &header) -> HeaderValueType &;

  friend std::ostream& operator<<(std::ostream &strm, const HeaderType&);
};




class Response : public Message
{
public:
  /**
   * @brief   Generates response string
   */
  auto to_payload() const -> std::string;
  /**
   * @brief   gets/Sets status code for response
   */
  auto status_code() -> StatusCode;
  void status_code(StatusCode status_code);
  /**
   * @brief   Convert Status code from int to enum type
   */
  auto static to_status_code(int status_code) -> StatusCode;
  /**
   * @brief   Gets reason phrase for this instance
   */
  auto reason_phrase() -> std::string;
  /**
   * @brief   Gets status line
   *
   * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
   */
  auto status_line() const -> std::string;
  auto static to_status_line(StatusCode status_code, int http_version_major = 1, int http_version_minor = 1) -> std::string;
  auto static to_status_line(int status_code, std::string reason, int http_version_major = 1, int http_version_minor = 1) -> std::string;

  /**
   * @brief   Append to body and set corresponding content-{type, length}
   *
   * Types:
   *    string
   *        sets content-type to text/html or text/plain
   *        sets content-length
   *    buffer,
   *        sets content-type to application/octet-stream
   *        sets content-length
   *    stream,
   *        sets content-type to application/octet-stream
   *    json,
   *        sets content-type to application/json
   */
  void write_text(std::string data);
  void write_range(char *data, int start, int end, int total);
  void write_json(JsonType data);

  /**
   * @brief   Clears body and resets size
   */
  void clear_body();

private:
  StatusCode status_code_ = StatusCode::OK; // defaults to 200 OK

public:
  /**
   * @brief   Gets numeric status code / reason phrase given StatusCode
   */
  static int status_code_to_int(StatusCode status_code);
  static char *status_code_to_reason(StatusCode status_code);
  friend std::ostream &operator<<(std::ostream &os, const Response &response);
};



enum class ParseStatus;

class Uri
{
public:
  std::string scheme_;
  std::string host_;
  std::string port_;
  std::string abs_path_;
  std::string query_;
  std::string fragment_;
  UriState state_ = UriState::uri_start;

public:
  /** @brief   Advance state for parsing uri given char */
  ParseStatus consume(char c);
  /** @brief   Decodes fields in uri */
  void decode();
  friend std::ostream& operator<<(std::ostream &strm, const Uri uri);
};


class Request : public Message
{

public:
  RequestMethod method_ = RequestMethod::UNDETERMINED;

  Uri uri_;
  std::unordered_map<std::string, std::string> param_;
  std::unordered_map<std::string, std::string> query_;

public:
  constexpr static const char *request_method_to_string(RequestMethod method)
  {
    return enum_map(request_methods, method);
  };
  constexpr static RequestMethod string_to_request_method(std::string &method)
  {
    switch (method.front())
    {
    case 'G':
      return static_cast<RequestMethod>(0);
    case 'H':
      return static_cast<RequestMethod>(1);
    case 'P':
    {
      switch (method[1])
      {
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
      -> std::ostream &
  {
    strm << "> " << Request::request_method_to_string(request.method_) << " "
         << request.uri_ << " "
         << Message::version(request.version_major_, request.version_minor_)
         << std::endl;
    for (auto header : request.headers_)
    {
      strm << "> " << header << std::endl;
    }
    strm << "> " << request.body_ << std::endl;

    return strm;
  }
};



} // namespace Theros
#endif // __CONSTANTS_H__
