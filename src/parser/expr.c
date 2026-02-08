/**
 * @file expr.c
 * @brief Expression parsing and evaluation
 * @details Implements recursive descent parsing for expressions with proper
 * operator precedence and associativity. Handles primary expressions, postfix
 * operations, unary operations, and binary operators at appropriate precedence
 * levels.
 */

#include <parser/expr.h>
#include <parser/parser.h>
#include <stdlib.h>
#include <string.h>

csq_node *expr_parse_primary(csq_parser *parser) {
  if (parser_match(parser, TOKEN_KEYWORD_TRUE)) {
    csq_node *node = node_create(NODE_LITERAL_BOOL, parser->previous.line,
                                 parser->previous.column);
    if (node)
      node->data.literal_bool.value = true;
    return node;
  }

  if (parser_match(parser, TOKEN_KEYWORD_FALSE)) {
    csq_node *node = node_create(NODE_LITERAL_BOOL, parser->previous.line,
                                 parser->previous.column);
    if (node)
      node->data.literal_bool.value = false;
    return node;
  }

  if (parser_match(parser, TOKEN_NUMBER)) {
    csq_token token = parser->previous;
    csq_node *node = node_create(NODE_LITERAL_INT, token.line, token.column);
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
    csq_token token = parser->previous;
    csq_node *node = node_create(NODE_LITERAL_STRING, token.line, token.column);
    if (!node)
      return NULL;

    size_t len = token.length > 2 ? token.length - 2 : 0;
    char *str = malloc(len + 1);
    if (!str) {
      node_free(node);
      return NULL;
    }
    if (len)
      memcpy(str, token.start + 1, len);
    str[len] = '\0';

    node->data.literal_string.value = str;
    node->data.literal_string.len = len;
    return node;
  }

  if (parser_match(parser, TOKEN_TAG)) {
    csq_token token = parser->previous;
    csq_node *node = node_create(NODE_LITERAL_TAG, token.line, token.column);
    if (!node)
      return NULL;

    size_t len = token.length > 1 ? token.length - 1 : 0;
    char *name = malloc(len + 1);
    if (!name) {
      node_free(node);
      return NULL;
    }
    if (len)
      memcpy(name, token.start + 1, len);
    name[len] = '\0';

    node->data.literal_tag.name = name;
    node->data.literal_tag.len = len;
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
    csq_node *expr = expr_parse(parser);
    parser_consume(parser, TOKEN_CLOSE_PAREN, "Expected ')' after expression");
    return expr;
  }

  if (parser_match(parser, TOKEN_OPEN_BRACKET)) {
    csq_node *array = node_create(NODE_ARRAY_LITERAL, parser->previous.line,
                                  parser->previous.column);
    if (!array)
      return NULL;

    node_list_init(&array->data.array_literal.elements);

    if (!parser_check(parser, TOKEN_CLOSE_BRACKET)) {
      do {
        csq_node *elem = expr_parse(parser);
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

csq_node *expr_parse_postfix(csq_parser *parser) {
  csq_node *node = expr_parse_primary(parser);
  if (!node)
    return NULL;

  for (;;) {
    if (parser_match(parser, TOKEN_OPEN_PAREN)) {
      csq_node *call = node_create(NODE_CALL, parser->previous.line,
                                   parser->previous.column);
      if (!call)
        return node;

      call->data.call.callee = node;
      node_list_init(&call->data.call.args);

      if (!parser_check(parser, TOKEN_CLOSE_PAREN)) {
        do {
          csq_node *arg = expr_parse(parser);
          if (arg)
            node_list_add(&call->data.call.args, arg);
        } while (parser_match(parser, TOKEN_COMMA));
      }

      parser_consume(parser, TOKEN_CLOSE_PAREN, "Expected ')' after arguments");
      node = call;
    } else if (parser_match(parser, TOKEN_OPEN_BRACKET)) {
      csq_node *index = node_create(NODE_INDEX, parser->previous.line,
                                    parser->previous.column);
      if (!index)
        return node;

      index->data.index.object = node;
      index->data.index.index = expr_parse(parser);

      parser_consume(parser, TOKEN_CLOSE_BRACKET, "Expected ']' after index");
      node = index;
    } else if (parser_match(parser, TOKEN_DOT)) {
      csq_node *access = node_create(NODE_ACCESS, parser->previous.line,
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

csq_node *expr_parse_unary(csq_parser *parser) {
  if (parser_match(parser, TOKEN_MINUS)) {
    csq_node *node = node_create(NODE_UNARY_OP, parser->previous.line,
                                 parser->previous.column);
    if (!node)
      return NULL;
    node->data.unary.op = UNOP_NEG;
    node->data.unary.operand = expr_parse_unary(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_BANG)) {
    csq_node *node = node_create(NODE_UNARY_OP, parser->previous.line,
                                 parser->previous.column);
    if (!node)
      return NULL;
    node->data.unary.op = UNOP_NOT;
    node->data.unary.operand = expr_parse_unary(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_INCREMENT)) {
    csq_node *node = node_create(NODE_UNARY_OP, parser->previous.line,
                                 parser->previous.column);
    if (!node)
      return NULL;
    node->data.unary.op = UNOP_PRE_INC;
    node->data.unary.operand = expr_parse_unary(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_DECREMENT)) {
    csq_node *node = node_create(NODE_UNARY_OP, parser->previous.line,
                                 parser->previous.column);
    if (!node)
      return NULL;
    node->data.unary.op = UNOP_PRE_DEC;
    node->data.unary.operand = expr_parse_unary(parser);
    return node;
  }

  return expr_parse_postfix(parser);
}

csq_node *expr_parse_multiplicative(csq_parser *parser) {
  csq_node *node = expr_parse_unary(parser);
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

    csq_node *right = expr_parse_unary(parser);
    csq_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
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

csq_node *expr_parse_additive(csq_parser *parser) {
  csq_node *node = expr_parse_multiplicative(parser);
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

    csq_node *right = expr_parse_multiplicative(parser);
    csq_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
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

csq_node *expr_parse_comparison(csq_parser *parser) {
  csq_node *node = expr_parse_additive(parser);
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

    csq_node *right = expr_parse_additive(parser);
    csq_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
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

csq_node *expr_parse_equality(csq_parser *parser) {
  csq_node *node = expr_parse_bitwise_or(parser);
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
    csq_node *right = expr_parse_bitwise_or(parser);
    csq_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
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

csq_node *expr_parse_bitwise_or(csq_parser *parser) {
  csq_node *node = expr_parse_bitwise_xor(parser);
  if (!node)
    return NULL;

  while (parser_match(parser, TOKEN_PIPE)) {
    csq_node *right = expr_parse_bitwise_xor(parser);
    csq_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                                parser->previous.column);
    if (!bin)
      return node;

    bin->data.binary.op = BINOP_BIT_OR;
    bin->data.binary.left = node;
    bin->data.binary.right = right;
    node = bin;
  }

  return node;
}

csq_node *expr_parse_bitwise_xor(csq_parser *parser) {
  csq_node *node = expr_parse_bitwise_and(parser);
  if (!node)
    return NULL;

  while (parser_match(parser, TOKEN_CARET)) {
    csq_node *right = expr_parse_bitwise_and(parser);
    csq_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                                parser->previous.column);
    if (!bin)
      return node;

    bin->data.binary.op = BINOP_BIT_XOR;
    bin->data.binary.left = node;
    bin->data.binary.right = right;
    node = bin;
  }

  return node;
}

csq_node *expr_parse_bitwise_and(csq_parser *parser) {
  csq_node *node = expr_parse_comparison(parser);
  if (!node)
    return NULL;

  while (parser_match(parser, TOKEN_AMPERSAND)) {
    csq_node *right = expr_parse_comparison(parser);
    csq_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                                parser->previous.column);
    if (!bin)
      return node;

    bin->data.binary.op = BINOP_BIT_AND;
    bin->data.binary.left = node;
    bin->data.binary.right = right;
    node = bin;
  }

  return node;
}

csq_node *expr_parse_and(csq_parser *parser) {
  csq_node *node = expr_parse_equality(parser);
  if (!node)
    return NULL;

  while (parser_match(parser, TOKEN_KEYWORD_AND) ||
         parser_match(parser, TOKEN_LOGICAL_AND)) {
    csq_node *right = expr_parse_equality(parser);
    csq_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
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

csq_node *expr_parse_or(csq_parser *parser) {
  csq_node *node = expr_parse_and(parser);
  if (!node)
    return NULL;

  while (parser_match(parser, TOKEN_KEYWORD_OR) ||
         parser_match(parser, TOKEN_LOGICAL_OR)) {
    csq_node *right = expr_parse_and(parser);
    csq_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
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

csq_node *expr_parse_assignment(csq_parser *parser) {
  csq_node *node = expr_parse_or(parser);
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

  csq_node *right = expr_parse_assignment(parser);
  csq_node *bin = node_create(NODE_BINARY_OP, parser->previous.line,
                              parser->previous.column);
  if (!bin)
    return node;

  bin->data.binary.op = op;
  bin->data.binary.left = node;
  bin->data.binary.right = right;
  return bin;
}

csq_node *expr_parse(csq_parser *parser) {
  return expr_parse_assignment(parser);
}
