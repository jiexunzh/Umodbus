#ifndef MODBUS_REGISTERS_H
#define MODBUS_REGISTERS_H

#include "modbus_common.h"

/* Modbus寄存器地址映射表 */
extern const ModbusReg_TypeDef MODBUS_REG_MAP[];
/* Modbus寄存器映射（数据）个数 */
extern const uint16_t MODBUS_MAP_NUM;

#endif /* MODBUS_REGISTERS_H */
