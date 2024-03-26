#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#ifdef TEST
  #include<stdlib.h>
  #include<stdio.h>
  #include <sys/mman.h>
  #include<assert.h>
  #define putch(ch) putchar(ch)
  extern   Area  heap;
  #define MB (1<<20)
#endif
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;
// 分配区域信息
typedef struct __header_t{
    uint32_t size;
    int magic; //用于sanity check,检查内存非法访问或重复释放分配等出乎意料问题
    struct __header_t *next;
} header_t;
// free list node
static header_t*head=NULL;

int rand(void) {
    // RAND_MAX assumed to be 32767
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

static void print_list() {
    Log("--------MEMORYLIST---------");
    for (header_t *pt = head; pt != NULL; pt = pt->next) {
        Log("%p=>%d mb", pt, (pt->size)>>20);
    }
    Log("--------------------------");
}
static int power_count(size_t size) { 
  int count = 0;
  unsigned int power = 1;
    while (power < size) {
        power *= 2;
        count++;
    }
  return count;
}
/**
 * 待实现需求
 * 1. 内存对齐要求
 * 2. 利用slab思想加速malloc
 * 3. 加锁实现并发数据结构
*/
#ifdef TEST
void* mymalloc(size_t size){
#else
void *malloc(size_t size) {
#endif
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  // printf("malloc custom call\n");
  // 当free_list不存在时首先初始化free_list
  if(!head){
    printf("first initialization\n");
    #ifdef TEST
      head = (header_t *)mmap(NULL, size,  PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      uintptr_t pmsize = size;
      heap.start = (void*)head;
      heap.end = (void*)head + pmsize;
      head->size = pmsize - sizeof(header_t);
      head->magic = 1234567;
      head->next = NULL;
      return NULL;
#else
      head = (header_t *)heap.start;
      uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
#endif
      head->size = pmsize - sizeof(header_t);
      head->magic = 1234567;
      head->next = NULL;
  }
  //考虑先利用first fit进行匹配
  header_t*p,*prevp;
  // size_t real_size = sizeof(header_t) + size;
  size_t size_align = 1 << power_count(size);
  assert(size_align >= size);

  size_t real_size = size_align + sizeof(header_t);
  // Log("real_size:%d mb\n", real_size >> 20);
  printf("real_size:%d\n", real_size);
  print_list();
  // 对list遍历，存在符合大小的块就直接分配
  for (p =prevp= head; p != NULL;prevp=p,p=p->next){
    if(p->size>=real_size){
      //内存对齐分配要求实现,可能有点问题当前实现
      uintptr_t p_end = ROUNDDOWN((uintptr_t)p + sizeof(header_t) + p->size,size_align);
      header_t *node = (header_t *)(p_end - real_size);
      // header_t *node =
      //     (header_t *)((uintptr_t)p + sizeof(header_t) + p->size -
      //     real_size);
      node->magic = 1234567;
      // node->size = size;
      node->size = size_align;
      // 更新p 指向block的size
      // p->size -= real_size;
      // p->size = (p_end - real_size - (uintptr_t)p);
      p->size = ((uintptr_t)node-sizeof(header_t)- (uintptr_t)p);
      printf("malloc,%p,%p,%p\n", (void*)(node+1),
             (void*)((uintptr_t)node + real_size), (void*)size_align);
      return (void *)(node + 1);
    }
  }
  //未找到符合大小的块处理
  panic("there is not enough to allocate\n");
  return NULL;
#endif
  return NULL;
}

#ifdef TEST
void myfree(void *ptr){
#else
void free(void *ptr) {
#endif
    // printf("free debug\n");
    header_t *hptr = (header_t *)ptr - 1;
    assert(hptr->magic == 1234567);  // 内存完整性检测
    panic_on(hptr < head, "the pointer to free out of heap");
    size_t real_size = sizeof(header_t) + hptr->size;
    printf("free,%p,%p,%d\n", (void *)(hptr + 1),
           (void *)((uintptr_t)hptr + real_size),
           (hptr->size));
    header_t *cur;
    // 这里维护一个有序的链表，按地址从低到高进行排列
    for (cur = head; !(hptr >cur && hptr < cur->next); cur = cur->next) {
        if (cur > cur->next && (hptr > cur || hptr < cur->next))
            break;  // 此时cur位于列表末端
  }
  //此时hptr一定位于两个块之间或者列表末尾，进行合并操作
  //lower merge
  if(((uintptr_t)hptr+sizeof(header_t)+hptr->size)==(uintptr_t)cur->next){
      hptr->size += sizeof(header_t) + cur->next->size;
      hptr->next = cur->next->next;
  } else {
      hptr->next = cur->next;
  }
  //upper merge
  if(((uintptr_t)cur+sizeof(header_t)+cur->size)==(uintptr_t)hptr){
      cur->size += sizeof(header_t) + hptr->size;
      cur->next = hptr->next;
  } else {
      cur->next = hptr;
  }
}

#endif
