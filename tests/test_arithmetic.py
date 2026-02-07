from test_helper import run_test, print_category, print_summary

def test_arithmetic():
    print_category("Arithmetic Operations")
    passed = 0
    total = 0
    
    if run_test("Addition", '''
func main() {
    int x = 10 + 20
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Subtraction", '''
func main() {
    int x = 30 - 10
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Multiplication", '''
func main() {
    int x = 5 * 6
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Division", '''
func main() {
    int x = 20 / 5
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Modulo", '''
func main() {
    int x = 20 % 3
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Complex arithmetic", '''
func main() {
    int x = (5 + 3) * 2 - 4
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Unary minus", '''
func main() {
    int x = -5
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_arithmetic()
    print_summary(passed, total)
