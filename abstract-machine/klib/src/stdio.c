#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
// #include <spinlock.h>
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
// static spinlock_t lk;
static char digits[] = "0123456789abcdef";

static void
printint(int xx, int base, int sign)
{
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    putch(buf[i]);
}

static void
printptr(uint64 x)
{
  int i;
  putch('0');
  putch('x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putch(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

static void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

static char* itoa(int num, char* str, int base) {
    int i = 0;
    int isNegative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }

    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    reverse(str, i);

    return str;
}

static char* ptrtoa(char*buf,uint64 x)
{
    
  int i=0;
  buf[i++] = '0';
  buf[i++] = 'x';
  for (; i < (sizeof(uint64) * 2)+2; i++, x <<= 4)
    buf[i] = digits[x >> (sizeof(uint64) * 8 - 4)];

  return buf;
}

// Print to the console. only understands %d, %x, %p, %s.
int printf(const char *fmt, ...)
{
  va_list ap;
  int i, c;
  char *s;

  if (fmt == 0)
    panic("null fmt");

  va_start(ap, fmt);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      putch(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(va_arg(ap, int), 10, 1);
      break;
    case 'x':
      printint(va_arg(ap, int), 16, 1);
      break;
    case 'p':
      printptr(va_arg(ap, uint64));
      break;
    case 's':
      if((s = va_arg(ap, char*)) == 0)
        s = "(null)";
      for(; *s; s++)
        putch(*s);
      break;
    case '%':
      putch('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      putch('%');
      putch(c);
      break;
    }
  }
  va_end(ap);

  return 0;
}



int vsprintf(char *out, const char *fmt, va_list ap) {
    int i, c;
    char *s;
    char *buf = out;
    char tmp[32];

    if (fmt == 0)
        panic("null fmt");
    
    for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
        if(c != '%'){
            *buf++ = c;
            continue;
        }
        c = fmt[++i] & 0xff;
        if(c == 0)
            break;
        switch(c){
        case 'd':
            itoa(va_arg(ap, int), tmp,10);
            memcpy(buf, tmp, strlen(tmp));
            buf += strlen(tmp);
            break;
        case 'x':
            itoa(va_arg(ap, int), tmp,16);
            memcpy(buf, tmp, strlen(tmp));
            buf += strlen(tmp);
            break;
        case 'p':
            ptrtoa(tmp,va_arg(ap, uint64));
            memcpy(buf, tmp, strlen(tmp));
            buf += strlen(tmp);
            break;
        case 's':
            if((s = va_arg(ap, char*)) == 0)
                s = "(null)";
            while(*s)
                *buf++ = *s++;
            break;
        case '%':
            *buf++ = '%';
            break;
        default:
            // Print unknown % sequence to draw attention.
            *buf++ = '%';
            *buf++ = c;
            break;
        }
    }
    
    *buf = '\0';
    return buf - out;
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
