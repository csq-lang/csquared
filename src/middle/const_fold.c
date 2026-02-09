/**
 * @file const_fold.c
 * @brief Constant folding optimization
 * @details Evaluates constant expressions at compile time and replaces them
 * with their computed values.
 */

#include <middle/const_fold.h>
#include <parser/ast.h>
#include <parser/node.h>
#include <stdlib.h>

/**
 * @brief Check if node is a constant value
 * @param node Node to check
 * @return True if node is a literal constant
 */
int const_fold_is_constant(csq_node *node) {
  if (!node)
    return 0;

  return node->type == NODE_LITERAL_INT || node->type == NODE_LITERAL_FLOAT ||
         node->type == NODE_LITERAL_STRING || node->type == NODE_LITERAL_BOOL ||
         node->type == NODE_LITERAL_NIL;
}

csq_node *const_fold_binary(csq_node *node) {
  if (!node || node->type != NODE_BINARY_OP)
    return node;

  csq_node *left = node->data.binary.left;
  csq_node *right = node->data.binary.right;

  if (!const_fold_is_constant(left) || !const_fold_is_constant(right))
    return node;

  if (left->type == NODE_LITERAL_INT && right->type == NODE_LITERAL_INT) {
    long long left_val = left->data.literal_int.value;
    long long right_val = right->data.literal_int.value;
    long long result = 0;
    int valid = 1;

    switch (node->data.binary.op) {
    case BINOP_ADD:
      result = left_val + right_val;
      break;
    case BINOP_SUB:
      result = left_val - right_val;
      break;
    case BINOP_MUL:
      result = left_val * right_val;
      break;
    case BINOP_DIV:
      if (right_val != 0)
        result = left_val / right_val;
      else
        valid = 0;
      break;
    case BINOP_MOD:
      if (right_val != 0)
        result = left_val % right_val;
      else
        valid = 0;
      break;
    case BINOP_BIT_AND:
      result = left_val & right_val;
      break;
    case BINOP_BIT_OR:
      result = left_val | right_val;
      break;
    case BINOP_BIT_XOR:
      result = left_val ^ right_val;
      break;
    case BINOP_SHL:
      result = left_val << right_val;
      break;
    case BINOP_SHR:
      result = left_val >> right_val;
      break;
    default:
      valid = 0;
    }

    if (valid)
      return ast_create_literal_int(result);
  }

  if (left->type == NODE_LITERAL_FLOAT && right->type == NODE_LITERAL_FLOAT) {
    double left_val = left->data.literal_float.value;
    double right_val = right->data.literal_float.value;
    double result = 0.0;
    int valid = 1;

    switch (node->data.binary.op) {
    case BINOP_ADD:
      result = left_val + right_val;
      break;
    case BINOP_SUB:
      result = left_val - right_val;
      break;
    case BINOP_MUL:
      result = left_val * right_val;
      break;
    case BINOP_DIV:
      if (right_val != 0.0)
        result = left_val / right_val;
      else
        valid = 0;
      break;
    default:
      valid = 0;
    }

    if (valid)
      return ast_create_literal_float(result);
  }

  return node;
}

csq_node *const_fold_unary(csq_node *node) {
  if (!node || node->type != NODE_UNARY_OP)
    return node;

  csq_node *operand = node->data.unary.operand;

  if (!const_fold_is_constant(operand))
    return node;

  if (operand->type == NODE_LITERAL_INT) {
    long long val = operand->data.literal_int.value;
    long long result = 0;

    switch (node->data.unary.op) {
    case UNOP_NEG:
      result = -val;
      return ast_create_literal_int(result);
    case UNOP_NOT:
      result = !val;
      return ast_create_literal_int(result);
    case UNOP_BIT_NOT:
      result = ~val;
      return ast_create_literal_int(result);
    default:
      break;
    }
  }

  if (operand->type == NODE_LITERAL_FLOAT) {
    double val = operand->data.literal_float.value;

    if (node->data.unary.op == UNOP_NEG)
      return ast_create_literal_float(-val);
  }

  if (operand->type == NODE_LITERAL_BOOL) {
    int val = operand->data.literal_bool.value;

    if (node->data.unary.op == UNOP_NOT)
      return ast_create_literal_bool(!val);
  }

  return node;
}

csq_node *const_fold_apply(csq_node *node) {
  if (!node)
    return node;

  if (node->type == NODE_BINARY_OP) {
    if (node->data.binary.left)
      node->data.binary.left = const_fold_apply(node->data.binary.left);
    if (node->data.binary.right)
      node->data.binary.right = const_fold_apply(node->data.binary.right);
    return const_fold_binary(node);
  }

  if (node->type == NODE_UNARY_OP) {
    if (node->data.unary.operand)
      node->data.unary.operand = const_fold_apply(node->data.unary.operand);
    return const_fold_unary(node);
  }

  return node;
}
