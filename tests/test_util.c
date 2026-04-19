#include "test_runner.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

void test_sec_free_null_outer(void) {
    sec_free_impl(NULL);
    /* if we reach here without crashing, the test passes */
    TEST_ASSERT(1, "sec_free_impl(NULL) must not crash");
}

void test_sec_free_null_inner(void) {
    void *p = NULL;
    sec_free_impl((void **)&p);
    TEST_ASSERT(p == NULL, "pointer must remain NULL");
}

void test_sec_free_poisons_pointer(void) {
    void *p = malloc(10);
    TEST_ASSERT(p != NULL, "malloc must succeed");
    sec_free_impl((void **)&p);
    TEST_ASSERT(p == NULL, "pointer must be NULL after sec_free_impl");
}

void test_sec_free_double_free_safe(void) {
    void *p = malloc(10);
    SEC_FREE(p);
    SEC_FREE(p); 
    TEST_ASSERT(p == NULL, "pointer must still be NULL");
}

void run_sec_free_impl_tests(void) {
    RUN_TEST(test_sec_free_null_outer);
    RUN_TEST(test_sec_free_null_inner);
    RUN_TEST(test_sec_free_poisons_pointer);
    RUN_TEST(test_sec_free_double_free_safe);
}

void test_sec_strdup_null_src(void) {
    char *dup = sec_strdup(NULL, 10);
    TEST_ASSERT(dup == NULL, "NULL src must return NULL");
}

void test_sec_strdup_empty_string(void) {
    char *dup = sec_strdup("", 1);
    TEST_ASSERT(dup != NULL,          "empty string with max_len=1 must succeed");
    TEST_ASSERT(strlen(dup) == 0,     "duplicated empty string must have length 0");
    free(dup);
}

void test_sec_strdup_len_equals_max_len(void) {
    char *dup = sec_strdup("hello", 5); 
    TEST_ASSERT(dup == NULL, "src length equal to max_len must return NULL");
}

void test_sec_strdup_len_exceeds_max_len(void) {
    char *dup = sec_strdup("hello", 4); 
    TEST_ASSERT(dup == NULL, "src longer than max_len must return NULL");
}

void test_sec_strdup_valid_copy(void) {
    char *dup = sec_strdup("hello", 6); 
    TEST_ASSERT(dup != NULL,              "valid input must return non-NULL");
    TEST_ASSERT(strcmp(dup, "hello") == 0, "duplicated string must match src");
    free(dup);
}

void test_sec_strdup_is_independent_copy(void) {
    char *src = "hello";
    char *dup = sec_strdup(src, 6);
    TEST_ASSERT(dup != NULL,  "valid input must return non-NULL");
    TEST_ASSERT(dup != src,   "result must be a distinct allocation");
    free(dup);
}

void test_sec_strdup_max_len_is_zero(void) {
    char *dup = sec_strdup("hello", 0); 
    TEST_ASSERT(dup == NULL, "max_len of 0 must return NULL");
}

void run_sec_strdup_tests(void) {
    RUN_TEST(test_sec_strdup_null_src);
    RUN_TEST(test_sec_strdup_empty_string);
    RUN_TEST(test_sec_strdup_len_equals_max_len);
    RUN_TEST(test_sec_strdup_len_exceeds_max_len);
    RUN_TEST(test_sec_strdup_valid_copy);
    RUN_TEST(test_sec_strdup_is_independent_copy);
    RUN_TEST(test_sec_strdup_max_len_is_zero);
}

void test_sec_mul_safe_both_zero(void) {
    int32_t out = 99;
    bool res = sec_mul_safe(0, 0, &out);
    TEST_ASSERT(res,       "0 x 0 should succeed");
    TEST_ASSERT(out == 0,  "0 x 0 should produce 0");
}

void test_sec_mul_safe_one_zero(void) {
    int32_t out = 99;
    bool res = sec_mul_safe(0, 5, &out);
    TEST_ASSERT(res,      "0 x 5 should succeed");
    TEST_ASSERT(out == 0, "0 x 5 should produce 0");

    out = 99;
    res = sec_mul_safe(5, 0, &out);
    TEST_ASSERT(res,      "5 x 0 should succeed");
    TEST_ASSERT(out == 0, "5 x 0 should produce 0");
}

void test_sec_mul_safe_valid_positive(void) {
    int32_t out = 99;
    bool res = sec_mul_safe(5, 3, &out);
    TEST_ASSERT(res,       "5 x 3 should succeed");
    TEST_ASSERT(out == 15, "5 x 3 should produce 15");
}

