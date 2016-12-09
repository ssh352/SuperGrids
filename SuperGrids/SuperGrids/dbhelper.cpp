#include "dbhelper.h"
extern sqlite3 * pDB;
extern char dbFile[];
extern time_t t; 
extern char currentDateTime[64]; 
extern char* datetimeFormat;

int openDb()
{
	int res = sqlite3_open(dbFile, &pDB);
	if( res )
	{
	//t = time(0);
	//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
	//cout << currentDateTime << "不能打开数据库: "<< sqlite3_errmsg(pDB);
		WriteLog("[Error][cannot open db!]");
		sqlite3_close(pDB);
		return -1;
	}
	else
	{	
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << "数据库已打开!" << endl;
		WriteLog("[Hint][db is opened!]");
	}
	return 0;
}

int insertTicks(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	stringstream strsql;	
	strsql << "INSERT INTO TICKS(TRADING_DAY,UPDATE_TIME,UPDATE_MILLI_SEC,INSTRUMENT_ID,"
		<< "LAST_PRICE,PRE_SETTLEMENT_PRICE,PRE_CLOSE_PRICE,PRE_OPEN_INTEREST,"
		<< "OPEN_PRICE,HIGHEST_PRICE,LOWEST_PRICE,VOLUME,TURNOVER,OPEN_INTEREST,"
		<< "CLOSE_PRICE,SETTLEMENT_PRICE,UPPER_LIMIT_PRICE,LOWER_LIMIT_PRICE,"
		<< "PRE_DELTA,CURR_DELTA,"
		<< "BID_PRICE1,BID_VOLUME1,ASK_PRICE1,ASK_VOLUME1,"
		<< "BID_PRICE2,BID_VOLUME2,ASK_PRICE2,ASK_VOLUME2,"
		<< "BID_PRICE3,BID_VOLUME3,ASK_PRICE3,ASK_VOLUME3,"
		<< "BID_PRICE4,BID_VOLUME4,ASK_PRICE4,ASK_VOLUME4,"
		<< "BID_PRICE5,BID_VOLUME5,ASK_PRICE5,ASK_VOLUME5"
		<< ") VALUES('"
		<< pDepthMarketData->TradingDay << "','" << pDepthMarketData->UpdateTime << "','" << pDepthMarketData->UpdateMillisec << "','" << pDepthMarketData->InstrumentID << "'," 
		<< pDepthMarketData->LastPrice << "," << pDepthMarketData->PreSettlementPrice << "," << pDepthMarketData->PreClosePrice << "," << pDepthMarketData->PreOpenInterest << "," 
		<< pDepthMarketData->OpenPrice << "," << pDepthMarketData->HighestPrice << "," << pDepthMarketData->LowestPrice << "," << pDepthMarketData->Volume << "," << pDepthMarketData->Turnover << ","
		<< pDepthMarketData->OpenInterest << "," << pDepthMarketData->ClosePrice << "," << pDepthMarketData->SettlementPrice << "," << pDepthMarketData->UpperLimitPrice << "," << pDepthMarketData->LowerLimitPrice << ","
		<< pDepthMarketData->PreDelta << "," << pDepthMarketData->CurrDelta << "," << pDepthMarketData->BidPrice1 << "," << pDepthMarketData->BidVolume1 << "," << pDepthMarketData->AskPrice1 << "," << pDepthMarketData->AskVolume1 << ","
		<< pDepthMarketData->BidPrice2 << "," << pDepthMarketData->BidVolume2 << "," << pDepthMarketData->AskPrice2 << "," << pDepthMarketData->AskVolume2 << ","
		<< pDepthMarketData->BidPrice3 << "," << pDepthMarketData->BidVolume3 << "," << pDepthMarketData->AskPrice3 << "," << pDepthMarketData->AskVolume3 << ","
		<< pDepthMarketData->BidPrice4 << "," << pDepthMarketData->BidVolume4 << "," << pDepthMarketData->AskPrice4 << "," << pDepthMarketData->AskVolume4 << ","
		<< pDepthMarketData->BidPrice5 << "," << pDepthMarketData->BidVolume5 << "," << pDepthMarketData->AskPrice5 << "," << pDepthMarketData->AskVolume5 << ")";
		
	char* errMsg;
	//cerr << strsql.str().c_str();
	int res= sqlite3_exec(pDB ,strsql.str().c_str() , 0 , 0 , &errMsg);
	if (res != SQLITE_OK)
	{
	//t = time(0);
	//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
	//cout << currentDateTime << "执行SQL 出错." << errMsg << std::endl;
		return -1;
	}
	return 0;
}

