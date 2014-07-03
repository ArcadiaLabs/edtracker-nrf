#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "reports.h"
#include "keycode.h"
#include "rf_protocol.h"
#include "rf_dngl.h"

#include "nrfdbg.h"

hid_kbd_report_t	usb_keyboard_report;
uint8_t				usb_consumer_report;

// contains the last received LED report
uint8_t usb_led_report;		// bit	LED
							// 0	CAPS
							// 1	NUM
							// 2	SCROLL

void reset_keyboard_report(void)
{
	usb_keyboard_report.modifiers = 0;
	usb_keyboard_report.keys[0] = KC_NO;
	usb_keyboard_report.keys[1] = KC_NO;
	usb_keyboard_report.keys[2] = KC_NO;
	usb_keyboard_report.keys[3] = KC_NO;
	usb_keyboard_report.keys[4] = KC_NO;
	usb_keyboard_report.keys[5] = KC_NO;
}
