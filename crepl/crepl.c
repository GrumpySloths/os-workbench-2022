#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<dlfcn.h>

int main(int argc, char *argv[]) {
  static char line[4096];
  void*handle=NULL;
  while (1) {
    printf("crepl> ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
      break;
    }
    //根据line是否以"int"开头进行判断
    if (strncmp(line, "int", 3) == 0) {
      // 将line通过mkstemp api写入文件到/tmp目录下,后缀是.c
      char template[] = "/tmp/creplXXXXXX.c";
      int fd=mkstemps(template,2);
      assert(fd>=0);
      printf("template=%s\n",template);
      write(fd,line,strlen(line));
      close(fd);
      //去除template 的.c后缀
      template[strlen(template)-2]=0;
      // 通过system调用gcc编译这个文件,使其变为.so文件
      char cmd[4096];
      snprintf(cmd, sizeof(cmd), "gcc -shared -fPIC %s.c -o %s.so", template, template);
      printf("cmd=%s\n",cmd);
      system(cmd);
      // 使用dlopen将.so文件加载到内存中
      handle = dlopen(template, RTLD_NOW);
      if (handle == NULL) {
        fprintf(stderr, "%s\n", dlerror());
        continue;
      }
      //利用dlsym执行foo函数
      int (*func)(void);
      func = dlsym(handle, "foo");
      printf("%d\n", func());
    }else{
      //定义一个返回int 的函数指针
      int (*func)(void);
      //通过dlsym获取函数指针,函数名为line
      func = dlsym(handle, line);
      //打印该函数的值
      printf("%d\n", func());
    }


  }
}
