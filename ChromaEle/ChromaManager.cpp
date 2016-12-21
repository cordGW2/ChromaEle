#include "stdafx.h"

#include "ChromaManager.h"

#ifdef _WIN64
#define CHROMASDKDLL        _T("RzChromaSDK64.dll")
#else
#define CHROMASDKDLL        _T("RzChromaSDK.dll")
#endif

typedef RZRESULT(*INIT)(void);
typedef RZRESULT(*CREATEKEYBOARDEFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEMOUSEEFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);

INIT Init = nullptr;
CREATEKEYBOARDEFFECT CreateKeyboardEffect = nullptr;
CREATEMOUSEEFFECT CreateMouseEffect = nullptr;

DWORD currentEffect = 0;

BOOL ChromaManager::Initialize() {
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

DWORD WINAPI EffectAttunementThread(LPVOID lpParam) {
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

void ChromaManager::EffectAttunement(COLORREF color, BYTE intensity) {
	effect_data* data = new effect_data();
	data->color = color;
	data->intensity = intensity;
	CreateThread(NULL, 0, EffectAttunementThread, data, 0, &currentEffect);
}

DWORD WINAPI EffectOverloadThread(LPVOID lpParam) {
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

void ChromaManager::EffectOverload(COLORREF color, BYTE intensity) {
	effect_data* data = new effect_data();
	data->color = color;
	data->intensity = intensity;
	CreateThread(NULL, 0, EffectOverloadThread, data, 0, &currentEffect);
}
