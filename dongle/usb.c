#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define DEFINE_USB_REGS

#include "reg24lu1.h"
#include "nrfutils.h"
#include "reports.h"
#include "rf_protocol.h"
#include "rf_dngl.h"
#include "nrfdbg.h"
#include "dongle_settings.h"

#include "usb.h"

#include "usb_regs.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

uint8_t		usb_current_config;
usb_state_t	usb_state;

uint8_t const __code * packetizer_data_ptr;
uint8_t packetizer_data_size;
bool new_set_report = false;
uint8_t feature_report[MAX_FEATURE_REPORT_BYTES];

// We are counting SOF packets as a timer for the HID idle rate.
// usbframel & usbframeh are not good enough for this because of
// difficulty accesing both LSB and MSB in a predictable manner
uint16_t usbFrameCnt = 0;
__xdata uint8_t usbHidIdle = 0;		// forever

void usbInit(void)
{
	// disconnect from USB-bus
	usbcs |= 0x08;
	delay_ms(50);
	usbcs &= ~0x08;
	
	// set up interrupts and clear interrupt flags
	usbien = 0x1B;	// bit	description
					// 5-7	unused
					// 4	uresie	USB reset interrupt enable
					// 3	suspie	USB suspend interrupt enable
					// 2	sutokie	SETUP token interrupt enable
					// 1	sofie	Start of frame interrupt enable
					// 0	sudavie	SETUP data valid interrupt enable
	
	// we only want to get interrupts for EP0 IN/OUT
	// the other interrupts are not needed (but EP1 IN is still used)
	in_ien = 0x03;		// enable IN interrupts on EP0 and EP1
	in_irq = 0x1f;		// reset IN interrupt flags
	out_ien = 0x01;		// enable OUT interrupts on EP0
	out_irq = 0x1f;		// reset OUT interrupt flags

	// setup the USB RAM
	bout1addr = USB_EP0_SIZE/2;
	bout2addr = (USB_EP0_SIZE + USB_EP1_SIZE)/2;
	bout3addr = (USB_EP0_SIZE + USB_EP1_SIZE + USB_EP2_SIZE)/2;
	bout4addr = (USB_EP0_SIZE + USB_EP1_SIZE + USB_EP2_SIZE + USB_EP3_SIZE)/2;
	bout5addr = (USB_EP0_SIZE + USB_EP1_SIZE + USB_EP2_SIZE + USB_EP3_SIZE + USB_EP4_SIZE)/2;

	binstaddr = 0xc0;		// IN start address

	bin1addr = USB_EP0_SIZE/2;
	bin2addr = (USB_EP0_SIZE + USB_EP1_SIZE)/2;
	bin3addr = (USB_EP0_SIZE + USB_EP1_SIZE + USB_EP2_SIZE)/2;
	bin4addr = (USB_EP0_SIZE + USB_EP1_SIZE + USB_EP2_SIZE + USB_EP3_SIZE)/2;
	bin5addr = (USB_EP0_SIZE + USB_EP1_SIZE + USB_EP2_SIZE + USB_EP3_SIZE + USB_EP4_SIZE)/2;

	// enable endpoints
	inbulkval = 0x03;	// enables IN endpoints EP0 and EP1
	outbulkval = 0x01;	// enables OUT endpoint EP0
	inisoval = 0x00;	// ISO not used
	outisoval = 0x00;	// ISO not used
}

bool usbHasIdleElapsed(void)
{
	bool retVal;
	
	if (usbHidIdle == 0)
		return false;

	retVal = usbFrameCnt >= usbHidIdle * 4;
	
	if (retVal)
		usbFrameCnt = 0;
	
	return retVal;
}

void packetizer_isr_ep0_in(void)
{
	__data uint8_t pckt_size, i;

	if (packetizer_data_size == 0)
	{
		// We are getting a ep0in interrupt when the host sends ACK and do not have
		// any more data to send. Arm the in0bsy bit by writing to byte count reg
		in0bc = 0;
		// ACK the status stage
		USB_EP0_HSNAK();
		return;
	}

	// Send the smallest of the data size and USB RAM EP0 IN size
	pckt_size = MIN(packetizer_data_size, USB_EP0_SIZE);

	// Copy data to the USB-controller buffer
	for (i = 0; i < pckt_size; ++i)
		in0buf[i] = *packetizer_data_ptr++;

	// Tell the USB-controller how many bytes to send
	// If a IN is received from host after this the USB-controller will send the data
	in0bc = pckt_size;

	// update the packetizer data count
	packetizer_data_size -= pckt_size;
}

