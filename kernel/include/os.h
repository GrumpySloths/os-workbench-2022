#include <common.h>

struct task {
    // TODO
};

struct spinlock {
    // TODO
    int locked;
    int cpu;
    char* name;
};

struct semaphore {
    // TODO
    char* name;
    int val;
    Queue waits;
    int lk;//semaphore's spin lock
};
