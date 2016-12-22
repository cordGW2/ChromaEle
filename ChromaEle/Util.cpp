#include "stdafx.h"

#include "Util.h"

COLORREF ColorFromIni(LPCTSTR section, LPCTSTR ini, LPCTSTR nameR, LPCTSTR nameG, LPCTSTR nameB) {
	COLORREF color;
	BYTE r, g, b;
	r = GetPrivateProfileInt(section, nameR, 0, ini);
	g = GetPrivateProfileInt(section, nameG, 0, ini);
	b = GetPrivateProfileInt(section, nameB, 0, ini);
	color = RGB(r, g, b);
	return color;
}