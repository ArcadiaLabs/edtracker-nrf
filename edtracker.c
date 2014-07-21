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
#include "settings.h"
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
	
	LED_YELLOW = 1;
	
	mpu_init(false);
	
	dbgFlush();
	
	rf_head_init();		// init the radio
	
	init_sleep();		// we need to wake up from RFIRQ

	LED_YELLOW = 0;

	dputs("init OK");
}

void test_bias(void)
{
	int32_t g[3];
	int32_t a[3];
	int16_t cnt = 0;
	uint8_t more;
	mpu_packet_t pckt;
	
	while (1)
	{
		if (cnt == 0)
		{
			g[0] = 0;
			g[1] = 0;
			g[2] = 0;

			a[0] = 0;
			a[1] = 0;
			a[2] = 0;
		}
		
		// wait for the interrupt
		while (MPU_IRQ)
			dbgPoll();
		while (!MPU_IRQ);

		dmp_read_fifo(&pckt, &more);
		
		a[0] += pckt.accel[0];
		a[1] += pckt.accel[1];
		a[2] += pckt.accel[2];

		g[0] += pckt.gyro[0];
		g[1] += pckt.gyro[1];
		g[2] += pckt.gyro[2];
		
		if (cnt == 200)
		{
			printf("%li   %li   %li\n", a[0] / cnt, a[1] / cnt, a[2] / cnt - 16384);
			cnt = 0;
		} else {
			++cnt;
		}
	}
}

#define LED_PCKT_TOTAL		150
#define LED_PCKT_LED_ON		2

int main(void)
{
	uint8_t more;
	uint8_t rf_pckt_ok = 0, rf_pckt_lost = 0;
	bool read_result;
	mpu_packet_t pckt;

	hw_init();
	
	for (;;)
	{
		// wait for the interrupt
		while (MPU_IRQ);
		while (!MPU_IRQ);
			
		do {
			// read all the packets in the MPU fifo
			do {
				read_result = dmp_read_fifo(&pckt, &more);
			} while (more);
			
			if (read_result)
			{
				pckt.flags = (RECENTER_BTN == 0 ? FLAG_RECENTER : 0);
				
				if (rf_head_send_message(&pckt, sizeof(pckt)))
					++rf_pckt_ok;
				else
					++rf_pckt_lost;

				// update the LEDs
				if (rf_pckt_lost + rf_pckt_ok == LED_PCKT_TOTAL)
				{
					if (rf_pckt_ok > rf_pckt_lost)
						LED_GREEN = 1;
					else
						LED_RED = 1;
						
				} else if (rf_pckt_lost + rf_pckt_ok == LED_PCKT_TOTAL + LED_PCKT_LED_ON) {
					LED_RED = 0;
					LED_GREEN = 0;

					rf_pckt_ok = rf_pckt_lost = 0;
				}
			}
			
		} while (more);
	}
}
