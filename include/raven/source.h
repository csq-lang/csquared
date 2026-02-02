#ifndef RAVEN_SOURCE_H
#define RAVEN_SOURCE_H

#include <stddef.h>

typedef struct {
  const char *path;
  char *abs_path;
  char *buffer;
  size_t size;
} rvn_source;

rvn_source *source_load(const char *path);
void source_free(rvn_source *source);

#endif
