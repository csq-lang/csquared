/**
 * @file keywrd.h
 * @brief Keyword and token mapping for the C² lexer.
 * @details Provides keyword lookup tables and utilities for recognizing reserved words
 * and mapping them to their corresponding token types during lexical analysis.
 */

#ifndef C2_KEYWRD_H
#define C2_KEYWRD_H

#include <stdint.h>
#include <csquare/token.h>

/**
 * @brief Maps keyword syntax to its corresponding token type.
 * @details Entry in the keyword table that associates a keyword string with its token type.
 * Used during lexical analysis to quickly identify and classify reserved words.
 */
struct keyword_entry {
    const char* text;       /**< The keyword text/syntax */
    size_t length;          /**< Length of the keyword string */
    csq_tktype type;        /**< The token type associated with this keyword */
};

/**
 * @brief Global keyword lookup table.
 * @details Array of all recognized keywords in the C² language with their token types.
 */
extern struct keyword_entry keywords[];

/**
 * @brief Total count of keywords in the lookup table.
 * @details The number of valid entries in the keywords array.
 */
extern const size_t KEYWORDS_COUNT;
#endif 