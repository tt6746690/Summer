#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <cstdio>


#include "json.hpp"
using JsonType = nlohmann::json;


#define eol '\n'
#define MARK_USED(x) (x=x)
#define __DO_NOTHING__ do{} while(0);


#ifdef _DEBUG_
    #define __FORMAT__(fmt, args...) \
        do { \
            fprintf(stderr, "%s (%s, line %d)\n" fmt, __func__, __FILE__, __LINE__ , ## args); \
        } while(0)
    #define NO_REACH __FORMAT__("should not reach here");
#else
    #define __FORMAT__(fmt, args...) __DO_NOTHING__
    #define NO_REACH __DO_NOTHING__
#endif


#ifdef _DEBUG_
    #define ASSERT(expr) do { if(expr) { __FORMAT__(## format) }  } while(0) 
#else
    #define ASSERT(expr) __DO_NOTHING__
#endif


#endif // __DEFINES_H__