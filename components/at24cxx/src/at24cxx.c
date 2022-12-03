#include "at24cxx.h"

#include <unistd.h>

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

/**
 * @brief 如果为NULL/0时执行返回
 *
 */
#define CHECK_ISZERO_RET(a, ret) \
    if (!(a)) {                  \
        return (ret);            \
    }

int at24cxxInit(AT24Cxx_t* at24cxx) {
    CHECK_ISZERO_RET(at24cxx, OS_FAILURE);
    switch (at24cxx->id) {
    case AT24C01:
        at24cxx->capacity = 128;
        at24cxx->page_count = 16;
        at24cxx->page_size = 8;
        at24cxx->i2c_addr = AT24Cxx_BASE_ADDR;
        break;
    case AT24C02:
        at24cxx->capacity = 256;
        at24cxx->page_count = 32;
        at24cxx->page_size = 8;
        at24cxx->i2c_addr = AT24Cxx_BASE_ADDR;
        break;
    case AT24C04:
        at24cxx->capacity = 512;
        at24cxx->page_count = 32;
        at24cxx->page_size = 16;
        at24cxx->i2c_addr = AT24Cxx_BASE_ADDR;
        break;
    case AT24C08:
        at24cxx->capacity = 1024;
        at24cxx->page_count = 32;
        at24cxx->page_size = 16;
        at24cxx->i2c_addr = AT24Cxx_BASE_ADDR;
        break;
    case AT24C16:
        at24cxx->capacity = 2048;
        at24cxx->page_count = 32;
        at24cxx->page_size = 16;
        at24cxx->i2c_addr = AT24Cxx_BASE_ADDR;
        break;
    default:
        at24cxx->id = AT24C02;
        at24cxx->capacity = 256;
        at24cxx->page_count = 32;
        at24cxx->page_size = 8;
        at24cxx->i2c_addr = AT24Cxx_BASE_ADDR;
        break;
    }
    return OS_SUCCESS;
}

int at24cxxDeInit(AT24Cxx_t* at24cxx) {
    CHECK_ISZERO_RET(at24cxx, OS_FAILURE);
    memset(at24cxx, 0, sizeof(AT24Cxx_t));
    return OS_SUCCESS;
}

int at24cxxDeInitWithFree(AT24Cxx_t** at24cxx) {
    CHECK_ISZERO_RET(at24cxx && *at24cxx, OS_FAILURE);
    free(*at24cxx);
    *at24cxx = NULL;
    return OS_SUCCESS;
}

int at24cxxSetBlock(AT24Cxx_t* at24cxx, uint8_t block) {
    CHECK_ISZERO_RET(at24cxx, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id == AT24C01 || at24cxx->id == AT24C02, OS_FAILURE);
    at24cxx->i2c_addr = AT24Cxx_BASE_ADDR | block;
    return OS_SUCCESS;
}

