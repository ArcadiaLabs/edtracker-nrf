#ifndef SETTINGS_H
#define SETTINGS_H

// this is the structure we save into the
// non-volatile section of the nRF24LE1
typedef struct
{
	uint8_t		is_empty;		// 0xff if empty, 0x00 if used
	int16_t		gyro_bias[3];
	int16_t		accel_bias[3];
} settings_t;

const settings_t __xdata * get_settings(void);
void save_settings(settings_t* pNewSettings);

// these are for testing
//void list_settings(void);
//void test_settings(void);

#endif	// SETTINGS_H
