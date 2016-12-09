#ifndef _SUPERGRIDS_H_
#define _SUPERGRIDS_H_

#pragma once

#include "windows.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
#include <string>
#include <Commctrl.h>
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include "mdspi.h"
#include "traderspi.h"
#include "common.h"
#include "resource.h"
#include "sqlite3.h"
#include "dbhelper.h"

using namespace std;

char dbFile[] = "trxdb.db";//数据库文件路径及名称
sqlite3 * pDB;
HANDLE g_hEvent;

int g_request_id=0;
char* g_md_front = new char[200];
char* g_td_front = new char[200];
char* g_inst_id_list = new char[200];

CThostFtdcBrokerUserPasswordField GUserInfo;
TradingInfo GTradingInfo;

HWND hWnd;
HWND hTxt[3];
HWND hLbl[2];
HWND hBtn[4];
HWND hLv;

vector<string> vcLog;
vector<char*>  vcInstIDlist;
vector<CThostFtdcDepthMarketDataField>  vcMarketData;

string sLog = "";

#endif