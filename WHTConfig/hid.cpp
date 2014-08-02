#include "stdafx.h"

#include "hid.h"

#ifndef BUILD_DDK

void (__stdcall *HidD_GetHidGuid) (LPGUID HidGuid) = 0;
BOOLEAN (__stdcall *HidD_GetAttributes) (HANDLE HidDeviceObject, PHIDD_ATTRIBUTES Attributes) = 0;
BOOLEAN (__stdcall *HidD_GetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength) = 0;
BOOLEAN (__stdcall *HidD_SetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength) = 0;

#endif