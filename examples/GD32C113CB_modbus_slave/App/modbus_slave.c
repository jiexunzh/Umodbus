#include "modbus_slave.h"
#include "modbus_port.h"
#include <string.h>

static ModbusSlave_TypeDef mbslave = {0};

uint8_t mbslave_init(uint8_t slave_addr, const ModbusReg_TypeDef* MB_REG, const uint16_t MB_DATA_NUM)
{
    /* 传参检查 */
    if (slave_addr < 1 || slave_addr > 247)
    {
        return FALSE;
    }

    mbslave.addr = slave_addr;
    mbslave.MB_REG = MB_REG;
    mbslave.mb_data_num = MB_DATA_NUM;

    /* 硬件初始化 */
    mbslave_hardware_init();

    return TRUE;
}

void mbslave_poll(void)
{
    switch (mbslave.sta)
    {
    case RECV_INIT:
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
        if (is_verify_ok(mbslave.addr, mbslave.recv_buf, mbslave.recv_len) == TRUE)
        {
            mbslave.sta = VERIFY;
        }
        else
        {
            mbslave.sta = RECV_INIT;
        }
        break;
    case ANALYSIS:
        /* 发送缓冲初始化 */
        memset(mbslave.send_buf, 0, mbslave.send_buf_len);
        /* 解析Modbus */
        mbslave.send_len = modbus_analysis(mbslave.MB_REG,
                                           mbslave.mb_data_num,
                                           mbslave.recv_buf,
                                           mbslave.recv_len,
                                           mbslave.send_buf);
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
            mbslave.sta = RECV_INIT;
        }
        break;
    default:
        break;
    }
}
