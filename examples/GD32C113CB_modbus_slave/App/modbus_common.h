
#ifndef MODBUS_COMMON_H
#define MODBUS_COMMON_H

#include <stdint.h>

#define TRUE  ((uint8_t)(1))
#define FALSE ((uint8_t)(0))

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

/* modbus异常码 */
typedef enum
{
    RSP_OK = 0,                 /*!< 成功，无异常 */
    IllegalFunctionCode = 0x01, /*!< 不支持的功能码 */
    IllegalRegAddr = 0x02,      /*!< 非法寄存器地址 */
    IllegalValue = 0x03,        /*!< 非法数据值：表示组合请求中剩余部分结构方面的错误，例如：隐含长度不正确 */
    IllegalWrite = 0x04,        /*!< 寄存器写入失败（不可写入） */
} ExceptionCode_TypeDef;

#endif /* MODBUS_COMMON_H */
