#ifndef MPU_H
#define MPU_H

void mpu_init(bool send_cal_gyro);
bool dmp_read_fifo(mpu_packet_t* pckt, uint8_t* more);

void mpu_calibrate_bias(void);

#endif	// MPU_H
