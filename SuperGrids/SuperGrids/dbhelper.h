#include "sqlite3.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "windows.h"
#include <float.h>
#include <string>
#include <cstring>
#include "time.h"
#include "windows.h"
#include "Common.h"
#include <iostream>
#include <fstream>

using namespace std;

int openDb();

int insertTicks(CThostFtdcDepthMarketDataField *pDepthMarketData);
int selectTradingDetails(CThostFtdcDepthMarketDataField *pDepthMarketData);
int insertTradingDetails(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail);

char* getConfig(char* configCode);
int getOneIntValue(char* cSelect);
char* getOneTextValue(char* cSelect);
