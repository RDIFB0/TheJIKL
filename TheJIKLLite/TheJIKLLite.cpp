// TheJIKLLite.cpp : Defines the entry point for the application.
//

// http://www.lotushints.com/2013/04/win32-hide-to-system-tray-part-3/

#include "stdafx.h"
#include "TheJIKLLite.h"
#include "NTKbdLites.h"
#include "Keyboard.h"

#include <stdio.h> // fopen

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Check another instance
	APP.mutex = CreateMutex(NULL, FALSE, TEXT("TheJIKL"));
	if (GetLastError() == ERROR_ALREADY_EXISTS || GetLastError() == ERROR_ACCESS_DENIED)
	{
		return 1;
	}

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_THEJIKLLITE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		CloseHandle(APP.mutex);
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_THEJIKLLITE));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	DeinitInstance();

	return (int) msg.wParam;
}


void TrayDrawIcon(HWND hWnd) {
	NOTIFYICONDATA nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICONUID;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_TRAYMESSAGE;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_THEJIKLLITE));
	LoadString(hInst, IDS_APP_TITLE, nid.szTip, 128);
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &nid);
}

void TrayDeleteIcon(HWND hWnd) {
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICONUID;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

void TrayLoadPopupMenu(HWND hWnd) {
	POINT cursor;
	HMENU hMenu;
	GetCursorPos(&cursor);
	hMenu = (HMENU) GetSubMenu(LoadMenu(hInst, MAKEINTRESOURCE(IDR_TRAYMENU)), 0);
	// hMenu = (HMENU) LoadMenu(hInst, MAKEINTRESOURCE(IDR_TRAYMENU));
	SetMenuDefaultItem(hMenu, IDM_TRAY_EXIT, FALSE);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, cursor.x, cursor.y, 0, hWnd, NULL);
}

BOOL ReadLayouts()
{
	const int MAX_LAYOUTS = 2;
	int nElements = GetKeyboardLayoutList(0, nullptr);
	if (nElements < MAX_LAYOUTS)
	{
		return FALSE;
	}

	// var ids = new IntPtr[nElements];
	APP.layoutInfo.lpList = (HKL*)LocalAlloc(LPTR, (nElements * sizeof(HKL)));
	// layoutInfo.lpList = new HKL[MAX_LAYOUTS];

	nElements = GetKeyboardLayoutList(nElements, APP.layoutInfo.lpList);
	if (MAX_LAYOUTS > nElements) {
		return FALSE;
	}

	APP.layoutInfo.first = TRUE;

	return TRUE;
}

void ChangeLayout(HKL hLayout)
{
	GUITHREADINFO threadInfo;
	threadInfo.cbSize = sizeof(threadInfo);
    GetGUIThreadInfo(0, &threadInfo);

	/*
    HWND hWnd = (threadInfo.hwndFocus != NULL) ? threadInfo.hwndFocus : threadInfo.hwndActive;
    PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_SYSCHARSET, reinterpret_cast<LPARAM>(hLayout));
	*/

	HWND hWnd = NULL;
	if (threadInfo.hwndCaret != NULL) {
		hWnd = threadInfo.hwndCaret;
	}
	else if (threadInfo.hwndFocus != NULL) {
		hWnd = threadInfo.hwndFocus;
	}
	else if (threadInfo.hwndActive != NULL) {
		hWnd = threadInfo.hwndActive;
	}

	if (hWnd != NULL) {
		PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_SYSCHARSET, reinterpret_cast<LPARAM>(hLayout));
	}

	/*
    PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_SYSCHARSET, (LPARAM)hLayout);

	DWORD recipients = BSM_APPLICATIONS;
	return (0 < ::BroadcastSystemMessage(BSF_POSTMESSAGE, &recipients, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_SYSCHARSET, reinterpret_cast<LPARAM>(mozc_hkl)));
	*/
}

