#include <common.h>
#ifdef TEST
#include<debug.h>
#endif
extern Area heap;
static int lk = 0;
void lock()   { while (atomic_xchg(&lk, 1)); }
void unlock() { atomic_xchg(&lk, 0); }

static void *kalloc(size_t size) {
    lock(&lk);
    void*pt=malloc(size);
    unlock(&lk);
    
    return pt;
}

static void *kalloc_safe(size_t size) {
  bool i = ienabled();
  iset(false);
  void *ret = kalloc(size);
  if (i) iset(true);
  return ret;
}

static void kfree(void *ptr) {
    lock(&lk);
    free(ptr);
    unlock(&lk);
}

static void kfree_safe(void *ptr) {
  int i = ienabled();
  iset(false);
  kfree(ptr);
  if (i) iset(true);
}
#ifndef TEST
static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("pmsize:%d\n",pmsize);
  lk = 0;
  Log("cpu_count:%d",cpu_count());
  Log("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
}
#else
// 测试代码的 pmm_init ()
static void pmm_init() {
  char *ptr  = malloc(HEAP_SIZE);
  // heap.start = ptr;
  // heap.end   = ptr + HEAP_SIZE;
  printf("it is a test\n");
  printf("Got %d MiB heap: [%p, %p)\n", HEAP_SIZE >> 20, heap.start, heap.end);
}
#endif

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc_safe,
  .free  = kfree_safe,
};
