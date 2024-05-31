#include <common.h>

union task {
    // TODO
    struct {
        const char* name;
        int id;
        void  (*entry)(void *);
        void* arg;
        AddrSpace* ar; //用户地址空间
        int page_cnt;//用户地址空间的页数
        void* va[64];
        void* pa[64];
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
    //interrupt state
    bool i;
};

struct semaphore {
    // TODO
    const char* name;
    int val;
    Queue waits;
    struct spinlock lk;//semaphore's spin lock
};
