/**
 * @file dead_code.h
 * @brief Dead Code Elimination (DCE) optimization pass.
 * @details Removes unused variables and unreachable code to reduce binary size
 * and improve runtime performance.
 */

#ifndef MIDDLE_DEAD_CODE_H
#define MIDDLE_DEAD_CODE_H

#include <parser/ast.h>

/**
 * @brief Live variable analysis state.
 * @details Tracks which variables are used (live) at different program points.
 */
typedef struct {
    csq_node** live_vars;   /**< Array of variables that are currently live */
    int live_count;         /**< Number of live variables */
    int capacity;           /**< Allocated capacity for live variables */
} live_analysis;

/**
 * @brief Creates a live analysis state.
 * @details Allocates and initializes a live analysis structure.
 * @return Pointer to newly allocated live analysis state.
 */
live_analysis* live_analysis_create(void);

/**
 * @brief Frees a live analysis state.
 * @details Deallocates the live analysis structure.
 * @param la The live analysis to free. Safe to call with NULL.
 */
void live_analysis_free(live_analysis* la);

/**
 * @brief Runs liveness analysis on an AST.
 * @details Analyzes which variables are used throughout the program.
 * @param node The root AST node to analyze.
 * @param la The live analysis state to populate.
 * @return The number of live variables found, or negative on error.
 */
int live_analysis_run(csq_node* node, live_analysis* la);

/**
 * @brief Eliminates dead code from an AST.
 * @details Removes unused variables and unreachable statements.
 * @param node The root AST node to optimize.
 * @return Optimized AST with dead code removed.
 */
csq_node* dead_code_eliminate(csq_node* node);

#endif
