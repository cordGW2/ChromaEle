#include "stdafx.h"

#include "KeyState.h"

KeyState::KeyState(){

}

void KeyState::InputLowlevel(int nCode, WPARAM wParam, LPARAM lParam) {
	pressedNew.clear();
	if (wParam != WM_KEYUP && wParam != WM_KEYDOWN) return;
	KBDLLHOOKSTRUCT *keyBoard = (KBDLLHOOKSTRUCT*)lParam;
	DWORD key = (DWORD)keyBoard->vkCode;
	if (wParam == WM_KEYDOWN && pressedKeys.find(key) == pressedKeys.end()) {
		pressedKeys.insert(key);
		pressedNew.insert(key);
	} else {
		pressedKeys.erase(key);
	}
}

bool KeyState::getKeyPress(DWORD key) {
	return pressedNew.find(key) != pressedNew.end();
}


KeyState::~KeyState(){
}
