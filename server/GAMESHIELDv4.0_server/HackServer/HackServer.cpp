#include "stdafx.h"
#include "resource.h"
#include "HackServer.h"
#include "MiniDump.h"
#include "ProcessManager.h"
#include "ServerDisplayer.h"
#include "ServerInfo.h"
#include "SocketManager.h"
#include "ReadFiles.h"
#include "BlackList.h"
#include "Authenticate.h"
#include "HidManager.h"
#include "Util.h"
#include "ScreenShow.h"
#include "ScreenShot.h"
#include "ClientManager.h"
#include "IpManager.h"
#include "ProcessListInfo.h"
#include <sstream>
#include <curl/curl.h>

HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
TCHAR szChildTitle[MAX_LOADSTRING];
TCHAR szChildClass[MAX_LOADSTRING];
HWND hWnd;
HWND hChildWnd;
HWND ScreenWnd;

HWND ProcessListWnd;
void UrlChecker();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) // OK
{
	//CMiniDump::Start();
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	UrlChecker();
	

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_HACKSERVER, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	if (InitInstance(hInstance, nCmdShow) == 0)
	{
		return 0;
	}

	gServerInfo.ReadStartupInfo("GameShield", ".\\Configs.ini");

#if(PROTECT_STATE == 1)

	gAuthenticate.Check();

#endif

	char buff[256];

	wsprintf(buff, "[%s] GameShield AntiHack + Desktop Capture (UserCount : %d)", HACKSERVER_VERSION_DUMMY, 0);

	SetWindowText(hWnd, buff);

	gServerDisplayer.Init(hWnd);

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) == 0)
	{
		if (gSocketManager.Start(gServerInfo.m_HackServerPort) != 0)
		{
			gServerInfo.ReadInit();

			SetTimer(hWnd, TIMER_1000, 1000, 0);

			SetTimer(hWnd, TIMER_5000, 5000, 0);

			SetTimer(ScreenWnd, TIMER_5001, 1, NULL);
		}
	}
	else
	{
		LogAdd(LOG_RED, "WSAStartup() failed with error: %d", WSAGetLastError());
	}

	gServerDisplayer.PaintAllInfo();

	SetTimer(hWnd, TIMER_2000, 2000, 0);

	HACCEL hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_HACKSERVER);

	MSG msg;

	while (GetMessage(&msg, 0, 0, 0) != 0)
	{
		if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg) == 0)
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}

	
	GdiplusShutdown(gdiplusToken);

	//CMiniDump::Clean();
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) // OK
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCSTR)IDC_HACKSERVER;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) // OK
{
	hInst = hInstance;

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_THICKFRAME, CW_USEDEFAULT, 0, ANCHO_VENTANA, ALTO_VENTANA, 0, 0, hInstance, 0);

	if (hWnd == 0)
	{
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return 1;
}

HWND scrr1;
HWND scrr2;
HWND scrr3;
HWND ed;

