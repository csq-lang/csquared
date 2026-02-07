#ifndef CODEGEN_X86_GEN_CORE_H
#define CODEGEN_X86_GEN_CORE_H

#include <parser/ast.h>
#include <parser/types.h>
#include <codegen/x86/emit.h>
#include <codegen/x86/label.h>
#include <codegen/x86/frame.h>
#include <core/diag.h>
#include <stdbool.h>

typedef struct {
    const char* name;
    int offset;
    csq_type* type;
    bool is_global;
    bool is_param;
} var_info;

typedef struct {
    var_info* vars;
    size_t count;
    size_t capacity;
    int next_offset;
} var_table;

typedef struct {
    char* label;
    char* value;
} string_constant;

typedef struct loop_label {
    char* start;
    char* end;
} loop_label;

typedef struct {
    emit_ctx* emit;
    emit_ctx* data_emit;
    label_gen* labels;
    frame_ctx* frame;
    ast_context* ast;
    var_table* vars;
    string_constant* strings;
    size_t string_count;
    size_t string_capacity;
    bool generate_asm;
    char* output_file;
    int string_counter;
    DiagReporter* diag;
    const char* source_path;
    const char* source_buffer;
    emit_syntax syntax_mode;
    loop_label* loop_stack;
    size_t loop_stack_count;
    size_t loop_stack_capacity;
} gen_ctx;

gen_ctx* gen_create(ast_context* ast);
void gen_free(gen_ctx* ctx);

void gen_set_output(gen_ctx* ctx, const char* filename);
void gen_set_asm_mode(gen_ctx* ctx, bool asm_mode);
void gen_set_syntax(gen_ctx* ctx, emit_syntax syntax);

int gen_generate(gen_ctx* ctx);
int gen_generate_file(gen_ctx* ctx, const char* filename);

void gen_emit_header(gen_ctx* ctx);
void gen_emit_footer(gen_ctx* ctx);
void gen_emit_data_section(gen_ctx* ctx);

#endif
