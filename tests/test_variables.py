from test_helper import run_test, print_category, print_summary

def test_variables():
    print_category("Variable Declaration and Types")
    passed = 0
    total = 0
    
    if run_test("Integer variable", '''
func main() {
    int x = 10
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("f32 type", '''
func main() {
    f32 x = 3.14
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("f64 type", '''
func main() {
    f64 x = 3.14159265
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Boolean type true", '''
func main() {
    bool x = true
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Boolean type false", '''
func main() {
    bool x = false
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("String variable", '''
func main() {
    string s = "Hello, C²!"
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Multiple variable declarations", '''
func main() {
    int a = 1
    int b = 2
    int c = 3
    int d = 4
    int e = 5
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_variables()
    print_summary(passed, total)
