#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <reg24le1.h>
#include <nrfutils.h>

#include "i2c.h"
#include "edtracker.h"
#include "mpu_regs.h"
#include "mpu_defines.h"
#include "mpu_dmp_firmware.h"
#include "rf_protocol.h"
#include "nrfdbg.h"

bool mpu_write_byte(uint8_t reg_addr, uint8_t val)
{
	return i2c_write(reg_addr, 1, &val);
}

/*
uint8_t mpu_read_byte(uint8_t reg_addr, uint8_t* val)
{
	uint8_t result;
	result = i2c_read(reg_addr, 1, val);
	return result ? 0 : 0xff;
}*/


#define FIFO_HZ		200

bool mpu_init(void)
{
	mpu_write_byte(PWR_MGMT_1, 0x80);		// reset
	delay_ms(100);
	mpu_write_byte(PWR_MGMT_1, 0);			// wakeup
	
	mpu_write_byte(GYRO_CONFIG, 0x18);
	mpu_write_byte(ACCEL_CONFIG, 0x00);
	mpu_write_byte(SMPLRT_DIV, 1000 / FIFO_HZ - 1);
	mpu_write_byte(CONFIG, INV_FILTER_98HZ);
	//mpu_write_byte(CONFIG, 0x03);
	//mpu_write_byte(INT_ENABLE, 0x00);
	mpu_write_byte(USER_CTRL, 0x20);
	mpu_write_byte(INT_PIN_CFG, 0x80);
	mpu_write_byte(PWR_MGMT_1, 0x40);
	mpu_write_byte(PWR_MGMT_2, 0x3F);
	delay_ms(50);
	mpu_write_byte(PWR_MGMT_1, 0x01);
	mpu_write_byte(PWR_MGMT_2, 0x00);
	delay_ms(50);
	//mpu_write_byte(INT_ENABLE, 0x01);
	//mpu_write_byte(INT_ENABLE, 0x00);
	mpu_write_byte(FIFO_EN, 0x00);		// disables all FIFO outputs
	mpu_write_byte(USER_CTRL, 0x00);
	mpu_write_byte(USER_CTRL, 0x04);	// reset FIFO
	mpu_write_byte(USER_CTRL, 0x40);	// enable FIFO
	delay_ms(50);
	//mpu_write_byte(INT_ENABLE, 0x00);
	mpu_write_byte(FIFO_EN, 0x78);
	//mpu_write_byte(SMPLRT_DIV, 0x04);
	//mpu_write_byte(CONFIG, INV_FILTER_20HZ);	// was 0x02

	return true;
}

bool mpu_write_mem(uint16_t mem_addr, uint16_t length, const uint8_t* data2write)
{
    uint8_t tmp[2];

    tmp[0] = (uint8_t)(mem_addr >> 8);
    tmp[1] = (uint8_t)(mem_addr & 0xFF);

    if (!i2c_write(BANK_SEL, 2, tmp))
        return false;
		
    if (!i2c_write(MEM_R_W, length, data2write))
        return false;

    return true;
}

bool mpu_read_mem(uint16_t mem_addr, uint16_t length, uint8_t* data2read)
{
    uint8_t tmp[2];

    tmp[0] = (uint8_t)(mem_addr >> 8);
    tmp[1] = (uint8_t)(mem_addr & 0xFF);

    if (!i2c_write(BANK_SEL, 2, tmp))
        return false;

    if (!i2c_read(MEM_R_W, length, data2read))
        return false;

    return true;
}

bool dmp_load_firmware(void)
{
#define LOAD_CHUNK  	16
#define START_ADDR		0x0400
#define MAX_RETRY		5

    uint16_t ii, this_write;
	
    uint8_t cur[LOAD_CHUNK], tmp[2];

	for (ii = 0; ii < DMP_CODE_SIZE; ii += this_write)
	{
        this_write = DMP_CODE_SIZE - ii;
		if (this_write > LOAD_CHUNK)
			this_write = LOAD_CHUNK;

		if (!mpu_write_mem(ii, this_write, dmp_memory + ii))
		{
			dputs("write failed");
			return false;
		}

		if (!mpu_read_mem(ii, this_write, cur))
		{
			dputs("read failed");
			return false;
		}

        if (memcmp(dmp_memory + ii, cur, this_write))
		{
			dputs("verify failed");
            return false;
		}
    }
	
    // Set program start address. 
    tmp[0] = START_ADDR >> 8;
    tmp[1] = START_ADDR & 0xFF;
    if (!i2c_write(PRGM_START_H, 2, tmp))
	{
		dputs("PRGM_START_H failed");
        return false;
	}

    return true;
}

