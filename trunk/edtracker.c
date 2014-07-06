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
	//int16_t lcnt = 0;
	//int32_t asum[3];
	//asum[0] = 0;
	//asum[1] = 0;
	//asum[2] = 0;

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
				
			//if (++lcnt == 200)
			//{
			//	asum[0] = pckt.accel[0];
			//	asum[1] = pckt.accel[1];
			//	asum[2] = pckt.accel[2] - 16384;
			//	lcnt = 1;
			//	puts("---");
			//} else {
			//	asum[0] += pckt.accel[0];
			//	asum[1] += pckt.accel[1];
			//	asum[2] += pckt.accel[2] - 16384;
			//}
			
			/*if (dbgEmpty())
				printf("%6i %6i %6i - %6li %6li %6li\n", 
								pckt.gyro[0], pckt.gyro[1], pckt.gyro[2],
								//pckt.accel[0], pckt.accel[1], pckt.accel[2]);
								asum[0] / lcnt, asum[1] / lcnt, asum[2] / lcnt);
								//pckt.quat[0], pckt.quat[1], pckt.quat[2], pckt.quat[3]);
								*/

			LED_YELLOW = 0;
		}
	}
}


			/*
			if (dbgEmpty())
				printf("%04x %04x %04x - %04x %04x %04x - %04x %04x %04x %04x\n", 
								pckt.gyro[0], pckt.gyro[1], pckt.gyro[2],
								pckt.accel[0], pckt.accel[1], pckt.accel[2],
								pckt.quat[0], pckt.quat[1], pckt.quat[2], pckt.quat[3]);
			*/
			
			/*
			if (!more)
			{
				int32_t iX, iY, iZ;
				float newZ, newY, newX;
				float qw, qx, qy, qz;
				
				qw = (float)(quat[0] >> 16) / 16384.0f;
				qx = (float)(quat[1] >> 16) / 16384.0f;
				qy = (float)(quat[2] >> 16) / 16384.0f;
				qz = (float)(quat[3] >> 16) / 16384.0f;

				// Calculate Yaw/Pitch/Roll
				// Update client with yaw/pitch/roll and tilt-compensated magnetometer data

				// use some code to convert to R P Y
				newZ =  atan2(2.0 * (qy * qz + qw * qx), qw * qw - qx * qx - qy * qy + qz * qz);
				newY = -asin(-2.0 * (qx * qz - qw * qy));
				newX = -atan2(2.0 * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz);
			
				newX = newX * 10430.06;
				newY = newY * 10430.06;
				newZ = newZ * 10430.06;
				
				// clamp at 90 degrees left and right
				newX = constrain(newX, -16383.0, 16383.0);
				newY = constrain(newY, -16383.0, 16383.0);
				newZ = constrain(newZ, -16383.0, 16383.0);

				// and scale to out target range plus a 'sensitivity' factor;
				iX = newX * 4.0;
				iY = newY * 4.0;
				iZ = newZ * 4.0;

				if (dbgEmpty())
					printf("%04x %04x %04x\n", iX >> 16, iY >> 16, iZ >> 16);
			}
			*/
