/**
 * @file keywrd.c
 * @brief Keyword definitions and lookup table
 * @details Defines all C² language keywords in sorted order for efficient
 * binary search lookup during lexical analysis.
 */

#include <csquare/keywrd.h>

struct keyword_entry keywords[] = {{"if", 2, TOKEN_KEYWORD_IF},
                                   {"in", 2, TOKEN_KEYWORD_IN},
                                   {"or", 2, TOKEN_KEYWORD_OR},
                                   {"and", 3, TOKEN_KEYWORD_AND},
                                   {"for", 3, TOKEN_KEYWORD_FOR},
                                   {"int", 3, TOKEN_KEYWORD_INT},
                                   {"nil", 3, TOKEN_KEYWORD_NIL},
                                   {"new", 3, TOKEN_KEYWORD_NEW},
                                   {"try", 3, TOKEN_KEYWORD_TRY},
                                   {"bool", 4, TOKEN_KEYWORD_BOOL},
                                   {"case", 4, TOKEN_KEYWORD_CASE},
                                   {"else", 4, TOKEN_KEYWORD_ELSE},
                                   {"enum", 4, TOKEN_KEYWORD_ENUM},
                                   {"func", 4, TOKEN_KEYWORD_FUNC},
                                   {"self", 4, TOKEN_KEYWORD_SELF},
                                   {"true", 4, TOKEN_KEYWORD_TRUE},
                                   {"break", 5, TOKEN_KEYWORD_BREAK},
                                   {"catch", 5, TOKEN_KEYWORD_CATCH},
                                   {"defer", 5, TOKEN_KEYWORD_DEFER},
                                   {"false", 5, TOKEN_KEYWORD_FALSE},
                                   {"float", 5, TOKEN_KEYWORD_FLOAT},
                                   {"spawn", 5, TOKEN_KEYWORD_SPAWN},
                                   {"throw", 5, TOKEN_KEYWORD_THROW},
                                   {"until", 5, TOKEN_KEYWORD_UNTIL},
                                   {"while", 5, TOKEN_KEYWORD_WHILE},
                                   {"import", 6, TOKEN_KEYWORD_IMPORT},
                                   {"repeat", 6, TOKEN_KEYWORD_REPEAT},
                                   {"return", 6, TOKEN_KEYWORD_RETURN},
                                   {"string", 6, TOKEN_KEYWORD_STRING},
                                   {"struct", 6, TOKEN_KEYWORD_STRUCT},
                                   {"switch", 6, TOKEN_KEYWORD_SWITCH},
                                   {"default", 7, TOKEN_KEYWORD_DEFAULT},
                                   {"private", 7, TOKEN_KEYWORD_PRIVATE},
                                   {"continue", 8, TOKEN_KEYWORD_CONTINUE},
                                   {"function", 8, TOKEN_KEYWORD_FUNCTION},
                                   {"optional", 8, TOKEN_KEYWORD_OPTIONAL}};

const size_t KEYWORDS_COUNT = sizeof(keywords) / sizeof(keywords[0]);
