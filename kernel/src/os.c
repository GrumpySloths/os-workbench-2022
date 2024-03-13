#include <os.h>
#define MB (1<<20)

#ifndef TEST
uint64_t uptime() { return io_read(AM_TIMER_UPTIME).us/1000; }
#endif
sem_t empty, fill;
static void producer(void* arg) {
    while (1){
        kmt->sem_wait(&empty);
        printf("(");
        kmt->sem_signal(&fill);
    } 
}
static void consumer(void *arg) {
    while (1){
        kmt->sem_wait(&fill);
        printf(")");
        kmt->sem_signal(&empty);
    } 
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
    for (int i = 0; i < tasks_id;i++){
      tasks[i]->next=tasks[(i+1)%tasks_id];
    } 
    kmt->sem_init(&empty, "empty", 1);
    kmt->sem_init(&fill, "fill", 0);
  }

static void os_run() {
    printf("os_run start\n");
    iset(true);  // 打开中断
    yield();
    while (1)
        ; //os-run代表着一个cpu，故该线程不能停止
}

static Context* os_trap(Event ev,Context*ctx){
  extern task_t*tasks[100];
  if (!current_task)
      current_task = tasks[0];
  else          current_task->context = ctx;
  do {
    current_task = current_task->next;
    panic_on(!current_task, "no task");
  } while (current_task->id % cpu_count() != cpu_current());
  return current_task->context;
}
MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
  .trap = os_trap,
};
