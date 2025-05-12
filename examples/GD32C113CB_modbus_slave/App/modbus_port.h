#ifndef MODBUS_PORT_H
#define MODBUS_PORT_H

#include <stdint.h>

void rs485_recv_enable(void);
uint8_t is_rs485_recv_finish(void);
uint16_t get_rs485_recv_len(void);
void rs485_send_enable(uint16_t send_len);
uint8_t is_rs485_send_finish(void);

#endif /* MODBUS_PORT_H */
