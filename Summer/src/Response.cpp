#include "asio.hpp"
#include "json.hpp"
#include <algorithm>
#include <ostream>
#include <string>
#include <utility>

#include "Constants.h"
#include "Response.h"

namespace Http {

auto Response::to_payload() const -> std::string {
  std::string payloads = status_line() + flatten_header() + body_;
  return payloads;
}

StatusCode Response::status_code() { return status_code_; }

void Response::status_code(StatusCode status_code) {
  status_code_ = status_code;
}

auto Response::to_status_code(int status_code) -> StatusCode {
  for (int i = 0; i < status_code_count; ++i) {
    if (status_codes[i] == status_code)
      return static_cast<StatusCode>(i);
  }
  return StatusCode::Not_Found;
}

std::string Response::reason_phrase() {
  return status_code_to_reason(status_code_);
}

auto Response::status_line() const -> std::string {
  return to_status_line(status_code_, version_major_, version_minor_);
};

auto Response::to_status_line(StatusCode status_code, int http_version_major,
                              int http_version_minor) -> std::string {
  return version(http_version_major, http_version_minor) + " " +
         std::to_string(status_code_to_int(status_code)) + " " +
         status_code_to_reason(status_code) + EOL;
}

auto Response::to_status_line(int status_code, std::string reason,
                              int http_version_major, int http_version_minor)
    -> std::string {
  return version(http_version_major, http_version_minor) + " " +
         std::to_string(status_code) + " " + reason + EOL;
}

auto Response::write_text(std::string data) -> void {
  if (data.find("<!doctype html>") == 0)
    content_type("text/html; charset=utf-8");
  else
    content_type("text/plain");

  content_length(content_length() + data.size());
  body_ += data;
}

auto Response::write_range(char* data, int start, int end, int total)
      -> void{
  set_header({"Content-Range", "bytes " + std::to_string(start) + "-" + std::to_string(end) + "/" + std::to_string(total)});
  content_length(content_length() + end - start);
  status_code_ = StatusCode::Partial_Content;

  body_ += std::string(data, end-start);
}

auto Response::write_json(json_type data) -> void {
  std::string dump = data.dump();

  content_type("application/json");
  content_length(content_length() + dump.size());

  body_ += dump;
}

auto Response::clear_body() -> void {
  body_.clear();
  content_length(0);
}

std::ostream &operator<<(std::ostream &strm, const Response &response) {
  strm << "< " << response.status_line();
  for (auto &header : response.headers_) {
    strm << "< " << header << std::endl;
  }
  strm << "< " << response.body_ << std::endl;
  return strm;
}
}