#include "ulib.h"

int test() { 

  putstr("hello form test\n");

  return 0;
}
int main() {

  // Example:
  test();
  putstr("Hello, world from user mode!\n");
  // printf("Hello, world from user mode!\n");
  while (1)
      ;


  return 0;
}
