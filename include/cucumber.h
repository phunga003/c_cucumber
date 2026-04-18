#ifndef CUCUMBER_H
#define CUCUMBER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h> // int32_t

#define CUCUMBER_API __attribute__((visibility("default")))

typedef enum StepResult : int32_t {
    STEP_PASSED = 0,
    STEP_FAILED = 1,
    STEP_PENDING = 2
} StepResult;

#ifdef __cplusplus
}
#endif

#endif // !CUCUMBER_H