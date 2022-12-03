#include <stdbool.h>
#include <stdio.h>

#include "driver/i2c.h"
#include "logging.h"
#include "os_errno.h"
#include "sdkconfig.h"

#ifndef __trans_i2c_h_
#define __trans_i2c_h_

/* I2C的芯片适配 */
#if CONFIG_IDF_TARGET_ESP32
#define CSP_HAVE_I2C1 1 /* 当芯片有I2C_NUM_1 */
#elif CONFIG_IDF_TARGET_ESP32S2
#define CSP_HAVE_I2C1 1 /* 当芯片有I2C_NUM_1 */
#elif CONFIG_IDF_TARGET_ESP32S3
#define CSP_HAVE_I2C1 1 /* 当芯片有I2C_NUM_1 */
#elif CONFIG_IDF_TARGET_ESP32C3
#define CSP_HAVE_I2C1 0 /* 当芯片无I2C_NUM_1 */
#endif

#define I2C_DEFAULT_TIMEOUT 1000

#define I2C0_TXD_BUFFER_SIZE 256
#define I2C0_RXD_BUFFER_SIZE 4096

#define DEFAULT_I2C0_USED_IO_SCL GPIO_NUM_1
#define DEFAULT_I2C0_USED_IO_SDA GPIO_NUM_2

#ifdef CONFIG_I2C0_USED_IO_SCL
#define I2C0_USED_IO_SCL CONFIG_I2C0_USED_IO_SCL
#else
#define I2C0_USED_IO_SCL DEFAULT_I2C0_USED_IO_SCL
#endif

#ifdef CONFIG_I2C0_USED_IO_SDA
#define I2C0_USED_IO_SDA CONFIG_I2C0_USED_IO_SDA
#else
#define I2C0_USED_IO_SDA DEFAULT_I2C0_USED_IO_SDA
#endif

#if CSP_HAVE_I2C1
#define I2C1_TXD_BUFFER_SIZE 256
#define I2C1_RXD_BUFFER_SIZE 4096

#define DEFAULT_I2C1_USED_IO_SCL GPIO_NUM_1
#define DEFAULT_I2C1_USED_IO_SDA GPIO_NUM_2

#ifdef CONFIG_I2C1_USED_IO_SCL
#define I2C1_USED_IO_SCL CONFIG_I2C1_USED_IO_SCL
#else
#define I2C1_USED_IO_SCL DEFAULT_I2C1_USED_IO_SCL
#endif

#ifdef CONFIG_I2C1_USED_IO_SDA
#define I2C1_USED_IO_SDA CONFIG_I2C1_USED_IO_SDA
#else
#define I2C1_USED_IO_SDA DEFAULT_I2C1_USED_IO_SDA
#endif
#endif

#define DEFAULT_I2C0_INTR_FLAG 0

#ifdef CONFIG_I2C0_INTR_FLAG
#define I2C0_INTR_FLAG CONFIG_I2C0_INTR_FLAG
#else
#define I2C0_INTR_FLAG DEFAULT_I2C0_INTR_FLAG
#endif

#define DEFAULT_I2C1_INTR_FLAG 0

#ifdef CONFIG_I2C1_INTR_FLAG
#define I2C1_INTR_FLAG CONFIG_I2C1_INTR_FLAG
#else
#define I2C1_INTR_FLAG DEFAULT_I2C1_INTR_FLAG
#endif

/**
 * @brief i2c0主机初始化
 *
 * @return int 正确与否
 */
int i2c0MasterInit(void);

/**
 * @brief i2c0从机初始化
 *
 * @return int 正确与否
 */
int i2c0SlaveInit(uint8_t slaveAddr);

/**
 * @brief i2c0反初始化
 *
 * @return int 正确与否
 */
int i2c0MasterOrSlaveDeInit(void);

/**
 * @brief i2c命令文本执行
 *
 * @param cmd 已创建的i2c命令文本
 * @return int 正确与否
 */
int i2c0MasterRunCmd(i2c_cmd_handle_t cmd);

#if CSP_HAVE_I2C1
/**
 * @brief i2c1主机初始化
 *
 * @return int 正确与否
 */
int i2c1MasterInit(void);

/**
 * @brief i2c0从机初始化
 *
 * @return int 正确与否
 */
