#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>
#include <queue.h>
#include<syscall.h>
#include<user.h>
#include<debug.h>

typedef uint32_t u32;

#define PAGESIZE 4096
#define MAX_CPU 8
#define STACK_SIZE 8192
#define CANARY_SZ 16
#define MAGIC 0x55555555
#define BOTTOM (STACK_SIZE/sizeof(u32)-1) 
#define SPIN_LIMIT (1e9)
#define MAX_TASKS 100

//tasks scheduler
// typedef struct __tasks_scheduler_t{
//     task_t tasks[100];
//     int cur;
// }tasks_scheduler_t;

// tasks_scheduler_t tasks_scheduler;

//定义对printf的自旋锁
spinlock_t* printf_lock;

int tasks_id ;
task_t *tasks[MAX_TASKS];

task_t *currents[MAX_CPU];
#define current_task currents[cpu_current()]

//构建一个数据结构 保存 int seq,int event ,handler_t handler
typedef struct  __handlerRegister_t{
    int seq;
    int event;
    handler_t handler;
} handlerRegister_t;
handlerRegister_t *handlers[100];

int handlers_id;

