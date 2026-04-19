# C-Cucumber

Minimal `cucumber.js` in C. For when you want BDD in C. Extend to your needs; reimplement if you don't trust it, consult architecture docs for how (wip).

### Fast local development — ASan + UBSan
```
cmake -B build -DCMAKE_BUILD_TYPE=Asan
cmake --build build
ctest --test-dir build --output-on-failure
```

### CI — debug build + Valgrind (registered as a CTest test)
```
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

### Release audit
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
objdump -d build/test_util | grep -A 20 sec_zero
```