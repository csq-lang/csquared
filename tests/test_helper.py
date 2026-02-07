import subprocess
import os
import tempfile

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

def run_test(name, code, should_succeed=True):
    with tempfile.NamedTemporaryFile(mode='w', suffix='.csq', delete=False) as f:
        f.write(code)
        f.flush()
        temp_file = f.name
    
    try:
        result = subprocess.run(
            ['./bin/raven', '-S', temp_file],
            cwd='/home/darek/Documents/Projects/C²',
            capture_output=True,
            timeout=5
        )
        
        success = (result.returncode == 0) == should_succeed
        
        if success:
            print(f"{Colors.GREEN}✓ PASS{Colors.RESET} - {name}")
        else:
            print(f"{Colors.RED}✗ FAIL{Colors.RESET} - {name}")
            print(f"  Exit code: {result.returncode}")
            if result.stderr:
                print(f"  Error: {result.stderr.decode()[:100]}")
        
        return success
    except Exception as e:
        print(f"{Colors.RED}✗ FAIL{Colors.RESET} - {name}: {str(e)}")
        return False
    finally:
        os.unlink(temp_file)

def print_category(name):
    print(f"\n{Colors.BOLD}{name}{Colors.RESET}")

def print_summary(passed, total):
    percentage = (passed / total * 100) if total > 0 else 0
    print(f"\n{Colors.BOLD}{'='*50}{Colors.RESET}")
    print(f"{Colors.BOLD}Results: {passed}/{total} tests passed ({percentage:.1f}%){Colors.RESET}")
    print(f"{Colors.BOLD}{'='*50}{Colors.RESET}\n")
