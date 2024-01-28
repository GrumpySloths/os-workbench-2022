#ifndef MALLOC_H__
#define MALLOC_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#define HEAP_START 0x100000
#define HEAP_SIZE 0x100000
// #define DEBUG
#ifdef DEBUG
#define Log(format, ...)                                                       \
    printf("\33[0m[\33[1;35mLog\33[0m]\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
           __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define Log(format, ...)
#endif
// Memory area for [@start, @end)
typedef struct {
    void *start, *end;
} Area;

Area heap = (Area){.start = (void *)HEAP_START,
                   .end = (void *)(HEAP_START + HEAP_SIZE)};

typedef struct __header_t {
    uint32_t size;
    int magic;  // 用于sanity check,检查内存非法访问或重复释放分配等出乎意料问题
    struct __header_t *next;
} header_t;
// free list node
static header_t *head = NULL;

static void print_list() {
    Log("--------MEMORYLIST---------");
    for (header_t *pt = head; pt != NULL; pt = pt->next) {
        Log("%p=>%d mb", pt, (pt->size) >> 20);
    }
    Log("--------------------------");
}
static void *malloc(size_t size) {
    // On native, malloc() will be called during initializaion of C runtime.
    // Therefore do not call panic() here, else it will yield a dead recursion:
    //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
    // 当free_list不存在时首先初始化free_list
    if (!head) {
        head = (header_t *)mmap(NULL, 125 * 1024 * 1024, PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        // head = (header_t *)heap.start;
        printf("first initialization\n");
        //   uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
        uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);

        head->magic = 1234567;
        head->size = pmsize - sizeof(header_t);
        head->next = NULL;
    }
    // 考虑先利用first fit进行匹配
    header_t *p, *prevp;
    size_t real_size = sizeof(header_t) + size;
    //   Log("real_size:%d mb\n", real_size>>20);
    print_list();
    // 对list遍历，存在符合大小的块就直接分配
    for (p = prevp = head; p != NULL; prevp = p, p = p->next) {
        if (p->size >= real_size) {
            header_t *node = (header_t *)((uintptr_t)p + sizeof(header_t) +
                                          p->size - real_size);
            node->magic = 1234567;
            node->size = size;
            // 更新p 指向block的size
            p->size -= real_size;
            return (void *)(node + 1);
        }
    }
    // 未找到符合大小的块处理
    printf("there is not enough to allocate\n");
    return NULL;
#endif
    return NULL;
}

#endif