int at24cxxResetSync(void) {
    gpio_set_direction(I2C0_USED_IO_SCL, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_direction(I2C0_USED_IO_SDA, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_pullup_en(I2C0_USED_IO_SCL);
    gpio_pullup_en(I2C0_USED_IO_SDA);

    taskENTER_CRITICAL(&mux);
    gpio_set_level(I2C0_USED_IO_SDA, 1);
    usleep(1);
    for (int i = 0; i < 9; i++) {
        gpio_set_level(I2C0_USED_IO_SCL, 1);
        usleep(1);
        gpio_set_level(I2C0_USED_IO_SCL, 0);
        usleep(1);
    }
    gpio_set_level(I2C0_USED_IO_SCL, 1);
    taskEXIT_CRITICAL(&mux);

    gpio_reset_pin(I2C0_USED_IO_SCL);
    gpio_reset_pin(I2C0_USED_IO_SDA);
    return OS_SUCCESS;
}

int at24cxxIsBusy(AT24Cxx_t* at24cxx) {
    CHECK_ISZERO_RET(at24cxx, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    i2c_cmd_handle_t cmd;
    i2cCreateCmd(&cmd);
    i2cCmdStart(cmd);
    uint8_t data[] = {at24cxx->i2c_addr << 1 | I2C_MASTER_WRITE};
    i2cCmdWriteData(cmd, data, sizeof(data), 1);
    i2cCmdStop(cmd);
    int ret = i2c0MasterRunCmd(cmd);
    i2c0MasterRunCmd(cmd);
    i2cDeleteCmd(cmd);
    if (ret != OS_FAILURE) {
        return false;
    }
    return true;
}

int at24cxxEraseChip(AT24Cxx_t* at24cxx, uint8_t content) {
    CHECK_ISZERO_RET(at24cxx, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    int ret = 0;
    switch (at24cxx->id) {
    case AT24C01:
        ret = at24cxxEraseBlock(at24cxx, content);
        break;
    case AT24C02:
        ret = at24cxxEraseBlock(at24cxx, content);
        break;
    case AT24C04:
        for (int i = 0; i < content; i++) {
            at24cxxSetBlock(at24cxx, i);
            ret |= at24cxxEraseBlock(at24cxx, content);
        }
        break;
    case AT24C08:
        for (int i = 0; i < content; i++) {
            at24cxxSetBlock(at24cxx, i);
            ret |= at24cxxEraseBlock(at24cxx, content);
        }
        break;
    case AT24C16:
        for (int i = 0; i < content; i++) {
            at24cxxSetBlock(at24cxx, i);
            ret |= at24cxxEraseBlock(at24cxx, content);
        }
        break;
    default:
        ret = at24cxxEraseBlock(at24cxx, content);
        break;
    }
    if (ret != OS_SUCCESS) {
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int at24cxxEraseBlock(AT24Cxx_t* at24cxx, uint8_t content) {
    CHECK_ISZERO_RET(at24cxx, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    for (int i = 0; i < at24cxx->capacity; i++) {
        printf("%d\n", content);
        at24cxxWriteByte(at24cxx, i, &content);
    }
    return OS_FAILURE;
}

int at24cxxWritePage(AT24Cxx_t* at24cxx, uint8_t pageOffset, uint8_t* content, uint8_t contentLen) {
    CHECK_ISZERO_RET(at24cxx && content, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    CHECK_ISZERO_RET(pageOffset < at24cxx->page_count, OS_FAILURE);
    uint8_t offset = pageOffset * at24cxx->page_size;
    i2c_cmd_handle_t cmd;
    i2cCreateCmd(&cmd);
    i2cCmdStart(cmd);

    uint8_t data[] = {at24cxx->i2c_addr << 1 | I2C_MASTER_WRITE, offset};
    i2cCmdWriteData(cmd, data, sizeof(data), 1);
    i2cCmdWriteData(cmd, content, contentLen, 1);

    i2cCmdStop(cmd);

#if AT24Cxx_LOCK_METHOD == 1
    int oriRet = at24cxx->lock.ori_lock;
    if (oriRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 2
    int atmRet = atomic_load_explicit(&at24cxx->lock.atm_lock, memory_order_relaxed);
    if (atmRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 3
    BaseType_t smoRet = xSemaphoreTake(at24cxx->lock.smo_lock, AT24Cxx_TIMEOUT);
    if (smoRet == pdPASS) {
#endif
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISLOCKED;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISLOCKED, memory_order_relaxed);
#endif
        int timeout = AT24Cxx_BUSY_TIMEOUT;
        while (timeout > 0 and at24cxxIsBusy(at24cxx) == true) {
            vTaskDelay(pdMS_TO_TICKS(10));
            timeout -= 10;
        }
        if (timeout <= 0) {
#if AT24Cxx_LOCK_METHOD == 1
            at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
            atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
            assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
            i2cDeleteCmd(cmd);
            return OS_FAILURE;
        }
        i2c0MasterRunCmd(cmd);
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
        assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
    } else {
        i2cDeleteCmd(cmd);
        return OS_FAILURE;
    }

    i2cDeleteCmd(cmd);
    return OS_SUCCESS;
}

int at24cxxWriteByte(AT24Cxx_t* at24cxx, uint8_t offset, uint8_t* content) {
    CHECK_ISZERO_RET(at24cxx && content, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    i2c_cmd_handle_t cmd;
    i2cCreateCmd(&cmd);
    i2cCmdStart(cmd);

    uint8_t data[] = {at24cxx->i2c_addr << 1 | I2C_MASTER_WRITE, offset, *content};
    i2cCmdWriteData(cmd, data, sizeof(data), 1);

    i2cCmdStop(cmd);

#if AT24Cxx_LOCK_METHOD == 1
    int oriRet = at24cxx->lock.ori_lock;
    if (oriRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 2
    int atmRet = atomic_load_explicit(&at24cxx->lock.atm_lock, memory_order_relaxed);
    if (atmRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 3
    BaseType_t smoRet = xSemaphoreTake(at24cxx->lock.smo_lock, AT24Cxx_TIMEOUT);
    if (smoRet == pdPASS) {
#endif
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISLOCKED;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISLOCKED, memory_order_relaxed);
#endif
        int timeout = AT24Cxx_BUSY_TIMEOUT;
        while (timeout > 0 and at24cxxIsBusy(at24cxx) == true) {
            vTaskDelay(pdMS_TO_TICKS(10));
            timeout -= 10;
        }
        if (timeout <= 0) {
#if AT24Cxx_LOCK_METHOD == 1
            at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
            atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
            assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
            i2cDeleteCmd(cmd);
            return OS_FAILURE;
        }
        i2c0MasterRunCmd(cmd);
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
        assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
    } else {
        i2cDeleteCmd(cmd);
        return OS_FAILURE;
    }

    i2cDeleteCmd(cmd);
    return OS_SUCCESS;
}

int at24cxxReadByte(AT24Cxx_t* at24cxx, uint8_t* content) {
    CHECK_ISZERO_RET(at24cxx && content, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    i2c_cmd_handle_t cmd;
    i2cCreateCmd(&cmd);
    i2cCmdStart(cmd);

    uint8_t data[] = {at24cxx->i2c_addr << 1 | I2C_MASTER_READ};
    i2cCmdWriteData(cmd, data, sizeof(data), 1);
    i2cCmdReadData(cmd, content, 1, 0);

    i2cCmdStop(cmd);

#if AT24Cxx_LOCK_METHOD == 1
    int oriRet = at24cxx->lock.ori_lock;
    if (oriRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 2
    int atmRet = atomic_load_explicit(&at24cxx->lock.atm_lock, memory_order_relaxed);
    if (atmRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 3
    BaseType_t smoRet = xSemaphoreTake(at24cxx->lock.smo_lock, AT24Cxx_TIMEOUT);
    if (smoRet == pdPASS) {
#endif
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISLOCKED;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISLOCKED, memory_order_relaxed);
#endif
        int timeout = AT24Cxx_BUSY_TIMEOUT;
        while (timeout > 0 and at24cxxIsBusy(at24cxx) == true) {
            vTaskDelay(pdMS_TO_TICKS(10));
            timeout -= 10;
        }
        if (timeout <= 0) {
#if AT24Cxx_LOCK_METHOD == 1
            at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
            atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
            assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
            i2cDeleteCmd(cmd);
            return OS_FAILURE;
        }
        i2c0MasterRunCmd(cmd);
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
        assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
    } else {
        i2cDeleteCmd(cmd);
        return OS_FAILURE;
    }

    i2cDeleteCmd(cmd);
    return OS_SUCCESS;
}

int at24cxxReadBytes(AT24Cxx_t* at24cxx, uint8_t* content, int contentLen) {
    CHECK_ISZERO_RET(at24cxx && content, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    i2c_cmd_handle_t cmd;
    i2cCreateCmd(&cmd);
    i2cCmdStart(cmd);

    uint8_t data[] = {at24cxx->i2c_addr << 1 | I2C_MASTER_READ};
    i2cCmdWriteData(cmd, data, sizeof(data), 1);
    i2cCmdReadData(cmd, content, contentLen, 0);

    i2cCmdStop(cmd);

#if AT24Cxx_LOCK_METHOD == 1
    int oriRet = at24cxx->lock.ori_lock;
    if (oriRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 2
    int atmRet = atomic_load_explicit(&at24cxx->lock.atm_lock, memory_order_relaxed);
    if (atmRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 3
    BaseType_t smoRet = xSemaphoreTake(at24cxx->lock.smo_lock, AT24Cxx_TIMEOUT);
    if (smoRet == pdPASS) {
#endif
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISLOCKED;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISLOCKED, memory_order_relaxed);
#endif
        int timeout = AT24Cxx_BUSY_TIMEOUT;
        while (timeout > 0 and at24cxxIsBusy(at24cxx) == true) {
            vTaskDelay(pdMS_TO_TICKS(10));
            timeout -= 10;
        }
        if (timeout <= 0) {
#if AT24Cxx_LOCK_METHOD == 1
            at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
            atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
            assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
            i2cDeleteCmd(cmd);
            return OS_FAILURE;
        }
        i2c0MasterRunCmd(cmd);
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
        assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
    } else {
        i2cDeleteCmd(cmd);
        return OS_FAILURE;
    }

    i2cDeleteCmd(cmd);
    return OS_SUCCESS;
}

int at24cxxReadCmdByte(AT24Cxx_t* at24cxx, uint8_t offset, uint8_t* content) {
    CHECK_ISZERO_RET(at24cxx && content, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    i2c_cmd_handle_t cmd;
    i2cCreateCmd(&cmd);
    i2cCmdStart(cmd);

    uint8_t data_write[] = {at24cxx->i2c_addr << 1 | I2C_MASTER_WRITE, offset};
    i2cCmdWriteData(cmd, data_write, sizeof(data_write), 1);

    i2cCmdStart(cmd);
    uint8_t data_read[] = {at24cxx->i2c_addr << 1 | I2C_MASTER_READ};
    i2cCmdWriteData(cmd, data_read, sizeof(data_read), 1);

    i2cCmdReadData(cmd, content, 1, 0);

    i2cCmdStop(cmd);

#if AT24Cxx_LOCK_METHOD == 1
    int oriRet = at24cxx->lock.ori_lock;
    if (oriRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 2
    int atmRet = atomic_load_explicit(&at24cxx->lock.atm_lock, memory_order_relaxed);
    if (atmRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 3
    BaseType_t smoRet = xSemaphoreTake(at24cxx->lock.smo_lock, AT24Cxx_TIMEOUT);
    if (smoRet == pdPASS) {
#endif
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISLOCKED;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISLOCKED, memory_order_relaxed);
#endif
        int timeout = AT24Cxx_BUSY_TIMEOUT;
        while (timeout > 0 and at24cxxIsBusy(at24cxx) == true) {
            vTaskDelay(pdMS_TO_TICKS(10));
            timeout -= 10;
        }
        if (timeout <= 0) {
#if AT24Cxx_LOCK_METHOD == 1
            at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
            atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
            assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
            i2cDeleteCmd(cmd);
            return OS_FAILURE;
        }
        i2c0MasterRunCmd(cmd);
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
        assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
    } else {
        i2cDeleteCmd(cmd);
        return OS_FAILURE;
    }

    i2cDeleteCmd(cmd);
    return OS_SUCCESS;
}

int at24cxxReadCmdBytes(AT24Cxx_t* at24cxx, uint8_t offset, uint8_t* content, int contentLen) {
    CHECK_ISZERO_RET(at24cxx && content, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    i2c_cmd_handle_t cmd;
    i2cCreateCmd(&cmd);
    i2cCmdStart(cmd);

    uint8_t data_write[] = {at24cxx->i2c_addr << 1 | I2C_MASTER_WRITE, offset};
    i2cCmdWriteData(cmd, data_write, sizeof(data_write), 1);

    i2cCmdStart(cmd);
    uint8_t data_read[] = {at24cxx->i2c_addr << 1 | I2C_MASTER_READ};
    i2cCmdWriteData(cmd, data_read, sizeof(data_read), 1);

    i2cCmdReadData(cmd, content, contentLen, 0);

    i2cCmdStop(cmd);

#if AT24Cxx_LOCK_METHOD == 1
    int oriRet = at24cxx->lock.ori_lock;
    if (oriRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 2
    int atmRet = atomic_load_explicit(&at24cxx->lock.atm_lock, memory_order_relaxed);
    if (atmRet == AT24Cxx_LOCK_ISRELEASE) {
#elif AT24Cxx_LOCK_METHOD == 3
    BaseType_t smoRet = xSemaphoreTake(at24cxx->lock.smo_lock, AT24Cxx_TIMEOUT);
    if (smoRet == pdPASS) {
#endif
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISLOCKED;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISLOCKED, memory_order_relaxed);
#endif
        int timeout = AT24Cxx_BUSY_TIMEOUT;
        while (timeout > 0 and at24cxxIsBusy(at24cxx) == true) {
            vTaskDelay(pdMS_TO_TICKS(10));
            timeout -= 10;
        }
        if (timeout <= 0) {
#if AT24Cxx_LOCK_METHOD == 1
            at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
            atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
            assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
            i2cDeleteCmd(cmd);
            return OS_FAILURE;
        }
        i2c0MasterRunCmd(cmd);
#if AT24Cxx_LOCK_METHOD == 1
        at24cxx->lock.ori_lock = AT24Cxx_LOCK_ISRELEASE;
#elif AT24Cxx_LOCK_METHOD == 2
        atomic_store_explicit(&at24cxx->lock.atm_lock, AT24Cxx_LOCK_ISRELEASE, memory_order_relaxed);
#elif AT24Cxx_LOCK_METHOD == 3
        assert(xSemaphoreGive(at24cxx->lock.smo_lock) == pdPASS);
#endif
    } else {
        i2cDeleteCmd(cmd);
        return OS_FAILURE;
    }

    i2cDeleteCmd(cmd);
    return OS_SUCCESS;
}

int at24cxxPrintAll(AT24Cxx_t* at24cxx) {
    CHECK_ISZERO_RET(at24cxx, OS_FAILURE);
    CHECK_ISZERO_RET(at24cxx->id, OS_FAILURE);
    uint8_t content;
    for (int i = 0; i < at24cxx->capacity; i++) {
        at24cxxReadCmdByte(at24cxx, i, &content);
        printf("AT24Cxx-%d: %d\n", i, content);
    }
    return OS_SUCCESS;
}
