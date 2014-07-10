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
	P0DIR = 0b11110000;		// P0.0 P0.1 P0.2 are the LEDs and they are outputs
							// P0.3 is the UART TX - output
							// P0.5 is the push button - input
							// P0.6 is the MPU interrupt pin - input

	P0CON = 0b01010101;		// turn on the pullup for the recenter button
	
	LED_RED		= 0;	// LEDs are off
	LED_YELLOW	= 0;
	LED_GREEN	= 0;
	
	dbgInit();
	i2c_init();

	dputs("init started");
	
	LED_RED = 1;
	
	if (!mpu_init()  ||  !dmp_init())
		while (1);
	
	dbgFlush();
	
	rf_head_init();		// init the radio
	
	init_sleep();		// we need to wake up from RFIRQ

	LED_RED = 0;

	dputs("init OK");
}

int main(void)
{
	uint8_t more = 0;
	mpu_packet_t pckt;

	hw_init();

	for (;;)
	{
		sleep_mpuirq();
		
		pckt.flags = (RECENTER_BTN == 0 ? FLAG_RECENTER : 0);
		
		do {
			dmp_read_fifo(&pckt, &more);
			
			rf_head_send_message(&pckt, sizeof(pckt));
			//if (rf_head_send_message(&pckt, sizeof(pckt)))
			//{
			//	LED_GREEN = 1;
			//	LED_RED = 0;
			//} else {
			//	LED_GREEN = 0;
			//	LED_RED = 1;
			//}

		} while (more);
	}
}
