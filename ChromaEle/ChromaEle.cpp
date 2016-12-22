#include "stdafx.h"
#include "resource.h"

#include "ChromaManager.h"
#include "Settings.h"
#include "KeyState.h"
#include "GameState.h"

using namespace std;

ChromaManager chroma;
Settings settings(_T(".\\chromaEle.ini"));
KeyState keys;
GameState game;
int attunement = -1	;

HWND Hwnd;
HMENU Hmenu;
NOTIFYICONDATA notifyIconData;
TCHAR szTIP[64] = TEXT("ChromaEle");
char szClassName[] = "chromaEleTray";

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	keys.InputLowlevel(nCode, wParam, lParam);
	if (!(settings.getCheckGW2() || game.isRunning())) return CallNextHookEx(NULL, nCode, wParam, lParam);
	if (keys.getKeyPress(VK_F1)) {
		if (settings.getTempest() && attunement == 1) {
			chroma.EffectOverload(settings.getFireColor(), settings.getFireOI());
		}
		else if (attunement != 1) chroma.EffectAttunement(settings.getFireColor(), settings.getFireSI());
		attunement = 1;
	} else if (keys.getKeyPress(VK_F2)) {
		if (settings.getTempest() && attunement == 2) {
			chroma.EffectOverload(settings.getWaterColor(), settings.getWaterOI());
		}
		else if (attunement != 2)  chroma.EffectAttunement(settings.getWaterColor(), settings.getWaterSI());
		attunement = 2;
	} else if (keys.getKeyPress(VK_F3)) {
		if (settings.getTempest() && attunement == 3) {
			chroma.EffectOverload(settings.getAirColor(), settings.getAirOI());
		}
		else if (attunement != 3)  chroma.EffectAttunement(settings.getAirColor(), settings.getAirSI());
		attunement = 3;
	} else if (keys.getKeyPress(VK_F4)) {
		if (settings.getTempest() && attunement == 4) {
			chroma.EffectOverload(settings.getEarthColor(), settings.getEarthOI());
		}
		else if (attunement != 4)  chroma.EffectAttunement(settings.getEarthColor(), settings.getEarthSI());
		attunement = 4;
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE:
			Hmenu = CreatePopupMenu();
			AppendMenu(Hmenu, settings.getTempest() ? MF_CHECKED : MF_UNCHECKED, ID_TRAY_TEMPEST, TEXT("Tempest"));
			AppendMenu(Hmenu, MF_SEPARATOR, 0, NULL);
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Quit"));
			break;
		case WM_SYSICON:
			if (lParam == WM_RBUTTONDOWN) {
				POINT curPoint;
				GetCursorPos(&curPoint);
				SetForegroundWindow(Hwnd);
				UINT clicked = TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hwnd, NULL);
				SendMessage(hwnd, WM_NULL, 0, 0);
				switch (clicked) {
					case  ID_TRAY_TEMPEST:
						settings.setTempest(settings.getTempest() ? 0 : 1);
						CheckMenuItem(Hmenu, ID_TRAY_TEMPEST, MF_BYCOMMAND | (settings.getTempest() ? MF_CHECKED : MF_UNCHECKED));
						break;
					case ID_TRAY_EXIT:
						Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
						PostQuitMessage(0);
						break;
					default: break;
				}
			}
			break;
		default: break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	OutputDebugString(_T("ChromaEle starting\n"));
	BOOL initalized = chroma.Initialize();
	HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
	if (initalized) {
		OutputDebugString(_T("ChromaEle initialized\n"));
		MSG msg;
		WNDCLASSEX wincl;
		wincl.hInstance = hInstance;
		wincl.lpszClassName = szClassName;
		wincl.lpfnWndProc = WindowProcedure;
		wincl.style = CS_DBLCLKS;
		wincl.cbSize = sizeof(WNDCLASSEX);
		wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
		wincl.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
		wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
		wincl.lpszMenuName = NULL;
		wincl.cbClsExtra = 0;
		wincl.cbWndExtra = 0;
		wincl.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 255, 255)));
		if (!RegisterClassEx(&wincl)) {
			OutputDebugString(_T("ChromaEle: RegisterClassEx failed\n"));
		}
		Hwnd = CreateWindowEx(0, szClassName, szClassName, NULL, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, HWND_DESKTOP, NULL, hInstance,NULL);
		memset(&notifyIconData, 0, sizeof(NOTIFYICONDATA));
		notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
		notifyIconData.hWnd = Hwnd;
		notifyIconData.uID = ID_TRAY_APP_ICON;
		notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		notifyIconData.uCallbackMessage = WM_SYSICON;
		notifyIconData.hIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
		strncpy_s(notifyIconData.szTip, szTIP, sizeof(szTIP));
		Shell_NotifyIcon(NIM_ADD, &notifyIconData);
		while (GetMessage(&msg, NULL, 0, 0) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} else OutputDebugString(_T("ChromaEle failed\n"));
	OutputDebugString(_T("ChromaEle stopping\n"));
	UnhookWindowsHookEx(hhkLowLevelKybd);
	return 0;
}
