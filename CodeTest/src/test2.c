#include<stdarg.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>

//给定size 计算最小的大于等于size的2的幂
static int power_count(size_t size) { 
  int count = 0;
  unsigned int power = 1;
    while (power < size) {
        power *= 2;
        count++;
    }
  return count;
}


int main(void) { 
  size_t size = 6; 
  printf("size:%ld\n",size);
  printf("power_count:%d\n",power_count(size));
  return 0;
}