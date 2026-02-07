/**
 * @file var.c
 * @brief Variable and constant declaration parsing
 * @details Handles parsing of variable and constant declarations with optional
 * type specifications and initializers.
 */

#include <parser/var.h>
#include <parser/parser.h>
#include <parser/node.h>
#include <parser/types.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Parse variable or constant declaration
 * @param parser Parser instance
 * @param is_const True for const declaration, false for var
 * @return Variable/constant declaration node
 */
csq_node* parse_var_declaration(csq_parser* parser, bool is_const) {
    
    csq_token type_token = parser->current;
    
    csq_node* type_spec = NULL;
    size_t declared_array_size = 0;
    
    if (parser_check(parser, TOKEN_OPEN_BRACKET)) {
        parser_advance(parser);
        
        csq_token size_token = parser->current;
        if (!parser_match(parser, TOKEN_NUMBER)) {
            parser_error(parser, "Expected array size");
            return NULL;
        }
        
        char* end;
        declared_array_size = (size_t)strtoll(size_token.start, &end, 0);
        
        parser_consume(parser, TOKEN_CLOSE_BRACKET, "Expected ']' after array size");
        
        csq_token base_type_token = parser->current;
        parser_advance(parser);
        
        type_spec = node_create(NODE_TYPE_SPEC, type_token.line, type_token.column);
        if (type_spec) {
            parser->previous = base_type_token;
            type_spec->data.type_spec.base = parse_identifier_node(parser);
            node_list_init(&type_spec->data.type_spec.args);
            
            csq_node* size_node = node_create(NODE_LITERAL_INT, size_token.line, size_token.column);
            if (size_node) {
                size_node->data.literal_int.value = declared_array_size;
                node_list_add(&type_spec->data.type_spec.args, size_node);
            }
        }
    } else {
        parser_advance(parser);
        
        type_spec = node_create(NODE_TYPE_SPEC, type_token.line, type_token.column);
        if (type_spec) {
            parser->previous = type_token;
            type_spec->data.type_spec.base = parse_identifier_node(parser);
            node_list_init(&type_spec->data.type_spec.args);
        }
    }
    
    csq_token name_token = parser->current;
    parser_consume(parser, TOKEN_IDENTIFIER, "Expected variable name");
    
    csq_node* node;
    if (is_const) 
        node = node_create(NODE_CONST_DECL, name_token.line, name_token.column);
    else 
        node = node_create(NODE_VAR_DECL, name_token.line, name_token.column);
    
    if (!node) return NULL;
    
    parser->previous = name_token;
    node->data.var_decl.name = parse_identifier_node(parser);
    node->data.var_decl.is_mutable = !is_const;
    node->data.var_decl.type_spec = type_spec;
    
    if (parser_match(parser, TOKEN_ASSIGN)) {
        csq_node* init = parser_parse_expression(parser);
        node->data.var_decl.init = init;
        
        if (init && init->type == NODE_ARRAY_LITERAL && declared_array_size > 0) {
            size_t actual_size = init->data.array_literal.elements.count;
            
            if (actual_size != declared_array_size) {
                parser_error_at_location(parser, DIAG_ERROR_ARRAY_TYPE_MISMATCH,
                                        init->line, init->column, 1,
                                        "Array size mismatch");
            }
        }
    } else if (is_const) {
        parser_error(parser, "Constants must be initialized");
    }
    
    return node;
}
