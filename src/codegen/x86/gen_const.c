/**
 * @file gen_const.c
 * @brief Constant/literal code generation
 * @details Generates code for literal values (integers, floats, strings, booleans).
 */

#include <codegen/x86/gen_const.h>
#include <parser/node.h>

bool gen_is_constant(gen_ctx* ctx, csq_node* node) {
    (void)ctx;
    if (!node) return false;
    
    switch (node->type) {
        case NODE_LITERAL_INT:
        case NODE_LITERAL_BOOL:
        case NODE_LITERAL_NULL:
            return true;
        case NODE_BINARY_OP:
            return gen_is_constant(ctx, node->data.binary.left) &&
                   gen_is_constant(ctx, node->data.binary.right);
        case NODE_UNARY_OP:
            return gen_is_constant(ctx, node->data.unary.operand);
        default:
            return false;
    }
}

long long gen_fold_constant(gen_ctx* ctx, csq_node* node) {
    (void)ctx;
    if (!node) return 0;
    
    switch (node->type) {
        case NODE_LITERAL_INT:
            return node->data.literal_int.value;
        case NODE_LITERAL_BOOL:
            return node->data.literal_bool.value ? 1 : 0;
        case NODE_LITERAL_NULL:
            return 0;
        case NODE_BINARY_OP: {
            long long left = gen_fold_constant(ctx, node->data.binary.left);
            long long right = gen_fold_constant(ctx, node->data.binary.right);
            switch (node->data.binary.op) {
                case BINOP_ADD: return left + right;
                case BINOP_SUB: return left - right;
                case BINOP_MUL: return left * right;
                case BINOP_DIV: return right != 0 ? left / right : 0;
                case BINOP_MOD: return right != 0 ? left % right : 0;
                case BINOP_AND: return left && right;
                case BINOP_OR: return left || right;
                case BINOP_BIT_AND: return left & right;
                case BINOP_BIT_OR: return left | right;
                case BINOP_BIT_XOR: return left ^ right;
                case BINOP_SHL: return left << right;
                case BINOP_SHR: return left >> right;
                case BINOP_EQ: return left == right;
                case BINOP_NE: return left != right;
                case BINOP_LT: return left < right;
                case BINOP_LE: return left <= right;
                case BINOP_GT: return left > right;
                case BINOP_GE: return left >= right;
                default: return 0;
            }
        }
        case NODE_UNARY_OP: {
            long long val = gen_fold_constant(ctx, node->data.unary.operand);
            switch (node->data.unary.op) {
                case UNOP_NEG: return -val;
                case UNOP_NOT: return !val;
                case UNOP_BIT_NOT: return ~val;
                default: return val;
            }
        }
        default:
            return 0;
    }
}