int i2c1SlaveInit(void);

/**
 * @brief i2c1反初始化
 *
 * @return int 正确与否
 */
int i2c1MasterOrSlaveDeInit(void);

/**
 * @brief i2c命令文本执行
 *
 * @param cmd 已创建的i2c命令文本
 * @return int 正确与否
 */
int i2c1MasterRunCmd(i2c_cmd_handle_t cmd);
#endif

/**
 * @brief i2c命令文本创建
 *
 * @param cmd 已创建的i2c命令文本
 * @return int 正确与否
 */
int i2cCreateCmd(i2c_cmd_handle_t* cmd);

/**
 * @brief i2c命令文本删除
 *
 * @param cmd 已创建的i2c命令文本
 * @return int 正确与否
 */
int i2cDeleteCmd(i2c_cmd_handle_t cmd);

/**
 * @brief i2c命令文本开启位
 *
 * @param cmd 已创建的i2c命令文本
 * @return int 正确与否
 */
int i2cCmdStart(i2c_cmd_handle_t cmd);

/**
 * @brief i2c命令文本结束位
 *
 * @param cmd 已创建的i2c命令文本
 * @return int 正确与否
 */
int i2cCmdStop(i2c_cmd_handle_t cmd);

/**
 * @brief 写入数据给具体设备
 *
 * @param cmd 已创建的i2c命令文本
 * @param data 设备地址(包括读写位)
 * @param dataSize 数据长度
 * @param ackFollow 尾随ack响应检查
 * @return int 正确与否
 */
int i2cCmdWriteData(i2c_cmd_handle_t cmd, uint8_t* data, int dataSize, bool ackFollow);

/**
 * @brief 读取具体设备的数据
 *
 * @param cmd 已创建的i2c命令文本
 * @param data 数据对应指针
 * @param dataSize 数据长度
 * @param ackFollow 结尾尾随ack响应
 * @return int 正确与否
 */
int i2cCmdReadData(i2c_cmd_handle_t cmd, uint8_t* data, int dataSize, bool ackFollow);

/**
 * @brief 读取设备中内容
 *
 * @param i2cNum i2c端口
 * @param devAddr 设备地址
 * @param data 存放数据数组
 * @param dataSize 数组长度
 * @return int 正确与否
 */
int i2cMasterReadDevice(int i2cNum, uint8_t devAddr, uint8_t* data, int dataSize);

/**
 * @brief 写入设备中内容
 *
 * @param i2cNum i2c端口
 * @param devAddr 设备地址
 * @param data 发送数据数组
 * @param dataSize 数组长度
 * @return int 正确与否
 */
int i2cMasterWriteDevice(int i2cNum, uint8_t devAddr, uint8_t* data, int dataSize);

/**
 * @brief 写入并读取设备中内容
 *
 * @param i2cNum i2c端口
 * @param devAddr 设备地址
 * @param wData 发送数据数组
 * @param wDataSize 发送数组长度
 * @param rData 存放数据数组
 * @param rDataSize 存放数组长度
 * @return int 正确与否
 */
int i2cMasterWriteReadDevice(int i2cNum, uint8_t devAddr, uint8_t* wData, int wDataSize, uint8_t* rData, int rDataSize);

/**
 * @brief 打印I2C总线上所有可用的硬件设备地址
 *
 * @param i2cNum 寻找i2c的端口号
 * @return int 正确与否
 */
int i2cMasterDetectSlaveAddress(int i2cNum);

/**
 * @brief i2c从机接收数据
 *
 * @param recvData 接收数据的空间指针
 * @param recvDataLen 接收数据的空间大小
 * @param isErrCleanCache 是否错误清理FIFO
 * @return int 正确与否
 */
int i2c0SlaveRecvData(uint8_t* recvData, size_t recvDataLen, bool isErrCleanCache);

/**
 * @brief i2c从机发送数据
 *
 * @param sendData 发送数据的空间指针
 * @param sendDataLen 发送数据的空间大小
 * @param isErrCleanCache 是否错误清理FIFO
 * @return int 正确与否
 */
int i2c0SlaveSendData(uint8_t* sendData, size_t sendDataLen, bool isErrCleanCache);

#endif  // __trans_i2c_h_
