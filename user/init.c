#include "ulib.h"

void hello_test();
void dfs_test();
// 构建一个会不断递归导致栈溢出的函数
void recursive_function() {
    int array[100]; // 每次递归调用都在栈上分配一个较大的数组，消耗栈空间
    for (int i = 0; i < 100;i++){
        array[i] = 10;
    }
    printf("array[0]: %d\n", array[0]);
    recursive_function();
}
//为sprintf函数设计测试用例
void sprintf_test() {
    char buffer[100];
    char buffer2[100];
    char buffer3[16];
    int a = 10;
    sprintf(buffer, "a = %d", a);
    sprintf(buffer2,"buffer addr:%p", buffer);
    sprintf(buffer3,"hex of 64:%x", 64);
    printf("%s\n", buffer);
    printf("%s\n", buffer2);
    printf("%s\n", buffer3);
    printf("buffer addr:%p\n", buffer);
}

int main() {
  printf("Hello from user\n");
  sleep(2);
  printf("it is a test\n");
  printf("▇▇▇");
  printf("▇▇▇");
  sprintf_test();
  // sleep(10);
  // recursive_function();
  // for (int i = 0; i < 1000000; i++) {
  //     printf("debug: %d\n", i);
  //     // sleep(1);
  // }
  // sleep(2);
  hello_test();
  // dfs_test();
  while (1);
}

void hello_test() {
  int pid = fork(), x = 0;

  const char *fmt;
  if (pid) {
    fmt = "Parent #%d proc_pid:%d curtime:%d ms\n";
  } else {
    sleep(1);
    fmt = "Child #%d proc_pid:%d curtime:%d ms\n";
  }

  while (1) {
    printf(fmt, ++x, getpid(), uptime());
    sleep(1);
  }
}

#define DEST  '+'
#define EMPTY '.'

static struct move {
  int x, y, ch;
} moves[] = {
  { 0, 1, '>' },
  { 1, 0, 'v' },
  { 0, -1, '<' },
  { -1, 0, '^' },
};

static char map[][16] = {
  "#####",
  "#..+#",
  "##..#",
  "#####",
  "",
};

void display();

void dfs(int x, int y) {
  if (map[x][y] == DEST) {
    display();
  } else {
    sleep(1);
    int nfork = 0;

    for (struct move *m = moves; m < moves + 4; m++) {
      int x1 = x + m->x, y1 = y + m->y;
      if (map[x1][y1] == DEST || map[x1][y1] == EMPTY) {
        int pid = fork();
        if (pid == 0) { // map[][] copied
          map[x][y] = m->ch;
          dfs(x1, y1);
        } else {
          nfork++;
        }
      }
    }
  }
  while (1) sleep(1);
}

void dfs_test() {
  dfs(1, 1);
  while (1) sleep(1);
}

void display() {
  for (int i = 0; ; i++) {
    for (const char *s = map[i]; *s; s++) {
      switch (*s) {
        case EMPTY: printf("   "); break;
        case DEST : printf(" ○ "); break;
        case '>'  : printf(" → "); break;
        case '<'  : printf(" ← "); break;
        case '^'  : printf(" ↑ "); break;
        case 'v'  : printf(" ↓ "); break;
        default   : printf("▇▇▇"); break;
      }
    }
    printf("\n");
    if (strlen(map[i]) == 0) break;
  }
}
