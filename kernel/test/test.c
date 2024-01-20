#include<common.h>
#include<stdio.h>
#include<debug.h>
#include<thread.h>
// static void entry(int tid) { malloc(128);
//     printf("alloc success\n");
// }
static void entry(int tid) { pmm->alloc(128); }
static void goodbye()      { printf("End.\n"); }
int main() {
    printf("test\n");
    malloc(128);
    pmm->init();
    pmm->alloc(128);
    for (int i = 0; i < 4; i++) create(entry);
    join(goodbye);
    printf("program terminal\n");
}