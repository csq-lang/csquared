#ifndef CODEGEN_X86_FRAME_H
#define CODEGEN_X86_FRAME_H

#include <codegen/x86/reg.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  int offset;
  x86_reg base;
  bool is_allocated;
} stack_slot;

typedef struct {
  stack_slot *slots;
  size_t count;
  size_t capacity;
  int current_offset;
  int saved_ebp_offset;
  int local_size;
  int param_count;
  char *epilogue_label;
} frame_ctx;

frame_ctx *frame_create(void);
void frame_free(frame_ctx *frame);

void frame_begin_function(frame_ctx *frame, int param_count);
void frame_end_function(frame_ctx *frame);

int frame_alloc_local(frame_ctx *frame, int size);
void frame_free_local(frame_ctx *frame, int offset);

int frame_get_param_offset(frame_ctx *frame, int param_index);
int frame_get_local_offset(frame_ctx *frame, int local_index);

void frame_emit_prologue(frame_ctx *frame, void *emit);
void frame_emit_epilogue(frame_ctx *frame, void *emit);

int frame_get_stack_size(frame_ctx *frame);

#endif
