#ifndef RAVEN_FLAG_H
#define RAVEN_FLAG_H

#include <stdbool.h>
#include <stddef.h>

typedef struct rvn_flag_ctx rvn_flag_ctx;

typedef struct {
  const char *name;
  char short_name;
  const char *description;
  bool *value;
} rvn_flag_bool;

typedef struct {
  const char *name;
  char short_name;
  const char *description;
  char **value;
} rvn_flag_string;

rvn_flag_ctx *flags_create(void);
void flags_destroy(rvn_flag_ctx *ctx);
void flags_register_bool(rvn_flag_ctx *ctx, const char *name, char short_name,
                         const char *description, bool *value);
void flags_register_string(rvn_flag_ctx *ctx, const char *name, char short_name,
                           const char *description, char **value);
int flags_parse(rvn_flag_ctx *ctx, int argc, char **argv);
void flags_print_help(const rvn_flag_ctx *ctx, const char *program_name);

#endif
