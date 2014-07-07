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

#define RECV_BUFF_SIZE	32

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
	bool joystick_report_ready = false;

	uint8_t prev_keycode = KC_NO;
	
	__xdata uint8_t recv_buffer[RECV_BUFF_SIZE];
	__xdata uint8_t bytes_received;
	
	P0DIR = 0x00;	// all outputs
	P0ALT = 0x00;	// all GPIO default behavior
	
	LED_off();

	usbInit();
	dbgInit();
	
	rf_dngl_init();

	reset_joystick_report();
	
	for (;;)
	{
		usbPoll();	// handles USB interrupts
		dbgPoll();	// send chars from the uart TX buffer
		
		// try to read the recv buffer
		bytes_received = rf_dngl_recv(recv_buffer, RECV_BUFF_SIZE);

		if (bytes_received)
		{
			// make a text message from the received packet
			mpu_packet_t* pckt = (mpu_packet_t*) recv_buffer;
		
			float newZ, newY, newX;
			float qw, qx, qy, qz;
			float qww, qxx, qyy, qzz;
			
			qw = (float)(pckt->quat[0]) / 16384.0f;
			qx = (float)(pckt->quat[1]) / 16384.0f;
			qy = (float)(pckt->quat[2]) / 16384.0f;
			qz = (float)(pckt->quat[3]) / 16384.0f;

			qww = qw * qw;
			qxx = qx * qx;
			qyy = qy * qy;
			qzz = qz * qz;
			
			// Calculate Yaw/Pitch/Roll
			// Update client with yaw/pitch/roll and tilt-compensated magnetometer data

			// use some code to convert to R P Y
			newZ =  atan2f(2.0 * (qy * qz + qw * qx), qww - qxx - qyy + qzz);
			newY = -asinf(-2.0 * (qx * qz - qw * qy));
			newX = -atan2f(2.0 * (qx * qy + qw * qz), qww + qxx - qyy - qzz);
		
			newX *= 10430.06;
			newY *= 10430.06;
			newZ *= 10430.06;
			
			// clamp at 90 degrees left and right
			newX = constrain(newX, -16383.0, 16383.0);
			newY = constrain(newY, -16383.0, 16383.0);
			newZ = constrain(newZ, -16383.0, 16383.0);

			// and scale to out target range plus a 'sensitivity' factor;
			usb_joystick_report.x = newX * 8.0;
			usb_joystick_report.y = newY * 8.0;
			usb_joystick_report.z = newZ * 8.0;
			
			//if (dbgEmpty())
			//	printf("%04x %04x %04x\n", usb_joystick_report.x, usb_joystick_report.y, usb_joystick_report.z);

			joystick_report_ready = true;
		}

		// send the report if the endpoint is not busy
		if ((in1cs & 0x02) == 0   &&   (joystick_report_ready  ||  usbHasIdleElapsed()))
		{
			// copy the joystick report into the endpoint buffer
			memcpy(in1buf, &usb_joystick_report, USB_EP1_SIZE);

			// send the data on it's way
			in1bc = USB_EP1_SIZE;
			
			joystick_report_ready = false;
		}
	}
}
