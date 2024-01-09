//32位系统
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
typedef struct{
    int x;
    char y;
    float f;
} header_t;
//10进制转16进制
void dec_to_hex(uintptr_t d){
    if (d/16)
        dec_to_hex(d/16);
    if(d%16>=10)
        putchar(d % 16 - 10 + 'a');
    else
        putchar(d % 16 + '0');
}
int main()
{
    header_t *hptr = (header_t *)malloc(sizeof(header_t));
    hptr->x = 10;
    hptr->f = 15.0;
    printf("hptr x:%d,f:%f\n", hptr->x, hptr->f);
    free(hptr);
    printf("hptr x:%d,f:%f\n", hptr->x, hptr->f);
    if((void*)hptr>=NULL){
        printf("this is a debug\n");
    }

    return 0;
}