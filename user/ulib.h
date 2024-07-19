#ifndef __ULIB_H__
#define __ULIB_H__
#include <stddef.h>
#include <stdint.h>
#include "../kernel/framework/syscall.h"
#include "../kernel/framework/user.h"

#define assert(cond) \
  do { \
    if (!(cond)) { \
      printf("Assertion fail at %s:%d\n", __FILE__, __LINE__); \
      }\
  } while (0)

#define putstr(s) \
  ({ for (const char *p = s; *p; p++) kputc(*p); })

static inline long syscall(int num, long x1, long x2, long x3, long x4) {
  register long a0 asm ("rax") = num;
  register long a1 asm ("rdi") = x1;
  register long a2 asm ("rsi") = x2;
  register long a3 asm ("rdx") = x3;
  register long a4 asm ("rcx") = x4;
  asm volatile("int $0x80"
    : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4)
    : "memory");
  return a0;
}

static inline int kputc(char ch) {
  return syscall(SYS_kputc, ch, 0, 0, 0);
}

static inline int fork() {
  return syscall(SYS_fork, 0, 0, 0, 0);
}

static inline int wait(int *status) {
  return syscall(SYS_wait, (uint64_t)status, 0, 0, 0);
}

static inline int exit(int status) {
  return syscall(SYS_exit, status, 0, 0, 0);
}

static inline int kill(int pid) {
  return syscall(SYS_kill, pid, 0, 0, 0);
}

static inline void *mmap(void *addr, int length, int prot, int flags) {
  return (void *)syscall(SYS_mmap, (uint64_t)addr, length, prot, flags);
}

static inline int getpid() {
  return syscall(SYS_getpid, 0, 0, 0, 0);
}

static inline int sleep(int seconds) {
  return syscall(SYS_sleep, seconds, 0, 0, 0);
}

static inline int64_t uptime() {
  return syscall(SYS_uptime, 0, 0, 0, 0);
}


//ulib.c
#define STRINGIFY(s)        #s
#define TOSTRING(s)         STRINGIFY(s)

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;


int printf(const char *fmt, ...);
int sprintf(char *out, const char *fmt, ...);
size_t strlen(const char *s);
char *strcpy(char *s, const char *t);
int strcmp(const char *p, const char *q);
void *memset(void *dst, int c, uint n);
char *strchr(const char *s, char c);
char *gets(char *buf, int max);
int atoi(const char *s);
void *memmove(void *vdst, const void *vsrc, int n);
int memcmp(const void *s1, const void *s2, uint n);
void *memcpy(void *dst, const void *src, uint n);
char *itoa(int num, char *str, int base);
#endif