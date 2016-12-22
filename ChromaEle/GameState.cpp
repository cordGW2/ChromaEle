#include "stdafx.h"

#include "GameState.h"

GameState::GameState() {
}

BOOL GameState::isRunning() {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	do {
		if (!_stricmp(entry.szExeFile, "Gw2.exe") || !_stricmp(entry.szExeFile, "Gw2-64.exe")) {
			CloseHandle(snapshot);
			return true;
		}
	} while (Process32First(snapshot, &entry));
	CloseHandle(snapshot);
	return false;
}
