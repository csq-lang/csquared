#include <parser/expr.h>
#include <parser/parser.h>
#include <stdlib.h>
#include <string.h>

rvn_node *expr_parse_primary(rvn_parser *parser) {
  if (parser_match(parser, TOKEN_KEYWORD_TRUE)) {
    rvn_node *node = node_create(NODE_LITERAL_BOOL, parser->previous.line,
                                 parser->previous.column);
    if (node)
      node->data.literal_bool.value = true;
    return node;
  }

  if (parser_match(parser, TOKEN_KEYWORD_FALSE)) {
    rvn_node *node = node_create(NODE_LITERAL_BOOL, parser->previous.line,
                                 parser->previous.column);
    if (node)
      node->data.literal_bool.value = false;
    return node;
  }

  if (parser_match(parser, TOKEN_NUMBER)) {
    Token token = parser->previous;
    rvn_node *node = node_create(NODE_LITERAL_INT, token.line, token.column);
    if (!node)
      return NULL;

    char *end;
    long long val = strtoll(token.start, &end, 0);
    node->data.literal_int.value = val;

    if (end < token.start + token.length &&
        (*end == '.' || *end == 'e' || *end == 'E')) {
      node->type = NODE_LITERAL_FLOAT;
      node->data.literal_float.value = strtod(token.start, NULL);
    }

    return node;
  }

  if (parser_match(parser, TOKEN_STRING)) {
    Token token = parser->previous;
    rvn_node *node = node_create(NODE_LITERAL_STRING, token.line, token.column);
    if (!node)
      return NULL;

    char *str = malloc(token.length - 1);
    if (!str) {
      node_free(node);
      return NULL;
    }
    memcpy(str, token.start + 1, token.length - 2);
    str[token.length - 2] = '\0';

    node->data.literal_string.value = str;
    node->data.literal_string.len = token.length - 2;
    return node;
  }

  if (parser_match(parser, TOKEN_TAG)) {
    Token token = parser->previous;
    rvn_node *node = node_create(NODE_LITERAL_TAG, token.line, token.column);
    if (!node)
      return NULL;

    char *name = malloc(token.length);
    if (!name) {
      node_free(node);
      return NULL;
    }
    memcpy(name, token.start + 1, token.length - 1);

    node->data.literal_tag.name = name;
    node->data.literal_tag.len = token.length - 1;
    return node;
  }

  if (parser_match(parser, TOKEN_IDENTIFIER)) {
    return parse_identifier_node(parser);
  }

  if (parser_match(parser, TOKEN_KEYWORD_SELF)) {
    return node_create(NODE_SELF, parser->previous.line,
                       parser->previous.column);
  }

  if (parser_match(parser, TOKEN_OPEN_PAREN)) {
    rvn_node *expr = expr_parse(parser);
    parser_consume(parser, TOKEN_CLOSE_PAREN, "Expected ')' after expression");
    return expr;
  }

  if (parser_match(parser, TOKEN_OPEN_BRACKET)) {
    rvn_node *array = node_create(NODE_ARRAY_LITERAL, parser->previous.line,
                                  parser->previous.column);
    if (!array)
      return NULL;

    node_list_init(&array->data.array_literal.elements);

    if (!parser_check(parser, TOKEN_CLOSE_BRACKET)) {
      do {
        rvn_node *elem = expr_parse(parser);
        if (elem)
          node_list_add(&array->data.array_literal.elements, elem);
      } while (parser_match(parser, TOKEN_COMMA));
    }

    parser_consume(parser, TOKEN_CLOSE_BRACKET,
                   "Expected ']' after array elements");
    return array;
  }

  parser_error(parser, "Expected expression");
  return NULL;
}

