#ifndef PARSER_SYMBTAB_H
#define PARSER_SYMBTAB_H

#include <parser/node.h>
#include <parser/types.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
  SYM_VARIABLE,
  SYM_CONSTANT,
  SYM_FUNCTION,
  SYM_PARAMETER,
  SYM_STRUCT,
  SYM_ENUM,
  SYM_ENUM_VARIANT,
  SYM_FIELD,
  SYM_TYPE_ALIAS,
  SYM_GENERIC_PARAM,
  SYM_LABEL,
  SYM_IMPORT,
  SYM_MODULE
} symbol_kind;

typedef struct sym_scope sym_scope;

struct rvn_symbol {
  symbol_kind kind;
  const char *name;
  size_t name_len;
  rvn_type *type;
  rvn_node *decl;
  sym_scope *scope;

  bool is_public;
  bool is_mutable;
  bool is_used;
  bool is_initialized;

  size_t line;
  size_t column;

  struct rvn_symbol *next;
  struct rvn_symbol *next_in_scope;
};

struct sym_scope {
  struct sym_scope *parent;
  struct rvn_symbol *symbols;
  size_t symbol_count;

  size_t level;
  bool is_function;
  bool is_loop;
  bool is_switch;
};

typedef struct {
  struct sym_scope *current;
  struct sym_scope *global;
  size_t scope_count;
} sym_table;

sym_table *sym_table_create(void);
void sym_table_free(sym_table *table);

struct sym_scope *sym_scope_create(struct sym_scope *parent);
void sym_scope_free(struct sym_scope *scope);

void sym_enter_scope(sym_table *table);
void sym_enter_scope_with_flags(sym_table *table, bool is_function,
                                bool is_loop, bool is_switch);
void sym_leave_scope(sym_table *table);

struct rvn_symbol *sym_declare(sym_table *table, symbol_kind kind,
                               const char *name, size_t len, rvn_node *decl);
struct rvn_symbol *sym_lookup(sym_table *table, const char *name, size_t len);
struct rvn_symbol *sym_lookup_current(sym_table *table, const char *name,
                                      size_t len);
struct rvn_symbol *sym_lookup_recursive(struct sym_scope *scope,
                                        const char *name, size_t len);

void sym_mark_used(struct rvn_symbol *sym);
void sym_mark_initialized(struct rvn_symbol *sym);

bool sym_is_global(sym_table *table, struct rvn_symbol *sym);
bool sym_is_in_loop(sym_table *table);
bool sym_is_in_function(sym_table *table);

void sym_check_unused(sym_table *table);

#endif
