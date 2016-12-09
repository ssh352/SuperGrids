#include "traderspi.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "ThostFtdcTraderApi.h"
#include "windows.h"
#include "common.h"

#pragma warning(disable :4996)

using namespace std;
// 会话参数
int	 frontId;	//前置编号
int	 sessionId;	//会话编号
char orderRef[13];

vector<CThostFtdcOrderField*> orderList;
vector<CThostFtdcTradeField*> tradeList;
vector<CThostFtdcInvestorPositionDetailField*> posiDetailList;

extern vector<char*> vcInstIDlist;
    
extern int g_request_id;  
extern CThostFtdcTraderApi* pTraderUserApi;
extern CtpTraderSpi* pTraderUserSpi;

extern CThostFtdcBrokerUserPasswordField GUserInfo;
extern TradingInfo GTradingInfo;
extern char** g_inst_id_list;

extern HINSTANCE hInst;	// 当前实例
extern HANDLE g_hEvent;
extern HWND hWnd;
extern HWND hTxtMd;
extern HWND hTxtTd;
extern HWND hLvRunLog;
extern HWND hLbl[20];
extern HWND hTxt[3];


void CtpTraderSpi::OnFrontConnected()
{
	WriteLog("[Hint][OnFrontConnected]Successed!");
	//setLvRunLog(hLvRunLog,"OnFrontConnected","","","Successed","");
	pTraderUserSpi->ReqUserLogin(GUserInfo.BrokerID,GUserInfo.UserID,GUserInfo.Password);
	SetEvent(g_hEvent);
}

void CtpTraderSpi::ReqUserLogin(TThostFtdcBrokerIDType	vbrokerId,
	        TThostFtdcUserIDType	vUserId,	TThostFtdcPasswordType	vPasswd)
{
  
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, vbrokerId); 
	strcpy(req.UserID, vUserId);  
	strcpy(req.Password, vPasswd);
	int ret = pTraderUserApi->ReqUserLogin(&req, ++g_request_id);

	if(ret != 0)
		WriteLog("[Warning][ReqUserLogin]Failed!");
	else
		WriteLog("[Hint][ReqUserLogin]Successed!");
}

void CtpTraderSpi::ReqQryInvestorPosition(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInvestorPositionField req;
	//CThostFtdcQryInvestorPositionDetailField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, GUserInfo.BrokerID);
	strcpy(req.InvestorID, GUserInfo.UserID);
	strcpy(req.InstrumentID, instId);	
	int ret = pTraderUserApi->ReqQryInvestorPosition(&req, ++g_request_id);
	if (ret != 0)
		WriteLog("[Warning][ReqQryInvestorPosition]Failed!");
	else
		WriteLog("[Hint][ReqQryInvestorPosition]Successed!");
	//vecIPF.clear();
}

int CtpTraderSpi::ReqQryInvestorPositionDetail(CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID)
{
	//cout << pQryInvestorPositionDetail->g_broker_id << " " << pQryInvestorPositionDetail->InvestorID  << " " << pQryInvestorPositionDetail->InstrumentID << endl;
	return 0;
}


void CtpTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ( !IsErrorRspInfo(pRspInfo) && pRspUserLogin ) 
	{  
		 // 保存会话参数	
		frontId = pRspUserLogin->FrontID;
		sessionId = pRspUserLogin->SessionID;
		int nextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		sprintf(orderRef, "%d", ++nextOrderRef);
		WriteLog("[Hint][OnRspUserLogin]Successed!");
		SendMessage(hTxt[2],WM_SETTEXT,0,(LPARAM)L"交易已登录");
		//确认结算
		ReqSettlementInfoConfirm();
	}
	else
	{
		WriteLog("[Warning][OnRspUserLogin]Failed! ");
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, GUserInfo.BrokerID);
	strcpy(req.InvestorID, GUserInfo.UserID);
	int ret = pTraderUserApi->ReqSettlementInfoConfirm(&req, ++g_request_id);
	if (ret != 0)
		WriteLog("[Warning][ReqSettlementInfoConfirm]Failed!");
	else
		WriteLog("[Hint][ReqSettlementInfoConfirm]Successed!");
}

