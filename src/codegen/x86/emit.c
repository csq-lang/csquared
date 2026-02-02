#include <codegen/x86/emit.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4096

emit_ctx *emit_create_file(FILE *file) {
  emit_ctx *ctx = malloc(sizeof(emit_ctx));
  if (!ctx)
    return NULL;

  ctx->file = file;
  ctx->buffer = NULL;
  ctx->buffer_size = 0;
  ctx->buffer_capacity = 0;
  ctx->use_buffer = false;
  ctx->indent_level = 0;

  return ctx;
}

emit_ctx *emit_create_buffer(void) {
  emit_ctx *ctx = malloc(sizeof(emit_ctx));
  if (!ctx)
    return NULL;

  ctx->file = NULL;
  ctx->buffer = malloc(INITIAL_CAPACITY);
  if (!ctx->buffer) {
    free(ctx);
    return NULL;
  }

  ctx->buffer[0] = '\0';
  ctx->buffer_size = 0;
  ctx->buffer_capacity = INITIAL_CAPACITY;
  ctx->use_buffer = true;
  ctx->indent_level = 0;

  return ctx;
}

void emit_free(emit_ctx *ctx) {
  if (!ctx)
    return;
  free(ctx->buffer);
  free(ctx);
}

static void ensure_capacity(emit_ctx *ctx, size_t needed) {
  if (!ctx->use_buffer)
    return;

  if (ctx->buffer_size + needed >= ctx->buffer_capacity) {
    size_t new_capacity = ctx->buffer_capacity * 2;
    while (new_capacity < ctx->buffer_size + needed) {
      new_capacity *= 2;
    }

    char *new_buffer = realloc(ctx->buffer, new_capacity);
    if (new_buffer) {
      ctx->buffer = new_buffer;
      ctx->buffer_capacity = new_capacity;
    }
  }
}

static void write_indent(emit_ctx *ctx) {
  for (int i = 0; i < ctx->indent_level; i++) {
    if (ctx->use_buffer) {
      ensure_capacity(ctx, 4);
      strcat(ctx->buffer, "    ");
      ctx->buffer_size += 4;
    } else if (ctx->file) {
      fprintf(ctx->file, "    ");
    }
  }
}

void emit_indent(emit_ctx *ctx) {
  if (ctx)
    ctx->indent_level++;
}

void emit_dedent(emit_ctx *ctx) {
  if (ctx && ctx->indent_level > 0)
    ctx->indent_level--;
}

void emit_newline(emit_ctx *ctx) {
  if (!ctx)
    return;

  if (ctx->use_buffer) {
    ensure_capacity(ctx, 2);
    strcat(ctx->buffer, "\n");
    ctx->buffer_size++;
  } else if (ctx->file) {
    fprintf(ctx->file, "\n");
  }
}

void emit_raw(emit_ctx *ctx, const char *str) {
  if (!ctx || !str)
    return;

  size_t len = strlen(str);

  if (ctx->use_buffer) {
    ensure_capacity(ctx, len + 1);
    strcat(ctx->buffer, str);
    ctx->buffer_size += len;
  } else if (ctx->file) {
    fprintf(ctx->file, "%s", str);
  }
}

void emit_format(emit_ctx *ctx, const char *fmt, ...) {
  if (!ctx || !fmt)
    return;

  va_list args;
  va_start(args, fmt);

  if (ctx->use_buffer) {
    char temp[1024];
    vsnprintf(temp, sizeof(temp), fmt, args);
    size_t len = strlen(temp);
    ensure_capacity(ctx, len + 1);
    strcat(ctx->buffer, temp);
    ctx->buffer_size += len;
  } else if (ctx->file) {
    vfprintf(ctx->file, fmt, args);
  }

  va_end(args);
}

void emit_line(emit_ctx *ctx, const char *fmt, ...) {
  if (!ctx || !fmt)
    return;

  write_indent(ctx);

  va_list args;
  va_start(args, fmt);

  if (ctx->use_buffer) {
    char temp[1024];
    vsnprintf(temp, sizeof(temp), fmt, args);
    size_t len = strlen(temp);
    ensure_capacity(ctx, len + 2);
    strcat(ctx->buffer, temp);
    strcat(ctx->buffer, "\n");
    ctx->buffer_size += len + 1;
  } else if (ctx->file) {
    vfprintf(ctx->file, fmt, args);
    fprintf(ctx->file, "\n");
  }

  va_end(args);
}

void emit_label(emit_ctx *ctx, const char *label) {
  if (!ctx || !label)
    return;

  if (ctx->use_buffer) {
    ensure_capacity(ctx, strlen(label) + 3);
    strcat(ctx->buffer, label);
    strcat(ctx->buffer, ":\n");
    ctx->buffer_size += strlen(label) + 2;
  } else if (ctx->file) {
    fprintf(ctx->file, "%s:\n", label);
  }
}

void emit_directive(emit_ctx *ctx, const char *directive) {
  emit_line(ctx, ".%s", directive);
}

void emit_instruction(emit_ctx *ctx, const char *opcode, const char *operands,
                      ...) {
  if (!ctx || !opcode)
    return;

  write_indent(ctx);

  va_list args;
  va_start(args, operands);

  if (operands && operands[0]) {
    if (ctx->use_buffer) {
      char temp[256];
      vsnprintf(temp, sizeof(temp), operands, args);
      ensure_capacity(ctx, strlen(opcode) + strlen(temp) + 3);
      strcat(ctx->buffer, opcode);
      strcat(ctx->buffer, " ");
      strcat(ctx->buffer, temp);
      strcat(ctx->buffer, "\n");
      ctx->buffer_size += strlen(opcode) + strlen(temp) + 2;
    } else if (ctx->file) {
      fprintf(ctx->file, "%s ", opcode);
      vfprintf(ctx->file, operands, args);
      fprintf(ctx->file, "\n");
    }
  } else {
    if (ctx->use_buffer) {
      emit_format(ctx, "%s\n", opcode);
    } else if (ctx->file) {
      fprintf(ctx->file, "%s\n", opcode);
    }
  }

  va_end(args);
}

char *emit_get_buffer(emit_ctx *ctx) {
  if (!ctx || !ctx->use_buffer)
    return NULL;
  return ctx->buffer;
}

size_t emit_get_buffer_size(emit_ctx *ctx) {
  if (!ctx)
    return 0;
  return ctx->buffer_size;
}

void emit_write_to_file(emit_ctx *ctx, FILE *file) {
  if (!ctx || !file || !ctx->use_buffer || !ctx->buffer)
    return;
  fprintf(file, "%s", ctx->buffer);
}
