#include <stdio.h>
#include <setjmp.h>
#include <stdint.h>
#define LOCAL_MACHINE

#ifdef LOCAL_MACHINE
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

enum co_status
{
    CO_NEW = 1, // 新创建，还未执行过
    CO_RUNNING, // 已经执行过
    CO_WAITING, // 在 co_wait 上等待
    CO_DEAD,    // 已经结束，但还未释放资源
};

static inline void stack_switch_call(void *sp, void *entry, uintptr_t arg)
{
    asm volatile(
#if __x86_64__
        "movq %0, %%rsp; movq %2, %%rdi; jmp *%1"
        : : "b"((uintptr_t)sp), "d"(entry), "a"(arg) : "memory"
#else
        "movl %0, %%esp; movl %2, 4(%0); jmp *%1"
        : : "b"((uintptr_t)sp - 8), "d"(entry), "a"(arg) : "memory"
#endif
    );
}
void co_yield (jmp_buf buf, int x, int y)
{
    printf("test\n");
    longjmp(buf, 1);
}
void printHello(int y)
{
    int x = 1;
    int z = 3;
    printf("hello world\n");
    x++;
    printf("x=%d,z=%d\n", x, z);
    jmp_buf buf1;
    setjmp(buf1);
    // x++, z++;
    printf("x=%d,z=%d\n", x, z);
    co_yield (buf1, x++, z++);
}
int main(void)
{
    int n = 0;
    jmp_buf buf;
    setjmp(buf);
    printf("Hello %d\n", n);
    printHello(2);
    longjmp(buf, n++);

    return 0;
}