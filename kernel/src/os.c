#include <os.h>
#include <devices.h>
#ifndef VME_V2
#include  "initcode.inc"
#endif

#define MB (1<<20)


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
  panic_on(ienabled(), "interrupt is open");
  extern task_t*tasks[100];
  if (!current_task)
      current_task = tasks[0];
  else          current_task->context = ctx;
  do {
    current_task = current_task->next;
    panic_on(!current_task, "no task");
  } while (current_task->pid % cpu_count() != cpu_current());
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
  } while (current_task->pid % cpu_count() != cpu_current());
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
  } while (current_task->pid % cpu_count() != cpu_current());
  return current_task->context;
}

//注册syscall中断函数
static Context* irq_syscall(Event ev,Context*ctx){
    panic_on(ienabled(), "interrupt is open");
    // 尝试在syscall中断函数中关闭中断
    //  iset(false);
    //  根据ctx.GPR1 来执行相应的系统调用
    switch (ctx->rax) {
    case SYS_exit:
      printf("syscall exit\n");
      break;
    case SYS_kputc:
      ctx->rax=uproc->kputc(current_task,ctx->rdi);
      break;
    case SYS_getpid:
      uproc->getpid(current_task);
      break;
    case SYS_fork:
        panic_on(ienabled(), "interrupt is open");
        ctx->rax = uproc->fork(current_task , ctx);
        break;
    case SYS_sleep:
        panic_on(ienabled(), "interrupt is open");
        ctx->rax = uproc->sleep(current_task, ctx->rdi);
        // ctx->rax = 0;
        break;
    default:
        // 打印rdi的值
        printf("rdi:%d\n", ctx->rdi);
        printf("undefined syscall,program exit\n");
        panic("undefined syscall");
        break;
  }

  // iset(true);
  panic_on(ienabled(), "interrupt is open");
  return ctx;
}

//注册pagefault中断函数
static Context* irq_pagefault(Event ev,Context*ctx){
  //打印pagefault信息
  printf("fault_addr:%p,cause:%p,cur_rip:%p\n",
                              ev.ref,ev.cause,ctx->rip);
  printf("err msg:%s\n",ev.msg);
  void*pa=pmm->alloc(PAGESIZE);
  void*va=(void*)ROUNDDOWN(ev.ref,PAGESIZE);

  current_task->va[current_task->page_cnt]=va;
  current_task->pa[current_task->page_cnt] = pa;
  current_task->page_cnt++;

  if(current_task->page_cnt==1&& current_task->pid==1){
      //针对第一个进程的特殊处理
      unsigned char* src = _init;
      unsigned int len = _init_len;
      //从src复制len长度内容到pa
      memcpy(pa,src,len);
      map(current_task->ar,va,pa,MMAP_WRITE|MMAP_READ);
      kmt->spin_lock(printf_lock);
      printf("mapping:%p->%p\n", va, pa);
      kmt->spin_unlock(printf_lock);
  } else {
      map(current_task->ar,va,pa,MMAP_WRITE|MMAP_READ);
      kmt->spin_lock(printf_lock);
      printf("mapping:%p->%p\n", va, pa);
      kmt->spin_unlock(printf_lock);
  }
  // panic("pagefault");
  return ctx;
}

//检测返回的context是否合法
bool sane_context(Context* ctx) { 

  if ((ctx->rflags & FL_IF) == 0){
    perror("interrupt is closed");
    printf("interrupt is closed\n");
    // iset(true);
    // ctx->rflags|=FL_IF;
    return true;
  }
  if(ctx->cr3==(void*)KERNEL_PAGETABLE){
    perror("cr3 is not set");
    printf("cr3 is not set\n");
    return true;
  }
  //检查rip 是否在用户空间
  if(ctx->rip<KERNEL_BOUND){
    perror("rip is not in user space");
    printf("rip is not in user space\n");
    return true;
  }

  return false;
}

//检查堆栈是否溢出
void canary_check(struct stack* s) { 
    u32* ptr = (u32*)s;
    for (int i = 0; i < CANARY_SZ;i++){
        panic_on(ptr[BOTTOM - i] != MAGIC, "stack underflow");
        panic_on(ptr[i]!=MAGIC,"stack overflow");
    }
}
//打印context的通用寄存器
static void print_context(Context*ctx){
  // unsigned long rsp_value;
  // asm("movq %%rsp, %0" : "=r" (rsp_value));
  // perror("RSP register value: 0x%lx\n", rsp_value);

  perror("ctx.rsp:%p\n",ctx->rsp);
  perror("ctx.rsp0:%p\n",ctx->rsp0);
  perror("ctx.rip:%p\n",ctx->rip);
  perror("ctx.rflags:%p\n",ctx->rflags);
  perror("ctx.rdi:%p\n",ctx->rdi);
  perror("ctx.cr3:%p\n",ctx->cr3);
  //打印当前cpu所执行task的context的上述寄存器的值
  perror("current_task->context.rsp:%p\n",current_task->context->rsp);
  perror("current_task->context.rsp0:%p\n",current_task->context->rsp0);
  perror("current_task->context.rip:%p\n",current_task->context->rip);
  perror("current_task->context.rflags:%p\n",current_task->context->rflags);  
  perror("current_task->context.rdi:%p\n",current_task->context->rdi);
  perror("current_task->context.cr3:%p\n",current_task->context->cr3);

}

static void os_init() { 
    pmm->init();
    kmt->init();

    handlers_id = 0;
    //注册timer中断
    os->on_irq(0, EVENT_IRQ_TIMER, irq_timer);
    //注册yield中断
    os->on_irq(1, EVENT_YIELD, irq_yield);
    //注册iodev中断
    os->on_irq(2, EVENT_IRQ_IODEV, irq_iodev);
    //注册syscall中断
    os->on_irq(3, EVENT_SYSCALL, irq_syscall);
    //注册pagefault中断
    os->on_irq(4, EVENT_PAGEFAULT, irq_pagefault);

#ifndef VME_DEBUG
    printk("kmt module test\n");
    #ifdef TEST1
        concurrency_test1();
    #else
        dev->init();

        kmt->create(task_alloc(), "tty_reader", tty_reader, "tty1");
        kmt->create(task_alloc(), "tty_reader", tty_reader, "tty2");
    #endif
#else
  uproc->init();
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
    
    // 打印当前的event msg
    printk("event %d: %s\n", ev.event, ev.msg);
    //打印寄存器现场
    print_context(ctx);
    //检查current_task是否合法
    // if(current_task){
    //   panic_on(sane_context(current_task->context), "current task is invalid");
    // }
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
    // printk("event %d: %s\n", ev.event, ev.msg);

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
  panic_on(sane_context(next), "returning to invalid context");
  //检查堆栈是否溢出
  canary_check((struct stack*)&(current_task->stack));
  //检查当前中断是否开启
  panic_on(ienabled(), "interrupt is open");
  // printf("next context:\n");
  // print_context(next);
  
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