void CtpTraderSpi::OnRspSettlementInfoConfirm(
        CThostFtdcSettlementInfoConfirmField  *pSettlementInfoConfirm, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	if( !IsErrorRspInfo(pRspInfo) && pSettlementInfoConfirm)
	{
		WriteLog("[Hint][OnRspSettlementInfoConfirm]Successed! ");

		//ReqQryTradingAccount();
		//ReqQryInvestorPosition("rb1601");
		//ReqQryInvestorPositionDetail("rb1601");
	}
	else
	{
		stringstream ss;
		ss << "[Waring][OnRspSettlementInfoConfirm]Failed! " << pRspInfo->ErrorID << pRspInfo->ErrorMsg << endl;
		WriteLog(ss.str());
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTraderSpi::ReqQryInstrument(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, instId);//为空表示查询所有合约
	int ret = pTraderUserApi->ReqQryInstrument(&req, ++g_request_id);
	if (ret != 0)
		WriteLog("[Warning][ReqQryInstrument]Failed!");
	else
		WriteLog("[Hint][ReqQryInstrument]Successed!");
}

void CtpTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, 
         CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 	
	if ( !IsErrorRspInfo(pRspInfo) &&  pInstrument){
	//t = time(0);
	//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
	//cout << currentDateTime << " 响应 | 合约:"<<pInstrument->InstrumentID
 //     <<" 交割月:"<<pInstrument->DeliveryMonth
 //     <<" 多头保证金率:"<<pInstrument->LongMarginRatio
 //     <<" 空头保证金率:"<<pInstrument->ShortMarginRatio<<endl;    
  }
  if(bIsLast) SetEvent(g_hEvent);
}

void CtpTraderSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, GUserInfo.BrokerID);
	strcpy(req.InvestorID, GUserInfo.UserID);
	int ret = pTraderUserApi->ReqQryTradingAccount(&req, ++g_request_id);
	if (ret != 0)
		WriteLog("[Hint][ReqQryTradingAccount]Failed!");
	else
		WriteLog("[Hint][ReqQryTradingAccount]Successed!");
}

void CtpTraderSpi::OnRspQryTradingAccount(
    CThostFtdcTradingAccountField *pTradingAccount, 
   CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
	if (!IsErrorRspInfo(pRspInfo) &&  pTradingAccount)
	{
		
		stringstream ss;
		ss << "[Hint][OnRspQryTradingAccount]<动态权益：" << std::fixed << pTradingAccount->Balance
			<< "; 可用资金： " << std::fixed << pTradingAccount->Available
			<<"; 占用保证金： "<<pTradingAccount->CurrMargin
			<<"; 持仓盈亏： "<<pTradingAccount->PositionProfit 
			<< ">";
		string sTemp;
		sTemp = ss.str();
		WriteLog(sTemp);

		//setLvRunLog(hLvRunLog, "OnRspQryTradingAccount", "", "", sTemp, "");
  }
  else
	  WriteLog("[Hint][OnRspQryTradingAccount]Failed!");
  if(bIsLast) SetEvent(g_hEvent);
}



void CtpTraderSpi::OnRspQryInvestorPosition(
    CThostFtdcInvestorPositionField *pInvestorPosition, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
  if( !IsErrorRspInfo(pRspInfo) &&  pInvestorPosition )
  {
	  //stringstream ss;
	  //ss << "OnRspQryInvestorPosition : InstrumentID = " << pInvestorPosition->InstrumentID
		 // << "; 方向 = " << MapDirection(pInvestorPosition->PosiDirection - 2, false)
		 // << "; 总持仓 = " << pInvestorPosition->Position
		 // << "; 昨仓 = " << pInvestorPosition->YdPosition
		 // << "; 今仓 = " << pInvestorPosition->TodayPosition
		 // << "; 持仓盈亏 = " << pInvestorPosition->PositionProfit
		 // << "; 保证金 = " << pInvestorPosition->UseMargin;
	  //string sTemp;
	  //sTemp = ss.str();
	  //WriteLog(sTemp);
	  //CThostFtdcInvestorPositionField* ipf = new CThostFtdcInvestorPositionField();
	  //memcpy(ipf, pInvestorPosition, sizeof(CThostFtdcInvestorPositionField));
	  //vecIPF.push_back(ipf);
  }
  else
  {
	  stringstream ss;
	  ss << "[Error][OnRspQryInvestorPosition]" << pRspInfo->ErrorID << pRspInfo->ErrorMsg << endl;
	  WriteLog(ss.str());
  }
  if (bIsLast)
  {
	  SetEvent(g_hEvent);
  }
}

