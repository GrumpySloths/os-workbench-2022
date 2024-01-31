#include<common.h>
#include<stdio.h>
// #include<debug.h>
#include<thread.h>
#include<stdlib.h>
#define MB (1<<20)

static int idx = 0;

static void entry_0(int tid){
    while(1){
        pmm->alloc(64);
        printf("idx:%d\n", idx++);
        // if(idx>=200)
        //     break;
        assert(idx * 64 <= 50*MB);
    }
}
//频繁地小内存释放测试，绝大多数的小于128b,
static void do_test_0(){
    printf("do test0:small storage apply\n");
    pmm->init();
    for (int i = 0; i < 4;i++){
        create(entry_0);
    }
}

static void entry_1(int tid){
    while(1){
        void* pt=pmm->alloc(1*MB);
        printf("idx:%d\n", idx++);
        // assert(idx  <= 130);
        if(pt==NULL)
            return;
    }
}
//频繁地小内存释放测试，绝大多数的小于128b,
static void do_test_1(){
    printf("do test1:large storage apply\n");
    pmm->init();
    for (int i = 0; i < 4;i++){
        create(entry_1);
    }
}

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

int main(int argc,char*argv[]) {
    if(argc<2)
        exit(1);
    switch(atoi(argv[1])){
        case 0:
            do_test_0();
            break;
        case 1:
            do_test_1();
            break;
        default:
            printf("argv[1] not coresponding expected\n");
        }

    // printf("test\n");
    // pmm->init();
    // for (int i = 0; i < 4; i++) create(entry);
    // // join(goodbye);
    // void* pt = pmm->alloc(32);
    // pmm->free(pt);
    // printf("program terminal\n");
}