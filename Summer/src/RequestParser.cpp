#include <iostream>
#include <vector> // emplace_back

#include "RequestParser.h"
#include "Uri.h" // uri.consume

namespace Http {

/*
        Request         = Request-Line                  ; Section 5.1
                        *(( general-header              ; Section 4.5
                            | request-header            ; Section 5.3
                            | entity-header ) CRLF)     ; Section 7.1
                        CRLF
                        [ message-body ]                ; Section 4.3

        Request-Line   = *(CRLF) Method SP Request-URI SP HTTP-Version CRLF

        Method          = "OPTIONS"                ; Section 9.2
                        | "GET"                    ; Section 9.3
                        | "HEAD"                   ; Section 9.4
                        | "POST"                   ; Section 9.5
                        | "PUT"                    ; Section 9.6
                        | "DELETE"                 ; Section 9.7
                        | "TRACE"                  ; Section 9.8
                        | "CONNECT"                ; Section 9.9
                        | extension-method
        extension-method = token

        Request-URI    = "*" | absoluteURI | abs_path | authority
        https://www.ietf.org/rfc/rfc2396.txt

        HTTP-Version   = "HTTP" "/" 1*DIGIT "." 1*DIGIT



        Message Headers
        message-header = field-name ":" [ field-value ]
        field-name     = token
        field-value    = *( field-content | LWS )
        field-content  = <the OCTETs making up the field-value
                            and consisting of either *TEXT or combinations
                            of token, separators, and quoted-string>

    */
/*
        Parsing caveats:
            1. did not verify method
            2. did not verify uri
    */
auto RequestParser::consume(Request &request, char c) -> ParseStatus {
  using s = RequestParser::State;
  using status = ParseStatus;

  switch (state_) {
  case s::req_start:
    if (is_cr(c)) {
      state_ = s::req_start_lf;
      return status::in_progress;
    }
    if (is_token(c)) {

      switch (c) {
      case 'G':
        request.method_ = RequestMethod::GET;
        break;
      case 'H':
        request.method_ = RequestMethod::HEAD;
        break;
      case 'P': // POST, PUT, PATCH
        request.method_ = RequestMethod::UNDETERMINED;
        break;
      case 'D':
        request.method_ = RequestMethod::DELETE;
        break;
      case 'C':
        request.method_ = RequestMethod::CONNECT;
        break;
      case 'O':
        request.method_ = RequestMethod::OPTIONS;
        break;
      case 'T':
        request.method_ = RequestMethod::TRACE;
        break;
      default:
        return status::reject;
      }
      state_ = s::req_method;
      return status::in_progress;
    }
    return status::reject;
  case s::req_start_lf:
    if (is_lf(c)) {
      state_ = s::req_start;
      return status::in_progress;
    }
    return status::reject;
  case s::req_method:
    if (is_token(c)) {
      if (request.method_ == RequestMethod::UNDETERMINED) {
        switch (c) {
        case 'O':
          request.method_ = RequestMethod::POST;
          return status::in_progress;
        case 'U':
          request.method_ = RequestMethod::PUT;
          return status::in_progress;
        case 'A':
          request.method_ = RequestMethod::PATCH;
          return status::in_progress;
        default:
          return status::reject;
        }
      }
      return status::in_progress;
    }
    if (is_sp(c)) {
      state_ = s::req_uri;
      return status::in_progress;
    }
    return status::reject;
  case s::req_uri:
    assert(request.method_ != RequestMethod::UNDETERMINED);
    if (is_uri(c)) {
      return request.uri_.consume(c);
    }
    if (is_sp(c)) {
      request.uri_.decode(); // decode fields here...
      state_ = s::req_http_h;
      return status::in_progress;
    }
    return status::reject;
  case s::req_http_h:
    if (c == 'H') {
      state_ = s::req_http_ht;
      return status::in_progress;
    }
    return status::reject;
  case s::req_http_ht:
    if (c == 'T') {
      state_ = s::req_http_htt;
      return status::in_progress;
    }
    return status::reject;
  case s::req_http_htt:
    if (c == 'T') {
      state_ = s::req_http_http;
      return status::in_progress;
    }
    return status::reject;
  case s::req_http_http:
    if (c == 'P') {
      state_ = s::req_http_slash;
      return status::in_progress;
    }
    return status::reject;
  case s::req_http_slash:
    if (c == '/') {
      state_ = s::req_http_major;
      return status::in_progress;
    }
    return status::reject;
  case s::req_http_major:
    if (is_digit(c)) {
      request.version_major_ = c - '0';
      state_ = s::req_http_dot;
      return status::in_progress;
    }
    return status::reject;
  case s::req_http_dot:
    if (c == '.') {
      state_ = s::req_http_minor;
      return status::in_progress;
    }
    return status::reject;
  case s::req_http_minor:
    if (is_digit(c)) {
      request.version_minor_ = c - '0';
      state_ = s::req_start_line_cr;
      return status::in_progress;
    }
    return status::reject;
  case s::req_start_line_cr:
    if (is_cr(c)) {
      state_ = s::req_start_line_lf;
      return status::in_progress;
    }
    return status::reject;
  case s::req_start_line_lf:
    if (is_lf(c)) {
      state_ = s::req_field_name_start;
      return status::in_progress;
    }
    return status::reject;
  case s::req_field_name_start:
    if (is_cr(c)) {
      state_ = s::req_header_end;
      return status::in_progress;
    }
    if (is_token(c)) {
      request.headers_.emplace_back();
      request.build_header_name(c);
      state_ = s::req_field_name;
      return status::in_progress;
    }
  case s::req_field_name:
    if (is_token(c)) {
      request.build_header_name(c);
      return status::in_progress;
    }
    if (c == ':') {
      state_ = s::req_field_value;
      return status::in_progress;
    }
    return status::reject;
  case s::req_field_value:
    if (is_sp(c) || is_ht(c)) {
      return status::in_progress;
    }
    if (is_cr(c)) {
      state_ = s::req_header_lf;
      return status::in_progress;
    }
    if (!is_ctl(c)) {
      request.build_header_value(c);
      return status::in_progress;
    }
    return status::reject;
  case s::req_header_lf:
    if (is_lf(c)) {
      state_ = s::req_header_lws;
      return status::in_progress;
    }
    return status::reject;
  case s::req_header_lws:
    /*
        LWS            = [CRLF] 1*( SP | HT )
        field-value    = *( field-content | LWS )

        3 branches
            1. c = (SP | HT)
                encounters \r\n(SP|HT), continue building header value
            2. c = \r
                encounters \r\n\r, header ended here
            3. c = valid chars
                encounters \r\n{c}, starts reading a new header name

    */
    if (is_sp(c) || is_ht(c)) {
      state_ = s::req_field_value;
      return status::in_progress;
    }
    if (is_cr(c)) {
      state_ = s::req_header_end;
      return status::in_progress;
    }
    if (is_token(c)) {
      request.headers_.emplace_back();
      request.build_header_name(c);
      state_ = s::req_field_name;
      return status::in_progress;
    }
    return status::reject;
  case s::req_header_end:
    if (is_lf(c)) {
      return status::accept;
    }
    return status::reject;
  default:
    break;
  }
  return status::reject;
}

auto RequestParser::view_state(RequestParser::State state, ParseStatus status,
                               char c) -> void {
  std::cout << "state: " << static_cast<int>(state) << "\tstatus: " << status
            << "\tchar: ";

  if (is_char(c)) {
    if (is_cr(c)) {
      std::cout << "\\r";
    } else if (is_lf(c)) {
      std::cout << "\\n";
    } else {
      std::cout << c;
    }
  } else {
    std::cout << static_cast<int>(c);
  }

  std::cout << std::endl;
}

auto operator<<(std::ostream &strm, ParseStatus &status) -> std::ostream & {
  switch (status) {
  case ParseStatus::accept:
    strm << "[Accept = ";
    break;
  case ParseStatus::reject:
    strm << "[Reject = ";
    break;
  case ParseStatus::in_progress:
    strm << "[In progress = ";
    break;
  default:
    break;
  }
  return strm << static_cast<int>(status) << "]";
}
}
