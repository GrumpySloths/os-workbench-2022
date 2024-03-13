#include <common.h>

struct task {
    // TODO
    const char* name;
    int id;
    Context* context;
    void  (*entry)(void *);
    void*stack;
    struct task* next;
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
