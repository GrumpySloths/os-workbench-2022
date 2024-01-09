#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;
// 分配区域信息
typedef struct __header_t{
    int size;
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
/**
 * 待实现需求
 * 1. 内存对齐要求
 * 2. 利用slab思想加速malloc
 * 3. 加锁实现并发数据结构
*/
void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  //当free_list不存在时首先初始化free_list
  if(!head){
      printf("first initialization\n");
      head = (header_t *)heap.start;
      uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
      head->size = pmsize - sizeof(header_t);
      head->magic = 1234567;
      head->next = NULL;
  }
  //考虑先利用first fit进行匹配
  header_t*p,*prevp;
  size_t real_size = sizeof(header_t) + size;
  Log("real_size:%d mb\n", real_size>>20);
  print_list();
  // 对list遍历，存在符合大小的块就直接分配
  for (p =prevp= head; p != NULL;prevp=p,p=p->next){
    if(p->size>=real_size){
        header_t *node =
            (header_t *)((uintptr_t)p + sizeof(header_t) + p->size - real_size);
        node->magic = 1234567;
        node->size = size;
        // 更新p 指向block的size
        p->size -= real_size;
        return (void *)(node + 1);
    }
  }
  //未找到符合大小的块处理
  panic("there is not enough to allocate\n");
  return NULL;
#endif
  return NULL;
}

void free(void *ptr) {
    // printf("free debug\n");
    header_t *hptr = (header_t *)ptr - 1;
    assert(hptr->magic == 1234567);  // 内存完整性检测
    panic_on(hptr < head, "the pointer to free out of heap");
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
