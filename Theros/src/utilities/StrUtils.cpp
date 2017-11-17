
#include <cstring>
#include "StrUtils.h"


namespace Theros {


std::string  find_common_prefix(const std::string& x, const std::string& y)
{
    typename std::string::const_iterator x_it = x.begin(),  y_it = y.begin();
    while(x_it != x.end() && y_it != y.end()) {
        if(*x_it != *y_it) break;
        ++x_it; ++y_it;
    }
    return std::string(x.begin(), x_it);
}

size_t find_common_prefix_len(const char* x, const char* y) 
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
    size_t len = find_common_prefix_len(x, y);
    return std::string(x, len);
}


void split_in_half(const std::string& s, size_t at, std::string& first, std::string& second)
{
    first = s.substr(0, at);
    second = s.substr(at, s.size());
}


std::pair<std::string, std::string>  split(std::string& s, char delim)
{
  auto pos = s.find(delim);
  return pos != std::string::npos ? std::make_pair(s.substr(0, pos), s.substr(pos + 1)) : std::make_pair("", "");
}


bool has_balanced_bracket(const char* s, int len) 
{
    std::vector<char> stack;

    for(int i = 0; i < len; ++i) {
        switch (s[i]) {
            case '<': stack.push_back('>'); break;
            case '{': stack.push_back('}'); break;
            case '[': stack.push_back(']'); break;
            case '>':
            case '}':
            case ']': {
                if (stack.back() != s[i])
                    return false;
                else
                    stack.erase(stack.end() - 1);
            }
            default: break;
        }
    }
    return stack.size() == 0;
}

void find_route_prefix_unstrict(const char* x,
                                const char* y,
                                int&        x_prefix_len,
                                int&        y_prefix_len,
                                std::vector<std::pair<std::string, std::string>>& kvs) 
{
    if (*x == '\0' || *y == '\0') {
        x_prefix_len = 0; y_prefix_len = 0;
        return;
    }

    const char* prefix = x;
    const char* query = y;
    const char* name;
    const char* value;
    
    while(*x != '\0' && *y != '\0') 
    {
        if (*x == *y) { ++x; ++y; continue; }
        
        if (*x == '<') {
            name  = x+1;
            value = y++;

            while (*x != '>') { ++x; }
            while (*y != '\0' && *y != '/') { ++y; }    // check for end of c_string

            std::string k(name, x-name);
            std::string v(value, y-value);
            kvs.push_back({k, v});

            // x == '>' and y == '/', advance x by 1
            ++x;
        } else {
            x_prefix_len = x - prefix;
            y_prefix_len = y - query;
            return;
        }
    }

    // cases when either one is exhausted
    if(*x ^ *y) {
        x_prefix_len = x - prefix;
        y_prefix_len = y - query;
        return;
    }

    x_prefix_len = (x-prefix+1 > strlen(prefix)) ? strlen(prefix) : x-prefix+1;
    y_prefix_len = y - query;
} 



void find_route_prefix_unstrict(std::string x,
                                std::string y, 
                                int&        x_prefix_len,
                                int&        y_prefix_len,
                                std::vector<std::pair<std::string, std::string>>& kvs) 
{
    return find_route_prefix_unstrict(x.c_str(), y.c_str(), x_prefix_len, y_prefix_len, kvs);
}






} // namespace Theros
