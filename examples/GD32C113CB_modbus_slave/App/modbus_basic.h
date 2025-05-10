/**
 * *********************************************************************************
 * @author ZJX
 * @note 定义读写Modbus保持寄存器通用接口
 *       由于本协议栈目前只使用了Modbus保持寄存器，因此增加了写入限制，当属性为只读时，
 *       保持寄存器即相当于输入寄存器，但功能码依旧是03H / 10H
 *
 * **********************************************************************************
 */
#ifndef MODBUS_BASIC_H
#define MODBUS_BASIC_H

#include "modbus_common.h"

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
RegRead_TypeDef read_modbus_reg(ModbusSlave_TypeDef* p_mbslave,
                                uint16_t reg_addr,
                                uint16_t* p_read);
/* 写入指定地址的Modbus寄存器 */
RegWrite_TypeDef write_modbus_reg(ModbusSlave_TypeDef* p_mbslave,
                                  uint16_t reg_addr,
                                  uint16_t reg_value);

#endif /* MODBUS_BASIC_H */
