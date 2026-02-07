/**
 * @file gen_stmt.c
 * @brief Statement code generation
 * @details Generates code for control flow statements (if, while, for, switch, etc).
 */

#include <codegen/x86/gen_stmt.h>
#include <codegen/x86/gen_expr.h>
#include <codegen/x86/gen_var.h>
#include <codegen/x86/gen_utils.h>
#include <codegen/x86/inst.h>
#include <codegen/x86/label.h>
#include <codegen/x86/frame.h>
#include <parser/node.h>
#include <stdlib.h>
#include <string.h>

static bool gen_push_loop(gen_ctx* ctx, const char* start_label, const char* end_label) {
    if (!ctx) return false;
    
    if (ctx->loop_stack_count >= ctx->loop_stack_capacity) {
        size_t new_capacity = ctx->loop_stack_capacity * 2;
        loop_label* new_stack = realloc(ctx->loop_stack, sizeof(loop_label) * new_capacity);
        if (!new_stack) return false;
        ctx->loop_stack = new_stack;
        ctx->loop_stack_capacity = new_capacity;
    }
    
    loop_label* entry = &ctx->loop_stack[ctx->loop_stack_count++];
    entry->start = strdup(start_label);
    entry->end = strdup(end_label);
    
    return true;
}

static void gen_pop_loop(gen_ctx* ctx) {
    if (!ctx || ctx->loop_stack_count == 0) return;
    
    loop_label* entry = &ctx->loop_stack[--ctx->loop_stack_count];
    free(entry->start);
    free(entry->end);
}

static loop_label* gen_current_loop(gen_ctx* ctx) {
    if (!ctx || ctx->loop_stack_count == 0) return NULL;
    return &ctx->loop_stack[ctx->loop_stack_count - 1];
}

void gen_program(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_PROGRAM) return;
    
    for (size_t i = 0; i < node->data.program.statements.count; i++) {
        csq_node* stmt = node->data.program.statements.items[i];
        if (stmt->type == NODE_FUNCTION_DECL) {
            gen_function(ctx, stmt);
        }
    }
}

void gen_statement(gen_ctx* ctx, csq_node* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_FUNCTION_DECL:
            gen_function(ctx, node);
            break;
        case NODE_VAR_DECL:
            gen_var_decl(ctx, node);
            break;
        case NODE_IF:
            gen_if(ctx, node);
            break;
        case NODE_WHILE:
            gen_while(ctx, node);
            break;
        case NODE_RETURN:
            gen_return(ctx, node);
            break;
        case NODE_BREAK:
            gen_break(ctx, node);
            break;
        case NODE_CONTINUE:
            gen_continue(ctx, node);
            break;
        case NODE_EXPR_STMT:
            gen_expression(ctx, node->data.expr_stmt.expr);
            break;
        case NODE_BLOCK:
            gen_block(ctx, node);
            break;
        default:
            gen_expression(ctx, node);
            break;
    }
}

