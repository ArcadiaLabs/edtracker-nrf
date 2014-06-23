#ifndef RF_PROTOCOL_H
#define RF_PROTOCOL_H

#define NRF_ADDR_SIZE	5

// the channel number is hardcoded for the moment
#define CHANNEL_NUM		115

extern const __code uint8_t HeadAddr[NRF_ADDR_SIZE];
extern const __code uint8_t DongleAddr[NRF_ADDR_SIZE];

// this message is sent to the USB dongle over the radio
typedef struct
{
	int16_t		gyro[3];
	int16_t		accel[3];
	int16_t		quat[4];
} mpu_packet_t;


#endif		// RF_PROTOCOL_H
