#pragma once

class Settings {
public:
	Settings(LPCSTR ini);
	COLORREF getFireColor();
	COLORREF getWaterColor();
	COLORREF getAirColor();
	COLORREF getEarthColor();
	BYTE getFireSI();
	BYTE getWaterSI();
	BYTE getAirSI();
	BYTE getEarthSI();
	BYTE getFireOI();
	BYTE getWaterOI();
	BYTE getAirOI();
	BYTE getEarthOI();
	BOOL getTempest();
	void setTempest(BOOL tempest);
	BOOL getCheckGW2();
private:
	LPCSTR ini;
	COLORREF fireColor, waterColor, airColor, earthColor;
	BYTE fireSI, waterSI, airSI, earthSI;
	BYTE fireOI, waterOI, airOI, earthOI;
	BOOL tempest, checkGW2;
};

