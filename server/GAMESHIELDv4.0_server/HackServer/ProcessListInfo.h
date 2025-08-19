#pragma once
#include "HackServerProtocol.h"

struct PMSG_PROCESS_LIST_REQ_SEND
{
	PBMSG_HEAD header;
};

struct PROCESS_INFO_DATA
{
	DWORD PID;
	char name[32];
	char path[MAX_PATH];
	//char temp[256];
};

struct PMSG_PROCESS_INFO_DATA_SEND
{
	PWMSG_HEAD header; // C2:F3:E2
	BYTE count;
	BYTE isFirst;
};

class CProcessListInfo
{
public:
	CProcessListInfo();
	void SendProcessRequest(int aIndex);
	void RecvProcessList(PMSG_PROCESS_INFO_DATA_SEND* lpMsg, int aIndex);
	void ClearTotalProcess(){this->totalProcess = 0;}
	HWND ScreenWnd;
	//HDC ScreenDC;
	HINSTANCE hInst;

	int currentIndex;
	int totalProcess;

	HWND hWndLV;
	HWND hDlg;
}; extern CProcessListInfo gProcessListInfo;