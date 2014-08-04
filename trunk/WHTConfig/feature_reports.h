#pragma once

#ifdef _MSC_VER
# pragma pack(push)
# pragma pack(1)
#endif

// this is the data we're sending back and forth between the WHT device and this program

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define AXIS_CONFIG_REPORT_ID			2

// data direction: dongle <-> PC
typedef struct
{
	uint8_t		report_id;		// == AXIS_CONFIG_REPORT_ID

	uint8_t		is_selfcenter;
	uint8_t		is_linear;

	// linear factors
	float		lin_fact_x;
	float		lin_fact_y;
	float		lin_fact_z;

	// exponential factors
	float		exp_fact_x;
	float		exp_fact_y;
	float		exp_fact_z;
	
} FeatRep_AxisConfig;

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define CALIBRATE_REPORT_ID			3

// direction: PC -> dongle
typedef struct
{
	uint8_t		report_id;

	uint8_t		calibrate;
} FeatRep_Calibrate;

// *****************************************************************
// *****************************************************************
// *****************************************************************

// direction: PC -> dongle
typedef struct
{
	uint8_t		report_id;

	uint8_t		recenter;		// non-zero to recenter the dongle
} FeatRep_Recenter;

// direction: dongle -> PC
typedef struct
{
	uint8_t		report_id;

	uint8_t		is_calibrated;

	uint16_t	gyro_x;
	uint16_t	gyro_y;
	uint16_t	gyro_z;

	uint16_t	accel_x;
	uint16_t	accel_y;
	uint16_t	accel_z;
} FeatRep_CalibrationValues;

#ifdef _MSC_VER
# pragma pack(pop)
#endif
