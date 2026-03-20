#ifndef _MISC_UTILS_H
#define _MISC_UTILS_H

#include <stddef.h>

const char *get_line(const char *src, int target_line, size_t *out_len);
char *read_file(const char *filename);

#endif
