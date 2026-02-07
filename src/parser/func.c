/**
 * @file func.c
 * @brief Function declaration parsing
 * @details Handles parsing of function declarations in both `function` and `func`
 * syntax styles, including parameters, return types, and function bodies.
 */

#include <parser/func.h>
#include <parser/parser.h>
#include <parser/node.h>
#include <parser/types.h>
#include <stdlib.h>
#include <string.h>

csq_node* parse_function_declaration(csq_parser* parser) {
    csq_token name_token = parser->current;
    parser_consume(parser, TOKEN_IDENTIFIER, "Expected function name");
    
    csq_node* node = node_create(NODE_FUNCTION_DECL, name_token.line, name_token.column);
    if (!node) return NULL;
    
    node->data.function_decl.name = parse_identifier_node(parser);
    node_list_init(&node->data.function_decl.params);
    node_list_init(&node->data.function_decl.generics);
    
    parser_consume(parser, TOKEN_OPEN_PAREN, "Expected '(' after function name");
    
    if (!parser_check(parser, TOKEN_CLOSE_PAREN)) {
        do {
            csq_token param_token = parser->current;
            parser_consume(parser, TOKEN_IDENTIFIER, "Expected parameter name");
            
            csq_node* param = node_create(NODE_PARAM, param_token.line, param_token.column);
            if (!param) continue;
            
            param->data.param.name = parse_identifier_node(parser);
            
            if (parser_match(parser, TOKEN_COLON)) 
                param->data.param.type_spec = parse_type_spec(parser);
            if (parser_match(parser, TOKEN_ASSIGN)) 
                param->data.param.default_val = parser_parse_expression(parser);
            
            
            node_list_add(&node->data.function_decl.params, param);
        } while (parser_match(parser, TOKEN_COMMA));
    }
    
    parser_consume(parser, TOKEN_CLOSE_PAREN, "Expected ')' after parameters");
    
    if (parser_match(parser, TOKEN_COLON)) {
        node->data.function_decl.return_type = parse_type_spec(parser);
    }
    
    parser_consume(parser, TOKEN_OPEN_BRACE, "Expected '{' before function body");
    node->data.function_decl.body = parse_block(parser);
    
    return node;
}

csq_node* parse_func_declaration(csq_parser* parser) {
    csq_token name_token = parser->current;
    parser_consume(parser, TOKEN_IDENTIFIER, "Expected function name");
    
    csq_node* node = node_create(NODE_FUNCTION_DECL, name_token.line, name_token.column);
    if (!node) return NULL;
    
    node->data.function_decl.name = parse_identifier_node(parser);
    node_list_init(&node->data.function_decl.params);
    node_list_init(&node->data.function_decl.generics);
    
    parser_consume(parser, TOKEN_OPEN_PAREN, "Expected '(' after function name");
    
    if (!parser_check(parser, TOKEN_CLOSE_PAREN)) {
        do {
            csq_node* param = node_create(NODE_PARAM, parser->current.line, parser->current.column);
            if (!param) continue;
            
            param->data.param.type_spec = parse_type_spec(parser);
            
            parser_consume(parser, TOKEN_IDENTIFIER, "Expected parameter name");
            param->data.param.name = parse_identifier_node(parser);
            
            if (parser_match(parser, TOKEN_ASSIGN)) {
                param->data.param.default_val = parser_parse_expression(parser);
            }
            
            node_list_add(&node->data.function_decl.params, param);
        } while (parser_match(parser, TOKEN_COMMA));
    }
    
    parser_consume(parser, TOKEN_CLOSE_PAREN, "Expected ')' after parameters");
    
    if (parser_match(parser, TOKEN_ARROW)) {
        node->data.function_decl.return_type = parse_type_spec(parser);
    }
    
    parser_consume(parser, TOKEN_OPEN_BRACE, "Expected '{' before function body");
    node->data.function_decl.body = parse_block(parser);
    
    return node;
}
