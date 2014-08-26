#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <compiler_mcs51.h>

#include "rf_protocol.h"
#include "proc_packet.h"
#include "mymath.h"
#include "reports.h"
#include "dongle_settings.h"

// process_packet function processes the data from the sensor MPU-6050 attached to the player's head,
// and calculated xyz coordinates from the quaternions received.
//
// Almost the entire function is more-or-less copied from the ED Tracker project.
// 
// ED Tracker can be found here: http://edtracker.org.uk/

#define APPLY_DRIFT_COMP_PACKETS	5

int16_t driftSamples = -2;
float lastX = 0, dX = 0, dY, dZ;
float lX = 0.0;
float dzX = 0.0;
uint8_t ticksInZone = 0;
uint8_t recalibrateSamples = 200;
float cx, cy, cz = 0.0;
bool calibrated = false;
int16_t sampleCount = 0;
uint8_t pckt_cnt = 0;
bool pc_recenter = false;

void save_x_drift_comp(void)
{
	// get the current settings
	FeatRep_DongleSettings __xdata new_settings;
	memcpy(&new_settings, get_settings(), sizeof(FeatRep_DongleSettings));
	
	// set the new value
	new_settings.x_drift_comp += get_curr_x_drift_comp();
	
	save_settings(&new_settings);
}

float get_curr_x_drift_comp(void)
{
	if (driftSamples > 0)
		return dX / (float)driftSamples;
		
	return 0;
}

void recenter(void)
{
	pc_recenter = true;
}

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

bool process_packet(mpu_packet_t* pckt)
{
	float newZ, newY, newX;
	float qw, qx, qy, qz;
	float qww, qxx, qyy, qzz;
	int32_t iX, iY, iZ;
	
	const FeatRep_DongleSettings __xdata * pSettings = get_settings();
	
	qw = (float)(pckt->quat[0]) / 16384.0f;
	qx = (float)(pckt->quat[1]) / 16384.0f;
	qy = (float)(pckt->quat[2]) / 16384.0f;
	qz = (float)(pckt->quat[3]) / 16384.0f;

	// calculate Yaw/Pitch/Roll

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
		{
			// accumulate samples
			cx += newX;
			cy += newY;
			cz += newZ;
			++sampleCount;
		} else {
			calibrated = true;

			cx /= (float)sampleCount;
			cy /= (float)sampleCount;
			cz /= (float)sampleCount;

			dX = dY = dZ = 0.0;
			driftSamples = -2;
			recalibrateSamples = 100;	// reduce calibrate next time around
		}

		return false;
	}

	// has the user pressed the recenter button on the tracker?
	if ((pckt->flags & FLAG_RECENTER)  ||  pc_recenter)
	{
		sampleCount = 0;
		cx = cy = cz = 0.0;
		calibrated = false;

		pc_recenter = false;
		
		return false;
	}

	// apply calibration offsets
	newX = newX - cx;

	// this should take us back to zero BUT we may have wrapped so ..
	if (newX < -32768.0)
		newX += 65536.0;
	else if (newX > 32768.0)
		newX -= 65536.0;

	newY = newY - cy;
	newZ = newZ - cz;

	// clamp at 90 degrees left and right
	newX = constrain_flt(newX);
	newY = constrain_flt(newY);
	newZ = constrain_flt(newZ);

	// dprintf("%6.0f %6.0f %6.0f\n", newX, newY, newZ);
	
	if (pSettings->is_linear)
	{
		iX = newX * pSettings->lin_fact_x;
		iY = newY * pSettings->lin_fact_y;
		iZ = newZ * pSettings->lin_fact_z;
	} else {
		iX = (0.000122076 * newX * newX * pSettings->exp_fact_x) * (newX / fabs(newX)); //side mount = yaw
		iY = (0.000122076 * newY * newY * pSettings->exp_fact_y) * (newY / fabs(newY)); //side mount = pitch
		iZ = (0.000122076 * newZ * newZ * pSettings->exp_fact_z) * (newZ / fabs(newZ)); //side mount = roll
	}

	// clamp after scaling to keep values within 16 bit range
	iX = constrain_16bit(iX);
	iY = constrain_16bit(iY);
	iZ = constrain_16bit(iZ);

	// Do it to it.
	usb_joystick_report.x = iX;
	usb_joystick_report.y = iY;
	usb_joystick_report.z = iZ;

	// self centering
	if (pSettings->is_selfcenter)
	{
		// if we're looking ahead, give or take
		//  and not moving
		//  and pitch is levelish then start to count
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
			dzX *= 0.1;
			cx += dzX * 0.1;
			ticksInZone = 0;
			dzX = 0.0;
		}
	}

	// Apply X axis drift compensation every APPLY_DRIFT_COMP_PACKETS packets
	if (++pckt_cnt == APPLY_DRIFT_COMP_PACKETS)
	{
		cx += pSettings->x_drift_comp;

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

	return true;
}
