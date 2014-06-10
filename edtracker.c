#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <reg24le1.h>
#include <nrfutils.h>
#include <nrfdbg.h>

#include "i2c.h"
#include "mpu_simple.h"
#include "mpu_regs.h"

void hw_init()
{
	P0DIR |= _BV(6);	// MPU interrupt input
	
	dbgInit();
	puts("\ni live...");

	i2c_init();
	
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
}

float constrain(float val, float min, float max)
{
	if (val < min)
		return min;
		
	if (val > max)
		return max;
		
	return val;
}

int main(void)
{
	int16_t gyro[3], accel[3];
	uint8_t more = 0;
	int32_t quat[4];
	bool had_int = false;
	
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
			dmp_read_fifo(gyro, accel, quat, &more);

			/*
			if (dbgEmpty())
				printf("%04x %04x %04x  %04x %04x %04x  %08lx %08lx %08lx %08lx\n", 
								gyro[0], gyro[1], gyro[2],
								accel[0], accel[1], accel[2],
								quat[0], quat[1], quat[2], quat[3]);
			*/
			
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

				/*
				if (dbgEmpty())
					printf("%04x %04x %04x\n", iX >> 16, iY >> 16, iZ >> 16);
					*/
			}
		}
	}
}
