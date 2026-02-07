#!/usr/bin/env python3

import sys
import os
sys.path.insert(0, os.path.dirname(__file__))

from test_functions import test_basic_functions
from test_variables import test_variables
from test_arrays import test_arrays
from test_arithmetic import test_arithmetic
from test_comparisons import test_comparisons
from test_logical import test_logical
from test_control_flow import test_control_flow
from test_loops import test_loops
from test_operators import test_operators
from test_structs_enums import test_structs_enums
from test_advanced import test_advanced
from test_helper import Colors, print_summary

def main():
    all_passed = 0
    all_total = 0
    
    test_modules = [
        test_basic_functions,
        test_variables,
        test_arrays,
        test_arithmetic,
        test_comparisons,
        test_logical,
        test_control_flow,
        test_loops,
        test_operators,
        test_advanced,
    ]
    
    for test_func in test_modules:
        passed, total = test_func()
        all_passed += passed
        all_total += total
    
    print(f"\n{Colors.BOLD}{'='*50}{Colors.RESET}")
    print(f"{Colors.BOLD}Overall Results: {all_passed}/{all_total} tests passed ({all_passed/all_total*100:.1f}%){Colors.RESET}")
    print(f"{Colors.BOLD}{'='*50}{Colors.RESET}\n")
    
    return 0 if all_passed == all_total else 1

if __name__ == '__main__':
    sys.exit(main())
