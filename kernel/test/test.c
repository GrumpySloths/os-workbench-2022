#include<common.h>
#include<thread.h>
static void entry(int tid) { pmm->alloc(128); }
static void goodbye()      { printf("End.\n"); }
int main() {
    printf("test\n");
    pmm->init();
    for (int i = 0; i < 4; i++) create(entry);
    join(goodbye);
}