#ifndef I2C_H
#define I2C_H

void i2c_init(void);
bool i2c_write(uint8_t reg_addr, uint8_t data_len, const uint8_t* data_ptr);
bool i2c_read(uint8_t reg_addr, uint8_t data_len, uint8_t *data_ptr);

#endif	// I2C_H