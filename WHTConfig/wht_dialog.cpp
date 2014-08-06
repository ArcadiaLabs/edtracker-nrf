#include "stdafx.h"

#include "resource.h"
#include "../dongle/reports.h"
#include "myutils.h"
#include "wht_device.h"
#include "wht_dialog.h"

#define WM_TRAYNOTIFY		(WM_APP+1)

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

	case WM_TIMER:

		if (pDlg)
			pDlg->OnTimer();

		return TRUE;

	case WM_SYSCOMMAND:

		if (pDlg  &&  wParam == SC_MINIMIZE)
		{
			pDlg->OnMinimize();
			return TRUE;
		}

		return FALSE;

	case WM_TRAYNOTIFY:

		if (pDlg)
			pDlg->OnTrayNotify(lParam);

		return TRUE;

	case WM_CLOSE:

		EndDialog(hDlg, 0);
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

	// create the icons
	hIconBig = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON,
						48, 48, LR_SHARED);

	SendMessage(hDialog, WM_SETICON, ICON_BIG, (LPARAM) hIconBig);

	hIconSmall = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON,
						16, 16, LR_SHARED);
	SendMessage(hDialog, WM_SETICON, ICON_SMALL, (LPARAM) hIconSmall);

	DisconnectedUI();

	SendMessage(GetDlgItem(hDialog, IDC_PRG_AXIS_X), PBM_SETRANGE, 0, MAKELPARAM(0, 0xffff));
	SendMessage(GetDlgItem(hDialog, IDC_PRG_AXIS_Y), PBM_SETRANGE, 0, MAKELPARAM(0, 0xffff));
	SendMessage(GetDlgItem(hDialog, IDC_PRG_AXIS_Z), PBM_SETRANGE, 0, MAKELPARAM(0, 0xffff));

	// start the timer
	SetTimer(hDialog, 1, 100, NULL);

	InitStatusbar();

	// SendMessage(this->get_hwnd(), SB_SETTEXT, part, (LPARAM) text.c_str());
}

WHTDialog::~WHTDialog()
{
	// clear the dialog pointer
	SetWindowLong(hDialog, GWL_USERDATA, 0);
}

void WHTDialog::OnCommand(int ctrl_id)
{
	if (ctrl_id == IDC_BTN_CALIBRATE)
	{
		FeatRep_Command rep;
		rep.report_id = COMMAND_REPORT_ID;
		rep.command = CMD_CALIBRATE;
		if (!device.SetFeatureReport((uint8_t*) &rep, sizeof(rep)))
			MessageBox(hDialog, L"Unable to send data to dongle.", L"Error", MB_OK | MB_ICONERROR);

	} else if (ctrl_id == IDC_BTN_SEND_TO_TRACKER) {
		SendConfigToDevice();
	} else if (ctrl_id == IDC_BTN_CONNECT) {
		if (!device.Open())
		{
			MessageBox(hDialog, L"Wireless head tracker dongle not found.", L"Error", MB_OK | MB_ICONERROR);
		} else {
			ConnectedUI();
			ReadConfigFromDevice();
			ReadCalibrationData();
		}

	} else if (ctrl_id == IDC_BTN_DISCONNECT) {
		device.Close();
		DisconnectedUI();
	}
}

void WHTDialog::OnTimer()
{
	// SetStatusbarText(0, int2str(GetTickCount()));
	hid_joystick_report_t rep;
	rep.report_id = JOYSTICK_REPORT_ID;
	if (device.GetInputReport((uint8_t*) &rep, sizeof(rep)))
	{
		SendMessage(GetDlgItem(hDialog, IDC_PRG_AXIS_X), PBM_SETPOS, (WPARAM) rep.x + 32768, 0);
		SendMessage(GetDlgItem(hDialog, IDC_PRG_AXIS_Y), PBM_SETPOS, (WPARAM) rep.y + 32768, 0);
		SendMessage(GetDlgItem(hDialog, IDC_PRG_AXIS_Z), PBM_SETPOS, (WPARAM) rep.z + 32768, 0);
	}
}

