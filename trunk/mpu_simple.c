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
#include "settings.h"
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
	mpu_write_byte(INT_ENABLE, 0x01);	// fifo enable
	mpu_write_byte(FIFO_EN, 0x78);		// enable gyro and accel FIFO
}

void mpu_set_gyro_bias(const int16_t* gyro_bias)
{
	uint8_t d[2], i;

	for (i = 0; i < 3; i++)
	{
		d[0] = (gyro_bias[i] >> 8) & 0xff;
		d[1] = (gyro_bias[i]) & 0xff;
		i2c_write(0x13 + 2 * i, 2, d);
	}
}

void mpu_read_accel_bias(int16_t* accel_bias)
{
	uint8_t d[2], i;

	for (i = 0; i < 3; i++)
	{
		i2c_read(0x06 + i * 2, 2, d);
		accel_bias[i] = (d[0] << 8) | d[1];
	}
}

void mpu_set_accel_bias(const int16_t* accel_bias)
{
	//uint16_t swp;
	uint8_t i;
	uint8_t data[2];

	// bit 0 of the 2 byte bias is for temp comp
	// calculations need to compensate for this and not change it
	for (i = 0; i < 3; i++)
	{
		// SDCC recognizes byte swapping (SDCC manual 8.1.10)
		//swp = accel_bias[i];
		//swp = ((swp << 8) | (swp >> 8));
		
		data[0] = (accel_bias[i] >> 8) & 0xff;
		data[1] = (accel_bias[i]) & 0xff;
		
		i2c_write(0x06 + i * 2, 2, data);
	}
}

void dmp_enable_feature(bool send_cal_gyro)
{
	{
	const uint8_t __code arr[] = {0x02,0xca,0xe3,0x09};
	mpu_write_mem(D_0_104, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0xa3,0xc0,0xc8,0xc2,0xc4,0xcc,0xc6,0xa3,0xa3,0xa3};
	mpu_write_mem(CFG_15, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x20};		// setting this to D8 disables tap, but also messes up the fifo rates
	mpu_write_mem(CFG_27, sizeof arr, arr);
	}
	
	if (send_cal_gyro)
	{
		{
		const uint8_t __code arr[] = {0xB8,0xAA,0xB3,0x8D,0xB4,0x98,0x0D,0x35,0x5D};	// dmp_enable_gyro_cal(1)
		mpu_write_mem(CFG_MOTION_BIAS, sizeof arr, arr);
		}

		{
		const uint8_t __code arr[] = {0xB2,0x8B,0xB6,0x9B};		// DMP_FEATURE_SEND_CAL_GYRO
		mpu_write_mem(CFG_MOTION_BIAS, sizeof arr, arr);
		}
	} else {
		{
		const uint8_t __code arr[] = {0xb8,0xaa,0xaa,0xaa,0xb0,0x88,0xc3,0xc5,0xc7};	// dmp_enable_gyro_cal(0)
		mpu_write_mem(CFG_MOTION_BIAS, sizeof arr, arr);
		}

		{
		const uint8_t __code arr[] = {0xB0,0x80,0xB4,0x90};		// DMP_FEATURE_SEND_RAW_GYRO
		mpu_write_mem(CFG_GYRO_RAW_DATA, sizeof arr, arr);
		}
	}
	
	{
	const uint8_t __code arr[] = {0xf8};
	mpu_write_mem(CFG_20, sizeof arr, arr);
	}
	
	// this configures tap which we don't need, but can't disable it
	// because disabling tap messes up fifo rates
	/*{
	const uint8_t __code arr[] = {0x50,0x00};
	mpu_write_mem(DMP_TAP_THX, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x3c,0x00};
	mpu_write_mem(D_1_36, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x50,0x00};
	mpu_write_mem(DMP_TAP_THY, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x3c,0x00};
	mpu_write_mem(D_1_40, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x50,0x00};
	mpu_write_mem(DMP_TAP_THZ, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x3c,0x00};
	mpu_write_mem(D_1_44, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x3f};
	mpu_write_mem(D_1_72, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x00};
	mpu_write_mem(D_1_79, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x00,0x14};
	mpu_write_mem(DMP_TAPW_MIN, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x00,0x64};
	mpu_write_mem(D_1_218, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x00,0x8e,0xf9,0x90};
	mpu_write_mem(D_1_92, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x00,0x08};
	mpu_write_mem(D_1_90, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x00,0x02};
	mpu_write_mem(D_1_88, sizeof arr, arr);
	}
	*/
	
	{
	const uint8_t __code arr[] = {0xd8};
	mpu_write_mem(CFG_ANDROID_ORIENT_INT, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x8b,0x8b,0x8b,0x8b};
	mpu_write_mem(CFG_LP_QUAT, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x20,0x28,0x30,0x38};
	mpu_write_mem(CFG_8, sizeof arr, arr);
	}

	reset_fifo();
	
	// this is dmp_set_fifo_rate()
	{
	const uint8_t __code arr[] = {0x00,0x03};
	mpu_write_mem(D_0_22, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0xfe,0xf2,0xab,0xc4,0xaa,0xf1,0xdf,0xdf,0xbb,0xaf,0xdf,0xdf};
	mpu_write_mem(CFG_6, sizeof arr, arr);
	}

	reset_fifo();
}

#define PACKET_LENGTH	32

