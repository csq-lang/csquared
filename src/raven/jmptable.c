/**
 * @file jmptable.c
 * @brief Lexer state machine jump table initialization
 * @details Sets up the character classification table that maps input characters
 * to their corresponding lexer functions for efficient tokenization.
 */

#include <csquare/lexer.h>

/**
 * @brief Initialize the lexer state jump table
 * @details Maps each character to its corresponding lexer function.
 * Whitespace, identifiers, numbers, strings, operators are all classified here.
 */static csq_lexstate state_table[256] = {0};

void initialize_state_table(void) {
    state_table[' '] = lex_whitespace;
    state_table['\t'] = lex_whitespace;
    state_table['\n'] = lex_whitespace;
    state_table['\r'] = lex_whitespace;
    state_table['\v'] = lex_whitespace;
    state_table['\f'] = lex_whitespace;
    
    for (char c = 'a'; c <= 'z'; c++) 
        state_table[(unsigned char)c] = lex_identifier;
    
    for (char c = 'A'; c <= 'Z'; c++) 
        state_table[(unsigned char)c] = lex_identifier;
    
    state_table['_'] = lex_identifier;
    
    for (char c = '0'; c <= '9'; c++) 
        state_table[(unsigned char)c] = lex_number;
    
    
    state_table['"'] = lex_string;
    state_table['\''] = lex_string;
    
    state_table['@'] = lex_tag;
    
    state_table['+'] = lex_operator;
    state_table['-'] = lex_operator;
    state_table['*'] = lex_operator;
    state_table['/'] = lex_operator;
    state_table['%'] = lex_operator;
    state_table['^'] = lex_operator;
    state_table['='] = lex_operator;
    state_table['!'] = lex_operator;
    state_table['<'] = lex_operator;
    state_table['>'] = lex_operator;
    state_table['&'] = lex_operator;
    state_table['|'] = lex_operator;
    state_table['.'] = lex_operator;
    state_table['('] = lex_operator;
    state_table[')'] = lex_operator;
    state_table['{'] = lex_operator;
    state_table['}'] = lex_operator;
    state_table['['] = lex_operator;
    state_table[']'] = lex_operator;
    state_table[':'] = lex_operator;
    state_table[';'] = lex_operator;
    state_table[','] = lex_operator;
    state_table['#'] = lex_operator;
    
    state_table['\0'] = NULL;
}

csq_lexstate get_lex_state(unsigned char c) {
    return state_table[c];
}
