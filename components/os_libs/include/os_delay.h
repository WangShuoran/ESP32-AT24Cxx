#include <stdint.h>
#include <unistd.h>

#include "esp_task_wdt.h"

#ifndef __os_delay_h_
#define __os_delay_h_

void osDelayMs(uint32_t delayMS);

#endif  // __os_delay_h_
