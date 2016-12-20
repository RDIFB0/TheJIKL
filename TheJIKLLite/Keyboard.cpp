#include "stdafx.h"
#include "Keyboard.h"

void Keyboard::KeyDown(WORD vkCode)
{
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vkCode;
	//input.ki.wScan = 0;
	//input.ki.wScan = MapVirtualKeyEx(vkCode, 0, (HKL)0xf0010413);
	input.ki.wScan = MapVirtualKey(vkCode, 0);
	input.ki.dwFlags = 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
}

void Keyboard::KeyUp(WORD vkCode)
{
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vkCode;
	input.ki.wScan = MapVirtualKey(vkCode, 0);
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
}

void Keyboard::KeyPress(WORD vkCode)
{
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vkCode;
	input.ki.wScan = MapVirtualKey(vkCode, 0);
	input.ki.dwFlags = 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

/*
void EmulateKeyDown(BYTE vkCode)
{
	BYTE scanCode = (BYTE)MapVirtualKey(vkCode, 0);
	keybd_event(vkCode, scanCode, KEYEVENTF_EXTENDEDKEY | 0, 0);
}

void EmulateKeyUp(BYTE vkCode)
{
	BYTE scanCode = (BYTE)MapVirtualKey(vkCode, 0);
	keybd_event(vkCode, scanCode, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

void KeyDownUp(BYTE vkCode)
{
	BYTE scanCode = (BYTE)MapVirtualKey(vkCode, 0);
	// 0x39
	keybd_event(vkCode, scanCode, KEYEVENTF_EXTENDEDKEY | 0, 0);
	keybd_event(vkCode, scanCode, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}
*/
