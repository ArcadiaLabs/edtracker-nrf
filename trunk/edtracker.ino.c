/* ============================================
EDTracker device code is placed under the MIT License

Copyright (c) 2014 Rob James, Dan Howell

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "reg24le1.h"
#include "nrfutils.h"
#include "nrf_shim.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

// uncomment this in to enable exponential scaling of head motion
// smaller movements near the centre, larger at the edges
//#define EXPONENTIAL

#ifdef EXPONENTIAL
float xScale = 20.0;
float yScale = 20.0;
float zScale = 20.0;
#else // standard linear response
float xScale = 4.0;
float yScale = 4.0;
float zScale = 4.0;
#endif

// variables used continual auto yaw compensation
float dzX = 0.0;
float lX = 0.0;
uint16_t ticksInZone = 0;
uint16_t reports = 0;

bool new_gyro, dmp_on;

//#define DEBUG

#ifdef DEBUG
# define DEBUG_PRINT(x)    Serial.print (x);
# define DEBUG_PRINTLN(x)  Serial.println (x);
#else
# define DEBUG_PRINT(x)
# define DEBUG_PRINTLN(x)
#endif

float xDriftComp = 0.0;

// EEPROM Offsets for config and calibration stuff
#define EE_VERSION 0
// these are now longs (4 bytes)
#define EE_XGYRO 1
#define EE_YGYRO 5
#define EE_ZGYRO 9
#define EE_XACCEL 13
#define EE_YACCEL 17
#define EE_ZACCEL 21
// 1 byte
#define EE_ORIENTATION 25
// 2 bytes
#define EE_XDRIFTCOMP 26

#define SDA_PIN 2
#define SCL_PIN 3

#define LED_PIN			P03
#define BUTTON_PIN		P11

// Starting sampling rate
#define DEFAULT_MPU_HZ    200

typedef enum {OMT_OFF, OMT_DBG, OMT_UI} outputModeType;

outputModeType outputMode = OMT_OFF;

float lastX, lastY, lastZ;
float dX, dY, dZ;
int16_t driftSamples = 0;

// packet structure for InvenSense teapot demo
uint32_t lastMillis;
uint32_t lastUpdate;

float cx, cy, cz = 0.0;

int32_t gBias[3], aBias[3];

// running count of samples - used when recalibrating
int16_t sampleCount = 0;
bool calibrated = false;

//Allows the MPU6050 to settle for 10 seconds.
//There should be no drift after this time
uint16_t calibrateTime = 10000;

//Number of samples to take when recalibrating
uint8_t recalibrateSamples =  200;

// Holds the time since sketch stared
uint32_t nowMillis;

// TrackState_t joySt;

// The mounting matrix below tells the MPL how to rotate the raw
// data from the driver(s).
uint8_t gyro_orients[4] =
{
	0b10001000,	// Z Up X Forward
	0b10000101,	// X right
	0b10101100,	// X Back
	0b10100001,	// X Left
}; // ZYX

uint8_t orientation = 1;

uint16_t millis(void)
{
	// TODO
	return 0;
}

float constrain_f(float val, float min, float max)
{
	if (val < min)
		return min;
	
	if (val > max)
		return max;
		
	return val;
}

int32_t constrain_i32(int32_t val, int32_t min, int32_t max)
{
	if (val < min)
		return min;
	
	if (val > max)
		return max;
		
	return val;
}

uint8_t constrain_u8(uint8_t val, uint8_t min, uint8_t max)
{
	if (val < min)
		return min;
	
	if (val > max)
		return max;
		
	return val;
}

void EEPROM_write(uint16_t addr, uint8_t val)
{
	addr, val;
}

uint8_t EEPROM_read(uint16_t addr)
{
	addr;
	return 0;
}

void writeIntEE(int address, int value)
{
	EEPROM_write(address + 1, value >> 8);	// upper byte
	EEPROM_write(address, value & 0xff);	// write lower byte
}

int readIntEE(int address)
{
	return EEPROM_read(address + 1) << 8 | EEPROM_read(address);
}

void writeLongEE(int address, long value)
{
	uint8_t i;
	for (i = 0; i < 4; i++)
	{
		EEPROM_write(address++, value & 0xff); // write lower byte
		value = value >> 8;
	}
}

long readLongEE(int address)
{
	return	(long)EEPROM_read(address + 3) << 24 |
			(long)EEPROM_read(address + 2) << 16 |
			(long)EEPROM_read(address + 1) << 8 |
			(long)EEPROM_read(address);
}

void blink(void)
{
	uint16_t delta = 100;

	if (calibrated)
		delta = 300;

	if (nowMillis > lastMillis + delta)
	{
		TogP(LED_PIN);
		lastMillis = nowMillis;
	}
}

void enable_mpu(void)
{
	// enable INT0
	//EICRB |= (1 << ISC60) | (1 << ISC61); // sets the interrupt type for EICRB (INT6)
	//EIMSK |= (1 << INT6); // activates the interrupt. 6 for 6, etc

	mpu_set_dmp_state(1);  // This enables the DMP; at this point, interrupts should commence
	dmp_on = 1;
}

void loadBiases(void)
{
	gBias[0] = readLongEE(EE_XGYRO);
	gBias[1] = readLongEE(EE_YGYRO);
	gBias[2] = readLongEE(EE_ZGYRO);

	aBias[0] = readLongEE(EE_XACCEL);
	aBias[1] = readLongEE(EE_YACCEL);
	aBias[2] = readLongEE(EE_ZACCEL);

	//dmp_set_gyro_bias(gBias); <- all sorts of undocumented shit
	//dmp_set_accel_bias(aBias);

	mpu_set_gyro_bias_reg(gBias);
	mpu_set_accel_bias_6050_reg(aBias, true);
}

void initialize_mpu()
{
	mpu_init();

	// Get/set hardware configuration. Start gyro.
	// Wake up all sensors.
	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);

	mpu_set_gyro_fsr(2000);
	mpu_set_accel_fsr(2);
	mpu_set_lpf(42);

	// Push both gyro and accel data into the FIFO.
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_set_sample_rate(DEFAULT_MPU_HZ);

	// To initialize the DMP:
	// 1. Call dmp_load_motion_driver_firmware()
	// 2. Push the gyro and accel orientation matrix to the DMP.
	// 4. Call dmp_enable_feature(mask) to enable different features.
	// 5. Call dmp_set_fifo_rate(freq) to select a DMP output rate.

	dmp_load_motion_driver_firmware();

	DEBUG_PRINTLN("Firmware Loaded ");

	dmp_set_orientation(gyro_orients[orientation]);
	//while (dmp_set_orientation( inv_orientation_matrix_to_scalar(gyro_orientation)))
	DEBUG_PRINTLN("orientation Loaded ");

	//dmp_register_tap_cb(&tap_cb);

	dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT
						| DMP_FEATURE_SEND_RAW_ACCEL
						| DMP_FEATURE_SEND_CAL_GYRO
						| DMP_FEATURE_GYRO_CAL);

	dmp_set_fifo_rate(DEFAULT_MPU_HZ);
}

void setup()
{
	//Serial.begin(115200);
	delay_ms(500);

#ifdef DEBUG
	outputMode = OMT_UI;
#endif

	//long l = readLongEE(0);
	//Serial.println(l);

	//pinMode(BUTTON_PIN, INPUT_PULLUP);
	//  pinMode(SDA_PIN, INPUT);
	//  pinMode(SCL_PIN, OUTPUT);
	//pinMode(LED_PIN, OUTPUT);

	// send a I2C stop signal
	//digitalWrite(SDA_PIN, HIGH);
	//digitalWrite(SDA_PIN, LOW);

	orientation = constrain_u8(EEPROM_read(EE_ORIENTATION), 0, 3);

	xDriftComp = (float)readIntEE(EE_XDRIFTCOMP) / 10000.0;

	// join I2C bus
	i2c_init();

	// Disable internal I2C pull-ups
	//  cbi(PORTC, 4);
	//  cbi(PORTC, 5);

	// Initialize the MPU:
	//
	// Gyro sensitivity:      2000 degrees/sec
	// Accel sensitivity:     2 g
	// Gyro Low-pass filter:  42Hz
	// DMP Update rate:       100Hz

	DEBUG_PRINTLN("M\tInitializing MPU... ");

	initialize_mpu();
	loadBiases();
	enable_mpu();

	DEBUG_PRINTLN("M\tInit Complete. Settling.");
}

/*****************************************
* Conversion Factors
*****************************************/