bool dmp_set_orientation(void)
{
	const uint8_t __code arr1[3] = {0xCD, 0x4C, 0x6C};
	const uint8_t __code arr2[3] = {0xC9, 0x0C, 0x2C};
	const uint8_t __code arr3[3] = {0x36, 0x57, 0x76};
	const uint8_t __code arr4[3] = {0x26, 0x47, 0x66};

	return mpu_write_mem(FCFG_1, 3, arr1)
			&& mpu_write_mem(FCFG_2, 3, arr2)
			&& mpu_write_mem(FCFG_3, 3, arr3)
			&& mpu_write_mem(FCFG_7, 3, arr4);
}

void reset_fifo(void)
{
	mpu_write_byte(INT_ENABLE, 0x00);
	mpu_write_byte(FIFO_EN, 0x00);
	mpu_write_byte(USER_CTRL, 0x00);
	mpu_write_byte(USER_CTRL, 0x04);
	mpu_write_byte(USER_CTRL, 0x40);
	delay_ms(50);
	mpu_write_byte(INT_ENABLE, 0x02);	// DMP fifo enable
	mpu_write_byte(FIFO_EN, 0x78);		// enable gyro and accel FIFO
}

void mpu_set_gyro_bias_reg(int32_t* gyro_bias)
{
	uint8_t d[2], i;

	for (i = 0; i < 3; i++) 
	{
		d[0] = (gyro_bias[i] >> 8) & 0xff;
		d[1] = (gyro_bias[i]) & 0xff;
		i2c_write(0x13 + 2 * i, 2, d);
	}
}

/**
 *  @brief      Read biases to the accel bias 6050 registers.
 *  This function reads from the MPU6050 accel offset cancellations registers.
 *  The format are G in +-8G format. The register is initialized with OTP 
 *  factory trim values.
 *  @param[in]  accel_bias  returned structure with the accel bias
 *  @return     0 if successful.
 */
void mpu_read_6050_accel_bias(int32_t* accel_bias)
{
	uint8_t d[2], i;

	for (i = 0; i < 3; i++)
	{
		i2c_read(0x06 + i * 2, 2, d);
		accel_bias[i] = ((int32_t)d[0] << 8) | d[1];
	}
}

/**
 *  @brief      Push biases to the accel bias 6050 registers.
 *  This function expects biases relative to the current sensor output, and
 *  these biases will be added to the factory-supplied values. Bias inputs are LSB
 *  in +-8G format.
 *  @param[in]  accel_bias  New biases.
 */
void mpu_set_accel_bias_6050_reg(const int32_t* accel_bias, uint8_t relative)
{
	uint8_t d[2];
	int32_t accel_reg_bias[3];
	int32_t mask = 0x0001;
	uint8_t mask_bit[3] = {0, 0, 0};
	uint8_t i;

	mpu_read_6050_accel_bias(accel_reg_bias);

	// bit 0 of the 2 byte bias is for temp comp
	// calculations need to compensate for this and not change it
	for (i = 0; i < 3; i++) 
	{
		if (accel_reg_bias[i] & mask)
			mask_bit[i] = 0x01;

		if (relative == 1)
			accel_reg_bias[i] -= accel_bias[i];
		else	// just dump the value in
			accel_reg_bias[i] = accel_bias[i];

		d[0] = (accel_reg_bias[i] >> 8) & 0xff;
		d[1] = (accel_reg_bias[i]) & 0xff;
		d[1] = d[1] | mask_bit[i];
		i2c_write(0x06 + i * 2, 2, d);
	}
}

