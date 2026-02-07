/**
 * @file gen_string.c
 * @brief String literal and manipulation code generation
 * @details Generates code for string operations and string constant data.
 */

#include <codegen/x86/gen_string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char* gen_add_string(gen_ctx* ctx, const char* value) {
    if (!ctx || !value) return NULL;
    
    for (size_t i = 0; i < ctx->string_count; i++) {
        if (strcmp(ctx->strings[i].value, value) == 0) {
            return ctx->strings[i].label;
        }
    }
    
    if (ctx->string_count >= ctx->string_capacity) {
        size_t new_capacity = ctx->string_capacity * 2;
        string_constant* new_strings = realloc(ctx->strings, sizeof(string_constant) * new_capacity);
        if (!new_strings) return NULL;
        ctx->strings = new_strings;
        ctx->string_capacity = new_capacity;
    }
    
    static char label[32];
    snprintf(label, sizeof(label), ".str%d", ctx->string_counter++);
    
    ctx->strings[ctx->string_count].label = strdup(label);
    ctx->strings[ctx->string_count].value = strdup(value);
    ctx->string_count++;
    
    return ctx->strings[ctx->string_count - 1].label;
}

const char* gen_get_string_label(gen_ctx* ctx) {
    if (!ctx) return NULL;
    static char label[32];
    snprintf(label, sizeof(label), ".str%d", ctx->string_counter++);
    return label;
}

void gen_collect_strings(gen_ctx* ctx, csq_node* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_LITERAL_STRING:
            gen_add_string(ctx, node->data.literal_string.value);
            break;
        case NODE_PROGRAM:
            for (size_t i = 0; i < node->data.program.statements.count; i++) {
                gen_collect_strings(ctx, node->data.program.statements.items[i]);
            }
            break;
        case NODE_BLOCK:
            for (size_t i = 0; i < node->data.block.statements.count; i++) {
                gen_collect_strings(ctx, node->data.block.statements.items[i]);
            }
            break;
        case NODE_VAR_DECL:
            gen_collect_strings(ctx, node->data.var_decl.init);
            break;
        case NODE_BINARY_OP:
            gen_collect_strings(ctx, node->data.binary.left);
            gen_collect_strings(ctx, node->data.binary.right);
            break;
        case NODE_UNARY_OP:
            gen_collect_strings(ctx, node->data.unary.operand);
            break;
        case NODE_IF:
            gen_collect_strings(ctx, node->data.if_stmt.condition);
            gen_collect_strings(ctx, node->data.if_stmt.then_branch);
            gen_collect_strings(ctx, node->data.if_stmt.else_branch);
            break;
        case NODE_WHILE:
            gen_collect_strings(ctx, node->data.while_stmt.condition);
            gen_collect_strings(ctx, node->data.while_stmt.body);
            break;
        case NODE_RETURN:
            gen_collect_strings(ctx, node->data.return_stmt.value);
            break;
        case NODE_CALL:
            gen_collect_strings(ctx, node->data.call.callee);
            for (size_t i = 0; i < node->data.call.args.count; i++) {
                gen_collect_strings(ctx, node->data.call.args.items[i]);
            }
            break;
        default:
            break;
    }
}