rvn_node *expr_parse_postfix(rvn_parser *parser) {
  rvn_node *node = expr_parse_primary(parser);
  if (!node)
    return NULL;

  for (;;) {
    if (parser_match(parser, TOKEN_OPEN_PAREN)) {
      rvn_node *call = node_create(NODE_CALL, parser->previous.line,
                                   parser->previous.column);
      if (!call)
        return node;

      call->data.call.callee = node;
      node_list_init(&call->data.call.args);

      if (!parser_check(parser, TOKEN_CLOSE_PAREN)) {
        do {
          rvn_node *arg = expr_parse(parser);
          if (arg)
            node_list_add(&call->data.call.args, arg);
        } while (parser_match(parser, TOKEN_COMMA));
      }

      parser_consume(parser, TOKEN_CLOSE_PAREN, "Expected ')' after arguments");
      node = call;
    } else if (parser_match(parser, TOKEN_OPEN_BRACKET)) {
      rvn_node *index = node_create(NODE_INDEX, parser->previous.line,
                                    parser->previous.column);
      if (!index)
        return node;

      index->data.index.object = node;
      index->data.index.index = expr_parse(parser);

      parser_consume(parser, TOKEN_CLOSE_BRACKET, "Expected ']' after index");
      node = index;
    } else if (parser_match(parser, TOKEN_DOT)) {
      rvn_node *access = node_create(NODE_ACCESS, parser->previous.line,
                                     parser->previous.column);
      if (!access)
        return node;

      access->data.access.object = node;
      access->data.access.is_arrow = false;

      parser_consume(parser, TOKEN_IDENTIFIER, "Expected identifier after '.'");
      access->data.access.member = parse_identifier_node(parser);

      node = access;
    } else {
      break;
    }
  }

  return node;
}

rvn_node *expr_parse_unary(rvn_parser *parser) {
  if (parser_match(parser, TOKEN_MINUS)) {
    rvn_node *node = node_create(NODE_UNARY_OP, parser->previous.line,
                                 parser->previous.column);
    if (!node)
      return NULL;
    node->data.unary.op = UNOP_NEG;
    node->data.unary.operand = expr_parse_unary(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_INCREMENT)) {
    rvn_node *node = node_create(NODE_UNARY_OP, parser->previous.line,
                                 parser->previous.column);
    if (!node)
      return NULL;
    node->data.unary.op = UNOP_PRE_INC;
    node->data.unary.operand = expr_parse_unary(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_DECREMENT)) {
    rvn_node *node = node_create(NODE_UNARY_OP, parser->previous.line,
                                 parser->previous.column);
    if (!node)
      return NULL;
    node->data.unary.op = UNOP_PRE_DEC;
    node->data.unary.operand = expr_parse_unary(parser);
    return node;
  }

  return expr_parse_postfix(parser);
}

rvn_node *expr_parse_multiplicative(rvn_parser *parser) {
  rvn_node *node = expr_parse_unary(parser);
  if (!node)
    return NULL;

  for (;;) {
    binary_op op;
    if (parser_match(parser, TOKEN_STAR))
      op = BINOP_MUL;
    else if (parser_match(parser, TOKEN_SLASH))
      op = BINOP_DIV;
    else if (parser_match(parser, TOKEN_PERCENT))
      op = BINOP_MOD;
    else
      break;

    rvn_node *right = expr_parse_unary(parser);
    rvn_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                                parser->previous.column);
    if (!bin)
      return node;

    bin->data.binary.op = op;
    bin->data.binary.left = node;
    bin->data.binary.right = right;
    node = bin;
  }

  return node;
}

rvn_node *expr_parse_additive(rvn_parser *parser) {
  rvn_node *node = expr_parse_multiplicative(parser);
  if (!node)
    return NULL;

  for (;;) {
    binary_op op;
    if (parser_match(parser, TOKEN_PLUS))
      op = BINOP_ADD;
    else if (parser_match(parser, TOKEN_MINUS))
      op = BINOP_SUB;
    else
      break;

    rvn_node *right = expr_parse_multiplicative(parser);
    rvn_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                                parser->previous.column);
    if (!bin)
      return node;

    bin->data.binary.op = op;
    bin->data.binary.left = node;
    bin->data.binary.right = right;
    node = bin;
  }

  return node;
}

