#ifndef _PARSER_H
#define _PARSER_H

#include "csquare/arena.h"
#include "csquare/lexer/lexer.h"
#include <stddef.h>
#define N(NAME, STR) NAME,
#define NODE_TYPES                                                             \
  N(N_ERROR, "ERROR")                                                          \
  N(N_PROGRAM, "PROGRAM")                                                      \
  N(N_INT_LITERAL, "INTEGER LITERAL")                                          \
  N(N_UINT_LITERAL, "UINT LITERAL")                                            \
  N(N_FLOAT_LITERAL, "FLOAT LITERAL")                                          \
  N(N_DOUBLE_LITERAL, "DOUBLE LITERAL")                                        \
  N(N_QUAD_LITERAL, "QUAD LITERAL")                                            \
  N(N_STRING_LITERAL, "STRING LITERAL")                                        \
  N(N_BOOL_LITERAL, "BOOLEAN LITERAL")                                         \
  N(N_IDENT, "IDENTIFIER")                                                     \
  N(N_BINARY_EXPR, "BINARY EXPRESSION")                                        \
  N(N_UNARY_EXPR, "UNARY EXPRESSION")                                          \
  N(N_VAR_DECL, "VARIABLE DECLARATION")

typedef enum { NODE_TYPES N__COUNT } node_type;
#undef N

extern const char *node_type_str[];

#define BASE node base

typedef struct {
  node_type type;
} node;

typedef struct {
  BASE;
  node **items;
  size_t count;
} node_program;

typedef struct {
  BASE;
  int v;
} int_node;
typedef struct {
  BASE;
  unsigned int v;
} uint_node;
typedef struct {
  BASE;
  float v;
} float_node;
typedef struct {
  BASE;
  double v;
} double_node;
typedef struct {
  BASE;
  _Float128 v;
} quad_node;
typedef struct {
  BASE;
  const char *v;
} string_node;
typedef struct {
  BASE;
  _Bool v;
} bool_node;
typedef struct {
  BASE;
  const char *v;
} ident_node;

#define VALID_OP(OP) (((int)OP >= T_EQ) && ((int)OP <= T_EXCLAMATION))

typedef struct {
  BASE;
  node *left;
  node *right;
  token_type op;
} bexpr_node;

typedef struct {
  BASE;
  node *operand;
  token_type op;
} uexpr_node;

typedef struct {
  BASE;
  const char *name;
  node *init;
} var_decl_node;

typedef struct {
  BASE;
  node **items;
  size_t count;
} block_node;

typedef struct {
  BASE;
  node *target;
  node *value;
} assign_node;

typedef struct {
  BASE;
  node *cond;
  node *then;
  node **elif_branches;
  size_t elif_count;
  node *else_branch;
} if_node;

typedef struct {
  BASE;
  const char *name;
  node **params;
  size_t param_count;
  block_node *body;
} func_decl_node;

#undef BASE

typedef struct {
  arena *node_arena;

  node **nodes;
  size_t node_cap;
  size_t node_count;
  size_t index;

  token_list *tokens;

  const char *src;
  const char *filename;
} parser;

node *new_node(arena *a, node_type type);
void add_node(parser *p, node *n);
void add_new_node(parser *p, node_type type);

parser *new_parser(token_list *tokens, const char *filename, const char *src);
void free_parser(parser *p);

token *current(parser *p);
void consume(parser *p, token_type tt);
void advance(parser *p);

void parse(parser *p);

#endif
