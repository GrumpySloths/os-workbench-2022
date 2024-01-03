#include <common.h>
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
typedef struct{
    int x;
    int y;
} s1_t;
static void os_init() { pmm->init(); }

static void os_run() {
    s1_t* s1 = (s1_t*)pmm->alloc(sizeof(s1_t));
    s1->x = 10;
    s1->y = 2;
    printf("s1: x=%d,y=%d\n", s1->x, s1->y);
    pmm->free(s1);
    // for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    //   putch(*s == '*' ? '0' + cpu_current() : *s);
    // }
    // while (1) ;
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
