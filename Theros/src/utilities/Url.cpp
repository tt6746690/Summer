#include <cstdio>
#include <sstream>

#include "Url.h"
#include "StrUtils.h"

namespace Theros {

/**
 * static strings
 */
static constexpr char unreserved_charset[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";
//static constexpr char reserved_charset[] = "!*'();:@&=+$,/?#[]";
static constexpr char uri_charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmn"
                                  "opqrstuvwxyz0123456789-_.~!*'();:@&=+$,/"
                                  "?#[]";

bool is_uri_unreserved(char c) 
{
  for (auto b = std::begin(unreserved_charset), e = std::end(unreserved_charset); b != e; b++) {
    if (c == *b) return true;
  }
  return false;
}

bool is_uri(char c) 
{
  for (auto b = std::begin(uri_charset), e = std::end(uri_charset); b != e; b++) {
    if (c == *b) return true;
  }
  return false;
}

/**
 * @brief   Converts 1 utf8 byte to its hex value
 */
std::string ctohex(unsigned int c) 
{
  std::ostringstream os;
  os << std::hex << std::uppercase << c;
  return os.str();
}


std::string urlencode(const std::string &url)
{
  if (url.empty())
    return {};

  std::string encoded{};
  for (auto c : url) 
  {
    if (is_uri_unreserved(c)) encoded += c;
    else encoded.append("%" + ctohex(static_cast<unsigned char>(c)));
  }
  return encoded;
}

std::string urldecode(const std::string &url) 
{
  if (url.empty())
    return {};

  std::string decoded{};
  char c;
  int cvt;

  for (auto itr = url.cbegin(); itr != url.cend(); ++itr) {
    c = *itr;
    if (c != '%')
      decoded += c;
    else {
      std::string hexhex{itr + 1, itr + 3};
      sscanf(hexhex.c_str(), "%x", &cvt);
      decoded += static_cast<unsigned char>(cvt);
      itr += 2;
    }
  }
  return decoded;
}

std::unordered_map<std::string, std::string> make_query(const std::string &qstr) 
{
  constexpr char tok_and = '&';
  constexpr char tok_equal = '=';

  std::string query = qstr;
  query += tok_and;

  std::size_t pos = 0;
  std::string token, key, value;
  std::unordered_map<std::string, std::string> query_map;

  while ((pos = query.find(tok_and)) != std::string::npos) {
    token = query.substr(0, pos);
    std::tie(key, value) = split(token, tok_equal);
    query_map.insert({key, value});
    query.erase(0, pos + 1);
  }
  return query_map;
}


} // namespace Theros