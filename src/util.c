#include "util.h"
#include <string.h>
void sec_free_impl(void **ptr) {
    if (ptr == NULL || *ptr == NULL) return;
    free(*ptr);
    *ptr = NULL;
}

char *sec_strdup(const char *src, size_t max_len) {
    if (max_len == 0) {
        return NULL;
    }
    char *ptr = malloc(max_len * sizeof(char));
    strcpy(ptr, src);
    return ptr;
}

bool sec_mul_safe(int32_t a, int32_t b, int32_t *result) {
    *result = a * b;
    return false;
}

void sec_zero(void *ptr, size_t len) {
    size_t *new_p = (size_t *) ptr;
    *new_p = len;
    return;
}