void usbGetDescriptor(void)
{
	uint8_t descriptor = usbReqGetDesc.descType;

	packetizer_data_ptr = 0;
	packetizer_data_size = 0;
	
	if (descriptor == USB_DESC_DEVICE)
	{
		packetizer_data_ptr = (__code uint8_t*) &usb_dev_desc;
		packetizer_data_size = MIN(usbReqGetDesc.lengthLSB, packetizer_data_ptr[0]);
	} else if (descriptor == USB_DESC_CONFIGURATION) {
		packetizer_data_ptr = (__code uint8_t*) &usb_conf_desc;
		packetizer_data_size = MIN(usbReqGetDesc.lengthLSB, sizeof usb_conf_desc);
	} else if (descriptor == USB_DESC_STRING) {

		uint8_t string_id = usbReqGetDesc.descIndex;
		
		// string index 0 is list of supported lang ids
		if (string_id < USB_STRING_DESC_COUNT)
		{
			if (usbReqGetDesc.descIndex == 0)
				packetizer_data_ptr = usb_string_desc_0;
			else if (string_id == 1)
				packetizer_data_ptr = (__code uint8_t*) usb_string_desc_1;
			else if (string_id == 2)
				packetizer_data_ptr = (__code uint8_t*) usb_string_desc_2;
			else
				packetizer_data_ptr = (__code uint8_t*) usb_string_desc_3;

			if (usbReqGetDesc.lengthMSB > 0)
				packetizer_data_size = 0xff;
			else
				packetizer_data_size = MIN(usbReqGetDesc.lengthLSB, packetizer_data_ptr[0]);
		}
	} else if (descriptor == USB_DESC_HID_REPORT) {
		if (usbReqHidGetDesc.interface == 0)
		{
			packetizer_data_ptr = joystick_hid_report_descriptor;
			packetizer_data_size = MIN(usbReqGetDesc.lengthLSB, JOYSTICK_HID_REPORT_DESC_SIZE);
		}
	}

	if (packetizer_data_ptr)
		packetizer_isr_ep0_in();
	else
		USB_EP0_STALL();
}

void usbStdDeviceRequest(void)
{
	switch (usbRequest.bRequest)
	{
	case USB_REQ_GET_STATUS:
		// We must be in ADDRESSED or CONFIGURED state, and wIndex must be 0
		if ((usb_state == ADDRESSED || usb_state == CONFIGURED)  &&  usbRequest.wIndexLSB == 0x00)
		{
			// We aren't self-powered and we don't support remote wakeup
			in0buf[0] = 0x00;
			in0buf[1] = 0x00;
			in0bc = 0x02;
		} else {
			// Stall for invalid requests
			USB_EP0_STALL();
		}
		break;

	case USB_REQ_SET_ADDRESS:
		// USB controller takes care of setting our address
		usb_state = ADDRESSED;
		break;

	case USB_REQ_GET_DESCRIPTOR:
		usbGetDescriptor();
		break;

	case USB_REQ_GET_CONFIGURATION:
	
		if (usb_state == ADDRESSED)
		{
			in0buf[0] = 0x00;
			in0bc = 0x01;
		} else if (usb_state == CONFIGURED) {
			in0buf[0] = usb_current_config;
			in0bc = 0x01;
		} else {
			// Behavior not specified in other states, so STALL
			USB_EP0_STALL();
		}

		break;

	case USB_REQ_SET_CONFIGURATION:

		if (usbRequest.wValueLSB == 0x00)
		{
			usb_state = ADDRESSED;
			usb_current_config = 0x00;
			// Since there isn't a data stage for this request,
			//   we have to explicitly clear the NAK bit
			USB_EP0_HSNAK();
		} else if (usbRequest.wValueLSB == 0x01) {
			usb_state = CONFIGURED;
			usb_current_config = 0x01;
			// Since there isn't a data stage for this request,
			//   we have to explicitly clear the NAK bit
			USB_EP0_HSNAK();
		} else {
			// Stall for invalid config values
			USB_EP0_STALL();
		}
		break;
	}
}

void usbStdEndpointRequest(void)
{
	if (usbRequest.bRequest == USB_REQ_GET_STATUS)
	{
		if (usb_state == CONFIGURED)
		{
			// return Halt feature status
			uint8_t endpoint = usbRequest.wIndexLSB;
			if (endpoint == 0x81)
				in0buf[0] = in1cs & 0x01;
			else if (endpoint == 0x01)
				in0buf[0] = out1cs & 0x01;

			in0bc = 0x02;
		}
	} else {
		USB_EP0_STALL();
	}
}

