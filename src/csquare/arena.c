#include "csquare/arena.h"
#include <stddef.h>
#include <stdlib.h>

#include <stdlib.h>
#include <string.h>

void *arena_alloc(arena *a, size_t size) {
  size = (size + 7) & ~7; // 8-byte alignment

  if (a->offset + size > a->cap) {
    size_t new_cap = a->cap ? a->cap : 1024;

    while (new_cap < a->offset + size) {
      new_cap *= 2;
    }

    unsigned char *new_mem = realloc(a->mem, new_cap);
    if (!new_mem) {
      abort();
    }

    a->mem = new_mem;
    a->cap = new_cap;
  }

  void *ptr = a->mem + a->offset;
  a->offset += size;

  return ptr;
}

void arena_free(arena *a) {
  free(a->mem);
  a->mem = NULL;
  a->cap = 0;
  a->offset = 0;
}
