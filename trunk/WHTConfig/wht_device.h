#pragma once

class WHTDevice
{
private:
	HANDLE	hDevice;

	void GetFeatureReportRaw(uint8_t* buffer, int report_size);
	void SetFeatureReportRaw(const uint8_t* buffer, int report_size);
	void GetInputReportRaw(uint8_t* buffer, int report_size);

	void ThrowException(const wchar_t* during, int report_id);

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
	void GetFeatureReport(T& t)
	{
		GetFeatureReportRaw((uint8_t*) &t, sizeof(t));
	}

	template <class T>
	void SetFeatureReport(const T& t)
	{
		SetFeatureReportRaw((const uint8_t*) &t, sizeof(t));
	}

	template <class T>
	void GetInputReport(T& t)
	{
		GetInputReportRaw((uint8_t*) &t, sizeof(t));
	}
};
