#include <common.h>


#define FL_IF          0x00000200  // Interrupt Enable
#define KERNEL_PAGETABLE 0x1000
#define KERNEL_BOUND 0x008000000000

bool sane_context(Context* ctx);
struct task {
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
        struct task* next;
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
