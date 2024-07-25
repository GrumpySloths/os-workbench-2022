#include <os.h>
#include <syscall.h>

#ifdef VME_V2
    #include <arch/x86_64-qemu.h>
    #include "initcode.inc"
#endif
typedef unsigned char  uchar;
typedef unsigned int   uint;


static void* pgalloc(int size){
    panic_on(size%4096!=0,"size must be multiple of 4096");
    void*ptr=pmm->alloc(size);

    return ptr;
}

static void pgfree(void* ptr){
    pmm->free(ptr);
}

void canary_init(struct stack* s) { 
    u32* ptr = (u32*)s;
    for (int i = 0; i < CANARY_SZ;i++){
        ptr[BOTTOM - i] = ptr[i] = MAGIC;
    }
}

//根据给定size的大小创建多页映射
// static int mappages(AddrSpace*ar,void*va,uint size,void*pa,int prot){
    
//     if (size == 0)
//         panic("mappages size is 0");

//     //根据size计算要分配的页数
//     int npages=ROUNDUP(size,4096)/4096;
    
//     //构建循环利用map函数进行地址映射
//     for (int i = 0; i < npages;i++){
//         map(ar,va+i*4096,pa+i*4096,prot);
//     } 

//     return 0;
// }
#ifdef VME_V2
//创建第一个用户进程
static void uvmfirst(AddrSpace*ar,uchar*src,uint sz){
    //将sz向上取整为pagesize的整数倍
    uint sz_aligned=ROUNDUP(sz,4096);
    //分配一个物理页
    char*mem=pgalloc(sz_aligned);
    //将mem的所有值赋0
    memset(mem,0,sz_aligned);
    //构建虚拟地址和物理地址的映射
    map(ar,ar->area.start,mem,MMAP_WRITE|MMAP_READ);
    //将src的内容复制到mem中
    memmove(mem,src,sz);

}
#else
int ucreate() { 

    extern int tasks_id;
    extern task_t* tasks[100];

    task_t* task = pmm->alloc(sizeof(task_t)); 
    tasks[tasks_id++] = task;
    
    task->ar=pmm->alloc(sizeof(AddrSpace));

    protect(task->ar);
    
    void* entry = task->ar->area.start;
    //构建堆栈
    // Area kstack=(Area){&task->context+1,task+1};
    Area kstack = (Area){&task->stack, &(task->stack[STACK_SIZE -CANARY_SZ*4- 1])};
    task->context = ucontext(task->ar, kstack, entry);
    //为task的堆栈添加canary保护
    canary_init((struct stack*)&task->stack);

    //检测tasks_id是否超过任务最大数量
    panic_on(tasks_id >= MAX_TASKS, "tasks_id is out of range");

    return tasks_id - 1;
}
#endif

void uproc_init() {
    printf("uproc_init\n");
    vme_init(pgalloc, pgfree);

    ucreate();

}

int uproc_kputc(task_t* task, char ch) { 
    putch(ch);
    return 0;
}

int uproc_getpid(task_t*task){
    return task->id;

}

int uproc_fork(task_t* task,Context*ctx) {
    int child_pid = ucreate(); 
    //修改 tasks[child_pid],使其上下文和内存与父进程一致
    task_t* child = tasks[child_pid];
    void* cr3 = child->ar->ptr;
    uint64_t rsp0 = child->context->rsp0;
    memcpy(child->context, ctx, sizeof(Context));
    child->context->rax = 0;
    child->context->cr3=cr3;
    child->context->rsp0 = rsp0;
    // 复制父进程的地址空间
    for (int i = 0; i < task->page_cnt;i++){
        void*pa=pmm->alloc(PAGESIZE);
        memcpy(pa,task->pa[i],PAGESIZE);
        map(child->ar,task->va[i],pa,MMAP_WRITE|MMAP_READ);
        child->va[i]=task->va[i];
        child->pa[i]=pa;
        child->page_cnt++;
    }
    //更新轮询链表
    for (int i = 0; i < tasks_id;i++){
      tasks[i]->next=tasks[(i+1)%tasks_id];
    } 
    //检查child的context和页表是否被正确的设置
    sane_context(child->context);
    assert(child->page_cnt > 0);

    return child_pid;
}

int uproc_sleep(task_t* task, int seconds) {
    printf("sleep is called\n");
    uint64_t cur_time = io_read(AM_TIMER_UPTIME).us / 1000;

    while(io_read(AM_TIMER_UPTIME).us/1000-cur_time<seconds*1000);

    printf("sleep is over\n");

    return 0;
}

MODULE_DEF(uproc) = {
    .init=uproc_init,
    .kputc=uproc_kputc,
    .getpid=uproc_getpid,
    .fork=uproc_fork,
    .sleep=uproc_sleep,
};
