#ifndef SPINLOCK_H__
#define SPINLOCK_H__
#include <stdint.h>
// Spinlock
typedef int spinlock_t;
#define SPIN_INIT() 0
void spin_lock(spinlock_t *);
void spin_unlock(spinlock_t *);
static inline int atomic_xchg(volatile int *addr, int newval) {
    int result;
    asm volatile("lock xchg %0, %1"
                 : "+m"(*addr), "=a"(result)
                 : "1"(newval)
                 : "memory");
    return result;
}
#endif