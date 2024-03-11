#ifndef AM_H__
#define AM_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Memory area for [@start, @end)
typedef struct {
  void *start, *end;
} Area;

Area  heap;
struct Context {
  void    *cr3;
  uint64_t rax, rbx, rcx, rdx,
           rbp, rsi, rdi,
           r8, r9, r10, r11,
           r12, r13, r14, r15,
           rip, cs, rflags,
           rsp, ss, rsp0;
};

// Arch-dependent processor context
typedef struct Context Context;

#endif