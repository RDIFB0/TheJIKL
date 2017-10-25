#pragma once

#include <shellapi.h>

#define TRAY_ICONUID 100
#define WM_TRAYMESSAGE (WM_USER + 1)

class TrayIcon
{
	HWND _hWnd;
	HINSTANCE _hInstance;
public:
	TrayIcon(HINSTANCE hInstance, HWND hWnd);
	~TrayIcon();
	void Show();
	void Hide();
};

