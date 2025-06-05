#include "modbus_port.h"
#include "bsp_dma.h"
#include "bsp_usart.h"

void rs485_recv_enable(void)
{
	RS485_RX_EN;
	DMA_Receive_En();
}

void rs485_send_enable(uint16_t send_len)
{
	RS485_TX_EN;
	DMA_Transmit_En(send_len);
}

uint8_t is_rs485_recv_finish(uint16_t* recv_len)
{
	uint8_t ret;
	
	ret = get_idle_flag();
	if (ret == SET)
	{
		clear_idle_flag();
		*recv_len = usart485_dma_read_len();
	}
	
    return ret;
}

uint8_t is_rs485_send_finish(void)
{
	uint8_t ret;
	
	ret = get_dma_transfer_complete_flag();
	
	if (ret == SET)
	{
		clear_dma_transfer_complete_flag();
		while(usart_flag_get(USART485, USART_FLAG_TC) == RESET);
	}
	
    return ret;
}
