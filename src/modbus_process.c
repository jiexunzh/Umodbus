#include "modbus_process.h"
#include "modbus_basic.h"
#include "crc16.h"
#include "data_convert.h"

void modbus_03H_process(ModbusSlave_TypeDef* p_mbslave)
{
    uint16_t i;
    uint16_t reg_addr, reg_num;
    static uint16_t reg_value[MAX_READ_REG_NUM];
    RegRead_TypeDef ret;

    if (p_mbslave->recv_len != 8)
    {
        generate_exception_ack(p_mbslave, IllegalValue); /* 非法数据值：结构方面有错误 */
        return;
    }

    reg_addr = BE_bytes_to_u16(&p_mbslave->recv_buf[2]); /* 解析寄存器地址 */
    reg_num = BE_bytes_to_u16(&p_mbslave->recv_buf[4]);  /* 解析读取寄存器个数 */

    if (reg_num > MAX_READ_REG_NUM)
    {
        generate_exception_ack(p_mbslave, IllegalValue); /* 非法数据值：寄存器数量不正确 */
        return;
    }

    for (i = 0; i < reg_num; i++)
    {
        ret = read_modbus_reg(p_mbslave, reg_addr, &reg_value[i]);
        if (ret == RegRead_SUCCESS)
        {
            reg_addr++;
        }
        else if (ret == RegRead_ADDR_ERROR)
        {
            generate_exception_ack(p_mbslave, IllegalRegAddr); /* 非法寄存器地址 */
            return;
        }
        else     
        {
            return;
        }          
    }

    /* 成功，填充发送报文 */
    p_mbslave->send_len = 0;
    p_mbslave->send_buf[p_mbslave->send_len++] = p_mbslave->recv_buf[0];
    p_mbslave->send_buf[p_mbslave->send_len++] = p_mbslave->recv_buf[1];
    p_mbslave->send_buf[p_mbslave->send_len++] = reg_num * 2; /* 读取字节数 */

    for (i = 0; i < reg_num; i++)
    {
        /* 大端序 */
        p_mbslave->send_buf[p_mbslave->send_len++] = (reg_value[i] >> 8) & 0xFF;
        p_mbslave->send_buf[p_mbslave->send_len++] = reg_value[i] & 0xFF;
    }
}

void modbus_10H_process(ModbusSlave_TypeDef* p_mbslave)
{
    uint16_t i;
    uint16_t reg_addr, reg_num;
    uint8_t byte_num;
    uint16_t reg_value;
    RegWrite_TypeDef ret;   
    
    if (p_mbslave->recv_len < 11)
    {
        generate_exception_ack(p_mbslave, IllegalValue); /* 非法数据值：结构方面有错误 */
        return;
    }

    reg_addr = BE_bytes_to_u16(&p_mbslave->recv_buf[2]); /* 解析寄存器地址 */
    reg_num = BE_bytes_to_u16(&p_mbslave->recv_buf[4]);  /* 解析读取寄存器个数 */   

    if (reg_num > MAX_WRITE_REG_NUM)
    {
        generate_exception_ack(p_mbslave, IllegalValue); /* 非法数据值：寄存器数量不正确 */
        return;
    }

    byte_num = p_mbslave->recv_buf[6];
    if (byte_num != 2 * reg_num)
    {
        generate_exception_ack(p_mbslave, IllegalValue); /* 非法数据值：写入字节数有错误 */
        return;
    }  
    
    for (i = 0; i < reg_num; i++)
    {
        /* 解析modbus大端序的寄存器值 */
        reg_value = BE_bytes_to_u16(&p_mbslave->recv_buf[7 + 2 * i]);
        ret = write_modbus_reg(p_mbslave, reg_addr, reg_value);
        if (ret == RegWrite_SUCCESS)
        {
            reg_addr++;
        }
        else if (ret == RegWrite_ADDR_ERROR)
        {
            generate_exception_ack(p_mbslave, IllegalRegAddr); /* 非法寄存器地址 */
            return;
        }
        else if (ret == RegWrite_ACCESS_ERROR)
        {
            generate_exception_ack(p_mbslave, IllegalWrite); /* 寄存器不可写入 */
            return;
        }
        else     
        {
            return;
        }  
    }
    
    /* 成功，填充发送报文 */
    for (i = 0; i < 6; i++)
    {
        p_mbslave->send_buf[i] = p_mbslave->recv_buf[i];
    }
    p_mbslave->send_len = 6;
}

void generate_exception_ack(ModbusSlave_TypeDef* p_mbslave, uint8_t exception_code)
{
    p_mbslave->send_buf[0] = p_mbslave->recv_buf[0];
    p_mbslave->send_buf[1] = p_mbslave->recv_buf[1] | 0x80;
    p_mbslave->send_buf[2] = exception_code;
    p_mbslave->send_len = 3;
}

void generate_crc_modbus(ModbusSlave_TypeDef* p_mbslave)
{
    uint16_t crc16;
    uint8_t crc_l, crc_h;

    crc16 = crc16_calculate(p_mbslave->send_buf, p_mbslave->send_len);
    crc_l = crc16 & 0xFF;        /* crc16 低八位 */
    crc_h = (crc16 >> 8) & 0xFF; /* crc16 高八位 */

    p_mbslave->send_buf[p_mbslave->send_len] = crc_l;
    p_mbslave->send_buf[p_mbslave->send_len + 1] = crc_h;
    p_mbslave->send_len += 2;
}