void CtpTraderSpi::ReqQryInvestorPositionDetail(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInvestorPositionDetailField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, GUserInfo.BrokerID);
	strcpy(req.InvestorID, GUserInfo.UserID);
	strcpy(req.InstrumentID, instId);	
	int ret = pTraderUserApi->ReqQryInvestorPositionDetail(&req, ++g_request_id);
	if(ret!=0 && GTradingInfo.EnableFlag==true)
		WriteLog("[Error][ReqQryInvestorPositionDetail]");
	GTradingInfo.BuyPosition = 0;
	GTradingInfo.SellPosition = 0;
}


///查询持仓明细
void CtpTraderSpi::OnRspQryInvestorPositionDetail(
    CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
  if( !IsErrorRspInfo(pRspInfo) &&  pInvestorPositionDetail )
  {
	//stringstream ss;
	//ss<<"OnRspQryInvestorPositionDetail:合约:"<<pInvestorPositionDetail->InstrumentID
	//<<" 方向:"<<MapDirection(pInvestorPositionDetail->Direction,false)
	//<<" 持仓:"<<pInvestorPositionDetail->Volume
	//<<" 开仓日期:"<<pInvestorPositionDetail->OpenDate 
	//<<" 开仓价格:"<<pInvestorPositionDetail->OpenPrice
	//<<" 交易日期:"<<pInvestorPositionDetail->TradingDay
	//<<" 成交编号:"<<pInvestorPositionDetail->TradeID <<endl;
	//string sTemp;
	//sTemp = ss.str();
	//WriteLog(sTemp);
	//将持仓明细存入vector
	CThostFtdcInvestorPositionDetailField* pdf = new CThostFtdcInvestorPositionDetailField();
	memcpy(pdf,pInvestorPositionDetail,sizeof(CThostFtdcInvestorPositionDetailField));
	posiDetailList.push_back(pdf); 

		if(strcmp(pdf->InstrumentID,"rb1601") == 0)
			if(pdf->Direction == THOST_FTDC_D_Buy)
				GTradingInfo.BuyPosition = GTradingInfo.BuyPosition + pdf->Volume;
			else
				if(pdf->Direction == THOST_FTDC_D_Sell)
					GTradingInfo.SellPosition = GTradingInfo.SellPosition + pdf->Volume;
  }
  if(bIsLast) 
  {
	  stringstream ss;
	  ss << "[Hint][OnRspQryInvestorPositionDetail]买持仓：" << GTradingInfo.BuyPosition << "；卖持仓：" << GTradingInfo.SellPosition << endl;
	  WriteLog(ss.str());
	  SetEvent(g_hEvent);
	}
}
  int CtpTraderSpi::ReqOrderInsert(TThostFtdcInstrumentIDType instId,
    TThostFtdcDirectionType dir, TThostFtdcCombOffsetFlagType kpp,
    TThostFtdcPriceType price,   TThostFtdcVolumeType vol)
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));	
	strcpy(req.BrokerID, GUserInfo.BrokerID);  //应用单元代码	
	strcpy(req.InvestorID, GUserInfo.UserID); //投资者代码	
	strcpy(req.InstrumentID, instId); //合约代码	
	strcpy(req.OrderRef, orderRef);  //报单引用
	
  int nextOrderRef = atoi(orderRef);
  sprintf(orderRef, "%d", ++nextOrderRef);
  
  req.LimitPrice = price;	//价格
  if(0==req.LimitPrice)
  {
	  req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;//价格类型=市价
	  req.TimeCondition = THOST_FTDC_TC_IOC;//有效期类型:立即完成，否则撤销
  }
  else
  {
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;//价格类型=限价	
    req.TimeCondition = THOST_FTDC_TC_GFD;  //有效期类型:当日有效
  }

	if('b'==dir||'B'==dir) 
		req.Direction = THOST_FTDC_D_Buy;  //买卖方向
	else 
		if('s'==dir||'S'==dir) 
			req.Direction = THOST_FTDC_D_Sell;


  if(kpp[0]=='O' || kpp[0]=='o')
	  req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
  else
	  if(kpp[0]=='C' || kpp[0]=='c')
		  req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	  else
		  if(kpp[0]=='J' || kpp[0]=='j')
			  req.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	      else
			  req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;

	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	  //组合投机套保标志	
	req.VolumeTotalOriginal = vol;	///数量		
	req.VolumeCondition = THOST_FTDC_VC_AV; //成交量类型:任何数量
	req.MinVolume = 1;	//最小成交量:1	
	req.ContingentCondition = THOST_FTDC_CC_Immediately;  //触发条件:立即
	
  //TThostFtdcPriceType	StopPrice;  //止损价
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;	//强平原因:非强平	
	req.IsAutoSuspend = 0;  //自动挂起标志:否	
	req.UserForceClose = 0;   //用户强评标志:否

	int ret = pTraderUserApi->ReqOrderInsert(&req, ++g_request_id);
	//cerr<<" 请求 | 发送报单..."<<((ret == 0)?"成功":"失败")<< endl;
	//string sTemp;
	//if (ret == 0)
	//	sTemp = "ReqOrderInsert...成功";
	//else
	//	sTemp = "ReqOrderInsert...失败";
	//WriteLog(sTemp);
	return ret;
}

