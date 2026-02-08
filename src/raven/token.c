/**
 * @file token.c
 * @brief Token type string mappings
 * @details Provides utility functions to convert token types to their string
 * representations for debugging and error reporting.
 */

#include <csquare/token.h>

#define X(token, str) str,
const char *const token_strings[] = {TOKEN_LIST};
#undef X

static const int token_count = sizeof(token_strings) / sizeof(token_strings[0]);

const char *token_type_to_string(csq_tktype type) {
  if (type >= 0 && type < token_count) {
    return token_strings[type];
  }
  return "UNKNOWN";
}
