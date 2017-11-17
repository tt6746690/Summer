#ifndef __STRUTILS_H__
#define __STRUTILS_H__

#include <string>
#include <utility>
#include <vector>

namespace Theros {


// Finds common prefix 
size_t      find_common_prefix_len(const char* x, const char* y);
std::string find_common_prefix(const char* x, const char *y);
std::string find_common_prefix(const std::string& x, const std::string& y);

// Splits
void split_in_half(const std::string& s, size_t at, std::string& first, std::string& second);
auto split(std::string& s, char delim) -> std::pair<std::string, std::string>;

// Check c string has balanced brackets <>, {}, []
bool has_balanced_bracket(const char* s, int len);


/**
 *  Find common prefix of x, the route path, and y, the query path
 *      -- Use / as deliminators, /< name >[/] matches with any string /value/
 *      -- (name, value) pair is stored in kvs, 
 *      -- x_prefix_len and y_prefix_len is modified to represent length of string consumed during prefix match
 *  Assumptions: y cannot contain '<'; x has balanced brackets
 */
void find_route_prefix_unstrict(const char* x,              // route
                                const char* y,              // query path 
                                int&        x_prefix_len,   // length of x consumed during matching
                                int&        y_prefix_len,   // length of y consumed during matching
                                std::vector<std::pair<std::string, std::string>>& kvs);

void find_route_prefix_unstrict(std::string& x,             // route
                                std::string& y,             // query path 
                                int&        x_prefix_len,   // length of x consumed during matching
                                int&        y_prefix_len,   // length of y consumed during matching
                                std::vector<std::pair<std::string, std::string>>& kvs);



} // namespace Theros
#endif // __STRUTILS_H__