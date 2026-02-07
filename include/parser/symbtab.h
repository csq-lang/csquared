/**
 * @file symbtab.h
 * @brief Symbol table and scope management for the C² parser.
 * @details Implements symbol tables, scoping, and name resolution for semantic analysis.
 * Supports multiple scopes with proper shadowing and symbol lookups.
 */

#ifndef PARSER_SYMBTAB_H
#define PARSER_SYMBTAB_H

#include <stddef.h>
#include <stdbool.h>
#include <parser/types.h>
#include <parser/node.h>

/**
 * @brief Symbol kind enumeration.
 * @details Categorizes different types of symbols that can appear in a program.
 */
typedef enum {
    SYM_VARIABLE,           /**< Variable symbol */
    SYM_CONSTANT,           /**< Constant symbol */
    SYM_FUNCTION,           /**< Function symbol */
    SYM_PARAMETER,          /**< Function parameter symbol */
    SYM_STRUCT,             /**< Struct type symbol */
    SYM_ENUM,               /**< Enumeration symbol */
    SYM_ENUM_VARIANT,       /**< Enum variant symbol */
    SYM_FIELD,              /**< Struct field symbol */
    SYM_TYPE_ALIAS,         /**< Type alias symbol */
    SYM_GENERIC_PARAM,      /**< Generic type parameter symbol */
    SYM_LABEL,              /**< Jump label symbol */
    SYM_IMPORT,             /**< Import statement symbol */
    SYM_MODULE              /**< Module symbol */
} symbol_kind;

typedef struct sym_scope sym_scope;

/**
 * @brief Symbol table entry.
 * @details Represents a single symbol in the symbol table with its properties,
 * type information, and scope information.
 */
struct csq_symbol {
    symbol_kind kind;           /**< Type of symbol */
    const char* name;           /**< Symbol name */
    size_t name_len;            /**< Length of symbol name */
    csq_type* type;             /**< Symbol type information */
    csq_node* decl;             /**< Pointer to declaration node */
    sym_scope* scope;           /**< Scope where symbol is declared */
    
    bool is_public;             /**< Visibility flag */
    bool is_mutable;            /**< Mutability flag */
    bool is_used;               /**< Usage tracking flag */
    bool is_initialized;        /**< Initialization status flag */
    
    size_t line;                /**< Declaration line number */
    size_t column;              /**< Declaration column number */
    
    struct csq_symbol* next;            /**< Hash table collision link */
    struct csq_symbol* next_in_scope;   /**< Scope chain link */
};

/**
 * @brief Symbol scope.
 * @details Represents a scope level containing a symbol table with hash buckets.
 * Supports nested scopes with parent pointers.
 */
struct sym_scope {
    struct sym_scope* parent;           /**< Parent scope */
    struct sym_hash_bucket* hash_table; /**< Hash table for symbols in this scope */
    size_t symbol_count;                /**< Number of symbols in this scope */
    
    size_t level;                       /**< Scope nesting level */
    bool is_function;                   /**< Function scope flag */
    bool is_loop;                       /**< Loop scope flag */
    bool is_switch;                     /**< Switch scope flag */
};

/**
 * @def SYM_HASH_SIZE
 * @brief Size of symbol table hash table.
 * @details Number of hash buckets for symbol lookup.
 */
#define SYM_HASH_SIZE 128

/**
 * @brief Hash bucket for symbol collision handling.
 * @details Bucket in hash table containing symbols that hash to the same index.
 */
struct sym_hash_bucket {
    struct csq_symbol* symbols;  /**< Linked list of symbols in this bucket */
    size_t count;                /**< Number of symbols in this bucket */
};

/**
 * @brief Symbol table context.
 * @details Main symbol table structure managing all scopes during compilation.
 */
typedef struct {
    struct sym_scope* current;  /**< Current active scope */
    struct sym_scope* global;   /**< Global scope */
    size_t scope_count;         /**< Total number of scopes created */
} sym_table;

/**
 * @brief Creates a new symbol table.
 * @details Initializes an empty symbol table with global scope.
 * @return Pointer to newly allocated symbol table.
 */
sym_table* sym_table_create(void);

/**
 * @brief Frees a symbol table and all its scopes.
 * @details Deallocates all memory associated with the symbol table.
 * @param table The symbol table to free. Safe to call with NULL.
 */