//static int callbackTradingDetails(void *NotUsed, int argc, char **argv, char **azColName)
//{
//	//for(int i = 0 ; i < argc ; i++)
//	//{
//	//	std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << ", " ;
//	//}
//	if(argv[0])
//		OpenPosi = atoi(argv[0]);
//	else
//		OpenPosi = 0;
//
//	return OpenPosi;
//}

int selectTradingDetails(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	char* errMsg;
	stringstream strsql;
	if(pDepthMarketData->AskPrice1 == DBL_MAX || pDepthMarketData->AskPrice1 == DBL_MIN)
	{
		pDepthMarketData->AskPrice1 = 0;
		cout << "pDepthMarketData->AskPrice1 = 0;" << endl;

	}
	if(pDepthMarketData->BidPrice1 == DBL_MAX || pDepthMarketData->BidPrice1 == DBL_MIN)
	{
		pDepthMarketData->BidPrice1 = 0;
		cout << "pDepthMarketData->BidPrice1 = 0;" << endl;

	}
	strsql << "SELECT SUM(ENTER_VOLUME-EXIT_VOLUME) FROM TRADE_DETAILS WHERE INSTRUMENT_ID='" << pDepthMarketData->InstrumentID << "'"
		<< " AND ENTER_VOLUME > EXIT_VOLUME AND ENTER_PRICE >= " << pDepthMarketData->BidPrice1 - 50
		<< " AND ENTER_PRICE <= "  << pDepthMarketData->AskPrice1 + 150;
	int OpenPosi = 0;//当前持仓量
	char* cSelect = new char[1024];
	//cout << strsql.str().c_str() << endl;
	//cout << strlen(strsql.str().c_str()) << endl;
	strcpy_s(cSelect,strlen(strsql.str().c_str())+1, strsql.str().c_str());
	OpenPosi = getOneIntValue(cSelect);
	//cout << "OpenPosi ：" << OpenPosi << endl;	
	return OpenPosi;
}

int getOneIntValue(char* cSelect)
{
	sqlite3_stmt * stmt;
	const char *zTail;
	int res = sqlite3_prepare(pDB,cSelect, -1,&stmt,&zTail);
	if(res == SQLITE_OK)
	{
		res = sqlite3_step(stmt);
		while (res == SQLITE_ROW)
		{
			//cout << "res = " << res << "  SQLITE_ROW=" << SQLITE_ROW << ":: " << cSelect << endl;
			res = sqlite3_column_int( stmt,0 );
			sqlite3_finalize(stmt);
			return res;
		}
	}
	return 0;
}

char* getOneTextValue(char* cSelect)
{
	sqlite3_stmt * stmt;
	const char *zTail;
	int res = sqlite3_prepare(pDB,cSelect, -1,&stmt,&zTail);
	if(res == SQLITE_OK)
	{
		res = sqlite3_step(stmt);
		while (res == SQLITE_ROW)
		{
			//cout << "res = " << res << "  SQLITE_ROW=" << SQLITE_ROW << ":: " << cSelect << endl;
			char *text = (char *)sqlite3_column_text( stmt,0 );
			//cout << "text = " << text << endl;
			char *text2 = new char[strlen(text)];
			strcpy_s(text2,strlen(text)+1,text);
			sqlite3_finalize(stmt);
			return text2;
		}
	}
	return "0";
}