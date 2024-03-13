#include <os.h>

#ifdef TEST
static inline int atomic_xchg(volatile int *addr, int newval) {
  int result;
  asm volatile ("lock xchg %0, %1":
    "+m"(*addr), "=a"(result) : "1"(newval) : "memory");
  return result;
}
#endif

//定义对printf的自旋锁
spinlock_t printf_lock;

void kmt_spin_init(spinlock_t *lk, const char *name) { 
    lk->locked = 0;
    lk->name = name;
#ifndef TEST
    lk->cpu = cpu_current();
#endif
}

void kmt_spin_lock(spinlock_t *lk) { 
    while (atomic_xchg(&lk->locked, 1));
#ifndef TEST
    lk->cpu = cpu_current();
#endif
}

void kmt_spin_unlock(spinlock_t *lk) { 
    atomic_xchg(&lk->locked, 0);
}

void kmt_sem_init(sem_t *sem, const char *name, int value) { 
    sem->val = value;
    sem->name = name;
    kmt_spin_init(&sem->lk, name);
    initQueue(&sem->waits);
}

void kmt_sem_wait(sem_t *sem) { 
    while (1) {
        kmt_spin_lock(&sem->lk);
        if (sem->val > 0) {
            sem->val--;
            kmt_spin_unlock(&sem->lk);
            break;
        }
        else {
            // enqueue(&sem->waits, sem->name);
            kmt_spin_unlock(&sem->lk);
#ifndef TEST
            yield();  //执行yield()函数，切换到其他线程
#endif
        }
    }
}

void kmt_sem_signal(sem_t *sem) { 
    kmt_spin_lock(&sem->lk);
    if (isQueueEmpty(&sem->waits)) {
        sem->val++;
    }
    else {
        dequeue(&sem->waits);
        sem->val++;
    }
    kmt_spin_unlock(&sem->lk);
}

static int kmt_create(task_t*task,const char *name, void (*entry)(void *arg), void *arg){
    extern int tasks_id;
    extern task_t* tasks[100];

    // 将task加入到tasks数组中
    tasks[tasks_id] = task;
    void *ptr = NULL;
    int test_kmt = 0;
    test_kmt++;
    // 为task传递参数并分配相应的栈空间
    task->name = name;
    task->entry = entry;
    task->id = tasks_id;
    tasks_id++;
    panic_on(tasks_id>=100,"too many tasks");
    ptr = pmm->alloc(STACK_SIZE);
    task->stack = ptr;
    kmt_spin_lock(&printf_lock);
    printf("task->stack=%p\n",task->stack);
    //打印heap地址
    printf("heap.start=%p,heap.end=%p\n",heap.start,heap.end);
    kmt_spin_unlock(&printf_lock);

    panic_on(!IN_RANGE((void*)task->stack, heap), "stack out of heap range");
    panic_on(task->stack == NULL, "alloc stack failed");
    Area stack=(Area){task->stack,task->stack+STACK_SIZE};
    task->context=kcontext(stack,entry,arg);
    panic_on(task->context==NULL,"kcontext failed");

    return 1;
}

void kmt_init(){
    extern int tasks_id;
    // 初始化tasks数组
    for(int i=0;i<100;i++){
        tasks[i]=NULL;
    }
    // 初始化currents数组
    for(int i=0;i<MAX_CPU;i++){
        currents[i]=NULL;
    }
    tasks_id=0;
    //初始化printf锁
    kmt_spin_init(&printf_lock, "printf_lock");
}

MODULE_DEF(kmt) = {
 // TODO
    .init=kmt_init,
    .spin_init = kmt_spin_init,
    .spin_lock = kmt_spin_lock,
    .spin_unlock = kmt_spin_unlock,
    .sem_init = kmt_sem_init,
    .sem_wait = kmt_sem_wait,
    .sem_signal = kmt_sem_signal,
    .create = kmt_create,
};
