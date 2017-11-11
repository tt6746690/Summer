#ifndef __URL_H__
#define __URL_H__ 

#include <string> 
#include <unordered_map>

namespace Theros {

// Converts char to hex digits
std::string ctohex(unsigned int c);


// Check if char is valid uri character or not
bool is_uri(char c);
bool is_uri_unreserved(char c);

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
std::string urlencode(const std::string &url);
/**
 * @brief   decode url
 *
 * @precond assumes url consists of uri allowed charset
 */
std::string urldecode(const std::string &url);

/**
 * @brief   Convert a query string to a map of key-value pairs
 */
std::unordered_map<std::string, std::string> make_query(const std::string &query);


} // namespace Theros
#endif // __URL_H__