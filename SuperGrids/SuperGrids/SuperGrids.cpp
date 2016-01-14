// SuperGrids.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "SuperGrids.h"

#define MAX_LOADSTRING 100
#define IDD_TXT0       9000  
#define IDD_TXT1       9001 

#define IDD_BTN0       9100  //��½
#define IDD_BTN1       9101  //����
#define IDD_BTN2       9102  //��ͣ
#define IDD_BTN3       9103  //ȫ��


#define IDD_LSV1       9200  
#define IDD_LSV2       9201

#define IDD_LV0        9300


CThostFtdcMdApi* pMdUserApi=CThostFtdcMdApi::CreateFtdcMdApi();
CtpMdSpi* pMdUserSpi=new CtpMdSpi(pMdUserApi); //�����ص����������MdSpi
CThostFtdcTraderApi* pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
CtpTraderSpi* pTraderUserSpi = new CtpTraderSpi(pTraderUserApi);

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void RunMD()
{
	pMdUserApi->RegisterSpi(pMdUserSpi);
	if(g_md_front!=NULL)
	{
		pMdUserApi->RegisterFront(g_md_front);		     // ע������ǰ�õ�ַ
		pMdUserApi->Init();      //�ӿ��߳�����, ��ʼ����
		//ShowMdCommand(pMdUserSpi,true); 
		pMdUserApi->Join();      //�ȴ��ӿ��߳��˳�
		//pUserApi->Release(); //�ӿڶ����ͷ�
	}
	else
	{
		WriteLog("[Error][RunMD]");
	}
}

void RunTrader(void)
{
	pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);			// ע���¼���
	pTraderUserApi->SubscribePublicTopic(THOST_TERT_RESTART);					// ע�ṫ����
	pTraderUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);			  // ע��˽����
	if(g_td_front!=NULL)
	{
		pTraderUserApi->RegisterFront(g_td_front);							// ע�ύ��ǰ�õ�ַ
		pTraderUserApi->Init();		//����CTP������
		//ShowTraderCommand(pTraderUserApi,true); 
		pTraderUserApi->Join();  
		//pTraderUserApi->Release();
	}
	else
	{
		WriteLog("[Error][RunTrader]");
	}
}


DWORD WINAPI MdProc(LPVOID lpParameter)
{
	RunMD();	
	return 0;
}

DWORD WINAPI TraderProc(LPVOID lpParameter)
{
	//WaitForSingleObject(g_hEvent,INFINITE);
	RunTrader();
	//SetEvent(g_hEvent);	
	return 0;
}

