#ifndef MODBUS_REGISTERS_H
#define MODBUS_REGISTERS_H

#include <stdint.h>

typedef enum
{
    USE_ONE_REG_8BIT = 0, /*!< 使用1个Modbus寄存器（8位数据） */
    USE_ONE_REG_16BIT,    /*!< 使用1个Modbus寄存器（16位数据） */
    USE_TWO_REG_32BIT,    /*!< 使用2个Modbus寄存器（32位数据） */
} RegNum_TypeDef;

typedef enum
{
    ONLY_READ = 0, /*!< 只读 */
    READ_WRITE,    /*!< 读写 */
} RegAccess_TypeDef;

typedef struct
{
    uint16_t start_addr;           /*!< 数据起始地址 */
    RegNum_TypeDef data_type;      /*!< 数据类型，占用的Modbus寄存器个数 */
    RegAccess_TypeDef access_type; /*!< 数据读写属性 */
    void* p_data;                  /*!< 数据指针 */
} ModbusReg_TypeDef;

/* Modbus寄存器地址映射表 */
extern const ModbusReg_TypeDef MODBUS_REG_MAP[];
/* Modbus寄存器映射（数据）个数 */
extern const uint16_t MODBUS_REG_MAP_NUM;

#endif /* MODBUS_REGISTERS_H */
