#ifndef MODBUS_PROCESS_H
#define MODBUS_PROCESS_H

#include "modbus_common.h"

#define MAX_READ_REG_NUM  125 /* 单次最大可读取寄存器个数 */
#define MAX_WRITE_REG_NUM 123 /* 单次最大可写入寄存器个数 */

void modbus_03H_process(ModbusSlave_TypeDef* p_mbslave);
void modbus_10H_process(ModbusSlave_TypeDef* p_mbslave);
void generate_exception_ack(ModbusSlave_TypeDef* p_mbslave, uint8_t exception_code);
void generate_crc_modbus(ModbusSlave_TypeDef* p_mbslave);

#endif /* MODBUS_PROCESS_H */
