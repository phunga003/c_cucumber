
# Architecture Decision Records
## ADR-001 — Language and Standard
**Decision:** C11, strict conformance, no GNU extensions (`-std=c11`, `CMAKE_C_EXTENSIONS OFF`)

**Reasoning:** GNU extensions reduce portability across compilers and embedded toolchains. Strict conformance means the codebase is auditable against a single document — the C11 standard. Extensions like VLAs introduce stack overflow vectors and are banned by MISRA C.

**Consequences:** Cannot use VLAs, statement expressions, or other GCC-specific constructs. Any use of `__attribute__` is a deliberate documented exception.

## ADR-002 — Public/Private Include Boundary
**Decision:** `include/public/` is exported to consumers. `include/internal/` is restricted to library internals via CMake `PRIVATE` visibility.

**Reasoning**: Consumers should only depend on the documented public API. Internal headers may change between versions without notice. Exposing them leaks implementation details and expands the ABI surface.

**Enforcement**: Two layers — CMake `PRIVATE` prevents the path from propagating to consumers. compile_commands.json ensures the linter sees the same boundaries as the compiler.

**Consequences**: Any function used across translation units but not intended for consumers must live in an internal header, never the public one.

## ADR-003 — Symbol Visibility
Decision: `-fvisibility=hidden` by default. Public API functions marked (`CUCUMBER_API (__attribute__((visibility("default")))`).

Reasoning: Hidden by default minimizes the dynamic symbol table — reducing the interposable surface. Symbol interposition via `LD_PRELOAD` is a real attack vector. Every exported symbol is a trust boundary that must be consciously chosen.
Consequences: Every public function must be explicitly marked `CUCUMBER_API`. Forgetting the annotation silently hides the symbol, caught at link time, not runtime.

## ADR-004 — ABI Stability Conventions
**Decision:** All public enums use explicit integer values and `INT32_MAX` sentinel. All public struct integer fields use `int32_t` not int.

**Reasoning:** Enum values are replaced by integers at compile time. Without explicit values, inserting or reordering members shifts all subsequent values, silently breaking any binary compiled against an older version. int size is platform-dependent; `int32_t` is not.

**Rule:** Never reassign existing enum values. Never remove a public enum member. New members get explicit values that do not conflict with existing ones.

## ADR-005 — Memory Ownership Convention
**Decision**: Every pointer crossing a function boundary has documented ownership. Owners are responsible for exactly one `free`. Ownership transfers are documented with "takes ownership" / "caller owns result".

**Reasoning**: Double-free and use-after-free are CVE-class vulnerabilities. Implicit ownership is the root cause of most memory safety bugs in C. Explicit documentation makes ownership auditable without running the code.

Convention:
```c
/* Takes ownership of text and datatable — caller must not free after this */
Step *step_create(Keyword kw, char *text, DataTable *datatable);

/* Frees step and all owned resources. step must not be accessed after this. */
void step_free(Step *step);
```

## ADR-006 — `sec_free_impl` and Pointer Poisoning
**Decision**: All frees go through `SEC_FREE` macro which delegates to `sec_free_impl`. After every free, the pointer is set to NULL.

**Reasoning**: Prevents use-after-free from producing silent memory corruption — a NULL dereference crashes loudly and detectably. Prevents double-free because the second call sees NULL and returns safely. Does not prevent double-free under concurrent access — documented as a precondition violation.

**Limitation**: Not thread-safe. Caller must ensure exclusive access. Adding atomics would change the API contract and add threading dependencies inappropriate for a single-threaded test runner.

## ADR-007 — `sec_strdup` Specification
**Decision**: Copies src if and only if strlen(src) < max_len. Returns NULL otherwise.

**Reasoning**: max_len is a validator not a truncator. Silent truncation causes step registry lookup failures and loses data without any signal to the caller. Rejecting oversized input at the boundary is the correct security posture — fail loudly, never silently corrupt.

**Consequence**: Callers must size max_len as at least strlen(src) + 1. The function never modifies input.

## ADR-008 — sec_mul_safe Specification
**Decision**: Rejects negative inputs. Stores result only on success. Leaves *out unchanged on failure.
**Reasoning**: sec_mul_safe exists specifically to compute allocation sizes — rows * cols * sizeof(char*). Negative dimensions are either a bug or malicious input, both of which should be rejected. Leaving *out unchanged on failure prevents callers from using a corrupted value if they forget to check the return.

**Signed overflow note**: Signed integer overflow is undefined behavior in C. The pre-check pattern (a > INT32_MAX / b) is used instead of post-multiplication detection because the compiler may optimize away any check that assumes overflow has already occurred.

## ADR-009 — sec_zero Optimizer Guarantee
**Decision**: Implemented using volatile pointer cast. memset_s used if `__STDC_LIB_EXT1__` is available.

**Reasoning**: The compiler is permitted to optimize away memset before free if it determines the memory is never read again. volatile forces the write to be emitted. This is not verifiable by unit tests — requires assembly inspection of the release build.

**Verification**:
```bash
objdump -d build_release/test_util | grep -A 20 sec_zero
```

## ADR-010 — Test Architecture
**Decision**: Custom fork-based test runner. No external framework. Valgrind and AddressSanitizer as separate verification layers.

**Reasoning**:
- Fork isolation — a segfault in one test does not kill the runner
- No framework dependency — every line is auditable
- ASan at 2x overhead for local development, Valgrind at 20-50x overhead for CI
- Tests written before implementation — specification is proven before code exists

***Layers***:
```
Tier 1 — every build:   -fsanitize=address -fsanitize=undefined
Tier 2 — every push:    Valgrind --leak-check=full --track-origins=yes
Tier 3 — every release: objdump assembly inspection for sec_zero
```

## ADR-011 — Symbol Interposition Defense
**Decision**: Internal functions use sec_free_impl naming convention without leading underscore. Leading underscores are reserved by the C standard for compiler and standard library use.

**Reasoning**: `_name` identifiers in file scope are reserved by the C standard. Using them is technically undefined behavior and produces warnings under -Wpedantic. The double-underscore suffix or descriptive prefix convention is used instead.


