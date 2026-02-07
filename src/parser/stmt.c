/**
 * @file stmt.c
 * @brief Statement parsing
 * @details Implements parsing of control flow statements (if, while, for, switch,
 * try-catch), loop control (break, continue), and other statement types.
 */

#include <parser/stmt.h>
#include <parser/parser.h>
#include <parser/node.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Parse a block of statements
 * @details Parses statements until closing brace is encountered.
 * @param parser Parser instance
 * @return Block node containing statements
 */
csq_node* parse_block(csq_parser* parser) {
    csq_node* block = node_create(NODE_BLOCK, parser->previous.line, parser->previous.column);
    if (!block) return NULL;
    
    node_list_init(&block->data.block.statements);
    
    while (!parser_check(parser, TOKEN_CLOSE_BRACE) && !parser_check(parser, TOKEN_EOF)) {
        csq_node* stmt = parser_parse_statement(parser);
        if (stmt) node_list_add(&block->data.block.statements, stmt);
    }
    
    parser_consume(parser, TOKEN_CLOSE_BRACE, "Expected '}' after block");
    return block;
}

/**
 * @brief Parse if statement with optional else/else-if
 * @param parser Parser instance
 * @return If statement node
 */
csq_node* parse_if_statement(csq_parser* parser) {
    csq_node* node = node_create(NODE_IF, parser->previous.line, parser->previous.column);
    if (!node) return NULL;
    
    node->data.if_stmt.condition = parser_parse_expression(parser);
    
    parser_consume(parser, TOKEN_OPEN_BRACE, "Expected '{' after if condition");
    node->data.if_stmt.then_branch = parse_block(parser);
    
    if (parser_match(parser, TOKEN_KEYWORD_ELSE)) {
        if (parser_match(parser, TOKEN_KEYWORD_IF)) {
            node->data.if_stmt.else_branch = parse_if_statement(parser);
        } else {
            parser_consume(parser, TOKEN_OPEN_BRACE, "Expected '{' after else");
            node->data.if_stmt.else_branch = parse_block(parser);
        }
    }
    
    return node;
}

/**
 * @brief Parse while loop statement
 * @param parser Parser instance
 * @return While loop node
 */
csq_node* parse_while_statement(csq_parser* parser) {
    csq_node* node = node_create(NODE_WHILE, parser->previous.line, parser->previous.column);
    if (!node) return NULL;
    
    node->data.while_stmt.condition = parser_parse_expression(parser);
    
    parser_consume(parser, TOKEN_OPEN_BRACE, "Expected '{' after while condition");
    node->data.while_stmt.body = parse_block(parser);
    
    return node;
}



/**
 * @brief Parse return statement
 * @param parser Parser instance
 * @return Return statement node
 */
csq_node* parse_return_statement(csq_parser* parser) {
    csq_node* node = node_create(NODE_RETURN, parser->previous.line, parser->previous.column);
    if (!node) return NULL;
    
    if (!parser_check(parser, TOKEN_SEMICOLON) && !parser_check(parser, TOKEN_CLOSE_BRACE)) 
        node->data.return_stmt.value = parser_parse_expression(parser);
    
    return node;
}

csq_node* parse_expression_statement(csq_parser* parser) {
    csq_node* expr = parser_parse_expression(parser);
    if (!expr) return NULL;
    
    csq_node* node = node_create(NODE_EXPR_STMT, expr->line, expr->column);
    if (!node) return expr;
    
    node->data.expr_stmt.expr = expr;
    return node;
}
