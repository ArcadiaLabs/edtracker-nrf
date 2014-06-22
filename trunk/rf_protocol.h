#ifndef RF_PROTOCOL_H
#define RF_PROTOCOL_H

// this message is sent to the USB dongle over the radio
typedef struct
{
	int16_t		gyro[3];
	int16_t		accel[3];
	int16_t		quat[4];
} mpu_packet_t;

#endif	// RF_PROTOCOL_H
