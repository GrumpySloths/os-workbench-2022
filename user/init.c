#include "ulib.h"

#define putstr(s) \
  ({ for (const char *p = s; *p; p++) kputc(*p); })

int main() {

  // Example:
  // printf("pid = %d\n", getpid());
  putstr("Hello, world from user mode!\n");

  while(1);

  return 0;
}
