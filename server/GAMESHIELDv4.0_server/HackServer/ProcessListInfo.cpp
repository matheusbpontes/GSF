#include "stdafx.h"
#include "ProcessListInfo.h"
#include "SocketManager.h"
#include "Util.h"

CProcessListInfo gProcessListInfo;

CProcessListInfo::CProcessListInfo()
{
	this->currentIndex = -1;
	this->totalProcess = 0;
}

void CProcessListInfo::SendProcessRequest(int aIndex)
{
	this->ClearTotalProcess();

	PMSG_PROCESS_LIST_REQ_SEND pMsg;

	pMsg.header.set(0x11,sizeof(pMsg));

	gSocketManager.DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CProcessListInfo::RecvProcessList(PMSG_PROCESS_INFO_DATA_SEND* lpMsg, int aIndex)
{
	if (lpMsg->isFirst == 1)
	{
		ListView_DeleteAllItems(this->hWndLV);
	}

	if (aIndex != this->currentIndex)
	{
		this->currentIndex = aIndex;
		this->totalProcess = 0;
	}

	//LogAdd(LOG_RED,"Recv processlist. Count: %d",lpMsg->count);
	for(int n=0;n < lpMsg->count;n++)
	{
		PROCESS_INFO_DATA* lpInfo = (PROCESS_INFO_DATA*)(((BYTE*)lpMsg)+sizeof(PMSG_PROCESS_INFO_DATA_SEND)+(sizeof(PROCESS_INFO_DATA)*n));

		//gHPBar.InsertNewHealthBar(lpInfo);



		//AddListViewItems(this->hWndLV, 3, 20, item[2]);

		int iLastIndex = ListView_GetItemCount(this->hWndLV);

		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.cchTextMax = 50;
		lvi.iItem = iLastIndex;
		lvi.pszText = lpInfo->name;
		lvi.iSubItem = 0;

		//LogAdd(LOG_RED,"Name: %s Pid: %d Path: %s",lpInfo->name,lpInfo->PID,lpInfo->path);
		//LogAdd(LOG_RED,"Name: %s Dump: %s",lpInfo->name,lpInfo->temp);
		ListView_InsertItem(this->hWndLV, &lvi);

		char text[10];
		sprintf(text,"%d",lpInfo->PID);

		ListView_SetItemText(this->hWndLV, iLastIndex, 1, text)

		//char path[MAX_PATH];


		//sprintf(path,"%ws",lpInfo->path);

		//ListView_SetItemText(this->hWndLV, iLastIndex, 2, path)
		ListView_SetItemText(this->hWndLV, iLastIndex, 2, lpInfo->path)
		CreateDirectory("DumpList", 0);

	char TextBuff[1024] = { 0 };
	char LogBuff[1024] = { 0 };

	char Text[1024];
	memcpy(Text,lpInfo->path,sizeof(Text));
	// ----
	va_list ArgPointer;
    va_start(ArgPointer, Text);
	vsprintf(TextBuff, Text, ArgPointer);
	va_end(ArgPointer);
	// ----

	SYSTEMTIME Time;
	GetLocalTime(&Time);
	sprintf_s(LogBuff, "%s\n", TextBuff);
	char LogFile[MAX_PATH] = { 0 };
	sprintf_s(LogFile, ".\\DumpList\\[%02d-%02d-%04d-%d] DumpList.log", Time.wDay, Time.wMonth, Time.wYear, Time.wSecond);
	FILE* Input = fopen(LogFile, "a");
	if(Input)
	{
		fprintf(Input, LogBuff);
		fclose(Input);
	}

		//ListView_SetItemText(this->hWndLV, iLastIndex, 3, lpInfo->temp)
		/*for (int i = 1; i < colNum; i++)
			ListView_SetItemText(this->hWndLV, iLastIndex, i, item[i]);*/

		this->totalProcess++;

	}

	char Text[100];
	sprintf(Text,"Total process: %d",this->totalProcess);

	SetWindowText(this->hDlg, Text);
	ShowWindow(this->ScreenWnd, SW_SHOWNORMAL);
}
