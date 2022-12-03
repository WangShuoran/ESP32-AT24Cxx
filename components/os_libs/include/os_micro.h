#include <stdlib.h>

#include "esp_log.h"

#ifndef __os_micro_h_
#define __os_micro_h_

#define FREEZERO(ptr)   \
    do {                \
        if (ptr) {      \
            free(ptr);  \
            ptr = NULL; \
        }               \
    } while (0)

#define OS_CHECK_TASK(taskStatus)             \
    do {                                      \
        if (taskStatus != pdPASS) {           \
            esp_system_abort("无法创建任务"); \
        }                                     \
    } while (0)

#define OS_ABORT(detail)          \
    do {                          \
        esp_system_abort(detail); \
    } while (0)

#endif  // __os_micro_h_
