#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <reg24le1.h>
#include <nrfutils.h>
#include <nrfdbg.h>

#include "i2c.h"
#include "mpu.h"

int main(void)
{
	uint8_t ptr;
	
	dbgInit();
	puts("i live...");
	
	i2c_init();

	delay_ms(500);

	mpu_init();

	while (1)
	{
		delay_ms(200);
		i2c_read(12, 1, &ptr);
		
		delay_ms(300);
		i2c_write(12, 1, &ptr);
	}
}
