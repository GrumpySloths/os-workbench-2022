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

#endif