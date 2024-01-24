#include<common.h>
#include<stdio.h>
// #include<debug.h>
#include<thread.h>
#define MB (1<<20)

static int idx = 0;

static void entry(int tid) {
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
            break;
        }
    // pmm->alloc(128);
    // printf("alloc success\n");
    }
}
static void goodbye() { printf("End.\n"); }

int main() {
    printf("test\n");
    pmm->init();
    // for (int i = 0; i < 4; i++) create(entry);
    // join(goodbye);
    int a = 3;
    int* pt = (int*)pmm->alloc(sizeof(int));
    pt = a;
    printf("pt=%d\n", *pt);
    printf("program terminal\n");
}