#ifndef _OPT_COMMON_H
#define _OPT_COMMON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum { OPT_KIND_FLAG, OPT_KIND_VAL, OPT_KIND_FUNC } opt_kind_t;

typedef struct {
  bool show_info;
  bool show_help;
} csq_options;

typedef void (*opt_func_t)(csq_options* opts, const char* arg);

typedef struct {
  const char *long_name;
  const char *short_name;
  opt_kind_t kind;
  size_t offset;
  opt_func_t func;
} opt_map_t;


csq_options *options_parse(int argc, char *argv[]);
void options_free(csq_options *opts);

#endif
