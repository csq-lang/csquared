#include "csquare/error.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define E(NAME, STR) STR,
const char *error_type_str[] = {ERROR_NAMES};

csq_error *new_error(error_type type, const char *filename, int line) {
  csq_error *e = malloc(sizeof(csq_error));

  e->type = type;
  e->filename = filename;
  e->line = line;

  e->col = 0;
  e->has_col = false;

  e->note_count = 0;
  e->note_cap = 8;
  e->notes = malloc(sizeof(char *) * e->note_cap);

  e->level = L_ERR;

  return e;
}

void set_col(csq_error *e, int col) {
  e->col = col;
  e->has_col = true;
}

void add_note(csq_error *e, const char *note) {
  if (e->note_count >= e->note_cap) {
    e->note_cap *= 2;
    e->notes = realloc(e->notes, sizeof(char *) * e->note_cap);
  }

  e->notes[e->note_count++] = (char *)note;
}

void free_error(csq_error *e) {
  if (!e)
    return;

  if (e->notes) {
    for (size_t i = 0; i < e->note_count; i++)
      free(e->notes[i]);
  }

  free(e);
}

void print_error(csq_error *e) {
  const char *prefix = e->level == L_ERR ? "\x1b[31merror" : "\x1b[33mwarning";

  printf("%s\x1b[0m: %s\n", prefix, error_type_str[e->type]);

  printf(" -> \x1b[1m%s:%d", e->filename, e->line);
  if (e->has_col)
    printf(":%d", e->col);
  printf("\n");

  for (size_t i = 0; i < e->note_count; i++) {
    printf(" \x1b[32mnote\x1b[0m: %s\n", e->notes[i]);
  }
}
