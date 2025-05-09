#ifndef MODBUS_SLAVE_H
#define MODBUS_SLAVE_H

#include <stdint.h>

typedef uint8_t (*GetIsRecvFinish_TypeDef)(void);
typedef uint32_t (*GetRecvLen_TypeDef)(void);
typedef void (*ModbusSend_TypeDef)(uint16_t sendlen);
typedef void (*VoidVoid_TypeDef)(void);

typedef struct
{
    uint8_t addr;
    uint8_t* p_recv_buf;
    uint8_t* p_send_buf;
    uint16_t recv_len;
    uint16_t send_len;
    GetIsRecvFinish_TypeDef is_recv_finish;
    GetRecvLen_TypeDef get_recv_len;
    VoidVoid_TypeDef send_init;
    VoidVoid_TypeDef recv_init;
    ModbusSend_TypeDef modbus_send;
} ModbusSlave_TypeDef;

#define TRUE  1
#define FALSE 0

#endif /* MODBUS_SLAVE_H */
