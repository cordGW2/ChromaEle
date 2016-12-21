#include "stdafx.h"

#include "Settings.h"

#include "Util.h"

Settings::Settings(LPCSTR ini){
	this->ini = ini;
	fireColor = ColorFromIni(_T("colors"), ini, _T("fireR"), _T("fireG"), _T("fireB"));
	waterColor = ColorFromIni(_T("colors"), ini, _T("waterR"), _T("waterG"), _T("waterB"));
	airColor = ColorFromIni(_T("colors"), ini, _T("airR"), _T("airG"), _T("airB"));
	earthColor = ColorFromIni(_T("colors"), ini, _T("earthR"), _T("earthG"), _T("earthB"));

	fireSI = GetPrivateProfileInt(_T("effects"), _T("fireSwitchIntensity"), 0, ini);
	waterSI = GetPrivateProfileInt(_T("effects"), _T("waterSwitchIntensity"), 0, ini);
	airSI = GetPrivateProfileInt(_T("effects"), _T("airSwitchIntensity"), 0, ini);
	earthSI = GetPrivateProfileInt(_T("effects"), _T("earthSwitchIntensity"), 0, ini);

	fireOI = GetPrivateProfileInt(_T("effects"), _T("fireOverloadIntensity"), 0, ini);
	waterOI = GetPrivateProfileInt(_T("effects"), _T("waterOverloadIntensity"), 0, ini);
	airOI = GetPrivateProfileInt(_T("effects"), _T("airOverloadIntensity"), 0, ini);
	earthOI = GetPrivateProfileInt(_T("effects"), _T("earthOverloadIntensity"), 0, ini);

	tempest = GetPrivateProfileInt(_T("effects"), _T("tempest"), 0, ini);
	checkGW2 = GetPrivateProfileInt(_T("effects"), _T("checkgw2"), 0, ini);
}

COLORREF Settings::getFireColor() {
	return fireColor;
}

COLORREF Settings::getWaterColor() {
	return waterColor;
}

COLORREF Settings::getAirColor() {
	return airColor;
}

COLORREF Settings::getEarthColor() {
	return earthColor;
}

BYTE Settings::getFireSI() {
	return fireSI;
}

BYTE Settings::getWaterSI() {
	return waterSI;
}

BYTE Settings::getAirSI() {
	return airSI;
}

BYTE Settings::getEarthSI() {
	return earthSI;
}

BYTE Settings::getFireOI() {
	return fireOI;
}

BYTE Settings::getWaterOI() {
	return waterOI;
}

BYTE Settings::getAirOI() {
	return airOI;
}

BYTE Settings::getEarthOI() {
	return earthOI;
}

BOOL Settings::getTempest() {
	return tempest;
}

void Settings::setTempest(BOOL tempest) {
	this->tempest = tempest;
	WritePrivateProfileString(TEXT("effects"), TEXT("tempest"), TEXT(tempest ? "1" : "0"), ini);
}

BOOL Settings::getCheckGW2() {
	return checkGW2;
}
