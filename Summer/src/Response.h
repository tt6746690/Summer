#ifndef RESPONSE_H
#define RESPONSE_H

#include "asio.hpp"
#include "json.hpp"
#include <array>

#include "Constants.h"
#include "Message.h"
#include "Utilities.h"

namespace Http
{

class Response : public Message
{
public:
  using BuildStatus = int;

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
  auto static to_status_line(StatusCode status_code, int http_version_major = 1,
                             int http_version_minor = 1) -> std::string;
  auto static to_status_line(int status_code, std::string reason,
                             int http_version_major = 1,
                             int http_version_minor = 1) -> std::string;

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
  auto write_text(std::string data) -> void;
  auto write_range(char *data, int start, int end, int total)
      -> void;
  auto write_json(json_type data) -> void;

  /**
   * @brief   Clears body and resets size
   */
  auto clear_body() -> void;

private:
  StatusCode status_code_ = StatusCode::OK; // defaults to 200 OK

public:
  /**
   * @brief   Gets numeric status code given StatusCode
   */
  static auto constexpr status_code_to_int(StatusCode status_code) -> int
  {
    return enum_map(status_codes, status_code);
  }
  /**
   * @brief   Gets reason phrase given StatusCode
   */
  static auto constexpr status_code_to_reason(StatusCode status_code)
      -> char *
  {
    return enum_map(reason_phrases, status_code);
  }

  friend std::ostream &operator<<(std::ostream &strm, const Response &response);
};
}

#endif