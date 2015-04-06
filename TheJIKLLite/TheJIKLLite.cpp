// TheJIKLLite.cpp : Defines the entry point for the application.
//

// http://www.lotushints.com/2013/04/win32-hide-to-system-tray-part-3/

#include "stdafx.h"
#include "TheJIKLLite.h"
#include "NTKbdLites.h"

LayoutInfo layoutInfo;

HHOOK hookHandle;
UINT shellHookMessage;
BOOL hooked;
BOOL modified;

// *****************


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

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
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICONUID;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_TRAYMESSAGE;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
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
	layoutInfo.lpList = (HKL*)LocalAlloc(LPTR, (nElements * sizeof(HKL)));
	// layoutInfo.lpList = new HKL[MAX_LAYOUTS];

	nElements = GetKeyboardLayoutList(nElements, layoutInfo.lpList);
	if (MAX_LAYOUTS > nElements) {
		return FALSE;
	}

	layoutInfo.first = TRUE;

	return TRUE;
}

void ChangeLayout(HKL hLayout)
{
	GUITHREADINFO threadInfo;
	threadInfo.cbSize = sizeof(threadInfo);
    GetGUIThreadInfo(0, &threadInfo);

    HWND hWnd = (threadInfo.hwndFocus != NULL) ? threadInfo.hwndFocus : threadInfo.hwndActive;
    PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, WM_INPUTLANGCHANGE_SYSCHARSET, (UINT)hLayout);
}

void SetHook(HWND hWnd)
{
	hooked = false;
	modified = false;
	hookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, KeyHandler, hInst, 0);

	shellHookMessage = RegisterWindowMessage(TEXT("SHELLHOOK"));
	RegisterShellHookWindow(hWnd);
}

void UnSetHook()
{
	if (hookHandle != NULL) {
		UnhookWindowsHookEx(hookHandle);
	}
}

void KeyDownUp(BYTE vkCode)
{
	BYTE scanCode = (BYTE)MapVirtualKey(vkCode, 0);
	// 0x39
	keybd_event(vkCode, scanCode, KEYEVENTF_EXTENDEDKEY | 0, 0);
	keybd_event(vkCode, scanCode, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
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
	layoutInfo.first = layout == layoutInfo.lpList[0];

	LightOn(layoutInfo.first ? FALSE : TRUE);
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
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

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

	LocalFree(layoutInfo.lpList);
	// delete[] layoutInfo.lpList;
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

	default:

		if (message == shellHookMessage)
		{
			switch (wParam)
			{
			case HSHELL_WINDOWACTIVATED:
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
	if (nCode < 0)
	{
		return CallNextHookEx(hookHandle, nCode, wParam, lParam);
	}

	// cout << ((KBDLLHOOKSTRUCT *) lParam)->vkCode << endl;
	KBDLLHOOKSTRUCT *khs = (KBDLLHOOKSTRUCT *) lParam;

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{
		switch(khs->vkCode)
		{
		case VK_CAPITAL:
			if (layoutInfo.first)
			{
				ChangeLayout(layoutInfo.lpList[1]);
				layoutInfo.first = FALSE;
				LightOn(TRUE);
			}
			else
			{
				ChangeLayout(layoutInfo.lpList[0]);
				layoutInfo.first = TRUE;
				LightOn(FALSE);
			}
			return 1;
			break;
		case VK_SPACE:
			modified = TRUE;
			hooked = FALSE;
			return 1;
			break;
		}
		if (modified)
			switch (khs->vkCode) {
			// WinAmp
			case VK_Z:
				hooked = TRUE;
				KeyDownUp(VK_MEDIA_PREV_TRACK);
				return 1;
			case VK_X :
				hooked = TRUE;
				KeyDownUp(VK_MEDIA_STOP);
				KeyDownUp(VK_MEDIA_PLAY_PAUSE);
				return 1;
			case VK_C:
				hooked = TRUE;
				KeyDownUp(VK_MEDIA_PLAY_PAUSE);
				return 1;
			case VK_V:
				hooked = TRUE;
				KeyDownUp(VK_MEDIA_STOP);
				return 1;
			case VK_B:
				hooked = TRUE;
				KeyDownUp(VK_MEDIA_NEXT_TRACK);
				return 1;

		}
	} else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {

		if (modified)
			switch (khs->vkCode) {
			// WinAmp
			case VK_Z:
			case VK_X:
			case VK_C:
			case VK_V:
			case VK_B:
				return 1;

			case VK_SPACE:
				modified = FALSE;
				if (hooked) {
					hooked = FALSE;
					return 1;
				}
				KeyDownUp(VK_SPACE);
				return 1;

			case VK_CAPITAL:
				return 1;
		}
	}

	return CallNextHookEx(hookHandle, nCode, wParam, lParam);
}
