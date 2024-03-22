#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#define __USE_GNU
#include<dlfcn.h>

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))
//构建全局的handles数组，大小为100，存储dlopen返回的指针
void* handles[100];
//构建全局的handle_count变量，用于记录handles数组中的指针个数
int handle_count = 0;
//构建全局的wrapper_count变量，记录expr的当前位置
int wrapper_count = 0;

int main(int argc, char *argv[]) {
  static char line[1024];
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
      char template[100] = "/tmp/creplXXXXXX.c";
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
      //创建新的变量为template加上.so后缀
      strcat(template,".so");
      handle = dlopen(template, RTLD_NOW);
      if (handle == NULL) {
        fprintf(stderr, "%s\n", dlerror());
        continue;
      }
      //将handle指针存储到handles数组中
      handles[handle_count++] = handle;

    }else{
      //line为表达式的处理
      //将line结尾的\n替换为\0
      line[strlen(line)-1]=0;
      //将line变为一个wrapper,构建成.c文件
      char template[100] = "/tmp/creplXXXXXX.c";
      int fd=mkstemps(template,2);
      assert(fd>=0);
      printf("template=%s\n",template);
      //将line 构造成int __expr_wrapper_{wrapper_count}() {return line;}
      //的格式并写入文件中
      char wrapper[4096];
      char expr_name[100];
      snprintf(expr_name,sizeof(expr_name),"__expr_wrapper_%d",wrapper_count);
      snprintf(wrapper, sizeof(wrapper), "int %s() {return %s;}", 
      expr_name, line);
      write(fd,wrapper,strlen(wrapper));
      close(fd);
      //去除template 的.c后缀
      template[strlen(template)-2]=0;
      // 通过system调用gcc编译这个文件,使其变为.so文件
      char cmd[4096];
      snprintf(cmd, sizeof(cmd), "gcc -shared -fPIC %s.c -o %s.so", template, template);
      printf("cmd=%s\n",cmd);
      system(cmd);
      // 使用dlopen将.so文件加载到内存中
      //创建新的变量为template加上.so后缀
      printf("cmd execve success\n");
      printf("find expr_name:%s\n",expr_name);
      strcat(template, ".so");
      handle = dlopen(template, RTLD_LAZY);
      if (handle == NULL) {
        fprintf(stderr, "%s\n", dlerror());
        continue;
      }
      printf("dlopen success\n");
      // 执行wrapper函数
      int (*wrapper_func)(void) = dlsym(handles[handle_count-1], expr_name);
      if (wrapper_func == NULL) {
        printf("can't find symbol %s\n", expr_name);
        fprintf(stderr, "%s\n", dlerror());
        continue;
      }
      printf("result:%d\n", wrapper_func());
    }


  }
}
