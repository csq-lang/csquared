#include "csquare/lexer/lexer.h"
#include "csquare/opt-common.h"
#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char *argv[]) {
  csq_options *opts = options_parse(argc, argv);
  if (!opts)
    return EXIT_FAILURE;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *src = read_file(argv[1]);
  if (!src)
    return EXIT_FAILURE;

  token_list *lexed = lex(src);

  for (size_t i = 0; i < lexed->count; i++) {
    print_token(lexed->tokens[i]);
  }

  free_token_list(lexed);
  free(src);
  return 0;
}