bool mpu_read_fifo_stream(uint16_t length, uint8_t* buffer, uint8_t* more)
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
		return false;

	if (!i2c_read(FIFO_R_W, length, buffer))
		return false;

	*more = (fifo_count != length);
	
	return true;
}

bool dmp_read_fifo(mpu_packet_t* pckt, uint8_t* more)
{
    uint8_t fifo_data[PACKET_LENGTH];
    uint8_t i;

	if (!mpu_read_fifo_stream(PACKET_LENGTH, fifo_data, more))
		return false;

	for (i = 0; i < 4; i++)
		pckt->quat[i] = (fifo_data[i * 4] << 8) | fifo_data[1 + i * 4];

	for (i = 0; i < 3; i++)
		pckt->accel[i] = (fifo_data[16 + i * 2] << 8) | fifo_data[17 + i * 2];

	for (i = 0; i < 3; i++)
		pckt->gyro[i] = (fifo_data[22 + i * 2] << 8) | fifo_data[23 + i * 2];

    return true;
}

void load_biases(void)
{
	const settings_t* pSettings = get_settings();

	if (pSettings)
	{
		dprintf("%s\ngyro %d %d %d\naccel %d %d %d\n",
						"loading",
						pSettings->gyro_bias[0], pSettings->gyro_bias[1], pSettings->gyro_bias[2],
						pSettings->accel_bias[0], pSettings->accel_bias[1], pSettings->accel_bias[2]);
						
		mpu_set_gyro_bias(pSettings->gyro_bias);
		mpu_set_accel_bias(pSettings->accel_bias);
	} else {
		dputs("no settings saved");
	}
}

void dmp_init(bool send_cal_gyro)
{
	if (!dmp_load_firmware())
	{
		dputs("dmp_load_firmware FAILED!!!");
		return;
	}

	if (!dmp_set_orientation())
	{
		dputs("dmp_set_orientation FAILED!!!");
		return;
	}

	dmp_enable_feature(send_cal_gyro);
	
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

	load_biases();
}

#define FIFO_HZ		200

void mpu_init(bool send_cal_gyro)
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
	
	dmp_init(send_cal_gyro);
}

void mpu_calibrate_bias(void)
{
	uint8_t more;
	uint8_t scnt;
	int8_t accel_step = 10;
	mpu_packet_t pckt;
	settings_t new_settings;

	LED_YELLOW = 1;
	
	dputs("**************** calibrating");
	
	mpu_init(true);
	
	memset(&new_settings, 0, sizeof(new_settings));
	
	// read the current accel bias
	mpu_read_accel_bias(new_settings.accel_bias);

	// set default gyro bias
	mpu_set_gyro_bias(new_settings.gyro_bias);
	
	dprintf("%s\ngyro %d %d %d\naccel %d %d %d\n",
					"old",
					new_settings.gyro_bias[0], new_settings.gyro_bias[1], new_settings.gyro_bias[2],
					new_settings.accel_bias[0], new_settings.accel_bias[1], new_settings.accel_bias[2]);
	
	for (scnt = 0; scnt < 200; scnt++)
	{
		while (MPU_IRQ == 1)
			dbgPoll();
		while (MPU_IRQ == 0)
			;
		
		if (scnt == 100)
			accel_step = 2;
		
		do {
			dmp_read_fifo(&pckt, &more);
		} while (more);

		if (dbgEmpty())
			dprintf("g %d %d %d  a %d %d %d\n",
						pckt.gyro[0], pckt.gyro[1], pckt.gyro[2],
						pckt.accel[0], pckt.accel[1], pckt.accel[2]);
			
		// accel
		if (pckt.accel[0] >= 1)
			new_settings.accel_bias[0] -= accel_step;
		else if (pckt.accel[0] <= -1)
			new_settings.accel_bias[0] += accel_step;

		if (pckt.accel[1] >= 1)
			new_settings.accel_bias[1] -= accel_step;
		else if (pckt.accel[1] <= -1)
			new_settings.accel_bias[1] += accel_step;

		if (pckt.accel[2] > 16384)
			new_settings.accel_bias[2] -= accel_step;
		else if (pckt.accel[2] < 16384)
			new_settings.accel_bias[2] += accel_step;

		// gyro
		if (pckt.gyro[0] > 1)
			new_settings.gyro_bias[0]--;
		else if (pckt.gyro[0] < -1)
			new_settings.gyro_bias[0]++;

		if (pckt.gyro[1] > 1)
			new_settings.gyro_bias[1]--;
		else if (pckt.gyro[1] < -1)
			new_settings.gyro_bias[1]++;

		if (pckt.gyro[2] > 1)
			new_settings.gyro_bias[2]--;
		else if (pckt.gyro[2] < -1)
			new_settings.gyro_bias[2]++;

		// push the biases to the MPU
		mpu_set_gyro_bias(new_settings.gyro_bias);
		mpu_set_accel_bias(new_settings.accel_bias);
		
		delay_ms(100);
	}

	// now save our settings
	save_settings(&new_settings);
	
	dprintf("%s\ngyro %d %d %d\naccel %d %d %d\n",
					"new",
					new_settings.gyro_bias[0], new_settings.gyro_bias[1], new_settings.gyro_bias[2],
					new_settings.accel_bias[0], new_settings.accel_bias[1], new_settings.accel_bias[2]);

	dbgFlush();
	
	mpu_init(false);

	LED_YELLOW = 0;
}
