#include <os.h>
#include <devices.h>

#define MB (1<<20)

#ifndef TEST
uint64_t uptime() { return io_read(AM_TIMER_UPTIME).us/1000; }
#endif

// sem_t empty, fill;

static inline task_t *task_alloc() {
  return pmm->alloc(sizeof(task_t));
}

// static void producer(void* arg) {
//     while (1){
//         kmt->sem_wait(&empty);
//         printf("(");
//         kmt->sem_signal(&fill);
//     } 
// }
// static void consumer(void *arg) {
//     while (1){
//         kmt->sem_wait(&fill);
//         printf(")");
//         kmt->sem_signal(&empty);
//     } 
// }
// static void create_threads() {
//   kmt->create(pmm->alloc(sizeof(task_t)),
//               "test-thread-1", producer, "xxx");
//   kmt->create(pmm->alloc(sizeof(task_t)),
//               "test-thread-2", consumer, "yyy");
// }

// static void concurrency_test1(){
//     kmt->sem_init(&empty, "empty", 5);  // 缓冲区大小为 5
//     kmt->sem_init(&fill,  "fill",  0);
//     for (int i = 0; i < 4; i++) // 4 个生产者
//       kmt->create(task_alloc(), "producer", producer, NULL);
//     for (int i = 0; i < 5; i++) // 5 个消费者
//       kmt->create(task_alloc(), "consumer", consumer, NULL);
// }

static void tty_reader(void *arg) {
  device_t *tty = dev->lookup(arg);
  char cmd[128], resp[128], ps[16];
  snprintf(ps, 16, "(%s) $ ", arg);
  while (1) {
    tty->ops->write(tty, 0, ps, strlen(ps));
    int nread = tty->ops->read(tty, 0, cmd, sizeof(cmd) - 1);
    cmd[nread] = '\0';
    sprintf(resp, "tty reader task: got %d character(s).\n", strlen(cmd));
    tty->ops->write(tty, 0, resp, strlen(resp));
  }
}


static void os_init() { 
    pmm->init();
    kmt->init();
    // create_threads();

    kmt->create(task_alloc(), "tty_reader", tty_reader, "tty1");
    kmt->create(task_alloc(), "tty_reader", tty_reader, "tty2");

    //构建轮询链表
    for (int i = 0; i < tasks_id;i++){
      tasks[i]->next=tasks[(i+1)%tasks_id];
    } 


    // kmt->sem_init(&empty, "empty", 1);
    // kmt->sem_init(&fill, "fill", 0);
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
