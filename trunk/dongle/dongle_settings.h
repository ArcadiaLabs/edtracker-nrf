#ifndef DONGLE_SETTINGS_H
#define DONGLE_SETTINGS_H

// this is the structure we save into the
// data pages of the nRF24LU1
typedef struct
{
	uint8_t		is_empty;		// 0x00 if used, 0xff if empty
	
	uint8_t		is_selfcenter;
	uint8_t		is_linear;

	// linear factors
	float		lin_fact_x;
	float		lin_fact_y;
	float		lin_fact_z;

	// exponential factors
	float		exp_fact_x;
	float		exp_fact_y;
	float		exp_fact_z;

} dongle_settings_t;

const dongle_settings_t __xdata * get_settings(void);
void save_settings(const dongle_settings_t __xdata * pNewSettings);

// these are for testing
//void list_settings(void);

#endif	// DONGLE_SETTINGS_H
