#include "stdafx.h"

#include "resource.h"
#include "../dongle/reports.h"
#include "myutils.h"
#include "wht_device.h"
#include "wht_dialog.h"

#define WM_TRAYNOTIFY		(WM_APP+1)

#define WIDEN2(x)		L ## x
#define WIDEN(x)		WIDEN2(x)

// status bar "parts"
#define STATBAR_RF_STATUS	0
#define STATBAR_MISC		1
#define STATBAR_VERSION		2

BOOL CALLBACK WHTDialog::MyDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WHTDialog* pDlg = reinterpret_cast<WHTDialog*> (GetWindowLong(hDlg, GWL_USERDATA));

	if (message == WM_INITDIALOG)
	{
		new WHTDialog(hDlg);
		return TRUE;
	}
	
	if (message == WM_DESTROY  &&  pDlg)
	{
		delete pDlg;
		pDlg = NULL;
	}

	if (pDlg)
		return pDlg->OnMessage(message, wParam, lParam);

	return FALSE;
}

WHTDialog::WHTDialog(HWND hDlg)
	: hDialog(hDlg)
{
	// first save the dialog pointer to user data
	SetWindowLong(hDialog, GWL_USERDATA, reinterpret_cast<LONG>(this));

	// create the icons
	hIconBig = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 48, 48, LR_SHARED);
	SendMessage(hDialog, WM_SETICON, ICON_BIG, (LPARAM) hIconBig);

	hIconSmall = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_SHARED);
	SendMessage(hDialog, WM_SETICON, ICON_SMALL, (LPARAM) hIconSmall);

	// setup the progress bar ranges
	SendMessage(GetCtrl(IDC_PRG_AXIS_X), PBM_SETRANGE, 0, MAKELPARAM(0, 0xffff));
	SendMessage(GetCtrl(IDC_PRG_AXIS_Y), PBM_SETRANGE, 0, MAKELPARAM(0, 0xffff));
	SendMessage(GetCtrl(IDC_PRG_AXIS_Z), PBM_SETRANGE, 0, MAKELPARAM(0, 0xffff));

	// start the timer
	SetTimer(hDialog, 1, 100, NULL);	// 100ms which is 10 Hz

	// init the the axis response combo box
	AddComboString(IDC_CMB_AXIS_RESPONSE, L"Exponential");
	AddComboString(IDC_CMB_AXIS_RESPONSE, L"Linear");
	SetComboSelection(IDC_CMB_AXIS_RESPONSE, 0);		// select exponential

	// disable the controls
	ChangeConnectedStateUI(false);

	// setup the status bar
	InitStatusbar();

	SetStatusbarText(STATBAR_VERSION, L"Build: " WIDEN(__DATE__) L" " WIDEN(__TIME__));
}

WHTDialog::~WHTDialog()
{
	// clear the dialog pointer
	SetWindowLong(hDialog, GWL_USERDATA, 0);
}

BOOL WHTDialog::OnMessage(int message, WPARAM wParam, LPARAM lParam)
{
	try {
		switch (message)
		{
		case WM_COMMAND:

			OnCommand(LOWORD(wParam));
			return FALSE;

		case WM_TIMER:

			OnTimer();
			return TRUE;

		case WM_SYSCOMMAND:

			if (wParam == SC_MINIMIZE)
			{
				// minimize to tray
				OnMinimize();
				return TRUE;
			}

			return FALSE;

		case WM_TRAYNOTIFY:

			OnTrayNotify(lParam);
			return TRUE;

		case WM_CLOSE:

			EndDialog(hDialog, 0);
			return TRUE;
		}

	} catch (std::wstring& e) {

		// close the HID device
		device.Close();

		// change the UI
		ChangeConnectedStateUI(false);

		MessageBox(hDialog, e.c_str(), L"Exception", MB_OK | MB_ICONERROR);
	}

	return FALSE;
}

void WHTDialog::OnCommand(int ctrl_id)
{
	if (ctrl_id == IDC_BTN_CALIBRATE)
	{
		FeatRep_Command rep;
		rep.report_id = COMMAND_REPORT_ID;
		rep.command = CMD_CALIBRATE;
		device.SetFeatureReport(rep);

	} else if (ctrl_id == IDC_BTN_SEND_TO_TRACKER) {
		SendConfigToDevice();
	} else if (ctrl_id == IDC_BTN_CONNECT) {
		if (device.IsOpen())
		{
			device.Close();
			ChangeConnectedStateUI(false);
		} else {
			if (!device.Open())
			{
				MessageBox(hDialog, L"Wireless head tracker dongle not found.", L"Error", MB_OK | MB_ICONERROR);
			} else {
				ReadConfigFromDevice();
				ReadCalibrationData();
				ChangeConnectedStateUI(true);
			}
		}

	} else if (ctrl_id == IDC_BTN_READ_CALIBRATION) {
		ReadCalibrationData();

	} else if (ctrl_id == IDC_BTN_RESET_DRIFT_COMP) {
		
		FeatRep_Command rep;
		rep.report_id = COMMAND_REPORT_ID;
		rep.command = CMD_RESET_DRIFT;
		device.SetFeatureReport(rep);

	} else if (ctrl_id == IDC_BTN_SAVE_DRIFT_COMP) {

		FeatRep_Command rep;
		rep.report_id = COMMAND_REPORT_ID;
		rep.command = CMD_SAVE_DRIFT;
		device.SetFeatureReport(rep);
	}
}