/****************************************
* Gyro/Accel/DMP State
****************************************/
uint32_t sensor_timestamp;

/****************************************
* Gyro/Accel/DMP Configuration
****************************************/

void recenter()
{
	if (outputMode == OMT_UI)
		puts("M\tRecentering");
		
	sampleCount = 0;
	cx = cy = cz = 0;
	calibrated = false;
}

void parseInput()
{
/*
	// read the incoming byte:
	uint8_t command = Serial.read();

	if (command == 'S')
	{
		outputMode = OMT_OFF;
		Serial.println("S"); // silent
		dmp_set_fifo_rate(DEFAULT_MPU_HZ);
	} else if (command == 'V') {
		Serial.println("V"); //verbose
		Serial.print("I\t");
		Serial.println(infoString);
		outputMode = OMT_UI;
		dmp_set_fifo_rate(DEFAULT_MPU_HZ/2);
	} else if (command == 'I') {
		Serial.print("I\t");
		Serial.println(infoString);
		Serial.println("M\t----------------");
		Serial.print("M\tOrientation ");
		Serial.println(orientation);

		Serial.print("M\tDrift Compensation");
		Serial.println(xDriftComp);

		Serial.print("M\tCurrent Drift Rate ");
		Serial.println((dX / (float)driftSamples));

		Serial.print("M\tGyro Bias ");
		Serial.print(gBias[0]); Serial.print(" / ");
		Serial.print(gBias[1]); Serial.print(" / ");
		Serial.println(gBias[2]);

		Serial.print("M\tAccel Bias ");
		Serial.print(aBias[0]); Serial.print(" / ");
		Serial.print(aBias[1]); Serial.print(" / ");
		Serial.println(aBias[2]);
	} else if (command == 'P') {
		mpu_set_dmp_state(0);
		orientation = (orientation + 1) % 4; //0 to 3
		dmp_set_orientation(gyro_orients[orientation]);
		mpu_set_dmp_state(1);
		Serial.print("M\tOrienation ");
		Serial.println(orientation);
		EEPROM.write(EE_ORIENTATION, orientation);
	} else if (command == 'R') {
		//recalibrate offsets
		recenter();
	} else if (command == 'D') {
		//Save Drift offset
		xDriftComp = (dX / (float)driftSamples) + xDriftComp;
		writeIntEE(EE_XDRIFTCOMP, (int)(xDriftComp * 10000.0));
		Serial.print("M\tSaved Drift Comp ");
		Serial.println(xDriftComp);
		Serial.print("R\t");
		Serial.println(xDriftComp);
	}
	
	while (Serial.available() > 0)
		command = Serial.read();
		*/
}