static int s1Pos, s1Min, s1Max, s2Pos, s2Min, s2Max;
static HWND s1Scroll, s2Scroll;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) // OK
{
	switch (message)
	{
	case WM_CREATE:
	{
		ScreenWnd = CreateDialogParamA(hInst, LPCTSTR(IDM_SCREEN), hWnd, (DLGPROC)Screen, 0);
		gScreenShow.ScreenWnd = ScreenWnd;
		gScreenShow.hInst = hInst;

		ProcessListWnd = CreateDialogParamA(hInst, LPCTSTR(IDM_PROCESS_LIST), hWnd, (DLGPROC)ProcessList, 0);
		gProcessListInfo.ScreenWnd = ProcessListWnd;
		gProcessListInfo.hInst = hInst;
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case ID_OTHER_BANNED:
			DialogBox(hInst, (LPCTSTR)IDM_BANNED, hWnd, (DLGPROC)Banned);
			break;
		case ID_OTHER_SCREEN:
			DialogBox(hInst, (LPCTSTR)IDD_INFO_DIALOG, hWnd, (DLGPROC)PlayersList);
			break;
		case ID_OTHER_DELETETEMPBANS:
			gHidManager.DeleteAllHardwareId();
			break;
		case IDM_EXIT:
			if (MessageBox(0, "Are you sure to terminate ?", "Ask terminate server", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				DestroyWindow(hWnd);
			}
			break;
		case IDM_RELOAD_CONFIG:
			gServerInfo.ReadConfig();
			break;
		case IDM_RELOAD_DUMP:
			gServerInfo.ReadDumpList();
			break;
		case IDM_RELOAD_CHECKSUM:
			gServerInfo.ReadChecksumList();
			break;
		case IDM_RELOAD_WINDOW:
			gServerInfo.ReadWindowList();
			break;
		case IDM_RELOAD_BLACKLIST:
			gServerInfo.ReadBlackList();
			break;
		case IDM_UPDATE:
			gReadFiles.UpdateInternalList();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case TIMER_1000:
			gProcessManager.CheckProcess();
			break;
		case TIMER_2000:
			gServerDisplayer.Run();
			break;
		case TIMER_5000:
			TimeoutProc();
			gProcessManager.ClearProcessCache();
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) // OK
{
	switch (message)
	{
	case WM_INITDIALOG:
		return 1;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return 1;
		}
		break;
	}

	return 0;
}

bool CheckListBox(HWND hDlg, int Item, char* text)
{
	HWND hlst;
	SetClassLong(hDlg, GCL_HICON, (long)LoadIcon(0, IDI_APPLICATION));
	hlst = GetDlgItem(hDlg, Item);

	int count = SendMessage(hlst, LB_GETCOUNT, 0, 0);

	char buff[100];

	for (int i = 0;i < count;i++)
	{
		SendMessage(hlst, LB_GETTEXT, (WPARAM)i, (LPARAM)buff);

		const char* SEPARATORS = " ";
		char* ptr = 0;       //Указатель
		ptr = strtok(buff, SEPARATORS); //Выдираем первое слово из строки
		ptr = strtok(0, SEPARATORS);   //Подбираем слово

		if (!strcmp(ptr, text))
		{
			return false;
		}
		ptr = 0;
	}

	return true;
}

LRESULT CALLBACK PlayersList(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) // OK
{
	char Filterip[16]={0};

	switch (message)
	{
	case WM_INITDIALOG:
	{
		SetClassLong(hDlg, GCL_HICON, (long)LoadIcon(0, IDI_APPLICATION));
		char WindowName[100];
		sprintf(WindowName, "Connected %d Users", GetUserCount());
		SetWindowText(hDlg, WindowName);
		HWND IpListBox = GetDlgItem(hDlg, IDC_LIST_IP);

		int count = 0;
		char text[64];

		for (int n = 0;n < MAX_CLIENT;n++)
		{
			if (gClientManager[n].CheckState() != 0)
			{
				if (CheckListBox(hDlg, IDC_LIST_IP, gClientManager[n].m_IpAddr) == 0)
				{
					continue;
				}
				count++;
				sprintf(text, "%d). %s", count, gClientManager[n].m_IpAddr);
				SendMessage(IpListBox, LB_ADDSTRING, NULL, LPARAM(text));
			}
		}
			SetDlgItemText(hDlg,ID_IPSEARCHLIST,Filterip);
	}
	break;

	case WM_COMMAND:
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case IDC_LIST_IP:
		{

			if (HIWORD(wParam) == LBN_SELCHANGE)
			{
				HWND IpListBox = GetDlgItem(hDlg, IDC_LIST_IP);
				HWND HwidListBox = GetDlgItem(hDlg, IDC_LIST_HWID);

				SendMessage(HwidListBox, LB_RESETCONTENT, 0, NULL);

				char Buf[100];
				int ListItem1 = (int)SendMessage(IpListBox, LB_GETCURSEL, 0, 0);
				if (ListItem1 != LB_ERR)
				{
					SendMessage(IpListBox, LB_GETTEXT, ListItem1, (LPARAM)Buf);
					const char* SEPARATORS = " ";
					char* ptr = 0;       //Указатель
					ptr = strtok(Buf, SEPARATORS); //Выдираем первое слово из строки
					ptr = strtok(0, SEPARATORS);   //Подбираем слово
					//MessageBox(0,ptr,"ListBox1",MB_OK);

					int count = 0;
					char text[64];

					for (int n = 0;n < MAX_CLIENT;n++)
					{
						if (gClientManager[n].CheckState() != 0)
						{
							if (!strcmp(ptr, gClientManager[n].m_IpAddr))
							{
								count++;
								sprintf(text, "%d). %s", count, gClientManager[n].m_HardwareId);
								SendMessage(HwidListBox, LB_ADDSTRING, NULL, LPARAM(text));
							}
						}
					}
				}
				return 0;
			}

		}
		break;
		case ID_RELOAD:
		{

			SetClassLong(hDlg, GCL_HICON, (long)LoadIcon(0, IDI_APPLICATION));
			HWND hlst = GetDlgItem(hDlg, IDC_LIST_IP);
			HWND HwidListBox = GetDlgItem(hDlg, IDC_LIST_HWID);
			SendMessage(hlst, LB_RESETCONTENT, 0, NULL);
			SendMessage(HwidListBox, LB_RESETCONTENT, 0, NULL);

			int count = 0;
			char text[64];

			for (int n = 0;n < MAX_CLIENT;n++)
			{
				if (gClientManager[n].CheckState() != 0)
				{
					if (CheckListBox(hDlg, IDC_LIST_IP, gClientManager[n].m_IpAddr) == 0)
					{
						continue;
					}
					count++;
					sprintf(text, "%d). %s", count, gClientManager[n].m_IpAddr);
					SendMessage(hlst, LB_ADDSTRING, NULL, LPARAM(text));
				}
			}
		}
		break;

		case ID_IPSEARCH:
		{
			GetDlgItemText(hDlg,ID_IPSEARCHLIST, Filterip,16);

			SetClassLong(hDlg, GCL_HICON, (long)LoadIcon(0, IDI_APPLICATION));
			HWND hlst = GetDlgItem(hDlg, IDC_LIST_IP);
			HWND HwidListBox = GetDlgItem(hDlg, IDC_LIST_HWID);
			SendMessage(hlst, LB_RESETCONTENT, 0, NULL);
			SendMessage(HwidListBox, LB_RESETCONTENT, 0, NULL);

			int count = 0;
			char text[64];

			for (int n = 0;n < MAX_CLIENT;n++)
			{
				if (gClientManager[n].CheckState() != 0)
				{
					if(strcmp(gClientManager[n].m_IpAddr, Filterip) != 0)
					{
						continue;
					}
					if (CheckListBox(hDlg, IDC_LIST_IP, Filterip) == 0)
					{
						continue;
					}
					count++;
					sprintf(text, "%d). %s", count, gClientManager[n].m_IpAddr);
					SendMessage(hlst, LB_ADDSTRING, NULL, LPARAM(text));
				}
			}
		}
		break;

		case ID_PROCESSLIST:
		{
			char text[100];
			char text1[100];
			if (GetSelecterListBoxItem(hDlg, IDC_LIST_IP, text) == 0)
			{
				return 0;
			}

			const char* SEPARATORS = " ";
			char* IpValue = 0;       //Указатель
			IpValue = strtok(text, SEPARATORS); //Выдираем первое слово из строки
			IpValue = strtok(0, SEPARATORS);   //Подбираем слово

			if (GetSelecterListBoxItem(hDlg, IDC_LIST_HWID, text1) == 0)
			{
				return 0;
			}

			char* HwidValue = 0;       //Указатель
			HwidValue = strtok(text1, SEPARATORS); //Выдираем первое слово из строки
			HwidValue = strtok(0, SEPARATORS);   //Подбираем слово

			int aIndex = GetIndexByIpAndHwid(IpValue, HwidValue);

			if (aIndex == -1)
			{
				LogAdd(LOG_RED, "[DesktopCapture] Error. Cant get Index!");
				return 0;
			}

			//ShowWindow(ProcessListWnd, SW_SHOWNORMAL);
			gProcessListInfo.SendProcessRequest(aIndex);
		}
		break;
		case ID_DESKTOP:
		{
			char text[100];
			char text1[100];
			if (GetSelecterListBoxItem(hDlg, IDC_LIST_IP, text) == 0)
			{
				return 0;
			}

			const char* SEPARATORS = " ";
			char* IpValue = 0;
			IpValue = strtok(text, SEPARATORS);
			IpValue = strtok(0, SEPARATORS);

			if (GetSelecterListBoxItem(hDlg, IDC_LIST_HWID, text1) == 0)
			{
				return 0;
			}

			char* HwidValue = 0;
			HwidValue = strtok(text1, SEPARATORS);
			HwidValue = strtok(0, SEPARATORS);

			int aIndex = GetIndexByIpAndHwid(IpValue, HwidValue);

			if (aIndex == -1)
			{
				LogAdd(LOG_RED, "[DesktopCapture] Error. Cant get Index!");
				return 0;
			}

			//LogAdd(LOG_RED,"Index = %d",aIndex);
			gScreenShow.SendScreenState(aIndex, 1);
			//MoveWindow(ScreenWnd,0,0,1920,1080,1);
			//ShowWindow(ScreenWnd, SW_SHOWNORMAL);
		}
		break;
		case ID_CAPTURE:
		{
			LogAdd(LOG_PURPLE, "ScreenShot button pressed");

			char text[100];
			char text1[100];
			if (GetSelecterListBoxItem(hDlg, IDC_LIST_IP, text) == 0)
			{
				return 0;
			}

			const char* SEPARATORS = " ";
			char* IpValue = 0;
			IpValue = strtok(text, SEPARATORS);
			IpValue = strtok(0, SEPARATORS);

			if (GetSelecterListBoxItem(hDlg, IDC_LIST_HWID, text1) == 0)
			{
				return 0;
			}

			char* HwidValue = 0;
			HwidValue = strtok(text1, SEPARATORS);
			HwidValue = strtok(0, SEPARATORS);

			int aIndex = GetIndexByIpAndHwid(IpValue, HwidValue);

			if (aIndex == -1)
			{
				LogAdd(LOG_RED, "[DesktopCapture] Error. Cant get Index!");
				return 0;
			}

			LogAdd(LOG_PURPLE,"Index = %d",aIndex);
			gScreenShot.SendScreenShotRequest(aIndex);
		}
		break;
		}
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			if (gScreenShow.WorkIndex != -1)
			{
				gScreenShow.SendScreenState(gScreenShow.WorkIndex, 0);
			}
			EndDialog(hDlg, LOWORD(wParam));
			return 1;
		}
		break;
	}

	return 0;
}