void sym_table_free(sym_table* table);

/**
 * @brief Creates a new scope.
 * @details Allocates and initializes a scope as a child of the parent.
 * @param parent The parent scope (NULL for global scope).
 * @return Pointer to newly allocated scope.
 */
struct sym_scope* sym_scope_create(struct sym_scope* parent);

/**
 * @brief Frees a scope and all its symbols.
 * @details Deallocates the scope and all contained symbol entries.
 * @param scope The scope to free. Safe to call with NULL.
 */
void sym_scope_free(struct sym_scope* scope);

/**
 * @brief Enters a new scope.
 * @details Creates and enters a new child scope of the current scope.
 * @param table The symbol table.
 */
void sym_enter_scope(sym_table* table);

/**
 * @brief Enters a new scope with specific context flags.
 * @details Creates and enters a new scope with flags indicating context type.
 * @param table The symbol table.
 * @param is_function True if entering a function scope.
 * @param is_loop True if entering a loop scope.
 * @param is_switch True if entering a switch scope.
 */
void sym_enter_scope_with_flags(sym_table* table, bool is_function, bool is_loop, bool is_switch);

/**
 * @brief Leaves the current scope.
 * @details Pops the current scope and restores the parent scope as current.
 * @param table The symbol table.
 */
void sym_leave_scope(sym_table* table);

/**
 * @brief Declares a new symbol in the current scope.
 * @details Adds a new symbol to the current scope's symbol table.
 * @param table The symbol table.
 * @param kind The kind of symbol being declared.
 * @param name The symbol name.
 * @param len The length of the name.
 * @param decl The declaration node for this symbol.
 * @return Pointer to the newly declared symbol.
 */
struct csq_symbol* sym_declare(sym_table* table, symbol_kind kind, const char* name, size_t len, csq_node* decl);

/**
 * @brief Looks up a symbol in the symbol table.
 * @details Searches from the current scope outward to outer scopes.
 * @param table The symbol table.
 * @param name The symbol name to search for.
 * @param len The length of the name.
 * @return Pointer to the symbol if found, NULL otherwise.
 */
struct csq_symbol* sym_lookup(sym_table* table, const char* name, size_t len);

/**
 * @brief Looks up a symbol only in the current scope.
 * @details Searches only the current scope, not outer scopes.
 * @param table The symbol table.
 * @param name The symbol name to search for.
 * @param len The length of the name.
 * @return Pointer to the symbol if found in current scope, NULL otherwise.
 */
struct csq_symbol* sym_lookup_current(sym_table* table, const char* name, size_t len);

/**
 * @brief Recursively looks up a symbol starting from a specific scope.
 * @details Searches the given scope and all parent scopes.
 * @param scope The scope to start searching from.
 * @param name The symbol name to search for.
 * @param len The length of the name.
 * @return Pointer to the symbol if found, NULL otherwise.
 */
struct csq_symbol* sym_lookup_recursive(struct sym_scope* scope, const char* name, size_t len);

/**
 * @brief Marks a symbol as used.
 * @details Records that a symbol has been referenced.
 * @param sym The symbol to mark.
 */
void sym_mark_used(struct csq_symbol* sym);

/**
 * @brief Marks a symbol as initialized.
 * @details Records that a symbol has been initialized with a value.
 * @param sym The symbol to mark.
 */
void sym_mark_initialized(struct csq_symbol* sym);

/**
 * @brief Checks if a symbol is in global scope.
 * @details Determines whether a symbol was declared at global scope level.
 * @param table The symbol table.
 * @param sym The symbol to check.
 * @return True if symbol is global, false otherwise.
 */
bool sym_is_global(sym_table* table, struct csq_symbol* sym);

/**
 * @brief Checks if currently in a loop scope.
 * @details Determines if the current scope is within a loop.
 * @param table The symbol table.
 * @return True if in a loop scope, false otherwise.
 */
bool sym_is_in_loop(sym_table* table);

/**
 * @brief Checks if currently in a function scope.
 * @details Determines if the current scope is within a function.
 * @param table The symbol table.
 * @return True if in a function scope, false otherwise.
 */
bool sym_is_in_function(sym_table* table);

/**
 * @brief Checks for unused symbols in the table.
 * @details Reports warnings for symbols that were declared but never used.
 * @param table The symbol table.
 */
void sym_check_unused(sym_table* table);

#endif