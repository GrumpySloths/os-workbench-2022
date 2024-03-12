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