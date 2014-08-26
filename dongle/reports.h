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

#define DONGLE_SETTINGS_REPORT_ID			2

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
	
	float		x_drift_comp;
} FeatRep_DongleSettings;

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define COMMAND_REPORT_ID			3

enum head_tracker_commands_t
{
	// the first two are sent to the head tracker over radio
	CMD_CALIBRATE			= 1,
	CMD_SEND_CALIB_DATA		= 2,
	
	// these are send from the PC to the dongle
	CMD_RECENTER			= 3,
	CMD_RESET_DRIFT			= 4,
	CMD_SAVE_DRIFT			= 5,
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

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define RF_STATUS_REPORT_ID				5

// direction: dongle -> PC
typedef struct
{
	uint8_t		report_id;		// RF_STATUS_REPORT_ID

	uint8_t		num_packets;	// number of packets received in the last second
} FeatRep_RFStatus;

#ifdef _MSC_VER
# pragma pack(pop)
#endif

#endif	// REPORTS_H