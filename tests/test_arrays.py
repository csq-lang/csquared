from test_helper import run_test, print_category, print_summary

def test_arrays():
    print_category("Array Operations")
    passed = 0
    total = 0
    
    if run_test("Array declaration", '''
func main() {
    [5]int arr = [1, 2, 3, 4, 5]
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Array access", '''
func main() {
    [3]int arr = [10, 20, 30]
    int x = arr[0]
    int y = arr[1]
    int z = arr[2]
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Array assignment", '''
func main() {
    [3]int arr = [10, 20, 30]
    arr[0] = 100
    arr[1] = 200
    arr[2] = 300
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("Array overflow detection", '''
func main() {
    [3]int test = [1, 2, 3, 4, 5]
    __builtin_syscall(1, 0)
}
''', should_succeed=False):
        passed += 1
    total += 1
    
    if run_test("Array negative index access", '''
func main() {
    [3]int arr = [5, 10, 15]
    int idx = -1
    int x = arr[idx]
    __builtin_syscall(1, 0)
}
''', should_succeed=False):
        passed += 1
    total += 1
    
    if run_test("2D array declaration", '''
func main() {
    [2][3]int matrix = [[1, 2, 3], [4, 5, 6]]
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    if run_test("For loop over array", '''
func main() {
    [3]int arr = [1, 2, 3]
    for x in arr {
        int y = x
    }
    __builtin_syscall(1, 0)
}
'''):
        passed += 1
    total += 1
    
    return passed, total

if __name__ == '__main__':
    passed, total = test_arrays()
    print_summary(passed, total)
