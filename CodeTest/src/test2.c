#include <sys/mman.h>
#include <stdint.h>
#include<stdio.h>
typedef struct __header_t{
    uint32_t size;
    int magic; //用于sanity check,检查内存非法访问或重复释放分配等出乎意料问题
    struct __header_t *next;
} header_t;
// free list node
// static header_t*head=NULL;
int main(void) { 
    // head = (header_t*)mmap(NULL, 4096*1024, PROT_READ | PROT_WRITE, MAP_PRIVATE| MAP_ANONYMOUS , -1, 0);
    // head->size = sizeof(header_t);
    // head->magic = 123456; // 这里可以根据实际需求设置一个合适的值
    // head->next = NULL;
    printf("hello world\n");
    printf("hello world\n");
    printf("分配成功\n");
    return 0;
}