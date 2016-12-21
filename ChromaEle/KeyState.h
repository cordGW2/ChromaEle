#pragma once

#include <set>

class KeyState {
public:
	KeyState();
	void InputLowlevel(int nCode, WPARAM wParam, LPARAM lParam);
	bool getKeyPress(DWORD key);
	~KeyState();
private:
	std::set<DWORD> pressedNew;
	std::set<DWORD> pressedKeys;
};

