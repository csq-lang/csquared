#include "csquare/utils.h"
#include <stdio.h>
#include <stdlib.h>

const char *get_line(const char *src, int target_line, size_t *out_len) {
  if (target_line < 1)
    return NULL;

  int line = 1;
  const char *p = src;

  while (*p) {
    if (line == target_line) {
      const char *line_start = p;
      size_t len = 0;
      while (p[len] && p[len] != '\n')
        len++;
      if (out_len)
        *out_len = len;
      return line_start;
    }

    if (*p == '\n')
      line++;
    p++;
  }

  if (out_len)
    *out_len = 0;
  return NULL;
}

char *read_file(const char *filename) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    perror("fopen");
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buffer = malloc(size + 1);
  if (!buffer) {
    perror("malloc");
    fclose(f);
    return NULL;
  }

  if (fread(buffer, 1, size, f) != (size_t)size) {
    perror("fread");
    free(buffer);
    fclose(f);
    return NULL;
  }
  buffer[size] = '\0';
  fclose(f);
  return buffer;
}
