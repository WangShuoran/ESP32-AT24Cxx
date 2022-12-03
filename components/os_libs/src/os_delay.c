#include "os_delay.h"

void osDelayMs(uint32_t delayMS) {
    if (delayMS <= 0) {
        return;
    }
    while (delayMS--) {
        usleep(1000);
        esp_task_wdt_reset();
    }
}
