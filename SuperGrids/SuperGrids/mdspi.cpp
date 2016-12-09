#include "mdspi.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <windows.h>
#include "common.h"
#include "traderspi.h"

#pragma warning(disable : 4996)

extern int g_request_id;  
extern CThostFtdcTraderApi* pTraderUserApi;
extern CtpTraderSpi* pTraderUserSpi;
extern CThostFtdcMdApi* pMdUserApi;
extern CtpMdSpi* pMdUserSpi;

extern CThostFtdcBrokerUserPasswordField GUserInfo;

extern vector<CThostFtdcInvestorPositionDetailField*> posiDetailList;
extern vector<CThostFtdcDepthMarketDataField>  vcMarketData;

extern HANDLE g_hEvent;
extern HWND hLbl[20] ;
extern HWND hTxt[3] ;
extern HWND hWnd;

extern char* g_inst_id_list;
extern vector<char*> vcInstIDlist;

void CtpMdSpi::OnFrontDisconnected(int nReason)
{
	WriteLog("(Error)(OnFrontDisconnected)");
}
		
void CtpMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	WriteLog("(Hint)(OnHeartBeatWarning)");
}

void CtpMdSpi::OnFrontConnected()
{
	WriteLog("(Hint)(OnFrontConnected)Successed!");
	pMdUserSpi->ReqUserLogin(GUserInfo.BrokerID,GUserInfo.UserID,GUserInfo.Password);
	SetEvent(g_hEvent);
}

void CtpMdSpi::ReqUserLogin(TThostFtdcBrokerIDType	g_broker_id,
	        TThostFtdcUserIDType	g_user_id,	TThostFtdcPasswordType	g_password)
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, g_broker_id);
	strcpy(req.UserID, g_user_id);
	strcpy(req.Password, g_password);
	int ret = pUserApi->ReqUserLogin(&req, ++g_request_id);
	if (ret !=0)
		WriteLog("(Error)(OnRspUserLogin)Failed!");
	else
		WriteLog("ReqUserLogin ... Successed!");

	SetEvent(g_hEvent);
}

void CtpMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && pRspUserLogin)
	{
		WriteLog("(Hint)(OnRspUserLogin)Successed!");
		SendMessage(hTxt[2],WM_SETTEXT,0,(LPARAM)L"行情已登录");
	}
	else
		WriteLog("(Error)(OnRspUserLogin)Failed!");

	pMdUserSpi->SubscribeMarketData(g_inst_id_list);
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpMdSpi::SubscribeMarketData(char* g_inst_id_list)
{
	vector<char*> list;
	char *token = strtok(g_inst_id_list, ",");
	while( token != NULL )
	{
		list.push_back(token); 
		token = strtok(NULL, ",");
	}
	unsigned int len = list.size();
	char** pInstId = new char* [len];  
	for(unsigned int i=0; i<len;i++)  pInstId[i]=list[i];

	int ret=pUserApi->SubscribeMarketData(pInstId, len);

	if(ret != 0)
		WriteLog("(Error)(SubscribeMarketData)Failed!");
	else
		WriteLog("SubscribeMarketData ... Successed!");
	SetEvent(g_hEvent);
}

void CtpMdSpi::OnRspSubMarketData(
         CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
         CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//updateStatus(" 响应 | 行情订阅...成功");  //为什么这行会显示乱码？
	if(bIsLast)  SetEvent(g_hEvent);
}

void CtpMdSpi::OnRspUnSubMarketData(
             CThostFtdcSpecificInstrumentField *pSpecificInstrument,
             CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	WriteLog("(Hint)(OnRspUnSubMarketData)Successed!");
	if(bIsLast)  SetEvent(g_hEvent);
}

void SaveDataVec(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	CThostFtdcDepthMarketDataField pDepthMarketDataTemp;
	memset(&pDepthMarketDataTemp, 0, sizeof(pDepthMarketDataTemp));
	pDepthMarketDataTemp = *pDepthMarketData;

	vcMarketData.push_back(pDepthMarketDataTemp);	

}

void CtpMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	SaveDataVec(pDepthMarketData);

	//UpdateWindow(hWnd);
}

void CtpMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

bool CtpMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{	
	bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (!ret)
	{
		stringstream ss;
		ss << "(Error)(IsErrorRspInfo)" << pRspInfo->ErrorID << pRspInfo->ErrorMsg;
		WriteLog(ss.str());
	}
	return ret;
}

