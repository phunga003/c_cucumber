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
#include <unistd.h>   /* isatty */

static int use_color = 0;

/* Call once at the start of main() */
#define TEST_INIT() do { \
    use_color = isatty(STDOUT_FILENO); \
} while(0)

#define COLOR_RED    (use_color ? "\x1b[31m" : "")
#define COLOR_GREEN  (use_color ? "\x1b[32m" : "")
#define COLOR_YELLOW (use_color ? "\x1b[33m" : "")
#define COLOR_RESET  (use_color ? "\x1b[0m"  : "")

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


#define RUN_TEST(fn) do {                                               \
    pid_t pid = fork();                                                 \
    if (pid == 0) { fn(); exit(tests_failed > 0 ? 1 : 0); }           \
    int status;                                                         \
    waitpid(pid, &status, 0);                                           \
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {               \
        printf("%s[ PASS ]%s %s\n", COLOR_GREEN, COLOR_RESET, #fn);   \
        tests_passed++;                                                 \
    } else {                                                            \
        printf("%s[ FAIL ]%s %s\n", COLOR_RED, COLOR_RESET, #fn);     \
        tests_failed++;                                                 \
    }                                                                   \
} while(0)

#define TEST_SUMMARY() do {                                             \
    printf("\n%s%d passed%s, %s%d failed%s\n",                         \
           COLOR_GREEN, tests_passed, COLOR_RESET,                     \
           COLOR_RED,   tests_failed, COLOR_RESET);                    \
    return tests_failed > 0 ? 1 : 0;                                   \
} while(0)

#ifdef __cplusplus
}
#endif

#endif /* CUCUMBER_TESTS_TEST_RUNNER_H */
