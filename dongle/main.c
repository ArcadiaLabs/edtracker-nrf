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

void main(void)
{
	bool joystick_report_ready = false;
	__xdata mpu_packet_t packet;

	P0DIR = 0x00;	// all outputs
	P0ALT = 0x00;	// all GPIO default behavior
	P0 = 0;
	
	LED_off();
	
	usbInit();
	dbgInit();

	rf_dngl_init();

	reset_joystick_report();
	
	for (;;)
	{
		usbPoll();	// handles USB interrupts
		dbgPoll();	// send chars from the UART TX buffer
		
		// try to read the recv buffer, then process the received data
		if (rf_dngl_recv(&packet, sizeof packet) == sizeof packet)
		{
			joystick_report_ready |= process_packet(&packet);
			TogP(P00);
		}

		// send the report if the endpoint is not busy
		if ((in1cs & 0x02) == 0   &&   (joystick_report_ready  ||  usbHasIdleElapsed()))
		{
			// copy the joystick report into the endpoint buffer
			memcpy(in1buf, &usb_joystick_report, USB_EP1_SIZE);

			// send the data on it's way
			in1bc = USB_EP1_SIZE;
			
			joystick_report_ready = false;
			
			TogP(P01);
		}
	}
}
