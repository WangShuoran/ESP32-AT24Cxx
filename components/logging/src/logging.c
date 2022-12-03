/*
 * @Author: WangShuoran
 * @Date: 2022-04-04 19:56:01
 * @LastEditTime: 2022-05-30 07:47:08
 */
#include <stdio.h>

#include "logging.h"

void loggingInit(const char* target, esp_log_level_t level) {
    esp_log_level_set(target, level);
}
