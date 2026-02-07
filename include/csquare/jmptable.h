/**
 * @file jmptable.h
 * @brief Lexer state jump table for character classification.
 * @details Provides state transition table and lookup functions for efficient lexer state management.
 * The jump table determines the lexer state based on the input character.
 */

#ifndef C2_JMPTABLE_H
#define C2_JMPTABLE_H

#include <csquare/lexer.h>

/**
 * @brief Initializes the lexer state jump table.
 * @details Sets up the character-to-state mapping table used by the lexer for efficient
 * state transitions. Must be called before using get_lex_state().
 */
void initialize_state_table(void);

/**
 * @brief Gets the lexer state for a given character.
 * @details Looks up the lexer state based on a single input character.
 * Uses the pre-computed jump table for O(1) lookup performance.
 * @param c The input character to classify.
 * @return The lexer state corresponding to the character classification.
 */
csq_lexstate get_lex_state(unsigned char c);

#endif