bool dmp_enable_feature(void)
{
	{
	const uint8_t __code arr[] = {0x02,0xCA,0xE3,0x09};
	mpu_write_mem(D_0_104, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0xA3,0xC0,0xC8,0xC2,0xC4,0xCC,0xC6,0xA3,0xA3,0xA3};
	mpu_write_mem(CFG_15, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0xD8};	// this influences the DMP fifo sample rate, just a guess...
	mpu_write_mem(CFG_27, sizeof arr, arr);
	}
	{	// dmp_enable_gyro_cal
	const uint8_t __code arr[] = {0xB8,0xAA,0xB3,0x8D,0xB4,0x98,0x0D,0x35,0x5D};
	mpu_write_mem(CFG_MOTION_BIAS, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0xB2,0x8B,0xB6,0x9B};
	mpu_write_mem(CFG_GYRO_RAW_DATA, sizeof arr, arr);
	}

	// disables TAP
	{
	const uint8_t __code arr[] = {0xD8};
	mpu_write_mem(CFG_20, sizeof arr, arr);
	}
	
	{
	const uint8_t __code arr[] = {0xD8};
	mpu_write_mem(CFG_ANDROID_ORIENT_INT, sizeof arr, arr);
	}
	{		// dmp_enable_lp_quat (disable)
	const uint8_t __code arr[] = {0x8B,0x8B,0x8B,0x8B};
	mpu_write_mem(CFG_LP_QUAT, sizeof arr, arr);
	}

	{		// dmp_enable_6x_lp_quat (enable)
	const uint8_t __code arr[] = {0x20,0x28,0x30,0x38};
	mpu_write_mem(CFG_8, sizeof arr, arr);
	}
	
	reset_fifo();

	// this is dmp_set_fifo_rate()
	{
	const uint8_t __code arr[] = {0x00,0x00};
	mpu_write_mem(D_0_22, sizeof arr, arr);
	}
	
	{
	const uint8_t __code arr[] = {0xFE,0xF2,0xAB,0xC4,0xAA,0xF1,0xDF,0xDF,0xBB,0xAF,0xDF,0xDF};
	mpu_write_mem(CFG_6, sizeof arr, arr);
	}

	return true;
}

void loadBiases(void)
{
	int32_t gBias[3] = {-78, -102, 65};
	int32_t aBias[3] = {65742, 132, -20};
	
	mpu_set_gyro_bias_reg(gBias);
	mpu_set_accel_bias_6050_reg(aBias, true);
}

bool dmp_init(void)
{
	if (!dmp_load_firmware())
	{
		dputs("dmp_load_firmware FAILED!!!");
		return false;
	}

	if (!dmp_set_orientation())
	{
		dputs("dmp_set_orientation FAILED!!!");
		return false;
	}

	if (!dmp_enable_feature())
	{
		dputs("dmp_enable_feature FAILED!!!");
		return false;
	}
	
	loadBiases();

	// enable DMP
	mpu_write_byte(INT_ENABLE, 0x00);
	//mpu_write_byte(SMPLRT_DIV, 0x04);
	mpu_write_byte(FIFO_EN, 0x00);
	mpu_write_byte(INT_ENABLE, 0x02);
	mpu_write_byte(INT_ENABLE, 0x00);
	mpu_write_byte(FIFO_EN, 0x00);
	mpu_write_byte(USER_CTRL, 0x00);
	mpu_write_byte(USER_CTRL, 0x0C);
	delay_ms(50);
	mpu_write_byte(USER_CTRL, 0xC0);
	mpu_write_byte(INT_ENABLE, 0x02);
	
	return true;
}

#define MAX_PACKET_LENGTH	28
//#define PACKET_LENGTH		28
#define MAX_FIFO			1024

bool mpu_read_fifo_stream(uint16_t length, uint8_t* data, uint8_t* more)
{
	uint8_t tmp[2];
	uint16_t fifo_count;

	// read number of bytes in the FIFO
	if (!i2c_read(FIFO_COUNT_H, 2, tmp))
		return false;

	fifo_count = (tmp[0] << 8) | tmp[1];
	
	if (fifo_count == 0)
	{
		*more = 0;
		return false;
	}

	// bytes in the fifo must be a multiple of packet length
	if (fifo_count % length)
	{
		reset_fifo();
		return false;
	}

	if (!i2c_read(FIFO_R_W, length, data))
		return false;

	*more = (fifo_count != length);
	
	return true;
}

