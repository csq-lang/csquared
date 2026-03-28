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

typedef struct node node;

struct node {
  node_type type;

  csq_error *e;

  union {
    int int_lit;
    unsigned uint_lit;
    float float_lit;
    double double_lit;
    _Float128 quad_lit;
    const char *str_lit;
    const char *ident;

    struct {
      node **items;
      size_t count;
      size_t cap;
    } program;

    struct {
      node *type;
      const char *name;
      node *init;
    } var_decl;

    struct {
      node **items;
      size_t count;
    } block;

    struct {
      const char *name;
      node **params;
      size_t param_count;
      node *body;
    } func_decl;

    struct {
      node *cond;
      node *then;
      node **elif_branches;
      size_t elif_count;
      node *else_branch;
    } if_stmt;
  };
};

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
node *error_node(arena *a, const char *filename, int line, error_type errtype);
void add_node(parser *p, node *n);
void add_new_node(parser *p, node_type type);

parser *new_parser(token_list *tokens, const char *filename, const char *src);
void free_parser(parser *p);

token *current(parser *p);
void consume(parser *p, token_type tt);
token *advance(parser *p);
token *peek(parser *p);
token *peekn(parser *p, int n);
int match(parser *p, token_type tt);

node *parse_declaration(parser *p);

void parse(parser *p);

// TODO: not enough progress on parsing to be implemented
// void print_node(node *node, int indent);

#endif
