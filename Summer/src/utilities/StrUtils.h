#ifndef __STRUTILS_H__
#define __STRUTILS_H__

#include <string>


namespace Summer {


// Finds common prefix 
int     find_common_prefix_len(const char* x, const char* y);
std::string find_common_prefix(const char* x, const char *y);
std::string find_common_prefix(const std::string& x, const std::string& y);

// Splits
void split_in_half(const std::string& s, int at, std::string& first, std::string& second);
auto split(std::string& s, char delim) -> std::pair<std::string, std::string>;

// compile time operations
constexpr bool c_streq(const char* x, const char* y) 
{ 
    while(*x && *y) { if (*x++ != *y++) return false; }
    return !(*x || *y) ? true : false;
}
constexpr size_t c_strlen(const char *s) 
{
    size_t len = 0;
    if(s) { while(*s++) ++len; }
    return len;
}


} // namespace Summer 
#endif // __STRUTILS_H__