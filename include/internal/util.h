#ifndef CUCUMBER_INTERNAL_UTIL_H
#define CUCUMBER_INTERNAL_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> // size_t
#include <stdint.h> // int32_t
#include <stdbool.h> // bool
#include <stdlib.h> // free

/*
 * SEC_FREE(ptr) — safely free and poison a pointer.
 *
 * Macro for call-site ergonomics; delegates to sec_free_impl
 * for actual work. After this call, ptr is NULL.
 * Do not pass a pointer you do not own.
 * ptr must be an lvalue (a named variable).
 */
#define SEC_FREE(ptr) sec_free_impl((void **)&(ptr))

/*
 * Frees *ptr and sets *ptr to NULL.
 * Safe no-op if ptr is NULL or *ptr is NULL.
 * NOT thread-safe — caller must ensure exclusive access to ptr.
 * Use SEC_FREE macro at call sites for ergonomics.
 */
void sec_free_impl(void **ptr);

/*
 * sec_strdup — bounds-checked string duplication.
 * Returns NULL if src is NULL or strlen(src) >= max_len.
 * Caller owns the returned pointer.
 */
char *sec_strdup(const char *src, size_t max_len);

/*
 * sec_mul_safe — multiply two int32_t values with overflow detection.
 * Returns true and writes to result if safe.
 * Returns false if overflow would occur, result is unchanged.
 */
bool sec_mul_safe(int32_t a, int32_t b, int32_t *result);

/*
 * sec_zero — zero memory in a way the compiler cannot optimize away.
 * Use before freeing buffers that may contain sensitive data.
 */
void sec_zero(void *ptr, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* CUCUMBER_INTERNAL_UTIL_H */
