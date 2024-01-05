#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
   size_t count= 0; 
   while(*s++)
       count++;
   return count;
}

char *strcpy(char *dst, const char *src)
{
    char *dst_pt = dst;
    while ((*dst_pt++ = *src++))
        ;
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{
    char *tmp = dst;
    while ((n-- > 0) && *src) {
        *tmp++ = *src++;
    }
    *tmp = '\0';
    return dst;
}

char *strcat(char *dst, const char *src)
{
  char*tmp=dst+strlen(dst);
  while((*tmp++=*src++))
      ;
  return dst;
}

int strcmp(const char *s1, const char *s2) {
   for (; *s1 == *s2;s1++,s2++)
    if(*s1=='\0'||*s2=='\0')
        return 0;
   return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    for (; n > 0&&(*s1==*s2);n--,s1++,s2++){
        if(*s1=='\0'||*s2=='\0')
            return 0;
    }
    if(n==0)
        return 0;
    else
        return *s1 - *s2;
}

void *memset(void *s, int c, size_t n)
{
  panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n)
{
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n)
{
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  panic("Not implemented");
}

#endif
