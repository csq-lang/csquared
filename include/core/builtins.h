#ifndef CORE_BUILTINS_H
#define CORE_BUILTINS_H

#include <stdbool.h>

typedef enum { BUILTIN_SYSCALL, BUILTIN_COUNT } builtin_type;

typedef struct {
  const char *name;
  builtin_type type;
  int min_args;
  int max_args;
} builtin_info;

bool builtin_is_builtin(const char *name);

const builtin_info *builtin_get_info(const char *name);

void builtin_gen_call(void *gen_ctx, const char *name, void *args,
                      int arg_count);

#endif
