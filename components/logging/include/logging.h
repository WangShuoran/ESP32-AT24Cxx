/*
 * @Author: WangShuoran
 * @Date: 2022-04-14 18:54:17
 * @LastEditTime: 2022-05-10 11:17:16
 */
#include "esp_log.h"

#ifndef __logging_h_
#define __logging_h_

#define OS_LOGE(tag, tips, ...) ({                                                                                                             \
    ESP_LOGE(tag, "[File:%s Line:%d Function:%s]-%s(%d):", __FILE__, __LINE__, __FUNCTION__, esp_log_system_timestamp(), esp_log_timestamp()); \
    ESP_LOGE(tag, tips, ##__VA_ARGS__);                                                                                                        \
})

#define OS_LOGW(tag, tips, ...) ({                                                                                                             \
    ESP_LOGW(tag, "[File:%s Line:%d Function:%s]-%s(%d):", __FILE__, __LINE__, __FUNCTION__, esp_log_system_timestamp(), esp_log_timestamp()); \
    ESP_LOGW(tag, tips, ##__VA_ARGS__);                                                                                                        \
})

#define OS_LOGI(tag, tips, ...) ({                                                                                                             \
    ESP_LOGI(tag, "[File:%s Line:%d Function:%s]-%s(%d):", __FILE__, __LINE__, __FUNCTION__, esp_log_system_timestamp(), esp_log_timestamp()); \
    ESP_LOGI(tag, tips, ##__VA_ARGS__);                                                                                                        \
})

#define OS_LOGD(tag, tips, ...) ({                                                                                                             \
    ESP_LOGD(tag, "[File:%s Line:%d Function:%s]-%s(%d):", __FILE__, __LINE__, __FUNCTION__, esp_log_system_timestamp(), esp_log_timestamp()); \
    ESP_LOGD(tag, tips, ##__VA_ARGS__);                                                                                                        \
})

#define OS_LOGV(tag, tips, ...) ({                                                                                                             \
    ESP_LOGV(tag, "[File:%s Line:%d Function:%s]-%s(%d):", __FILE__, __LINE__, __FUNCTION__, esp_log_system_timestamp(), esp_log_timestamp()); \
    ESP_LOGV(tag, tips, ##__VA_ARGS__);                                                                                                        \
})

#define OS_CHECK_ERROR(returned, expected, tag, tips, ...) ({ \
    if (returned != expected) {                               \
        OS_LOGE(tag, tips, ##__VA_ARGS__);                    \
    }                                                         \
})

#define OS_CHECK_WARN(returned, expected, tag, tips, ...) ({ \
    if (returned != expected) {                              \
        OS_LOGW(tag, tips, ##__VA_ARGS__);                   \
    }                                                        \
})

#define OS_CHECK_INFO(returned, expected, tag, tips, ...) ({ \
    if (returned != expected) {                              \
        OS_LOGI(tag, tips, ##__VA_ARGS__);                   \
    }                                                        \
})

#define OS_CHECK_DEBUG(returned, expected, tag, tips, ...) ({ \
    if (returned != expected) {                               \
        OS_LOGD(tag, tips, ##__VA_ARGS__);                    \
    }                                                         \
})

#define OS_CHECK_VERBOSE(returned, expected, tag, tips, ...) ({ \
    if (returned != expected) {                                 \
        OS_LOGV(tag, tips, ##__VA_ARGS__);                      \
    }                                                           \
})

void loggingInit(const char* target, esp_log_level_t level);

#endif  // __logging_h_
