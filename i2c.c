#include <stdint.h>
#include <stdbool.h>

#include "tgtdefs.h"
#include "i2c.h"
#include "nrfutils.h"

#define BROADCAST_ENABLE		7
#define CLOCK_STOP				6
#define X_STOP					5
#define X_START					4
#define CLOCK_FREQUENCY_1		3
#define CLOCK_FREQUENCY_0		2
#define MASTER_SELECT			1
#define WIRE_2_ENABLE			0

#define MASK_IRQ				5
#define BROADCAST				4
#define INT_STOP				3
#define INT_ADDR_MATCH			2
#define NACK					1
#define DATA_READY				0

void i2c_init(void)
{
	W2CON0 |= _BV(WIRE_2_ENABLE);
	W2CON0 |= _BV(MASTER_SELECT);
}

static bool i2c_wait_data_ready(void)
{
	uint8_t w2_status;
	bool data_ready, ack_received;

	do {
		w2_status = W2CON1;
		data_ready = (w2_status & _BV(DATA_READY));
		ack_received = ((w2_status & _BV(NACK)) == 0);
		delay_us(10);
	} while (!data_ready);

	return ack_received;
}

static void i2c_wait_data_ready_simple(void)
{
	while ((W2CON1 & _BV(DATA_READY)) == 0)
		delay_us(10);
}

int i2c_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, const uint8_t* data)
{
	// start, address and write bit (0)
	W2DAT = slave_addr << 1;
	i2c_wait_data_ready_simple();

	W2DAT = reg_addr;
	i2c_wait_data_ready_simple();
	
	while (length--)
	{
		W2DAT = *data++;
		i2c_wait_data_ready();
	}

	return 0;
}

int i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t* data)
{
	// start, address and read bit (1)
	W2DAT = (slave_addr << 1) | 0x01;
	i2c_wait_data_ready_simple();

	W2DAT = reg_addr;
	i2c_wait_data_ready_simple();
	
	while (length--)
	{
		W2DAT = *data++;
		i2c_wait_data_ready();
	}

	return 0;
}
