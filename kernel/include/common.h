#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>
#include <queue.h>

#define MAX_CPU 8
#define STACK_SIZE 8192
//tasks scheduler
// typedef struct __tasks_scheduler_t{
//     task_t tasks[100];
//     int cur;
// }tasks_scheduler_t;

// tasks_scheduler_t tasks_scheduler;

int tasks_id ;
task_t *tasks[100];

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

