#include "modbus_slave.h"
#include "crc16.h"
#include "modbus_port.h"
#include "modbus_process.h"
#include <string.h>

static uint8_t modbus_verify(uint8_t slave_addr, uint8_t* recv_buf, uint8_t recv_len);
static void modbus_analysis(ModbusSlave_TypeDef* p_mbslave);

uint8_t mbslave_init(ModbusSlave_TypeDef *p_mbslave, 
                     uint8_t slave_addr,
                     const ModbusReg_TypeDef* REG_MAP_TABLE,
                     const uint16_t MAP_NUM,
                     uint8_t* recv_buf,
                     uint8_t* send_buf,
                     const uint16_t RECV_BUF_LEN,
                     const uint16_t SEND_BUF_LEN)
{
    /* 传参检查 */
    if (slave_addr < 1 || slave_addr > 247)
    {
        return FALSE;
    }

    p_mbslave->addr = slave_addr;
    p_mbslave->reg_map_table = REG_MAP_TABLE;
    p_mbslave->map_num = MAP_NUM;
    p_mbslave->recv_buf = recv_buf;
    p_mbslave->send_buf = send_buf;
    p_mbslave->recv_buf_len = RECV_BUF_LEN;
    p_mbslave->send_buf_len = SEND_BUF_LEN;

    return TRUE;
}

void mbslave_poll(ModbusSlave_TypeDef *p_mbslave)
{
    switch (p_mbslave->sta)
    {
    case RECV_ENABLE:
        /* 接收缓冲初始化 */
        memset(p_mbslave->recv_buf, 0, p_mbslave->recv_buf_len);
        /* 485接收使能 */
        rs485_recv_enable();
        p_mbslave->sta = RECVING;
        break;
    case RECVING:
        if (is_rs485_recv_finish(&p_mbslave->recv_len) == TRUE)
        {
            p_mbslave->sta = VERIFY;
        }
        break;
    case VERIFY:
        if (modbus_verify(p_mbslave->addr, p_mbslave->recv_buf, p_mbslave->recv_len) == TRUE)
        {
            p_mbslave->sta = ANALYSIS;
        }
        else
        {
            /* 校验不通过则不应答 */
            p_mbslave->sta = RECV_ENABLE;
        }
        break;
    case ANALYSIS:
        /* 发送缓冲初始化 */
        memset(p_mbslave->send_buf, 0, p_mbslave->send_buf_len);
        /* 解析Modbus */
        modbus_analysis(p_mbslave);
        p_mbslave->sta = SEND_ENABLE;
        break;
    case SEND_ENABLE:
        /* 485发送使能 */
        rs485_send_enable(p_mbslave->send_len);
        p_mbslave->sta = SENDING;
        break;
    case SENDING:
        if (is_rs485_send_finish() == TRUE)
        {
            p_mbslave->sta = RECV_ENABLE;
        }
        break;
    default:
        break;
    }
}

static uint8_t modbus_verify(uint8_t slave_addr, uint8_t* recv_buf, uint8_t recv_len)
{
    uint16_t crc16;
    uint8_t crc_l, crc_h;

    if (slave_addr != recv_buf[0])
    {
        return FALSE;
    }

    if (recv_len < 4)
    {
        return FALSE;
    }

    crc16 = crc16_modbus(recv_buf, recv_len - 2);
    crc_l = crc16 & 0xFF;        /* crc16 低八位 */
    crc_h = (crc16 >> 8) & 0xFF; /* crc16 高八位 */
    if (crc_l == recv_buf[recv_len - 2] && crc_h == recv_buf[recv_len - 1])
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void modbus_analysis(ModbusSlave_TypeDef* p_mbslave)
{
    switch (p_mbslave->recv_buf[1])
    {
    case 0x03:
        modbus_03H_process(p_mbslave);
        break;

    case 0x10:
        modbus_10H_process(p_mbslave);
        break;

    default:
        generate_exception_ack(p_mbslave, IllegalFunctionCode);
        break;
    }

    generate_crc_modbus(p_mbslave);
}
