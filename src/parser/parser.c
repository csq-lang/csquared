#include "csquare/parser/parser.h"
#include "csquare/arena.h"
#include "csquare/error.h"
#include "csquare/lexer/lexer.h"
// #include "csquare/parser/rules/statements.h"
#include "csquare/utils.h"
#include <stdio.h>
#include <stdlib.h>

#define N(NAME, STR) STR,
const char *node_type_str[] = {NODE_TYPES};

node *new_node(arena *a, node_type type, size_t size) {
  node *n = arena_alloc(a, size);
  n->type = type;
  return n;
}

node *error_node(arena *a, const char *filename, int line, error_type errtype) {
  node *n = new_node(a, N_ERROR, sizeof(node));
  n->e = new_error(errtype, filename, line);
  return n;
}

void add_node(parser *p, node *n) {
  if (p->node_count >= p->node_cap) {
    p->node_cap *= 2;
    p->nodes = realloc(p->nodes, sizeof(node *) * p->node_cap);
  }

  p->nodes[p->node_count++] = n;
}

void add_new_node(parser *p, node_type type, size_t size) {
  node *n = new_node(p->node_arena, type, size);
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
    csq_error *e;
    quick_error(E_NULLPTR, L_ERR);
    add_note(e, "current(p) when consuming");
    print_error(e);
    free_error(e);
    free_parser(p);
    exit(1);
  }

  if (current(p)->type != tt) {
    printf("\n");

    csq_error *e;
    quick_error(E_UNEXPECTED_TOKEN, L_ERR);
    print_error(e);
    free_error(e);
    free_parser(p);
    exit(1);
  }

  p->index++;
}

token *advance(parser *p) {
  if (!p || !p->tokens || p->index >= p->tokens->count) {
    csq_error *e;
    quick_error(E_NULLPTR, L_ERR);
    const char *note = "couldn't advance";
    if (!p)
      note = "couldn't advance; parser is null";
    else if (!p->tokens)
      note = "couldn't advance; parser's tokens are null";
    else if (p->index >= p->tokens->count)
      note = "couldn't advance; parser's index is out of bounds";
    add_note(e, note);
    print_error(e);
    free_error(e);
    free_parser(p);
    exit(1);
  }
  return p->tokens->tokens[p->index++];
}

token *peekn(parser *p, int n) {
  if ((p->index + n >= p->tokens->count) || current(p)->type == T_EOF) {
    csq_error *e;
    quick_error(E_PEEK_OUT_OF_BOUNDS, L_ERR);
    print_error(e);
    free_parser(p);
    exit(1);
  }

  token *tk = p->tokens->tokens[p->index + n];
  return tk;
}

token *peek(parser *p) { return peekn(p, 1); }

int match(parser *p, token_type tt) { return current(p)->type == tt; }

void parse(parser *p) {
  if (!p || (!p->tokens || p->tokens->count == 0 || !p->tokens->tokens) ||
      !p->node_arena) {
    csq_error *e;
    quick_error(E_NULLPTR, L_ERR);
    const char *note = "?"; // lol
    if (!p)
      note = "couldn't parse; parser is null";
    else if (!p->tokens || p->tokens->count == 0 || !p->tokens->tokens)
      note = "couldn't parse; no tokens";
    else if (!p->node_arena)
      note = "couldn't parse; parser's node arena is null";
    add_note(e, note);
    print_error(e);
    free_error(e);
    free_parser(p);
    exit(1);
  }

  program_node *prog =
      (program_node *)new_node(p->node_arena, N_PROGRAM, sizeof(program_node));
  prog->count = 0;
  prog->cap = 8;
  prog->items = arena_alloc(p->node_arena, sizeof(node *) * prog->cap);

  // clang-format off
  // TODO: parse_statement doesn't have any references
  // while (current(p)->type != T_EOF) {
  //   node *n = parse_statement(p);
  //   if (!n)
  //     simple_fatal("parsed node is NULL", __LINE__, __FILE_NAME__,
  //                  INERR_PARSED_NULL, ERROR_LEVEL_ERROR);
  //   if (prog->count >= prog->cap) {
  //     size_t new_cap = prog->cap * 2;
  //     node **new_items = arena_alloc(p->node_arena, sizeof(node *) * new_cap);
  //     memcpy(new_items, prog->items, sizeof(node *) * prog->count);
  //     prog->items = new_items;
  //     prog->cap = new_cap;
  //   }
  //   prog->items[prog->count++] = n;
  // }
  // clang-format on

  add_node(p, (node *)prog);
}

static void print_tabs(int indent) {
  for (int i = 0; i < indent; i++)
    printf("\t");
}
