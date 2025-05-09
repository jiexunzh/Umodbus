#include "bsp_usart.h"
#include "bsp_dma.h" /* idle使用 */
#include "nvic_config.h"
#include <string.h>

static volatile FlagStatus idle_flag = RESET; /* IDLE标志位 */

uint8_t USART_RX_BUF[USART485_RX_BUF_SIZE] = {0}; /* 485串口接收缓冲 */
uint8_t USART_TX_BUF[USART485_TX_BUF_SIZE] = {0}; /* 485串口发送缓冲 */

/**
 * @brief  485使能引脚初始化
 * @param
 * @retval
 */
void USART485_EN_init(void)
{
    rcu_periph_clock_enable(RS485_EN_RCU_GPIO);

    gpio_init(RS485_EN_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RS485_EN_Pin);

    RS485_RX_EN; /* 默认为接收模式 */
}

/**
 * @brief  GD32C113 485串口初始化
 * @param  bound_index 波特率索引
 *  @arg 0-2400
 * 	@arg 1-4800
 * 	@arg 2-9600
 * 	@arg 3-19200
 * 	@arg 4-38400
 * 	@arg 5-57600
 * 	@arg 6-115200
 * 	@arg 7-230400
 * 	@arg 8-460800
 * @param  dataFormat_index 数据格式索引
 * 	@arg 0-8N1
 * 	@arg 1-8N2
 * 	@arg 2-8E1
 * 	@arg 3-8E2
 * 	@arg 4-8O1
 * 	@arg 5-8O2
 * @retval None
 */
void USART485_init(uint8_t bound_index, uint8_t dataFormat_index)
{
    const uint32_t bound_list[] = {2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800};
    const dataFormatTypeDef format_list[] = {
        // n81
        USART_WL_8BIT, USART_PM_NONE, USART_STB_1BIT,
        // n82
        USART_WL_8BIT, USART_PM_NONE, USART_STB_2BIT,
        // e81
        USART_WL_8BIT, USART_PM_EVEN, USART_STB_1BIT,
        // e82
        USART_WL_8BIT, USART_PM_EVEN, USART_STB_2BIT,
        // o81
        USART_WL_8BIT, USART_PM_ODD, USART_STB_1BIT,
        // o82
        USART_WL_8BIT, USART_PM_ODD, USART_STB_2BIT};

    /* enable GPIO clock */
    rcu_periph_clock_enable(USART485_RCU_GPIO);

    /* enable USART clock */
    rcu_periph_clock_enable(USART485_RCU_USART);

    /* connect port to USARTx_Tx */
    gpio_init(USART485_TX_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, USART485_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_init(USART485_RX_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, USART485_RX_PIN);

    /* USART configure USART0 */
    usart_deinit(USART485);
    usart_baudrate_set(USART485, bound_list[bound_index]);                     /* 波特率 */
    usart_word_length_set(USART485, format_list[dataFormat_index].wordLength); /* 数据字长 */
    usart_parity_config(USART485, format_list[dataFormat_index].parity);       /* 奇偶校验位 */
    usart_stop_bit_set(USART485, format_list[dataFormat_index].stopbits);      /* 停止位 */

    usart_receive_config(USART485, USART_RECEIVE_ENABLE);   /* 接收使能 */
    usart_transmit_config(USART485, USART_TRANSMIT_ENABLE); /* 发送使能 */

    /* 设置串口（空闲）中断优先级 */
    nvic_irq_enable(USART485_IRQn, USART485_IDLE_PRE_Priority, USART485_IDLE_SUB_Priority);

    /* 使能总线空闲中断 */
    usart_interrupt_enable(USART485, USART_INT_IDLE);

    /* 串口使能 */
    usart_enable(USART485);

    /* 清除IDLE空闲标志，防止上电即误触发空闲 */
    usart_flag_clear(USART485, USART_FLAG_IDLE);
}

/**
 * @brief  串口1 隔离串口 作为传感器通讯口
 * @param	None
 * @retval None
 */
#ifdef USE_USART0
void USART0_IRQHandler(void)
#endif
#ifdef USE_USART1
    void USART1_IRQHandler(void)
#endif
{
    /* 如果是串口空闲中断 */
    if (RESET != usart_interrupt_flag_get(USART485, USART_INT_FLAG_IDLE))
    {
        /* 关闭DMA接收，防止在没有处理完该接收帧数据之前DMA再接收数据 */
        DMA_Receive_Dis();

        set_idle_flag();

        /* 清除空闲中断标志位 */
        usart_interrupt_flag_clear(USART485, USART_INT_FLAG_IDLE);

        /* 再读USART_DATA才可清除 IDLE 标志位 */
        usart_data_receive(USART485);
    }
}

void clear_USART_RX_BUF(void)
{
    memset(USART_RX_BUF, 0, sizeof(USART_RX_BUF));
}

void clear_USART_TX_BUF(void)
{
    memset(USART_TX_BUF, 0, sizeof(USART_TX_BUF));
}

FlagStatus get_idle_flag(void)
{
    return idle_flag;
}

void set_idle_flag(void)
{
    idle_flag = SET;
}

void clear_idle_flag(void)
{
    idle_flag = RESET;
}
