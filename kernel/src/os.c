#include <os.h>
#define MB (1<<20)

#ifndef TEST
uint64_t uptime() { return io_read(AM_TIMER_UPTIME).us/1000; }
#endif

static void producer(void*arg){
    while (1) printf("(");
}
static void consumer(void *arg) {
    while (1) printf(")");
}
static void create_threads() {
  kmt->create(pmm->alloc(sizeof(task_t)),
              "test-thread-1", producer, "xxx");
  kmt->create(pmm->alloc(sizeof(task_t)),
              "test-thread-2", consumer, "yyy");
}

typedef struct{
    int x;
    int y;
} s1_t;
static void os_init() { 
    pmm->init();
    kmt->init();
    create_threads();
}

static void os_run() {
    iset(true); //打开中断
    while(1)
        ; //os-run代表着一个cpu，故该线程不能停止
}

static Context* os_trap(Event ev,Context*ctx){
  extern task_t*tasks[100];
  if (!current_task)
      current_task = tasks[0];
  else          current_task->context = ctx;
  do {
    current_task = current_task->next;
  } while (current_task->id  % cpu_count() != cpu_current());
  return current_task->context;
}
MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
  .trap = os_trap,
};
