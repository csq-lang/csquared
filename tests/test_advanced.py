from test_helper import run_test, print_category, print_summary

def test_advanced():
    print_category("Advanced Features")
    passed = 0
    total = 0
    
    if run_test("Return from function", '''
func getValue() -> int {
    return 42
}

func main() {
    int x = getValue()
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Early return", '''
func check(int x) -> bool {
    if x < 0 {
        return false
    }
    return true
}

func main() {
    bool result = check(10)
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Multiple parameters", '''
func multiply(int a, int b, int c) -> int {
    return a * b * c
}

func main() {
    int result = multiply(2, 3, 4)
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Chained function calls", '''
func add(int a, int b) -> int {
    return a + b
}

func multiply(int a, int b) -> int {
    return a * b
}

func main() {
    int result = multiply(add(2, 3), 4)
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Switch with cases", '''
func main() {
    int x = 2
    switch x {
        case 1: int y = 10
        case 2: int z = 20
        default: int w = 30
    }
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Minimal program", '''
func main() {
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_advanced()
    print_summary(passed, total)
