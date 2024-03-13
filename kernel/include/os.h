#include <common.h>

union task {
    // TODO
    struct {
        const char* name;
        int id;
        void  (*entry)(void *);
        void* arg;
        union task* next;
        Context* context;
    };
    uint8_t stack[STACK_SIZE];

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
