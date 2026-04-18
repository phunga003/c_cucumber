#ifndef CUCUMBER_TESTS_TEST_RUNNER_H
#define CUCUMBER_TESTS_TEST_RUNNER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
    AI GENERATED SIMPLE TEST RUNNER. Made with Claude
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  /* fork, waitpid */
#include <sys/wait.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(cond, msg) do {                      \
    if (!(cond)) {                                       \
        fprintf(stderr, "  FAIL: %s (%s:%d)\n",         \
                msg, __FILE__, __LINE__);                \
        tests_failed++;                                  \
    } else {                                             \
        tests_passed++;                                  \
    }                                                    \
} while(0)

#define RUN_TEST(fn) do {                                \
    printf("[ RUN  ] %s\n", #fn);                       \
    pid_t pid = fork();                                  \
    if (pid == 0) { fn(); exit(tests_failed > 0 ? 1 : 0); } \
    int status;                                          \
    waitpid(pid, &status, 0);                            \
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {\
        printf("[ PASS ] %s\n", #fn);                   \
        tests_passed++;                                  \
    } else {                                             \
        printf("[ FAIL ] %s\n", #fn);                   \
        tests_failed++;                                  \
    }                                                    \
} while(0)

#define TEST_SUMMARY() do {                              \
    printf("\n%d passed, %d failed\n",                   \
           tests_passed, tests_failed);                  \
    return tests_failed > 0 ? 1 : 0;                    \
} while(0)

#ifdef __cplusplus
}
#endif

#endif /* CUCUMBER_TESTS_TEST_RUNNER_H */