#ifndef SPINLOCK_H__
#define SPINLOCK_H__
#include <stdint.h>
// Spinlock
typedef int spinlock_t;
#define SPIN_INIT() 0
void spin_lock(spinlock_t *);
void spin_unlock(spinlock_t *);

#endif