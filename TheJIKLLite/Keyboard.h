#pragma once

#define VK_KEY_Z 0x5A
#define VK_KEY_X 0x58
#define VK_KEY_C 0x43
#define VK_KEY_V 0x56
#define VK_KEY_B 0x42
#define VK_KEY_M 0x4D

class Keyboard
{
	static bool IsExtendedKey(WORD vkCode);
public:
	static void KeyDown(WORD vkCode);
	static void KeyUp(WORD vkCode);
	static void KeyPress(WORD vkCode);

	static void ScanDown(WORD scanCode);
	static void ScanUp(WORD scanCode);
	static void ScanPress(WORD scanCode);
};

