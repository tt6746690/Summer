#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "json.hpp"
#include <unordered_map>

namespace Http {

/**
 * types
 */
using ssmap = std::unordered_map<std::string, std::string>;
using json_type = nlohmann::json;

constexpr char EOL[] = "\r\n";

/**
 * Uri
 */

enum class UriState {
  uri_start,
  uri_scheme,
  uri_slash,
  uri_slash_shash,
  uri_host,
  uri_port,
  uri_abs_path,
  uri_query,
  uri_fragment
};

/**
 * RequestParser
 */
enum class ParseStatus { accept = 1, reject, in_progress };

/**
 * Requests
 */

enum class RequestMethod {
  GET = 0,
  HEAD,
  POST,
  PUT,
  PATCH,
  DELETE,
  CONNECT,
  OPTIONS,
  TRACE,
  UNDETERMINED
};

constexpr int method_count = static_cast<int>(RequestMethod::UNDETERMINED) -
                             static_cast<int>(RequestMethod::GET);

constexpr static char *request_methods[] = {
    (char *)"GET",     (char *)"HEAD",    (char *)"POST",
    (char *)"PUT",     (char *)"PATCH",   (char *)"DELETE",
    (char *)"CONNECT", (char *)"OPTIONS", (char *)"TRACE"};

enum class RequestHeaderName {
  Accept = 0,
  Accept_Charset,
  Accept_Encoding,
  Accept_Language,
  Access_Control_Request_Method,
  Access_Control_Request_Headers,
  Authorization,
  Cache_Control,
  Connection,
  Cookie,
  Content_Length,
  Content_Encoding,
  Content_Type,
  Date,
  Expect,
  Forwarded,
  From,
  Host,
  If_Match,
  If_Modified_Since,
  If_None_Match,
  If_Range,
  If_Unmodified_Since,
  Max_Forwards,
  Origin,
  Pragma,
  Proxy_Authorization,
  Range,
  Referer,
  TE,
  User_Agent,
  Upgrade,
  Via,
  Warning
};

/**
 * Response
 */

enum class StatusCode {
  Continue = 0,
  Switching_Protocols,
  OK,
  Created,
  Accepted,
  Non_Authoritative_Information,
  No_Content,
  Reset_Content,
  Partial_Content,
  Multiple_Choices,
  Moved_Permanently,
  Found,
  See_Other,
  Not_Modified,
  Use_Proxy,
  Temporary_Redirect,
  Bad_Request,
  Unauthorized,
  Payment_Required,
  Forbidden,
  Not_Found,
  Method_Not_Allowed,
  Not_Acceptable,
  Proxy_Authentication_Required,
  Request_Timeout,
  Conflict,
  Gone,
  Length_Required,
  Precondition_Failed,
  Request_Entity_Too_Large,
  Request_URI_Too_Large,
  Unsupported_Media_Type,
  Requested_Range_Not_Satisfiable,
  Expectation_Failed,
  Internal_Server_Error,
  Not_Implemented,
  Bad_Gateway,
  Service_Unavailable,
  Gateway_Timeout,
  HTTP_Version_Not_Supported
};

constexpr int status_code_count =
    static_cast<int>(StatusCode::HTTP_Version_Not_Supported) -
    static_cast<int>(StatusCode::Continue) + 1;

enum class ResponseHeaderName {
  Access_Control_Allow_Origin,
  Access_Control_Allow_Credentials,
  Access_Control_Expose_Headers,
  Access_Control_Max_Age,
  Access_Control_Allow_Methods,
  Access_Control_Allow_Headers,
  Accept_Patch,
  Accept_Ranges,
  Age,
  Allow,
  Alt_Svc,
  Cache_Control,
  Connection,
  Content_Disposition,
  Content_Encoding,
  Content_Language,
  Content_Length,
  Content_Location,
  Content_Range,
  Content_Type,
  Date,
  ETag,
  Expires,
  Last_Modified,
  Link,
  Location,
  P3P,
  Pragma,
  Proxy_Authenticate,
  Public_Key_Pins,
  Retry_After,
  Server,
  Set_Cookie,
  Strict_Transport_Security,
  Trailer,
  Transfer_Encoding,
  Tk,
  Upgrade,
  Vary,
  Via,
  Warning,
  WWW_Authenticate
};

constexpr static int status_codes[] = {
    100, 101, 200, 201, 202, 203, 204, 205, 206, 300, 301, 302, 303, 304,
    305, 307, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411,
    412, 413, 414, 415, 416, 417, 500, 501, 502, 503, 504, 505};

constexpr static char *reason_phrases[] = {
    (char *)"Continue",
    (char *)"Switching Protocols",
    (char *)"OK",
    (char *)"Created",
    (char *)"Accepted",
    (char *)"Non_Authoritative Information",
    (char *)"No Content",
    (char *)"Reset Content",
    (char *)"Partial Content",
    (char *)"Multiple Choices",
    (char *)"Moved Permanently",
    (char *)"Found",
    (char *)"See Other",
    (char *)"Not Modified",
    (char *)"Use Proxy",
    (char *)"Temporary Redirect",
    (char *)"Bad Request",
    (char *)"Unauthorized",
    (char *)"Payment Required",
    (char *)"Forbidden",
    (char *)"Not Found",
    (char *)"Method Not Allowed",
    (char *)"Not Acceptable",
    (char *)"Proxy Authentication Required",
    (char *)"Request Time-out",
    (char *)"Conflict",
    (char *)"Gone",
    (char *)"Length Required",
    (char *)"Precondition Failed",
    (char *)"Request Entity Too Large",
    (char *)"Request URI Too Large",
    (char *)"Unsupported Media Type",
    (char *)"Requested Range NotSatisfiable",
    (char *)"Expectation Failed",
    (char *)"Internal Server Error",
    (char *)"Not Implemented",
    (char *)"Bad Gateway",
    (char *)"Service Unavailable",
    (char *)"Gateway Timeout",
    (char *)"HTTP Version Not Supported"};
}

#endif