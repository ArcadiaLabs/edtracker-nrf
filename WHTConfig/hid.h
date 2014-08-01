#pragma once

// If you don't want to install the entire Windows DDK, leave this uncommented.
// We define only the stuff we need from the DDK bellow mad load it dynamically from the DLLs.
// This is very dirty, but hey - tit for tat ;)

//#define BUILD_DDK

#ifdef BUILD_DDK
// get the HID stuff
extern "C"
{
	#include <setupapi.h>
#ifdef __GNUC__
	#include <ddk/hidsdi.h>
#else
	#include <api/hidsdi.h>
	#pragma comment(lib, "hid.lib")
	#pragma comment(lib, "setupapi.lib")
#endif
	#include <ddk/hidclass.h>
}
#else
typedef PVOID HDEVINFO;

typedef struct _SP_DEVICE_INTERFACE_DATA
{
    DWORD cbSize;
    GUID  InterfaceClassGuid;
    DWORD Flags;
    ULONG_PTR Reserved;
} SP_DEVICE_INTERFACE_DATA, *PSP_DEVICE_INTERFACE_DATA;

typedef struct _SP_DEVICE_INTERFACE_DETAIL_DATA_A
{
	DWORD	cbSize;
	WCHAR	DevicePath[ANYSIZE_ARRAY];
} SP_DEVICE_INTERFACE_DETAIL_DATA_A, *PSP_DEVICE_INTERFACE_DETAIL_DATA_A;

typedef SP_DEVICE_INTERFACE_DETAIL_DATA_A SP_DEVICE_INTERFACE_DETAIL_DATA;

typedef struct _HIDD_ATTRIBUTES {
    ULONG   Size; // = sizeof (struct _HIDD_ATTRIBUTES)

    //
    // Vendor ids of this hid device
    //
    USHORT  VendorID;
    USHORT  ProductID;
    USHORT  VersionNumber;

    //
    // Additional fields will be added to the end of this structure.
    //
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

extern void (__stdcall *HidD_GetHidGuid) (LPGUID HidGuid);

#define WINSETUPAPI DECLSPEC_IMPORT

extern HDEVINFO (WINAPI *SetupDiGetClassDevsW)(CONST GUID *ClassGuid, PWSTR Enumerator, HWND hwndParent, DWORD Flags);

//
// Flags controlling what is included in the device information set built
// by SetupDiGetClassDevs
//
#define DIGCF_DEFAULT           0x00000001  // only valid with DIGCF_DEVICEINTERFACE
#define DIGCF_PRESENT           0x00000002
#define DIGCF_ALLCLASSES        0x00000004
#define DIGCF_PROFILE           0x00000008
#define DIGCF_DEVICEINTERFACE   0x00000010

typedef struct _SP_DEVINFO_DATA {
    DWORD cbSize;
    GUID  ClassGuid;
    DWORD DevInst;    // DEVINST handle
    ULONG_PTR Reserved;
} SP_DEVINFO_DATA, *PSP_DEVINFO_DATA;

extern BOOL (WINAPI *SetupDiEnumDeviceInterfaces) (HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, CONST GUID *InterfaceClassGuid, DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);
extern BOOL (WINAPI *SetupDiDestroyDeviceInfoList) (HDEVINFO DeviceInfoSet);

extern BOOL (WINAPI *SetupDiGetDeviceInterfaceDetailW) (HDEVINFO DeviceInfoSet,
	PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
	PSP_DEVICE_INTERFACE_DETAIL_DATA_A DeviceInterfaceDetailData, 
	DWORD DeviceInterfaceDetailDataSize,
	PDWORD RequiredSize,
	PSP_DEVINFO_DATA DeviceInfoData);

extern BOOLEAN (__stdcall *HidD_GetAttributes) (HANDLE HidDeviceObject, PHIDD_ATTRIBUTES Attributes);
extern BOOLEAN (__stdcall *HidD_GetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);
extern BOOLEAN (__stdcall *HidD_SetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);

#endif
