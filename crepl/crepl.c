#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>

int main(int argc, char *argv[]) {
  static char line[4096];
  while (1) {
    printf("crepl> ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
      break;
    }
    printf("Got %zu chars.\n", strlen(line)); // ??
    // 将line通过mkstemp api写入文件到/tmp目录下,后缀是.c
    int fd=mkstemps("/tmp/creplXXXXXX.c",2);
    assert(fd>=0);
    write(fd,line,strlen(line));
    close(fd);
    // 通过system调用gcc编译这个文件
    // 通过system调用./a.out执行这个文件


  }
}
