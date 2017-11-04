
#include <cstring>
#include "StrUtils.h"


namespace Summer {


std::string find_common_prefix(const std::string& x, const std::string& y)
{
    typename std::string::const_iterator x_it = x.begin(),  y_it = y.begin();
    while(x_it != x.end() && y_it != y.end()) {
        if(*x_it != *y_it) break;
        ++x_it; ++y_it;
    }
    return std::string(x.begin(), x_it);
}

int find_common_prefix_len(const char* x, const char* y) 
{
    const char *begin = x;
    while(*x != '\0' && *y != '\0') {
        if(*x != *y)  break;
        ++x; ++y;
    }
    return x - begin;
}


std::string find_common_prefix(const char* x, const char *y)
{
    int len = find_common_prefix_len(x, y);
    return std::string(x, len);
}



} // namespace Summer
