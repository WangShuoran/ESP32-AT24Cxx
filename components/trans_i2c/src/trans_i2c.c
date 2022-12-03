/*
 * @Author: WangShuoran
 * @Date: 2022-03-25 21:25:13
 * @LastEditTime: 2022-03-29 16:24:54
 */
#include "trans_i2c.h"

static const char* TAG = "trans_i2c";

int i2c0MasterInit() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C0_USED_IO_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C0_USED_IO_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    esp_err_t ret = i2c_param_config(I2C_NUM_0, &conf);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_param_config失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }

    ret = i2c_driver_install(I2C_NUM_0, conf.mode, I2C0_RXD_BUFFER_SIZE, I2C0_TXD_BUFFER_SIZE, I2C0_INTR_FLAG);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_driver_install失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2c0SlaveInit(uint8_t slaveAddr) {
    i2c_config_t conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C0_USED_IO_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C0_USED_IO_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = slaveAddr,
        .slave.maximum_speed = 400000,
    };
    esp_err_t ret = i2c_param_config(I2C_NUM_0, &conf);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_param_config失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }

    ret = i2c_driver_install(I2C_NUM_0, conf.mode, I2C0_RXD_BUFFER_SIZE, I2C0_TXD_BUFFER_SIZE, I2C0_INTR_FLAG);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_driver_install失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2c0MasterOrSlaveDeInit() {
    esp_err_t ret = i2c_driver_delete(I2C_NUM_0);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_driver_delete失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2c0MasterRunCmd(i2c_cmd_handle_t cmd) {
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(I2C_DEFAULT_TIMEOUT));
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_master_cmd_begin失败");
        // assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

#if CSP_HAVE_I2C1
int i2c1MasterInit() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C1_USED_IO_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C1_USED_IO_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    esp_err_t ret = i2c_param_config(I2C_NUM_1, &conf);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_param_config失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }

    ret = i2c_driver_install(I2C_NUM_1, conf.mode, I2C1_RXD_BUFFER_SIZE, I2C1_TXD_BUFFER_SIZE, I2C1_INTR_FLAG);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_driver_install失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2c1SlaveInit(void) {
    i2c_config_t conf_slave = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C0_USED_IO_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C0_USED_IO_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = slaveAddr,
        .slave.maximum_speed = 400000,
    };
    esp_err_t ret = i2c_param_config(I2C_NUM_1, &conf);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_param_config失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }

    ret = i2c_driver_install(I2C_NUM_1, conf.mode, I2C1_RXD_BUFFER_SIZE, I2C1_TXD_BUFFER_SIZE, I2C1_INTR_FLAG);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_driver_install失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2c1MasterOrSlaveDeInit() {
    esp_err_t ret = i2c_driver_delete(I2C_NUM_1);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_driver_delete失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2c1MasterRunCmd(i2c_cmd_handle_t cmd) {
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_1, cmd, pdMS_TO_TICKS(I2C_DEFAULT_TIMEOUT));
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_master_cmd_begin失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}
#endif