void GetConfig()
{
	string ss=GetConfigSetting("td_ip");
	strcpy_s(g_td_front,strlen(ss.c_str())+1,ss.c_str());
	ss=GetConfigSetting("md_ip");
	strcpy_s(g_md_front,strlen(ss.c_str())+1,ss.c_str());
	ss=GetConfigSetting("broker_id");
	strcpy_s(GUserInfo.BrokerID,strlen(ss.c_str())+1,ss.c_str());
	ss=GetConfigSetting("user_id");
	strcpy_s(GUserInfo.UserID,strlen(ss.c_str())+1,ss.c_str());
	ss=GetConfigSetting("password");
	strcpy_s(GUserInfo.Password,strlen(ss.c_str())+1,ss.c_str());
	ss=GetConfigSetting("instrument_id_list");
	strcpy_s(g_inst_id_list,strlen(ss.c_str())+1,ss.c_str());

	char* c_temp = new char[200];
	char* c_buff = new char[200];
	char* c_token = new char[200];
	strcpy_s(c_temp,strlen(ss.c_str())+1,ss.c_str());
	c_token = strtok_s(c_temp, ",",&c_buff);
	while( c_token != NULL )
	{
		vcInstIDlist.push_back(c_token); 
		c_token = strtok_s(NULL, ",",&c_buff);
	}
	unsigned int len = vcInstIDlist.size();
	char** pInstId = new char* [len];  
	for(unsigned int i=0; i<len;i++)  pInstId[i]=vcInstIDlist[i];

	WriteLog(g_md_front);
	WriteLog(g_td_front);
	WriteLog(g_inst_id_list);
	WriteLog(GUserInfo.BrokerID);
	WriteLog(GUserInfo.UserID);

	UpdateWindow(hWnd);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SUPERGRIDS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SUPERGRIDS));

	//OpenDb();
	GetConfig();


	g_hEvent=CreateEvent(NULL, true, false, NULL); 
	HANDLE hThread1;
	HANDLE hThread2;
	hThread1=CreateThread(NULL,0,MdProc,NULL,0,NULL);
	hThread2=CreateThread(NULL,0,TraderProc,NULL,0,NULL);

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SUPERGRIDS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SUPERGRIDS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 600, 500, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDD_BTN0:
			{
				//pTraderUserSpi->ReqQryInvestorPositionDetail(vcInstIDlist[0]);
				GTradingInfo.EnableFlag = !GTradingInfo.EnableFlag;
				if (GTradingInfo.EnableFlag)
					SendMessage((HWND)lParam, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"�ѵ�¼");
				else
					SendMessage((HWND)lParam, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"δ��¼");

				if(GTradingInfo.LastPrice != 0)
				{
					int ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'S', "O", GTradingInfo.LastPrice + GTradingInfo.GridSize,GTradingInfo.TrxVolumn);
					ret = pTraderUserSpi->ReqOrderInsert(vcInstIDlist[0], 'B', "O", GTradingInfo.LastPrice - GTradingInfo.GridSize,GTradingInfo.TrxVolumn);
				}
				else
					MessageBox(hWnd,L"gLastPrice is zero !",L"Warning",NULL);
			}
			break;
		case IDD_BTN1:
			{
				//MessageBox(hWnd, L"������˵�һ����ť��",L"��ʾ",MB_OK | MB_ICONINFORMATION);
				//gEnableFlag = !gEnableFlag;
				//if (gEnableFlag)
				//	SendMessage((HWND)lParam, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"������");
				//else
				//	SendMessage((HWND)lParam, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"δ����");
				//pTraderUserSpi->ReqQryInvestorPosition("rb1601");
				//gBuyPosition = 0;
				//gSellPosition = 0;
				//pTraderUserSpi->ReqQryInvestorPositionDetail(vcInstIDlist[0]);
				//showText(hLbl[21],vcInstIDlist[0]);
				//showPrice(hLbl[22],gLastPrice);
				//showPrice(hLbl[23],gBuyPosition);
				//showPrice(hLbl[24],gSellPosition);

				//CThostFtdcQryOrderField* pQryOrder = new CThostFtdcQryOrderField();
				//strcpy(pQryOrder->g_broker_id,g_broker_id);
				//strcpy(pQryOrder->InstrumentID,vcInstIDlist[0]);
				//strcpy(pQryOrder->InvestorID,g_user_id);

				//pTraderUserApi->ReqQryOrder(pQryOrder, g_request_id); 
				//ExecuteSelect();
			
				MessageBox(hWnd,L"gLastPrice is zero !",L"Warning",NULL);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...
		EndPaint(hWnd, &ps);
		break;
	case WM_CREATE:
		{
			int topX = 20,topY = 20;
			const int HEIGHT = 24, WIDTH = 16;

			hLbl[0] = CreateWindow(TEXT("STATIC"), TEXT("�˺�"), WS_CHILD|WS_VISIBLE,                  topX+WIDTH*0,topY+HEIGHT*0, WIDTH*4, HEIGHT, hWnd, NULL, NULL, NULL);
			hTxt[0] = CreateWindow(TEXT("EDIT"),   TEXT(""),     WS_CHILD|WS_VISIBLE|WS_BORDER|BS_TEXT,topX+WIDTH*4,topY+HEIGHT*0, WIDTH*6, HEIGHT, hWnd, (HMENU)IDD_TXT0, hInst, NULL);
			hLbl[1] = CreateWindow(TEXT("STATIC"), TEXT("����"), WS_CHILD|WS_VISIBLE,                  topX+WIDTH*10,topY+HEIGHT*0, WIDTH*4, HEIGHT, hWnd, NULL, NULL, NULL);
			hTxt[1] = CreateWindow(TEXT("EDIT"),   TEXT(""),     WS_CHILD|WS_VISIBLE|WS_BORDER|BS_TEXT,topX+WIDTH*14,topY+HEIGHT*0, WIDTH*6, HEIGHT, hWnd, (HMENU)IDD_TXT1, hInst, NULL);

			hBtn[0] = CreateWindow(TEXT("BUTTON"), TEXT("��¼"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,topX+WIDTH*0,topY+HEIGHT*2,WIDTH*5,HEIGHT,hWnd,(HMENU)IDD_BTN0, hInst, NULL);
			hBtn[1] = CreateWindow(TEXT("BUTTON"), TEXT("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,topX+WIDTH*5,topY+HEIGHT*2,WIDTH*5,HEIGHT,hWnd,(HMENU)IDD_BTN1, hInst, NULL);
			hBtn[2] = CreateWindow(TEXT("BUTTON"), TEXT("��ͣ"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,topX+WIDTH*10,topY+HEIGHT*2,WIDTH*5,HEIGHT,hWnd,(HMENU)IDD_BTN2, hInst, NULL);
			hBtn[3] = CreateWindow(TEXT("BUTTON"), TEXT("ȫ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,topX+WIDTH*15,topY+HEIGHT*2,WIDTH*5,HEIGHT,hWnd,(HMENU)IDD_BTN3, hInst, NULL);

			hLv = CreateWindow(WC_LISTVIEW, TEXT("WC_LISTVIEW"), WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL | WS_HSCROLL,topX,topY+HEIGHT*4,WIDTH*25,HEIGHT*5, hWnd, (HMENU)IDD_LV0, hInst, NULL);
			//initLvRunLog(hLvRunLog);
			//hWndLBTd = CreateWindow(WC_LISTVIEW, TEXT("WC_LISTVIEW"), WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL | WS_HSCROLL, topX,topY+HEIGHT*15,WIDTH*25,HEIGHT*5, hWnd, (HMENU)IDL_LISTV, hInst, NULL);
			//initListView(hWndLBTd);
			UpdateWindow(hWnd);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