void gen_function(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_FUNCTION_DECL) return;
    
    const char* name = node->data.function_decl.name->data.identifier.name;
    emit_label(ctx->emit, name);
    
    ctx->vars->count = 0;
    ctx->vars->next_offset = 0;
    
    int param_count = node->data.function_decl.params.count;
    frame_begin_function(ctx->frame, param_count);
    
    for (size_t i = 0; i < node->data.function_decl.params.count; i++) {
        csq_node* param = node->data.function_decl.params.items[i];
        if (param->type == NODE_PARAM) {
            const char* param_name = param->data.param.name->data.identifier.name;
            csq_type* param_type = param->type_info;
            
            if (!param_type && param->data.param.type_spec) {
                param_type = resolve_type_spec(param->data.param.type_spec);
                param->type_info = param_type;
            }
            
            if (!param_type) {
                param_type = type_int();
                param->type_info = param_type;
            }
            
            int param_offset = 8 + (i * 4);
            
            var_info* var = var_table_add(ctx->vars, param_name, param_type);
            if (var) {
                var->offset = param_offset;
                var->is_param = true;
            }
        }
    }
    
    ctx->vars->next_offset = 0;
    
    gen_collect_local_vars(ctx, node->data.function_decl.body);
    
    if (ctx->vars->next_offset < 0) {
        ctx->frame->local_size = -ctx->vars->next_offset;
    }
    
    emit_indent(ctx->emit);
    frame_emit_prologue(ctx->frame, ctx->emit);
    
    if (node->data.function_decl.body) {
        gen_block(ctx, node->data.function_decl.body);
    }
    
    if (strcmp(name, "main") == 0) {
        emit_dedent(ctx->emit);
        emit_label(ctx->emit, ".main_exit");
        emit_indent(ctx->emit);
        emit_line(ctx->emit, "movl $1, %%eax");
        emit_line(ctx->emit, "movl $0, %%ebx");
        emit_line(ctx->emit, "int $0x80");
    } else {
        frame_emit_epilogue(ctx->frame, ctx->emit);
    }
    emit_dedent(ctx->emit);
    frame_end_function(ctx->frame);
    emit_newline(ctx->emit);
}

void gen_block(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_BLOCK) return;
    
    for (size_t i = 0; i < node->data.block.statements.count; i++) {
        gen_statement(ctx, node->data.block.statements.items[i]);
    }
}

void gen_if(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_IF) return;
    
    char* else_label = label_gen_new(ctx->labels, "else");
    char* end_label = label_gen_new(ctx->labels, "endif");
    
    gen_expression(ctx, node->data.if_stmt.condition);
    inst_cmp_reg_imm(ctx->emit, REG_EAX, 0);
    inst_je_label(ctx->emit, else_label);
    
    emit_indent(ctx->emit);
    gen_statement(ctx, node->data.if_stmt.then_branch);
    emit_dedent(ctx->emit);
    inst_jmp_label(ctx->emit, end_label);
    
    emit_dedent(ctx->emit);
    emit_label(ctx->emit, else_label);
    emit_indent(ctx->emit);
    if (node->data.if_stmt.else_branch) {
        gen_statement(ctx, node->data.if_stmt.else_branch);
    }
    emit_dedent(ctx->emit);
    
    emit_label(ctx->emit, end_label);
    emit_indent(ctx->emit);
}

void gen_while(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_WHILE) return;
    
    char* start_label = label_gen_new(ctx->labels, "while");
    char* end_label = label_gen_new(ctx->labels, "endwhile");
    
    gen_push_loop(ctx, start_label, end_label);
    
    emit_dedent(ctx->emit);
    emit_label(ctx->emit, start_label);
    emit_indent(ctx->emit);
    gen_expression(ctx, node->data.while_stmt.condition);
    inst_cmp_reg_imm(ctx->emit, REG_EAX, 0);
    inst_je_label(ctx->emit, end_label);
    
    emit_indent(ctx->emit);
    gen_statement(ctx, node->data.while_stmt.body);
    emit_dedent(ctx->emit);
    inst_jmp_label(ctx->emit, start_label);
    
    emit_dedent(ctx->emit);
    emit_label(ctx->emit, end_label);
    emit_indent(ctx->emit);
    
    gen_pop_loop(ctx);
}

void gen_return(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_RETURN) return;
    
    if (node->data.return_stmt.value) {
        gen_expression(ctx, node->data.return_stmt.value);
    } else {
        inst_mov_reg_imm(ctx->emit, REG_EAX, 0);
    }
}

void gen_break(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_BREAK) return;
    
    loop_label* current = gen_current_loop(ctx);
    if (current) {
        inst_jmp_label(ctx->emit, current->end);
    }
}

void gen_continue(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_CONTINUE) return;
    
    loop_label* current = gen_current_loop(ctx);
    if (current) {
        inst_jmp_label(ctx->emit, current->start);
    }
}
