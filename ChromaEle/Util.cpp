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

bool IsProcessRunning(const char* processName) {
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp(entry.szExeFile, processName))
				exists = true;
	CloseHandle(snapshot);
	return exists;
}