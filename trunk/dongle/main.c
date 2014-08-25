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

// These are called by the USB code in usb.c
void on_set_report(void)
{
	if (out0buf[0] == DONGLE_SETTINGS_REPORT_ID)
	{
		// save the data structure
		save_settings((FeatRep_DongleSettings*) out0buf);

	} else if (out0buf[0] == COMMAND_REPORT_ID) {
	
		// tell the head tracker to recalibrate
		uint8_t ack_payload = out0buf[1];
		rf_dngl_queue_ack_payload(&ack_payload, 1);
	}
}

void on_get_report(void)
{
	// this requests the HID report we defined with the HID report descriptor.
	// this is usually requested over EP1 IN, but can be requested over EP0 too.
	if (usbReqHidGetSetReport.reportID == JOYSTICK_REPORT_ID)
	{
		memcpy(in0buf, &usb_joystick_report, sizeof(usb_joystick_report));

		// send the data on it's way
		in0bc = sizeof(usb_joystick_report);

	} else if (usbReqHidGetSetReport.reportID == DONGLE_SETTINGS_REPORT_ID) {

		// at the moment dongle_settings_t and FeatRep_AxisConfig fields
		// have the same members, so we're abusing this fact.
		// I'm sure it will come back and bite me in the ass,
		// but I'll leave it as is for now
		memcpy(in0buf, get_settings(), sizeof(FeatRep_DongleSettings));
		
		in0buf[0] = DONGLE_SETTINGS_REPORT_ID;
		
		// send the data on it's way
		in0bc = sizeof(FeatRep_DongleSettings);

	} else if (usbReqHidGetSetReport.reportID == CALIBRATION_DATA_REPORT_ID) {

		// tell the head tracker to send us the calibration data
		uint8_t ack_payload = CMD_SEND_CALIB_DATA;
		uint8_t pckt_cnt, bytes_rcvd;
		calib_data_t calib_data;
		FeatRep_CalibrationData __xdata * pReport = (FeatRep_CalibrationData __xdata *) in0buf;
		
		rf_dngl_queue_ack_payload(&ack_payload, 1);

		pReport->report_id = CALIBRATION_DATA_REPORT_ID;
		pReport->has_tracker_responded = 0;
		
		// now wait for that data
		for (pckt_cnt = 0; pckt_cnt < 5; ++pckt_cnt)
		{
			bytes_rcvd = rf_dngl_recv(&calib_data, sizeof(calib_data));
			if (bytes_rcvd == sizeof(calib_data))
			{
				pReport->has_tracker_responded = 1;
				pReport->is_calibrated = calib_data.is_calibrated;
				pReport->gyro_bias[0] = calib_data.gyro_bias[0];
				pReport->gyro_bias[1] = calib_data.gyro_bias[1];
				pReport->gyro_bias[2] = calib_data.gyro_bias[2];
				pReport->accel_bias[0] = calib_data.accel_bias[0];
				pReport->accel_bias[1] = calib_data.accel_bias[1];
				pReport->accel_bias[2] = calib_data.accel_bias[2];

				dprintf("%c %d %d %d\n", calib_data.is_calibrated ? 'C' : 'N',
										calib_data.accel_bias[0], calib_data.accel_bias[1], calib_data.accel_bias[2]);
				
				break;
			}
			
			delay_ms(20);
		}

		// send the data
		in0bc = sizeof(FeatRep_CalibrationData);
	}
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
		usbPoll();	// handles USB events
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
