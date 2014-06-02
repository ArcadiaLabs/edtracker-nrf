#pragma once

#include "nrfutils.h"

extern int i2c_write(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char const *data);
extern int i2c_read(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char *data);
extern void get_ms(unsigned long *count);

#define log_i(...) do { } while (0)
#define log_e(...) do { } while (0)
