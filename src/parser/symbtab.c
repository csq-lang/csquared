/**
 * @file symbtab.c
 * @brief Symbol table management for semantic analysis
 * @details Implements symbol table operations including scope management,
 * symbol declaration and lookup, and usage tracking for semantic validation.
 */

#include <parser/symbtab.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Hash function for symbol names
 * @param name Symbol name
 * @param len Length of name
 * @return Hash index in range [0, SYM_HASH_SIZE)
 */
static unsigned int sym_hash(const char *name, size_t len) {
  unsigned int hash = 5381;
  for (size_t i = 0; i < len; i++) {
    hash = ((hash << 5) + hash) + (unsigned char)name[i];
  }
  return hash & (SYM_HASH_SIZE - 1);
}

/**
 * @brief Create a new symbol table
 * @details Initializes the global scope and sets up for nested scope
 * management.
 * @return Allocated symbol table
 */
sym_table *sym_table_create(void) {
  sym_table *table = malloc(sizeof(sym_table));
  if (!table)
    return NULL;

  table->current = NULL;
  table->global = NULL;
  table->scope_count = 0;

  sym_enter_scope(table);
  table->global = table->current;

  return table;
}

/**
 * @brief Free symbol table and all scopes
 * @param table Symbol table to deallocate
 */
void sym_table_free(sym_table *table) {
  if (!table)
    return;

  while (table->current) {
    sym_scope *parent = table->current->parent;
    sym_scope_free(table->current);
    table->current = parent;
  }

  free(table);
}

sym_scope *sym_scope_create(sym_scope *parent) {
  sym_scope *scope = calloc(1, sizeof(sym_scope));
  if (!scope)
    return NULL;

  scope->hash_table = calloc(SYM_HASH_SIZE, sizeof(struct sym_hash_bucket));
  if (!scope->hash_table) {
    free(scope);
    return NULL;
  }

  scope->parent = parent;
  scope->symbol_count = 0;
  scope->level = parent ? parent->level + 1 : 0;
  scope->is_function = false;
  scope->is_loop = false;
  scope->is_switch = false;

  return scope;
}

void sym_scope_free(sym_scope *scope) {
  if (!scope)
    return;

  if (scope->hash_table) {
    for (int i = 0; i < SYM_HASH_SIZE; i++) {
      struct csq_symbol *sym = scope->hash_table[i].symbols;
      while (sym) {
        struct csq_symbol *next = sym->next_in_scope;
        free(sym);
        sym = next;
      }
    }
    free(scope->hash_table);
  }

  free(scope);
}

/**
 * @brief Enter a new scope
 * @param table Symbol table
 */
void sym_enter_scope(sym_table *table) {
  sym_enter_scope_with_flags(table, false, false, false);
}

void sym_enter_scope_with_flags(sym_table *table, bool is_function,
                                bool is_loop, bool is_switch) {
  sym_scope *scope = sym_scope_create(table->current);
  if (!scope)
    return;

  scope->is_function = is_function;
  scope->is_loop = is_loop;
  scope->is_switch = is_switch;

  table->current = scope;
  table->scope_count++;
}

/**
 * @brief Leave current scope and return to parent
 * @param table Symbol table
 */
void sym_leave_scope(sym_table *table) {
  if (!table || !table->current || !table->current->parent)
    return;

  sym_scope *scope = table->current;
  table->current = scope->parent;
  sym_scope_free(scope);
  table->scope_count--;
}

/**
 * @brief Declare a symbol in current scope
 * @param table Symbol table
 * @param kind Symbol kind (variable, function, etc)
 * @param name Symbol name
 * @param len Length of name
 * @param decl Declaration AST node
 * @return Allocated symbol, or NULL if already declared
 */
struct csq_symbol *sym_declare(sym_table *table, symbol_kind kind,
                               const char *name, size_t len, csq_node *decl) {
  if (!table || !table->current)
    return NULL;

  struct csq_symbol *existing = sym_lookup_current(table, name, len);
  if (existing)
    return NULL;

  struct csq_symbol *sym = calloc(1, sizeof(struct csq_symbol));
  if (!sym)
    return NULL;

  unsigned int bucket_idx = sym_hash(name, len);
  struct sym_hash_bucket *bucket = &table->current->hash_table[bucket_idx];

  sym->kind = kind;
  sym->name = name;
  sym->name_len = len;
  sym->decl = decl;
  sym->scope = table->current;

  if (decl) {
    sym->line = decl->line;
    sym->column = decl->column;
  }

  sym->next_in_scope = bucket->symbols;
  bucket->symbols = sym;
  bucket->count++;
  table->current->symbol_count++;

  return sym;
}

/**
 * @brief Look up symbol in current and parent scopes
 * @param table Symbol table
 * @param name Symbol name
 * @param len Length of name
 * @return Found symbol, or NULL if not found
 */
struct csq_symbol *sym_lookup(sym_table *table, const char *name, size_t len) {
  if (!table || !table->current)
    return NULL;
  return sym_lookup_recursive(table->current, name, len);
}

/**
 * @brief Look up symbol in current scope only
 * @param table Symbol table
 * @param name Symbol name
 * @param len Length of name
 * @return Found symbol, or NULL if not in current scope
 */
struct csq_symbol *sym_lookup_current(sym_table *table, const char *name,
                                      size_t len) {
  if (!table || !table->current)
    return NULL;

  unsigned int bucket_idx = sym_hash(name, len);
  struct sym_hash_bucket *bucket = &table->current->hash_table[bucket_idx];

  struct csq_symbol *sym = bucket->symbols;
  while (sym) {
    if (sym->name_len == len && memcmp(sym->name, name, len) == 0) {
      return sym;
    }
    sym = sym->next_in_scope;
  }
  return NULL;
}

struct csq_symbol *sym_lookup_recursive(sym_scope *scope, const char *name,
                                        size_t len) {
  while (scope) {
    unsigned int bucket_idx = sym_hash(name, len);
    struct sym_hash_bucket *bucket = &scope->hash_table[bucket_idx];

    struct csq_symbol *sym = bucket->symbols;
    while (sym) {
      if (sym->name_len == len && memcmp(sym->name, name, len) == 0) {
        return sym;
      }
      sym = sym->next_in_scope;
    }
    scope = scope->parent;
  }
  return NULL;
}

void sym_mark_used(struct csq_symbol *sym) {
  if (sym)
    sym->is_used = true;
}

void sym_mark_initialized(struct csq_symbol *sym) {
  if (sym)
    sym->is_initialized = true;
}

bool sym_is_global(sym_table *table, struct csq_symbol *sym) {
  return sym && sym->scope == table->global;
}

bool sym_is_in_loop(sym_table *table) {
  sym_scope *scope = table->current;
  while (scope) {
    if (scope->is_loop)
      return true;
    if (scope->is_function)
      return false;
    scope = scope->parent;
  }
  return false;
}

bool sym_is_in_function(sym_table *table) {
  sym_scope *scope = table->current;
  while (scope) {
    if (scope->is_function)
      return true;
    scope = scope->parent;
  }
  return false;
}

void sym_check_unused(sym_table *table) {
  if (!table)
    return;

  sym_scope *scope = table->current;
  while (scope) {
    for (int i = 0; i < SYM_HASH_SIZE; i++) {
      struct sym_hash_bucket *bucket = &scope->hash_table[i];
      struct csq_symbol *sym = bucket->symbols;
      while (sym) {
        if (!sym->is_used && sym->kind == SYM_VARIABLE &&
            !sym_is_global(table, sym)) {
        }
        sym = sym->next_in_scope;
      }
    }
    scope = scope->parent;
  }
}
