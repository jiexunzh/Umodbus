#ifndef MODBUS_PORT_H
#define MODBUS_PORT_H

#include <stdint.h>

void mbslave_hardware_init(void);
void set_485_recv_mode(void);
void set_485_send_mode(void);
uint8_t is_modbus_recv_finish(void);
void claer_recv_finish_flag(void);
uint16_t get_modbus_recv_len(void);
void modbus_send_enable(uint8_t* send_buf, uint16_t send_len);
uint8_t is_modbus_send_finish(void);
void claer_send_finish_flag(void);

#endif /* MODBUS_PORT_H */