void ChangeLayoutEmulate()
{
	keybd_event(VK_LWIN, 0, KEYEVENTF_EXTENDEDKEY, 0);
	keybd_event(VK_SPACE, 0, KEYEVENTF_EXTENDEDKEY, 0);
	Sleep(10);
	keybd_event(VK_SPACE, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	keybd_event(VK_LWIN, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

void SetHook(HWND hWnd)
{
	APP.hooked = false;
	APP.modified = false;

	APP.hookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, KeyHandler, hInst, 0);

	APP.shellHookMessage = RegisterWindowMessage(TEXT("SHELLHOOK"));
	RegisterShellHookWindow(hWnd);

	HookRawInput(hWnd);
}

void UnSetHook()
{
	if (APP.hookHandle != NULL) {
		UnhookWindowsHookEx(APP.hookHandle);
	}
}


void LightOn(BOOL on)
{
	HANDLE hndKbdDev;

	hndKbdDev = OpenKeyboardDevice(NULL);
	if (hndKbdDev == INVALID_HANDLE_VALUE)		// Was the device opened?
	{
		return;
	}

	TurnKeyboardLight(hndKbdDev, KEYBOARD_CAPS_LOCK_ON, on);
	CloseKeyboardDevice(hndKbdDev);
}

void SetupLayout(HWND hWnd)
{
	DWORD processId;
	DWORD threadId = GetWindowThreadProcessId(hWnd, &processId);
	HKL layout = GetKeyboardLayout(threadId);
	APP.layoutInfo.first = layout == APP.layoutInfo.lpList[0];

	LightOn(APP.layoutInfo.first ? FALSE : TRUE);
}

void DrawStats()
{
	HDC hDC_Desktop = GetDC(0);

	RECT rect = { 20, 20, 200, 200 };
	HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
	FillRect(hDC_Desktop, &rect, blueBrush);
}

void ErrorLastDebugString()
{
	wchar_t buf[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	OutputDebugString(buf);
	OutputDebugString(L"\n");
}

bool HookRawInput(HWND hWnd)
{
	UINT numDevices;
	if (GetRawInputDeviceList(NULL, &numDevices, sizeof(RAWINPUTDEVICELIST)) != 0) {
		return FALSE;
	}
	auto pDeviceData = new RAWINPUTDEVICELIST[numDevices];
	if (GetRawInputDeviceList(pDeviceData, &numDevices, sizeof(RAWINPUTDEVICELIST)) == -1) {
		delete[] pDeviceData;
		return FALSE;
	}

	UINT rimHIDregistered = 0;
	for (UINT i = 0; i < numDevices; ++i)
	{
		HANDLE hDevice = pDeviceData[i].hDevice;
		DWORD dwType = pDeviceData[i].dwType;

		switch (dwType) {
		case RIM_TYPEMOUSE:
		case RIM_TYPEKEYBOARD:
			continue;
		case RIM_TYPEHID:
			break;
		}

		/*
		UINT infoSize;
		if (GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, nullptr, &infoSize) == -1) {
			ErrorLastDebugString();
		}
		wchar_t *deviceName = new wchar_t[infoSize];
		if (GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, deviceName, &infoSize) == -1) {
			ErrorLastDebugString();
		}
		OutputDebugString(deviceName);
		OutputDebugString(L"\n");
		delete[] deviceName;
		*/

		UINT infoSize;
		if (GetRawInputDeviceInfo(hDevice, RIDI_DEVICEINFO, nullptr, &infoSize) == -1) {
			ErrorLastDebugString();
		}

		RID_DEVICE_INFO deviceInfo = { 0 };
		deviceInfo.cbSize = infoSize = sizeof(deviceInfo);
		if (GetRawInputDeviceInfo(hDevice, RIDI_DEVICEINFO, &deviceInfo, &infoSize) == -1) {
			ErrorLastDebugString();
		}

		WCHAR debugBuffer[255];
		wsprintf(debugBuffer, L"Vendor ID: %d (0x%04x)\n", deviceInfo.hid.dwVendorId, deviceInfo.hid.dwVendorId);
		OutputDebugString(debugBuffer);
		wsprintf(debugBuffer, L"Product ID: %d (0x%04x)\n", deviceInfo.hid.dwProductId, deviceInfo.hid.dwProductId);
		OutputDebugString(debugBuffer);
		wsprintf(debugBuffer, L"Version number: %d (0x%04x)\n", deviceInfo.hid.dwVersionNumber, deviceInfo.hid.dwVersionNumber);
		OutputDebugString(debugBuffer);
		wsprintf(debugBuffer, L"Usage page: %d (0x%02x)\n", deviceInfo.hid.usUsagePage, deviceInfo.hid.usUsagePage);
		OutputDebugString(debugBuffer);
		wsprintf(debugBuffer, L"Usage: %d (0x%02x)\n", deviceInfo.hid.usUsage, deviceInfo.hid.usUsage);
		OutputDebugString(debugBuffer);

		// Das Keyboard Model S for Mac:
		// HID\VID_04D9&PID_1900&MI_01\8 & C9452 & 0 & 0000
		// Vendor ID: 1241 (0x04d9)
		// Product ID : 6400 (0x1900)
		// Version number : 264 (0x0108)
		// Usage page : 12 (0x0c)
		// Usage : 1 (0x01)
		if (deviceInfo.hid.dwVendorId == 0x04D9 && deviceInfo.hid.dwProductId == 0x1900) {
			rimHIDregistered = 1;

			RAWINPUTDEVICE rid = { 0 };
			rid.usUsagePage = deviceInfo.hid.usUsagePage;
			rid.usUsage = deviceInfo.hid.usUsage;
			rid.dwFlags = RIDEV_INPUTSINK;
			rid.hwndTarget = hWnd;
			if (RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE)) == FALSE) {
				DebugBreak();
				ErrorLastDebugString();
			}
		}
	}

	delete[] pDeviceData;

	return rimHIDregistered > 0;
}

void OutputDebugHex(void *ptr, DWORD dwSize)
{
	WCHAR charBuf[255];
	DWORD i;
	BYTE *b = (BYTE *)ptr;
	for (i = 0; i < dwSize; ++i) {
		wsprintf(charBuf, L"0x%02x ", b[i]);
		OutputDebugString(charBuf);
		if (i % 16 == 0)
			OutputDebugString(L"\n");
	}
	OutputDebugString(L"--------------------\n");
}

void OutputDebugHexFile(void *ptr, DWORD dwSize)
{
	FILE *f;
	fopen_s(&f, "debug.log", "a");
	DWORD i;
	BYTE *b = (BYTE *)ptr;
	for (i = 0; i < dwSize; i++) {
		fprintf(f, "0x%02x ", b[i]);
		if (i % 16 == 0) {
			//fprintf(f, "\n");
		}
	}
	fprintf(f, "\n--------------------\n");
	fclose(f);
}

void OutputDebugHexFileB(BYTE *bytes, DWORD dwSize)
{
	FILE *f;
	fopen_s(&f, "debug.bin", "a");
	fwrite(bytes, sizeof(BYTE), dwSize, f);
	fclose(f);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_THEJIKLLITE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_THEJIKLLITE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_THEJIKLLITE));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   // ShowWindow(hWnd, nCmdShow);
   // UpdateWindow(hWnd);

   if (!ReadLayouts()) {
		DestroyWindow(hWnd);
		return FALSE;
   }

   ShowWindow(hWnd, SW_HIDE);
   TrayDrawIcon(hWnd);
   SetHook(hWnd);

   return TRUE;
}

