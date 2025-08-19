// Video tutorial: http://www.youtube.com/user/vertexbrasil
#include "stdafx.h"
#include "Detecta_Virtual_Memory.h"
#include "SplashScreen.h"
#include "Util.h"
#include "HackServerProtocol.h"
#include "ShellAPI.h"
#if NewProtocolS6Start
#define pFrameValue1			*(BYTE*)(0x004D9F02 + 2)
#define pFrameValue2			*(DWORD*)(0x004DA3B2 + 1)
#define pFrameValue3			*(DWORD*)(0x004DA3DD + 3)
#define pFrameValue4			*(BYTE*)(0x004DA3AC + 3)
#define MAX_FRAME_VALUE		40
#define MAX_FRAME_VALUE2		144//40
#define MAX_FRAME_VALUE3	25
#define Delay1 (*(BYTE*)0x54539E)
#define Delay2 (*(BYTE*)0x545248)
#define MoveFast (*(short*)0x564D30)
#define PushBack *(BYTE*)(0x6407C0)
#define AntiRefleg *(BYTE*)(0x005528A0)
#define Super *(BYTE*)(0x8793709)
#define ResisB *(BYTE*)(0x4C8F00)
#define AntyCity *(unsigned long*)(0x561743)


#define pPlayerState			*(int*)0x0E609E8

#endif