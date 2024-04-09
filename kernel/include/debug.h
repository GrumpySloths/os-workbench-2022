#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define DEBUG
#ifdef PRINT_DEBUG 
#define printk(format, ...) \
    kmt->spin_lock(printf_lock);\
    printf(format, ##__VA_ARGS__);\
    kmt->spin_unlock(printf_lock);
#else
#define printk(format, ...)
#endif

#ifdef PERROR_DEBUG
#define perror(format, ...) \
    kmt->spin_lock(printf_lock);\
    printf("\33[1;35mDebug1: \33[0m" format, ##__VA_ARGS__);\
    kmt->spin_unlock(printf_lock);
#else
#define perror(format, ...)
#endif

#endif