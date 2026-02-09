/**
 * @file opt-common.c
 * @brief Command-line option parsing
 * @details Parses compiler command-line arguments including flags for
 * debugging, assembly output, optimization, and file specification.
 */

#include <csquare/opt-common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*option_handler)(const char *arg, int idx, int argc, char **argv,
                              csq_options *opts);

typedef struct {
  const char *name;
  option_handler handler;
  int takes_arg;
} option_entry;

static int is_file_argument(const char *arg) {
  if (!arg || arg[0] == '-')
    return 0;

  size_t len = strlen(arg);
  return len >= 4 && strcmp(arg + len - 4, ".c2p") == 0;
}

static int handle_debug_lexer(const char *arg, int idx, int argc, char **argv,
                              csq_options *opts) {
  (void)arg;
  (void)idx;
  (void)argc;
  (void)argv;
  opts->debug_lexer = 1;
  return 0;
}

static int handle_help(const char *arg, int idx, int argc, char **argv,
                       csq_options *opts) {
  (void)arg;
  (void)idx;
  (void)argc;
  (void)argv;
  printf("C² Project\n");

  return 0;
}

static int handle_debug_ast(const char *arg, int idx, int argc, char **argv,
                            csq_options *opts) {
  (void)arg;
  (void)idx;
  (void)argc;
  (void)argv;
  opts->debug_ast = 1;
  return 0;
}

static int handle_emit_asm(const char *arg, int idx, int argc, char **argv,
                           csq_options *opts) {
  const char *backend = NULL;
  opts->emit_asm = 1;

  if (arg[2] == '=') {
    backend = arg + 3;
  } else if (arg[2] != '\0') {
    backend = arg + 2;
  } else if (idx + 1 < argc && argv[idx + 1][0] != '-' &&
             !is_file_argument(argv[idx + 1])) {
    backend = argv[idx + 1];
    return 1;
  } else {
    opts->asm_backend = ASM_BACKEND_AT_T;
    return 0;
  }

  if (backend && *backend != '\0') {
    if (strcmp(backend, "intel") == 0) {
      opts->asm_backend = ASM_BACKEND_INTEL;
    } else if (strcmp(backend, "at&t") == 0 || strcmp(backend, "att") == 0) {
      opts->asm_backend = ASM_BACKEND_AT_T;
    } else {
      printf("error: unknown assembly backend '%s'\n", backend);
      return -1;
    }
  } else {
    opts->asm_backend = ASM_BACKEND_AT_T;
  }

  return 0;
}

static int handle_output(const char *arg, int idx, int argc, char **argv,
                         csq_options *opts) {
  const char *value = NULL;

  if (arg[2] == '=') {
    value = arg + 3;
  } else if (arg[1] == 'o' && arg[2] == '\0') {
    if (idx + 1 >= argc) {
      printf("error: option -o requires an argument\n");
      return -1;
    }
    value = argv[idx + 1];
    return 1;
  } else if (strncmp(arg, "--output=", 9) == 0) {
    value = arg + 9;
  } else if (strcmp(arg, "--output") == 0) {
    if (idx + 1 >= argc) {
      printf("error: option --output requires an argument\n");
      return -1;
    }
    value = argv[idx + 1];
    return 1;
  }

  if (value && *value != '\0') {
    opts->output_file = (char *)value;
    return 0;
  }

  return -1;
}

static const option_entry options[] = {{"--debug-lexer", handle_debug_lexer, 0},
                                       {"--debug-ast", handle_debug_ast, 0},
                                       {"-h", handle_help, 1},
                                       {"-S", handle_emit_asm, 0},
                                       {"-o", handle_output, 1},
                                       {"--output", handle_output, 1},
                                       {NULL, NULL, 0}};

static const option_entry *find_option(const char *arg) {
  for (int i = 0; options[i].name != NULL; i++) {
    size_t len = strlen(options[i].name);
    if (strncmp(arg, options[i].name, len) == 0) {
      const char *after = arg + len;
      if (*after == '\0' || *after == '=' || !options[i].takes_arg)
        return &options[i];
    }
  }
  return NULL;
}

csq_options *options_parse(int argc, char **argv) {
  csq_options *opts = malloc(sizeof(csq_options));
  if (!opts)
    return NULL;

  opts->debug_lexer = 0;
  opts->debug_ast = 0;
  opts->emit_asm = 0;
  opts->asm_backend = ASM_BACKEND_NONE;
  opts->output_file = NULL;
  opts->input_file = NULL;

  if (argc < 2) {
    printf("usage: csq [options] <filepath>\n");
    free(opts);
    return NULL;
  }

  for (int i = 1; i < argc;) {
    const char *arg = argv[i];
    i++;

    if (is_file_argument(arg)) {
      if (opts->input_file) {
        printf("error: multiple input files specified\n");
        free(opts);
        return NULL;
      }
      opts->input_file = (char *)arg;
      continue;
    }

    if (arg[0] != '-') {
      printf("error: invalid argument '%s'\n", arg);
      free(opts);
      return NULL;
    }

    const option_entry *opt = find_option(arg);
    if (!opt) {
      printf("error: unknown option '%s'\n", arg);
      free(opts);
      return NULL;
    }

    int result = opt->handler(arg, i - 1, argc, argv, opts);
    if (result < 0) {
      free(opts);
      return NULL;
    }
    i += result;
  }

  if (!opts->input_file) {
    printf("error: no input file specified\n");
    free(opts);
    return NULL;
  }

  return opts;
}

void options_free(csq_options *opts) {
  if (opts)
    free(opts);
}
