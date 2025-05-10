/**
 * *********************************************************************************
 * @author ZJX
 * @note 构建Modbus寄存器地址映射表
 *
 * **********************************************************************************
 */
#include "modbus_registers.h"

/* 供映射的内存区域 */
uint8_t u8MapMem[3];    // 8位数据类型
uint16_t u16MapMem[3];  // 16位数据类型
uint32_t u32MapMem[3];  // 32位数据类型
float fMapMem[3];       // 32位数据类型

/*  Modbus寄存器地址映射表：
    注意：32位数据需占用两个连续的寄存器，所以32位数据后的地址必须+2，
    暂时没有添加地址合法性检查代码，使用时需注意数据地址不要重合。
 */
const ModbusReg_TypeDef MODBUS_REG_MAP[] = {
    {0x0000, USE_ONE_REG_8BIT, READ_WRITE, &u8MapMem[0]}, /* 0x0000 */
    {0x0001, USE_ONE_REG_8BIT, READ_WRITE, &u8MapMem[1]}, /* 0x0001 */
    {0x0002, USE_ONE_REG_8BIT, ONLY_READ, &u8MapMem[2]},  /* 0x0002 */

    {0x0100, USE_ONE_REG_16BIT, READ_WRITE, &u16MapMem[0]},
    {0x0101, USE_ONE_REG_16BIT, READ_WRITE, &u16MapMem[1]},
    {0x0102, USE_ONE_REG_16BIT, ONLY_READ, &u16MapMem[2]},

    {0x0200, USE_TWO_REG_32BIT, READ_WRITE, &u32MapMem[0]}, /* 0x0200 ~ 0x0201 */
    {0x0202, USE_TWO_REG_32BIT, READ_WRITE, &u32MapMem[1]}, /* 0x0202 ~ 0x0203 */
    {0x0204, USE_TWO_REG_32BIT, ONLY_READ, &u32MapMem[2]},  /* 0x0204 ~ 0x0205 */

    {0x0300, USE_TWO_REG_32BIT, READ_WRITE, &fMapMem[0]},
    {0x0302, USE_TWO_REG_32BIT, READ_WRITE, &fMapMem[1]},
    {0x0304, USE_TWO_REG_32BIT, ONLY_READ, &fMapMem[2]},
};

/* Modbus寄存器映射（数据）个数 */
const uint16_t MODBUS_MAP_NUM = sizeof(MODBUS_REG_MAP) / sizeof(ModbusReg_TypeDef);
