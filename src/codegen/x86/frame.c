/**
 * @file frame.c
 * @brief Stack frame management
 * @details Manages function stack frames including prologue/epilogue,
 * local variable allocation, and parameter passing.
 */

#include <codegen/x86/frame.h>
#include <codegen/x86/inst.h>
#include <codegen/x86/emit.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16
#define STACK_ALIGNMENT 16

frame_ctx* frame_create(void) {
    frame_ctx* frame = malloc(sizeof(frame_ctx));
    if (!frame) return NULL;
    
    frame->slots = malloc(sizeof(stack_slot) * INITIAL_CAPACITY);
    if (!frame->slots) {
        free(frame);
        return NULL;
    }
    
    frame->count = 0;
    frame->capacity = INITIAL_CAPACITY;
    frame->current_offset = 0;
    frame->saved_ebp_offset = 0;
    frame->local_size = 0;
    frame->param_count = 0;
    frame->epilogue_label = NULL;
    
    return frame;
}

void frame_free(frame_ctx* frame) {
    if (!frame) return;
    free(frame->slots);
    free(frame);
}

void frame_begin_function(frame_ctx* frame, int param_count) {
    if (!frame) return;
    
    frame->current_offset = 0;
    frame->local_size = 0;
    frame->param_count = param_count;
    frame->count = 0;
    frame->epilogue_label = NULL;
}

void frame_end_function(frame_ctx* frame) {
    if (!frame) return;
    frame->count = 0;
    frame->current_offset = 0;
}

int frame_alloc_local(frame_ctx* frame, int size) {
    if (!frame) return 0;
    
    int aligned_size = (size + 15) & ~15;
    frame->current_offset -= aligned_size;
    frame->local_size += aligned_size;
    
    if (frame->count >= frame->capacity) {
        size_t new_capacity = frame->capacity * 2;
        stack_slot* new_slots = realloc(frame->slots, sizeof(stack_slot) * new_capacity);
        if (!new_slots) return frame->current_offset;
        frame->slots = new_slots;
        frame->capacity = new_capacity;
    }
    
    frame->slots[frame->count].offset = frame->current_offset;
    frame->slots[frame->count].base = REG_EBP;
    frame->slots[frame->count].is_allocated = true;
    frame->count++;
    
    return frame->current_offset;
}

void frame_free_local(frame_ctx* frame, int offset) {
    if (!frame) return;
    
    for (size_t i = 0; i < frame->count; i++) {
        if (frame->slots[i].offset == offset) {
            frame->slots[i].is_allocated = false;
            return;
        }
    }
}

int frame_get_param_offset(frame_ctx* frame, int param_index) {
    if (!frame) return 0;
    return 8 + (param_index * 4);
}

int frame_get_local_offset(frame_ctx* frame, int local_index) {
    if (!frame || local_index < 0 || (size_t)local_index >= frame->count) {
        return 0;
    }
    return frame->slots[local_index].offset;
}

void frame_emit_prologue(frame_ctx* frame, void* emit) {
    emit_ctx* ctx = (emit_ctx*)emit;
    if (!frame || !ctx) return;
    
    emit_line(ctx, "pushl %%ebp");
    emit_line(ctx, "movl %%esp, %%ebp");
    
    if (frame->local_size > 0) {
        emit_line(ctx, "subl $%d, %%esp", frame->local_size);
    }
}

void frame_emit_epilogue(frame_ctx* frame, void* emit) {
    emit_ctx* ctx = (emit_ctx*)emit;
    if (!frame || !ctx) return;
    
    emit_line(ctx, "movl %%ebp, %%esp");
    emit_line(ctx, "popl %%ebp");
    emit_line(ctx, "ret");
}

int frame_get_stack_size(frame_ctx* frame) {
    if (!frame) return 0;
    return frame->local_size;
}