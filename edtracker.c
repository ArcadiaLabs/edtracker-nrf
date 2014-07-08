#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <reg24le1.h>
#include <nrfutils.h>
#include <nrfdbg.h>

#include "i2c.h"
#include "rf_protocol.h"
#include "mpu_simple.h"
#include "mpu_regs.h"
#include "sleeping.h"
#include "rf_head.h"
#include "edtracker.h"

void hw_init()
{
	P0DIR |= _BV(6);	// MPU interrupt input
	
	P0DIR &= 0x18;		// LEDs are outputs

	LED_RED		= 0;
	LED_YELLOW	= 0;
	LED_GREEN	= 0;
	
	dbgInit();
	i2c_init();

	LED_RED = 1;
	
	if (!mpu_init())
	{
		puts("mpu init FAILED!!!");
		
		while (1);
		
	} else {
		puts("mpu init ok");

		if (!dmp_init())
		{
			puts("dmp_init FAILED!!!");
			while (1);
		} else {
			puts("dmp_init ok");
		}
	}
	
	dbgFlush();
	
	rf_head_init();		// init the radio
	
	init_sleep();		// we need to wake up from RFIRQ

	LED_RED = 0;
}

int main(void)
{
	uint8_t more = 0;
	uint8_t fifo_cnt = 0;
	bool had_int = false;
	mpu_packet_t pckt;
	float f = HALF_PI;

	hw_init();

	for (;;)
	{
		if (P06 == 0)
		{
			// wait for P06 to rise
			while (P06 == 0)
				;
				
			had_int = true;
		} else {
			had_int = false;
		}
		
		dbgPoll();
		
		if (had_int  ||  more)
		{
			dmp_read_fifo(&pckt, &more);
			
			LED_YELLOW = 1;
			if (rf_head_send_message(&pckt, sizeof(pckt)))
			{
				LED_GREEN = 1;
				LED_RED = 0;
			} else {
				LED_GREEN = 0;
				LED_RED = 1;
			}
				
			LED_YELLOW = 0;
		}
	}
}
