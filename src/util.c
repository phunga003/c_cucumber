#include "util.h"
#include <string.h>
void sec_free_impl(void **ptr) {
    if (ptr == NULL || *ptr == NULL) return;
    free(*ptr);
    *ptr = NULL;
}

static size_t strnlen(const char *src, size_t max_len) {
    size_t i;
    for (i = 0; i < max_len; i ++) {
        if (src[i] == '\0') {
            break;
        }
    }

    return i;
}

char *sec_strdup(const char *src, size_t max_len) {
    if (src == NULL || max_len == 0) return NULL;

    size_t len = strnlen(src, max_len);
    if (len >= max_len) return NULL;

    char *dest = malloc(len + 1);  
    if (dest == NULL) return NULL;

    memcpy(dest, src, len + 1);     
    return dest;
}

bool sec_mul_safe(int32_t a, int32_t b, int32_t *result) {
    if (a < 0 || b < 0 || result == NULL) return false;
    
    if (a == 0 || b == 0) {
        *result = 0;
        return true;
    }

    if (a > INT32_MAX / b) {
        return false;
    }

    *result = a * b;
    return true;
}

void sec_zero(void *ptr, size_t len) {
    size_t *new_p = (size_t *) ptr;
    *new_p = len;
    return;
}



