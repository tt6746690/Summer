
#include "Message.h"
#include "Defines.h"  // eol
#include "Utils.h"    // enum_map


namespace Theros
{

std::string version_as_string(HttpVersion v)
{
  switch(v) {
    case HttpVersion::zero_nine: return "HTTP/0.9"; 
    case HttpVersion::one_zero:  return "HTTP/1.0";
    case HttpVersion::one_one:   return "HTTP/1.1";
    case HttpVersion::two_zero:  return "HTTP/2.0";
    default: return "";
  }
}


std::string uri_as_string(const Uri& uri)
{
  std::string s;
  if (uri.scheme.size())    s += uri.scheme + "://" + uri.host;
  if (uri.port.size())      s += ":" + uri.port;
  if (uri.abs_path.size())  s += uri.abs_path;
  if (uri.query.size())     s += "?" + uri.query;
  if (uri.fragment.size())  s += "#" + uri.fragment;
  return s;
}


std::string request_method_as_string(RequestMethod method)
{
  return enum_map(request_methods, method);
}

RequestMethod request_method_from_cstr(const char* method)
{
  switch (method[0]) {
    case 'G': return RequestMethod::GET;
    case 'H': return RequestMethod::HEAD;
    case 'P': {
      switch(method[1]) {
        case 'O': return RequestMethod::POST;
        case 'U': return RequestMethod::PUT;
        case 'A': return RequestMethod::PATCH;
        default: break;
      }
    }
    case 'D': return RequestMethod::DELETE;
    case 'C': return RequestMethod::CONNECT;
    case 'O': return RequestMethod::OPTIONS;
    case 'T': return RequestMethod::TRACE;
    default:  return RequestMethod::UNDETERMINED;
  }
}


std::ostream& operator<<(std::ostream& os, const Request& req)
{
  std::string s;
  s += "> " + request_method_as_string(req.method) + " " + 
    uri_as_string(req.uri) + " " + version_as_string(req.version) + eol;
  os << s;
  for(auto& h : req.headers) { os << "> " << h << eol; }
  os << "> " << req.body << eol;
  return os;
}


std::string Response::ToPayload() const 
{
  std::string payload = StatusLine() + HeaderLine() + body;
  return payload;
}

std::string Response::StatusLine() const
{
  std::string s;
  s += version_as_string(version) + " " + std::to_string(status_code_as_int(status_code)) + " "
    + status_code_as_reason(status_code) + CRLF;
  return s;
}

std::string Response::HeaderLine() const
{
    std::string s;
    for (const auto& header : headers)
        s += header.name + ": " + header.value + CRLF;
    return s + CRLF;
}

std::ostream& operator<<(std::ostream& os, const Response& res) {
  std::string s;
  s += res.StatusLine();
  for(auto& header : res.headers)
    s += "< " + header.name + ": " + header.value + eol;
  s += "< " + res.body + eol;
  return os << s;
}

int status_code_as_int(StatusCode status_code) 
{ 
  return enum_map(status_codes, status_code); 
}
const char* status_code_as_reason(StatusCode status_code) 
{ 
  return enum_map(reason_phrases, status_code); 
}

StatusCode status_code_from_int(int status_code) 
{
  for (int i = 0; i < status_code_count; ++i) {
    if (status_codes[i] == status_code)
      return static_cast<StatusCode>(i);
  }
  return StatusCode::Not_Found;
}

} // namespace Theros
