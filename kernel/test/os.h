#include <common.h>

struct task {
    // TODO
};

struct spinlock {
    // TODO
    int locked;
    int cpu;
    const char* name;
};

struct semaphore {
    // TODO
    const char* name;
    int val;
    Queue waits;
    struct spinlock lk;//semaphore's spin lock
};
