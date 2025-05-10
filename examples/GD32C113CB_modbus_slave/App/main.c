#include "bsp_systick.h"
#include "bsp_led.h"
#include "modbus_registers.h"
#include "modbus_slave.h"

int main(void)
{
	SysTick_init();
	led_init();

    while (1)
	{
		// mbslave_poll();
		// LED_Toggle();
		// delay_ms(2);
    }
}


