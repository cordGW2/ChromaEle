#pragma once

COLORREF ColorFromIni(LPCTSTR section, LPCTSTR ini, LPCTSTR nameR, LPCTSTR nameG, LPCTSTR nameB);
bool IsProcessRunning(const char* processName);