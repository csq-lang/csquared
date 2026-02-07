/**
 * @file const_fold.h
 * @brief Constant folding optimization pass.
 * @details Evaluates constant expressions at compile-time and replaces them with their results.
 * Reduces runtime computation for expressions with only constant operands.
 */

#ifndef MIDDLE_CONST_FOLD_H
#define MIDDLE_CONST_FOLD_H

#include <parser/ast.h>

/**
 * @brief Applies constant folding to an AST node.
 * @details Recursively evaluates constant expressions and replaces them with literal results.
 * @param node The AST node to optimize.
 * @return Optimized node (may be different from input if folded).
 */
csq_node* const_fold_apply(csq_node* node);

/**
 * @brief Checks if a node represents a constant expression.
 * @details Determines if an expression can be evaluated at compile-time.
 * @param node The AST node to check.
 * @return Non-zero if node is constant, zero otherwise.
 */
int const_fold_is_constant(csq_node* node);

/**
 * @brief Folds a binary operation.
 * @details Evaluates a binary operation with constant operands.
 * @param node The binary operation node to fold.
 * @return Optimized node with folded result.
 */
csq_node* const_fold_binary(csq_node* node);

/**
 * @brief Folds a unary operation.
 * @details Evaluates a unary operation with constant operand.
 * @param node The unary operation node to fold.
 * @return Optimized node with folded result.
 */
csq_node* const_fold_unary(csq_node* node);

#endif
