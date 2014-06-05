#include <stdint.h>
#include <stdbool.h>

#include <reg24le1.h>

#include "i2c.h"
#include "nrfutils.h"

#define MPU_ADDR			0x68

// the MPU-6050 registers
#define REG_rate_div       0x19
#define REG_lpf            0x1A
#define REG_prod_id        0x0C
#define REG_user_ctrl      0x6A
#define REG_fifo_en        0x23
#define REG_gyro_cfg       0x1B
#define REG_accel_cfg      0x1C
#define REG_motion_thr     0x1F
#define REG_motion_dur     0x20
#define REG_fifo_count_h   0x72
#define REG_fifo_r_w       0x74
#define REG_raw_gyro       0x43
#define REG_raw_accel      0x3B
#define REG_temp           0x41
#define REG_int_enable     0x38
#define REG_dmp_int_status 0x39
#define REG_int_status     0x3A
#define REG_pwr_mgmt_1     0x6B
#define REG_pwr_mgmt_2     0x6C
#define REG_int_pin_cfg    0x37
#define REG_mem_r_w        0x6F
#define REG_accel_offs     0x06
#define REG_i2c_mst        0x24
#define REG_bank_sel       0x6D
#define REG_mem_start_addr 0x6E
#define REG_prgm_start_h   0x70

#define BIT_I2C_MST_VDDIO   (0x80)
#define BIT_FIFO_EN         (0x40)
#define BIT_DMP_EN          (0x80)
#define BIT_FIFO_RST        (0x04)
#define BIT_DMP_RST         (0x08)
#define BIT_FIFO_OVERFLOW   (0x10)
#define BIT_DATA_RDY_EN     (0x01)
#define BIT_DMP_INT_EN      (0x02)
#define BIT_MOT_INT_EN      (0x40)
#define BITS_FSR            (0x18)
#define BITS_LPF            (0x07)
#define BITS_HPF            (0x07)
#define BITS_CLK            (0x07)
#define BIT_FIFO_SIZE_1024  (0x40)
#define BIT_FIFO_SIZE_2048  (0x80)
#define BIT_FIFO_SIZE_4096  (0xC0)
#define BIT_RESET           (0x80)
#define BIT_SLEEP           (0x40)
#define BIT_S0_DELAY_EN     (0x01)
#define BIT_S2_DELAY_EN     (0x04)
#define BITS_SLAVE_LENGTH   (0x0F)
#define BIT_SLAVE_BYTE_SW   (0x40)
#define BIT_SLAVE_GROUP     (0x10)
#define BIT_SLAVE_EN        (0x80)
#define BIT_I2C_READ        (0x80)
#define BITS_I2C_MASTER_DLY (0x1F)
#define BIT_AUX_IF_EN       (0x20)
#define BIT_ACTL            (0x80)
#define BIT_LATCH_EN        (0x20)
#define BIT_ANY_RD_CLR      (0x10)
#define BIT_BYPASS_EN       (0x02)
#define BITS_WOM_EN         (0xC0)
#define BIT_LPA_CYCLE       (0x20)
#define BIT_STBY_XA         (0x20)
#define BIT_STBY_YA         (0x10)
#define BIT_STBY_ZA         (0x08)
#define BIT_STBY_XG         (0x04)
#define BIT_STBY_YG         (0x02)
#define BIT_STBY_ZG         (0x01)
#define BIT_STBY_XYZA       (BIT_STBY_XA | BIT_STBY_YA | BIT_STBY_ZA)
#define BIT_STBY_XYZG       (BIT_STBY_XG | BIT_STBY_YG | BIT_STBY_ZG)

uint8_t mpu_write_byte(uint8_t reg_addr, uint8_t val)
{
	return i2c_write(MPU_ADDR, reg_addr, 1, &val) ? 0 : 0xff;
}

bool mpu_init(void)
{
	// Reset device.
	if (mpu_write_byte(REG_pwr_mgmt_1, BIT_RESET))
		return false;

	delay_ms(100);

	// Wake up chip.
	if (mpu_write_byte(REG_pwr_mgmt_1, 0x00))
		return false;

	// mpu_set_gyro_fsr(2000);
	mpu_write_byte(REG_gyro_cfg, 0x18);
	
	// mpu_set_accel_fsr(2);
	mpu_write_byte(REG_accel_cfg, 0x00);
	
	// mpu_set_lpf(42);
	mpu_write_byte(REG_lpf, 3);
	
	//mpu_set_sample_rate(50);
	//mpu_configure_fifo(0);
    //
	//// Already disabled by setup_compass.
	//if (mpu_set_bypass(0))
	//	return -1;
    //
	//mpu_set_sensors(0);
	
	return true;
}

/*
dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL);
dmp_load_motion_driver_firmware();
dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);
dmp_set_fifo_rate(DEFAULT_MPU_HZ);
dmp_set_fifo_rate(DEFAULT_MPU_HZ/2);
dmp_set_orientation(gyro_orients[orientation]);
mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
mpu_get_compass_reg(mag, &timestamp);
mpu_init();
mpu_set_accel_bias_6050_reg(aBias, true);
mpu_set_accel_fsr(2);
mpu_set_dmp_state(0);
mpu_set_dmp_state(1);
mpu_set_dmp_state(1);  // This enables the DMP; at this point, interrupts should commence
mpu_set_gyro_bias_reg(gBias);
mpu_set_gyro_fsr(2000);
mpu_set_lpf(42);
mpu_set_sample_rate(DEFAULT_MPU_HZ);
mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
*/