void loop()
{
	blink();
	nowMillis = millis();

	// If the MPU Interrupt occurred, read the fifo and process the data

	if (new_gyro && dmp_on)
	{
		int16_t gyro[3], accel[3], sensors;
		uint8_t more = 0;
		int32_t quat[4];
		sensor_timestamp = 1;

		dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);

		if (!more)
			new_gyro = 0;

		if (sensor_timestamp == 0)
		{
			short mag[3];
			uint32_t timestamp;
			int32_t iX, iY, iZ;
			float newZ, newY, newX;
			float qw, qx, qy, qz;
			
			qw = (float)(quat[0] >> 16) / 16384.0f;
			qx = (float)(quat[1] >> 16) / 16384.0f;
			qy = (float)(quat[2] >> 16) / 16384.0f;
			qz = (float)(quat[3] >> 16) / 16384.0f;

			// Calculate Yaw/Pitch/Roll
			// Update client with yaw/pitch/roll and tilt-compensated magnetometer data

			// Use some code to convert to R P Y
			newZ =  atan2f(2.0 * (qy * qz + qw * qx), qw * qw - qx * qx - qy * qy + qz * qz);
			newY = -asinf(-2.0 * (qx * qz - qw * qy));
			newX = -atan2f(2.0 * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz);

			// if we're still in the initial 'settling' period do nothing else...
			if (nowMillis < calibrateTime)
				return;

			// scale to range -32767 to 32767
			newX = newX * 10430.06;
			newY = newY * 10430.06;
			newZ = newZ * 10430.06;

			if (!calibrated)
			{
				if (sampleCount < recalibrateSamples)
				{
					cx += newX;		// accumulate samples
					cy += newY;
					cz += newZ;
					sampleCount++;
				} else {
					calibrated = true;
					cx = cx / (float)sampleCount;
					cy = cy / (float)sampleCount;
					cz = cz / (float)sampleCount;
					dX = dY = dZ = 0.0;
					driftSamples = -2;
					recalibrateSamples = 200;	// reduce calibrate next time around
					if (outputMode == OMT_UI)
					{
						//printf("I\t");
						//puts(infoString);
						//puts("M\tRecentered");
					}
				}

				return;
			}

			// Have we been asked to recalibrate ?
			if (BUTTON_PIN == 0)
			{
				recenter();
				return;
			}

			mpu_get_compass_reg(mag, &timestamp);

			// apply calibration offsets
			newX = newX - cx;
			newY = newY - cy;
			newZ = newZ - cz;

			// Before we mess with any of the DMP data log it to the UI if enabled
			if (outputMode == OMT_UI)
			{
				//printf("%5i\t%5i\t%5i\n", newX, newY, newZ);				// yaw, pitch, roll
				//printf("%5i\t%5i\t%5i\n", accel[0], accel[1], accel[2]);	// yaw, pitch, roll
				//printf("%5i\t%5i\t%5i\n", gyro[0], gyro[1], gyro[2]);		// yaw, pitch, roll
			}

			// clamp at 90 degrees left and right
			newX = constrain_f(newX, -16383.0, 16383.0);
			newY = constrain_f(newY, -16383.0, 16383.0);
			newZ = constrain_f(newZ, -16383.0, 16383.0);

#ifdef EXPONENTIAL
			iX = (0.0000304704 * newX * newX * xScale) * (newX / abs(newX));
			iY = (0.0000304704 * newY * newY * yScale) * (newY / abs(newY));
			iZ = (0.0000304704 * newZ * newZ * zScale) * (newZ / abs(newZ));
#else
			// and scale to out target range plus a 'sensitivity' factor;
			iX = newX * xScale;
			iY = newY * yScale;
			iZ = newZ * zScale;
#endif

			// clamp after scaling to keep values within 16 bit range
			iX = constrain_i32(iX, -32767, 32767);
			iY = constrain_i32(iY, -32767, 32767);
			iZ = constrain_i32(iZ, -32767, 32767);

			// Do it to it.
			//joySt.xAxis = iX;
			//joySt.yAxis = iY;
			//joySt.zAxis = iZ;
            //
			//Tracker.setState(&joySt);
			reports++;

			//self centering
			// if we're looking ahead, give or take
			//  and not moving
			//  and pitch is levelish then start to count
			if (outputMode != OMT_UI)
			{
				if (fabsf(iX) < 3000.0  &&  fabsf(iX - lX) < 5.0  &&  fabsf(iY) < 600)
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
					dzX = dzX / (float)10;
					cx += dzX * 0.1;
					ticksInZone = 0;
					dzX = 0.0;
				}
			}

			parseInput();

			// Apply X axis drift compensation every 1 second
			if (nowMillis > lastUpdate)
			{
				//depending on your mounting
				cx = cx + xDriftComp;
				//        cy = cy + yDriftComp;
				//        cz = cz + zDriftComp;

				lastUpdate = nowMillis + 1000;

				driftSamples++;

				if (driftSamples > 0)
				{
					dX += (newX - lastX);
					//        dY += (newY - lastY);
					//        dZ += (newZ - lastZ);
				}
				lastX = newX;
				// lastY = newY;
				// lastZ = newZ;

				/*
				DEBUG_PRINT("X/Y/Z\t");
				DEBUG_PRINT(newX  );
				DEBUG_PRINT("\t\t");
				DEBUG_PRINT(newY );
				DEBUG_PRINT("\t\t");
				DEBUG_PRINT(newZ );
				DEBUG_PRINT("\t\t");

				DEBUG_PRINTLN(dX / (float)driftSamples);
				if (outputMode == OMT_UI)
				{
					Serial.print("D\t");
					Serial.print(dX / (float)driftSamples);
					Serial.print("\t");
					Serial.println(xDriftComp);

					//          Serial.print("M\t Updates per second ");
					//          Serial.println(reports);
					reports = 0;
				}
				*/
				// DEBUG_PRINT("\t\t");
				// DEBUG_PRINT(dY / (float)driftSamples );
				// DEBUG_PRINT("\t\t");
				// DEBUG_PRINTLN(dZ / (float)driftSamples );
			}
		}
	}
}

// Every time new gyro data is available, this function is called in an
// ISR context. In this example, it sets a flag protecting the FIFO read
// function.
void gyro_data_ready_cb(void)
{
	new_gyro = 1;
}

void ISR_EXT_INT0(void) __interrupt INTERRUPT_IPF
{
	new_gyro = 1;
}

int main(void)
{
	setup();
	for (;;)
		loop();
}