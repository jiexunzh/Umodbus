#include "modbus_slave.h"
#include "modbus_port.h"
#include <string.h>

static uint8_t modbus_verify(uint8_t slave_addr, uint8_t* recv_buf, uint8_t recv_len);
static uint16_t modbus_analysis(const ModbusReg_TypeDef* MB_REG_MAP,
                                const uint16_t MAP_NUM,
                                uint8_t* recv_buf,
                                uint8_t recv_len,
                                uint8_t* send_buf);

static ModbusSlave_TypeDef mbslave = {0};

uint8_t mbslave_init(uint8_t slave_addr,
                     const ModbusReg_TypeDef* MB_REG_MAP,
                     const uint16_t MAP_NUM,
                     uint8_t* recv_buf,
                     uint8_t* send_buf,
                     const uint16_t RECV_BUF_LEN,
                     const uint16_t SEND_BUF_LEN,
                     pGetTickFunc get_tick_ms)
{
    /* 传参检查 */
    if (slave_addr < 1 || slave_addr > 247)
    {
        return FALSE;
    }

    mbslave.addr = slave_addr;
    mbslave.mb_reg_map = MB_REG_MAP;
    mbslave.map_num = MAP_NUM;
    mbslave.recv_buf = recv_buf;
    mbslave.send_buf = send_buf;
    mbslave.recv_buf_len = RECV_BUF_LEN;
    mbslave.send_buf_len = SEND_BUF_LEN;
    mbslave.get_tick_ms = get_tick_ms;

    /* 硬件初始化 */
    mbslave_hardware_init();

    return TRUE;
}

void mbslave_poll(void)
{
    static uint32_t sent_tick; /* 记录发送完成时刻 */

    switch (mbslave.sta)
    {
    case RECV_ENABLE:
        /* 接收缓冲初始化 */
        memset(mbslave.recv_buf, 0, mbslave.recv_buf_len);
        /* 设置接收模式 */
        set_485_recv_mode();
        mbslave.sta = RECVING;
        break;
    case RECVING:
        if (is_modbus_recv_finish() == TRUE)
        {
            claer_recv_finish_flag();
            mbslave.sta = VERIFY;
        }
        break;
    case VERIFY: /* 校验Modbus帧基本结构、从站地址 */
        mbslave.recv_len = get_modbus_recv_len();
        if (modbus_verify(mbslave.addr, mbslave.recv_buf, mbslave.recv_len) == TRUE)
        {
            mbslave.sta = ANALYSIS;
        }
        else
        {
            mbslave.sta = RECV_ENABLE;
        }
        break;
    case ANALYSIS:
        /* 发送缓冲初始化 */
        memset(mbslave.send_buf, 0, mbslave.send_buf_len);
        /* 解析Modbus */
        mbslave.send_len = modbus_analysis(mbslave.mb_reg_map,
                                           mbslave.map_num,
                                           mbslave.recv_buf,
                                           mbslave.recv_len,
                                           mbslave.send_buf);
        mbslave.sta = SEND_ENABLE;
        break;
    case SEND_ENABLE:
        /* 设置发送模式 */
        set_485_send_mode();
        /* 使能发送 */
        modbus_send_enable(mbslave.send_buf, mbslave.send_buf_len);
        mbslave.sta = SENDING;
        break;
    case SENDING:
        if (is_modbus_send_finish() == TRUE)
        {
            claer_send_finish_flag();
            sent_tick = mbslave.get_tick_ms();
            mbslave.sta = SENT;
        }
    case SENT:
        /* 短暂延时，确保数据已全部发送出串口 */
        if ((mbslave.get_tick_ms() - sent_tick) >= 2)
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

    return 0;
}

static uint16_t modbus_analysis(const ModbusReg_TypeDef* MB_REG_MAP,
                                const uint16_t MAP_NUM,
                                uint8_t* recv_buf,
                                uint8_t recv_len,
                                uint8_t* send_buf)
{

    return 0;
}
