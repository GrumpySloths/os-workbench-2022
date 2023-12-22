#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>

typedef struct{
    int x;
    int y;
} Square; //定义小方块对象

void splash();
void screen_update();
void print_key();
static inline void puts(const char *s) {
  for (; *s; s++) putch(*s);
}
