#ifndef _PARSER_H
#define _PARSER_H

#include "csquare/arena.h"
#include "csquare/error.h"
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
  N(N_EXPR, "EXPRESSION")                                                      \
  N(N_VAR_DECL, "VARIABLE DECLARATION")                                        \
  N(N_FUNC_DECL, "FUNCTION DECLARATION")                                       \
  N(N_BLOCK, "BLOCK")                                                          \
  N(N_IF_STMT, "IF STATEMENT")

typedef enum { NODE_TYPES N__COUNT } node_type;
#undef N

extern const char *node_type_str[];

typedef struct {
  node_type type;

  const char *errmsg;
  error_type errtype;
  int line;
  int col;
  const char *start;
  int length;
} node;

typedef struct {
  node base;
  node **items;
  size_t count;
  size_t cap;
} program_node;

typedef struct {
  node base;
  node *type;
  const char *name;
  node *init;
} var_decl_node;

typedef struct {
  node base;
  node **items;
  size_t count;
} block_node;

typedef struct {
  node base;
  const char *name;
  node **params;
  size_t param_count;
  block_node *body;
} func_decl_node;

typedef struct {
  node base;
  node *cond;
  node *then;
  node **elif_branches;
  size_t elif_count;
  node *else_branch;
} if_node;

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

node *new_node(arena *a, node_type type, size_t size);
node *error_node(arena *a, const char *msg, error_type errtype, token *tk);
void add_node(parser *p, node *n);
void add_new_node(parser *p, node_type type, size_t size);

parser *new_parser(token_list *tokens, const char *filename, const char *src);
void free_parser(parser *p);

token *current(parser *p);
void consume(parser *p, token_type tt);
token *advance(parser *p);
token *peek(parser *p);
token *peekn(parser *p, int n);
int match(parser *p, token_type tt);

void parse(parser *p);

// TODO: not enough progress on parsing to be implemented
// void print_node(node *node, int indent);

#endif
