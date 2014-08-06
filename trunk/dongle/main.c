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

void handle_set_report(void)
{
	if (feature_report[0] == AXIS_CONFIG_REPORT_ID)
	{
		// save the data structure
		save_settings((dongle_settings_t*) feature_report);

	} else if (feature_report[0] == COMMAND_REPORT_ID) {
	
		// tell the head tracker to recalibrate or send the calibration data
		uint8_t ack_payload = feature_report[1];
		rf_dngl_queue_ack_payload(&ack_payload, 1);
	}

	new_set_report = false;
}

void main(void)
{
	bool joystick_report_ready = false;
	__xdata mpu_packet_t packet;
	uint8_t last_timer_capture;
	uint8_t total_packets[10], total_packets_ndx;		// last 1 second of packets
	
	P0DIR = 0x00;	// all outputs
	P0ALT = 0x00;	// all GPIO default behavior
	P0 = 0;			// all low
	
	// timer init
	T2CON =	0b10000001;		// start 1/24 timer
	CCEN =	0b11000000;		// capture on write to CCL3
	last_timer_capture = 0;
	
	memset(total_packets, 0, sizeof(total_packets));
	total_packets_ndx = 0;
	
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
			total_packets_ndx++;
			if (total_packets_ndx == sizeof(total_packets))
				total_packets_ndx = 0;
				
			total_packets[total_packets_ndx] = 0;
		}
		last_timer_capture = CCH3;

		// handle incoming feature reports
		if (new_set_report)
			handle_set_report();
		
		// reset the timer
		// try to read the recv buffer, then process the received data
		if (rf_dngl_recv(&packet, sizeof packet) == sizeof packet)
		{
			joystick_report_ready |= process_packet(&packet);

			total_packets[total_packets_ndx]++;
			
			LED_on();
		} else {
			LED_off();
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
