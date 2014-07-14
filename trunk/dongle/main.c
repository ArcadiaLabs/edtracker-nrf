#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mymath.h"

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

float constrain_flt(float val)
{
	if (val < -16383.0)
		return -16384.0;
		
	if (val > 16383.0)
		return 16383.0;
		
	return val;
}

int32_t constrain_16bit(int32_t val)
{
	if (val < -32768)
		return -32768;
		
	if (val > 32767)
		return 32767;
		
	return val;
}

#define xExpScale	12.0
#define yExpScale	12.0
#define zExpScale	12.0

#define xScale		8.0
#define yScale		8.0
#define zScale		8.0

#define SELF_CENTERING
#define EXP_SCALE_MODE

int16_t driftSamples = -2;
float lastX = 0, dX = 0, dY, dZ;
float lX = 0.0;
float dzX = 0.0;
uint16_t ticksInZone = 0;
uint8_t recalibrateSamples = 200;
float cx, cy, cz = 0.0;
bool calibrated = false;
int16_t sampleCount = 0;
float xDriftComp = 0.0;
uint8_t pckt_cnt = 0;

bool process_packet(mpu_packet_t* pckt)
{
	float newZ, newY, newX;
	float qw, qx, qy, qz;
	float qww, qxx, qyy, qzz;
	int32_t iX, iY, iZ;
	
	qw = (float)(pckt->quat[0]) / 16384.0f;
	qx = (float)(pckt->quat[1]) / 16384.0f;
	qy = (float)(pckt->quat[2]) / 16384.0f;
	qz = (float)(pckt->quat[3]) / 16384.0f;

	// Calculate Yaw/Pitch/Roll

	qww = qw * qw;
	qxx = qx * qx;
	qyy = qy * qy;
	qzz = qz * qz;

	newZ =  atan2(2.0 * (qy * qz + qw * qx), qww - qxx - qyy + qzz);
	newY = -asin(-2.0 * (qx * qz - qw * qy));
	newX = -atan2(2.0 * (qx * qy + qw * qz), qww + qxx - qyy - qzz);

	//newZ =  atan2(2.0 * (qy * qz + qw * qx), qw * qw - qx * qx - qy * qy + qz * qz);
	//newY = -asin(-2.0 * (qx * qz - qw * qy));                                    
	//newX = -atan2(2.0 * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz);

	newX *= 10430.06;
	newY *= 10430.06;
	newZ *= 10430.06;

	if (!calibrated)
	{
		if (sampleCount < recalibrateSamples)
		{ // accumulate samples
			cx += newX;
			cy += newY;
			cz += newZ;
			++sampleCount;
		} else {
			calibrated = true;
			cx = cx / (float)sampleCount;
			cy = cy / (float)sampleCount;
			cz = cz / (float)sampleCount;

			dX = dY = dZ = 0.0;
			driftSamples = -2;
			recalibrateSamples = 100;// reduce calibrate next time around
		}

		return false;
	}

	// Have we been asked to recalibrate ?
	if (pckt->flags & FLAG_RECENTER)
	{
		sampleCount = 0;
		cx = cy = cz = 0;
		calibrated = false;

		return false;
	}

	// apply calibration offsets
	newX = newX - cx;

	// this should take us back to zero BUT we may have wrapped so ..
	if (newX < -32768.0)
		newX += 65536.0;

	if (newX > 32768.0)
		newX -= 65536.0;

	newY = newY - cy;
	newZ = newZ - cz;

	// clamp at 90 degrees left and right
	newX = constrain_flt(newX);
	newY = constrain_flt(newY);
	newZ = constrain_flt(newZ);

	// printf_fast_f("%6.0f %6.0f %6.0f\n", newX, newY, newZ);
	
#ifdef EXP_SCALE_MODE
	iX = (0.000122076 * newX * newX * xExpScale) * (newX / fabs(newX)); //side mount = yaw
	iY = (0.000122076 * newY * newY * yExpScale) * (newY / fabs(newY)); //side mount = pitch
	iZ = (0.000122076 * newZ * newZ * zExpScale) * (newZ / fabs(newZ)); //side mount = roll
#else
	iX = newX * xScale;
	iY = newY * yScale;
	iZ = newZ * zScale;
#endif

	// clamp after scaling to keep values within 16 bit range
	iX = constrain_16bit(iX);
	iY = constrain_16bit(iY);
	iZ = constrain_16bit(iZ);

	// Do it to it.
	usb_joystick_report.x = iX;
	usb_joystick_report.y = iY;
	usb_joystick_report.z = iZ;

	//self centering
	// if we're looking ahead, give or take
	//  and not moving
	//  and pitch is levelish then start to count
#ifdef SELF_CENTERING
	if (labs(iX) < 3000  &&  labs(iX - lX) < 5  &&  labs(iY) < 600)
	{
		ticksInZone++;
		dzX += iX;
	} else {
		ticksInZone = 0;
		dzX = 0.0;
	}
	lX = iX;

	// if we stayed looking ahead-ish long enough then adjust yaw offset
	if (ticksInZone >= 10)
	{
		// NB this currently causes a small but visible jump in the
		// view. Useful for debugging!
		dzX = dzX * 0.1;
		cx += dzX * 0.1;
		ticksInZone = 0;
		dzX = 0.0;
	}
#endif

	// Apply X axis drift compensation every 5th packet
	if (++pckt_cnt == 5)
	{
		cx = cx + xDriftComp;	// depending on your mounting

		if (cx > 65536.0)
			cx -= 65536.0;
		else if (cx < -65536.0)
			cx += 65536.0;

		pckt_cnt = 0;

		driftSamples++;

		if (driftSamples > 0)
			dX += newX - lastX;
		lastX = newX;
	}

	//if (dbgEmpty())
	//	printf_tiny("%d %d %d\n", usb_joystick_report.x, usb_joystick_report.y, usb_joystick_report.z);
		
	//if (dbgEmpty())
	//	printf("%04x %04x %04x\n", usb_joystick_report.x, usb_joystick_report.y, usb_joystick_report.z);
	//printf_fast_f("%d %f\n", driftSamples, dX / driftSamples);
	//printf_fast_f("%f\n", dX / driftSamples);
	
	return true;
}

void main(void)
{
	bool joystick_report_ready = false;
	mpu_packet_t packet;

	P0DIR = 0x00;	// all outputs
	P0ALT = 0x00;	// all GPIO default behavior
	
	LED_off();

	usbInit();
	dbgInit();
	
	dputs("\nlu1 online");

	rf_dngl_init();

	reset_joystick_report();
	
	for (;;)
	{
		usbPoll();	// handles USB interrupts
		dbgPoll();	// send chars from the uart TX buffer
		
		// try to read the recv buffer, then process the received data
		if (rf_dngl_recv(&packet, sizeof packet) == sizeof packet)
			joystick_report_ready = process_packet(&packet);

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
