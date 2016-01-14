// SuperGrids.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SuperGrids.h"

#define MAX_LOADSTRING 100
#define IDD_TXT0       9000  
#define IDD_TXT1       9001 

#define IDD_BTN0       9100  //登陆
#define IDD_BTN1       9101  //启动
#define IDD_BTN2       9102  //暂停
#define IDD_BTN3       9103  //全清


#define IDD_LSV1       9200  
#define IDD_LSV2       9201

#define IDD_LV0        9300


CThostFtdcMdApi* pMdUserApi=CThostFtdcMdApi::CreateFtdcMdApi();
CtpMdSpi* pMdUserSpi=new CtpMdSpi(pMdUserApi); //创建回调处理类对象MdSpi
CThostFtdcTraderApi* pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
CtpTraderSpi* pTraderUserSpi = new CtpTraderSpi(pTraderUserApi);

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void RunMD()
{
	pMdUserApi->RegisterSpi(pMdUserSpi);
	if(g_md_front!=NULL)
	{
		pMdUserApi->RegisterFront(g_md_front);		     // 注册行情前置地址
		pMdUserApi->Init();      //接口线程启动, 开始工作
		//ShowMdCommand(pMdUserSpi,true); 
		pMdUserApi->Join();      //等待接口线程退出
		//pUserApi->Release(); //接口对象释放
	}
	else
	{
		WriteLog("[Error][RunMD]");
	}
}

void RunTrader(void)
{
	pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);			// 注册事件类
	pTraderUserApi->SubscribePublicTopic(THOST_TERT_RESTART);					// 注册公有流
	pTraderUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);			  // 注册私有流
	if(g_td_front!=NULL)
	{
		pTraderUserApi->RegisterFront(g_td_front);							// 注册交易前置地址
		pTraderUserApi->Init();		//连接CTP服务器
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

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SUPERGRIDS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
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

	// 主消息循环:
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
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
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
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

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
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
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
		// 分析菜单选择:
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
					SendMessage((HWND)lParam, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"已登录");
				else
					SendMessage((HWND)lParam, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"未登录");

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
				//MessageBox(hWnd, L"您点击了第一个按钮。",L"提示",MB_OK | MB_ICONINFORMATION);
				//gEnableFlag = !gEnableFlag;
				//if (gEnableFlag)
				//	SendMessage((HWND)lParam, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"已启动");
				//else
				//	SendMessage((HWND)lParam, WM_SETTEXT, (WPARAM)NULL, (LPARAM)L"未启动");
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
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_CREATE:
		{
			int topX = 20,topY = 20;
			const int HEIGHT = 24, WIDTH = 16;

			hLbl[0] = CreateWindow(TEXT("STATIC"), TEXT("账号"), WS_CHILD|WS_VISIBLE,                  topX+WIDTH*0,topY+HEIGHT*0, WIDTH*4, HEIGHT, hWnd, NULL, NULL, NULL);
			hTxt[0] = CreateWindow(TEXT("EDIT"),   TEXT(""),     WS_CHILD|WS_VISIBLE|WS_BORDER|BS_TEXT,topX+WIDTH*4,topY+HEIGHT*0, WIDTH*6, HEIGHT, hWnd, (HMENU)IDD_TXT0, hInst, NULL);
			hLbl[1] = CreateWindow(TEXT("STATIC"), TEXT("密码"), WS_CHILD|WS_VISIBLE,                  topX+WIDTH*10,topY+HEIGHT*0, WIDTH*4, HEIGHT, hWnd, NULL, NULL, NULL);
			hTxt[1] = CreateWindow(TEXT("EDIT"),   TEXT(""),     WS_CHILD|WS_VISIBLE|WS_BORDER|BS_TEXT,topX+WIDTH*14,topY+HEIGHT*0, WIDTH*6, HEIGHT, hWnd, (HMENU)IDD_TXT1, hInst, NULL);

			hBtn[0] = CreateWindow(TEXT("BUTTON"), TEXT("登录"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,topX+WIDTH*0,topY+HEIGHT*2,WIDTH*5,HEIGHT,hWnd,(HMENU)IDD_BTN0, hInst, NULL);
			hBtn[1] = CreateWindow(TEXT("BUTTON"), TEXT("启动"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,topX+WIDTH*5,topY+HEIGHT*2,WIDTH*5,HEIGHT,hWnd,(HMENU)IDD_BTN1, hInst, NULL);
			hBtn[2] = CreateWindow(TEXT("BUTTON"), TEXT("暂停"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,topX+WIDTH*10,topY+HEIGHT*2,WIDTH*5,HEIGHT,hWnd,(HMENU)IDD_BTN2, hInst, NULL);
			hBtn[3] = CreateWindow(TEXT("BUTTON"), TEXT("全清"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,topX+WIDTH*15,topY+HEIGHT*2,WIDTH*5,HEIGHT,hWnd,(HMENU)IDD_BTN3, hInst, NULL);

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

// “关于”框的消息处理程序。
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
