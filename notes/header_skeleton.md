# Skeleton for header

Essential for secure code.
1. Include guards
```c
#ifndef H
#define H
// ... declarations ...
#endif /* H */
```
`#pragma` once achieves the same goal and is simpler, but it's a compiler extension, not standard C.

2. extern "C"
```c
#ifdef __cplusplus
extern "C" {
#endif

// ... declarations ...

#ifdef __cplusplus
}
#endif
```
If someone includes your C header from a C++ test harness, without extern "C" the linker won't find your symbols because it's looking for mangled names that don't exist. This guard makes your library linkable from both languages.

3. Forward declarations of dependencies
If your header uses a type it didn't define, it must include or forward-declare it. A header should be self-contained: any .c file that includes only your header should compile without errors.
```c
#include <stddef.h>   /* size_t */
#include <stdint.h>   /* int32_t, uint8_t etc */
```
The rule: include the minimum set of standard headers your declarations directly require. Nothing more, because unnecessary includes slow compilation and pollute the consumer's namespace.

# Putting it together
```c
#ifndef HEADER
#define HEADER

#ifdef __cplusplus
extern "C" {
#endif

/* standard includes */
#include <stddef.h>
#include <stdint.h>

/* declarations go here */

#ifdef __cplusplus
}
#endif

#endif /* HEADER */

```