from test_helper import run_test, print_category, print_summary

def test_basic_functions():
    print_category("Basic Function Declaration")
    passed = 0
    total = 0
    
    if run_test("Simple function", '''
func add(int a, int b) -> int {
    return a + b
}

func main() {
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Function with no parameters", '''
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
    
    if run_test("Function with void return", '''
func doNothing() {
    return
}

func main() {
    doNothing()
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_basic_functions()
    print_summary(passed, total)
