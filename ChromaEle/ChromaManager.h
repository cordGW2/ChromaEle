#pragma once

#ifndef DLL_COMPILED
#define DLL_INTERNAL __declspec( dllexport )
#endif 

struct effect_data {
	COLORREF color;
	BYTE intensity;
};

class ChromaManager {
public:
	BOOL Initialize();
	void EffectAttunement(COLORREF color, BYTE intensity);
	void EffectOverload(COLORREF color, BYTE intensity);
private:
	HMODULE chromaSDKModule;
};

