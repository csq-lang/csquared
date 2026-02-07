/**
 * @file strength_reduce.c
 * @brief Strength reduction optimization
 * @details Replaces expensive operations with cheaper alternatives
 * (e.g., multiplication by power of 2 becomes bit shift).
 */

#include <middle/strength_reduce.h>
#include <parser/node.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief Check if node is a strength reduction candidate
 * @param node Node to check
 * @return True if node can be strength reduced
 */
int strength_reduce_is_candidate(csq_node* node)
{
    if (!node || node->type != NODE_BINARY_OP)
        return 0;
    
    if (node->data.binary.op != BINOP_MUL && node->data.binary.op != BINOP_DIV)
        return 0;
    
    return (node->data.binary.left->type == NODE_LITERAL_INT && node->data.binary.op == BINOP_MUL) ||
           (node->data.binary.right->type == NODE_LITERAL_INT && (node->data.binary.op == BINOP_MUL || node->data.binary.op == BINOP_DIV));
}

static int is_power_of_two(long long val)
{
    if (val <= 0)
        return 0;
    return (val & (val - 1)) == 0;
}

static int log2_int(long long val)
{
    int count = 0;
    while (val > 1) {
        val >>= 1;
        count++;
    }
    return count;
}

csq_node* strength_reduce_mul_by_const(csq_node* node)
{
    if (!node || node->type != NODE_BINARY_OP || node->data.binary.op != BINOP_MUL)
        return node;
    
    long long val = 0;
    csq_node* operand = NULL;
    
    if (node->data.binary.right->type == NODE_LITERAL_INT) {
        val = node->data.binary.right->data.literal_int.value;
        operand = node->data.binary.left;
    } else if (node->data.binary.left->type == NODE_LITERAL_INT) {
        val = node->data.binary.left->data.literal_int.value;
        operand = node->data.binary.right;
    } else {
        return node;
    }
    
    if (val == 1)
        return operand;
    
    if (val == 0)
        return ast_create_literal_int(0);
    
    if (is_power_of_two(val)) {
        int shift_amount = log2_int(val);
        csq_node* shift_node = ast_create_literal_int(shift_amount);
        return ast_create_binary(BINOP_SHL, operand, shift_node);
    }
    
    return node;
}

csq_node* strength_reduce_div_by_const(csq_node* node)
{
    if (!node || node->type != NODE_BINARY_OP || node->data.binary.op != BINOP_DIV)
        return node;
    
    if (node->data.binary.right->type != NODE_LITERAL_INT)
        return node;
    
    long long val = node->data.binary.right->data.literal_int.value;
    
    if (val == 1)
        return node->data.binary.left;
    
    if (is_power_of_two(val)) {
        int shift_amount = log2_int(val);
        csq_node* shift_node = ast_create_literal_int(shift_amount);
        return ast_create_binary(BINOP_SHR, node->data.binary.left, shift_node);
    }
    
    return node;
}

csq_node* strength_reduce_apply(csq_node* node)
{
    if (!node)
        return node;
    
    if (node->type == NODE_BINARY_OP) {
        node->data.binary.left = strength_reduce_apply(node->data.binary.left);
        node->data.binary.right = strength_reduce_apply(node->data.binary.right);
        
        if (!strength_reduce_is_candidate(node))
            return node;
        
        if (node->data.binary.op == BINOP_MUL)
            return strength_reduce_mul_by_const(node);
        
        if (node->data.binary.op == BINOP_DIV)
            return strength_reduce_div_by_const(node);
    }
    
    return node;
}
