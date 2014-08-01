#include "stdafx.h"

#include "hid.h"

#ifndef BUILD_DDK

void (__stdcall *HidD_GetHidGuid) (LPGUID HidGuid) = 0;
HDEVINFO (WINAPI *SetupDiGetClassDevsW)(CONST GUID *ClassGuid, PWSTR Enumerator, HWND hwndParent, DWORD Flags) = 0;
BOOL (WINAPI *SetupDiEnumDeviceInterfaces) (HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, CONST GUID *InterfaceClassGuid, DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData) = 0;
BOOL (WINAPI *SetupDiDestroyDeviceInfoList) (HDEVINFO DeviceInfoSet) = 0;
BOOL (WINAPI *SetupDiGetDeviceInterfaceDetailW) (HDEVINFO DeviceInfoSet,
	PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
	PSP_DEVICE_INTERFACE_DETAIL_DATA_A DeviceInterfaceDetailData, 
	DWORD DeviceInterfaceDetailDataSize,
	PDWORD RequiredSize,
	PSP_DEVINFO_DATA DeviceInfoData) = 0;

BOOLEAN (__stdcall *HidD_GetAttributes) (HANDLE HidDeviceObject, PHIDD_ATTRIBUTES Attributes) = 0;
BOOLEAN (__stdcall *HidD_GetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength) = 0;
BOOLEAN (__stdcall *HidD_SetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength) = 0;

#endif