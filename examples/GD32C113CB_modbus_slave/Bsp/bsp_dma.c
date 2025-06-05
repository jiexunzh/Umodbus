#include "bsp_dma.h"
#include "nvic_config.h"

#define USART485_RX_BUF_SIZE    256
#define USART485_TX_BUF_SIZE    256
extern uint8_t USART_TX_BUF[USART485_TX_BUF_SIZE];
extern uint8_t USART_RX_BUF[USART485_RX_BUF_SIZE];

static void DMA_TX_Init(void);
static void DMA_RX_Init(void);

static volatile FlagStatus dma_transfer_complete_flag = RESET; /* DMA发送完成标志位 */

/**
 * @brief  DMA初始化，包括DMA发送和DMA接收
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
 * @brief  DMA发送配置初始化
 * @param
 * @retval
 */
static void DMA_TX_Init(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize DMA0 channel3 dma参数恢复缺省值（发送） */
    dma_deinit(USART485_DMA, USART485_DMA_TX_CHANNEL);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;     // 内存到外设  TX
    dma_init_struct.memory_addr = (uint32_t)USART_TX_BUF;     // 内存
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;  // 内存地址增大增加
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;     // 数据宽度8
    // dma_init_struct.number = USART_TX_BUF_SIZE;            // 传输数据个数
    dma_init_struct.periph_addr = USART485_DATA_ADDRESS;       // 外设
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;  // 外设地址指针不递增
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;  // 外设宽度
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;        // 优先等级  超高优先级，  DMA_PRIORITY_HIGH 高优先级
    dma_init(USART485_DMA, USART485_DMA_TX_CHANNEL, &dma_init_struct);

    /* configure DMA mode */
    dma_memory_to_memory_disable(USART485_DMA, USART485_DMA_TX_CHANNEL);  // 内存到内存禁止
    dma_circulation_disable(USART485_DMA, USART485_DMA_TX_CHANNEL);       // 循环模式禁止（单次模式）

    /* 设置DMA发送完成中断优先级 */
    nvic_irq_enable(USART485_DMA_TX_CHANNEL_IRQn, USART485_DMA_SEND_FINISH_PRE_Priority, USART485_DMA_SEND_FINISH_SUB_Priority); 
}

/**
 * @brief  DMA接收配置初始化
 * @param
 * @retval
 */
static void DMA_RX_Init(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize DMA0 channel4 （接收）*/
    dma_deinit(USART485_DMA, USART485_DMA_RX_CHANNEL);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;     // 外设到内存    RX
    dma_init_struct.memory_addr = (uint32_t)USART_RX_BUF;     // 接收缓冲区
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;  // 内存指针自动增加
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = USART485_RX_BUF_SIZE;
    dma_init_struct.periph_addr = USART485_DATA_ADDRESS;  // 外设
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;  // 优先级  超高优先级
    dma_init(USART485_DMA, USART485_DMA_RX_CHANNEL, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(USART485_DMA, USART485_DMA_RX_CHANNEL);       // 循环模式禁止（单次模式）
    dma_memory_to_memory_disable(USART485_DMA, USART485_DMA_RX_CHANNEL);  // 内存到内存禁止
}

/**
 * @brief  打开DMA传输
 * @param  send_len 设置发送长度
 * @retval None
 */
void DMA_Transmit_En(uint16_t send_len)
{
    /* 禁用DMA通道以配置新的传输 */
    dma_channel_disable(USART485_DMA, USART485_DMA_TX_CHANNEL);

    /* 设置新的传输数据量 */
    dma_transfer_number_config(USART485_DMA, USART485_DMA_TX_CHANNEL, send_len);

    /* 使能串口DMA传输 */
    usart_dma_transmit_config(USART485, USART_TRANSMIT_DMA_ENABLE);

    /* 使能DMA传输完成（buf溢满）中断 */
    dma_interrupt_enable(USART485_DMA, USART485_DMA_TX_CHANNEL, DMA_INT_FTF);

    /* 使能DMA传输通道 */
    dma_channel_enable(USART485_DMA, USART485_DMA_TX_CHANNEL);
}

/**
 * @brief  打开dma接收，设置接收数据长度为接收缓冲区长度
 * @param
 * @retval
 */
void DMA_Receive_En(void)
{
    /* 禁用DMA通道以配置新的接收 */
    dma_channel_disable(USART485_DMA, USART485_DMA_RX_CHANNEL);

    /* 重新设置接收数据量为最大 */
    dma_transfer_number_config(USART485_DMA, USART485_DMA_RX_CHANNEL, USART485_RX_BUF_SIZE);

    /* 使能串口DMA接收 */
    usart_dma_receive_config(USART485, USART_RECEIVE_DMA_ENABLE);

    /* 使能DMA接收通道 */
    dma_channel_enable(USART485_DMA, USART485_DMA_RX_CHANNEL);
}

/**
 * @brief  关闭dma接收，防止在处理的过程中有数据到来而出错。
 * @param
 * @retval
 */
void DMA_Receive_Dis(void)
{
    /* 失能串口DMA接收 */
    usart_dma_receive_config(USART485, USART_RECEIVE_DMA_DISABLE);

    /* 失能DMA接收通道 */
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
 * @brief  获取DMA接收到的数据长度
 * @param
 * @retval
 */
uint32_t usart485_dma_read_len(void)
{
    /*
    dma_transfer_number_get 是获取当前指针计数值，
    用内存缓冲区大小 - 此计数值 = 接收到的数据长度（这里单位为字节）。
    在读取数据长度的时候需要先把接收DMA关闭，读取完了或者是数据处理完了在打开接收DMA，
    防止在处理的过程中有数据到来而出错。
    */
    return USART485_RX_BUF_SIZE - dma_transfer_number_get(USART485_DMA, USART485_DMA_RX_CHANNEL);
}

/**
 * @brief  USART485_TX_DMA通道中断
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
    /* 是否是DMA传输完成中断 */
    if (SET == dma_interrupt_flag_get(USART485_DMA, USART485_DMA_TX_CHANNEL, DMA_INT_FLAG_FTF))
    {
        /* DMA发送完成标志位置位 */
        set_dma_transfer_complete_flag();

        /* 清除 DMA 通道全局中断标志位状态 */
        dma_interrupt_flag_clear(USART485_DMA, USART485_DMA_TX_CHANNEL, DMA_INT_FLAG_G);
    }
}
