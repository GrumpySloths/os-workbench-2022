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
void ucreate() { 

    extern int tasks_id;
    extern task_t* tasks[100];

    task_t* task = pmm->alloc(sizeof(task_t)); 
    tasks[tasks_id++] = task;

    task->ar=pmm->alloc(sizeof(AddrSpace));

    protect(task->ar);
    
    void* entry = task->ar->area.start;
    //构建堆栈
    Area kstack=(Area){&task->context+1,task+1};
    task->context = ucontext(task->ar, kstack, entry);
}
#endif

void uproc_init() {
    printf("uproc_init\n");
    vme_init(pgalloc, pgfree);

#ifndef VME_V2
    ucreate();
#else
    task_t*task=pmm->alloc(sizeof(task_t));
    //为task->ar分配空间
    task->ar=pmm->alloc(sizeof(AddrSpace));
    protect(task->ar);

    void (*entry)(void *arg) = (void*)task->ar->area.start;
    //打印entry地址
    printf("entry:%p\n",entry);
    
    kmt->create(task, "initcode", entry, NULL);

    // 为task创建相应地址空间映射
    uvmfirst(task->ar,_init,_init_len);
    //为进程创建用户栈
    Context* ctx = task->context;

    map(task->ar,(void*)(ctx->rsp-PAGESIZE),
                        (void*)(ctx->rsp0),MMAP_READ|MMAP_WRITE);
    // ctx->rsp -= 40;
    // ctx->rsp0 -= 40;
    // mappages(task->ar, (void*)ctx->rsp, PAGESIZE,
    //                         (void*)ctx->rsp0, MMAP_WRITE);
#endif 
}

int uproc_kputc(task_t* task, char ch) { 
    putch(ch);
    return 0;
}

int uproc_getpid(task_t*task){
    return task->id;

}

MODULE_DEF(uproc) = {
    .init=uproc_init,
    .kputc=uproc_kputc,
    .getpid=uproc_getpid,
};
