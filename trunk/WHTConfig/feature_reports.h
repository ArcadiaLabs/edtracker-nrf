#pragma once

// this is the data we're sending back and forth between the WHT device and this program

struct FeatureReport_Recenter
{
	uint8_t		report_id;

	uint8_t		recenter;		// non-zero if the dongle needs to recenter
};

struct FeatureReport_Autocenter
{
	uint8_t		report_id;

	uint8_t		autocenter;		// non-zero if auto-venter should be anbled
};

struct FeatureReport_Calibrate
{
	uint8_t		report_id;

	uint8_t		calibrate;
};

struct FeatureReport_CalibrationValues
{
	uint8_t		report_id;

	uint8_t		is_calibrated;

	uint16_t	gyro_x;
	uint16_t	gyro_y;
	uint16_t	gyro_z;

	uint16_t	accel_x;
	uint16_t	accel_y;
	uint16_t	accel_z;
};

struct FeatureReport_AxisMode
{
	uint8_t		report_id;

	uint8_t		is_linear;

	// Factors are divided by 10 on the dongle.
	// So, a factor value of 144 is actually 14.4
	// This is to save some bytes of bandwidth

	// linear factors
	uint8_t		lin_fact_x;
	uint8_t		lin_fact_y;
	uint8_t		lin_fact_z;

	// exponential factors
	uint8_t		lin_fact_x;
	uint8_t		lin_fact_y;
	uint8_t		lin_fact_z;
};