bool dmp_read_fifo(mpu_packet_t* pckt, uint8_t* more)
{
    uint8_t fifo_data[MAX_PACKET_LENGTH];
    uint8_t ii = 0, i;

	if (!mpu_read_fifo_stream(MAX_PACKET_LENGTH, fifo_data, more))
		return false;

	pckt->quat[0] = (fifo_data[ 0] << 8) | fifo_data[ 1];
	pckt->quat[1] = (fifo_data[ 4] << 8) | fifo_data[ 5];
	pckt->quat[2] = (fifo_data[ 8] << 8) | fifo_data[ 9];
	pckt->quat[3] = (fifo_data[12] << 8) | fifo_data[13];
	
	/*
# define QUAT_ERROR_THRESH			(1L<<24)
# define QUAT_MAG_SQ_NORMALIZED		(1L<<28)
# define QUAT_MAG_SQ_MIN			(QUAT_MAG_SQ_NORMALIZED - QUAT_ERROR_THRESH)
# define QUAT_MAG_SQ_MAX			(QUAT_MAG_SQ_NORMALIZED + QUAT_ERROR_THRESH)

	{			
		int32_t quat_q14[4], quat_mag_sq;
		quat_q14[0] = pckt->quat[0];
		quat_q14[1] = pckt->quat[1];
		quat_q14[2] = pckt->quat[2];
		quat_q14[3] = pckt->quat[3];

		quat_mag_sq = quat_q14[0] * quat_q14[0] + quat_q14[1] * quat_q14[1] +
						quat_q14[2] * quat_q14[2] + quat_q14[3] * quat_q14[3];

		if (quat_mag_sq < QUAT_MAG_SQ_MIN)
		{
			putchar('L');
		} else if (quat_mag_sq > QUAT_MAG_SQ_MAX) {
			// Quaternion is outside of the acceptable threshold.
			putchar('G');
		} else {
			putchar(' ');
		}
	}
	*/
	
	ii += 16;
	
	for (i = 0; i < 3; i++)
		pckt->accel[i] = ((int16_t)fifo_data[ii+i*2] << 8) | fifo_data[ii+i*2+1];

	ii += 6;

	for (i = 0; i < 3; i++)
		pckt->gyro[i] = ((int16_t)fifo_data[ii+i*2] << 8) | fifo_data[ii+i*2+1];

    return true;
}

void msg(char* m, int32_t* v)
{
	m, v;
	//printf("%s %li   %li   %li\n", m, v[0], v[1], v[2]);
}

void update_bias(void)
{
	uint8_t more;
	uint16_t s16cnt;
	mpu_packet_t pckt;
	int32_t gBias[3], aBias[3], fBias[3];

	dputs("update_bias()...");
	
	mpu_read_6050_accel_bias(fBias);
	mpu_read_6050_accel_bias(aBias);
	
	aBias[0] = 65750;
	
	msg("faccel ", fBias);
	
	gBias[0] = 0;
	gBias[1] = 0;
	gBias[2] = 0;

	// set gyro to zero and accel to factory bias
	mpu_set_gyro_bias_reg(gBias);

	delay_ms(100);

	for (s16cnt = 0; s16cnt < 500; s16cnt++)
	{
		//physical values in Q16.16 format
		//mpu_get_biases(gBias, aBias);

		while (MPU_IRQ == 1)
			dbgPoll();
		while (MPU_IRQ == 0)
			;
		
		do {
			dmp_read_fifo(&pckt, &more);
		} while (more);

		if (pckt.accel[0] >= 1)
			aBias[0] += 1;
		else if (pckt.accel[0] <= -1)
			aBias[0] -= 1;

		if (pckt.accel[1] >= 1)
			aBias[1]++;
		else if (pckt.accel[1] <= -1)
			aBias[1]--;

		if (pckt.accel[2] > 16384)
			aBias[2]++;
		else if (pckt.accel[2] < 16384)
			aBias[2]--;

		if (pckt.gyro[0] > 1)
			gBias[0]--;
		else if (pckt.gyro[0] < -1)
			gBias[0]++;

		if (pckt.gyro[1] > 1)
			gBias[1]--;
		else if (pckt.gyro[1] < -1)
			gBias[1]++;

		if (pckt.gyro[2] > 1)
			gBias[2]--;
		else if (pckt.gyro[2] < -1)
			gBias[2]++;

		//msg("gyro ", gBias);
		//msg("accel ", aBias);

		// push the factory bias back
		mpu_set_accel_bias_6050_reg(fBias, 0);
		mpu_set_gyro_bias_reg(gBias);
		mpu_set_accel_bias_6050_reg(aBias, 1);
		
		// delay_ms(10);
	}

	msg("gyro ", gBias);
	msg("accel ", aBias);

	dbgFlush();
	
	//saveBias();
	//loadBiases();
}
