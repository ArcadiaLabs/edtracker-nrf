#ifndef MPU_REGS_H
#define MPU_REGS_H

// MPU-6050 registers excluding the I2C master and external sensors
#define SMPLRT_DIV			0x19		// 25
#define CONFIG				0x1A		// 26 AKA LPF
#define GYRO_CONFIG			0x1B		// 27
#define ACCEL_CONFIG		0x1C		// 28
#define FIFO_EN				0x23		// 35
#define INT_PIN_CFG			0x37		// 55
#define INT_ENABLE			0x38		// 56
#define INT_STATUS			0x3A		// 58
#define ACCEL_XOUT_H		0x3B		// 59
#define ACCEL_XOUT_L		0x3C		// 60
#define ACCEL_YOUT_H		0x3D		// 61
#define ACCEL_YOUT_L		0x3E		// 62
#define ACCEL_ZOUT_H		0x3F		// 63
#define ACCEL_ZOUT_L		0x40		// 64
#define TEMP_OUT_H			0x41		// 65
#define TEMP_OUT_L			0x42		// 66
#define GYOR_XOUT_H			0x43		// 67
#define GYOR_XOUT_L			0x44		// 68
#define GYOR_YOUT_H			0x45		// 69
#define GYOR_YOUT_L			0x46		// 70
#define GYOR_ZOUT_H			0x47		// 71
#define GYOR_ZOUT_L			0x48		// 72
#define SIGNAL_PATH_RESET	0x68		// 104
#define USER_CTRL			0x6A		// 106
#define PWR_MGMT_1			0x6B		// 107
#define PWR_MGMT_2			0x6C		// 108
#define FIFO_COUNT_H		0x72		// 114
#define FIFO_COUNT_L		0x73		// 115
#define FIFO_R_W			0x74		// 116
#define WHO_AM_I			0x75		// 117

// these are not mentioned in the register map PDF,
// but you can find them in the motion driver source
#define ACCEL_OFFS			0x06		// 6
#define BANK_SEL			0x6D		// 109
#define MEM_START_ADDR		0x6E		// 110
#define MEM_R_W				0x6F		// 111
#define PRGM_START_H		0x70		// 112
#define PRGM_START_L		0x71		// 113 this one's implied

#endif	// MPU_REGS_H