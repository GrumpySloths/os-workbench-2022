#include <common.h>
#include<spinlock.h>
#ifdef TEST
#include<debug.h>
#endif
static spinlock_t lk;
extern Area heap;

static void *kalloc(size_t size) {
    spin_lock(&lk);
    void*pt=malloc(size);
    spin_unlock(&lk);
    
    return pt;
}

static void kfree(void *ptr) {
    spin_lock(&lk);
    free(ptr);
    spin_unlock(&lk);
}
#ifndef TEST
static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  lk = SPIN_INIT();
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
}
#else
// 测试代码的 pmm_init ()
static void pmm_init() {
  // char *ptr  = malloc(HEAP_SIZE);
  // heap.start = ptr;
  // heap.end   = ptr + HEAP_SIZE;
  printf("it is a test\n");
  printf("Got %d MiB heap: [%p, %p)\n", HEAP_SIZE >> 20, heap.start, heap.end);
}
#endif

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
