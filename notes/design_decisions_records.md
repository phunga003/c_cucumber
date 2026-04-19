
# Architecture Decision Records
## ADR-001 — Language and Standard
**Status:** Accepted </br>
**Date:** 2026-04-18 </br>

**Decision:** C11, strict conformance, no GNU extensions (`-std=c11`, `CMAKE_C_EXTENSIONS OFF`)

**Reasoning:** GNU extensions reduce portability across compilers and embedded toolchains. Strict conformance means the codebase is auditable against a single document — the C11 standard. Extensions like VLAs introduce stack overflow vectors and are banned by MISRA C.

**Consequences:** Cannot use VLAs, statement expressions, or other GCC-specific constructs. Any use of `__attribute__` is a deliberate documented exception.

## ADR-002 — Public/Private Include Boundary
**Status:** Accepted </br>
**Date:** 2026-04-18 </br>

**Decision:** `include/public/` is exported to consumers. `include/internal/` is restricted to library internals via CMake `PRIVATE` visibility.

**Reasoning**: Consumers should only depend on the documented public API. Internal headers may change between versions without notice. Exposing them leaks implementation details and expands the ABI surface.

**Enforcement**: Two layers — CMake `PRIVATE` prevents the path from propagating to consumers. compile_commands.json ensures the linter sees the same boundaries as the compiler.

**Consequences**: Any function used across translation units but not intended for consumers must live in an internal header, never the public one.

## ADR-003 — Symbol Visibility
**Status:** Accepted </br>
**Date:** 2026-04-18 </br>

**Decision:** `-fvisibility=hidden` by default. Public API functions marked (`CUCUMBER_API (__attribute__((visibility("default")))`).

**Reasoning:** Hidden by default minimizes the dynamic symbol table — reducing the interposable surface. Symbol interposition via `LD_PRELOAD` is a real attack vector. Every exported symbol is a trust boundary that must be consciously chosen.

**Consequences:** Every public function must be explicitly marked `CUCUMBER_API`. Forgetting the annotation silently hides the symbol, caught at link time, not runtime.

## ADR-004 — ABI Stability Conventions
**Status:** Accepted </br>
**Date:** 2026-04-18 </br>

**Decision:** All public enums use explicit integer values and `INT32_MAX` sentinel. All public struct integer fields use `int32_t` not int.

**Reasoning:** Enum values are replaced by integers at compile time. Without explicit values, inserting or reordering members shifts all subsequent values, silently breaking any binary compiled against an older version. int size is platform-dependent; `int32_t` is not.

**Rule:** Never reassign existing enum values. Never remove a public enum member. New members get explicit values that do not conflict with existing ones.

## ADR-005 — Memory Ownership Convention
**Status:** Accepted </br>
**Date:** 2026-04-18 </br>

**Decision**: Every pointer crossing a function boundary has documented ownership. Owners are responsible for exactly one `free`. Ownership transfers are documented with "takes ownership" / "caller owns result".

**Reasoning**: Double-free and use-after-free are CVE-class vulnerabilities. Implicit ownership is the root cause of most memory safety bugs in C. Explicit documentation makes ownership auditable without running the code.

**Convention:**
```c
/* Takes ownership of text and datatable — caller must not free after this */
Step *step_create(Keyword kw, char *text, DataTable *datatable);

/* Frees step and all owned resources. step must not be accessed after this. */
void step_free(Step *step);
```

## ADR-006 — `sec_free_impl` and Pointer Poisoning
**Status:** Accepted </br>
**Date:** 2026-04-18 </br>

**Decision**: All frees go through `SEC_FREE` macro which delegates to `sec_free_impl`. After every free, the pointer is set to NULL.

**Reasoning**: Prevents use-after-free from producing silent memory corruption — a NULL dereference crashes loudly and detectably. Prevents double-free because the second call sees NULL and returns safely. Does not prevent double-free under concurrent access — documented as a precondition violation.

**Limitation**: Not thread-safe. Caller must ensure exclusive access. Adding atomics would change the API contract and add threading dependencies inappropriate for a single-threaded test runner.

