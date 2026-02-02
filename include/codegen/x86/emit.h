#ifndef CODEGEN_X86_EMIT_H
#define CODEGEN_X86_EMIT_H

#include <stdbool.h>
#include <stdio.h>

typedef struct {
  FILE *file;
  char *buffer;
  size_t buffer_size;
  size_t buffer_capacity;
  bool use_buffer;
  int indent_level;
} emit_ctx;

emit_ctx *emit_create_file(FILE *file);
emit_ctx *emit_create_buffer(void);
void emit_free(emit_ctx *ctx);

void emit_indent(emit_ctx *ctx);
void emit_dedent(emit_ctx *ctx);
void emit_newline(emit_ctx *ctx);

void emit_raw(emit_ctx *ctx, const char *str);
void emit_format(emit_ctx *ctx, const char *fmt, ...);
void emit_line(emit_ctx *ctx, const char *fmt, ...);
void emit_label(emit_ctx *ctx, const char *label);
void emit_directive(emit_ctx *ctx, const char *directive);
void emit_instruction(emit_ctx *ctx, const char *opcode, const char *operands,
                      ...);

char *emit_get_buffer(emit_ctx *ctx);
size_t emit_get_buffer_size(emit_ctx *ctx);
void emit_write_to_file(emit_ctx *ctx, FILE *file);

#endif
