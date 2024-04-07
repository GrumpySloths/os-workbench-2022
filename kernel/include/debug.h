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

#endif