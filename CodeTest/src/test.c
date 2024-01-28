#include "malloc.h"
#include<assert.h>
int main(void) {
    printf("it is a test\n");
    puts("hello\n");
    printf("it is a test\n");
    assert(1 == 2);
    printf("assert pass\n");
    malloc(128);
    return 0;
}