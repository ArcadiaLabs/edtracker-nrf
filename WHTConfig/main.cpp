#include "stdafx.h"

#include "resource.h"
#include "dllwrap.h"
#include "hid.h"
#include "wht_device.h"

WHTDevice dev;

void InitDialog(HWND hDlg)
{
	// create the icon
	HICON hIconBig = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON,
						48, 48, LR_SHARED);

	SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM) hIconBig);

	HICON hIconSmall = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON,
						16, 16, LR_SHARED);
	SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM) hIconSmall);
}

BOOL CALLBACK MyDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int ctrl_id;
	//HWND hCtrl;

	switch (message)
	{
	case WM_INITDIALOG:
		
		InitDialog(hDlg);
		return TRUE;

	case WM_COMMAND:

		ctrl_id = LOWORD(wParam);	// get the control ID
		//hCtrl	= (HWND) lParam;

		if (ctrl_id == IDC_BTN_CALIBRATE)
			::MessageBox(hDlg, L"Calibrate", L"Title", MB_OK | MB_ICONINFORMATION);
		else if (ctrl_id == IDC_BTN_SEND_TO_TRACKER) {
			uint8_t buff[31];
			memset(buff, 0, sizeof buff);
			dev.SetFeatureReport(buff, sizeof(buff), 2);
		} else if (ctrl_id == IDC_BTN_CONNECT) {
			if (!dev.Open())
				::MessageBox(hDlg, L"Wireless head tracker dongle not found.", L"Error", MB_OK | MB_ICONERROR);

		} else if (ctrl_id == IDC_BTN_DISCONNECT) {
			dev.Close();
		}

		return FALSE;

	case WM_CLOSE:

		::EndDialog(hDlg, 0);
		return TRUE;

	case WM_DESTROY:
		/*
		{
			// save the settings

			// get the exe path
			std::string ini_filename(GetIniFileName());

			char buffer[MAX_PATH];

			ComboBox_GetText(GetDlgItem(hDlg, IDC_MCU), buffer, MAX_PATH);
			WritePrivateProfileString("Settings", "MCU", buffer, ini_filename.c_str());

			LRESULT bytes = SendMessage(GetDlgItem(hDlg, IDC_HEX_FILE), WM_GETTEXT, MAX_PATH, (LPARAM) buffer);
			WritePrivateProfileString("Settings", "HEXFile", buffer, ini_filename.c_str());

			WritePrivateProfileString("Settings", "WaitForDevice", IsDlgButtonChecked(hDlg, IDC_WAIT_FOR_DEVICE) == BST_CHECKED ? "1" : "0", ini_filename.c_str());
			WritePrivateProfileString("Settings", "RebootAfterProgramming", IsDlgButtonChecked(hDlg, IDC_REBOOT_AFTER_PROG) == BST_CHECKED ? "1" : "0", ini_filename.c_str());
		}
		*/

		return FALSE;
	}

	return FALSE;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// load the DLLs
	dll hiddll;
		
	if (!hiddll.load(L"hid.dll"))
	{
		::MessageBox(0, L"Unable to load hid.dll", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	// get the function from the loaded DLLs
	hiddll.get_proc(HidD_GetHidGuid, "HidD_GetHidGuid");
	hiddll.get_proc(HidD_GetAttributes, "HidD_GetAttributes");
	hiddll.get_proc(HidD_GetFeature, "HidD_GetFeature");
	hiddll.get_proc(HidD_SetFeature, "HidD_SetFeature");

	if (!HidD_GetHidGuid  ||  !HidD_GetAttributes  ||  !HidD_GetFeature  ||  !HidD_SetFeature)
	{
		::MessageBox(0, L"Unable to load function from DLLs.", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	CoInitialize(0);		// GetOpenFileName needs this

	InitCommonControls();	// for the progress bar

	// start the dialog
	/*INT_PTR result =*/ DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), 0, (DLGPROC) MyDlgProc, 0);

	CoUninitialize();

	return 0;
}