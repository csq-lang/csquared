/**
 * @file gen_utils.c
 * @brief Code generation utility functions
 * @details Helper functions for register management, stack operations,
 * and common code generation patterns.
 */

#include <codegen/x86/gen_utils.h>
#include <codegen/x86/gen_var.h>
#include <parser/node.h>
#include <core/builtins.h>
#include <string.h>

size_t get_type_size(csq_type* type) {
    if (!type) return 8;
    
    switch (type->kind) {
        case TYPE_BOOL:
            return 1;
        case TYPE_INT:
        case TYPE_POINTER:
        case TYPE_STRING:
        case TYPE_F64:
            return 8;
        case TYPE_F32:
            return 4;
        default:
            return 8;
    }
}

csq_type* resolve_type_spec(csq_node* type_spec_node) {
    if (!type_spec_node || type_spec_node->type != NODE_TYPE_SPEC) {
        return type_int();
    }
    
    csq_node* base = type_spec_node->data.type_spec.base;
    if (!base || base->type != NODE_IDENTIFIER) {
        return type_int();
    }
    
    const char* type_name = base->data.identifier.name;
    size_t name_len = base->data.identifier.len;
    
    if (name_len == 4 && strncmp(type_name, "bool", 4) == 0) {
        return type_bool();
    } else if (name_len == 3 && strncmp(type_name, "int", 3) == 0) {
        return type_int();
    } else if (name_len == 3 && strncmp(type_name, "f32", 3) == 0) {
        return type_f32();
    } else if (name_len == 3 && strncmp(type_name, "f64", 3) == 0) {
        return type_f64();
    } else if (name_len == 6 && strncmp(type_name, "string", 6) == 0) {
        return type_string();

    } else if (name_len == 5 && strncmp(type_name, "float", 5) == 0) {
        return type_f32();
    }
    
    return type_int();
}

int get_string_index(gen_ctx* ctx, const char* value) {
    for (size_t i = 0; i < ctx->string_count; i++) {
        if (strcmp(ctx->strings[i].value, value) == 0) {
            return (int)i;
        }
    }
    return -1;
}

void gen_collect_local_vars(gen_ctx* ctx, csq_node* node) {
    if (!node) return;
    
    if (node->type == NODE_BLOCK) {
        for (size_t i = 0; i < node->data.block.statements.count; i++) {
            csq_node* stmt = node->data.block.statements.items[i];
            if (stmt->type == NODE_VAR_DECL) {
                const char* name = stmt->data.var_decl.name->data.identifier.name;
                csq_type* type = stmt->type_info;
                
                if (!type && stmt->data.var_decl.type_spec) {
                    type = resolve_type_spec(stmt->data.var_decl.type_spec);
                    stmt->type_info = type;
                }
                
                if (!type) {
                    type = type_int();
                    stmt->type_info = type;
                }
                
                var_table_add(ctx->vars, name, type);
            } else if (stmt->type == NODE_BLOCK) {
                gen_collect_local_vars(ctx, stmt);
            }
        }
    }
}
