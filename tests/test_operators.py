from test_helper import run_test, print_category, print_summary

def test_operators():
    print_category("Assignment and Operators")
    passed = 0
    total = 0
    
    if run_test("Addition assignment", '''
func main() {
    int x = 10
    x += 5
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Subtraction assignment", '''
func main() {
    int x = 10
    x -= 3
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Multiplication assignment", '''
func main() {
    int x = 5
    x *= 2
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Division assignment", '''
func main() {
    int x = 20
    x /= 4
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Increment operator", '''
func main() {
    int x = 5
    x++
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Decrement operator", '''
func main() {
    int x = 5
    x--
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_operators()
    print_summary(passed, total)
