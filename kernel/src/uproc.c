#include <os.h>
#include <syscall.h>

#include "initcode.inc"

static void* pgalloc(int size){
    panic_on(size%4096!=0,"size must be multiple of 4096");
    void*ptr=pmm->alloc(size);

    return ptr;
}

static void pgfree(void* ptr){
    pmm->free(ptr);
}

void uproc_init() {
    printf("uproc_init\n");
    void (*entry)(void *arg) = (void*)_init;

    kmt->create(pmm->alloc(sizeof(task_t)), "initcode", entry, NULL);
    vme_init(pgalloc, pgfree);
    
}

int uproc_kputc(task_t* task, char ch) { 
    putch(ch);
    return 0;
}

MODULE_DEF(uproc) = {
    .init=uproc_init,
    .kputc=uproc_kputc,
};
