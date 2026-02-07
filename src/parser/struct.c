/**
 * @file struct.c
 * @brief Structure and type specification parsing
 * @details Implements parsing of type specifications including array types
 * and named types.
 */

#include <parser/struct.h>
#include <parser/parser.h>
#include <parser/node.h>
#include <parser/types.h>
#include <stdlib.h>
#include <string.h>

csq_node* parse_type_spec(csq_parser* parser) {
    csq_node* node = node_create(NODE_TYPE_SPEC, parser->current.line, parser->current.column);
    if (!node) return NULL;
    
    
    if (parser_match(parser, TOKEN_OPEN_BRACKET)) {
        csq_node* elem_type = parse_type_spec(parser);
        parser_consume(parser, TOKEN_SEMICOLON, "Expected ';' in array type");
        parser_consume(parser, TOKEN_NUMBER, "Expected array size");
        csq_token size_token = parser->previous;
        parser_consume(parser, TOKEN_CLOSE_BRACKET, "Expected ']' after array type");
        
        node->data.type_spec.base = elem_type;
        node_list_init(&node->data.type_spec.args);
        
        csq_node* size_node = node_create(NODE_LITERAL_INT, size_token.line, size_token.column);
        char* end;
        size_node->data.literal_int.value = strtoll(size_token.start, &end, 0);
        node_list_add(&node->data.type_spec.args, size_node);
        
        return node;
    }
    
    
    if (parser_match(parser, TOKEN_IDENTIFIER) ||
        parser_match(parser, TOKEN_KEYWORD_BOOL) ||
        parser_match(parser, TOKEN_KEYWORD_INT) ||
        parser_match(parser, TOKEN_KEYWORD_STRING) ||
        parser_match(parser, TOKEN_KEYWORD_FLOAT)) {
        node->data.type_spec.base = parse_identifier_node(parser);
    } else {
        parser_error(parser, "Expected type name");
        return node;
    }
    
    node_list_init(&node->data.type_spec.args);
    
    return node;
}
