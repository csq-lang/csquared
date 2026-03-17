#include "csquare/parser/parser.h"
#include "csquare/arena.h"
#include "csquare/lexer/lexer.h"
#include <stdlib.h>

#define N(NAME, STR) STR,
const char *node_type_str[] = {NODE_TYPES};

node *new_node(arena *a, node_type type) {
  node *n = arena_alloc(a, sizeof(node));
  n->type = type;
  return n;
}

void add_node(parser *p, node *n) {
  if (p->node_count >= p->node_cap) {
    p->node_cap *= 2;
    p->nodes = realloc(p->nodes, sizeof(node *) * p->node_cap);
  }

  p->nodes[p->node_count++] = n;
}

void add_new_node(parser *p, node_type type) {
  node *n = new_node(p->node_arena, type);
  add_node(p, n);
}

parser *new_parser(token_list *tokens, const char *filename, const char *src) {
  parser *p = malloc(sizeof(parser));
  p->tokens = tokens;

  p->node_arena = malloc(sizeof(arena));
  p->node_arena->cap = 32;
  p->node_arena->offset = 0;
  p->node_arena->mem = malloc(p->node_arena->cap);

  p->node_count = 0;
  p->node_cap = 32;
  p->nodes = malloc(sizeof(node *) * p->node_cap);
  p->index = 0;
  p->filename = filename;
  p->src = src;
  return p;
}

void free_parser(parser *p) {
  if (!p)
    return;

  arena_free(p->node_arena);
}

token *current(parser *p) {
  if (p->index > p->tokens->count) {
    return NULL;
  }

  return p->tokens->tokens[p->index];
}

void consume(parser *p, token_type tt) {
  if (!current(p)) {
    // TODO: handle error
  }

  if (current(p)->type != tt) {
    // TODO: handle error
  }

  p->index++;
}

void advance(parser *p) { p->index++; }

void parse(parser *p) {}
