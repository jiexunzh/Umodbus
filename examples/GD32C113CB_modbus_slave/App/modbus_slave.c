#include "modbus_slave.h"
#include "crc16.h"
#include "modbus_port.h"
#include "modbus_process.h"
#include <string.h>

static uint8_t modbus_verify(uint8_t slave_addr, uint8_t* recv_buf, uint8_t recv_len);
static void modbus_analysis(ModbusSlave_TypeDef* p_mbslave);

static ModbusSlave_TypeDef mbslave = {0};

uint8_t mbslave_init(uint8_t slave_addr,
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

    mbslave.addr = slave_addr;
    mbslave.reg_map_table = REG_MAP_TABLE;
    mbslave.map_num = MAP_NUM;
    mbslave.recv_buf = recv_buf;
    mbslave.send_buf = send_buf;
    mbslave.recv_buf_len = RECV_BUF_LEN;
    mbslave.send_buf_len = SEND_BUF_LEN;

    return TRUE;
}

void mbslave_poll(void)
{
    switch (mbslave.sta)
    {
    case RECV_ENABLE:
        /* 接收缓冲初始化 */
        memset(mbslave.recv_buf, 0, mbslave.recv_buf_len);
        /* 485接收使能 */
        rs485_recv_enable();
        mbslave.sta = RECVING;
        break;
    case RECVING:
        if (is_rs485_recv_finish() == TRUE)
        {
            /* 获取RS485数据接收长度 */
            mbslave.recv_len = get_rs485_recv_len();
            mbslave.sta = VERIFY;
        }
        break;
    case VERIFY:
        if (modbus_verify(mbslave.addr, mbslave.recv_buf, mbslave.recv_len) == TRUE)
        {
            mbslave.sta = ANALYSIS;
        }
        else
        {
            /* 校验不通过则不应答 */
            mbslave.sta = RECV_ENABLE;
        }
        break;
    case ANALYSIS:
        /* 发送缓冲初始化 */
        memset(mbslave.send_buf, 0, mbslave.send_buf_len);
        /* 解析Modbus */
        modbus_analysis(&mbslave);
        mbslave.sta = SEND_ENABLE;
        break;
    case SEND_ENABLE:
        /* 485发送使能 */
        rs485_send_enable(mbslave.send_len);
        mbslave.sta = SENDING;
        break;
    case SENDING:
        if (is_rs485_send_finish() == TRUE)
        {
            mbslave.sta = RECV_ENABLE;
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
