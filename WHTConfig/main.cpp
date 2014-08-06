#include "stdafx.h"

#include "resource.h"
#include "hid.h"
#include "wht_device.h"
#include "myutils.h"
#include "wht_dialog.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	if (!InitHID())
		return -1;

	CoInitialize(0);

	InitCommonControls();	// for the progress bar

	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), 0, (DLGPROC) WHTDialog::MyDlgProc, 0);

	CoUninitialize();

	return 0;
}