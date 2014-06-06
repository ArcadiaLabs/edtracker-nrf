#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <reg24le1.h>
#include <nrfdbg.h>
#include <nrfutils.h>

#include "i2c.h"

// this is based on the Nordic HAL I2C library, and is specific to MPU-6050

#define MPU_ADDR_READ			0xD1
#define MPU_ADDR_WRITE			0xD0

#define I2C_PIN_SDA						P05
#define I2C_PIN_SCL						P04
#define I2C_CLEAR_SDA_SCL				(P0 &= 0xCF)
#define I2C_OVERRIDE_SDA_SCL(a, b)		P0DIR = (P0DIR & 0xCF) | (a << 5) | (b << 4)

// W2CON0 bits
#define BROADCAST_ENABLE		7
#define CLOCK_STOP				6
#define X_STOP					5
#define X_START					4
#define CLOCK_FREQUENCY_1		3
#define CLOCK_FREQUENCY_0		2
#define MASTER_SELECT			1
#define WIRE_2_ENABLE			0

// W2CON1 bits
#define MASK_IRQ				5
#define BROADCAST				4
#define INT_STOP				3
#define INT_ADDR_MATCH			2
#define NACK					1
#define DATA_READY				0

#define I2C_ISSUE_START_COND	(W2CON0 |= _BV(X_START))
#define I2C_ISSUE_STOP_COND		(W2CON0 |= _BV(X_STOP))
#define I2C_WAIT_FOR_INTERRUPT	{ while(!SPIF); SPIF = 0; }
#define I2C_WRITE(a)			W2DAT = (a)
#define I2C_READ()				W2DAT

#define I2C_DIR_READ		1
#define I2C_DIR_WRITE		0

void i2c_soft_reset(void);

void i2c_init(void)
{
	W2CON0 |= _BV(WIRE_2_ENABLE);
	W2CON0 |= _BV(MASTER_SELECT) | _BV(CLOCK_FREQUENCY_0);	// 100kHz
	//W2CON0 |= _BV(MASTER_SELECT) | _BV(CLOCK_FREQUENCY_1);	// 400kHz
	
	I2C_ISSUE_STOP_COND;
}

uint8_t i2c_wait_data_ready(void)
{
	uint8_t w2_status;
	bool data_ready, ack_received;

	do {
		w2_status = W2CON1;
		data_ready = (w2_status & _BV(DATA_READY));
		ack_received = ((w2_status & _BV(NACK)) == 0);
		delay_us(10);
	} while (!data_ready);

	return w2_status;
}

bool i2c_init_transfer(uint8_t address)
{
	uint8_t w2_status;

	I2C_ISSUE_START_COND;
	I2C_WRITE(address);

	w2_status = i2c_wait_data_ready();

	if (w2_status & _BV(NACK))
		return false;	// NACK received from slave or timeout

	return true;		// ACK received from slave
}

bool i2c_write(uint8_t reg_addr, uint8_t data_len, const uint8_t* data_ptr)
{
	bool ack_received;
	uint8_t w2_status;
		
	ack_received = i2c_init_transfer(MPU_ADDR_WRITE);

	I2C_WRITE(reg_addr);
	w2_status = i2c_wait_data_ready();
	if (w2_status & _BV(NACK))
		ack_received = false;
	
	while (data_len--  &&  ack_received)
	{
		I2C_WRITE(*data_ptr++);
		w2_status = i2c_wait_data_ready();
		if (w2_status & _BV(NACK))
			ack_received = false;
	}
	
	I2C_ISSUE_STOP_COND;

	return ack_received;
}

bool i2c_read(uint8_t reg_addr, uint8_t data_len, uint8_t *data_ptr)
{
	uint8_t w2_status;

	// start and write slave address
	bool ack_received = i2c_init_transfer(MPU_ADDR_WRITE);

	// register address
	I2C_WRITE(reg_addr);
	w2_status = i2c_wait_data_ready();
	if ((w2_status & _BV(NACK)) == 0)
	{
		// repeated start and slave address
		if (i2c_init_transfer(MPU_ADDR_READ))
		{
			while (data_len-- && ack_received)
			{
				if (data_len == 0)
					I2C_ISSUE_STOP_COND;

				w2_status = i2c_wait_data_ready();

				*data_ptr++ = I2C_READ();
				ack_received = !(w2_status & _BV(NACK));
			}

			return true;
		} else {
			// This situation (NACK received on bus while trying to read from a slave) leads to a deadlock in the 2-wire interface. 
			i2c_soft_reset(); // Workaround for the deadlock
		}
	}

	return false;
}

void i2c_soft_reset(void)
{
	uint8_t pulsecount, w2_freq;

	// Store the selected 2-wire frequency 
	w2_freq = W2CON0 & 0x0C;
	// Prepare the GPIO's to take over SDA & SCL
	I2C_CLEAR_SDA_SCL;
	I2C_OVERRIDE_SDA_SCL(1, 1);
	//P0DIR = 0xFF;

	// Reset 2-wire. SCL goes high.
	W2CON0 = 0x03;
	W2CON0 = 0x07;

	// Disable 2-wire.
	W2CON0 = 0x06;

	// SDA and SCL are now under software control, and both are high. 
	// Complete first SCL pulse.
	//P0DIR = 0xEF;
	I2C_OVERRIDE_SDA_SCL(1, 0);

	// SCL low
	delay_us(5);
	//P0DIR = 0xCF;
	I2C_OVERRIDE_SDA_SCL(0, 0);

	// SDA low
	// Create SCL pulses for 7 more data bits and ACK/NACK
	delay_us(5);
	for (pulsecount = 0; pulsecount < 8; ++pulsecount)
	{
		//P0DIR = 0xDF;
		I2C_OVERRIDE_SDA_SCL(0, 1);
		delay_us(5);
		//P0DIR = 0xCF;
		I2C_OVERRIDE_SDA_SCL(0, 0);
		delay_us(5);
	}

	// Generating stop condition by driving SCL high
	delay_us(5);
	//P0DIR = 0xDF;
	I2C_OVERRIDE_SDA_SCL(0, 1);

	// Drive SDA high
	delay_us(5);
	//P0DIR = 0xFF;
	I2C_OVERRIDE_SDA_SCL(1, 1);

	// Work-around done. Return control to 2-wire.
	W2CON0 = 0x07;

	// Reset 2-wire and return to master mode at the frequency selected before calling this function
	W2CON0 = 0x03;
	W2CON0 = 0x03 | w2_freq;
}
