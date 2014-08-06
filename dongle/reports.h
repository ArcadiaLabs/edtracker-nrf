#ifndef REPORTS_H
#define REPORTS_H

#ifdef _MSC_VER
# pragma pack(push)
# pragma pack(1)
#endif



// This is the main HID joystick report.
// It contains the axis data we're sending to the PC.

#define JOYSTICK_REPORT_ID				1

typedef struct
{
	uint8_t	report_id;		// == JOYSTICK_REPORT_ID

	int16_t	x;
	int16_t	y;
	int16_t	z;

} hid_joystick_report_t;

extern hid_joystick_report_t	usb_joystick_report;	// the HID keyboard report

void reset_joystick_report(void);

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

#define COMMAND_REPORT_ID			3

enum head_tracker_commands_t
{
	CMD_CALIBRATE			= 1,
	CMD_SEND_CALIB_DATA		= 2,
	CMD_RECENTER			= 3,
};

// direction: PC -> dongle
typedef struct
{
	uint8_t		report_id;		// COMMAND_REPORT_ID
	uint8_t		command;
} FeatRep_Command;

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define CALIBRATION_DATA_REPORT_ID		4

// direction: dongle -> PC
typedef struct
{
	uint8_t		report_id;		// CALIBRATE_REPORT_ID

	uint8_t		has_tracker_responded;
	
	uint8_t		is_calibrated;

	int16_t		gyro_bias[3];
	int16_t		accel_bias[3];
} FeatRep_CalibrationData;



#ifdef _MSC_VER
# pragma pack(pop)
#endif

#endif	// REPORTS_H