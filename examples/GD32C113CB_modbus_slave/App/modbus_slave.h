#ifndef MODBUS_SLAVE_H
#define MODBUS_SLAVE_H

#include "modbus_common.h"

typedef enum
{
    RECV_ENABLE, /* 接收使能 */
    RECVING,     /* 接收中 */
    VERIFY,      /* 校验 */
    ANALYSIS,    /* 解析 */
    SEND_ENABLE, /* 发送使能 */
    SENDING,     /* 发送中 */
    SENT,        /* 发送完成 */
} SLAVE_STA;

typedef uint32_t (*pGetTickFunc)(void);

typedef struct
{
    uint8_t addr;
    SLAVE_STA sta;
    uint16_t recv_len;
    uint16_t send_len;
    uint16_t recv_buf_len;
    uint16_t send_buf_len;
    uint16_t map_num;
    uint8_t* recv_buf;
    uint8_t* send_buf;
    const ModbusReg_TypeDef* mb_reg_map;
    pGetTickFunc get_tick_ms;
} ModbusSlave_TypeDef;

#endif /* MODBUS_SLAVE_H */
