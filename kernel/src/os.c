#include <common.h>
#define MB (1<<20)
// enum ops { OP_ALLOC = 1, OP_FREE };
// struct malloc_op {
//   enum ops type;
//   union { size_t sz; void *addr; };
// };
// //返回 0-1之间的随机数
// static float randn() { return (float)rand() / 32768; }
// static struct malloc_op random_op(){
//   if(randn()<0.5){
//       struct malloc_op op = (struct malloc_op){.type = OP_ALLOC, .sz = rand()};
//       return op;
//   }else{
//       struct malloc_op op = (struct malloc_op){.type = OP_FREE, .sz = rand()};
//   }
// }
uint64_t uptime() { return io_read(AM_TIMER_UPTIME).us/1000; }
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
    int idx = 0;
    Log("test");
    uint64_t t0 = uptime();
    while (1) {
        void* pt;
        if (idx % 2) {
            pt = pmm->alloc(1 * MB);
        } else {
            pt = pmm->alloc(2 * MB);
        }

        pmm->free(pt);
        printf("idx:%d\n", idx++);
        if(idx==10000){
            printf("program run time:%d", (uptime() - t0));
        }
        assert(idx<=10000);
    }
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
