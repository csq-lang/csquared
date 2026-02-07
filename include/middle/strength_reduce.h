/**
 * @file strength_reduce.h
 * @brief Strength Reduction optimization pass.
 * @details Replaces expensive operations with cheaper equivalent operations.
 * For example, multiplication by power of 2 becomes bit shift operation.
 */

#ifndef MIDDLE_STRENGTH_REDUCE_H
#define MIDDLE_STRENGTH_REDUCE_H

#include <parser/ast.h>

/**
 * @brief Applies strength reduction to an AST node.
 * @details Analyzes operations and replaces expensive ones with cheaper equivalents.
 * @param node The AST node to optimize.
 * @return Optimized node with reduced-strength operations.
 */
csq_node* strength_reduce_apply(csq_node* node);

/**
 * @brief Checks if a node is a candidate for strength reduction.
 * @details Determines if an operation can be replaced with a cheaper one.
 * @param node The AST node to check.
 * @return Non-zero if node can be optimized, zero otherwise.
 */
int strength_reduce_is_candidate(csq_node* node);

/**
 * @brief Reduces multiplication by a constant.
 * @details Replaces multiplication by power-of-2 with bit shift when beneficial.
 * @param node The multiplication operation node.
 * @return Optimized node with reduced strength operation.
 */
csq_node* strength_reduce_mul_by_const(csq_node* node);

/**
 * @brief Reduces division by a constant.
 * @details Replaces division by power-of-2 with bit shift when beneficial.
 * @param node The division operation node.
 * @return Optimized node with reduced strength operation.
 */
csq_node* strength_reduce_div_by_const(csq_node* node);

#endif
