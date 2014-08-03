#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "reg24lu1.h"

#include "reports.h"
#include "rf_protocol.h"
#include "rf_dngl.h"
#include "nrfutils.h"

#include "usb.h"
#include "usb_regs.h"
#include "hw_defs.h"

#include "nrfdbg.h"
#include "proc_packet.h"
#include "dongle_settings.h"

void main(void)
{
	bool joystick_report_ready = false;
	__xdata mpu_packet_t packet;
	uint8_t last_timer_capture;
	uint8_t packet_cnt = 0, last_total_packets = 0;

	P0DIR = 0x00;	// all outputs
	P0ALT = 0x00;	// all GPIO default behavior
	P0 = 0;			// all low
	
	// timer init
	T2CON =	0b10000001;		// start 1/24 timer
	CCEN =	0b11000000;		// capture on write to CCL3
	last_timer_capture = 0;
	
	LED_off();
	
	dbgInit();
	dputs("\nI live...");

	usbInit();

	rf_dngl_init();

	reset_joystick_report();
	
	for (;;)
	{
		usbPoll();	// handles USB interrupts
		dbgPoll();	// send chars from the UART TX buffer
		
		CCL3 = 1;	// capture CCH3
		if (last_timer_capture > CCH3)
		{
			last_total_packets = packet_cnt;
			packet_cnt = 0;
		}
		last_timer_capture = CCH3;
		
		// reset the timer
		// try to read the recv buffer, then process the received data
		if (rf_dngl_recv(&packet, sizeof packet) == sizeof packet)
		{
			joystick_report_ready |= process_packet(&packet);

			packet_cnt++;
		}

		// send the report if the endpoint is not busy
		if ((in1cs & 0x02) == 0   &&   (joystick_report_ready  ||  usbHasIdleElapsed()))
		{
			// copy the joystick report into the endpoint buffer
			memcpy(in1buf, &usb_joystick_report, sizeof(usb_joystick_report));

			// send the data on it's way
			in1bc = sizeof(usb_joystick_report);
			
			joystick_report_ready = false;
		}
	}
}
