#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;
// 分配区域信息
typedef struct{
    int size;
    int magic; //用于sanity check,检查内存非法访问或重复释放分配等出乎意料问题
} header_t;
// free list node
typedef struct __node_t{
    int size;
    struct __node_t *next;
} node_t;

static node_t *head=NULL;

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
  for (node_t *pt = head; pt != NULL;pt=pt->next){
        Log("%p=>%d", pt, pt->size);
    }
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
      head = (node_t*)heap.start;
      uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
      head->size = pmsize - sizeof(node_t);
      head->next = NULL;
  }
  //考虑先利用first fit进行匹配
  node_t *p,*prevp;
  size_t real_size = sizeof(header_t) + size;
  Log("real_size:%d\n", real_size);
  print_list();
  // 对list遍历，存在符合大小的块就直接分配
  for (p =prevp= head; p != NULL;prevp=p,p=p->next){
    if(p->size>=real_size){
      //块的内存足够大时处理结果
      if(p==head){
        //p指针刚好指向head的处理，需要直接更改head
        head = (node_t *)((uintptr_t)head + real_size);
        head->next = p->next;
        head->size = p->size - real_size;
        //为空出来的空间添加header信息并返回对应的指针
        header_t* header = (header_t*)p;
        header->magic = 1234567;
        header->size = size;
        return (void *)(header + 1);
      }else{
        //p指针指向非head的node处理
        prevp->next = (node_t *)((uintptr_t)p + real_size);
        prevp->next->next = p->next;
        prevp->next->size = p->size - real_size;
        //为空出来的空间添加header信息并返回对应的指针
        header_t* header = (header_t*)p;
        header->magic = 1234567;
        header->size = size;
        return (void *)(header + 1);
      }
    }
  }
  //未找到符合大小的块处理
  panic("there is not enough to allocate\n");
  return NULL;
#endif
  return NULL;
}

void free(void *ptr) { 
  header_t *hptr = (header_t *)ptr - 1;
  assert(hptr->magic == 1234567); //内存完整性检测
  int size = hptr->size;
  //遍历列表，找到要释放的指针该要合并到的区域,如果没有找到插入到链表头部
  for (node_t *pt = head; pt != NULL;pt=pt->next){
    //hptr区间可以添加到区间顶部处理
    if((hptr+sizeof(header_t)+hptr->size)==pt){
      
    }
  } 
  node_t *tmp = head;
  head = (node_t *)hptr;
  head->size = size+sizeof(header_t)-sizeof(node_t);
  head->next = tmp;

}

#endif
