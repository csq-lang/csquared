/**
 * @file gen_core.c
 * @brief Core code generation functions
 * @details Main code generation entry points and context management.
 */

#include <codegen/x86/gen_core.h>
#include <codegen/x86/gen_var.h>
#include <codegen/x86/gen_utils.h>
#include <codegen/x86/gen_string.h>
#include <codegen/x86/gen_stmt.h>
#include <codegen/x86/emit.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 64

gen_ctx* gen_create(ast_context* ast) {
    gen_ctx* ctx = malloc(sizeof(gen_ctx));
    if (!ctx) return NULL;
    
    ctx->emit = emit_create_buffer();
    if (!ctx->emit) {
        free(ctx);
        return NULL;
    }
    
    ctx->data_emit = emit_create_buffer();
    if (!ctx->data_emit) {
        emit_free(ctx->emit);
        free(ctx);
        return NULL;
    }
    
    ctx->labels = label_gen_create();
    if (!ctx->labels) {
        emit_free(ctx->data_emit);
        emit_free(ctx->emit);
        free(ctx);
        return NULL;
    }
    
    ctx->frame = frame_create();
    if (!ctx->frame) {
        label_gen_free(ctx->labels);
        emit_free(ctx->data_emit);
        emit_free(ctx->emit);
        free(ctx);
        return NULL;
    }
    
    ctx->vars = var_table_create();
    if (!ctx->vars) {
        frame_free(ctx->frame);
        label_gen_free(ctx->labels);
        emit_free(ctx->data_emit);
        emit_free(ctx->emit);
        free(ctx);
        return NULL;
    }
    
    ctx->strings = malloc(sizeof(string_constant) * INITIAL_CAPACITY);
    if (!ctx->strings) {
        var_table_free(ctx->vars);
        frame_free(ctx->frame);
        label_gen_free(ctx->labels);
        emit_free(ctx->data_emit);
        emit_free(ctx->emit);
        free(ctx);
        return NULL;
    }
    
    ctx->loop_stack = malloc(sizeof(loop_label) * INITIAL_CAPACITY);
    if (!ctx->loop_stack) {
        free(ctx->strings);
        var_table_free(ctx->vars);
        frame_free(ctx->frame);
        label_gen_free(ctx->labels);
        emit_free(ctx->data_emit);
        emit_free(ctx->emit);
        free(ctx);
        return NULL;
    }
    
    ctx->ast = ast;
    ctx->generate_asm = true;
    ctx->output_file = NULL;
    ctx->string_counter = 0;
    ctx->string_count = 0;
    ctx->string_capacity = INITIAL_CAPACITY;
    ctx->loop_stack_count = 0;
    ctx->loop_stack_capacity = INITIAL_CAPACITY;
    ctx->diag = NULL;
    ctx->source_path = NULL;
    ctx->source_buffer = NULL;
    ctx->syntax_mode = ASM_SYNTAX_ATT;
    
    return ctx;
}

void gen_free(gen_ctx* ctx) {
    if (!ctx) return;
    for (size_t i = 0; i < ctx->string_count; i++) {
        free(ctx->strings[i].label);
        free(ctx->strings[i].value);
    }
    free(ctx->strings);
    var_table_free(ctx->vars);
    frame_free(ctx->frame);
    label_gen_free(ctx->labels);
    emit_free(ctx->data_emit);
    emit_free(ctx->emit);
    free(ctx->loop_stack);
    free(ctx->output_file);
    free(ctx);
}

void gen_set_output(gen_ctx* ctx, const char* filename) {
    if (!ctx) return;
    free(ctx->output_file);
    ctx->output_file = filename ? strdup(filename) : NULL;
}

void gen_set_asm_mode(gen_ctx* ctx, bool asm_mode) {
    if (ctx) ctx->generate_asm = asm_mode;
}

void gen_set_syntax(gen_ctx* ctx, emit_syntax syntax) {
    if (ctx) {
        ctx->syntax_mode = syntax;
        emit_set_syntax(ctx->emit, syntax);
        emit_set_syntax(ctx->data_emit, syntax);
    }
}

int gen_generate(gen_ctx* ctx) {
    if (!ctx || !ctx->ast) return -1;
    
    if (ctx->ast->root) {
        gen_collect_strings(ctx, ctx->ast->root);
    }
    
    gen_emit_header(ctx);
    
    if (ctx->string_count > 0) {
        emit_line(ctx->emit, ".section .rodata");
        for (size_t i = 0; i < ctx->string_count; i++) {
            emit_line(ctx->emit, "%s:", ctx->strings[i].label);
            emit_line(ctx->emit, "    .string \"%s\"", ctx->strings[i].value);
        }
        emit_line(ctx->emit, ".text");
        emit_newline(ctx->emit);
    }
    
    if (ctx->ast->root) {
        gen_program(ctx, ctx->ast->root);
    }
    
    gen_emit_footer(ctx);
    
    if (ctx->output_file) {
        return gen_generate_file(ctx, ctx->output_file);
    }
    
    return 0;
}

int gen_generate_file(gen_ctx* ctx, const char* filename) {
    if (!ctx || !filename) return -1;
    
    FILE* file = fopen(filename, "w");
    if (!file) return -1;
    
    if (ctx->string_count > 0) {
        fprintf(file, ".section .rodata\n");
        for (size_t i = 0; i < ctx->string_count; i++) {
            fprintf(file, "%s:\n", ctx->strings[i].label);
            fprintf(file, "    .string \"%s\"\n", ctx->strings[i].value);
        }
        fprintf(file, "\n");
    }
    
    emit_write_to_file(ctx->emit, file);
    fclose(file);
    
    return 0;
}

void gen_emit_header(gen_ctx* ctx) {
    emit_ctx* e = ctx->emit;
    if (ctx->syntax_mode == ASM_SYNTAX_INTEL) {
        emit_directive(e, "intel_syntax noprefix");
    }
    emit_directive(e, "code32");
    emit_line(e, ".text");
    emit_line(e, ".globl main");
    emit_newline(e);
}

void gen_emit_footer(gen_ctx* ctx) {
    (void)ctx;
}
