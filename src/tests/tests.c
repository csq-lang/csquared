#include "csquare/tests/tests.h"
#include "csquare/tests/lexer.h"
#include <stdio.h>

int tests_main(void) {
  int failures = 0;
  failures += test_percent_error();
  failures += test_multiple_errors();

  if (failures == 0) {
    printf("All lexer tests \x1b[32mpassed\x1b[0m.\n");
    return 0;
  } else {
    printf("\x1b[34m%d\x1b[0m lexer tests \x1b[31mfailed\x1b[0m.\n", failures);
    return 1;
  }
}
