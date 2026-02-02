#ifndef PARSER_NODE_H
#define PARSER_NODE_H

#include <raven/token.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct rvn_node rvn_node;

typedef enum {
  NODE_PROGRAM,
  NODE_BLOCK,
  NODE_VAR_DECL,
  NODE_CONST_DECL,
  NODE_FUNCTION_DECL,
  NODE_PARAM,
  NODE_STRUCT_DECL,
  NODE_ENUM_DECL,
  NODE_ENUM_VARIANT,
  NODE_FIELD,
  NODE_IF,
  NODE_SWITCH,
  NODE_CASE,
  NODE_WHILE,
  NODE_FOR,
  NODE_REPEAT,
  NODE_RETURN,
  NODE_THROW,
  NODE_TRY,
  NODE_CATCH,
  NODE_DEFER,
  NODE_BREAK,
  NODE_CONTINUE,
  NODE_ASSIGN,
  NODE_BINARY_OP,
  NODE_UNARY_OP,
  NODE_CALL,
  NODE_INDEX,
  NODE_ACCESS,
  NODE_IDENTIFIER,
  NODE_LITERAL_INT,
  NODE_LITERAL_FLOAT,
  NODE_LITERAL_STRING,
  NODE_LITERAL_BOOL,
  NODE_LITERAL_NULL,
  NODE_LITERAL_TAG,
  NODE_ARRAY_LITERAL,
  NODE_MAP_LITERAL,
  NODE_MAP_ENTRY,
  NODE_SPAWN,
  NODE_IMPORT,
  NODE_SELF,
  NODE_CAST,
  NODE_TYPE_SPEC,
  NODE_GENERIC_INST,
  NODE_RANGE,
  NODE_TUPLE,
  NODE_EXPR_STMT,
  NODE_EMPTY
} node_type;

typedef enum {
  BINOP_ADD,
  BINOP_SUB,
  BINOP_MUL,
  BINOP_DIV,
  BINOP_MOD,
  BINOP_POW,
  BINOP_EQ,
  BINOP_NE,
  BINOP_LT,
  BINOP_GT,
  BINOP_LE,
  BINOP_GE,
  BINOP_AND,
  BINOP_OR,
  BINOP_BIT_AND,
  BINOP_BIT_OR,
  BINOP_BIT_XOR,
  BINOP_SHL,
  BINOP_SHR,
  BINOP_ASSIGN,
  BINOP_ADD_ASSIGN,
  BINOP_SUB_ASSIGN,
  BINOP_MUL_ASSIGN,
  BINOP_DIV_ASSIGN,
  BINOP_MOD_ASSIGN
} binary_op;

typedef enum {
  UNOP_NEG,
  UNOP_NOT,
  UNOP_BIT_NOT,
  UNOP_PRE_INC,
  UNOP_PRE_DEC,
  UNOP_POST_INC,
  UNOP_POST_DEC,
  UNOP_DEREF,
  UNOP_ADDR
} unary_op;

typedef struct {
  rvn_node **items;
  size_t count;
  size_t capacity;
} node_list;

struct rvn_node {
  node_type type;
  size_t line;
  size_t column;
  struct rvn_type *type_info;
  struct rvn_symbol *symbol;

  union {
    struct {
      node_list statements;
    } program;

    struct {
      node_list statements;
    } block;

    struct {
      rvn_node *name;
      rvn_node *type_spec;
      rvn_node *init;
      bool is_mutable;
    } var_decl;

    struct {
      rvn_node *name;
      rvn_node *type_spec;
      rvn_node *init;
    } const_decl;

    struct {
      rvn_node *name;
      node_list params;
      rvn_node *return_type;
      rvn_node *body;
      node_list generics;
      bool is_async;
    } function_decl;

    struct {
      rvn_node *name;
      rvn_node *type_spec;
      rvn_node *default_val;
    } param;

    struct {
      rvn_node *name;
      node_list fields;
      node_list methods;
      node_list generics;
    } struct_decl;

    struct {
      rvn_node *name;
      node_list variants;
    } enum_decl;

    struct {
      rvn_node *name;
      node_list fields;
      int value;
    } enum_variant;

    struct {
      rvn_node *name;
      rvn_node *type_spec;
    } field;

    struct {
      rvn_node *condition;
      rvn_node *then_branch;
      rvn_node *else_branch;
    } if_stmt;

    struct {
      rvn_node *expr;
      node_list cases;
      rvn_node *default_case;
    } switch_stmt;

    struct {
      rvn_node *value;
      rvn_node *body;
    } case_stmt;

    struct {
      rvn_node *condition;
      rvn_node *body;
    } while_stmt;

    struct {
      rvn_node *var;
      rvn_node *iterable;
      rvn_node *body;
    } for_stmt;

    struct {
      rvn_node *body;
      rvn_node *condition;
    } repeat_stmt;

    struct {
      rvn_node *value;
    } return_stmt;

    struct {
      rvn_node *value;
    } throw_stmt;

    struct {
      rvn_node *try_block;
      node_list catch_blocks;
    } try_stmt;

    struct {
      rvn_node *var;
      rvn_node *type_spec;
      rvn_node *block;
    } catch_block;

    struct {
      rvn_node *stmt;
    } defer_stmt;

    struct {
      binary_op op;
      rvn_node *left;
      rvn_node *right;
    } binary;

    struct {
      unary_op op;
      rvn_node *operand;
    } unary;

    struct {
      rvn_node *callee;
      node_list args;
    } call;

    struct {
      rvn_node *object;
      rvn_node *index;
    } index;

    struct {
      rvn_node *object;
      rvn_node *member;
      bool is_arrow;
    } access;

    struct {
      const char *name;
      size_t len;
    } identifier;

    struct {
      long long value;
    } literal_int;

    struct {
      double value;
    } literal_float;

    struct {
      const char *value;
      size_t len;
    } literal_string;

    struct {
      bool value;
    } literal_bool;

    struct {
      const char *name;
      size_t len;
    } literal_tag;

    struct {
      node_list elements;
    } array_literal;

    struct {
      node_list entries;
    } map_literal;

    struct {
      rvn_node *key;
      rvn_node *value;
    } map_entry;

    struct {
      rvn_node *call_expr;
    } spawn;

    struct {
      const char *path;
      size_t len;
      rvn_node *alias;
      bool is_wildcard;
    } import;

    struct {
      rvn_node *expr;
      rvn_node *target_type;
    } cast;

    struct {
      rvn_node *base;
      node_list args;
    } type_spec;

    struct {
      rvn_node *base;
      node_list type_args;
    } generic_inst;

    struct {
      rvn_node *start;
      rvn_node *end;
      bool inclusive;
    } range;

    struct {
      node_list elements;
    } tuple;

    struct {
      rvn_node *expr;
    } expr_stmt;
  } data;
};

rvn_node *node_create(node_type type, size_t line, size_t column);
void node_free(rvn_node *node);
void node_list_init(node_list *list);
void node_list_free(node_list *list);
void node_list_add(node_list *list, rvn_node *node);

#endif
