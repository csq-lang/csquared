/**
 * @file optimizer.h
 * @brief Middle-end optimizer framework for the C² compiler.
 * @details Provides optimization pipeline management and configuration.
 * Controls various optimization passes including constant folding, dead code elimination,
 * common subexpression elimination, and strength reduction.
 */

#ifndef MIDDLE_OPTIMIZER_H
#define MIDDLE_OPTIMIZER_H

#include <parser/ast.h>

/**
 * @brief Optimization configuration flags.
 * @details Enables/disables specific optimization passes and sets optimization level.
 */
typedef struct {
    int enable_const_folding;       /**< Enable constant folding optimization */
    int enable_dce;                 /**< Enable dead code elimination */
    int enable_cse;                 /**< Enable common subexpression elimination */
    int enable_strength_reduction;  /**< Enable strength reduction optimization */
    int enable_inlining;            /**< Enable function inlining */
    int enable_loop_unroll;         /**< Enable loop unrolling */
    int enable_alias_analysis;      /**< Enable alias analysis */
    int opt_level;                  /**< Overall optimization level (0-3) */
} optimizer_config;

/**
 * @brief Optimizer state and context.
 * @details Maintains state during optimization passes including AST and configuration.
 */
typedef struct optimizer_state {
    ast_context* ast;               /**< AST being optimized */
    optimizer_config config;        /**< Optimization configuration */
    int changed;                    /**< Flag indicating if AST was modified */
} optimizer_state;

/**
 * @brief Creates an optimizer state for a given AST.
 * @details Allocates and initializes an optimizer with default configuration.
 * @param ast The AST to be optimized.
 * @return Pointer to newly allocated optimizer state.
 */
optimizer_state* optimizer_create(ast_context* ast);

/**
 * @brief Frees optimizer state.
 * @details Deallocates the optimizer state.
 * @param opt The optimizer state to free. Safe to call with NULL.
 */
void optimizer_free(optimizer_state* opt);

/**
 * @brief Configures optimization level and passes.
 * @details Sets which optimization passes are enabled based on optimization level.
 * @param opt The optimizer state.
 * @param opt_level Optimization level (0=none, 1=basic, 2=moderate, 3=aggressive).
 */
void optimizer_configure(optimizer_state* opt, int opt_level);

/**
 * @brief Runs all enabled optimization passes.
 * @details Executes the optimization pipeline on the AST.
 * @param opt The optimizer state.
 * @return The number of optimizations applied, or negative on error.
 */
int optimizer_run(optimizer_state* opt);

#endif
