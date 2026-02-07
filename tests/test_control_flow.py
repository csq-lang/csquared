from test_helper import run_test, print_category, print_summary

def test_control_flow():
    print_category("Control Flow - If/Else")
    passed = 0
    total = 0
    
    if run_test("If statement", '''
func main() {
    int x = 10
    if x > 5 {
        int y = 1
    }
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("If-else statement", '''
func main() {
    int x = 3
    if x > 5 {
        int y = 1
    } else {
        int y = 2
    }
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Nested if statements", '''
func main() {
    int x = 10
    if x > 5 {
        if x < 20 {
            int y = 1
        }
    }
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_control_flow()
    print_summary(passed, total)
