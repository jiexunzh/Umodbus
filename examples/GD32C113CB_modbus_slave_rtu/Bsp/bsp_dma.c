#include "bsp_dma.h"
#include "nvic_config.h"

#define USART485_RX_BUF_SIZE    256
#define USART485_TX_BUF_SIZE    256
extern uint8_t USART_TX_BUF[USART485_TX_BUF_SIZE];
extern uint8_t USART_RX_BUF[USART485_RX_BUF_SIZE];

static void DMA_TX_Init(void);
static void DMA_RX_Init(void);

static volatile FlagStatus dma_transfer_complete_flag = RESET; /* DMA������ɱ�־λ */

/**
 * @brief  DMA��ʼ��������DMA���ͺ�DMA����
 * @param
 * @retval
 */
void USART485_DMA_Init(void)
{
    /* enable DMA0 clock */
    rcu_periph_clock_enable(USART485_DMA_RCU);

    DMA_TX_Init();
    DMA_RX_Init();
}

/**
 * @brief  DMA�������ó�ʼ��
 * @param
 * @retval
 */
static void DMA_TX_Init(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize DMA0 channel3 dma�����ָ�ȱʡֵ�����ͣ� */
    dma_deinit(USART485_DMA, USART485_DMA_TX_CHANNEL);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;     // �ڴ浽����  TX
    dma_init_struct.memory_addr = (uint32_t)USART_TX_BUF;     // �ڴ�
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;  // �ڴ��ַ��������
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;     // ���ݿ��8
    // dma_init_struct.number = USART_TX_BUF_SIZE;            // �������ݸ���
    dma_init_struct.periph_addr = USART485_DATA_ADDRESS;       // ����
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;  // �����ַָ�벻����
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;  // ������
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;        // ���ȵȼ�  �������ȼ���  DMA_PRIORITY_HIGH �����ȼ�
    dma_init(USART485_DMA, USART485_DMA_TX_CHANNEL, &dma_init_struct);

    /* configure DMA mode */
    dma_memory_to_memory_disable(USART485_DMA, USART485_DMA_TX_CHANNEL);  // �ڴ浽�ڴ��ֹ
    dma_circulation_disable(USART485_DMA, USART485_DMA_TX_CHANNEL);       // ѭ��ģʽ��ֹ������ģʽ��

    /* ����DMA��������ж����ȼ� */
    nvic_irq_enable(USART485_DMA_TX_CHANNEL_IRQn, USART485_DMA_SEND_FINISH_PRE_Priority, USART485_DMA_SEND_FINISH_SUB_Priority); 
}

/**
 * @brief  DMA�������ó�ʼ��
 * @param
 * @retval
 */
