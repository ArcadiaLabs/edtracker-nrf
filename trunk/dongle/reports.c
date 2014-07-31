#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <compiler_mcs51.h>

#include "reports.h"
#include "usb.h"
#include "rf_protocol.h"
#include "rf_dngl.h"

#include "nrfdbg.h"

hid_joystick_report_t	usb_joystick_report;

void reset_joystick_report(void)
{
	usb_joystick_report.report_id = JOYSTICK_REPORT_ID;
	usb_joystick_report.x = 0;
	usb_joystick_report.y = 0;
	usb_joystick_report.z = 0;
}
