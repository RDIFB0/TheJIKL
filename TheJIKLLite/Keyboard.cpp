#include "stdafx.h"
#include "Keyboard.h"

void Keyboard::KeyDown(WORD vkCode)
{
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vkCode;
	//input.ki.wScan = MapVirtualKeyEx(vkCode, 0, (HKL)0xf0010413);
	input.ki.wScan = MapVirtualKey(vkCode, 0);
	input.ki.dwFlags = IsExtendedKey(vkCode) ? KEYEVENTF_EXTENDEDKEY : 0;
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
	input.ki.dwFlags = KEYEVENTF_KEYUP | (IsExtendedKey(vkCode) ? KEYEVENTF_EXTENDEDKEY : 0);
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
	input.ki.dwFlags = IsExtendedKey(vkCode) ? KEYEVENTF_EXTENDEDKEY : 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
	input.ki.dwFlags = KEYEVENTF_KEYUP | (IsExtendedKey(vkCode) ? KEYEVENTF_EXTENDEDKEY : 0);
	SendInput(1, &input, sizeof(INPUT));
}

void Keyboard::ScanDown(WORD scanCode)
{
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.wScan = scanCode;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
}

void Keyboard::ScanUp(WORD scanCode)
{
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.wScan = scanCode;
	input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
}

void Keyboard::ScanPress(WORD scanCode)
{
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.wScan = scanCode;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
	input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

bool Keyboard::IsExtendedKey(WORD vk)
{
	// From the SDK:
	// The extended-key flag indicates whether the keystroke message originated from one of
	// the additional keys on the enhanced keyboard. The extended keys consist of the ALT and
	// CTRL keys on the right-hand side of the keyboard; the INS, DEL, HOME, END, PAGE UP,
	// PAGE DOWN, and arrow keys in the clusters to the left of the numeric keypad; the NUM LOCK
	// key; the BREAK (CTRL+PAUSE) key; the PRINT SCRN key; and the divide (/) and ENTER keys in
	// the numeric keypad. The extended-key flag is set if the key is an extended key.
	//
	// - docs appear to be incorrect. Use of Spy++ indicates that break is not an extended key.
	// Also, menu key and windows keys also appear to be extended.
	return vk == VK_RMENU ||
		vk == VK_RCONTROL ||
		vk == VK_NUMLOCK ||
		vk == VK_INSERT ||
		vk == VK_DELETE ||
		vk == VK_HOME ||
		vk == VK_END ||
		vk == VK_PRIOR ||
		vk == VK_NEXT ||
		vk == VK_UP ||
		vk == VK_DOWN ||
		vk == VK_LEFT ||
		vk == VK_RIGHT ||
		vk == VK_APPS ||
		vk == VK_RWIN ||
		vk == VK_LWIN;
	// Note that there are no distinct values for the following keys:
	// numpad divide
	// numpad enter
}