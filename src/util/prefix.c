/**
 * @file prefix.c
 * @brief Number literal prefix and base validation
 * @details Helper functions for validating and classifying numeric literals
 * with various base prefixes (hex, binary, octal).
 */

#include <util/prefix.h>

/**
 * @brief Check if character pair is a valid number base prefix
 * @param c1 First character (should be '0')
 * @param c2 Second character (x, X, b, B, o, O)
 * @return True if valid prefix
 */
bool is_valid_base_prefix(char c1, char c2) {
    return c1 == '0' && (c2 == 'x' || c2 == 'X' ||
                          c2 == 'b' || c2 == 'B' ||
                          c2 == 'o' || c2 == 'O');
}

/**
 * @brief Check if character is valid digit for given base
 * @param c Character to check
 * @param base Numeric base (2, 8, 10, 16)
 * @return True if valid digit for base
 */
bool is_digit_in_base(char c, int base) {
    if (base == 2) return c == '0' || c == '1';
    if (base == 8) return c >= '0' && c <= '7';
    if (base == 10) return c >= '0' && c <= '9';
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}