#include "stdafx.h"

#include "resource.h"
#include "feature_reports.h"
#include "myutils.h"
#include "wht_device.h"
#include "wht_dialog.h"

BOOL CALLBACK WHTDialog::MyDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WHTDialog* pDlg = reinterpret_cast<WHTDialog*> (GetWindowLong(hDlg, GWL_USERDATA));

	switch (message)
	{
	case WM_INITDIALOG:
		
		pDlg = new WHTDialog(hDlg);
		return TRUE;

	case WM_COMMAND:

		if (pDlg)
			pDlg->OnCommand(LOWORD(wParam));

		return FALSE;

	case WM_CLOSE:

		::EndDialog(hDlg, 0);
		return TRUE;

	case WM_DESTROY:

		if (pDlg)
			delete pDlg;

		return FALSE;
	}

	return FALSE;
}

WHTDialog::WHTDialog(HWND hDlg)
	: hDialog(hDlg)
{
	// first save the dialog pointer to user data
	SetWindowLong(hDialog, GWL_USERDATA, reinterpret_cast<LONG>(this));

	// create the icon
	HICON hIconBig = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON,
						48, 48, LR_SHARED);

	SendMessage(hDialog, WM_SETICON, ICON_BIG, (LPARAM) hIconBig);

	HICON hIconSmall = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON,
						16, 16, LR_SHARED);
	SendMessage(hDialog, WM_SETICON, ICON_SMALL, (LPARAM) hIconSmall);
}

WHTDialog::~WHTDialog()
{
	// clear the dialog pointer
	SetWindowLong(hDialog, GWL_USERDATA, 0);
}

void WHTDialog::OnCommand(int ctrl_id)
{
	if (ctrl_id == IDC_BTN_CALIBRATE)
		::MessageBox(hDialog, L"Calibrate", L"Title", MB_OK | MB_ICONINFORMATION);
	else if (ctrl_id == IDC_BTN_SEND_TO_TRACKER) {
		SendConfigToDevice();
	} else if (ctrl_id == IDC_BTN_CONNECT) {
		if (!device.Open())
			::MessageBox(hDialog, L"Wireless head tracker dongle not found.", L"Error", MB_OK | MB_ICONERROR);

		ReadConfigFromDevice();

	} else if (ctrl_id == IDC_BTN_DISCONNECT) {
		device.Close();
	}
}

void WHTDialog::SetCtrlText(int ctrl_id, const std::wstring& text)
{
	SendMessage(GetDlgItem(hDialog, ctrl_id), WM_SETTEXT, 0, (LPARAM) text.c_str());
}

float WHTDialog::GetCtrlTextFloat(int ctrl_id)
{
	const int BUFF_SIZE = 256;
	wchar_t buff[BUFF_SIZE];
	SendMessage(GetDlgItem(hDialog, ctrl_id), WM_GETTEXT, BUFF_SIZE, (LPARAM) buff);

	return (float) _wtof(buff);
}

void WHTDialog::SetRadioState(int ctrl_id, bool new_state)
{
	SendMessage(GetDlgItem(hDialog, ctrl_id), BM_SETCHECK, (WPARAM) (new_state ? BST_CHECKED : BST_UNCHECKED), 0);
}

void WHTDialog::ReadConfigFromDevice()
{
	debug(L"reading FeatRep_AxisConfig");

	FeatRep_AxisConfig rep;
	rep.report_id = AXIS_CONFIG_REPORT_ID;
	device.GetFeatureReport((uint8_t*) &rep, sizeof(rep));

	SetRadioState(IDC_RDB_LINEAR, rep.is_linear != 0);
	SetRadioState(IDC_RDB_EXPONENTIAL, rep.is_linear == 0);

	SetCheckState(IDC_CHK_AUTOCENTER, rep.is_autocenter != 0);

	SetCtrlText(IDC_EDT_LIN_FACT_X, int2flt(rep.lin_fact_x));
	SetCtrlText(IDC_EDT_LIN_FACT_Y, int2flt(rep.lin_fact_y));
	SetCtrlText(IDC_EDT_LIN_FACT_Z, int2flt(rep.lin_fact_z));

	SetCtrlText(IDC_EDT_EXP_FACT_X, int2flt(rep.exp_fact_x));
	SetCtrlText(IDC_EDT_EXP_FACT_Y, int2flt(rep.exp_fact_y));
	SetCtrlText(IDC_EDT_EXP_FACT_Z, int2flt(rep.exp_fact_z));
}

void WHTDialog::SendConfigToDevice()
{
	debug(L"sending FeatRep_AxisConfig");

	FeatRep_AxisConfig rep;

	rep.is_linear = GetRadioState(IDC_RDB_LINEAR) ? 1 : 0;
	rep.is_autocenter = GetCheckState(IDC_CHK_AUTOCENTER) ? 1 : 0;

	rep.lin_fact_x = GetCtrlTextFloat(IDC_EDT_LIN_FACT_X);
	rep.lin_fact_y = GetCtrlTextFloat(IDC_EDT_LIN_FACT_Y);
	rep.lin_fact_z = GetCtrlTextFloat(IDC_EDT_LIN_FACT_Z);

	rep.exp_fact_x = GetCtrlTextFloat(IDC_EDT_EXP_FACT_X);
	rep.exp_fact_y = GetCtrlTextFloat(IDC_EDT_EXP_FACT_Y);
	rep.exp_fact_z = GetCtrlTextFloat(IDC_EDT_EXP_FACT_Z);

	rep.report_id = AXIS_CONFIG_REPORT_ID;
	if (!device.SetFeatureReport((uint8_t*) &rep, sizeof(rep)))
		::MessageBox(hDialog, L"Not sent!", L"Error", MB_OK | MB_ICONERROR);
}

void WHTDialog::ConnectedUI()
{
}

void WHTDialog::DisconnectedUI()
{
}
