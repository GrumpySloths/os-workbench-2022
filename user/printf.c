#include<stdarg.h>
#include<stddef.h>
#include "ulib.h"

#define panic_on(cond, s) \
  ({ if (cond) { \
      putstr("AM Panic: "); putstr(s); \
      putstr(" @ " __FILE__ ":" TOSTRING(__LINE__) "  \n"); \
    } })

#define panic(s) panic_on(1, s)



size_t strlen(const char *s) {
   size_t count= 0; 
   while(*s++)
       count++;
   return count;
}
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
    kputc(buf[i]);
}

static void
printptr(uint64 x)
{
  int i;
  kputc('0');
  kputc('x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    kputc(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

static char* ptrtoa(char*buf,uint64 x)
{
  // static char buf[16];
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
      kputc(c);
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
        kputc(*s);
      break;
    case '%':
      kputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      kputc('%');
      kputc(c);
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

