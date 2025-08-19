#include "stdafx.h"
#include "ScreenShot.h"
#include "ServerDisplayer.h"
#include "Util.h"
#include "SocketManager.h"

CScreenShot gScreenShot;

CScreenShot::CScreenShot()
{
	this->Init();
}


void CScreenShot::Init()
{
	this->buffer.clear();
	this->workbuffer.clear();
	this->TotalAdded = 0;
	CreateDirectoryA("ScreenShots",0);
}


void CScreenShot::RecvInfo(PMSG_CLIENT_SCREEN_SHOT_SEND* lpMsg, int aIndex)
{
	static int lastindex = 0;

	//if (lpMsg->index != lastindex)
	//{
	//	//LogAdd(LOG_RED,"LastIndex != CurrentIndex [%d] [%d]",lastindex, lpMsg->index);
	//}
	//else
	//{
	//	//LogAdd(LOG_RED,"[Good] LastIndex == CurrentIndex [%d] [%d]",lastindex, lpMsg->index);
	//}

	if (lpMsg->index == 0)
	{
		lastindex=0;
		this->Init();
	}

	lastindex++;

	for(int n=0;n < lpMsg->count;n++)
	{
		PMSG_SCREEN* lpInfo = (PMSG_SCREEN*)(((BYTE*)lpMsg)+sizeof(PMSG_CLIENT_SCREEN_SHOT_SEND)+(sizeof(PMSG_SCREEN)*n));
		this->buffer.push_back(lpInfo->value);
		this->TotalAdded++;

		if (this->buffer.size() == lpMsg->bufferSize)
		{
			this->workbuffer.clear();
			this->workbuffer = this->buffer;

			this->SaveScreen(lpMsg->account,lpMsg->name,aIndex);
		}
		else if (this->buffer.size() >= lpMsg->bufferSize)
		{
			LogAdd(LOG_GREEN,"ERROR! Buffer length bigger then expected size!");
		}
	}
}

void CScreenShot::RecvInfo(PMSG_SERVER_SCREEN_SHOT_RECV* lpMsg, int aIndex)
{
	static int lastindex = 0;

	if (lpMsg->index == 0)
	{
		lastindex = 0;
		this->Init();
	}

	lastindex++;

	for (int n = 0; n < lpMsg->count; n++)
	{
		PMSG_SCREEN* lpInfo = (PMSG_SCREEN*)(((BYTE*)lpMsg) + sizeof(PMSG_CLIENT_SCREEN_SHOT_SEND) + (sizeof(PMSG_SCREEN) * n));
		this->buffer.push_back(lpInfo->value);
		this->TotalAdded++;

		if (this->buffer.size() == lpMsg->bufferSize)
		{
			this->workbuffer.clear();
			this->workbuffer = this->buffer;

			this->SaveScreen(lpMsg->account, lpMsg->name, aIndex);
		}
		else if (this->buffer.size() >= lpMsg->bufferSize)
		{
			LogAdd(LOG_GREEN, "ERROR! Buffer length bigger then expected size!");
		}
	}
	gScreenShot.IsProcessing = false;
}

void CScreenShot::SaveScreen(char* account,char* name,int aIndex)
{
	if (this->TotalAdded==0)
	{
		return;
	}

	if (this->workbuffer.size() == 0)
	{
		return;
	}

	UINT len = this->workbuffer.size();
	HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE *pmem = (BYTE*)GlobalLock(hMem);
	memcpy(pmem, &this->workbuffer[0], len);
	IStream    *stream = NULL;
	CreateStreamOnHGlobal(hMem, FALSE, &stream);
	CImage image;
	image.Load(stream);
	stream->Release();
	GlobalUnlock(hMem);
	GlobalFree(hMem);

	//CImage LogoImage;
	//LogoImage.Load("logo.png");

	wchar_t text[50];
	wchar_t text1[50];
	wchar_t text2[50];
	wchar_t text3[50];

	swprintf(text,L"Account: %hs",account);
	swprintf(text1,L"Name: %hs",name);
	swprintf(text2,L"IP Address: %hs",GetIpByIndex(aIndex));
	swprintf(text3,L"Hwid: %hs",GetHwidByIndex(aIndex));

	Gdiplus::Graphics graphics ( image.GetDC () );
	Gdiplus::Font font ( &FontFamily ( L"Arial" ), 16 );
	Gdiplus::SolidBrush brush ( Color ( 255, 0, 0, 255 ) );
	graphics.DrawString ( text, -1, &font, PointF(0.0f, 0.0f), &brush );
	graphics.DrawString ( text1, -1, &font, PointF(0.0f, 20.0f), &brush );
	graphics.DrawString ( text2, -1, &font, PointF(0.0f, 40.0f), &brush );
	graphics.DrawString ( text3, -1, &font, PointF(0.0f, 60.0f), &brush );
	image.ReleaseDC();

	SYSTEMTIME time;
	GetLocalTime(&time);
	
	char path[128];
	sprintf(path,"ScreenShots\\%s-%s\\%04d-%02d-%02d-%02d-%02d-%02d-%02d.jpg",account,name,time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute, time.wSecond, time.wMilliseconds);

	char directory[128];
	sprintf(directory,"ScreenShots\\%s-%s",account,name);
	CreateDirectoryA(directory,0);
	
	HRESULT ret = image.Save(path,ImageFormatJPEG);
	//LogAdd(LOG_RED,"res = %d",ret);
	//image.Draw(hdc,0,0,gScreenShow.ScreenWidth,gScreenShow.ScreenHeight,0,0,gScreenShow.ScreenWidth,gScreenShow.ScreenHeight+gScreenShow.ScreenDiff);
	//image.StretchBlt(hdc,0,0,gScreenShow.ScreenWidth,gScreenShow.ScreenHeight/*-30*//*image.GetWidth(),image.GetHeight()*/,SRCCOPY);
	//LogoImage.StretchBlt(hdc,gScreenShow.ScreenWidth - LogoImage.GetWidth()-6-10,10,LogoImage.GetWidth(),LogoImage.GetHeight()/*image.GetWidth(),image.GetHeight()*/,SRCCOPY);

	

	image.Destroy();
//	LogoImage.Destroy();
}

void CScreenShot::SendScreenShotRequest(int index) {
	if (gScreenShot.IsProcessing == false) {
		gScreenShot.IsProcessing = true;
		PMSG_SERVER_SCREEN_SHOT_SEND pMsg;
		pMsg.header.set(0x04, sizeof(pMsg));
		gSocketManager.DataSend(index, (BYTE*)&pMsg, pMsg.header.size);
		LogAdd(LOG_PURPLE, "[SendScreenShotRequest] data sent");
	}
	else {
		LogAdd(LOG_PURPLE, "[SendScreenShotRequest] Another screenshot is still being processed");
	}
}