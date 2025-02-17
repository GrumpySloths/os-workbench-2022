#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define DEBUG
#ifdef DEBUG
#define Log(format, ...) \
    printf("\33[0m[\33[1;35mLog\33[0m]\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)
#else
#define Log(format, ...)
#endif

#endif