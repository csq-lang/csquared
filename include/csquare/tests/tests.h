#ifndef _TESTS_H
#define _TESTS_H

#include <stdio.h>

#define CLR_RESET "\x1b[0m"
#define CLR_RED "\x1b[31m"
#define CLR_GREEN "\x1b[32m"
#define CLR_BLUE "\x1b[34m"
#define CLR_DIM "\x1b[90m"

extern int tests_failed;

#define TEST(name) int name(void)

#define RUN_TEST(name)                                                         \
  do {                                                                         \
    printf("Running" CLR_BLUE " %-32s" CLR_RESET, #name);                      \
    int r = name();                                                            \
    if (r == 0) {                                                              \
      printf(CLR_GREEN "[PASS]" CLR_RESET "\n");                               \
    } else {                                                                   \
      printf(CLR_RED "[FAIL]" CLR_RESET "\n\n");                               \
      tests_failed++;                                                          \
    }                                                                          \
  } while (0)

#define ASSERT(cond, fmt, ...)                                                 \
  do {                                                                         \
    if (!(cond)) {                                                             \
      printf("\n    " CLR_RED "Assertion failed:" CLR_RESET " ");              \
      printf(fmt, ##__VA_ARGS__);                                              \
      printf("\n");                                                            \
      return 1;                                                                \
    }                                                                          \
  } while (0)

int tests_main(void);

#endif