//经测试ReqOrderInsert下单成功后不执行OnRspOrderInsert回调函数
void CtpTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
          CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInputOrder )
	{
		//stringstream ss;
		//ss << "OnRspOrderInsert...成功.报单引用:" << pInputOrder->OrderRef ;
		//WriteLog(ss.str());
		MessageBox(hWnd,L"OnRspOrderInsert",L"OnRspOrderInsert",NULL);
		CThostFtdcInvestorPositionDetailField pdf;
		memset(&pdf, 0, sizeof(pdf));
		strcpy(pdf.BrokerID, pInputOrder->BrokerID);
		strcpy(pdf.InvestorID, pInputOrder->InvestorID);
		strcpy(pdf.InstrumentID , pInputOrder->InstrumentID);
		strcpy(pdf.OpenDate , pInputOrder->GTDDate);
		pdf.Direction = pInputOrder->Direction;
		pdf.OpenPrice = pInputOrder->LimitPrice;
		pdf.Volume = pInputOrder->VolumeTotalOriginal;
	}
	else
	{
		stringstream ss;
		ss << "[Error][OnRspOrderInsert]" << pRspInfo->ErrorID << pRspInfo->ErrorMsg
		<< ";InstrumentID:" << pInputOrder->InstrumentID
		<< ";Direction:"<< pInputOrder->Direction 
		<< ";CombOffsetFlag:" << pInputOrder->CombOffsetFlag
		<< ";VolumeTotalOriginal:" << pInputOrder->VolumeTotalOriginal;
		WriteLog(ss.str());
	}
	if(bIsLast) SetEvent(g_hEvent);	
}


bool CtpTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if(ret)
	{
		stringstream ss;
		ss << "[Error][IsErrorRspInfo]" << pRspInfo->ErrorID << pRspInfo->ErrorMsg;
		WriteLog(ss.str());
	}
	return ret;
}

void CtpTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	if( !IsErrorRspInfo(pRspInfo) && pInputOrder )
	{
		stringstream ss;
		ss << "[Error][OnErrRtnOrderInsert]" << pRspInfo->ErrorID << pRspInfo->ErrorMsg;
		WriteLog(ss.str());
	}
}

void CtpTraderSpi::ReqOrderAction(TThostFtdcSequenceNoType orderSeq)
{
	bool found=false; unsigned int i=0;
	for(i=0;i<orderList.size();i++){
	if(orderList[i]->BrokerOrderSeq == orderSeq){ found = true; break;}
	}
	if(!found)
	{	
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " 请求 | 报单不存在."<<endl; return;
	} 

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, GUserInfo.BrokerID);   //经纪公司代码	
	strcpy(req.InvestorID, GUserInfo.UserID); //投资者代码
	//strcpy(req.OrderRef, pOrderRef); //报单引用	
	//req.FrontID = frontId;           //前置编号	
	//req.SessionID = sessionId;       //会话编号
  strcpy(req.ExchangeID, orderList[i]->ExchangeID);
  strcpy(req.OrderSysID, orderList[i]->OrderSysID);
	req.ActionFlag = THOST_FTDC_AF_Delete;  //操作标志 

	int ret = pTraderUserApi->ReqOrderAction(&req, ++g_request_id);
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " 请求 | 发送撤单..." <<((ret == 0)?"成功":"失败") << endl;
}

void CtpTraderSpi::OnRspOrderAction(
      CThostFtdcInputOrderActionField *pInputOrderAction, 
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
  if (!IsErrorRspInfo(pRspInfo) && pInputOrderAction){
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " 响应 | 撤单成功..."
  //    << "交易所:"<<pInputOrderAction->ExchangeID
  //    <<" 报单编号:"<<pInputOrderAction->OrderSysID<<endl;
  }
  if(bIsLast) SetEvent(g_hEvent);	
}

