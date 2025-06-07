#ifndef HW_DMA_H_
#define HW_DMA_H_

#include "bsp_usart.h"
#include "gd32c11x.h"

#ifdef USE_USART0
#define USART485_DMA_RCU             RCU_DMA0
#define USART485_DMA                 DMA0
#define USART485_DMA_TX_CHANNEL      DMA_CH3
#define USART485_DMA_RX_CHANNEL      DMA_CH4
#define USART485_DMA_TX_CHANNEL_IRQn DMA0_Channel3_IRQn
#endif

#ifdef USE_USART1
#define USART485_DMA_RCU             RCU_DMA0
#define USART485_DMA                 DMA0
#define USART485_DMA_TX_CHANNEL      DMA_CH6
#define USART485_DMA_RX_CHANNEL      DMA_CH5
#define USART485_DMA_TX_CHANNEL_IRQn DMA0_Channel6_IRQn
#endif

void USART485_DMA_Init(void);
void DMA_Transmit_En(uint16_t send_len);
void DMA_Receive_En(void);
void DMA_Receive_Dis(void);

FlagStatus get_dma_transfer_complete_flag(void);
void set_dma_transfer_complete_flag(void);
void clear_dma_transfer_complete_flag(void);
uint32_t usart485_dma_read_len(void);

#endif
