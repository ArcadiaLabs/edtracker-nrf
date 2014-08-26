#pragma once

class WHTDevice
{
private:
	HANDLE	hDevice;

	void GetFeatureReportRaw(void* buffer, int report_size);
	void SetFeatureReportRaw(const void* buffer, int report_size);
	void GetInputReportRaw(void* buffer, int report_size);

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
		GetFeatureReportRaw(&t, sizeof(t));
	}

	template <class T>
	void SetFeatureReport(const T& t)
	{
		SetFeatureReportRaw(&t, sizeof(t));
	}

	template <class T>
	void GetInputReport(T& t)
	{
		GetInputReportRaw(&t, sizeof(t));
	}
};
