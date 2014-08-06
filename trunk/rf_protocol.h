#ifndef RF_PROTOCOL_H
#define RF_PROTOCOL_H

#define NRF_ADDR_SIZE	5

// the channel number is hard-coded for the moment
#define CHANNEL_NUM		115

extern __code const uint8_t HeadAddr[NRF_ADDR_SIZE];
extern __code const uint8_t DongleAddr[NRF_ADDR_SIZE];

// these message is sent to the USB dongle over the radio
enum mpu_packet_flags
{
	FLAG_RECENTER	= 0x01,
};

typedef struct
{
	uint8_t		flags;		// can be 0 or FLAG_RECENTER
	int16_t		gyro[3];
	int16_t		accel[3];
	int16_t		quat[4];
} mpu_packet_t;

typedef struct
{
	uint8_t		is_calibrated;
	int16_t		gyro_bias[3];
	int16_t		accel_bias[3];
} calib_data_t;

// these are command sent from the USB dongle to the head tracker using ACK payloads

enum head_tracker_commands_t
{
	CMD_CALIBRATE			= 1,
	CMD_SEND_CALIB_DATA		= 2,
};

#endif		// RF_PROTOCOL_H
