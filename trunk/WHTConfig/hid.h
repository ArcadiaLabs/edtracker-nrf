#pragma once

// We define only the stuff we need from the DDK bellow and load it dynamically from the DLLs.
// This is dirty, of course, but it beats installing the entire Windows DDK because of only 5 function

#include <setupapi.h>

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
extern BOOLEAN (__stdcall *HidD_GetAttributes) (HANDLE HidDeviceObject, PHIDD_ATTRIBUTES Attributes);
extern BOOLEAN (__stdcall *HidD_GetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);
extern BOOLEAN (__stdcall *HidD_SetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);
extern BOOLEAN (__stdcall *HidD_GetInputReport) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);

bool InitHID();
