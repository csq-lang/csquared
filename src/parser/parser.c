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

node *error_node(arena *a, const char *msg, error_type errtype, token *tk) {
  node *n = new_node(a, N_ERROR, sizeof(node));
  n->errmsg = msg;
  n->errtype = errtype;
  n->line = tk->line;
  n->col = tk->col;
  n->start = tk->start;
  n->length = tk->length;
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
    simple_fatal("Current node is NULL", __LINE__, __FILE_NAME__,
                 INERR_CURR_NODE_NULL, ERROR_LEVEL_ERROR);
  }

  if (current(p)->type != tt) {
    printf("\n");
    token *tk = current(p);
    size_t line_len;
    const char *line = get_line(p->src, tk->line, &line_len);
    if (!line) {
      line = "";
      line_len = 0;
    }

    int highlight_start = tk->start - line;
    int highlight_len = tk->length;

    char msg[512];
    sprintf(msg, "unexpected token '%.*s', expected '%s'", tk->length,
            tk->start, token_type_str[tt]);
    error_info e = new_error_info(
        msg, SYNERR_UNEXPECTED_TOKEN, ERROR_LEVEL_ERROR, p->filename, tk->line,
        tk->col, line, highlight_start, highlight_len);
    print_error(&e);
    exit(1);
  }

  p->index++;
}

token *advance(parser *p) {
  if (!p || !p->tokens || p->index >= p->tokens->count)
    simple_fatal("couldn't advance because parser is NULL, tokens are NULL or "
                 "parser index is out of bounds.",
                 __LINE__, __FILE_NAME__, INERR_NULL_PTR, ERROR_LEVEL_ERROR);
  return p->tokens->tokens[p->index++];
}

token *peekn(parser *p, int n) {
  if ((p->index + n >= p->tokens->count))
    simple_fatal("tried to peek out of bounds", __LINE__, __FILE_NAME__,
                 INERR_PEEK_OOB, ERROR_LEVEL_ERROR);
  if (current(p)->type == T_EOF)
    simple_fatal("tried to peek at the end of file", __LINE__, __FILE_NAME__,
                 INERR_PEEK_OOB, ERROR_LEVEL_ERROR);

  token *tk = p->tokens->tokens[p->index + n];
  return tk;
}

token *peek(parser *p) { return peekn(p, 1); }

int match(parser *p, token_type tt) { return current(p)->type == tt; }

void parse(parser *p) {
  if (!p) {
    simple_fatal("parser pointer is NULL", __LINE__, __FILE__, INERR_NULL_PTR,
                 ERROR_LEVEL_ERROR);
  }

  if (!p->tokens || p->tokens->count == 0 || !p->tokens->tokens) {
    simple_fatal("parser has no tokens", __LINE__, __FILE__, INERR_NULL_PTR,
                 ERROR_LEVEL_ERROR);
  }

  if (!p->node_arena) {
    simple_fatal("parser has no node arena", __LINE__, __FILE__, INERR_NULL_PTR,
                 ERROR_LEVEL_ERROR);
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
