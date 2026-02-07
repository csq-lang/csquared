/**
 * @file cse.h
 * @brief Common Subexpression Elimination (CSE) optimization pass.
 * @details Identifies and eliminates redundant expression computations by reusing
 * results of identical subexpressions.
 */

#ifndef MIDDLE_CSE_H
#define MIDDLE_CSE_H

#include <parser/ast.h>

/**
 * @brief Entry in the CSE table.
 * @details Maps a subexpression to a temporary variable holding its result.
 */
typedef struct {
    csq_node* expr;         /**< The subexpression */
    csq_node* temp_var;     /**< Temporary variable holding the result */
} cse_entry;

/**
 * @brief CSE lookup table.
 * @details Stores mappings of subexpressions to temporary variables.
 */
typedef struct {
    cse_entry* entries;     /**< Array of CSE entries */
    int count;              /**< Number of entries currently stored */
    int capacity;           /**< Allocated capacity for entries */
} cse_table;

/**
 * @brief Creates a new CSE table.
 * @details Allocates and initializes an empty CSE table.
 * @return Pointer to newly allocated CSE table.
 */
cse_table* cse_table_create(void);

/**
 * @brief Frees a CSE table.
 * @details Deallocates the CSE table and all its entries.
 * @param table The table to free. Safe to call with NULL.
 */
void cse_table_free(cse_table* table);

/**
 * @brief Inserts a subexpression mapping into the table.
 * @details Records that an expression's result is stored in a temporary variable.
 * @param table The CSE table.
 * @param expr The subexpression.
 * @param temp The temporary variable holding the result.
 * @return Index of inserted entry, or negative on error.
 */
int cse_table_insert(cse_table* table, csq_node* expr, csq_node* temp);

/**
 * @brief Looks up a subexpression in the CSE table.
 * @details Finds if an identical expression has already been computed.
 * @param table The CSE table.
 * @param expr The subexpression to look up.
 * @return Pointer to temporary variable if found, NULL otherwise.
 */
csq_node* cse_table_lookup(cse_table* table, csq_node* expr);

/**
 * @brief Performs common subexpression elimination on an AST.
 * @details Analyzes and optimizes the AST to eliminate redundant computations.
 * @param node The root AST node to optimize.
 * @return Optimized AST node.
 */
csq_node* common_subexpr_eliminate(csq_node* node);

#endif
