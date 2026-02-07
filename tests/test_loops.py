from test_helper import run_test, print_category, print_summary

def test_loops():
    print_category("Loops")
    passed = 0
    total = 0
    
    if run_test("While loop", '''
func main() {
    int x = 0
    while x < 10 {
        x = x + 1
    }
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_loops()
    print_summary(passed, total)
