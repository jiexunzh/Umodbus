#ifndef MODBUS_REGISTERS_H
#define MODBUS_REGISTERS_H

#include "modbus_common.h"

/* Modbus地址映射表 */
extern const ModbusReg_TypeDef MODBUS_REGISTERS[];
/* Modbus地址映射中的数据个数 */
extern const uint16_t MODBUS_DATA_NUM;

#endif /* MODBUS_REGISTERS_H */
