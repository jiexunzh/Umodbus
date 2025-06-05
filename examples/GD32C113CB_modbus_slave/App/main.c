#include "bsp_dma.h"
#include "bsp_led.h"
#include "bsp_systick.h"
#include "bsp_usart.h"
#include "modbus_registers.h"
#include "modbus_slave.h"

#define MODBUS_SLAVE_ADDR    0x01
#define USART485_RX_BUF_SIZE 256
#define USART485_TX_BUF_SIZE 256

uint8_t USART_RX_BUF[USART485_RX_BUF_SIZE] = {0}; /* 485串口接收缓冲 */
uint8_t USART_TX_BUF[USART485_TX_BUF_SIZE] = {0}; /* 485串口发送缓冲 */

uint32_t led_tick = 0;
ModbusSlave_TypeDef mbslave = {0};

int main(void)
{
    SysTick_init();
    led_init();

    /* 串口初始化 */
    USART485_EN_init();  /* 485使能引脚 */
    USART485_DMA_Init(); /* 485串口DMA发送、接收、发送中断初始化 */
    USART485_init(6, 0); /* 485串口初始化 */
	
    mbslave_init(&mbslave,
                 MODBUS_SLAVE_ADDR,
                 MODBUS_REG_MAP,
                 MODBUS_MAP_NUM,
                 USART_RX_BUF,
                 USART_TX_BUF,
                 USART485_RX_BUF_SIZE,
                 USART485_TX_BUF_SIZE);

    while (1)
    {
        mbslave_poll(&mbslave);

        if (nonBlock(led_tick, 500))
        {
            led_tick = get_tick();
            LED_Toggle();
        }
    }
}
