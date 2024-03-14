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
//   panic("Not implemented");
    char c, *s;
    int d;
    uintptr_t pt;
    char pre;  // 代表前一个字符
    size_t count = strlen(fmt);
    size_t len = count;
    size_t i = 0;
    while (count-- > 0) {
        if((pre=*fmt++)=='%'){
            switch(*fmt++){
                case 's': /* string */
                    s = va_arg(ap, char *);
                    while(*s){
                        out[i++]=*s++;
                    }
                    break;
                case 'd':              /* int */
                    d = va_arg(ap, int32_t);
                    char buf[100];
                    int j = 0;
                    if(d<0){
                        out[i++]='-';
                        d=-d;
                    }
                    if(d==0){
                        buf[j++]='0';
                    }
                    while(d){
                        buf[j++]=d%10+'0';
                        d/=10;
                    }
                    while(j--){
                        out[i++]=buf[j];
                    }
                    break;
                case 'c':              /* char */
                    /* need a cast here since va_arg only
                        takes fully promoted types */
                    c = (char) va_arg(ap, int);
                    out[i++]=c;
                    break;
                case 'p':
                    pt = va_arg(ap, uintptr_t);
                    char buf2[100];
                    int k = 0;
                    out[i++]='0';
                    out[i++]='x';
                    if(pt==0){
                        out[i++]='0';
                    }
                    while(pt){
                        if(pt%16>=10)
                            buf2[k++]=pt%16-10+'a';
                        else
                            buf2[k++]=pt%16+'0';
                        pt/=16;
                    }
                    while(k--){
                        out[i++]=buf2[k];
                    }
                    break;
                default:
                    panic("current behavior is not defined,program exited\n");
                }
        }else{
            out[i++]=pre;
        }
    }
    out[i]='\0';
    return len;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(out, n, fmt, ap);
    va_end(ap);
    return ret;

}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    //vsnprintf实现
    char c, *s;
    int d;
    uintptr_t pt;
    char pre;  // 代表前一个字符
    size_t count = strlen(fmt);
    size_t len = count;
    size_t i = 0; 
    //对count和n进行比较
    if(count>n){
        count=n;
    }
    while (count-- > 0) {
        if((pre=*fmt++)=='%'){
            switch(*fmt++){
                case 's': /* string */
                    s = va_arg(ap, char *);
                    while(*s){
                        out[i++]=*s++;
                    }
                    break;
                case 'd':              /* int */
                    d = va_arg(ap, int32_t);
                    char buf[100];
                    int j = 0;
                    if(d<0){
                        out[i++]='-';
                        d=-d;
                    }
                    if(d==0){
                        buf[j++]='0';
                    }
                    while(d){
                        buf[j++]=d%10+'0';
                        d/=10;
                    }
                    while(j--){
                        out[i++]=buf[j];
                    }
                    break;
                case 'c':              /* char */
                    /* need a cast here since va_arg only
                        takes fully promoted types */
                    c = (char) va_arg(ap, int);
                    out[i++]=c;
                    break;
                case 'p':
                    pt = va_arg(ap, uintptr_t);
                    char buf2[100];
                    int k = 0;
                    out[i++]='0';
                    out[i++]='x';
                    if(pt==0){
                        out[i++]='0';
                    }
                    while(pt){
                        if(pt%16>=10)
                            buf2[k++]=pt%16-10+'a';
                        else
                            buf2[k++]=pt%16+'0';
                        pt/=16;
                    }
                    while(k--){
                        out[i++]=buf2[k];
                    }
                    break;
                default:
                    panic("current behavior is not defined,program exited\n");
                }
        }else{
            out[i++]=pre;
        }
    }

    out[i]='\0';
    
    return len; 
}

#endif
