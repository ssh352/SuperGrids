#ifndef _COMMON_H_
#define _COMMON_H_

#include "string.h"
#include <iostream>
#include "windows.h"

using namespace std;

struct TradingInfo
{
	bool EnableFlag;
	double LastBuyPrice;
	double LastSellPrice;
	double LastPrice;
	double GridSize;
	int BuyPosition;
	int SellPosition;
	int MaxPositions;
	int DifferenceLongShort;
	int LastOffsetFlag;
	int LastDirection;
	int TrxVolumn;
};

string GetConfigSetting(char* p_key);

string GetNow();

void showText(HWND hWnd,string s);

void WriteLog(string s);

void showPrice(HWND hwnd,double price);


#endif