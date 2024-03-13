#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
// #include <spinlock.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
// static spinlock_t lk;
// 打印正数
void putnum_positive(int d){
    if (d) {
        putnum_positive(d / 10);
        putch(d % 10+'0');
    }

    assert(d >= 0);

}
//打印全体数字
void putnum(int d){
    if(d<0){
        putch('-');
        putnum_positive(-1 * d);
    }else if(d==0){
        putch('0');
    }else{
        putnum_positive(d);
    }
}
//打印16进制
void printhex(uintptr_t d){
    assert(d >= 0);
    if (d / 16)
        printhex(d/16);
    if(d%16>=10)
        putch(d % 16 - 10 + 'a');
    else
        putch(d % 16 + '0');
}
int printf(const char *fmt, ...) {
    va_list ap;
    char c, *s;
    int d;
    uintptr_t pt;
    char pre;  // 代表前一个字符
    va_start(ap, fmt);
    size_t count = strlen(fmt);
    size_t len = count;
    // spin_lock(&lk);
    while (count-- > 0) {
        if((pre=*fmt++)=='%'){
            switch(*fmt++){
                case 's': /* string */
                    s = va_arg(ap, char *);
                    putstr(s);
                    break;
                case 'd':              /* int */
                    d = va_arg(ap, int32_t);
                    putnum(d);
                    break;
                case 'c':              /* char */
                    /* need a cast here since va_arg only
                        takes fully promoted types */
                    c = (char) va_arg(ap, int);
                    putch(c);
                    break;
                case 'p':
                    pt = va_arg(ap, uintptr_t);
                    putstr("0x");
                    printhex(pt);
                    break;
                default:
                    panic("current behavior is not defined,program exited\n");
                }
        }else{
            putch(pre);
            // printf("%c", pre);
        }
    }
    // spin_unlock(&lk);
    va_end(ap);
    return len;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
