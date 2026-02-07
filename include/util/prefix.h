/**
 * @file prefix.h
 * @brief Numeric literal prefix parsing utilities.
 * @details Provides utilities for recognizing and validating numeric base prefixes
 * (hex, octal, binary) and digit characters in different numeric bases.
 */

#ifndef C2_UTIL_PREFIX_H
#define C2_UTIL_PREFIX_H

#include <stdbool.h>

/**
 * @brief Validates a numeric base prefix.
 * @details Checks if two characters form a valid numeric base prefix
 * (e.g., "0x" for hex, "0b" for binary, "0o" for octal).
 * @param c1 First character of potential prefix.
 * @param c2 Second character of potential prefix.
 * @return True if characters form a valid base prefix, false otherwise.
 */
bool is_valid_base_prefix(char c1, char c2);

/**
 * @brief Checks if a character is a digit in a given base.
 * @details Validates whether a character is a valid digit for the specified numeric base.
 * @param c The character to check.
 * @param base The numeric base (2, 8, 10, 16).
 * @return True if character is a valid digit in the base, false otherwise.
 */
bool is_digit_in_base(char c, int base);

#endif 