## ADR-007 — `sec_strdup` Specification
**Status:** Accepted </br>
**Date:** 2026-04-18 </br>

**Decision**: Copies src if and only if strlen(src) < max_len. Returns NULL otherwise.

**Reasoning**: max_len is a validator not a truncator. Silent truncation causes step registry lookup failures and loses data without any signal to the caller. Rejecting oversized input at the boundary is the correct security posture — fail loudly, never silently corrupt.

**Consequence**: Callers must size max_len as at least `strlen(src) + 1`. The function never modifies input.

## ADR-008 — `sec_mul_safe` Specification
**Status:** Accepted </br>
**Date:** 2026-04-18 </br>

**Decision**: Rejects negative inputs. Stores result only on success. Leaves `*out` unchanged on failure.

**Reasoning**: sec_mul_safe exists specifically to compute allocation sizes: `rows * cols * sizeof(char*)`. Negative dimensions are either a bug or malicious input, both of which should be rejected. Leaving *out unchanged on failure prevents callers from using a corrupted value if they forget to check the return.

**Signed overflow note**: Signed integer overflow is undefined behavior in C. The pre-check pattern `(a > INT32_MAX / b)` is used instead of post-multiplication detection because the compiler may optimize away any check that assumes overflow has already occurred.

**Consequences:** 
- NULL check must be the unconditional first guard — no side 
  effects before all inputs are validated (SEI CERT C: EXP34-C)
- Test cases must combine boundary values across parameters

## ADR-009 — sec_zero Optimizer Guarantee
**Status:** Accepted </br>
**Date:** 2026-04-18 </br>

**Decision**: Implemented using volatile pointer cast. `memset_s` used if `__STDC_LIB_EXT1__` is available.

**Reasoning**: The compiler is permitted to optimize away memset before free if it determines the memory is never read again. volatile forces the write to be emitted. This is not verifiable by unit tests — requires assembly inspection of the release build.

**Verification**:
```bash
objdump -d build_release/test_util | grep -A 20 sec_zero
```

## ADR-010 — Test Architecture
**Status:** Accepted </br>
**Date:** 2026-04-18 </br>

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

---

## ADR-012 — `sec_strdup` Security Contract Consideration
**Status:** Accepted </br>
**Date:** 2026-04-19 </br>
**Related:** ADR-007

**Context**: During implementation, there was a concern that the caller providing a larger than needed `max_len` could allow for the function to scan beyond the expected string boundary, potentially reading adjacent allocations. A proposal was made for `sec_strdup` to have a more rigid contract, where the function would duplicate if and only if the provided length match the length of the source string. 

**Decision**: `ADR-007` is kept as-is.

**Reasoning**: The decision to reject the rigid contract was because `sec_strdup` is used to copy strings from a read buffer for lookups after the parse stage. The lexer will reject embedded nulls before `sec_strdup` is called. The injection vector does not reach this function. Defense at the lexer is the correct layer, `sec_strdup` does not need to duplicate that check.

**Consequence**: 
- Call site pass an upper limit and not a pre-measured length, simpler call sites in the parser
- Lexer must reject embeded nulls before `sec_strdup` is called
- If the lexer has a bug that allows embedded nulls through, `sec_strdup` will not catch them

## ADR-013 — `sec_mull_safe` Updated tests cases
**Status:** Accepted </br>
**Date:** 2026-04-19 </br>
**Related:** ADR-008

**Context**: During implementation, is was discovered that the test cases does not cover all possible combinations of inputs, which leads to the combinations of `a` and `b` being zero while `result` is null being untested. This resulted in a segmentation fault since the implementation does not verify whether `result` is null before dereferencing it.  

**Decision**: Added additional test cases to cover more input combinations involving result being null. Fixed implementation to pass all test cases. Added Consequence section in ADR-008 to document implications.

**Reasoning**: This was a mistake in producing test cases that adhere to specifications and standard testing practices. Future test cases must test all combinations of boundary values across all parameters. 



