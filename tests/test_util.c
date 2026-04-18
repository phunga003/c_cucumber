#include "test_runner.h"
#include "internal/util.h"
#include <stdlib.h>

/* Case: ptr argument itself is NULL — must not crash */
void test_sec_free_null_outer(void) {
    sec_free_impl(NULL);
    /* if we reach here without crashing, the test passes */
    TEST_ASSERT(1, "sec_free_impl(NULL) must not crash");
}

/* Case: inner pointer is NULL — must not crash, pointer stays NULL */
void test_sec_free_null_inner(void) {
    void *p = NULL;
    sec_free_impl((void **)&p);
    TEST_ASSERT(p == NULL, "pointer must remain NULL");
}

/* Case: valid allocation — must free and poison to NULL */
void test_sec_free_poisons_pointer(void) {
    void *p = malloc(10);
    TEST_ASSERT(p != NULL, "malloc must succeed");
    sec_free_impl((void **)&p);
    TEST_ASSERT(p == NULL, "pointer must be NULL after sec_free_impl");
}

/* Case: double call via SEC_FREE — second call must be safe no-op */
void test_sec_free_double_free_safe(void) {
    void *p = malloc(10);
    SEC_FREE(p);
    SEC_FREE(p);   /* p is NULL — must be safe */
    TEST_ASSERT(p == NULL, "pointer must still be NULL");
}

void run_sec_free_impl_tests(void) {
    RUN_TEST(test_sec_free_null_outer);
    RUN_TEST(test_sec_free_null_inner);
    RUN_TEST(test_sec_free_poisons_pointer);
    RUN_TEST(test_sec_free_double_free_safe);
}


int main(void) {
    run_sec_free_impl_tests();
    TEST_SUMMARY();
}