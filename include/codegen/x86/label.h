#ifndef CODEGEN_X86_LABEL_H
#define CODEGEN_X86_LABEL_H

#include <stddef.h>

typedef struct {
  int counter;
  char **labels;
  size_t count;
  size_t capacity;
} label_gen;

label_gen *label_gen_create(void);
void label_gen_free(label_gen *gen);

char *label_gen_new(label_gen *gen, const char *prefix);
char *label_gen_anon(label_gen *gen);

#endif
