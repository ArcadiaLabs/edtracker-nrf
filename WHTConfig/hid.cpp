#include "stdafx.h"

#include "hid.h"
#include "dllwrap.h"

dll hiddll;

void (__stdcall *HidD_GetHidGuid) (LPGUID HidGuid) = 0;
BOOLEAN (__stdcall *HidD_GetAttributes) (HANDLE HidDeviceObject, PHIDD_ATTRIBUTES Attributes) = 0;
BOOLEAN (__stdcall *HidD_GetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength) = 0;
BOOLEAN (__stdcall *HidD_SetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength) = 0;
BOOLEAN (__stdcall *HidD_GetInputReport) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength) = 0;

bool InitHID()
{
	if (!hiddll.load(L"hid.dll"))
	{
		::MessageBox(0, L"Unable to load hid.dll", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// get the function from the loaded DLLs
	hiddll.get_proc(HidD_GetHidGuid, "HidD_GetHidGuid");
	hiddll.get_proc(HidD_GetAttributes, "HidD_GetAttributes");
	hiddll.get_proc(HidD_GetFeature, "HidD_GetFeature");
	hiddll.get_proc(HidD_SetFeature, "HidD_SetFeature");
	hiddll.get_proc(HidD_GetInputReport, "HidD_GetInputReport");

	if (!HidD_GetHidGuid  ||  !HidD_GetAttributes  ||  !HidD_GetFeature  ||  !HidD_SetFeature  ||  !HidD_GetInputReport)
	{
		::MessageBox(0, L"Unable to load all necessary function from hid.dll", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}