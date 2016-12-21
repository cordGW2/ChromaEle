#include "stdafx.h"
#include "resource.h"

#include "ChromaEle.h"
#include "Util.h"

#ifdef _WIN64
#define CHROMASDKDLL        _T("RzChromaSDK64.dll")
#else
#define CHROMASDKDLL        _T("RzChromaSDK.dll")
#endif

using namespace std;

typedef RZRESULT(*INIT)(void);
typedef RZRESULT(*CREATEKEYBOARDEFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEMOUSEEFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);

INIT Init = nullptr;
CREATEKEYBOARDEFFECT CreateKeyboardEffect = nullptr;
CREATEMOUSEEFFECT CreateMouseEffect = nullptr;

ChromaImplementation chroma;
int attunement = -1	;
DWORD currentEffect = 0;

HWND Hwnd;
HMENU Hmenu;
NOTIFYICONDATA notifyIconData;
TCHAR szTIP[64] = TEXT("ChromaEle");
char szClassName[] = "chromaEleTray";

COLORREF fireColor, waterColor, airColor, earthColor;
BYTE fireSI, waterSI, airSI, earthSI;
BYTE fireOI, waterOI, airOI, earthOI;
BOOL tempest, checkGW2;

BOOL ChromaImplementation::Initialize() {
	if (chromaSDKModule == nullptr) {
		chromaSDKModule = LoadLibrary(CHROMASDKDLL);
		if (chromaSDKModule == nullptr) return FALSE;
	}
	if (Init == nullptr) {
		RZRESULT Result = RZRESULT_INVALID;
		Init = reinterpret_cast<INIT>(GetProcAddress(chromaSDKModule, "Init"));
		if (!Init) return FALSE;
		Result = Init();
		if (Result == RZRESULT_SUCCESS) {
			CreateKeyboardEffect = reinterpret_cast<CREATEKEYBOARDEFFECT>(GetProcAddress(chromaSDKModule, "CreateKeyboardEffect"));
			CreateMouseEffect = reinterpret_cast<CREATEMOUSEEFFECT>(GetProcAddress(chromaSDKModule, "CreateMouseEffect"));
			if (CreateKeyboardEffect && CreateMouseEffect) return TRUE;
		}
	}
	return FALSE;
}

void colorEffect(COLORREF color) {
	ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE kbEffect = {};
	for (UINT row = 0; row < ChromaSDK::Keyboard::MAX_ROW; row++) {
		for (UINT col = 0; col < ChromaSDK::Keyboard::MAX_COLUMN; col++) {
			kbEffect.Color[row][col] = color;
		}
	}
	CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &kbEffect, nullptr);

	ChromaSDK::Mouse::CUSTOM_EFFECT_TYPE2 msEffect = {};
	for (UINT row = 0; row < ChromaSDK::Mouse::MAX_ROW; row++) {
		for (UINT col = 0; col < ChromaSDK::Mouse::MAX_COLUMN; col++) {
			msEffect.Color[row][col] = color;
		}
	}
	CreateMouseEffect(ChromaSDK::Mouse::CHROMA_CUSTOM2, &msEffect, nullptr);
}

bool IsGW2Running() {
	return IsProcessRunning("Gw2-64.exe") || IsProcessRunning("Gw2.exe") || !checkGW2;
}

DWORD WINAPI EffectAttunementThread(LPVOID lpParam) {
	if (!IsGW2Running()) return 0;
	currentEffect = GetCurrentThreadId();
	effect_data* data = (effect_data*)lpParam;
	COLORREF color = data->color;
	BYTE blinkRed = 255, blinkGreen = 255, blinkBlue = 255;
	if (GetRValue(color) + data->intensity <= 255) blinkRed = GetRValue(color) + data->intensity;
	if (GetGValue(color) + data->intensity <= 255) blinkGreen = GetGValue(color) + data->intensity;
	if (GetBValue(color) + data->intensity <= 255) blinkBlue = GetBValue(color) + data->intensity;
	COLORREF blinkColor = RGB(blinkRed, blinkGreen, blinkBlue);
	colorEffect(blinkColor);
	Sleep(150);
	colorEffect(color);
	delete data;
	return 0;
}

void ChromaImplementation::EffectAttunement(COLORREF color, BYTE intensity) {
	effect_data* data = new effect_data();
	data->color = color;
	data->intensity = intensity;
	CreateThread(NULL, 0, EffectAttunementThread, data, 0, &currentEffect);
}

DWORD WINAPI EffectOverloadThread(LPVOID lpParam) {
	if (!IsGW2Running()) return 0;
	currentEffect = GetCurrentThreadId();
	effect_data* data = (effect_data*)lpParam;
	COLORREF color = data->color;
	int time = 0;
	while (time < 5000) {
		if (currentEffect != GetCurrentThreadId()) return 0;
		BYTE blinkRed = 255, blinkGreen = 255, blinkBlue = 255;
		float factor = -0.5f * cosf(time / 125.0f) + 0.5f;
		BYTE intensity = (BYTE)(factor * data->intensity);
		if (GetRValue(color) + intensity <= 255) blinkRed = GetRValue(color) + intensity;
		if (GetGValue(color) + intensity <= 255) blinkGreen = GetGValue(color) + intensity;
		if (GetBValue(color) + intensity <= 255) blinkBlue = GetBValue(color) + intensity;
		COLORREF blinkColor = RGB(blinkRed, blinkGreen, blinkBlue);
		colorEffect(blinkColor);
		time += 20;
		Sleep(20);
	}
	colorEffect(color);
	delete data;
	return 0;
}

