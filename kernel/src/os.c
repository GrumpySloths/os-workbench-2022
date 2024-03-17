#include <os.h>
#include <devices.h>

#define MB (1<<20)

#ifndef TEST
uint64_t uptime() { return io_read(AM_TIMER_UPTIME).us/1000; }
#endif


static inline task_t *task_alloc() {
  return pmm->alloc(sizeof(task_t));
}
#ifdef TEST1
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

static void concurrency_test1(){
    kmt->sem_init(&empty, "empty", 5);  // 缓冲区大小为 5
    kmt->sem_init(&fill,  "fill",  0);
    for (int i = 0; i < 4; i++) // 4 个生产者
      kmt->create(task_alloc(), "producer", producer, NULL);
    for (int i = 0; i < 5; i++) // 5 个消费者
      kmt->create(task_alloc(), "consumer", consumer, NULL);
}

#else
// static void tty_reader(void *arg) {
//   device_t *tty = dev->lookup(arg);
//   char cmd[128], resp[128], ps[16];
//   snprintf(ps, 16, "(%s) $ ", arg);
//   while (1) {
//     tty->ops->write(tty, 0, ps, strlen(ps));
//     printf("tty start reading\n");
//     int nread = tty->ops->read(tty, 0, cmd, sizeof(cmd) - 1);
//     cmd[nread] = '\0';
//     sprintf(resp, "tty reader task: got %d character(s).\n", strlen(cmd));
//     tty->ops->write(tty, 0, resp, strlen(resp));
//   }
// }

#endif

//注册 timer 中断函数
static Context* irq_timer(Event ev,Context*ctx){
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

//注册 yield中断函数
static Context* irq_yield(Event ev,Context*ctx){
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


static void os_init() { 
    pmm->init();
    kmt->init();
    handlers_id = 0;
    //注册timer中断
    os->on_irq(0, EVENT_IRQ_TIMER, irq_timer);
    //注册yield中断
    os->on_irq(1, EVENT_YIELD, irq_yield);

#ifdef TEST1
    concurrency_test1();
#else
    dev->init();

    // kmt->create(task_alloc(), "tty_reader", tty_reader, "tty1");
    // kmt->create(task_alloc(), "tty_reader", tty_reader, "tty2");
#endif
    //构建轮询链表
    for (int i = 0; i < tasks_id;i++){
      tasks[i]->next=tasks[(i+1)%tasks_id];
    } 


    // kmt->sem_init(&empty, "empty", 1);dev_input_task
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
  Context *next = NULL;
  //遍历handlers,当ev与handlers[i]->event相等时，调用handlers[i]->handler
  for (int i = 0; i < handlers_id;i++){
    if(handlers[i]->event==EVENT_NULL||handlers[i]->event==ev.event){
      Context*r=handlers[i]->handler(ev,ctx);
      panic_on(r && next, "returning multiple contexts");
      if(r)
          next = r;
    }
  } 

  panic_on(!next, "returning NULL context");

  return next;
}

static void os_on_irq(int seq,int event,handler_t handler){
    extern handlerRegister_t* handlers[100];
    // 为h分配空间
    handlerRegister_t* h = pmm->alloc(sizeof(handlerRegister_t));
    h->seq = seq;
    h->handler = handler;
    h->event = event;

    handlers[handlers_id++] = h;
}

MODULE_DEF(os) = {
    .init = os_init, .run = os_run, .trap = os_trap, .on_irq = os_on_irq,
}
;
