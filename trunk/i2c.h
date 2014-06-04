#pragma once

void i2c_init(void);
int i2c_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t const* data);
int i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t* data);
