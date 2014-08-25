#pragma once

class WHTDevice
{
private:
	HANDLE	hDevice;

	bool GetFeatureReportRaw(uint8_t* buffer, int report_size);
	bool SetFeatureReportRaw(const uint8_t* buffer, int report_size);
	bool GetInputReportRaw(uint8_t* buffer, int report_size);

public:
	WHTDevice();
	~WHTDevice();

	bool Open();
	void Close();

	bool IsOpen() const
	{
		return hDevice != NULL;
	}
	
	template <class T>
	bool GetFeatureReport(T& t)
	{
		return GetFeatureReportRaw((uint8_t*) &t, sizeof(t));
	}

	template <class T>
	bool SetFeatureReport(const T& t)
	{
		return SetFeatureReportRaw((const uint8_t*) &t, sizeof(t));
	}

	template <class T>
	bool GetInputReport(T& t)
	{
		return GetInputReportRaw((uint8_t*) &t, sizeof(t));
	}
};
