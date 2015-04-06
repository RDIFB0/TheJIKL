#pragma once

#include "resource.h"

#include <shellapi.h>

#define MAX_LOADSTRING 100

#define WM_INPUTLANGCHANGE_SYSCHARSET 0x0001

#define VK_Z 0x5A
#define VK_X 0x58
#define VK_C 0x43
#define VK_V 0x56
#define VK_B 0x42

#define TRAY_ICONUID 100
#define WM_TRAYMESSAGE (WM_USER + 1)


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

void DeinitInstance();
void DeinitWindow(HWND hWnd);

void TrayDrawIcon(HWND hWnd);
void TrayDeleteIcon(HWND hWnd);
void TrayLoadPopupMenu(HWND hWnd);
BOOL ReadLayouts();
void ChangeLayout(HKL hLayout);
void SetHook(HWND hWnd);
void UnSetHook();
void KeyDownUp(BYTE vkCode);
void LightOn(BOOL on);
void SetupLayout(HWND hWnd);