void ChromaImplementation::EffectOverload(COLORREF color, BYTE intensity) {
	effect_data* data = new effect_data();
	data->color = color;
	data->intensity = intensity;
	CreateThread(NULL, 0, EffectOverloadThread, data, 0, &currentEffect);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	static char lastKey;
	if (wParam == WM_KEYUP) {
		lastKey = 0;
	}
	if (wParam != WM_KEYDOWN) return CallNextHookEx(NULL, nCode, wParam, lParam);
	KBDLLHOOKSTRUCT *keyBoard = (KBDLLHOOKSTRUCT*)lParam;
	char pressedKey = (char)keyBoard->vkCode;
	if (pressedKey == lastKey) return CallNextHookEx(NULL, nCode, wParam, lParam);
	lastKey = pressedKey;
	switch (pressedKey) {
		case VK_F1:
			if (tempest && attunement == 1) {
				chroma.EffectOverload(fireColor, fireOI);
			} else if (attunement != 1) chroma.EffectAttunement(fireColor, fireSI);
			attunement = 1;
			break;
		case VK_F2:
			if (tempest && attunement == 2) {
				chroma.EffectOverload(waterColor, waterOI);
			} else if (attunement != 2)  chroma.EffectAttunement(waterColor, waterSI);
			attunement = 2;
			break;
		case VK_F3:
			if (tempest && attunement == 3) {
				chroma.EffectOverload(airColor, airOI);
			} else if (attunement != 3)  chroma.EffectAttunement(airColor, airSI);
			attunement = 3;
			break;
		case VK_F4:
			if (tempest && attunement == 4) {
				chroma.EffectOverload(earthColor, earthOI);
			} else if (attunement != 4)  chroma.EffectAttunement(earthColor, earthSI);
			attunement = 4;
			break;
		default: break;
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE:
			Hmenu = CreatePopupMenu();
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Quit"));
			break;
		case WM_SYSICON:
			if (lParam == WM_RBUTTONDOWN) {
				POINT curPoint;
				GetCursorPos(&curPoint);
				SetForegroundWindow(Hwnd);
				UINT clicked = TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hwnd, NULL);
				SendMessage(hwnd, WM_NULL, 0, 0);
				if (clicked == ID_TRAY_EXIT) {
					Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
					PostQuitMessage(0);
				}
			}
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	OutputDebugString(_T("ChromaEle starting\n"));
	fireColor = ColorFromIni(_T("colors"), _T(".\\chromaEle.ini"), _T("fireR"), _T("fireG"), _T("fireB"));
	waterColor = ColorFromIni(_T("colors"), _T(".\\chromaEle.ini"), _T("waterR"), _T("waterG"), _T("waterB"));
	airColor = ColorFromIni(_T("colors"), _T(".\\chromaEle.ini"), _T("airR"), _T("airG"), _T("airB"));
	earthColor = ColorFromIni(_T("colors"), _T(".\\chromaEle.ini"), _T("earthR"), _T("earthG"), _T("earthB"));

	fireSI = GetPrivateProfileInt(_T("effects"), _T("fireSwitchIntensity"), 0, _T(".\\chromaEle.ini"));
	waterSI = GetPrivateProfileInt(_T("effects"), _T("waterSwitchIntensity"), 0, _T(".\\chromaEle.ini"));
	airSI = GetPrivateProfileInt(_T("effects"), _T("airSwitchIntensity"), 0, _T(".\\chromaEle.ini"));
	earthSI = GetPrivateProfileInt(_T("effects"), _T("earthSwitchIntensity"), 0, _T(".\\chromaEle.ini"));

	fireOI = GetPrivateProfileInt(_T("effects"), _T("fireOverloadIntensity"), 0, _T(".\\chromaEle.ini"));
	waterOI = GetPrivateProfileInt(_T("effects"), _T("waterOverloadIntensity"), 0, _T(".\\chromaEle.ini"));
	airOI = GetPrivateProfileInt(_T("effects"), _T("airOverloadIntensity"), 0, _T(".\\chromaEle.ini"));
	earthOI = GetPrivateProfileInt(_T("effects"), _T("earthOverloadIntensity"), 0, _T(".\\chromaEle.ini"));

	tempest = GetPrivateProfileInt(_T("effects"), _T("tempest"), 0, _T(".\\chromaEle.ini"));
	checkGW2 = GetPrivateProfileInt(_T("effects"), _T("checkgw2"), 0, _T(".\\chromaEle.ini"));
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
