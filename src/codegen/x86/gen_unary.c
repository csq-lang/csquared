/**
 * @file gen_unary.c
 * @brief Unary operation code generation
 * @details Generates code for unary operators like negation, logical not,
 * bitwise not, and pre/post increment/decrement.
 */

#include <codegen/x86/gen_unary.h>
#include <codegen/x86/gen_expr.h>
#include <codegen/x86/inst.h>
#include <codegen/x86/reg.h>
#include <core/builtins.h>
#include <parser/node.h>

void gen_unary_op(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_UNARY_OP) return;
    
    gen_expression(ctx, node->data.unary.operand);
    
    switch (node->data.unary.op) {
        case UNOP_NEG:
            inst_neg_reg(ctx->emit, REG_EAX);
            break;
        case UNOP_NOT:
            inst_test_reg_reg(ctx->emit, REG_EAX, REG_EAX);
            inst_setne_reg(ctx->emit, REG_EAX);
            inst_xor_reg_imm(ctx->emit, REG_EAX, 1);
            break;
        case UNOP_BIT_NOT:
            inst_not_reg(ctx->emit, REG_EAX);
            break;
        default:
            break;
    }
}

void gen_call(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_CALL) return;
    
    if (node->data.call.callee->type == NODE_IDENTIFIER) {
        const char* name = node->data.call.callee->data.identifier.name;
        if (builtin_is_builtin(name)) {
            builtin_gen_call(ctx, name, node->data.call.args.items, node->data.call.args.count);
            return;
        }
    }
    
    for (int i = node->data.call.args.count - 1; i >= 0; i--) {
        csq_node* arg = node->data.call.args.items[i];
        if (arg->type == NODE_LITERAL_INT) {
            inst_push_imm(ctx->emit, (int)arg->data.literal_int.value);
        } else {
            gen_expression(ctx, arg);
            inst_push_reg(ctx->emit, REG_EAX);
        }
    }
    
    if (node->data.call.callee->type == NODE_IDENTIFIER) {
        const char* name = node->data.call.callee->data.identifier.name;
        emit_format(ctx->emit, "    call %s\n", name);
    }
    
    if (node->data.call.args.count > 0) {
        inst_add_reg_imm(ctx->emit, REG_ESP, node->data.call.args.count * 4);
    }
}
