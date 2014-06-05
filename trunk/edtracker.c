#include <stdint.h>
#include <stdbool.h>

#include "nrfutils.h"
#include "i2c.h"
#include "mpu.h"

int main(void)
{
	uint8_t __idata ptr;
	
	i2c_init();
	mpu_init();

	while (1)
	{
		i2c_read(0x68, 12, 12, &ptr);
		i2c_write(0x68, 12, 12, &ptr);
	}
}
