#include <common.h>
#define MB (1<<20)

static int idx = 0;
#ifndef TEST
uint64_t uptime() { return io_read(AM_TIMER_UPTIME).us/1000; }
#endif

typedef struct{
    int x;
    int y;
} s1_t;
static void os_init() { pmm->init(); }

static void os_run() {
    s1_t* s1 = (s1_t*)pmm->alloc(sizeof(s1_t));
    s1_t* s2 = (s1_t*)pmm->alloc(sizeof(s1_t));

    s1->x = 10;
    s1->y = 2;
    s2->x = 20;
    s2->y = 30;
    printf("s1: x=%d,y=%d\n", s1->x, s1->y);
    printf("s2: x=%d,y=%d\n", s2->x, s2->y);
    pmm->free(s1);
    pmm->free(s2);
    // pmm->free((void*)123);
    printf("s1: x=%d,y=%d\n", s1->x, s1->y);
    printf("s2: x=%d,y=%d\n", s2->x, s2->y);
    Log("test");
#ifndef TEST
    uint64_t t0 = uptime();
#endif
    int count = 0;
    while (1) {
        void* pt;
        if (idx % 2) {
            pt = pmm->alloc(1 * MB);
        } else {
            pt = pmm->alloc(2 * MB);
        }

        pmm->free(pt);
        printf("idx:%d\n", idx++);
        if(count++>10000){
#ifndef TEST
            printf("program run time:%d ms \n", (uptime() - t0));
            halt(1);
#endif
            break;
        }
        // assert(idx<=10000);
    }
    while(1)
        ; //os-run代表着一个cpu，故该线程不能停止
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
