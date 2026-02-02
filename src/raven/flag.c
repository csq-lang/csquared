#include <raven/flag.h>
#include <raven/logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 8

struct rvn_flag_ctx {
  rvn_flag_bool *bool_flags;
  rvn_flag_string *string_flags;
  size_t bool_count;
  size_t string_count;
  size_t bool_capacity;
  size_t string_capacity;
};

rvn_flag_ctx *flags_create(void) {
  rvn_flag_ctx *ctx = malloc(sizeof(rvn_flag_ctx));
  if (!ctx) {
    RVN_FATAL("Not enough memory to create flag context");
    return NULL;
  }

  ctx->bool_flags = malloc(sizeof(rvn_flag_bool) * INITIAL_CAPACITY);
  ctx->string_flags = malloc(sizeof(rvn_flag_string) * INITIAL_CAPACITY);

  if (!ctx->bool_flags || !ctx->string_flags) {
    free(ctx->bool_flags);
    free(ctx->string_flags);
    free(ctx);
    RVN_FATAL("Not enough memory to create flag arrays");
    return NULL;
  }

  ctx->bool_count = 0;
  ctx->string_count = 0;
  ctx->bool_capacity = INITIAL_CAPACITY;
  ctx->string_capacity = INITIAL_CAPACITY;

  return ctx;
}

void flags_destroy(rvn_flag_ctx *ctx) {
  if (!ctx)
    return;
  free(ctx->bool_flags);
  free(ctx->string_flags);
  free(ctx);
}

static int ensure_bool_capacity(rvn_flag_ctx *ctx) {
  if (ctx->bool_count >= ctx->bool_capacity) {
    size_t new_capacity = ctx->bool_capacity * 2;
    rvn_flag_bool *new_array =
        realloc(ctx->bool_flags, sizeof(rvn_flag_bool) * new_capacity);
    if (!new_array) {
      RVN_ERROR("Failed to expand boolean flag array");
      return -1;
    }
    ctx->bool_flags = new_array;
    ctx->bool_capacity = new_capacity;
  }
  return 0;
}

static int ensure_string_capacity(rvn_flag_ctx *ctx) {
  if (ctx->string_count >= ctx->string_capacity) {
    size_t new_capacity = ctx->string_capacity * 2;
    rvn_flag_string *new_array =
        realloc(ctx->string_flags, sizeof(rvn_flag_string) * new_capacity);
    if (!new_array) {
      RVN_ERROR("Failed to expand string flag array");
      return -1;
    }
    ctx->string_flags = new_array;
    ctx->string_capacity = new_capacity;
  }
  return 0;
}

void flags_register_bool(rvn_flag_ctx *ctx, const char *name, char short_name,
                         const char *description, bool *value) {
  if (!ctx)
    return;
  if (ensure_bool_capacity(ctx) < 0)
    return;

  ctx->bool_flags[ctx->bool_count].name = name;
  ctx->bool_flags[ctx->bool_count].short_name = short_name;
  ctx->bool_flags[ctx->bool_count].description = description;
  ctx->bool_flags[ctx->bool_count].value = value;
  ctx->bool_count++;
}

void flags_register_string(rvn_flag_ctx *ctx, const char *name, char short_name,
                           const char *description, char **value) {
  if (!ctx)
    return;
  if (ensure_string_capacity(ctx) < 0)
    return;

  ctx->string_flags[ctx->string_count].name = name;
  ctx->string_flags[ctx->string_count].short_name = short_name;
  ctx->string_flags[ctx->string_count].description = description;
  ctx->string_flags[ctx->string_count].value = value;
  ctx->string_count++;
}

static bool match_long_flag(const char *arg, const char *flag_name) {
  if (arg[0] != '-' || arg[1] != '-')
    return false;
  return strcmp(arg + 2, flag_name) == 0;
}

static bool match_short_flag(const char *arg, char short_name) {
  if (short_name == '\0')
    return false;
  if (arg[0] != '-' || arg[1] == '-')
    return false;
  return arg[1] == short_name && arg[2] == '\0';
}

int flags_parse(rvn_flag_ctx *ctx, int argc, char **argv) {
  if (!ctx)
    return -1;

  int i = 1;
  while (i < argc) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      flags_print_help(ctx, argv[0]);
      exit(0);
    }

    bool found = false;

    for (size_t j = 0; j < ctx->bool_count; j++) {
      if (match_long_flag(argv[i], ctx->bool_flags[j].name) ||
          match_short_flag(argv[i], ctx->bool_flags[j].short_name)) {
        *ctx->bool_flags[j].value = true;
        found = true;
        break;
      }
    }

    if (!found) {
      for (size_t j = 0; j < ctx->string_count; j++) {
        if (match_long_flag(argv[i], ctx->string_flags[j].name) ||
            match_short_flag(argv[i], ctx->string_flags[j].short_name)) {
          if (i + 1 >= argc) {
            RVN_ERROR("Flag --%s requires a value", ctx->string_flags[j].name);
            return -1;
          }
          *ctx->string_flags[j].value = argv[i + 1];
          i++;
          found = true;
          break;
        }
      }
    }

    if (!found) {
      if (argv[i][0] == '-') {
        RVN_ERROR("Unknown flag: %s", argv[i]);
        return -1;
      }
      break;
    }

    i++;
  }

  return i;
}

void flags_print_help(const rvn_flag_ctx *ctx, const char *program_name) {
  printf("Usage: %s [flags] <filepath>\n\n", program_name);
  printf("Flags:\n");
  printf("  -h, --help    Show this help message\n");

  if (ctx) {
    for (size_t i = 0; i < ctx->bool_count; i++) {
      if (ctx->bool_flags[i].short_name) {
        printf("  -%c, --%-10s %s\n", ctx->bool_flags[i].short_name,
               ctx->bool_flags[i].name, ctx->bool_flags[i].description);
      } else {
        printf("      --%-10s %s\n", ctx->bool_flags[i].name,
               ctx->bool_flags[i].description);
      }
    }

    for (size_t i = 0; i < ctx->string_count; i++) {
      if (ctx->string_flags[i].short_name) {
        printf("  -%c, --%-10s %s\n", ctx->string_flags[i].short_name,
               ctx->string_flags[i].name, ctx->string_flags[i].description);
      } else {
        printf("      --%-10s %s\n", ctx->string_flags[i].name,
               ctx->string_flags[i].description);
      }
    }
  }
}
