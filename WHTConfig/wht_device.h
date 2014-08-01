#pragma once

class WHTDevice
{
public:
	HANDLE	hDevice;
	HANDLE	hWriteEvent;

public:
	WHTDevice();
	~WHTDevice();

	bool Open();
	void Close();
	
	bool GetFeatureReport(uint8_t* buffer, int report_size, uint8_t report_id);
	bool SetFeatureReport(const uint8_t* buffer, int bytes, int report_size, uint8_t report_id);
};