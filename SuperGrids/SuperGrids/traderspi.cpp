#include "traderspi.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "ThostFtdcTraderApi.h"
#include "windows.h"
#include "common.h"

#pragma warning(disable :4996)

using namespace std;
// �Ự����
int	 frontId;	//ǰ�ñ��
int	 sessionId;	//�Ự���
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

extern HINSTANCE hInst;	// ��ǰʵ��
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
		 // ����Ự����	
		frontId = pRspUserLogin->FrontID;
		sessionId = pRspUserLogin->SessionID;
		int nextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		sprintf(orderRef, "%d", ++nextOrderRef);
		WriteLog("[Hint][OnRspUserLogin]Successed!");
		SendMessage(hTxt[2],WM_SETTEXT,0,(LPARAM)L"�����ѵ�¼");
		//ȷ�Ͻ���
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
	strcpy(req.InstrumentID, instId);//Ϊ�ձ�ʾ��ѯ���к�Լ
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
	//cout << currentDateTime << " ��Ӧ | ��Լ:"<<pInstrument->InstrumentID
 //     <<" ������:"<<pInstrument->DeliveryMonth
 //     <<" ��ͷ��֤����:"<<pInstrument->LongMarginRatio
 //     <<" ��ͷ��֤����:"<<pInstrument->ShortMarginRatio<<endl;    
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
		ss << "[Hint][OnRspQryTradingAccount]<��̬Ȩ�棺" << std::fixed << pTradingAccount->Balance
			<< "; �����ʽ� " << std::fixed << pTradingAccount->Available
			<<"; ռ�ñ�֤�� "<<pTradingAccount->CurrMargin
			<<"; �ֲ�ӯ���� "<<pTradingAccount->PositionProfit 
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
		 // << "; ���� = " << MapDirection(pInvestorPosition->PosiDirection - 2, false)
		 // << "; �ֲܳ� = " << pInvestorPosition->Position
		 // << "; ��� = " << pInvestorPosition->YdPosition
		 // << "; ��� = " << pInvestorPosition->TodayPosition
		 // << "; �ֲ�ӯ�� = " << pInvestorPosition->PositionProfit
		 // << "; ��֤�� = " << pInvestorPosition->UseMargin;
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


///��ѯ�ֲ���ϸ
void CtpTraderSpi::OnRspQryInvestorPositionDetail(
    CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
  if( !IsErrorRspInfo(pRspInfo) &&  pInvestorPositionDetail )
  {
	//stringstream ss;
	//ss<<"OnRspQryInvestorPositionDetail:��Լ:"<<pInvestorPositionDetail->InstrumentID
	//<<" ����:"<<MapDirection(pInvestorPositionDetail->Direction,false)
	//<<" �ֲ�:"<<pInvestorPositionDetail->Volume
	//<<" ��������:"<<pInvestorPositionDetail->OpenDate 
	//<<" ���ּ۸�:"<<pInvestorPositionDetail->OpenPrice
	//<<" ��������:"<<pInvestorPositionDetail->TradingDay
	//<<" �ɽ����:"<<pInvestorPositionDetail->TradeID <<endl;
	//string sTemp;
	//sTemp = ss.str();
	//WriteLog(sTemp);
	//���ֲ���ϸ����vector
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
	  ss << "[Hint][OnRspQryInvestorPositionDetail]��ֲ֣�" << GTradingInfo.BuyPosition << "�����ֲ֣�" << GTradingInfo.SellPosition << endl;
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
	strcpy(req.BrokerID, GUserInfo.BrokerID);  //Ӧ�õ�Ԫ����	
	strcpy(req.InvestorID, GUserInfo.UserID); //Ͷ���ߴ���	
	strcpy(req.InstrumentID, instId); //��Լ����	
	strcpy(req.OrderRef, orderRef);  //��������
	
  int nextOrderRef = atoi(orderRef);
  sprintf(orderRef, "%d", ++nextOrderRef);
  
  req.LimitPrice = price;	//�۸�
  if(0==req.LimitPrice)
  {
	  req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;//�۸�����=�м�
	  req.TimeCondition = THOST_FTDC_TC_IOC;//��Ч������:������ɣ�������
  }
  else
  {
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;//�۸�����=�޼�	
    req.TimeCondition = THOST_FTDC_TC_GFD;  //��Ч������:������Ч
  }

	if('b'==dir||'B'==dir) 
		req.Direction = THOST_FTDC_D_Buy;  //��������
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

	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	  //���Ͷ���ױ���־	
	req.VolumeTotalOriginal = vol;	///����		
	req.VolumeCondition = THOST_FTDC_VC_AV; //�ɽ�������:�κ�����
	req.MinVolume = 1;	//��С�ɽ���:1	
	req.ContingentCondition = THOST_FTDC_CC_Immediately;  //��������:����
	
  //TThostFtdcPriceType	StopPrice;  //ֹ���
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;	//ǿƽԭ��:��ǿƽ	
	req.IsAutoSuspend = 0;  //�Զ������־:��	
	req.UserForceClose = 0;   //�û�ǿ����־:��

	int ret = pTraderUserApi->ReqOrderInsert(&req, ++g_request_id);
	//cerr<<" ���� | ���ͱ���..."<<((ret == 0)?"�ɹ�":"ʧ��")<< endl;
	//string sTemp;
	//if (ret == 0)
	//	sTemp = "ReqOrderInsert...�ɹ�";
	//else
	//	sTemp = "ReqOrderInsert...ʧ��";
	//WriteLog(sTemp);
	return ret;
}

//������ReqOrderInsert�µ��ɹ���ִ��OnRspOrderInsert�ص�����
void CtpTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
          CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInputOrder )
	{
		//stringstream ss;
		//ss << "OnRspOrderInsert...�ɹ�.��������:" << pInputOrder->OrderRef ;
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
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
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
		//cout << currentDateTime << " ���� | ����������."<<endl; return;
	} 

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, GUserInfo.BrokerID);   //���͹�˾����	
	strcpy(req.InvestorID, GUserInfo.UserID); //Ͷ���ߴ���
	//strcpy(req.OrderRef, pOrderRef); //��������	
	//req.FrontID = frontId;           //ǰ�ñ��	
	//req.SessionID = sessionId;       //�Ự���
  strcpy(req.ExchangeID, orderList[i]->ExchangeID);
  strcpy(req.OrderSysID, orderList[i]->OrderSysID);
	req.ActionFlag = THOST_FTDC_AF_Delete;  //������־ 

	int ret = pTraderUserApi->ReqOrderAction(&req, ++g_request_id);
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " ���� | ���ͳ���..." <<((ret == 0)?"�ɹ�":"ʧ��") << endl;
}

