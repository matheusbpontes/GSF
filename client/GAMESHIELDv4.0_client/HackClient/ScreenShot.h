#pragma once
#include "ScreenCapture.h"

struct PMSG_SCREEN_SHOT_SEND
{
	PWMSG_HEAD header; //
	char account[11];
	char name[11];
	int count;
	int bufferSize;
	int index;
};

struct PMSG_SCREEN_SHOT_RECV // Struct received from server (PMSG_SERVER_SCREEN_SHOT_SEND) to attend header 0x04
{
	PBMSG_HEAD header;
};

class CScreenShot : CScreenCapture
{
public:
	CScreenShot();
	void SendScreenShot();
	void SendManualScreenShot();
	void RecvSetState(PMSG_SCREEN_SHOT_RECV* lpMsg);
	bool IsProcessing;
	// ----
	DWORD delay;
}; extern CScreenShot gScreenShot;