///报单回报
void CtpTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{	
  CThostFtdcOrderField* order = new CThostFtdcOrderField();
  memcpy(order,  pOrder, sizeof(CThostFtdcOrderField));
  bool founded=false;    unsigned int i=0;
  for(i=0; i<orderList.size(); i++){
    if(orderList[i]->BrokerOrderSeq == order->BrokerOrderSeq) {
      founded=true;    break;
    }
  }
  if(founded) orderList[i]= order;   
  else  orderList.push_back(order);
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " 回报 | 报单已提交...序号:"<<order->BrokerOrderSeq << "；StatusMsg：" << order->StatusMsg<<endl;
  SetEvent(g_hEvent);	
}

///成交通知
void CtpTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
  CThostFtdcTradeField* trade = new CThostFtdcTradeField();
  memcpy(trade,  pTrade, sizeof(CThostFtdcTradeField));
  bool founded=false;     unsigned int i=0;

  //pTraderUserSpi->ReqQryInvestorPositionDetail(vcInstIDlist[0]);
  //stringstream ss;
  //ss << "gBuyPosition:" << gBuyPosition  << " gSellPosition:" << gSellPosition ;
  //if(gEnableFlag)
	 // MessageBox(NULL,string2lpcwstr(ss.str()),L"Position",MB_OK);

  if (GTradingInfo.EnableFlag && abs(GTradingInfo.BuyPosition-GTradingInfo.SellPosition) <= GTradingInfo.MaxPositions/2 && GTradingInfo.BuyPosition+GTradingInfo.SellPosition <= GTradingInfo.MaxPositions)
	  if(strcmp(pTrade->InstrumentID,vcInstIDlist[0])==0)
		  if(pTrade->Direction == THOST_FTDC_D_Buy)
			  if(pTrade->OffsetFlag == THOST_FTDC_OF_Open)
			  {
				  
				  GTradingInfo.BuyPosition = GTradingInfo.BuyPosition + pTrade->Volume;
				  GTradingInfo.LastDirection = THOST_FTDC_D_Buy;
				  GTradingInfo.LastOffsetFlag = THOST_FTDC_OF_Open;
				  GTradingInfo.LastBuyPrice = trade->Price;
				  if (GTradingInfo.EnableFlag)
				  {
					int ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'S', "J", GTradingInfo.LastBuyPrice + GTradingInfo.GridSize,GTradingInfo.TrxVolumn);
					//ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'S', "O", gLastBuyPrice + gGridSize,gTrxVolumn);
					//ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'B', "O", gLastBuyPrice - gGridSize,gTrxVolumn);
				  }
			  }
			  else
			  {
				  GTradingInfo.SellPosition = GTradingInfo.SellPosition - pTrade->Volume;
				  GTradingInfo.LastDirection = THOST_FTDC_D_Buy;
				  GTradingInfo.LastOffsetFlag = THOST_FTDC_OF_Close;
				  GTradingInfo.LastBuyPrice = trade->Price;

				  if (GTradingInfo.EnableFlag)
				  {
					  //int ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'B', "O", gLastBuyPrice - gGridSize,gTrxVolumn);
					  int ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'S', "O", GTradingInfo.LastBuyPrice + GTradingInfo.GridSize,GTradingInfo.TrxVolumn);
				  }
			  }
		  else
			  if(pTrade->Direction == THOST_FTDC_D_Sell)
				  if(pTrade->OffsetFlag == THOST_FTDC_OF_Open)
				  {
					  GTradingInfo.SellPosition = GTradingInfo.SellPosition + pTrade->Volume;
					  GTradingInfo.LastDirection = THOST_FTDC_D_Sell;
					  GTradingInfo.LastOffsetFlag = THOST_FTDC_OF_Open;
					  GTradingInfo.LastSellPrice = trade->Price;

					  if (GTradingInfo.EnableFlag)
					  {
						  int ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'B', "J", GTradingInfo.LastSellPrice - GTradingInfo.GridSize,GTradingInfo.TrxVolumn);
						  //ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'B', "O", gLastSellPrice - gGridSize,gTrxVolumn);
						  //ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'B', "S", gLastSellPrice + gGridSize,gTrxVolumn);
					  }
				  }
				  else
				  {
					  GTradingInfo.BuyPosition = GTradingInfo.BuyPosition - pTrade->Volume;
					  GTradingInfo.LastDirection = THOST_FTDC_D_Sell;
					  GTradingInfo.LastOffsetFlag = THOST_FTDC_OF_Close;
					  GTradingInfo.LastSellPrice = trade->Price;

					  if (GTradingInfo.EnableFlag)
					  {
						  //int ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'S', "O", gLastSellPrice + gGridSize,gTrxVolumn);
						  int ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'B', "O", GTradingInfo.LastSellPrice - GTradingInfo.GridSize,GTradingInfo.TrxVolumn);
					  }
				  }

	//stringstream ss1;
	//ss1 << "gBuyPosition:" << gBuyPosition  << " gSellPosition:" << gSellPosition ;
	//if(gEnableFlag)
	//	MessageBox(NULL,string2lpcwstr(ss1.str()),L"Position",MB_OK);
	
  showPrice(hLbl[22],GTradingInfo.LastPrice);
  showPrice(hLbl[23],GTradingInfo.BuyPosition);
  showPrice(hLbl[24],GTradingInfo.SellPosition);

	//if(gEnableFlag)
	//{
	//	pTraderUserSpi->ReqQryInvestorPosition(pTrade->InstrumentID);
	//	stringstream ss;
	//	ss << "[][OnRtnTrade]:报单编号：" << trade->OrderSysID
	//		<< "；Direction：" << trade->Direction 
	//		<< "；OffsetFlag：" << trade->OffsetFlag 
	//		<< "；Price：" << trade->Price
	//		<< "；Volume：" << pTrade->Volume
	//		<< "；B：" << gBuyPosition
	//		<< "；S：" << gSellPosition;
	//	WriteLog(ss.str());
	//}

  SetEvent(g_hEvent);
}

