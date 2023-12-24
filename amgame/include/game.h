#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>

typedef struct{
    int x;
    int y;
} Square; //定义小方块对象

void splash();
void flush();//清空整个屏幕
void screen_update(Square *);
void print_key();
void read_key(Square*);//读取key并做出相应的逻辑处理
static inline void puts(const char *s) {
    for (; *s; s++) putch(*s);
}
