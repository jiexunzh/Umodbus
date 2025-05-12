#include "bsp_systick.h"
#include "bsp_led.h"
#include "modbus_registers.h"
#include "modbus_slave.h"

#define MODBUS_SLAVE_ADDR 0x01
#define MODBUS_BUF_LEN    256

uint8_t modbus_rx_buf[MODBUS_BUF_LEN] = {0};
uint8_t modbus_tx_buf[MODBUS_BUF_LEN] = {0};

uint32_t modbus_tick, led_tick = 0;

int main(void)
{
    SysTick_init();
    led_init();
    /* 串口初始化 */

    mbslave_init(MODBUS_SLAVE_ADDR,
                 MODBUS_REG_MAP,
                 MODBUS_MAP_NUM,
                 modbus_rx_buf,
                 modbus_tx_buf,
                 MODBUS_BUF_LEN,
                 MODBUS_BUF_LEN);

    while (1)
    {
        if (nonBlock(modbus_tick, 5))
        {
            modbus_tick = get_tick();
            mbslave_poll();
        }
        if (nonBlock(led_tick, 500))
        {
            led_tick = get_tick();
            LED_Toggle();
        }
    }
}