rvn_node *expr_parse_comparison(rvn_parser *parser) {
  rvn_node *node = expr_parse_additive(parser);
  if (!node)
    return NULL;

  for (;;) {
    binary_op op;
    if (parser_match(parser, TOKEN_LESS))
      op = BINOP_LT;
    else if (parser_match(parser, TOKEN_GREATER))
      op = BINOP_GT;
    else if (parser_match(parser, TOKEN_LESS_EQUAL))
      op = BINOP_LE;
    else if (parser_match(parser, TOKEN_GREATER_EQUAL))
      op = BINOP_GE;
    else
      break;

    rvn_node *right = expr_parse_additive(parser);
    rvn_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                                parser->previous.column);
    if (!bin)
      return node;

    bin->data.binary.op = op;
    bin->data.binary.left = node;
    bin->data.binary.right = right;
    node = bin;
  }

  return node;
}

rvn_node *expr_parse_equality(rvn_parser *parser) {
  rvn_node *node = expr_parse_comparison(parser);
  if (!node)
    return NULL;

  for (;;) {
    binary_op op;
    if (parser_match(parser, TOKEN_EQUAL))
      op = BINOP_EQ;
    else if (parser_match(parser, TOKEN_NOT_EQUAL))
      op = BINOP_NE;
    else
      break;
    rvn_node *right = expr_parse_comparison(parser);
    rvn_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                                parser->previous.column);
    if (!bin)
      return node;

    bin->data.binary.op = op;
    bin->data.binary.left = node;
    bin->data.binary.right = right;
    node = bin;
  }

  return node;
}

rvn_node *expr_parse_and(rvn_parser *parser) {
  rvn_node *node = expr_parse_equality(parser);
  if (!node)
    return NULL;

  while (parser_match(parser, TOKEN_KEYWORD_AND)) {
    rvn_node *right = expr_parse_equality(parser);
    rvn_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                                parser->previous.column);
    if (!bin)
      return node;

    bin->data.binary.op = BINOP_AND;
    bin->data.binary.left = node;
    bin->data.binary.right = right;
    node = bin;
  }

  return node;
}

rvn_node *expr_parse_or(rvn_parser *parser) {
  rvn_node *node = expr_parse_and(parser);
  if (!node)
    return NULL;

  while (parser_match(parser, TOKEN_KEYWORD_OR)) {
    rvn_node *right = expr_parse_and(parser);
    rvn_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                                parser->previous.column);
    if (!bin)
      return node;

    bin->data.binary.op = BINOP_OR;
    bin->data.binary.left = node;
    bin->data.binary.right = right;
    node = bin;
  }

  return node;
}

rvn_node *expr_parse_assignment(rvn_parser *parser) {
  rvn_node *node = expr_parse_or(parser);
  if (!node)
    return NULL;
  binary_op op;
  if (parser_match(parser, TOKEN_ASSIGN))
    op = BINOP_ASSIGN;
  else if (parser_match(parser, TOKEN_PLUS_ASSIGN))
    op = BINOP_ADD_ASSIGN;
  else if (parser_match(parser, TOKEN_MINUS_ASSIGN))
    op = BINOP_SUB_ASSIGN;
  else if (parser_match(parser, TOKEN_STAR_ASSIGN))
    op = BINOP_MUL_ASSIGN;
  else if (parser_match(parser, TOKEN_SLASH_ASSIGN))
    op = BINOP_DIV_ASSIGN;
  else
    return node;

  rvn_node *right = expr_parse_assignment(parser);
  rvn_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                              parser->previous.column);
  if (!bin)
    return node;

  bin->data.binary.op = op;
  bin->data.binary.left = node;
  bin->data.binary.right = right;
  return bin;
}

rvn_node *expr_parse(rvn_parser *parser) {
  return expr_parse_assignment(parser);
}
