/**
 * @file diag.c
 * @brief Diagnostic reporting system
 * @details Manages compilation errors, warnings, and informational messages
 * with proper source location tracking and formatting.
 */

#include <core/ansi.h>
#include <core/diag.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *error_type_strings[] = {"invalid character",
                                           "unterminated string",
                                           "invalid escape sequence",
                                           "malformed number",
                                           "invalid number base",
                                           "unrecognized token",
                                           "unexpected end of file",
                                           "invalid identifier",
                                           "undefined identifier",
                                           "undefined variable",
                                           "undefined type",
                                           "undefined function",
                                           "duplicate variable",
                                           "duplicate function",
                                           "type mismatch",
                                           "invalid type",
                                           "invalid argument",
                                           "too many arguments",
                                           "too few arguments",
                                           "invalid operation",
                                           "divide by zero",
                                           "stack overflow",
                                           "null pointer",
                                           "out of bounds",
                                           "array index out of bounds",
                                           "array index must be integer",
                                           "array element type mismatch",
                                           "array is empty"};

/**
 * @brief Create diagnostic reporter
 * @return Allocated reporter structure
 */
DiagReporter *diag_reporter_create(void) {
  DiagReporter *reporter = malloc(sizeof(DiagReporter));
  if (!reporter)
    return NULL;
  reporter->head = NULL;
  reporter->tail = NULL;
  reporter->count = 0;
  reporter->error_count = 0;
  return reporter;
}

/**
 * @brief Free diagnostic reporter
 * @param reporter Reporter to deallocate
 */
void diag_reporter_free(DiagReporter *reporter) {
  if (!reporter)
    return;
  DiagNode *current = reporter->head;
  while (current) {
    DiagNode *next = current->next;
    free(current);
    current = next;
  }
  free(reporter);
}

/**
 * @brief Report a diagnostic error
 * @param reporter Reporter instance
 * @param type Error type enumeration
 * @param path Source file path
 * @param line Line number
 * @param column Column number
 * @param length Error region length
 * @param message Error message
 */
void diag_report(DiagReporter *reporter, DiagErrorType type, const char *path,
                 size_t line, size_t column, size_t length,
                 const char *message) {
  if (!reporter)
    return;

  DiagNode *node = malloc(sizeof(DiagNode));
  if (!node)
    return;

  node->error.type = type;
  node->error.path = path;
  node->error.line = line;
  node->error.column = column;
  node->error.length = length;
  node->error.message = message;
  node->next = NULL;

  if (reporter->tail) {
    reporter->tail->next = node;
  } else {
    reporter->head = node;
  }
  reporter->tail = node;
  reporter->count++;
  reporter->error_count++;
}

void diag_report_error(DiagReporter *reporter, DiagErrorType type,
                       const char *path, size_t line, size_t column,
                       size_t length,
                       const char *source_line __attribute__((unused)),
                       const char *message) {
  diag_report(reporter, type, path, line, column, length, message);
}

const char *diag_error_type_string(DiagErrorType type) {
  if (type < sizeof(error_type_strings) / sizeof(error_type_strings[0])) {
    return error_type_strings[type];
  }
  return "unknown error";
}

static const char *find_line_start(const char *buffer, size_t line) {
  const char *p = buffer;
  size_t current_line = 1;

  while (*p && current_line < line) {
    if (*p == '\n')
      current_line++;
    p++;
  }
  return p;
}

static size_t line_length(const char *start) {
  const char *p = start;
  while (*p && *p != '\n' && *p != '\r')
    p++;
  return (size_t)(p - start);
}

void diag_print_error(const DiagError *error, const char *source_buffer) {
  if (!error)
    return;

  printf(ANSI_RED ANSI_BOLD "error" ANSI_RESET ": ");
  printf(ANSI_BOLD "%s" ANSI_RESET "\n", error->message);

  const char *display_path = error->path ? error->path : "<unknown>";

  printf(ANSI_DIM "  --> " ANSI_RESET "%s" ANSI_DIM ":%zu:%zu" ANSI_RESET "\n",
         display_path, error->line, error->column);

  if (source_buffer) {
    const char *line_start = find_line_start(source_buffer, error->line);
    size_t len = line_length(line_start);

    if (len > 0) {
      printf(ANSI_DIM "%4zu | " ANSI_RESET, error->line);

      char line_buffer[1024];
      size_t copy_len = len > 1023 ? 1023 : len;
      memcpy(line_buffer, line_start, copy_len);
      line_buffer[copy_len] = '\0';

      printf("%s\n", line_buffer);

      printf(ANSI_DIM "     | " ANSI_RESET);

      size_t caret_pos = error->column - 1;
      if (caret_pos > copy_len)
        caret_pos = copy_len;

      for (size_t i = 0; i < caret_pos; i++) {
        printf(" ");
      }

      printf(ANSI_RED ANSI_BOLD);
      size_t highlight_len = error->length;
      if (highlight_len == 0)
        highlight_len = 1;
      if (caret_pos + highlight_len > copy_len) {
        highlight_len = copy_len - caret_pos;
      }
      for (size_t i = 0; i < highlight_len; i++) {
        printf("^");
      }
      printf(ANSI_RESET "\n");
    }
  }

  printf(ANSI_DIM "  = " ANSI_RESET "help: ");
  printf("this is a %s\n", diag_error_type_string(error->type));
}

void diag_print_all(const DiagReporter *reporter, const char *source_buffer) {
  if (!reporter)
    return;

  DiagNode *current = reporter->head;
  while (current) {
    diag_print_error(&current->error, source_buffer);
    if (current->next)
      printf("\n");
    current = current->next;
  }
}

bool diag_has_errors(const DiagReporter *reporter) {
  return reporter && reporter->error_count > 0;
}
