/**
 * @file gen_expr.c
 * @brief Expression code generation dispatch
 * @details Routes expression nodes to appropriate code generators based
 * on expression type (binary, unary, literal, call, etc).
 */

#include <codegen/x86/gen_expr.h>
#include <codegen/x86/gen_binary.h>
#include <codegen/x86/gen_unary.h>
#include <codegen/x86/gen_var.h>
#include <codegen/x86/gen_utils.h>
#include <codegen/x86/gen_string.h>
#include <codegen/x86/inst.h>
#include <codegen/x86/label.h>
#include <codegen/x86/emit.h>
#include <parser/node.h>
#include <parser/types.h>
#include <string.h>
#include <core/diag.h>

void gen_expression(gen_ctx* ctx, csq_node* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_BINARY_OP:
            gen_binary_op(ctx, node);
            break;
        case NODE_UNARY_OP:
            gen_unary_op(ctx, node);
            break;
        case NODE_CALL:
            gen_call(ctx, node);
            break;
        case NODE_IDENTIFIER:
            gen_identifier(ctx, node);
            break;
        case NODE_LITERAL_INT:
        case NODE_LITERAL_BOOL:
        case NODE_LITERAL_NULL:
        case NODE_LITERAL_STRING:
            gen_literal(ctx, node);
            break;
        case NODE_INDEX:
            gen_index(ctx, node);
            break;
        case NODE_ARRAY_LITERAL:
            gen_array_literal(ctx, node);
            break;
        default:
            break;
    }
}

void gen_identifier(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_IDENTIFIER) return;
    
    const char* name = node->data.identifier.name;
    var_info* var = var_table_find(ctx->vars, name);
    
    if (var) {
        inst_mov_reg_mem(ctx->emit, REG_EAX, REG_EBP, var->offset);
    } else {
        if (ctx->diag && ctx->source_path) {
            diag_report_error(ctx->diag, DIAG_ERROR_UNDEFINED_VARIABLE,
                             ctx->source_path, node->line, node->column,
                             strlen(name), NULL,
                             "Undefined variable");
        }
    }
}

void gen_literal(gen_ctx* ctx, csq_node* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_LITERAL_INT:
            inst_mov_reg_imm(ctx->emit, REG_EAX, (int)node->data.literal_int.value);
            break;
        case NODE_LITERAL_BOOL:
            inst_mov_reg_imm(ctx->emit, REG_EAX, node->data.literal_bool.value ? 1 : 0);
            break;
        case NODE_LITERAL_NULL:
            inst_mov_reg_imm(ctx->emit, REG_EAX, 0);
            break;
        case NODE_LITERAL_STRING: {
            int idx = get_string_index(ctx, node->data.literal_string.value);
            if (idx < 0) {
                const char* label = gen_add_string(ctx, node->data.literal_string.value);
                if (label) {
                    emit_line(ctx->emit, "movl $%s, %%eax", label);
                }
            } else {
                emit_line(ctx->emit, "movl $%s, %%eax", ctx->strings[idx].label);
            }
            break;
        }
        default:
            break;
    }
}

void gen_array_literal(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_ARRAY_LITERAL) return;
    
    size_t count = node->data.array_literal.elements.count;
    if (count == 0) {
        inst_mov_reg_imm(ctx->emit, REG_EAX, 0);
        return;
    }
    
    size_t elem_size = 4;
    size_t total_size = (count * elem_size + 15) & ~15;
    
    inst_sub_reg_imm(ctx->emit, REG_ESP, total_size);
    inst_mov_reg_reg(ctx->emit, REG_EAX, REG_ESP);
    inst_push_reg(ctx->emit, REG_EAX);
    
    for (size_t i = 0; i < count; i++) {
        gen_expression(ctx, node->data.array_literal.elements.items[i]);
        emit_line(ctx->emit, "movl %%eax, %zu(%%esp)", i * elem_size + 4);
    }
    
    inst_pop_reg(ctx->emit, REG_EAX);
}

void gen_index(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_INDEX) return;
    
    gen_expression(ctx, node->data.index.object);
    inst_push_reg(ctx->emit, REG_EAX);
    
    gen_expression(ctx, node->data.index.index);
    
    char* valid_label = label_gen_new(ctx->labels, "array_valid");
    char* bounds_check_label = label_gen_new(ctx->labels, "array_bounds");
    
    inst_cmp_reg_imm(ctx->emit, REG_EAX, 0);
    emit_line(ctx->emit, "jge %s", bounds_check_label);
    
    emit_line(ctx->emit, "movl $%d, %%eax", DIAG_ERROR_ARRAY_INDEX_OUT_OF_BOUNDS);
    emit_line(ctx->emit, "int $0x80");
    
    emit_line(ctx->emit, "%s:", bounds_check_label);
    
    bool is_string = false;
    csq_type* obj_type = node->data.index.object->type_info;
    
    if (!obj_type && node->data.index.object->type == NODE_IDENTIFIER) {
        const char* name = node->data.index.object->data.identifier.name;
        var_info* var = var_table_find(ctx->vars, name);
        if (var) {
            obj_type = var->type;
        }
    }
    
    if (obj_type && obj_type->kind == TYPE_STRING) {
        is_string = true;
    }
    
    if (obj_type && type_is_array(obj_type)) {
        size_t array_size = obj_type->data.array.count;
        inst_cmp_reg_imm(ctx->emit, REG_EAX, (int)array_size);
        emit_line(ctx->emit, "jl %s", valid_label);
        
        emit_line(ctx->emit, "movl $%d, %%eax", DIAG_ERROR_ARRAY_INDEX_OUT_OF_BOUNDS);
        emit_line(ctx->emit, "int $0x80");
    }
    
    emit_line(ctx->emit, "%s:", valid_label);
    
    inst_mov_reg_reg(ctx->emit, REG_EBX, REG_EAX);
    inst_pop_reg(ctx->emit, REG_EAX);
    
    if (is_string) {
        inst_add_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    } else {
        emit_line(ctx->emit, "movl (%%eax, %%ebx, 4), %%eax");
    }
}


