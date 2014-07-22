#include <stdint.h>
#include <stdbool.h>

#include "usb.h"

__code const usb_dev_desc_t usb_dev_desc =
{
	sizeof(usb_dev_desc_t),
	USB_DESC_DEVICE, 
	0x0110,			// bcdUSB
	0,				// bDeviceClass		- deferred to interface
	0,				// bDeviceSubclass
	0,				// bDeviceProtocol
	USB_EP0_SIZE,	// bMaxPacketSize0
	0x40aa,			// idVendor			- some unknown vendor id
	0x9005,			// idProduct
	0x0001,			// bcdDevice
	1,				// iManufacturer
	2,				// iProduct
	3,				// iSerialNumber
	1,				// bNumConfigurations
};

__code const usb_conf_desc_joystick_t usb_conf_desc = 
{
	// configuration descriptor
	{
		sizeof(usb_conf_desc_t),
		USB_DESC_CONFIGURATION,
		sizeof(usb_conf_desc_joystick_t),
		2,		// bNumInterfaces
		1,		// bConfigurationValue
		2,		// iConfiguration
		0x80,	// bmAttributes - bus powered, no remote wakeup
		20,		// bMaxPower == 40mA
	},
	
	// joystick interface descriptor
	{
		sizeof(usb_if_desc_t),
		USB_DESC_INTERFACE,
		0,		// bInterfaceNumber
		0,		// bAlternateSetting
		1,		// bNumEndpoints
		3,		// bInterfaceClass		- HID
		0,		// bInterfaceSubClass
		0,		// bInterfaceProtocol
		0,		// iInterface
	},
	// HID descriptor
	{
		sizeof(usb_hid_desc_t),
		USB_DESC_HID,
		0x0111,							// bcdHID
		0,								// bCountryCode
		1,								// bNumDescriptors
		USB_DESC_HID_REPORT,			// bDescriptorType_HID
		USB_JOY_HID_REPORT_DESC_SIZE,	// wDescriptorLength
	},
	// endpoint descriptor EP1IN
	{
		sizeof(usb_ep_desc_t),
		USB_DESC_ENDPOINT,
		0x81,				// bEndpointAddress
		USB_EP_TYPE_INT,	// bmAttributes
		8,					// wMaxPacketSize
		10,					// bInterval  10ms
	},
	
	// control interface descriptor
	{
		sizeof(usb_if_desc_t),
		USB_DESC_INTERFACE,
		0,		// bInterfaceNumber
		0,		// bAlternateSetting
		1,		// bNumEndpoints
		3,		// bInterfaceClass		- HID
		0,		// bInterfaceSubClass
		0,		// bInterfaceProtocol
		0,		// iInterface
	},
	// HID descriptor
	{
		sizeof(usb_hid_desc_t),
		USB_DESC_HID,
		0x0111,							// bcdHID
		0,								// bCountryCode
		1,								// bNumDescriptors
		USB_DESC_HID_REPORT,			// bDescriptorType_HID
		USB_CTRL_HID_REPORT_DESC_SIZE,	// wDescriptorLength
	},
	// endpoint descriptor EP2IN
	{
		sizeof(usb_ep_desc_t),
		USB_DESC_ENDPOINT,
		0x82,				// bEndpointAddress
		USB_EP_TYPE_INT,	// bmAttributes
		64,					// wMaxPacketSize
		10,					// bInterval  10ms
	},	
};

const __code uint8_t usb_joystick_report_descriptor[USB_JOY_HID_REPORT_DESC_SIZE] =
{
	0x05, 0x01,			// USAGE_PAGE (Generic Desktop)
	0x09, 0x04,			// USAGE (Joystick)
	0xa1, 0x01,			// COLLECTION (Application)
	0x09, 0x01,			//   USAGE (Pointer)
	0xa1, 0x00,			//   COLLECTION (Physical)
	0x09, 0x30,			//     USAGE (X)
	0x09, 0x31,			//     USAGE (Y)
	0x09, 0x32,			//     USAGE (Z)
	0x16, 0x00, 0x80,	//     LOGICAL_MINIMUM (-32768)
	0x26, 0xff, 0x7f,	//     LOGICAL_MAXIMUM (32767)
	0x75, 0x10,			//     REPORT_SIZE (16)
	0x95, 0x03,			//     REPORT_COUNT (3)
	0x81, 0x82,			//     INPUT (Data,Var,Abs,Vol)
	0xc0,				//   END_COLLECTION
	0xc0				// END_COLLECTION
};

const __code uint8_t usb_control_report_descriptor[USB_CTRL_HID_REPORT_DESC_SIZE] =
{
	0x06, 0x00, 0xff,				// USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,						// USAGE (Vendor Usage 1)
	0xa1, 0x01,						// COLLECTION (Application)
	0x15, 0x00,						//	 LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,				//	 LOGICAL_MAXIMUM (255)
	0x75, 0x08,						//	 REPORT_SIZE (8)

	0x85, CTRL_REP_FIRST_ID,		//	 REPORT_ID
	0x95, CTRL_REP_FIRST_BYTES,		//	 REPORT_COUNT
	0x09, 0x00,						//	 USAGE (Undefined)
	0xb2, 0x02, 0x01,				//	 FEATURE (Data,Var,Abs,Buf)

	0x85, CTRL_REP_SECOND_ID,		//	 REPORT_ID
	0x95, CTRL_REP_SECOND_BYTES,	//	 REPORT_COUNT
	0x09, 0x00,						//	 USAGE (Undefined)
	0xb2, 0x02, 0x01,				//	 FEATURE (Data,Var,Abs,Buf)

	0xc0							// END_COLLECTION
};


// string table 
__code const uint8_t usb_string_desc_0[] = {0x04, 0x03, 0x09, 0x04};

__code const uint16_t usb_string_desc_1[] =
{
	0x0300 | sizeof(usb_string_desc_1),		// string descriptor ID and length
	'F','e','r','e','n','c',' ','S','z','i','l','i'
};

__code const uint16_t usb_string_desc_2[] =
{
	0x0300 | sizeof(usb_string_desc_2),		// string descriptor ID and length
	'E','D','T','r','a','c','k','e','r',' ','W','i','r','e','l','e','s','s'
};

__code const uint16_t usb_string_desc_3[] =
{
	0x0300 | sizeof(usb_string_desc_3),		// string descriptor ID and length
	'2','0','1','4','-','0','7','-','0','6'
};