void DeinitWindow(HWND hWnd)
{
	// remove shell hook
	DeregisterShellHookWindow(hWnd);

	// remove tray icon
	TrayDeleteIcon(hWnd);
}

void DeinitInstance()
{
	// OutputDebugString(L"DeInitInstance\n");

	// remove ll keyboard hook
	UnSetHook();

	LocalFree(APP.layoutInfo.lpList);
	// delete[] layoutInfo.lpList;

	CloseHandle(APP.mutex);
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_TRAY_EXIT:
		case IDM_EXIT:
			DeinitWindow(hWnd);
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;


	case WM_TRAYMESSAGE:
		switch(lParam) {
		case WM_LBUTTONDBLCLK:
			ShowWindow(hWnd, SW_SHOW);
			break;
		case WM_RBUTTONUP:
			TrayLoadPopupMenu(hWnd);
			break;
		}
		break;

	case WM_INPUT: {
		UINT dwSize;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL)
		{
			return 0;
		}

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
			ErrorLastDebugString();
		}

		RAWINPUT* raw = (RAWINPUT*)lpb;
		if (raw->header.dwType == RIM_TYPEHID) {
			UINT infoSize;
			if (GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICEINFO, nullptr, &infoSize) == -1) {
				ErrorLastDebugString();
			}
			RID_DEVICE_INFO deviceInfo = { 0 };
			deviceInfo.cbSize = infoSize = sizeof(deviceInfo);
			if (GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICEINFO, &deviceInfo, &infoSize) == -1) {
				ErrorLastDebugString();
			}
			if (deviceInfo.hid.dwVendorId == 0x04D9 && deviceInfo.hid.dwProductId == 0x1900) {
				/*
				for (DWORD i = 0; i < raw->data.hid.dwCount; i++) {
					BYTE* buf = &raw->data.hid.bRawData[raw->data.hid.dwSizeHid * i];
					OutputDebugHexFileB(buf, raw->data.hid.dwSizeHid);
				}
				*/
				unsigned short value = *(unsigned short *)&raw->data.hid.bRawData;
				if (value == 0x0800) {
					Keyboard::KeyDown(VK_INSERT);
				}
				else if (value == 0x0000) {
					Keyboard::KeyUp(VK_INSERT);
				}
			}
		}

		delete[] lpb;
		return DefWindowProc(hWnd, message, wParam, lParam);
		//return 0;
	}

	default:

		if (message == APP.shellHookMessage)
		{
			switch (wParam)
			{
			case HSHELL_WINDOWACTIVATED:
			case HSHELL_RUDEAPPACTIVATED:
				SetupLayout((HWND)lParam);
				break;
			}
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Low level keyboard hook handler
LRESULT CALLBACK KeyHandler(int nCode, WPARAM wParam, LPARAM lParam)
{
	// == HC_ACTION
	if (nCode < 0 || nCode != HC_ACTION)
	{
		return CallNextHookEx(APP.hookHandle, nCode, wParam, lParam);
	}

	// cout << ((KBDLLHOOKSTRUCT *) lParam)->vkCode << endl;
	KBDLLHOOKSTRUCT *khs = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

	/*
	SYSKEYS:
		ALT + any key
		F10
	*/
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{
		// if ((khs->flags & LLKHF_INJECTED) == 0) { }

		switch(khs->vkCode)
		{
		case VK_CAPITAL:
			if (APP.layoutInfo.first)
			{
				 ChangeLayout(APP.layoutInfo.lpList[1]);
				//ChangeLayoutEmulate();
				APP.layoutInfo.first = FALSE;
				LightOn(TRUE);
			}
			else
			{
				 ChangeLayout(APP.layoutInfo.lpList[0]);
				//ChangeLayoutEmulate();
				APP.layoutInfo.first = TRUE;
				LightOn(FALSE);
			}
			Swallow
			break;

		case VK_SPACE:
			APP.modified = TRUE;
			APP.hooked = FALSE;
			//DrawStats();
			Swallow
			break;
		}

		if (APP.isMacEmu) {
			switch (khs->vkCode)
			{
			case VK_LWIN:
				if ((khs->flags & LLKHF_INJECTED) == 0) {
					Keyboard::KeyDown(VK_LMENU);
					Swallow
				}
				break;
			case VK_RWIN:
				if ((khs->flags & LLKHF_INJECTED) == 0) {
					Keyboard::KeyDown(VK_RMENU);
					Swallow
				}
				break;
			case VK_LMENU:
				if ((khs->flags & LLKHF_INJECTED) == 0) {
					Keyboard::KeyDown(VK_LWIN);
					Swallow
				}
				break;
			case VK_RMENU:
				if ((khs->flags & LLKHF_INJECTED) == 0) {
					Keyboard::KeyDown(VK_RWIN);
					Swallow
				}
				break;
			case VK_F13:
				Keyboard::KeyDown(VK_SNAPSHOT);
				Swallow
			case VK_F14:
				Keyboard::KeyDown(VK_SCROLL);
				Swallow
			case VK_F15:
				Keyboard::KeyDown(VK_PAUSE);
				Swallow
			}
		}

		if (APP.modified)
			switch (khs->vkCode) {
			// WinAmp
			case VK_KEY_Z:
				APP.hooked = TRUE;
				Keyboard::KeyPress(VK_MEDIA_PREV_TRACK);
				Swallow
			case VK_KEY_X :
				APP.hooked = TRUE;
				Keyboard::KeyPress(VK_MEDIA_STOP);
				Keyboard::KeyPress(VK_MEDIA_PLAY_PAUSE);
				Swallow
			case VK_KEY_C:
				APP.hooked = TRUE;
				Keyboard::KeyPress(VK_MEDIA_PLAY_PAUSE);
				Swallow
			case VK_KEY_V:
				APP.hooked = TRUE;
				Keyboard::KeyPress(VK_MEDIA_STOP);
				Swallow
			case VK_KEY_B:
				APP.hooked = TRUE;
				Keyboard::KeyPress(VK_MEDIA_NEXT_TRACK);
				Swallow
			// Mac emu
			case VK_KEY_M:
				APP.hooked = TRUE;
				APP.isMacEmu = !APP.isMacEmu;
				Swallow
			// Volume
			case VK_UP:
				APP.hooked = TRUE;
				Keyboard::KeyPress(VK_VOLUME_UP);
				Swallow
			case VK_DOWN:
				APP.hooked = TRUE;
				Keyboard::KeyPress(VK_VOLUME_DOWN);
				Swallow
		}
	} else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {

		if (APP.modified) {
			switch (khs->vkCode) {
				// WinAmp
			case VK_KEY_Z:
			case VK_KEY_X:
			case VK_KEY_C:
			case VK_KEY_V:
			case VK_KEY_B:
				Swallow
				// Mac emu
			case VK_KEY_M:
				Swallow
				// Volume
			case VK_UP:
			case VK_DOWN:
				Swallow

			case VK_SPACE:
				APP.modified = FALSE;
				if (APP.hooked) {
					APP.hooked = FALSE;
					Swallow
				}
				Keyboard::KeyPress(VK_SPACE);
				Swallow
			}
		}
		else {

			switch (khs->vkCode) {
			case VK_CAPITAL:
				Swallow
			}
			// if APP.isMacEmu
		}

		if (APP.isMacEmu) {
			switch (khs->vkCode) {
			case VK_LWIN:
				if ((khs->flags & LLKHF_INJECTED) == 0) {
					Keyboard::KeyUp(VK_LMENU);
					Swallow
				}
				break;
			case VK_RWIN:
				if ((khs->flags & LLKHF_INJECTED) == 0) {
					Keyboard::KeyUp(VK_RMENU);
					Swallow
				}
				break;
			case VK_LMENU:
				if ((khs->flags & LLKHF_INJECTED) == 0) {
					Keyboard::KeyUp(VK_LWIN);
					Swallow
				}
				break;
			case VK_RMENU:
				if ((khs->flags & LLKHF_INJECTED) == 0) {
					Keyboard::KeyUp(VK_RWIN);
					Swallow
				}
				break;
			case VK_F13:
				Keyboard::KeyUp(VK_SNAPSHOT);
				Swallow
			case VK_F14:
				Keyboard::KeyUp(VK_SCROLL);
				Swallow
			case VK_F15:
				Keyboard::KeyUp(VK_PAUSE);
				Swallow
			}
		}
	}

	return CallNextHookEx(APP.hookHandle, nCode, wParam, lParam);
}
