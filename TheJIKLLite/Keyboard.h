#pragma once

#define VK_KEY_Z 0x5A
#define VK_KEY_X 0x58
#define VK_KEY_C 0x43
#define VK_KEY_V 0x56
#define VK_KEY_B 0x42
#define VK_KEY_M 0x4D

class Keyboard
{
public:
	static void KeyDown(WORD vkCode);
	static void KeyUp(WORD vkCode);
	static void KeyPress(WORD vkCode);
};

