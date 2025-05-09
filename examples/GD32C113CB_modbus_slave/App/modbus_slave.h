#ifndef MODBUS_SLAVE_H
#define MODBUS_SLAVE_H

#include "modbus_common.h"

typedef enum
{
    RECV_INIT, /* 接收初始化 */
    RECVING,   /* 接收中 */
    VERIFY,    /* 校验 */
    ANALYSIS,  /* 解析 */
    SENDING,   /* 发送中 */
} SLAVE_STA;

typedef struct
{
    uint8_t addr;
    SLAVE_STA sta;
    uint16_t recv_len;
    uint16_t send_len;
    uint16_t recv_buf_len;
    uint16_t send_buf_len;
    uint16_t mb_data_num;
    uint8_t* recv_buf;
    uint8_t* send_buf;
    const ModbusReg_TypeDef* MB_REG;
} ModbusSlave_TypeDef;

#endif /* MODBUS_SLAVE_H */
