/**
 * @file source.c
 * @brief Source file loading and management
 * @details Handles reading source files from disk, managing file buffers,
 * and tracking source file paths and metadata.
 */

#include <csquare/logger.h>
#include <csquare/source.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#endif

/**
 * @brief Load a source file from disk
 * @details Reads the entire file into memory and initializes a source
 * structure.
 * @param path File path to load
 * @return Allocated source structure, or NULL on failure
 */
csq_source *source_load(const char *path) {
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

  csq_source *src = (csq_source *)malloc(sizeof(csq_source));
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

/**
 * @brief Free source file resources
 * @param source Source structure to deallocate
 */
void source_free(csq_source *source) {
  if (source) {
    free(source->abs_path);
    free(source->buffer);
    free(source);
  }
}
