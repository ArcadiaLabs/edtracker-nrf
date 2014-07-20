#ifndef MPU_H
#define MPU_H

bool mpu_init(void);
bool dmp_init(void);

bool dmp_read_fifo(mpu_packet_t* pckt, uint8_t* more);

#endif	// MPU_H
