#ifndef _TESTS_H
#define _TESTS_H

#define ASSERT(cond, msg, ...)                                                 \
  do {                                                                         \
    if (!(cond)) {                                                             \
      printf("\x1b[31mFAIL\x1b[0m: ");                                         \
      printf(msg, __VA_ARGS__);                                                \
      printf("\n");                                                            \
      return 1;                                                                \
    }                                                                          \
  } while (0)

#define TEST_PASS printf("PASS: %s\n", __func__)

int tests_main(void);

#endif
