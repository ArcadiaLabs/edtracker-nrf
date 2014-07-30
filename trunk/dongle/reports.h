#ifndef REPORTS_H
#define REPORTS_H

void reset_joystick_report(void);

// this is the HID report structure
// this is what the data that we send to the host is comprised of
typedef struct
{
	int16_t	x;
	int16_t	y;
	int16_t	z;
} hid_joystick_report_t;

extern hid_joystick_report_t	usb_joystick_report;	// the HID keyboard report

#endif	// REPORTS_H