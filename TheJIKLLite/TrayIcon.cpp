#include "stdafx.h"
#include "TrayIcon.h"
#include "Resource.h"


TrayIcon::TrayIcon(HINSTANCE hInstance, HWND hWnd): _hInstance(hInstance), _hWnd(hWnd)
{
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::Show()
{
	NOTIFYICONDATA nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = _hWnd;
	nid.uID = TRAY_ICONUID;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_TRAYMESSAGE;
	nid.hIcon = LoadIcon(_hInstance, MAKEINTRESOURCE(IDI_THEJIKLLITE));
	LoadString(_hInstance, IDS_APP_TITLE, nid.szTip, 128);
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &nid);
}

void TrayIcon::Hide()
{
	NOTIFYICONDATA nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = _hWnd;
	nid.uID = TRAY_ICONUID;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}