void test_sec_mul_safe_max_boundary(void) {
    int32_t out = 99;
    bool res = sec_mul_safe(INT32_MAX, 1, &out);
    TEST_ASSERT(res,              "INT32_MAX x 1 should succeed");
    TEST_ASSERT(out == INT32_MAX, "INT32_MAX x 1 should produce INT32_MAX");
}

void test_sec_mul_safe_overflow(void) {
    int32_t out = 5;
    bool res = sec_mul_safe(INT32_MAX, 16, &out);
    TEST_ASSERT(!res,     "INT32_MAX x 16 should fail");
    TEST_ASSERT(out == 5, "out must be unchanged on overflow");
}

void test_sec_mul_safe_negative_a(void) {
    int32_t out = 5;
    bool res = sec_mul_safe(-1, 16, &out);
    TEST_ASSERT(!res,     "-1 x 16 should fail");
    TEST_ASSERT(out == 5, "out must be unchanged on negative input");
}

void test_sec_mul_safe_negative_b(void) {
    int32_t out = 5;
    bool res = sec_mul_safe(16, -1, &out);
    TEST_ASSERT(!res,     "16 x -1 should fail");
    TEST_ASSERT(out == 5, "out must be unchanged on negative input");
}

void test_sec_mul_safe_null_out(void) {
    bool res = sec_mul_safe(5, 3, NULL);
    TEST_ASSERT(!res, "NULL out must return false");
}

void run_sec_mul_safe_tests(void) {
    RUN_TEST(test_sec_mul_safe_both_zero);
    RUN_TEST(test_sec_mul_safe_one_zero);
    RUN_TEST(test_sec_mul_safe_valid_positive);
    RUN_TEST(test_sec_mul_safe_max_boundary);
    RUN_TEST(test_sec_mul_safe_overflow);
    RUN_TEST(test_sec_mul_safe_negative_a);
    RUN_TEST(test_sec_mul_safe_negative_b);
    RUN_TEST(test_sec_mul_safe_null_out);
}

/*
 * NOTE: these tests verify observable runtime behavior of sec_zero.
 * They do NOT verify the optimizer guarantee — see release-check target.
 */

void test_sec_zero_full_buffer(void) {
    int32_t *arr = malloc(sizeof(int32_t) * 4);
    for (int32_t i = 0; i < 4; i++) arr[i] = i + 1;

    sec_zero(arr, sizeof(int32_t) * 4);

    for (int32_t i = 0; i < 4; i++) {
        TEST_ASSERT(arr[i] == 0, "all bytes must be zero after full wipe");
    }

    free(arr);
}

void test_sec_zero_partial_buffer(void) {
    int32_t *arr = malloc(sizeof(int32_t) * 4);
    for (int32_t i = 0; i < 4; i++) arr[i] = i + 1;

    sec_zero(arr, sizeof(int32_t) * 2);   /* zero first two elements only */

    for (int32_t i = 0; i < 4; i++) {
        if (i < 2) {
            TEST_ASSERT(arr[i] == 0,     "index < 2 must be zeroed");
        } else {
            TEST_ASSERT(arr[i] == i + 1, "index >= 2 must be unchanged");
        }
    }

    free(arr);
}

void test_sec_zero_zero_length(void) {
    int32_t *arr = malloc(sizeof(int32_t) * 4);
    for (int32_t i = 0; i < 4; i++) arr[i] = i + 1;

    sec_zero(arr, 0);

    for (int32_t i = 0; i < 4; i++) {
        TEST_ASSERT(arr[i] == i + 1, "zero-length wipe must not modify buffer");
    }

    free(arr);
}

void test_sec_zero_null_pointer(void) {
    sec_zero(NULL, 4);
    TEST_ASSERT(1, "sec_zero(NULL, n) must not crash");
}

void run_sec_zero_tests(void) {
    RUN_TEST(test_sec_zero_full_buffer);
    RUN_TEST(test_sec_zero_partial_buffer);
    RUN_TEST(test_sec_zero_zero_length);
    RUN_TEST(test_sec_zero_null_pointer);
}


int main(void) {
    TEST_INIT();

    run_sec_free_impl_tests();
    run_sec_strdup_tests();
    run_sec_mul_safe_tests();
    run_sec_zero_tests();

    TEST_SUMMARY();
}

