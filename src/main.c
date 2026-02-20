#include <stdio.h>
#include "csquare/opt-common.h"

int main(int argc, char* argv[])
{
    csq_options *opts = options_parse(argc, argv);
    if(!opts) return EXIT_FAILURE;
    return 0;
}
