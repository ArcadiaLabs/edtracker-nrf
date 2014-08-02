#include "stdafx.h"

#include "wht_device.h"
#include "hid.h"
#include "debug.h"

#define VENDOR_ID	0x40AA
#define PRODUCT_ID	0x9007

WHTDevice::WHTDevice()
	: hDevice(NULL), hWriteEvent(NULL)
{}

WHTDevice::~WHTDevice()
{
	Close();
}

bool WHTDevice::Open()
{
	Close();

	hDevice = NULL;

	GUID guid;
	HDEVINFO info;
	DWORD index, required_size;
	SP_DEVICE_INTERFACE_DATA iface;
	SP_DEVICE_INTERFACE_DETAIL_DATA* details;
	HIDD_ATTRIBUTES attrib;
	HANDLE h;
	BOOL ret;

	HidD_GetHidGuid(&guid);
	info = SetupDiGetClassDevsW(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (info == INVALID_HANDLE_VALUE)
		return false;

	for (index = 0; 1; index++)
	{
		debug(index);

		iface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		if (!SetupDiEnumDeviceInterfaces(info, NULL, &guid, index, &iface))
		{
			SetupDiDestroyDeviceInfoList(info);
			break;
		}

		SetupDiGetDeviceInterfaceDetailW(info, &iface, NULL, 0, &required_size, NULL);
		required_size *= 3;
		details = (SP_DEVICE_INTERFACE_DETAIL_DATA*) malloc(required_size);
		if (details == NULL)
			continue;


		memset(details, 0, required_size);
		details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		ret = SetupDiGetDeviceInterfaceDetailW(info, &iface, details, required_size, NULL, NULL);
		if (!ret)
		{
			debug(::GetLastError());
			free(details);
			continue;
		}

		h = CreateFile(details->DevicePath, GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
						FILE_FLAG_OVERLAPPED, NULL);
		free(details);
		if (h == INVALID_HANDLE_VALUE)
			continue;
		attrib.Size = sizeof(HIDD_ATTRIBUTES);
		ret = HidD_GetAttributes(h, &attrib);
		if (!ret)
		{
			CloseHandle(h);
			continue;
		}
		
		if (attrib.VendorID != VENDOR_ID  ||  attrib.ProductID != PRODUCT_ID)
		{
			CloseHandle(h);
			continue;
		}

		SetupDiDestroyDeviceInfoList(info);
		hDevice = h;
	}

	if (!hDevice)
		return false;

	return true;
}

void WHTDevice::Close()
{
	if (!hDevice)
		return;

	CloseHandle(hDevice);

	hDevice = NULL;
}

bool WHTDevice::GetFeatureReport(uint8_t* buffer, int report_size, uint8_t report_id)
{
	const int buff_size = report_size + 1;
	std::auto_ptr<uint8_t> rcvBuff(new uint8_t(buff_size));
	rcvBuff.get()[0] = report_id;
	if (HidD_GetFeature(hDevice, buffer, buff_size) == 0)
		//throw std::string("Unable to read data from HID device");
		return false;

	memcpy(buffer, rcvBuff.get() + 1, report_size);
	
#ifdef LOG_HID_TRAFFIC
	printf("-- Rep ");
	for (int c = 0; c < report_size; ++c)
		printf("%02x ", buffer[c]);
	printf("\n");
#endif	

	return true;
}

bool WHTDevice::SetFeatureReport(const uint8_t* buffer, int bytes, int report_size, uint8_t report_id)
{
#ifdef LOG_HID_TRAFFIC
	printf("-- W  ");
	for (int c = 0; c < bytes; ++c)
		printf("%02x ", buffer[c]);
	printf("\n");
#endif	

	// alloc and clear the buffer
	int buff_size = report_size + 1;
	std::auto_ptr<uint8_t> sendBuff(new uint8_t(buff_size));
	memset(sendBuff.get(), 0, buff_size);

	// set the report ID and the data
	sendBuff.get()[0] = report_id;
	memcpy(sendBuff.get() + 1, buffer, bytes);
		
	return HidD_SetFeature(hDevice, sendBuff.get(), buff_size) == TRUE;
}