int i2cCreateCmd(i2c_cmd_handle_t* cmd) {
    if (cmd == NULL) {
        OS_LOGD(TAG, "i2cCreateCmd失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    *cmd = i2c_cmd_link_create();
    if (*cmd == NULL) {
        OS_LOGD(TAG, "i2cCreateCmd失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2cDeleteCmd(i2c_cmd_handle_t cmd) {
    i2c_cmd_link_delete(cmd);
    return OS_SUCCESS;
}

int i2cCmdStart(i2c_cmd_handle_t cmd) {
    esp_err_t ret = i2c_master_start(cmd);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_master_start失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2cCmdStop(i2c_cmd_handle_t cmd) {
    esp_err_t ret = i2c_master_stop(cmd);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_master_stop失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2cCmdWriteData(i2c_cmd_handle_t cmd, uint8_t* data, int dataSize, bool ackFollow) {
    esp_err_t ret = i2c_master_write(cmd, data, dataSize, ackFollow);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_master_write失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2cCmdReadData(i2c_cmd_handle_t cmd, uint8_t* data, int dataSize, bool ackFollow) {
    i2c_ack_type_t follow = ackFollow ? I2C_MASTER_ACK : I2C_MASTER_LAST_NACK;
    esp_err_t ret = i2c_master_read(cmd, data, dataSize, follow);
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_master_read失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2cMasterReadDevice(int i2cNum, uint8_t devAddr, uint8_t* data, int dataSize) {
    esp_err_t ret = i2c_master_read_from_device(i2cNum, devAddr, data, dataSize, pdMS_TO_TICKS(I2C_DEFAULT_TIMEOUT));
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_master_read_from_device失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2cMasterWriteDevice(int i2cNum, uint8_t devAddr, uint8_t* data, int dataSize) {
    esp_err_t ret = i2c_master_write_to_device(i2cNum, devAddr, data, dataSize, pdMS_TO_TICKS(I2C_DEFAULT_TIMEOUT));
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_master_write_to_device失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2cMasterWriteReadDevice(int i2cNum, uint8_t devAddr, uint8_t* wData, int wDataSize, uint8_t* rData, int rDataSize) {
    esp_err_t ret = i2c_master_write_read_device(i2cNum, devAddr, wData, wDataSize, rData, rDataSize, pdMS_TO_TICKS(I2C_DEFAULT_TIMEOUT));
    if (ret != ESP_OK) {
        OS_LOGD(TAG, "i2c_master_write_read_device失败");
        assert(0);  // 在开发时强制重启
        return OS_FAILURE;
    }
    return OS_SUCCESS;
}

int i2cMasterDetectSlaveAddress(int i2cNum) {
    uint8_t address;
    esp_err_t ret;
    for (address = 1; address < 127; address++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        ret = i2c_master_start(cmd);
        if (ret != ESP_OK) {
            OS_LOGD(TAG, "i2c_master_start失败");
            assert(0);  // 在开发时强制重启
            return OS_FAILURE;
        }
        ret = i2c_master_write_byte(cmd, address << 1, 1);
        if (ret != ESP_OK) {
            OS_LOGD(TAG, "i2c_master_write_byte失败");
            assert(0);  // 在开发时强制重启
            return OS_FAILURE;
        }
        ret = i2c_master_stop(cmd);
        if (ret != ESP_OK) {
            OS_LOGD(TAG, "i2c_master_stop失败");
            assert(0);  // 在开发时强制重启
            return OS_FAILURE;
        }
        if (!i2c_master_cmd_begin(i2cNum, cmd, pdMS_TO_TICKS(I2C_DEFAULT_TIMEOUT))) {
            printf("slaver address: %d\n", address);
        }
        i2c_cmd_link_delete(cmd);
    }
    return OS_SUCCESS;
}

int i2c0SlaveRecvData(uint8_t* recvData, size_t recvDataLen, bool isErrCleanCache) {
    int err = i2c_slave_read_buffer(I2C_NUM_0, recvData, recvDataLen, pdMS_TO_TICKS(I2C_DEFAULT_TIMEOUT));
    if (err) {
        if (isErrCleanCache) {
            err = i2c_reset_rx_fifo(I2C_NUM_0);
        }
        if (err) {
            OS_LOGD(TAG, "i2c0SlaveRecvData失败");
            return OS_FAILURE;
        }
        OS_LOGD(TAG, "i2c0SlaveRecvData失败,但reset成功");

        return OS_SUCCESS;
    }
    return OS_SUCCESS;
}

int i2c0SlaveSendData(uint8_t* sendData, size_t sendDataLen, bool isErrCleanCache) {
    int err = i2c_slave_write_buffer(I2C_NUM_0, sendData, sendDataLen, pdMS_TO_TICKS(I2C_DEFAULT_TIMEOUT));
    if (err) {
        if (isErrCleanCache) {
            err = i2c_reset_tx_fifo(I2C_NUM_0);
        }
        if (err) {
            OS_LOGD(TAG, "i2c0SlaveSendData失败,且reset失败");
            return OS_FAILURE;
        }
        OS_LOGD(TAG, "i2c0SlaveSendData失败,但reset成功");
        return OS_SUCCESS;
    }
    return OS_SUCCESS;
}