LRESULT CALLBACK Banned(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) // OK
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		char WindowName[100];
		int UsersCount = gHidManager.m_HardwareIdInfo.size();
		sprintf(WindowName, "Temporary Banned %d Users", UsersCount);
		SetWindowText(hDlg, WindowName);
		HWND AccsListBox = GetDlgItem(hDlg, IDC_LIST1);

		for (std::vector<HARDWARE_ID_INFO>::iterator it = gHidManager.m_HardwareIdInfo.begin();it != gHidManager.m_HardwareIdInfo.end();it++)
		{
			SendMessage(AccsListBox, LB_ADDSTRING, NULL, LPARAM(it->HardwareId));
		}
	}
	break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return 1;
		}
		break;
	}

	return 0;
}

LRESULT CALLBACK Screen(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) // OK
{
	PAINTSTRUCT  paint;
	HGDIOBJ old, old1;
	RECT rect;
	HDC hdc;
	// для примера
	static RECT   cur, tmp;
	static POINT  pos, pt;
	static int    size = 100;
	static HBRUSH brush = NULL;
	static BOOL   drag = FALSE;


	switch (message)
	{
	case WM_INITDIALOG:
	{
		//char WindowName[100];
		//int UsersCount = gHidManager.m_HardwareIdInfo.size();
		//sprintf(WindowName, "Temporary Banned %d Users", UsersCount);
		SetWindowText(hDlg, "SCREEN");
		//HWND AccsListBox = GetDlgItem(hDlg, IDC_LIST1); 

		/*for(std::vector<HARDWARE_ID_INFO>::iterator it=gHidManager.m_HardwareIdInfo.begin();it != gHidManager.m_HardwareIdInfo.end();it++)
		{
			SendMessage(AccsListBox,LB_ADDSTRING,NULL,LPARAM(it->HardwareId));
		}*/
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		hdc = BeginPaint(ScreenWnd, &ps);
		//OnPaint(hdc, ps.rcPaint);
		gScreenShow.DrawScreen(ScreenWnd, hdc);
		//Graphics g(hdc);
		//Pen pen(Color(255, 0, 255, 128));

		//draw a circle
		//g.DrawEllipse(&pen, 200, 50, 100, 100);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_CLOSE:
	{
		ShowWindow(ScreenWnd, SW_HIDE);
		gScreenShow.WindowOpen = false;
	}
	break;
	case WM_COMMAND:
	case WM_TIMER:
		switch (wParam)
		{
		case TIMER_5001:
		{
			hdc = GetDC(hDlg);
			GetClientRect(hDlg, &rect);
			gScreenShow.DrawScreen(ScreenWnd, hdc);
			//Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
			ReleaseDC(hDlg, hdc);
			//PAINTSTRUCT ps;
			//hdc = BeginPaint(ScreenWnd, &ps);
			//OnPaint(hdc, ps.rcPaint);
			//gScreenShow.DrawScreen(ScreenWnd,hdc);
			//Graphics g(hdc);
			//Pen pen(Color(255, 0, 255, 128));

			//draw a circle
			//g.DrawEllipse(&pen, 200, 50, 100, 100);
			//EndPaint(hWnd, &ps);
			//if (true)
			//{
			//	PAINTSTRUCT ps;
			//	HDC hdc;
			//	//get the hdc from our hwnd
			//	hdc = BeginPaint(ScreenWnd, &ps);

			//	//call our drawing method
			//	//OnPaint(hwnd, hdc);
			//	gScreenShow.DrawScreen(ScreenWnd,hdc);

			//	//end painting
			//	EndPaint(ScreenWnd, &ps);

			//	ReleaseDC(ScreenWnd, hdc);

			//	//RECT rect;
			//	//HDC hdc = GetDC(ScreenWnd);
			//	//GetClientRect(ScreenWnd, &rect);
			//	///*for (int i=0;i<10000;i++)
			//	//{
			//	//	SetPixel(hdc, 5+(i/100), 5+(i/100), RGB(0, 0, 0));
			//	//}*/
			//	////gScreenShow.DrawScreen(hdc);
			//	//
			//	//ReleaseDC(ScreenWnd, hdc);
			//}
		}
		break;
		}

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			gScreenShow.SendScreenState(gScreenShow.WorkIndex, 0);
			EndDialog(hDlg, LOWORD(wParam));
			return 1;
		}
		break;
	}

	return 0;
}


