#pragma once

class WHTDevice
{
public:
	HANDLE	hDevice;

public:
	WHTDevice();
	~WHTDevice();

	bool Open();
	void Close();

	bool IsOpen() const
	{
		return hDevice != NULL;
	}
	
	bool GetFeatureReport(uint8_t* buffer, int report_size);
	bool SetFeatureReport(const uint8_t* buffer, int report_size);
	bool GetInputReport(uint8_t* buffer, int report_size);
};
