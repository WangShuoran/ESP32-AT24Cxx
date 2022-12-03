#include <stdio.h>
#include <unistd.h>

#include "at24cxx.h"
#include "trans_i2c.h"

SemaphoreHandle_t mutexHandle = NULL;

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

int at24cxxSetError(void) {
    // uint8_t cmd = 80 << 1 | I2C_MASTER_WRITE;
    uint8_t cmd = 80 << 1 | I2C_MASTER_READ;
    gpio_set_direction(I2C0_USED_IO_SCL, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_direction(I2C0_USED_IO_SDA, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_pullup_en(I2C0_USED_IO_SCL);
    gpio_pullup_en(I2C0_USED_IO_SDA);

    taskENTER_CRITICAL(&mux);
    // I2C开启
    gpio_set_level(I2C0_USED_IO_SDA, 1);
    gpio_set_level(I2C0_USED_IO_SCL, 1);
    usleep(1);
    gpio_set_level(I2C0_USED_IO_SDA, 0);
    usleep(1);
    // I2C发送数据
    for (int8_t i = 7; i >= 0; i--) {
        bool bit = cmd >> i & 1;
        gpio_set_level(I2C0_USED_IO_SCL, 0);
        gpio_set_level(I2C0_USED_IO_SDA, bit);
        usleep(1);
        gpio_set_level(I2C0_USED_IO_SCL, 1);
        usleep(1);
    }
    // I2C结束发送
    gpio_set_level(I2C0_USED_IO_SCL, 0);
    usleep(1);
    gpio_set_level(I2C0_USED_IO_SCL, 1);
    gpio_set_level(I2C0_USED_IO_SDA, 1);  // SDA主机释放必须在SCL释放之后
    usleep(1);
    // I2C获取ACK
    int ret = gpio_get_level(I2C0_USED_IO_SDA);
    gpio_set_level(I2C0_USED_IO_SCL, 0);
    usleep(1);
    // I2C结束
    gpio_set_level(I2C0_USED_IO_SDA, 0);
    usleep(1);
    gpio_set_level(I2C0_USED_IO_SCL, 1);
    usleep(1);
    gpio_set_level(I2C0_USED_IO_SDA, 1);
    usleep(1);
    taskEXIT_CRITICAL(&mux);

    gpio_reset_pin(I2C0_USED_IO_SCL);
    gpio_reset_pin(I2C0_USED_IO_SDA);

    printf("当前响应电平: %d\n", ret);
    return ret;
}

void app_main(void) {
    i2c0MasterInit();
    AT24Cxx_t *at24cxx = (AT24Cxx_t *)malloc(sizeof(AT24Cxx_t));
    at24cxx->id = AT24C02;
    at24cxxInit(at24cxx);
    mutexHandle = xSemaphoreCreateMutex();
    if (mutexHandle == NULL) {
        printf("互斥量创建失败，程序结束\n");
        return;
    }
    at24cxx->lock.smo_lock = mutexHandle;
    printf("延时\n");
    vTaskDelay(pdMS_TO_TICKS(3000));
    printf("开始\n");
    uint8_t data = 123;
    at24cxxWriteByte(at24cxx, 7, &data);

    data = 0;
    at24cxxReadByte(at24cxx, &data);
    printf("data:%d\n", data);
    uint8_t datas[260] = {0};
    at24cxxReadBytes(at24cxx, datas, 256);
    for (int i = 0; i < 256; i++) {
        printf("byte-datas:%d,%d\n", i, datas[i]);
    }

    // 当前电平
    int sdaLev = gpio_get_level(I2C0_USED_IO_SDA);
    int sclLev = gpio_get_level(I2C0_USED_IO_SCL);
    printf("I2C控制器开始时当前SDA SCL电平: %d %d\n", sdaLev, sclLev);

    // 时钟同步(防止I2C总线挂死)
    i2c0MasterOrSlaveDeInit();
    at24cxxSetError();
    at24cxxResetSync();
    i2c0MasterInit();

    data = 123;
    at24cxxWriteByte(at24cxx, 0, &data);
    data = 0;
    at24cxxReadCmdByte(at24cxx, 0, &data);
    printf("data:%d\n", data);
    for (int i = 0; i < 256; i++) {
        at24cxxWriteByte(at24cxx, i, &i);
    }

    memset(datas, 0, sizeof(uint8_t[260]));
    at24cxxReadCmdBytes(at24cxx, 0, datas, 256);
    for (int i = 0; i < 256; i++) {
        printf("cmd-datas:%d, %d\n", i, datas[i]);
    }

    // 得到数据是[64,226,1,0]，原因是MCU是小端模式
    int wIntData = 123456;
    at24cxxWritePage(at24cxx, 3, &wIntData, 4);
    int rIntData = 0;
    at24cxxReadCmdBytes(at24cxx, 3 * at24cxx->page_size, &rIntData, 4);
    printf("rIntData: %d\n", rIntData);

    at24cxxPrintAll(at24cxx);
    printf("结束\n");

    return;
}
