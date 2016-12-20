#pragma once

#include "resource.h"

#include <shellapi.h>

#define MAX_LOADSTRING 100

#define TRAY_ICONUID 100
#define WM_TRAYMESSAGE (WM_USER + 1)

#define Swallow return 1;


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	KeyHandler(int nCode, WPARAM wParam, LPARAM lParam);

struct LayoutInfo {
	HKL *lpList;	
	BOOL first;
};

struct App_t
{
	HANDLE mutex;

	LayoutInfo layoutInfo;

	HHOOK hookHandle;
	UINT shellHookMessage;
	BOOL hooked;
	BOOL modified;

	BOOL isMacEmu;
} APP;

void DeinitInstance();
void DeinitWindow(HWND hWnd);

void TrayDrawIcon(HWND hWnd);
void TrayDeleteIcon(HWND hWnd);
void TrayLoadPopupMenu(HWND hWnd);
BOOL ReadLayouts();
void ChangeLayout(HKL hLayout);
void ChangeLayoutEmulate();
void SetHook(HWND hWnd);
void UnSetHook();

void LightOn(BOOL on);
void SetupLayout(HWND hWnd);

bool HookRawInput(HWND hWnd);

