#ifndef MODBUS_SLAVE_H
#define MODBUS_SLAVE_H

#include "modbus_registers.h"

#define TRUE  ((uint8_t)(1))
#define FALSE ((uint8_t)(0))

/* modbus异常码 */
typedef enum
{
    RSP_OK = 0,                 /*!< 成功，无异常 */
    IllegalFunctionCode = 0x01, /*!< 不支持的功能码 */
    IllegalRegAddr = 0x02,      /*!< 非法寄存器地址 */
    IllegalValue = 0x03,        /*!< 非法数据值：表示组合请求中剩余部分结构方面的错误，例如：隐含长度不正确 */
    IllegalWrite = 0x04,        /*!< 寄存器写入失败（不可写入） */
} ExceptionCode_TypeDef;

typedef enum
{
    RECV_ENABLE, /*!< 接收使能 */
    RECVING,     /*!< 接收中 */
    VERIFY,      /*!< 校验Modbus帧从站地址、CRC */
    ANALYSIS,    /*!< 解析 */
    SEND_ENABLE, /*!< 发送使能 */
    SENDING,     /*!< 发送中 */
} SlaveSta_TypeDef;

typedef struct
{
    uint8_t addr;
    const ModbusReg_TypeDef* reg_map_table;
    uint16_t map_num;
    uint8_t* recv_buf;
    uint8_t* send_buf;
    uint16_t recv_buf_len;
    uint16_t send_buf_len;    
    SlaveSta_TypeDef sta;
    uint16_t recv_len;
    uint16_t send_len;    
} ModbusSlave_TypeDef;

uint8_t mbslave_init(ModbusSlave_TypeDef *p_mbslave, 
                     uint8_t slave_addr,
                     const ModbusReg_TypeDef* REG_MAP_TABLE,
                     const uint16_t MAP_NUM,
                     uint8_t* recv_buf,
                     uint8_t* send_buf,
                     const uint16_t RECV_BUF_LEN,
                     const uint16_t SEND_BUF_LEN);                     
void mbslave_poll(ModbusSlave_TypeDef *p_mbslave);

#endif /* MODBUS_SLAVE_H */
