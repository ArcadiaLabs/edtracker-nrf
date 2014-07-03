#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "reg24lu1.h"

#include "keycode.h"
#include "reports.h"
#include "rf_protocol.h"
#include "rf_dngl.h"
#include "nrfutils.h"

#include "usb.h"
#include "usb_regs.h"
#include "hw_defs.h"

#include "nrfdbg.h"

#define RECV_BUFF_SIZE		32

float constrain(float val, float min, float max)
{
	if (val < min)
		return min;
		
	if (val > max)
		return max;
		
	return val;
}

void main()
{
	bool keyboard_report_ready = false;
	bool consumer_report_ready = false;

	uint8_t prev_keycode = KC_NO;
	
	__xdata uint8_t recv_buffer[RECV_BUFF_SIZE];
	__xdata uint8_t bytes_received;
	
	P0DIR = 0x00;	// all outputs
	P0ALT = 0x00;	// all GPIO default behavior
	
	LED_off();

	usbInit();
	dbgInit();
	
	rf_dngl_init();

	reset_keyboard_report();
	
	for (;;)
	{
		usbPoll();	// handles USB interrupts
		dbgPoll();	// send chars from the uart TX buffer
		
		// try to read the recv buffer
		bytes_received = rf_dngl_recv(recv_buffer, RECV_BUFF_SIZE);

		if (bytes_received  &&  dbgEmpty())
		{
			// make a text message from the received packet
			mpu_packet_t* pckt = (mpu_packet_t*) recv_buffer;
		
			int32_t iX, iY, iZ;
			float newZ, newY, newX;
			float qw, qx, qy, qz;
			
			qw = (float)(pckt->quat[0]) / 16384.0f;
			qx = (float)(pckt->quat[1]) / 16384.0f;
			qy = (float)(pckt->quat[2]) / 16384.0f;
			qz = (float)(pckt->quat[3]) / 16384.0f;

			// Calculate Yaw/Pitch/Roll
			// Update client with yaw/pitch/roll and tilt-compensated magnetometer data

			// use some code to convert to R P Y
			newZ =  atan2f(2.0 * (qy * qz + qw * qx), qw * qw - qx * qx - qy * qy + qz * qz);
			newY = -asinf(-2.0 * (qx * qz - qw * qy));
			newX = -atan2f(2.0 * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz);
		
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
		
			printf("%04x %04x %04x\n", iX, iY, iZ);
			
			/*
			printf("%04x %04x %04x - %04x %04x %04x - %04x %04x %04x %04x\n",
									pckt->gyro[0], pckt->gyro[1], pckt->gyro[2],
									pckt->accel[0], pckt->accel[1], pckt->accel[2],
									pckt->quat[0], pckt->quat[1], pckt->quat[2], pckt->quat[3]);
			*/
		}

		/*
		if (!keyboard_report_ready  &&  !msg_empty())
		{
			// get the next char from the stored text message
			uint8_t c = msg_peek();
			uint8_t new_keycode = get_keycode_for_char(c);

			reset_keyboard_report();

			// if the keycode is different than the previous
			// otherwise just send an empty report to simulate key went up
			if (new_keycode != prev_keycode  ||  new_keycode == KC_NO)
			{
				usb_keyboard_report.keys[0] = new_keycode;
				usb_keyboard_report.modifiers = get_modifiers_for_char(c);
				
				msg_pop();	// remove char from the buffer
			} else {
				new_keycode = KC_NO;
			}
			
			keyboard_report_ready = true;
			
			prev_keycode = new_keycode;		// remember for later
		}
		*/
		
		// send the report if the endpoint is not busy
		if ((in1cs & 0x02) == 0   &&   (keyboard_report_ready  ||  usbHasIdleElapsed()))
		{
			// copy the keyboard report into the endpoint buffer
			in1buf[0] = usb_keyboard_report.modifiers;
			in1buf[1] = 0;
			in1buf[2] = usb_keyboard_report.keys[0];
			in1buf[3] = usb_keyboard_report.keys[1];
			in1buf[4] = usb_keyboard_report.keys[2];
			in1buf[5] = usb_keyboard_report.keys[3];
			in1buf[6] = usb_keyboard_report.keys[4];
			in1buf[7] = usb_keyboard_report.keys[5];

			// send the data on it's way
			in1bc = 8;
			
			keyboard_report_ready = false;
		}

		// send the consumer report if the endpoint is not busy
		if ((in2cs & 0x02) == 0   &&   (consumer_report_ready  ||  usbHasIdleElapsed()))
		{
			in2buf[0] = usb_consumer_report;
			in2bc = 1;
		
			consumer_report_ready = false;
		}
	}
}