void CtpTraderSpi::OnFrontDisconnected(int nReason)
{
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " 响应 | 连接中断..." 
	 // << " reason=" << nReason << endl;
}
		
void CtpTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " 响应 | 心跳超时警告..." 
	 // << " TimerLapse = " << nTimeLapse << endl;
}

void CtpTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}



void CtpTraderSpi::PrintOrders(){
  CThostFtdcOrderField* pOrder; 
  for(unsigned int i=0; i<orderList.size(); i++){
    pOrder = orderList[i];
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " 报单 | 合约:"<<pOrder->InstrumentID
  //    <<" 方向:"<<MapDirection(pOrder->Direction,false)
  //    <<" 开平:"<<MapOffset(pOrder->CombOffsetFlag[0],false)
  //    <<" 价格:"<<pOrder->LimitPrice
  //    <<" 数量:"<<pOrder->VolumeTotalOriginal
  //    <<" 序号:"<<pOrder->BrokerOrderSeq 
  //    <<" 报单编号:"<<pOrder->OrderSysID
  //    <<" 状态:"<<pOrder->StatusMsg<<endl;
  }
  SetEvent(g_hEvent);
}
void CtpTraderSpi::PrintTrades(){
  CThostFtdcTradeField* pTrade;
  for(unsigned int i=0; i<tradeList.size(); i++){
    pTrade = tradeList[i];
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " 成交 | 合约:"<< pTrade->InstrumentID 
  //    <<" 方向:"<<MapDirection(pTrade->Direction,false)
  //    <<" 开平:"<<MapOffset(pTrade->OffsetFlag,false) 
  //    <<" 价格:"<<pTrade->Price
  //    <<" 数量:"<<pTrade->Volume
  //    <<" 报单编号:"<<pTrade->OrderSysID
  //    <<" 成交编号:"<<pTrade->TradeID<<endl;
  }
  SetEvent(g_hEvent);
}

void CtpTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder,  CThostFtdcRspInfoField *pRspInfo,  int nRequestID,  bool bIsLast)
{
	stringstream ss;
	ss << pOrder->InstrumentID << "," <<  pOrder ->OrderSysID << "," << 
		pOrder->Direction << "," <<  pOrder->CombOffsetFlag << "," << 
		pOrder->LimitPrice << "," <<  pOrder->VolumeTotalOriginal << "," <<  
		pOrder->VolumeTraded << "," <<  pOrder->VolumeTotal << "," <<  
		pOrder->OrderStatus << "," ;
	WriteLog(ss.str());
	if(bIsLast)
		MessageBox(hWnd,L"OnRspQryOrder",L"OnRspQryOrder",NULL);

}