void WHTDialog::OnMinimize()
{
	CreateTrayIcon();
	Hide();
}

void WHTDialog::OnTrayNotify(LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN)
	{
		Show();
		RemoveTrayIcon();
	}
}

void WHTDialog::InitStatusbar()
{
	// make the status bar parts
	const int NUM_PARTS = 5;
	int parts[NUM_PARTS];
	parts[0] = 80;
	parts[1] = 160;
	parts[2] = 240;
	parts[3] = 320;
	parts[4] = -1;

	SendMessage(GetDlgItem(hDialog, IDC_STATUS_BAR), SB_SETPARTS, NUM_PARTS, (LPARAM) parts);
}

void WHTDialog::SetStatusbarText(int part, const std::wstring& text)
{
	SendMessage(GetDlgItem(hDialog, IDC_STATUS_BAR), SB_SETTEXT, part, (LPARAM) text.c_str());
}

void WHTDialog::CreateTrayIcon()
{
	NOTIFYICONDATA nid;
 
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hDialog;
	nid.uID = 100;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_TRAYNOTIFY;
	nid.hIcon = hIconSmall;
	wcscpy_s(nid.szTip, L"Wireless Head Tracker");
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
 
	Shell_NotifyIcon(NIM_ADD, &nid);
}

void WHTDialog::RemoveTrayIcon()
{
	NOTIFYICONDATA nid;
 
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hDialog;
	nid.uID = 100;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_TRAYNOTIFY;
	nid.hIcon = hIconSmall;
	wcscpy_s(nid.szTip, L"Wireless Head Tracker");
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
 
	Shell_NotifyIcon(NIM_DELETE, &nid);
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
	if (!device.GetFeatureReport((uint8_t*) &rep, sizeof(rep)))
	{
		MessageBox(hDialog, L"Unable to read from dongle.\n", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	SetRadioState(IDC_RDB_LINEAR, rep.is_linear != 0);
	SetRadioState(IDC_RDB_EXPONENTIAL, rep.is_linear == 0);

	SetCheckState(IDC_CHK_SELFCENTER, rep.is_selfcenter != 0);

	SetCtrlText(IDC_EDT_LIN_FACT_X, flt2str(rep.lin_fact_x));
	SetCtrlText(IDC_EDT_LIN_FACT_Y, flt2str(rep.lin_fact_y));
	SetCtrlText(IDC_EDT_LIN_FACT_Z, flt2str(rep.lin_fact_z));

	SetCtrlText(IDC_EDT_EXP_FACT_X, flt2str(rep.exp_fact_x));
	SetCtrlText(IDC_EDT_EXP_FACT_Y, flt2str(rep.exp_fact_y));
	SetCtrlText(IDC_EDT_EXP_FACT_Z, flt2str(rep.exp_fact_z));
}

void WHTDialog::ReadCalibrationData()
{
	debug(L"reading FeatRep_CalibrationData");

	ClearCtrlText(IDC_CALIB_STATUS);
	ClearCtrlText(IDC_GYRO_BIAS_X);
	ClearCtrlText(IDC_GYRO_BIAS_Y);
	ClearCtrlText(IDC_GYRO_BIAS_Z);
	ClearCtrlText(IDC_ACCEL_BIAS_X);
	ClearCtrlText(IDC_ACCEL_BIAS_Y);
	ClearCtrlText(IDC_ACCEL_BIAS_Z);

	FeatRep_CalibrationData rep;
	rep.report_id = CALIBRATION_DATA_REPORT_ID;
	if (!device.GetFeatureReport((uint8_t*) &rep, sizeof(rep)))
	{
		MessageBox(hDialog, L"Unable to read calibration data.\n", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	if (rep.has_tracker_responded == 0)
	{
		SetCtrlText(IDC_CALIB_STATUS, L"Tracker not found");
	} else {
		SetCtrlText(IDC_CALIB_STATUS, rep.is_calibrated ? L"Calibrated" : L"Not calibrated");

		SetCtrlText(IDC_GYRO_BIAS_X, int2str(rep.gyro_bias[0]));
		SetCtrlText(IDC_GYRO_BIAS_Y, int2str(rep.gyro_bias[1]));
		SetCtrlText(IDC_GYRO_BIAS_Z, int2str(rep.gyro_bias[2]));

		SetCtrlText(IDC_ACCEL_BIAS_X, int2str(rep.accel_bias[0]));
		SetCtrlText(IDC_ACCEL_BIAS_Y, int2str(rep.accel_bias[1]));
		SetCtrlText(IDC_ACCEL_BIAS_Z, int2str(rep.accel_bias[2]));
	}
}

void WHTDialog::SendConfigToDevice()
{
	debug(L"sending FeatRep_AxisConfig");

	FeatRep_AxisConfig rep;

	rep.is_linear = GetRadioState(IDC_RDB_LINEAR) ? 1 : 0;
	rep.is_selfcenter = GetCheckState(IDC_CHK_SELFCENTER) ? 1 : 0;

	rep.lin_fact_x = GetCtrlTextFloat(IDC_EDT_LIN_FACT_X);
	rep.lin_fact_y = GetCtrlTextFloat(IDC_EDT_LIN_FACT_Y);
	rep.lin_fact_z = GetCtrlTextFloat(IDC_EDT_LIN_FACT_Z);

	rep.exp_fact_x = GetCtrlTextFloat(IDC_EDT_EXP_FACT_X);
	rep.exp_fact_y = GetCtrlTextFloat(IDC_EDT_EXP_FACT_Y);
	rep.exp_fact_z = GetCtrlTextFloat(IDC_EDT_EXP_FACT_Z);

	rep.report_id = AXIS_CONFIG_REPORT_ID;
	if (!device.SetFeatureReport((uint8_t*) &rep, sizeof(rep)))
		MessageBox(hDialog, L"Not sent!", L"Error", MB_OK | MB_ICONERROR);
}

void WHTDialog::ConnectedUI()
{
    EnableWindow(GetDlgItem(hDialog, IDC_BTN_CONNECT), FALSE);
    EnableWindow(GetDlgItem(hDialog, IDC_BTN_DISCONNECT), TRUE);

	EnableWindow(GetDlgItem(hDialog, IDC_BTN_CALIBRATE), TRUE);
	EnableWindow(GetDlgItem(hDialog, IDC_BTN_SEND_TO_TRACKER), TRUE);
	EnableWindow(GetDlgItem(hDialog, IDC_RDB_LINEAR), TRUE);
    EnableWindow(GetDlgItem(hDialog, IDC_RDB_EXPONENTIAL), TRUE);
	EnableWindow(GetDlgItem(hDialog, IDC_CHK_SELFCENTER), TRUE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_LIN_FACT_X), TRUE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_LIN_FACT_Y), TRUE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_LIN_FACT_Z), TRUE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_EXP_FACT_X), TRUE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_EXP_FACT_Y), TRUE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_EXP_FACT_Z), TRUE);
}

void WHTDialog::DisconnectedUI()
{
    EnableWindow(GetDlgItem(hDialog, IDC_BTN_CONNECT), TRUE);
    EnableWindow(GetDlgItem(hDialog, IDC_BTN_DISCONNECT), FALSE);

	EnableWindow(GetDlgItem(hDialog, IDC_BTN_CALIBRATE), FALSE);
	EnableWindow(GetDlgItem(hDialog, IDC_BTN_SEND_TO_TRACKER), FALSE);
	EnableWindow(GetDlgItem(hDialog, IDC_RDB_LINEAR), FALSE);
    EnableWindow(GetDlgItem(hDialog, IDC_RDB_EXPONENTIAL), FALSE);
	EnableWindow(GetDlgItem(hDialog, IDC_CHK_SELFCENTER), FALSE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_LIN_FACT_X), FALSE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_LIN_FACT_Y), FALSE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_LIN_FACT_Z), FALSE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_EXP_FACT_X), FALSE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_EXP_FACT_Y), FALSE);
	EnableWindow(GetDlgItem(hDialog, IDC_EDT_EXP_FACT_Z), FALSE);
}
