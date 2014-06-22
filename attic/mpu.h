#ifndef MPU_H
#define MPU_H

bool mpu_init(void);
bool dmp_init(void);

bool mpu_reset_fifo(void);
bool dmp_read_fifo(int16_t* gyro, int16_t* accel, int32_t* quat, uint8_t* more);

#endif	// MPU_H