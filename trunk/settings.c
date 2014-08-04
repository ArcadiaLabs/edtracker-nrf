#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <compiler_mcs51.h>

#include "settings.h"
#include "reg24le1.h"
#include "nrfdbg.h"

/*
// size of these is 256 bytes
#define NV_DATA_EXT_ENDUR0_ADDR			((settings_t __xdata *) 0xfa00)
#define NV_DATA_EXT_ENDUR0_PAGE_NUM		32
#define NV_DATA_EXT_ENDUR1_ADDR			((settings_t __xdata *) 0xfb00)
#define NV_DATA_EXT_ENDUR1_PAGE_NUM		33
*/

// size of these is 512 bytes
#define NV_DATA_NORM_ENDUR0_ADDR		((settings_t __xdata *) 0xfc00)
#define NV_DATA_NORM_ENDUR0_PAGE_NUM	34
#define NV_DATA_NORM_ENDUR1_ADDR		((settings_t __xdata *) 0xfe00)
#define NV_DATA_NORM_ENDUR1_PAGE_NUM	35

#define NV_DATA_PAGE_SIZE				0x200
// how many settings block can we store in the two pages
#define BLOCKS_CAPACITY					((NV_DATA_PAGE_SIZE*2) / sizeof(settings_t))

/*
void test_settings(void)
{
	settings_t s;
	const __xdata settings_t* ps = get_settings();
	
	memset(&s, 0, sizeof(settings_t));
	
	if (ps == 0)
	{
		uint8_t cnt;
		for (cnt = 0; cnt < BLOCKS_CAPACITY - 2; cnt++)
		{
			s.accel_bias[0] = cnt;
			save_settings(&s);
		}
	} else {
		s.accel_bias[0] = -ps->accel_bias[0];
		save_settings(&s);
	}
	
	list_settings();
	
	ps = get_settings();
	dprintf("curr %04x %04x %04x %04x %04x %04x\n",
			ps->gyro_bias[0], ps->gyro_bias[1], ps->gyro_bias[2],
			ps->accel_bias[0], ps->accel_bias[1], ps->accel_bias[2]);
}

void list_settings(void)
{
	const __xdata settings_t* pStart = NV_DATA_NORM_ENDUR0_ADDR;
	uint8_t cnt;
	
	dputs("------------\n");
	for (cnt = 0; cnt < BLOCKS_CAPACITY; ++cnt)
	{
		dprintf("%2i %04x %04x %04x %04x %04x %04x\n",
				cnt,
				pStart[cnt].gyro_bias[0], pStart[cnt].gyro_bias[1], pStart[cnt].gyro_bias[2],
				pStart[cnt].accel_bias[0], pStart[cnt].accel_bias[1], pStart[cnt].accel_bias[2]);
	}
	
	dbgFlush();
}
*/

void flash_page_erase(uint8_t pn)
{
    WEN = 1;	// enable flash write
    FCR = pn;	// page number to erase
    WEN = 0;	// disable flash write
}

int8_t get_current_settings_ndx(void)
{
	const settings_t __xdata * pStart = NV_DATA_NORM_ENDUR0_ADDR;
	uint8_t cnt;
	
	// if no settings have been saved yet
	if (pStart->is_empty == 0xff)
		return -1;

	// find the last valid settings block (with is_empty == 0x00) in the page
	for (cnt = 0; cnt < BLOCKS_CAPACITY - 1  &&  pStart[cnt + 1].is_empty != 0xff; ++cnt)
		;

	return cnt;
}

const settings_t __xdata * get_settings(void)
{
	const settings_t __xdata * pStart = NV_DATA_NORM_ENDUR0_ADDR;
	int8_t ndx = get_current_settings_ndx();
	
	if (ndx == -1)
		return 0;
		
	return pStart + ndx;
}

void save_settings(settings_t* pNewSettings)
{
	const settings_t __xdata * pStart = NV_DATA_NORM_ENDUR0_ADDR;
	int8_t new_ndx = get_current_settings_ndx() + 1;

	// if the two pages are full
	if (new_ndx == BLOCKS_CAPACITY)
	{
		// erase the two pages and start from the beginning
		flash_page_erase(NV_DATA_NORM_ENDUR0_PAGE_NUM);
		flash_page_erase(NV_DATA_NORM_ENDUR1_PAGE_NUM);
		
		new_ndx = 0;
	}
	
	pNewSettings->is_empty = 0x00;
	
	// save the value
	WEN = 1;
	PCON &= ~(1 << 4);	// clear PMW
	memcpy(pStart + new_ndx, pNewSettings, sizeof(settings_t));
	WEN = 0;
}
