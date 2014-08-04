#ifndef RF_PROTOCOL_H
#define RF_PROTOCOL_H

#define NRF_ADDR_SIZE	5

// the channel number is hardcoded for the moment
#define CHANNEL_NUM		115

extern __code const uint8_t HeadAddr[NRF_ADDR_SIZE];
extern __code const uint8_t DongleAddr[NRF_ADDR_SIZE];

// this message is sent to the USB dongle over the radio
enum mpu_packet_flags
{
	FLAG_RECENTER	= 0x01,
};

typedef struct
{
	uint8_t		flags;
	int16_t		gyro[3];
	int16_t		accel[3];
	int16_t		quat[4];
} mpu_packet_t;


// these are command sent from the USB dongle over the radio

enum head_tracker_commands_t
{
	CMD_CALIBRATE		= 1,
};

#endif		// RF_PROTOCOL_H
