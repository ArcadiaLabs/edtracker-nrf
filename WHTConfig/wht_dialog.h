#pragma once

class WHTDialog
{
private:
	HWND		hDialog;
	HICON		hIconSmall;
	HICON		hIconBig;
	WHTDevice	device;

	void ReadConfigFromDevice();
	void ReadCalibrationData();
	void SendConfigToDevice();

	HWND GetCtrl(int ctrl_id)
	{
		return GetDlgItem(hDialog, ctrl_id);
	}

	void InitStatusbar();
	void SetStatusbarText(int part, const std::wstring& text);

	void SetCtrlText(int ctrl_id, const std::wstring& text);
	void SetCtrlTextFloat(int ctrl_id, float flt)
	{
		SetCtrlText(ctrl_id, flt2str(flt));
	}

	void ClearCtrlText(int ctrl_id)
	{
		SetCtrlText(ctrl_id, L"");
	}

	float GetCtrlTextFloat(int ctrl_id);

	void SetRadioState(int ctrl_id, bool new_state);
	bool GetRadioState(int ctrl_id)
	{
		return SendMessage(GetDlgItem(hDialog, ctrl_id), BM_GETCHECK, 0, 0) == BST_CHECKED;
	}

	void SetCheckState(int ctrl_id, bool new_state)
	{
		CheckDlgButton(hDialog, ctrl_id, new_state ? BST_CHECKED : BST_UNCHECKED);
	}

	bool GetCheckState(int ctrl_id)
	{
		return IsDlgButtonChecked(hDialog, ctrl_id) == BST_CHECKED;
	}

	void CreateTrayIcon();
	void RemoveTrayIcon();

	void OnCommand(int ctrl_id);
	void OnTimer();
	void OnTrayNotify(LPARAM lParam);
	void OnMinimize();

	void ChangeConnectedStateUI(bool is_connected);

	void Hide()
	{
		ShowWindow(hDialog, SW_HIDE);
	}

	void Show()
	{
		ShowWindow(hDialog, SW_SHOW);
	}

public:
	explicit WHTDialog(HWND hDlg);
	~WHTDialog();

	static BOOL CALLBACK MyDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};
