#pragma once

// MPU-6050 registers excluding the I2C master and external sensors
#define SMPLRT_DIV			25
#define CONFIG				26
#define GYRO_CONFIG			27
#define ACCEL_CONFIG		28
#define FIFO_EN				35
#define INT_PIN_CFG			55
#define INT_ENABLE			56
#define INT_STATUS			58
#define ACCEL_XOUT_H		59
#define ACCEL_XOUT_L		60
#define ACCEL_YOUT_H		61
#define ACCEL_YOUT_L		62
#define ACCEL_ZOUT_H		63
#define ACCEL_ZOUT_L		64
#define TEMP_OUT_H			65
#define TEMP_OUT_L			66
#define GYOR_XOUT_H			67
#define GYOR_XOUT_L			68
#define GYOR_YOUT_H			69
#define GYOR_YOUT_L			70
#define GYOR_ZOUT_H			71
#define GYOR_ZOUT_L			72
#define SIGNAL_PATH_RESET	104
#define USER_CTRL			106
#define PWR_MGMT_1			107
#define PWR_MGMT_2			108
#define FIFO_COUNTH			114
#define FIFO_COUNTL			115
#define FIFO_R_W			116
#define WHO_AM_I			117
