# K23 SEARCH

## Runbook

```bash
./task.sh init            # Initializes meson
./task.sh compile         # Compiles the project
./task.sh format          # Runs clang-format for all the source files
./task.sh test [test]     # Runs the test suite
./task.sh test-mem [test] # Runs the test suite with valgrind
./task.sh coverage        # Generates the code coverage report. Should be run after the tests
```
