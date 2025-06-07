#ifndef BSP_USART_H
#define BSP_USART_H

#include "gd32c11x.h"

/* 根据实际情况选择USART0 / 1 和 485引脚 */
// #define USE_USART0
#define  USE_USART1

#ifdef USE_USART0
#define RS485_EN_RCU_GPIO  RCU_GPIOA
#define RS485_EN_GPIO_Port GPIOA
#define RS485_EN_Pin       GPIO_PIN_8

#define USART485_RCU_GPIO  RCU_GPIOA
#define USART485_RCU_USART RCU_USART0
#define USART485_TX_PORT   GPIOA
#define USART485_TX_PIN    GPIO_PIN_9
#define USART485_RX_PORT   GPIOA
#define USART485_RX_PIN    GPIO_PIN_10
#define USART485           USART0
#define USART485_IRQn      USART0_IRQn
#endif

#ifdef USE_USART1
#define RS485_EN_RCU_GPIO  RCU_GPIOA
#define RS485_EN_GPIO_Port GPIOA
#define RS485_EN_Pin       GPIO_PIN_1

#define USART485_RCU_GPIO  RCU_GPIOA
#define USART485_RCU_USART RCU_USART1
#define USART485_TX_PORT   GPIOA
#define USART485_TX_PIN    GPIO_PIN_2
#define USART485_RX_PORT   GPIOA
#define USART485_RX_PIN    GPIO_PIN_3
#define USART485           USART1
#define USART485_IRQn      USART1_IRQn
#endif

#define USART485_TX_BUF_SIZE  256
#define USART485_RX_BUF_SIZE  256

#define USART485_DATA_ADDRESS ((uint32_t) & USART_DATA(USART485))
#define RS485_TX_EN           gpio_bit_set(RS485_EN_GPIO_Port, RS485_EN_Pin)
#define RS485_RX_EN           gpio_bit_reset(RS485_EN_GPIO_Port, RS485_EN_Pin)

typedef struct
{
    uint32_t wordLength;
    uint32_t parity;
    uint32_t stopbits;
} dataFormatTypeDef;

void clear_USART_RX_BUF(void);
void USART485_EN_init(void);
void USART485_init(uint8_t bound_index, uint8_t dataFormat_index);
FlagStatus get_idle_flag(void);
void set_idle_flag(void);
void clear_idle_flag(void);
void clear_USART_TX_BUF(void);

#endif /* BSP_USART_H */