static void DMA_RX_Init(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize DMA0 channel4 �����գ�*/
    dma_deinit(USART485_DMA, USART485_DMA_RX_CHANNEL);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;     // ���赽�ڴ�    RX
    dma_init_struct.memory_addr = (uint32_t)USART_RX_BUF;     // ���ջ�����
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;  // �ڴ�ָ���Զ�����
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = USART485_RX_BUF_SIZE;
    dma_init_struct.periph_addr = USART485_DATA_ADDRESS;  // ����
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;  // ���ȼ�  �������ȼ�
    dma_init(USART485_DMA, USART485_DMA_RX_CHANNEL, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(USART485_DMA, USART485_DMA_RX_CHANNEL);       // ѭ��ģʽ��ֹ������ģʽ��
    dma_memory_to_memory_disable(USART485_DMA, USART485_DMA_RX_CHANNEL);  // �ڴ浽�ڴ��ֹ
}

/**
 * @brief  ��DMA����
 * @param  send_len ���÷��ͳ���
 * @retval None
 */
void DMA_Transmit_En(uint16_t send_len)
{
    /* ����DMAͨ���������µĴ��� */
    dma_channel_disable(USART485_DMA, USART485_DMA_TX_CHANNEL);

    /* �����µĴ��������� */
    dma_transfer_number_config(USART485_DMA, USART485_DMA_TX_CHANNEL, send_len);

    /* ʹ�ܴ���DMA���� */
    usart_dma_transmit_config(USART485, USART_TRANSMIT_DMA_ENABLE);

    /* ʹ��DMA������ɣ�buf�������ж� */
    dma_interrupt_enable(USART485_DMA, USART485_DMA_TX_CHANNEL, DMA_INT_FTF);

    /* ʹ��DMA����ͨ�� */
    dma_channel_enable(USART485_DMA, USART485_DMA_TX_CHANNEL);
}

/**
 * @brief  ��dma���գ����ý������ݳ���Ϊ���ջ���������
 * @param
 * @retval
 */
void DMA_Receive_En(void)
{
    /* ����DMAͨ���������µĽ��� */
    dma_channel_disable(USART485_DMA, USART485_DMA_RX_CHANNEL);

    /* �������ý���������Ϊ��� */
    dma_transfer_number_config(USART485_DMA, USART485_DMA_RX_CHANNEL, USART485_RX_BUF_SIZE);

    /* ʹ�ܴ���DMA���� */
    usart_dma_receive_config(USART485, USART_RECEIVE_DMA_ENABLE);

    /* ʹ��DMA����ͨ�� */
    dma_channel_enable(USART485_DMA, USART485_DMA_RX_CHANNEL);
}

/**
 * @brief  �ر�dma���գ���ֹ�ڴ���Ĺ����������ݵ���������
 * @param
 * @retval
 */
void DMA_Receive_Dis(void)
{
    /* ʧ�ܴ���DMA���� */
    usart_dma_receive_config(USART485, USART_RECEIVE_DMA_DISABLE);

    /* ʧ��DMA����ͨ�� */
    dma_channel_disable(USART485_DMA, USART485_DMA_RX_CHANNEL);
}

FlagStatus get_dma_transfer_complete_flag(void)
{
    return dma_transfer_complete_flag;
}

void set_dma_transfer_complete_flag(void)
{
    dma_transfer_complete_flag = SET;
}

void clear_dma_transfer_complete_flag(void)
{
    dma_transfer_complete_flag = RESET;
}

/**
 * @brief  ��ȡDMA���յ������ݳ���
 * @param
 * @retval
 */
uint32_t usart485_dma_read_len(void)
{
    /*
    dma_transfer_number_get �ǻ�ȡ��ǰָ�����ֵ��
    ���ڴ滺������С - �˼���ֵ = ���յ������ݳ��ȣ����ﵥλΪ�ֽڣ���
    �ڶ�ȡ���ݳ��ȵ�ʱ����Ҫ�Ȱѽ���DMA�رգ���ȡ���˻��������ݴ��������ڴ򿪽���DMA��
    ��ֹ�ڴ���Ĺ����������ݵ���������
    */
    return USART485_RX_BUF_SIZE - dma_transfer_number_get(USART485_DMA, USART485_DMA_RX_CHANNEL);
}

/**
 * @brief  USART485_TX_DMAͨ���ж�
 * @param
 * @retval
 */
#ifdef USE_USART0
void DMA0_Channel3_IRQHandler(void)
#endif
#ifdef USE_USART1
void DMA0_Channel6_IRQHandler(void)
#endif
{
    /* �Ƿ���DMA��������ж� */
    if (SET == dma_interrupt_flag_get(USART485_DMA, USART485_DMA_TX_CHANNEL, DMA_INT_FLAG_FTF))
    {
        /* DMA������ɱ�־λ��λ */
        set_dma_transfer_complete_flag();

        /* ��� DMA ͨ��ȫ���жϱ�־λ״̬ */
        dma_interrupt_flag_clear(USART485_DMA, USART485_DMA_TX_CHANNEL, DMA_INT_FLAG_G);
    }
}
