#include "bsp_usart.h"
#include "bsp_dma.h" /* idleʹ�� */
#include "nvic_config.h"
#include <string.h>

static volatile FlagStatus idle_flag = RESET; /* IDLE��־λ */

uint8_t USART_RX_BUF[USART485_RX_BUF_SIZE] = {0}; /* 485���ڽ��ջ��� */
uint8_t USART_TX_BUF[USART485_TX_BUF_SIZE] = {0}; /* 485���ڷ��ͻ��� */

/**
 * @brief  485ʹ�����ų�ʼ��
 * @param
 * @retval
 */
void USART485_EN_init(void)
{
    rcu_periph_clock_enable(RS485_EN_RCU_GPIO);

    gpio_init(RS485_EN_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RS485_EN_Pin);

    RS485_RX_EN; /* Ĭ��Ϊ����ģʽ */
}

/**
 * @brief  GD32C113 485���ڳ�ʼ��
 * @param  bound_index ����������
 *  @arg 0-2400
 * 	@arg 1-4800
 * 	@arg 2-9600
 * 	@arg 3-19200
 * 	@arg 4-38400
 * 	@arg 5-57600
 * 	@arg 6-115200
 * 	@arg 7-230400
 * 	@arg 8-460800
 * @param  dataFormat_index ���ݸ�ʽ����
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
    usart_baudrate_set(USART485, bound_list[bound_index]);                     /* ������ */
    usart_word_length_set(USART485, format_list[dataFormat_index].wordLength); /* �����ֳ� */
    usart_parity_config(USART485, format_list[dataFormat_index].parity);       /* ��żУ��λ */
    usart_stop_bit_set(USART485, format_list[dataFormat_index].stopbits);      /* ֹͣλ */

    usart_receive_config(USART485, USART_RECEIVE_ENABLE);   /* ����ʹ�� */
    usart_transmit_config(USART485, USART_TRANSMIT_ENABLE); /* ����ʹ�� */

    /* ���ô��ڣ����У��ж����ȼ� */
    nvic_irq_enable(USART485_IRQn, USART485_IDLE_PRE_Priority, USART485_IDLE_SUB_Priority);

    /* ʹ�����߿����ж� */
    usart_interrupt_enable(USART485, USART_INT_IDLE);

    /* ����ʹ�� */
    usart_enable(USART485);

    /* ���IDLE���б�־����ֹ�ϵ缴�󴥷����� */
    usart_flag_clear(USART485, USART_FLAG_IDLE);
}

/**
 * @brief  ����1 ���봮�� ��Ϊ������ͨѶ��
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
    /* ����Ǵ��ڿ����ж� */
    if (RESET != usart_interrupt_flag_get(USART485, USART_INT_FLAG_IDLE))
    {
        /* �ر�DMA���գ���ֹ��û�д�����ý���֡����֮ǰDMA�ٽ������� */
        DMA_Receive_Dis();

        set_idle_flag();

        /* ��������жϱ�־λ */
        usart_interrupt_flag_clear(USART485, USART_INT_FLAG_IDLE);

        /* �ٶ�USART_DATA�ſ���� IDLE ��־λ */
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
