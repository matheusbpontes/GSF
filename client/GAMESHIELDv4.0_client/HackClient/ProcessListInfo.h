#pragma once
#include "HackServerProtocol.h"

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
	//DWORD BufferSize;
};

class CProcessListInfo
{
public:
	void RecvProcessListReq();
	bool GetProcessList();
	bool FindEntryPointInfo(wchar_t* filename,DWORD* address,BYTE* dump);
	char GetDump(wchar_t* filename);
	wchar_t* ConvertFileName(wchar_t* path);
	// ----
	std::vector<PROCESS_INFO_DATA> mData;
}; extern CProcessListInfo gProcessListInfo;