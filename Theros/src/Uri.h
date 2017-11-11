#ifndef __URI_H__
#define __URI_H__

#include <iosfwd>
#include <string>
#include <Constants.h>

namespace Theros
{

// forward declare
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

} // namespace Theros
#endif // __URI_H__