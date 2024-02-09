#include<common.h>
#include<list.h>
#include<stdio.h>
// #include<debug.h>
#include<thread.h>
#include<thread-sync.h>
#include<stdlib.h>
#define MB (1<<20)
#define SMP 4

mutex_t lk = MUTEX_INIT();
enum ops { OP_ALLOC = 1, OP_FREE };
struct malloc_op {
  enum ops type;
  union { size_t sz; void *addr; };
};
//返回 0-1之间的随机数
static float randn() { return (float)rand() / 32768; }
// static struct malloc_op random_op(){
//   if(randn()<0.5){
//       struct malloc_op op = (struct malloc_op){.type = OP_ALLOC, .sz = rand()};
//       return op;
//   }else{
//       struct malloc_op op = (struct malloc_op){.type = OP_FREE, .sz = rand()};
//   }
// }

static int idx = 0;

static void entry_0(int tid){
    while(1){
        pmm->alloc(64);
        printf("idx:%d\n", idx++);
        assert(idx * 80 <= 100*MB);
        // printf("test0 not pass");
    }
}
//频繁地小内存释放测试，绝大多数的小于128b,
static void do_test_0(){
    printf("do test0:small storage apply\n");
    pmm->init();
    for (int i = 0; i < SMP;i++){
        create(entry_0);
    }
}

static void entry_1(int tid){
    while(1){
        if(randn()<0.5){
            //随机分配内存
            void* pt = pmm->alloc(1 * MB);
            mutex_lock(&lk);
            malloclist->push(pt);
            mutex_unlock(&lk);
            // printf("idx:%d\n", idx++);
            if(pt==NULL)
                return;
        } else {
            //随机释放内存
            // printf("内存释放测试\n");
            mutex_lock(&lk);
            void* pt = malloclist->pop();
            mutex_unlock(&lk);
            if (pt == NULL)
                continue;
            pmm->free(pt);
            idx--;
        }
        // void* pt=pmm->alloc(1*MB);
        // printf("idx:%d\n", idx++);
        // assert(idx  <= 130);
        // if(pt==NULL)
        //     return;
    }
}
//频繁地小内存释放测试，绝大多数的小于128b,
static void do_test_1(){
    printf("do test1:large storage apply\n");
    pmm->init();
    for (int i = 0; i < SMP;i++){
        create(entry_1);
    }
    // join();
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

}