void WHTDialog::OnTimer()
{
	if (device.IsOpen())
	{
		// get the axis states
		hid_joystick_report_t rep;
		rep.report_id = JOYSTICK_REPORT_ID;
		device.GetInputReport(rep);

		SendMessage(GetCtrl(IDC_PRG_AXIS_X), PBM_SETPOS, (WPARAM) rep.x + 0x8000, 0);
		SendMessage(GetCtrl(IDC_PRG_AXIS_Y), PBM_SETPOS, (WPARAM) rep.y + 0x8000, 0);
		SendMessage(GetCtrl(IDC_PRG_AXIS_Z), PBM_SETPOS, (WPARAM) rep.z + 0x8000, 0);

		// get the RF state
		FeatRep_RFStatus repRF;
		repRF.report_id = RF_STATUS_REPORT_ID;
		device.GetFeatureReport(repRF);

		std::wstring res;
		if (repRF.num_packets >= 48)
			res = L"100";
		else
			res = int2str(repRF.num_packets * 2);

		SetStatusbarText(STATBAR_RF_STATUS, L"RF packets: " + res + L"%");

	} else {

		SendMessage(GetCtrl(IDC_PRG_AXIS_X), PBM_SETPOS, 0, 0);
		SendMessage(GetCtrl(IDC_PRG_AXIS_Y), PBM_SETPOS, 0, 0);
		SendMessage(GetCtrl(IDC_PRG_AXIS_Z), PBM_SETPOS, 0, 0);

		SetStatusbarText(STATBAR_RF_STATUS, L"Disconnected");
	}

	// SetStatusbarText(STATBAR_MISC, int2str(GetTickCount()));
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
	const int NUM_PARTS = 3;
	int parts[NUM_PARTS];
	parts[0] = 95;
	parts[1] = 325;
	parts[2] = -1;

	SendMessage(GetCtrl(IDC_STATUS_BAR), SB_SETPARTS, NUM_PARTS, (LPARAM) parts);
}

void WHTDialog::SetStatusbarText(int part, const std::wstring& text)
{
	SendMessage(GetCtrl(IDC_STATUS_BAR), SB_SETTEXT, part, (LPARAM) text.c_str());
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
	SendMessage(GetCtrl(ctrl_id), WM_SETTEXT, 0, (LPARAM) text.c_str());
}

float WHTDialog::GetCtrlTextFloat(int ctrl_id)
{
	const int BUFF_SIZE = 256;
	wchar_t buff[BUFF_SIZE];
	SendMessage(GetCtrl(ctrl_id), WM_GETTEXT, BUFF_SIZE, (LPARAM) buff);

	return (float) _wtof(buff);
}

void WHTDialog::ReadConfigFromDevice()
{
	debug(L"reading FeatRep_AxisConfig");

	FeatRep_DongleSettings rep;
	rep.report_id = DONGLE_SETTINGS_REPORT_ID;
	device.GetFeatureReport(rep);

	SetComboSelection(IDC_CMB_AXIS_RESPONSE, rep.is_linear ? 1 : 0);

	SetCheckState(IDC_CHK_SELFCENTER, rep.is_selfcenter != 0);

	SetCtrlText(IDC_EDT_APPLIED_DRIFT_COMP, flt2str(rep.x_drift_comp));

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
	device.GetFeatureReport(rep);

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
	debug(L"sending FeatRep_DongleSettings");

	FeatRep_DongleSettings rep;

	rep.is_linear = GetComboSelection(IDC_CMB_AXIS_RESPONSE) == 1 ? 1 : 0;
	rep.is_selfcenter = GetCheckState(IDC_CHK_SELFCENTER) ? 1 : 0;

	rep.x_drift_comp = GetCtrlTextFloat(IDC_EDT_APPLIED_DRIFT_COMP);

	rep.lin_fact_x = GetCtrlTextFloat(IDC_EDT_LIN_FACT_X);
	rep.lin_fact_y = GetCtrlTextFloat(IDC_EDT_LIN_FACT_Y);
	rep.lin_fact_z = GetCtrlTextFloat(IDC_EDT_LIN_FACT_Z);

	rep.exp_fact_x = GetCtrlTextFloat(IDC_EDT_EXP_FACT_X);
	rep.exp_fact_y = GetCtrlTextFloat(IDC_EDT_EXP_FACT_Y);
	rep.exp_fact_z = GetCtrlTextFloat(IDC_EDT_EXP_FACT_Z);

	rep.report_id = DONGLE_SETTINGS_REPORT_ID;
	device.SetFeatureReport(rep);
}

void WHTDialog::ChangeConnectedStateUI(bool is_connected)
{
	SetCtrlText(IDC_BTN_CONNECT, is_connected ? L"Disconnect" : L"Connect");

	EnableWindow(GetCtrl(IDC_BTN_READ_CALIBRATION), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_BTN_CALIBRATE), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_BTN_SEND_TO_TRACKER), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_EDT_APPLIED_DRIFT_COMP), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_EDT_NEW_DRIFT_COMP), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_BTN_RESET_DRIFT_COMP), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_BTN_SAVE_DRIFT_COMP), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_CMB_AXIS_RESPONSE), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_CHK_SELFCENTER), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_EDT_LIN_FACT_X), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_EDT_LIN_FACT_Y), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_EDT_LIN_FACT_Z), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_EDT_EXP_FACT_X), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_EDT_EXP_FACT_Y), is_connected ? TRUE : FALSE);
	EnableWindow(GetCtrl(IDC_EDT_EXP_FACT_Z), is_connected ? TRUE : FALSE);
}
