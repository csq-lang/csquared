/**
 * @file gen_var.c
 * @brief Variable access code generation
 * @details Generates code for variable accesses, assignments, and operations.
 */

#include <codegen/x86/gen_var.h>
#include <codegen/x86/gen_utils.h>
#include <codegen/x86/gen_const.h>
#include <codegen/x86/gen_string.h>
#include <codegen/x86/gen_expr.h>
#include <codegen/x86/inst.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 64

var_table* var_table_create(void) {
    var_table* table = malloc(sizeof(var_table));
    if (!table) return NULL;
    
    table->vars = malloc(sizeof(var_info) * INITIAL_CAPACITY);
    if (!table->vars) {
        free(table);
        return NULL;
    }
    
    table->count = 0;
    table->capacity = INITIAL_CAPACITY;
    table->next_offset = 0;
    
    return table;
}

void var_table_free(var_table* table) {
    if (!table) return;
    free(table->vars);
    free(table);
}

var_info* var_table_add(var_table* table, const char* name, csq_type* type) {
    if (!table || !name) return NULL;
    
    if (table->count >= table->capacity) {
        size_t new_capacity = table->capacity * 2;
        var_info* new_vars = realloc(table->vars, sizeof(var_info) * new_capacity);
        if (!new_vars) return NULL;
        table->vars = new_vars;
        table->capacity = new_capacity;
    }
    
    size_t size = get_type_size(type);
    size_t aligned_size = (size + 3) & ~3;
    table->next_offset -= aligned_size;
    
    var_info* var = &table->vars[table->count];
    var->name = name;
    var->offset = table->next_offset;
    var->type = type;
    var->is_global = false;
    var->is_param = false;
    
    table->count++;
    return var;
}

var_info* var_table_find(var_table* table, const char* name) {
    if (!table || !name) return NULL;
    
    for (size_t i = 0; i < table->count; i++) {
        if (strcmp(table->vars[i].name, name) == 0) {
            return &table->vars[i];
        }
    }
    return NULL;
}

void gen_var_decl(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_VAR_DECL) return;
    
    const char* name = node->data.var_decl.name->data.identifier.name;
    csq_type* type = node->type_info;
    
    if (!type && node->data.var_decl.type_spec) {
        type = resolve_type_spec(node->data.var_decl.type_spec);
        node->type_info = type;
    }
    
    if (!type) {
        type = type_int();
        node->type_info = type;
    }
    
    var_info* var = var_table_find(ctx->vars, name);
    if (!var) {
        var = var_table_add(ctx->vars, name, type);
        if (!var) return;
    }
    
    csq_node* init = node->data.var_decl.init;
    
    if (init) {
        if (gen_is_constant(ctx, init)) {
            long long val = gen_fold_constant(ctx, init);
            emit_line(ctx->emit, "movl $%lld, %d(%%ebp)", val, var->offset);
        } else if (init->type == NODE_LITERAL_STRING) {
            int idx = get_string_index(ctx, init->data.literal_string.value);
            if (idx >= 0) {
                emit_line(ctx->emit, "movl $%s, %d(%%ebp)", ctx->strings[idx].label, var->offset);
            } else {
                const char* label = gen_add_string(ctx, init->data.literal_string.value);
                if (label) {
                    emit_line(ctx->emit, "movl $%s, %d(%%ebp)", label, var->offset);
                }
            }
        } else if (init->type == NODE_LITERAL_FLOAT) {
            double dval = init->data.literal_float.value;
            unsigned int ival;
            float fval = (float)dval;
            memcpy(&ival, &fval, sizeof(ival));
            emit_line(ctx->emit, "movl $0x%x, %d(%%ebp)", ival, var->offset);
        } else {
            gen_expression(ctx, init);
            inst_mov_mem_reg(ctx->emit, REG_EBP, var->offset, REG_EAX);
        }
    }
}
