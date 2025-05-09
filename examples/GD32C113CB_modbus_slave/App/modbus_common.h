/**
 * *********************************************************************************
 * @author ZJX
 * @note 定义读写Modbus保持寄存器通用接口
 *
 * **********************************************************************************
 */

#ifndef MODBUS_COMMON_H
#define MODBUS_COMMON_H

#include <stdint.h>

typedef enum
{
    RegRead_SUCCESS = 0, /*!< 寄存器读取成功 */
    RegRead_ADDR_ERROR,  /*!< 寄存器地址错误（地址不存在） */
} RegRead_TypeDef;

typedef enum
{
    RegWrite_SUCCESS = 0,  /*!< 寄存器写入成功 */
    RegWrite_ADDR_ERROR,   /*!< 寄存器地址（地址不存在） */
    RegWrite_ACCESS_ERROR, /*!< 寄存器不允许写入 */
} RegWrite_TypeDef;

/* 读取指定地址的Modbus寄存器 */
RegRead_TypeDef read_modbus_reg(uint16_t reg_addr, uint16_t* p_read);
/* 写入指定地址的Modbus寄存器 */
RegWrite_TypeDef write_modbus_reg(uint16_t reg_addr, uint16_t reg_value);

#endif /* MODBUS_COMMON_H */
