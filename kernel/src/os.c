#include <os.h>
#include <devices.h>

#define MB (1<<20)
#define FL_IF          0x00000200  // Interrupt Enable

#ifndef TEST
uint64_t uptime() { return io_read(AM_TIMER_UPTIME).us/1000; }
#endif

#ifdef DEV_TIMER_TRACE
static int trace_counter = 0;
#define TRACE_ENTRY printf("[trace %d] %s:entry\n", trace_counter, __func__);
#define TRACE_EXIT printf("[trace %d] %s:exit\n", trace_counter, __func__);\
  trace_counter++;
#else
#define TRACE_ENTRY ((void)0)
#define TRACE_EXIT ((void)0)
#endif

//trace iodev
#ifdef DEV_IODEV_TRACE
static int iodev_trace_counter = 0;
#define IODEV_TRACE_ENTRY printf("[iodev trace %d] %s:entry\n" \
                                  , iodev_trace_counter, __func__);
#define IODEV_TRACE_EXIT printf("[iodev trace %d] %s:exit\n"\
                                , iodev_trace_counter, __func__);\
                                  iodev_trace_counter++;
#else
#define IODEV_TRACE_ENTRY ((void)0)
#define IODEV_TRACE_EXIT ((void)0)
#endif

#ifndef VME_DEBUG
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
static void tty_reader(void *arg) {
    TRACE_ENTRY;
    device_t* tty = dev->lookup(arg);
    char cmd[128], resp[128], ps[16];
    snprintf(ps, 16, "(%s) $ ", arg);
    while (1) {
        tty->ops->write(tty, 0, ps, strlen(ps));
        printf("tty start reading\n");
        int nread = tty->ops->read(tty, 0, cmd, sizeof(cmd) - 1);
        cmd[nread] = '\0';
        sprintf(resp, "tty reader task: got %d character(s).\n", strlen(cmd));
        tty->ops->write(tty, 0, resp, strlen(resp));
    }
    TRACE_EXIT;
}

#endif
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

//注册 iodev中断函数
static Context* irq_iodev(Event ev,Context*ctx){
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

//注册syscall中断函数
static Context* irq_syscall(Event ev,Context*ctx){

  //根据ctx.GPR1 来执行相应的系统调用
  switch(ctx->rax){
    case SYS_exit:
      printf("syscall exit\n");
      break;
    case SYS_kputc:
      uproc->kputc(current_task,ctx->rdi);
      break;
    default:
      //打印rdi的值
      printf("rdi:%d\n",ctx->rdi);
      printf("undefined syscall,program exit\n");
      panic("undefined syscall");
      break;
  }
  return ctx;
}

//检测返回的context是否合法
static bool sane_context(Context* ctx) { 
  return (ctx->rflags & FL_IF) == 0; 
}

static void os_init() { 
    pmm->init();
    kmt->init();
    uproc->init();

    handlers_id = 0;
    //注册timer中断
    os->on_irq(0, EVENT_IRQ_TIMER, irq_timer);
    //注册yield中断
    os->on_irq(1, EVENT_YIELD, irq_yield);
    //注册iodev中断
    os->on_irq(2, EVENT_IRQ_IODEV, irq_iodev);
    //注册syscall中断
    os->on_irq(3, EVENT_SYSCALL, irq_syscall);

#ifndef VME_DEBUG
#ifdef TEST1
    concurrency_test1();
#else
    dev->init();

    kmt->create(task_alloc(), "tty_reader", tty_reader, "tty1");
    kmt->create(task_alloc(), "tty_reader", tty_reader, "tty2");
#endif
#endif

    //构建轮询链表
    for (int i = 0; i < tasks_id;i++){
      tasks[i]->next=tasks[(i+1)%tasks_id];
    } 


  }

static void os_run() {
    printf("os_run start\n");
    iset(true);  // 打开中断
    yield();
    while (1)
        ; //os-run代表着一个cpu，故该线程不能停止
}

// static int msg_counter = 0;
static Context* os_trap(Event ev, Context* ctx) {
    Context* next = NULL;
    // 遍历handlers,当ev与handlers[i]->event相等时，调用handlers[i]->handler
    for (int i = 0; i < handlers_id; i++) {
        if (handlers[i]->event == EVENT_NULL ||
            handlers[i]->event == ev.event) {
            Context* r = handlers[i]->handler(ev, ctx);
            panic_on(r && next, "returning multiple contexts");
            if (r)
                next = r;
        }
    }
    // 打印当前的event msg
    printf("event  %s\n", ev.msg);

    // 如果是timer中断打印该信息
#ifdef DEV_TIMER_TRACE
  if(ev.event==EVENT_IRQ_TIMER){
      TRACE_ENTRY;
      printf("event:%s\n", ev.msg);
      TRACE_EXIT;
  }
#endif

#ifdef DEV_IODEV_TRACE
  //如果是iodev中断打印该信息
  if(ev.event==EVENT_IRQ_IODEV){
      IODEV_TRACE_ENTRY;
      printf("event:%s\n", ev.msg);
      IODEV_TRACE_EXIT;
  }
#endif

  panic_on(!next, "returning NULL context");
  //检查中断是否开启
  panic_on(sane_context(next), "interrupt is closed");

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
