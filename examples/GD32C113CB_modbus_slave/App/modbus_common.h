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

#define TRUE               (1)
#define FALSE              (0)

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
RegRead_TypeDef read_modbus_reg(const ModbusReg_TypeDef* MB_REG,
                                const uint16_t MB_DATA_NUM,
                                uint16_t reg_addr,
                                uint16_t* p_read);
/* 写入指定地址的Modbus寄存器 */
RegWrite_TypeDef write_modbus_reg(const ModbusReg_TypeDef* MB_REG,
                                  const uint16_t MB_DATA_NUM,
                                  uint16_t reg_addr,
                                  uint16_t reg_value);

#endif /* MODBUS_COMMON_H */
