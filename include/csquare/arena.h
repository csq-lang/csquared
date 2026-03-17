#ifndef _ARENA_ALLOCATOR_H
#define _ARENA_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint8_t *mem;
  size_t cap;
  size_t offset;
} arena;

void *arena_alloc(arena *a, size_t size);
void arena_free(arena *a);

#endif
