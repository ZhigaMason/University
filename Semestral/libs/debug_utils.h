#ifndef _H_DEBUG_UTILS
#define _H_DEBUG_UTILS

#ifdef DEBUG
        #define DEBUG_OUTPUT(fmt, ...) printf(fmt __VA_OPT__(,) __VA_ARGS__)
#else
        #define DEBUG_OUTPUT(fmt, ...)
#endif

#endif
