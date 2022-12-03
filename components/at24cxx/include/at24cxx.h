#include <iso646.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "logging.h"
#include "trans_i2c.h"

#ifndef __bsp_at24cxx_h_
#define __bsp_at24cxx_h_

#define AT24Cxx_BASE_ADDR 0b01010000

#define AT24Cxx_TIMEOUT 3000
#define AT24Cxx_BUSY_TIMEOUT 1000

/**
 * @brief 设置AT24Cxx锁方式
 * @details 1:ori_lock, 2:atm_lock, 3:smo_lock
 */
#define AT24Cxx_LOCK_METHOD 3

#define AT24Cxx_LOCK_ISLOCKED 1
#define AT24Cxx_LOCK_ISRELEASE 0

/**
 * @brief 设置AT24Cxx允许系统
 * @details 1:NONOS, 2:FreeRTOS, 3:ESP-IDF
 */
#define AT24Cxx_RUN_OS 3

typedef enum {
    AT24C01 = 1, /* 128 */
    AT24C02,     /* 256 */
    AT24C04,     /* 512 */
    AT24C08,     /* 1024 */
    AT24C16,     /* 2048 */
} AT24Cxx_ID_t;

typedef struct {
    AT24Cxx_ID_t id; /* 设备类型 */
    union {
        int ori_lock;               /* 裸机锁指针 */
        atomic_int atm_lock;        /* 原子锁指针 */
        SemaphoreHandle_t smo_lock; /* 互斥锁指针 */
    } lock;
    uint32_t capacity;  /* 容量大小(byte) */
    uint8_t page_count; /* 一个扇区页个数 */
    uint8_t page_size;  /* 每页字节数大小 */
    uint8_t i2c_addr;   /* 设备读写命令,这个意义在于选择块 */
} AT24Cxx_t;

/**
 * @brief 初始化AT24Cxx
 * @details 需要先设置AT24Cxx的类型
 * @warning 线程不安全
 *
 * @param at24cxx 指向at24cxx实体
 * @return int 正确与否
 */
int at24cxxInit(AT24Cxx_t* at24cxx);

/**
 * @brief 反初始化AT24Cxx
 * @param at24cxx at24cxx指针的指针
 * @warning 线程不安全
 *
 * @return int 正确与否
 */
int at24cxxDeInit(AT24Cxx_t* at24cxx);

/**
 * @brief 反初始化并释放AT24Cxx
 * @warning 线程不安全
 *
 * @param at24cxx at24cxx指针的指针
 * @return int 正确与否
 */
int at24cxxDeInitWithFree(AT24Cxx_t** at24cxx);

/**
 * @brief 重置AT24Cxx的数据
 * @warning 此函数运行不能初始化I2C，在其中会初始化GPIO并执行重置数据，执行完会反初始化GPIO
 *
 * @return int 正确与否
 */
int at24cxxResetSync(void);

/**
 * @brief 检查AT24Cxx是否忙碌
 *
 * @param at24cxx at24cxx指针
 * @return int 正确与否
 */
int at24cxxIsBusy(AT24Cxx_t* at24cxx);

/**
 * @brief 格式化AT24Cxx全部
 *
 * @param at24cxx at24cxx指针
 * @param content 每字节内容
 * @return int 正确与否
 */
int at24cxxEraseChip(AT24Cxx_t* at24cxx, uint8_t content);

/**
 * @brief 格式化AT24Cxx扇区
 *
 * @param at24cxx at24cxx指针
 * @param content 每字节内容
 * @return int 正确与否
 */
int at24cxxEraseBlock(AT24Cxx_t* at24cxx, uint8_t content);

/**
 * @brief 写AT24Cxx扇区
 * @warning 线程不安全
 *
 * @param at24cxx at24cxx指针
 * @param pageOffset 第几页
 * @param content 内容指针
 * @param contentLen 内容长度
 * @return int 正确与否
 */
int at24cxxWritePage(AT24Cxx_t* at24cxx, uint8_t pageOffset, uint8_t* content, uint8_t contentLen);

/**
 * @brief 位写入AT24Cxx
 * @warning 线程不安全
 *
 * @param at24cxx at24cxx指针
 * @param offset 数据位置
 * @param content 内容指针
 * @return int 正确与否
 */
int at24cxxWriteByte(AT24Cxx_t* at24cxx, uint8_t offset, uint8_t* content);

/**
 * @brief 按位读AT24Cxx
 * @warning 线程不安全
 *
 * @param at24cxx at24cxx指针
 * @param content 内容指针
 * @return int 正确与否
 */
int at24cxxReadByte(AT24Cxx_t* at24cxx, uint8_t* content);

/**
 * @brief 连续读AT24Cxx
 * @warning 线程不安全
 *
 * @param at24cxx at24cxx指针
 * @param content 内容指针
 * @param contentLen 内容长度
 * @return int 正确与否
 */
int at24cxxReadBytes(AT24Cxx_t* at24cxx, uint8_t* content, int contentLen);

/**
 * @brief 指定位置读AT24Cxx
 * @warning 线程不安全
 *
 * @param at24cxx  at24cxx指针
 * @param offset 内容指针
 * @param content 内容长度
 * @return int 正确与否
 */
int at24cxxReadCmdByte(AT24Cxx_t* at24cxx, uint8_t offset, uint8_t* content);

/**
 * @brief 指定位置连续读AT24Cxx
 * @warning 线程不安全
 *
 * @param at24cxx  at24cxx指针
 * @param offset 内容指针
 * @param content 内容长度
 * @param contentLen 内容长度
 * @return int 正确与否
 */
int at24cxxReadCmdBytes(AT24Cxx_t* at24cxx, uint8_t offset, uint8_t* content, int contentLen);

/**
 * @brief 打印所有数据
 *
 * @param at24cxx at24cxx指针
 * @return int 正确与否
 */
int at24cxxPrintAll(AT24Cxx_t* at24cxx);

#endif
