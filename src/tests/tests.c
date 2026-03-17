#include "csquare/tests/tests.h"
#include "csquare/tests/lexer.h"

int tests_failed = 0;

int tests_main(void) {
  printf("\n=== C² Test Suite ===\n\n");

  RUN_TEST(test_multiple_errors);
  RUN_TEST(test_percent_error);

  printf("\n");

  if (tests_failed == 0) {
    printf(CLR_GREEN "All tests passed" CLR_RESET "\n\n");
    return 0;
  } else {
    printf(CLR_RED "%d test(s) failed" CLR_RESET "\n\n", tests_failed);
    return 1;
  }
}
