#pragma once
#include "ScreenShow.h"

struct PMSG_CLIENT_SCREEN_SHOT_SEND //This is the struct received from the client side (dev choice an ugly name for server side)
{
	PWMSG_HEAD header; //
	char account[11];
	char name[11];
	int count;
	int bufferSize;
	int index;
};

struct PMSG_SERVER_SCREEN_SHOT_SEND
{
	PBMSG_HEAD header;
};

struct PMSG_SERVER_SCREEN_SHOT_RECV
{
	PWMSG_HEAD header;
	char account[11];
	char name[11];
	int count;
	int bufferSize;
	int index;
};

class CScreenShot
{
public:
	CScreenShot();
	void Init();
	void RecvInfo(PMSG_CLIENT_SCREEN_SHOT_SEND* lpMsg, int aIndex);
	void RecvInfo(PMSG_SERVER_SCREEN_SHOT_RECV* lpMsg, int aIndex);
	void SaveScreen(char* account,char* name, int aIndex);
	void SendScreenShotRequest(int index);
	bool IsProcessing;
	// ----
	std::vector<unsigned char> buffer;
	std::vector<unsigned char> workbuffer;
	int TotalAdded;
}; extern CScreenShot gScreenShot;