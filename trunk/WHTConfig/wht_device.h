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
	
	bool GetFeatureReport(uint8_t* buffer, int report_size, uint8_t report_id);
	bool SetFeatureReport(const uint8_t* buffer, int report_size, uint8_t report_id);
	bool GetInputReport(uint8_t* buffer, int report_size, uint8_t report_id);
};