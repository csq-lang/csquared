from test_helper import run_test, print_category, print_summary

def test_structs_enums():
    print_category("Structs and Enums")
    passed = 0
    total = 0
    
    if run_test("Struct with fields", '''
struct Point {
    int x
    int y
}

func main() {
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Enum with variants", '''
enum Color {
    Red
    Green
    Blue
}

func main() {
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_structs_enums()
    print_summary(passed, total)
