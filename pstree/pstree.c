#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{
  printf("argc=%d \n", argc);
  for (int i = 0; i < argc; i++)
  {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("hello world\n");
  // printf("hello world");
  assert(!argv[argc]);
  return 0;
}
