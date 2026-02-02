#ifndef CORE_DIAG_H
#define CORE_DIAG_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
  DIAG_ERROR_INVALID_CHAR,
  DIAG_ERROR_UNTERMINATED_STRING,
  DIAG_ERROR_INVALID_ESCAPE,
  DIAG_ERROR_MALFORMED_NUMBER,
  DIAG_ERROR_INVALID_BASE,
  DIAG_ERROR_UNRECOGNIZED_TOKEN,
  DIAG_ERROR_UNEXPECTED_EOF,
  DIAG_ERROR_INVALID_IDENTIFIER,
  DIAG_ERROR_UNDEFINED_IDENTIFIER
} DiagErrorType;

typedef struct {
  const char *path;
  size_t line;
  size_t column;
  size_t length;
  DiagErrorType type;
  const char *message;
} DiagError;

typedef struct DiagNode {
  DiagError error;
  struct DiagNode *next;
} DiagNode;

typedef struct {
  DiagNode *head;
  DiagNode *tail;
  size_t count;
  size_t error_count;
} DiagReporter;

DiagReporter *diag_reporter_create(void);
void diag_reporter_free(DiagReporter *reporter);
void diag_report(DiagReporter *reporter, DiagErrorType type, const char *path,
                 size_t line, size_t column, size_t length,
                 const char *message);
void diag_report_error(DiagReporter *reporter, DiagErrorType type,
                       const char *path, size_t line, size_t column,
                       size_t length, const char *source_line,
                       const char *message);
void diag_print_all(const DiagReporter *reporter, const char *source_buffer);
void diag_print_error(const DiagError *error, const char *source_buffer);
const char *diag_error_type_string(DiagErrorType type);
bool diag_has_errors(const DiagReporter *reporter);

#endif