#define IDC_LISTVIEW                      1111



int SetListViewColumns(HWND hWndLV, int colNum, int textMaxLen, char** header, int* sizes)
{
	RECT rcl;
	GetClientRect(hWndLV, &rcl);

	int index = -1;

	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = (rcl.right - rcl.left) / colNum;
	lvc.cchTextMax = textMaxLen;

	for (int i = 0; i < colNum; i++)
	{
		lvc.cx = sizes[i];
		lvc.pszText = header[i];
		index = ListView_InsertColumn(hWndLV, i, &lvc);
		if (index == -1) break;
	}

	return index;
}

HWND CreateListView(HWND hWndParent, UINT uId)
{
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	RECT rcl;
	GetClientRect(hWndParent, &rcl);

	HWND hWndLV = CreateWindow(WC_LISTVIEW, "",
		WS_CHILD | LVS_REPORT,
		0, 0, rcl.right - rcl.left, rcl.bottom - rcl.top - 50,
		hWndParent, (HMENU)uId, GetModuleHandle(NULL), NULL);

	// Чтобы определялись строка (item) и столбец (subitem) обязательно устанавливаем
	// расширенный стиль LVS_EX_FULLROWSELECT.
	ListView_SetExtendedListViewStyleEx(hWndLV, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return (hWndLV);
}

LRESULT CALLBACK ProcessList(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) // OK
{
	PAINTSTRUCT  paint;
	HGDIOBJ old, old1;
	RECT rect;
	HDC hdc;
	// для примера
	static RECT   cur, tmp;
	static POINT  pos, pt;
	static int    size = 100;
	static HBRUSH brush = NULL;
	static BOOL   drag = FALSE;

	static HWND hWndLV = NULL;

	int const colNum = 3;
	char* header[colNum] = { "Name", "PID", "Dump" };
	int sizes[3] = { 200,70,2000 };

	switch (message)
	{
	case WM_INITDIALOG:
	{
		if ((hWndLV = CreateListView(hDlg, IDC_LISTVIEW)) == NULL)
		{
			MessageBox(NULL, "Невозможно создать элемент ListView", "Ошибка", MB_OK);
		}

		gProcessListInfo.hWndLV = hWndLV;
		gProcessListInfo.hDlg = hDlg;

		SetListViewColumns(hWndLV, colNum, 50, header, sizes);


		//RECT rcl;
		//GetClientRect(hWndLV, &rcl);

		////int index = -1;

		//LVCOLUMN lvc;
		//lvc.mask = LVCF_TEXT | LVCF_WIDTH;
		////lvc.cx = (rcl.right - rcl.left) / colNum;
		//lvc.cx = 100;
		//lvc.cchTextMax = 50;

		//lvc.pszText = header[0];
		//ListView_InsertColumn(hWndLV, 0, &lvc);

		//lvc.pszText = header[1];
		//lvc.cx = 50;
		//ListView_InsertColumn(hWndLV, 1, &lvc);

		//lvc.pszText = header[2];
		//lvc.cx = 300;
		//ListView_InsertColumn(hWndLV, 2, &lvc);

		///*for (int i = 0; i < colNum; i++)
		//{
		//	lvc.pszText = header[i];
		//	index = ListView_InsertColumn(hWndLV, i, &lvc);
		//	if (index == -1) break;
		//}*/





		//AddListViewItems(hWndLV, colNum, textMaxLen, item[0]);
		//AddListViewItems(hWndLV, colNum, textMaxLen, item[1]);
		//AddListViewItems(hWndLV, colNum, textMaxLen, item[2]);
		ShowWindow(hWndLV, SW_SHOWDEFAULT);

		//char WindowName[100];
		//int UsersCount = gHidManager.m_HardwareIdInfo.size();
		//sprintf(WindowName, "Temporary Banned %d Users", UsersCount);
		SetWindowText(hDlg, "Process List");
		//HWND AccsListBox = GetDlgItem(hDlg, IDC_LIST1); 

		/*for(std::vector<HARDWARE_ID_INFO>::iterator it=gHidManager.m_HardwareIdInfo.begin();it != gHidManager.m_HardwareIdInfo.end();it++)
		{
			SendMessage(AccsListBox,LB_ADDSTRING,NULL,LPARAM(it->HardwareId));
		}*/
	}
	break;
	//case WM_PAINT:
	//	{
	//	PAINTSTRUCT ps;
//          hdc = BeginPaint(ScreenWnd, &ps);
//          //OnPaint(hdc, ps.rcPaint);
	  //	gScreenShow.DrawScreen(ScreenWnd,hdc);
	  //	//Graphics g(hdc);
	  //	//Pen pen(Color(255, 0, 255, 128));

	  //	//draw a circle
	  //	//g.DrawEllipse(&pen, 200, 50, 100, 100);
//          EndPaint(hWnd, &ps);
	  //	}
//          break;
	case WM_CLOSE:
	{
		ShowWindow(hDlg, SW_HIDE);
		//gScreenShow.WindowOpen = false;
	}
	break;
	case WM_COMMAND:
	case WM_TIMER:
		switch (wParam)
		{
			//case TIMER_5001:
			//	{
			//		hdc = GetDC(hDlg);
			//		GetClientRect(hDlg, &rect);
			//		gScreenShow.DrawScreen(ScreenWnd,hdc);
			//		//Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
			//		ReleaseDC(hDlg, hdc);
			//		//PAINTSTRUCT ps;
			//		//hdc = BeginPaint(ScreenWnd, &ps);
			//		//OnPaint(hdc, ps.rcPaint);
			//		//gScreenShow.DrawScreen(ScreenWnd,hdc);
			//		//Graphics g(hdc);
			//		//Pen pen(Color(255, 0, 255, 128));

			//		//draw a circle
			//		//g.DrawEllipse(&pen, 200, 50, 100, 100);
			//		//EndPaint(hWnd, &ps);
			//		//if (true)
			//		//{
			//		//	PAINTSTRUCT ps;
			//		//	HDC hdc;
			//		//	//get the hdc from our hwnd
			//		//	hdc = BeginPaint(ScreenWnd, &ps);

			//		//	//call our drawing method
			//		//	//OnPaint(hwnd, hdc);
			//		//	gScreenShow.DrawScreen(ScreenWnd,hdc);

			//		//	//end painting
			//		//	EndPaint(ScreenWnd, &ps);

			//		//	ReleaseDC(ScreenWnd, hdc);

			//		//	//RECT rect;
			//		//	//HDC hdc = GetDC(ScreenWnd);
			//		//	//GetClientRect(ScreenWnd, &rect);
			//		//	///*for (int i=0;i<10000;i++)
			//		//	//{
			//		//	//	SetPixel(hdc, 5+(i/100), 5+(i/100), RGB(0, 0, 0));
			//		//	//}*/
			//		//	////gScreenShow.DrawScreen(hdc);
			//		//	//
			//		//	//ReleaseDC(ScreenWnd, hdc);
			//		//}
			//	}
			//	break;
		}

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			/*gScreenShow.SendScreenState(gScreenShow.WorkIndex,0);
			EndDialog(hDlg,LOWORD(wParam));*/
			return 1;
		}
		break;
	}

	return 0;
}


