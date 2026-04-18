#ifndef CUCUMBER_H
#define CUCUMBER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h> // INT32_MAX, int32_t

#define CUCUMBER_API __attribute__((visibility("default")))

typedef enum {
    STEP_PASSED = 0,
    STEP_FAILED = 1,
    STEP_PENDING = 2,
    STEP_RESULT_MAX = INT32_MAX /* do not use — ABI size sentinel */
} StepResult;

typedef struct {
    int32_t rows;
    int32_t cols;
    char **cells; /* row-major: cells[row * cols + col] */
} DataTable;

typedef enum {
    GIVEN = 0,
    THEN = 1,
    WHEN = 2,
    AND = 3,
    BUT = 4,
    KEYWORD_MAX = INT32_MAX /* do not use — ABI size sentinel */
} Keyword;

/*
 * Represents a single Gherkin step.
 * keyword is stored for reporting purposes only — never used for matching.
 * datatable is NULL if the step has no attached table.
 */
typedef struct {
    Keyword    keyword;
    char      *text;        /* step text, owned by this struct */
    DataTable *datatable;   /* NULL if absent, owned by this struct */
} Step;

void datatable_free(DataTable *dt);

#ifdef __cplusplus
}
#endif

#endif // !CUCUMBER_H