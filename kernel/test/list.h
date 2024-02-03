#ifndef LIST_H
#define LIST_H

#include <kernel.h>

// 该头文件主要用于维护测试用例中的分配链表以产生随机的测试用例
typedef struct __list_t {
    /* data */
    void* pt;  // 存储malloc分配的指针
    struct __list_t* next;
} list_t;

static list_t* head = NULL;
static int length = 0;  // 记录链表的长度

MODULE(malloclist) {
    void (*push)(void* pt);
    void* (*pop)();
};

#endif