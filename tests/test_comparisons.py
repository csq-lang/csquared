from test_helper import run_test, print_category, print_summary

def test_comparisons():
    print_category("Comparison Operations")
    passed = 0
    total = 0
    
    if run_test("Equal comparison", '''
func main() {
    bool x = 5 == 5
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Not equal comparison", '''
func main() {
    bool x = 5 != 3
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Less than", '''
func main() {
    bool x = 3 < 5
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Greater than", '''
func main() {
    bool x = 5 > 3
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Less or equal", '''
func main() {
    bool x = 5 <= 5
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Greater or equal", '''
func main() {
    bool x = 5 >= 5
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_comparisons()
    print_summary(passed, total)
