#include <raven/logger.h>
#include <raven/source.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#endif

rvn_source *source_load(const char *path) {
  FILE *file = fopen(path, "rb");
  if (!file) {
    RVN_ERROR("Could not open file: %s", path);
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char *buffer = (char *)malloc(size + 1);
  if (!buffer) {
    RVN_FATAL("Not enough memory to read: %s", path);
    fclose(file);
    return NULL;
  }

  size_t read = fread(buffer, sizeof(char), size, file);
  if (read < size) {
    RVN_ERROR("Could not read file: %s", path);
    free(buffer);
    fclose(file);
    return NULL;
  }

  buffer[read] = '\0';
  fclose(file);

  rvn_source *src = (rvn_source *)malloc(sizeof(rvn_source));
  src->path = path;
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__) ||           \
    defined(__Jadeite__)
  src->abs_path = realpath(path, NULL);
#else
  src->abs_path = NULL;
#endif
  src->buffer = buffer;
  src->size = size;

  RVN_DEBUG_LOG("Loaded source: %s (%zu bytes)", path, size);
  return src;
}

void source_free(rvn_source *source) {
  if (source) {
    free(source->abs_path);
    free(source->buffer);
    free(source);
  }
}