void CtpTraderSpi::OnRspOrderAction(
      CThostFtdcInputOrderActionField *pInputOrderAction, 
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
  if (!IsErrorRspInfo(pRspInfo) && pInputOrderAction){
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " ��Ӧ | �����ɹ�..."
  //    << "������:"<<pInputOrderAction->ExchangeID
  //    <<" �������:"<<pInputOrderAction->OrderSysID<<endl;
  }
  if(bIsLast) SetEvent(g_hEvent);	
}

///�����ر�
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
		//cout << currentDateTime << " �ر� | �������ύ...���:"<<order->BrokerOrderSeq << "��StatusMsg��" << order->StatusMsg<<endl;
  SetEvent(g_hEvent);	
}

///�ɽ�֪ͨ
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
	//	ss << "[][OnRtnTrade]:������ţ�" << trade->OrderSysID
	//		<< "��Direction��" << trade->Direction 
	//		<< "��OffsetFlag��" << trade->OffsetFlag 
	//		<< "��Price��" << trade->Price
	//		<< "��Volume��" << pTrade->Volume
	//		<< "��B��" << gBuyPosition
	//		<< "��S��" << gSellPosition;
	//	WriteLog(ss.str());
	//}

  SetEvent(g_hEvent);
}

void CtpTraderSpi::OnFrontDisconnected(int nReason)
{
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " ��Ӧ | �����ж�..." 
	 // << " reason=" << nReason << endl;
}
		
void CtpTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " ��Ӧ | ������ʱ����..." 
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
		//cout << currentDateTime << " ���� | ��Լ:"<<pOrder->InstrumentID
  //    <<" ����:"<<MapDirection(pOrder->Direction,false)
  //    <<" ��ƽ:"<<MapOffset(pOrder->CombOffsetFlag[0],false)
  //    <<" �۸�:"<<pOrder->LimitPrice
  //    <<" ����:"<<pOrder->VolumeTotalOriginal
  //    <<" ���:"<<pOrder->BrokerOrderSeq 
  //    <<" �������:"<<pOrder->OrderSysID
  //    <<" ״̬:"<<pOrder->StatusMsg<<endl;
  }
  SetEvent(g_hEvent);
}
void CtpTraderSpi::PrintTrades(){
  CThostFtdcTradeField* pTrade;
  for(unsigned int i=0; i<tradeList.size(); i++){
    pTrade = tradeList[i];
		//t = time(0);
		//strftime( currentDateTime, sizeof(currentDateTime), datetimeFormat,localtime(&t) ); 
		//cout << currentDateTime << " �ɽ� | ��Լ:"<< pTrade->InstrumentID 
  //    <<" ����:"<<MapDirection(pTrade->Direction,false)
  //    <<" ��ƽ:"<<MapOffset(pTrade->OffsetFlag,false) 
  //    <<" �۸�:"<<pTrade->Price
  //    <<" ����:"<<pTrade->Volume
  //    <<" �������:"<<pTrade->OrderSysID
  //    <<" �ɽ����:"<<pTrade->TradeID<<endl;
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