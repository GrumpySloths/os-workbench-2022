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

MODULE_DEF(uproc) = {
    .init=uproc_init,
};
