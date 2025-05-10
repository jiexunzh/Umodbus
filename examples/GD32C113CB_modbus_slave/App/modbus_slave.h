#ifndef MODBUS_SLAVE_H
#define MODBUS_SLAVE_H

#include "modbus_common.h"

uint8_t mbslave_init(uint8_t slave_addr,
                     const ModbusReg_TypeDef* REG_MAP_TABLE,
                     const uint16_t MAP_NUM,
                     uint8_t* recv_buf,
                     uint8_t* send_buf,
                     const uint16_t RECV_BUF_LEN,
                     const uint16_t SEND_BUF_LEN,
                     pGetTickFunc get_tick_ms);
void mbslave_poll(void);

#endif /* MODBUS_SLAVE_H */