void UrlChecker()
{
	/*
	CURL* curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, URL_LICENCIA); //
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		/* Perform the request, res will get the return code 
		res = curl_easy_perform(curl);

		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		std::string number;
		std::stringstream strstream;
		strstream << http_code;
		strstream >> number;

		//std::string number2;
		//std::stringstream strstream2;
		//strstream2 << http_code;
		//strstream2 >> number2;
		//MessageBox(hWnd, number.c_str(), "Debug info HTTP_CODE", MB_OK);
		//MessageBox(hWnd, number2.c_str(), "Debug info STATUS CURL", MB_OK);
		/* Check for errors 




		// CODIGO PHP PARA que abra la app (IMPORTANTE TAMBIEN EDITAR EN "filedownloadhttp.cpp" en la mitad del archivo
		// ES NECESARIO que en el archivo .php (de la ruta de licencia) tenga las lineas para devolver error 401
		//
		//<?php
		//header('HTTP/1.0 401 Forbidden');
		//?>


		if (http_code != 401)
		{
			curl_easy_cleanup(curl);
			HANDLE process = GetCurrentProcess();
			DWORD exit;
			GetExitCodeProcess(process, &exit);
			TerminateProcess(process, exit);
			ExitProcess(0);
		}

		/* always cleanup 
		curl_easy_cleanup(curl);
	}
	*/
}
