#ifndef __STRUTILS_H__
#define __STRUTILS_H__

#include <string>


namespace Summer {



// Finds common prefix 
int find_common_prefix_len(const char* x, const char* y);
std::string find_common_prefix(const char* x, const char *y);
std::string find_common_prefix(const std::string& x, const std::string& y);


// Splits
void split_in_half(const std::string& s, int at, std::string& first, std::string& second);




} // namespace Summer 
#endif // __STRUTILS_H__