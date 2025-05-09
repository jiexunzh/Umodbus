#include "modbus_slave.h"
#include <string.h>

static ModbusSlave_TypeDef modbus_slave = {0};

void modbus_slave_init(void)
{
    /* 信息初始化 */

    /* Modbus接收初始化 */
}

void modbus_slave_poll(void)
{
    /* 接收到一帧完整的Modbus报文 */
    if (modbus_slave.is_recv_finish() == TRUE)
    {
        /* 获取Modbus报文长度 */
        modbus_slave.recv_len = modbus_slave.get_recv_len();

        /* 判断接收长度是否合法 */

        /* 判断从站地址 */

        /* CRC校验 */

        /* Modbus报文处理 */

        /* Modbus应答报文发送 */

        /* Modbus接收初始化 */
    }
}