from test_helper import run_test, print_category, print_summary

def test_logical():
    print_category("Logical Operations")
    passed = 0
    total = 0
    
    if run_test("Logical AND", '''
func main() {
    bool x = true && true
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Logical OR", '''
func main() {
    bool x = false || true
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Logical NOT", '''
func main() {
    bool x = !true
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Complex boolean expression", '''
func main() {
    bool result = (5 > 3) && (10 < 20) || (1 == 1)
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Bitwise XOR", '''
func main() {
    int x = 5 ^ 3
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_logical()
    print_summary(passed, total)