void usbStdInterfaceRequest(void)
{
	uint8_t bRequest = usbRequest.bRequest;
	
	if (bRequest == USB_REQ_GET_STATUS)
	{
		if (usb_state == CONFIGURED)
		{
			// all values are reserved for interfaces
			in0buf[0] = 0x00;
			in0buf[1] = 0x00;
			in0bc = 0x02;
		}
	} else if (bRequest == USB_REQ_GET_DESCRIPTOR) {
		// this requests the HID report descriptor
		usbGetDescriptor();
	} else {
		USB_EP0_STALL();
	}
}

void usbHidRequest(void)
{
	uint8_t bRequest = usbRequest.bRequest;

	if (bRequest == USB_REQ_HID_SET_REPORT)
	{
		// we get and process the actual data in usbRequestDataReceived()

	} else if (bRequest == USB_REQ_HID_GET_REPORT) {

		on_get_report();

	} else if (bRequest == USB_REQ_HID_GET_IDLE) {

		in0buf[0] = usbHidIdle;
		in0bc = 0x01;
	
	} else if (bRequest == USB_REQ_HID_SET_IDLE) {

		usbHidIdle = usbRequest.wValueMSB;
		
		// wValueLSB holds the reportID for which this rate applies,
		// but we only have one, so this does not concern us

		usbFrameCnt = 0;	// reset idle counter

		// send an empty packet and ACK the request
		in0bc = 0x00;
		USB_EP0_HSNAK();
		
	} else {
		USB_EP0_STALL();
	}
}

void usbRequestReceived(void)
{
	uint8_t requestType = usbRequest.bmRequestType & 0x60;

	// reset the ep0 packetizer
	packetizer_data_ptr = 0;
	packetizer_data_size = 0;
	
	if (requestType == 0x00)		// standard request
	{
		uint8_t recipient = usbRequest.bmRequestType & 0x1f;

		if (recipient == 0)			// device
			usbStdDeviceRequest();
		else if (recipient == 1)	// interface
			usbStdInterfaceRequest();
		else if (recipient == 2)	// endpoint
			usbStdEndpointRequest();
		else
			USB_EP0_STALL();
			
	} else if (requestType == 0x20) {	// class request
		usbHidRequest();
	} else {
		// stall on unsupported requests
		USB_EP0_STALL();
	}
}

void usbRequestDataReceived(void)
{
	if (usbRequest.bRequest == USB_REQ_HID_SET_REPORT)
		on_set_report();		// size == usbReqHidGetSetReport.wLength
		
	// send an empty packet to ACK the data
	in0bc = 0x00;
	USB_EP0_HSNAK();
}

void usbPoll(void)
{
	if (!USBIRQ)
		return;

	// clear USB interrupt flag
	USBIRQ = 0;

	switch (ivec)
	{
	case INT_SUDAV:		// SETUP data packet
		usbirq = 0x01;	// clear interrupt flag

		usbRequestReceived();	// process setup data
		
		// arm the EP0 OUT in case we have data after the request
		out0bc = USB_EP0_SIZE;
		
		break;
	case INT_SOF:		// SOF packet
		usbirq = 0x02;	// clear interrupt flag
		++usbFrameCnt;
		break;
	/*
	case INT_SUTOK:		// setup token
		usbirq = 0x04;	// clear interrupt flag
		break;
		*/
	case INT_SUSPEND:	// SUSPEND signal
		usbirq = 0x08;	// clear interrupt flag
		break;
	case INT_USBRESET:	// USB bus reset
		usbirq = 0x10;	// clear interrupt flag
		usb_state = DEFAULT;	// reset internal states
		usb_current_config = 0;
		break;

	case INT_EP0IN:
		in_irq = 0x01;	// clear interrupt flag
		// update USB RAM EP0 IN with new data
		packetizer_isr_ep0_in();
		break;
		
	case INT_EP0OUT:
		out_irq = 0x01;	// clear interrupt flag
		usbRequestDataReceived();
		out0bc = USB_EP0_SIZE;	// rearm the next EP0 OUT
		break;

	case INT_EP1IN:
		in_irq = 0x02;	// clear interrupt flag
		out0bc = USB_EP0_SIZE;
		break;
	/*
	case INT_EP2IN:
		in_irq = 0x04;	// clear interrupt flag
		break;
		*/
	}
}
