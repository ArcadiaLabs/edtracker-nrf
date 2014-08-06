#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <compiler_mcs51.h>

#include "dongle_settings.h"
#include "reg24lu1.h"
#include "nrfdbg.h"

// 512 bytes each
#define DATA0_ADDR		((dongle_settings_t __xdata *) 0x7c00)
#define DATA0_PAGE_NUM	62
#define DATA1_ADDR		((dongle_settings_t1 __xdata *) 0x7e00)
#define DATA1_PAGE_NUM	63

#define DATA_PAGE_SIZE			0x200
// how many settings blocks can we store in the two pages
#define BLOCKS_CAPACITY			((DATA_PAGE_SIZE*2) / sizeof(dongle_settings_t))

/*
void list_settings(void)
{
	const __xdata dongle_settings_t* pStart = DATA0_ADDR;
	uint8_t cnt;
	
	dputs("------------");
	for (cnt = 0; cnt < BLOCKS_CAPACITY; ++cnt)
	{
		// define PRINTF as printf_fast_f in the makefile
		dprintf("%d %d %d %f %f %f %f %f %f\n",
				cnt,
				pStart[cnt].is_selfcenter, pStart[cnt].is_linear,
				pStart[cnt].lin_fact_x, pStart[cnt].lin_fact_y, pStart[cnt].lin_fact_z,
				pStart[cnt].exp_fact_x, pStart[cnt].exp_fact_y, pStart[cnt].exp_fact_z);
	}
	
	dbgFlush();
}
*/

__xdata dongle_settings_t default_settings =
{
	0,		// is_empty
	
	1,		// is_selfcenter
	0,		// is_linear

	8.0,	// lin_fact_x
	8.0,	// lin_fact_y
	8.0,	// lin_fact_z

	12.0,	// exp_fact_x
	12.0,	// exp_fact_y
	12.0,	// exp_fact_z
};

void flash_page_erase(uint8_t pn)
{
    CKCON = 0x01;   // See nRF24LU1p AX PAN
    
    FCR = 0xAA;		// Enable flash write operation
    FCR = 0x55;
    WEN = 1;

    FCR = pn;			// Write the page address to FCR to start the page erase operation

    while(RDYN == 1)	// Wait for the erase operation to finish
        ;

    WEN = 0;
    CKCON = 0x02;
}

void flash_bytes_write(uint16_t a, uint8_t __xdata * p, uint16_t n)
{
    uint8_t __xdata * __data pb;

    CKCON = 0x01;
    
    FCR = 0xAA;		// enable flash write operation
    FCR = 0x55;
    WEN = 1;

    pb = (uint8_t __xdata *) a;	// write the bytes directly to the flash
    while (n--)
    {
        *pb++ = *p++;

        // wait for the write operation to finish
        while (RDYN == 1)
            ;
    }

    WEN = 0;
    CKCON = 0x02;
}

int8_t get_ndx_of_current_settings(void)
{
	const dongle_settings_t __xdata * pStart = DATA0_ADDR;
	uint8_t cnt;
	
	// if no settings have been saved yet
	if (pStart->is_empty == 0xff)
		return -1;

	// find the last valid settings block (with valid == 0x00) in the page
	for (cnt = 0; cnt < BLOCKS_CAPACITY - 1  &&  pStart[cnt + 1].is_empty != 0xff; ++cnt)
		;

	return cnt;
}

const dongle_settings_t __xdata * get_settings(void)
{
	const dongle_settings_t __xdata * pStart = DATA0_ADDR;
	int8_t ndx;

	ndx = get_ndx_of_current_settings();
	
	// no settings yet, so make a default one
	if (ndx == -1)
	{
		save_settings(&default_settings);
		ndx = 0;
	}
		
	return pStart + ndx;
}

void save_settings(const dongle_settings_t __xdata * pNewSettings)
{
	// get the next empty slot
	dongle_settings_t save_img;
	int8_t new_ndx = get_ndx_of_current_settings() + 1;

	// if all slots are full
	if (new_ndx == BLOCKS_CAPACITY)
	{
		// erase the two pages and start from the beginning
		flash_page_erase(DATA0_PAGE_NUM);
		flash_page_erase(DATA1_PAGE_NUM);
		
		new_ndx = 0;
	}
	
	memcpy(&save_img, pNewSettings, sizeof(save_img));
	
	save_img.is_empty = 0x00;
	
	// save the new settings
	flash_bytes_write((uint16_t)(DATA0_ADDR + new_ndx), (uint8_t __xdata *) &save_img, sizeof(save_img));
}
