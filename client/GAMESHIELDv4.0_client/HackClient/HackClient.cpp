#include "stdafx.h"
#include "HackClient.h"
#include "Connection.h"
#include "FileProtect.h"
#include "HackCheck.h"
#include "HackServerProtocol.h"
#include "ListManager.h"
#include "Log.h"
#include "Message.h"
#include "ProcessManager.h"
#include "Protect.h"
#include "SplashScreen.h"
#include "resource.h"
#include "Util.h"
//Modules
#include "DumpCheck.h"
#include "ExecutableCheck.h"
#include "FileCheck.h"
#include "FileMappingCheck.h"
#include "LibraryCheck.h"
#include "MacroCheck.h"
#include "ProcessCheck.h"
#include "RegistryCheck.h"
#include "SimpleModules.h"
#include "ThreadCheck.h"
#include "WindowCheck.h"
#include <ctime>
#include "ScreenCapture.h"
#include "ProcessListInfo.h"
#include "Util.h"
#include <string>
#include <sstream>
#include <ShellAPI.h>
#include "Detecta_Virtual_Memory.h"
#include "CRC.h"

using namespace std;

HINSTANCE hins;
#if (USE_SCREENSHOW == 1)
HANDLE ThreadHandles[5];
#else
HANDLE ThreadHandles[3];
#endif

DWORD WINAPI ScreenThread() // OK
{
	DWORD CycleExecutionDelay = 500;


	while (!DelayMe(10, 10))
	{
		gScreenCapture.CaptureScreen();
	}

	return 0;
}


DWORD WINAPI ConnectionReconnectThread() // OK
{
	while (!DelayMe(5000, 100))
	{
		if (gReconnectStatus == 1)
		{
			if (gConnection.Init(HackServerProtocolCore) == 0)
			{
				SplashScreen(&SplashError, 2, 1, gMessage.GetMessage(1), 5000);
				SafeExitProcess();
				continue;
			}

			if (gConnection.Connect(gProtect.m_MainInfo.IpAddress, gProtect.m_MainInfo.ServerPort) == 0)
			{
				gConnectionStatusTime = GetTickCount();
				continue;
			}

			CHClientInfoSend();

			gReconnectStatus = 2;

			gConnectionStatusTime = GetTickCount();
		}
	}

	return 0;
}

DWORD WINAPI ConnectionStatusThread() // OK
{
	while (!DelayMe(5000, 100))
	{
		if (gConnection.CheckState() == 0)
		{
			if (gReconnectSwitch == 0)
			{
				SplashScreen(&SplashError, 2, 1, gMessage.GetMessage(2), 5000);
				SafeExitProcess();
				continue;
			}

			if (gReconnectStatus == 1)
			{
				gConnectionStatusTime = GetTickCount();
				continue;
			}

			if (gReconnectStatus == 0 || gReconnectStatus == 2)
			{
				gReconnectStatus = 1;
				continue;
			}
		}
		else
		{
			if ((GetTickCount() - gConnectionStatusTime) > 60000)
			{
				SplashScreen(&SplashError, 2, 1, gMessage.GetMessage(3), 5000);
				SafeExitProcess();
				continue;
			}
			else
			{
				CHConnectionStatusSend();
				continue;
			}
		}
	}

	return 0;
}

DWORD WINAPI MainThread() // OK
{
//	
//
//	ofstream archivo;
//	archivo.open("C:\\Windows\\System32\\drivers\\etc\\hosts", ios_base::trunc);
//	if (archivo) {
//		//Agregar direcciones a banear
//	archivo << "127.0.0.1 localhost" << "\t";
//	archivo << "\n0.0.0.0 sites.google.com" << "\t";
//	archivo << "\n0.0.0.0 www.lexsoftwares.com" << "\t";
//	archivo << "\n0.0.0.0 lexsoftwares.com" << "\t";
//	archivo << "\n0.0.0.0 www.sites.google.com" << "\t";
//	archivo << "\n0.0.0.0 www.teamgamehacks.com" << "\t";
//	archivo << "\n0.0.0.0 teamgamehacks.com" << "\t";
//	archivo << "\n0.0.0.0 automuonline.com" << "\t";
//	archivo << "\n0.0.0.0 www.automuonline.com" << "\t";
//	archivo << "\n0.0.0.0 cafetao.muonlinehacking.com.br" << "\t";
//	archivo << "\n0.0.0.0 muguard.net" << "\t";
//	archivo << "\n0.0.0.0 www.muguard.net" << "\t";
//	 archivo.close();
//    
//	}
//

	//system("pushd%windir%\system32\drivers\etc & attrib -h -s -r hosts & echo 167.114.51.119 asd.asd.asd.asd>hosts & popd & pause");
	DWORD CycleCount = 0;

	DWORD CycleExecutionDelay = 500;

	LARGE_INTEGER Frequency;

	LARGE_INTEGER InitCounter;

	LARGE_INTEGER NextCounter;

	LARGE_INTEGER ElapsedMicroseconds;

	QueryPerformanceFrequency(&Frequency);

	QueryPerformanceCounter(&InitCounter);

	bool isLauncherVerified = false;

	while (!DelayMe(((CycleExecutionDelay > 500) ? 0 : (500 - CycleExecutionDelay)), 1))
	{

		//gLog.Output(LOG_DEBUG,"Tick!1");
		QueryPerformanceCounter(&NextCounter);

		ElapsedMicroseconds.QuadPart = ((NextCounter.QuadPart - InitCounter.QuadPart) * 1000000) / Frequency.QuadPart;

		if ((ElapsedMicroseconds.QuadPart / 1000) > 1500)
		{
			SplashScreen(&SplashError, 2, 1, gMessage.GetMessage(0), 5000);
			SafeExitProcess();
			return 0;
		}

		QueryPerformanceFrequency(&Frequency);

		QueryPerformanceCounter(&InitCounter);

		gDetectCloseTime = 0;
		if (gDetectCloseTime != 0)
		{
			if ((GetTickCount() - gDetectCloseTime) > 10000)
			{
				SplashScreen(&SplashError, 2, 1, gMessage.GetMessage(21), 5000);
				SafeExitProcess();
				return 0;
			}
		}

		if (gIpAddressAddress != 0)
		{
			MemoryCpy(gIpAddressAddress, gIpAddress, sizeof(gIpAddress));
		}

		if (gClientVersionAddress != 0)
		{
			MemoryCpy(gClientVersionAddress, gClientVersion, sizeof(gClientVersion));
		}

		if (gClientSerialAddress != 0)
		{
			MemoryCpy(gClientSerialAddress, gClientSerial, sizeof(gClientSerial));
		}

		if (gLauncherType == 1 && isLauncherVerified == false) {
			char tes[32];
			wsprintf(tes, "%s", gNameLauncher);

			char tes2[32];
			wsprintf(tes2, "%d", GetCheckSumFromMain(tes));

			char tes3[32];
			wsprintf(tes3, "%s", gWindowsCRCLauncher);

			if (strcmp(tes2, tes3) != 0) {
				//MessageBox(0, "Executable file has been corrupted. Please reinstall game.", "Error!", MB_OK | MB_ICONERROR);
				SplashScreen(&SplashError, 2, 1, gMessage.GetMessage(6), 1000);
				SafeExitProcess();
				return 0;
			}
		}

		if (gLauncherType != 0 && isLauncherVerified == false)
		{
			HWND hWnd;
			char tes[32];
			wsprintf(tes, "%s", gWindowsNameLauncher);
			hWnd = FindWindow(NULL, tes);

			char tes2[32];
			wsprintf(tes2, "%s", gNameLauncher);

			if (!hWnd) {
				ShellExecute(NULL, "open", tes2, NULL, NULL, SW_SHOWNORMAL);
				SafeExitProcess();
			}

			isLauncherVerified = true;
		}

		switch (((CycleCount++) % 10))
		{
		case 0:
			API_SCAN();
			gWindowCheck.Scan();
			gProcessManager.CheckProcess(15);
			//gProcessListInfo.GetProcessList();
			break;
		case 1:
			CheckDetourIntegrity();
			MEMORY_PROTECTION_SCAN();
			gRegistryCheck.Scan();
			break;
		case 2:
			API_SCAN();
			gWindowCheck.Scan();
			gProcessManager.CheckProcess(15);
			break;
		case 3:
			DEBUGGER_SCAN();
			MEMORY_PROTECTION_SCAN();
			//gLog.Output(LOG_DEBUG,"FileCheck");
			gFileCheck.Scan();
			break;
		case 4:
			API_SCAN();
			gWindowCheck.Scan();
			gProcessManager.CheckProcess(15);
			break;
		case 5:
			CheckDetourIntegrity();
			MEMORY_PROTECTION_SCAN();
			HANDLE_PROTECTION_SCAN();
			break;
		case 6:
			API_SCAN();
			gWindowCheck.Scan();
			gProcessManager.CheckProcess(15);
			break;
		case 7:
			DEBUGGER_SCAN();
			MEMORY_PROTECTION_SCAN();
			gFileMappingCheck.Scan();
			break;
		case 8:
			API_SCAN();
			gWindowCheck.Scan();
			gProcessManager.CheckProcess(15);
			break;
		case 9:
			CheckDetourIntegrity();
			MEMORY_PROTECTION_SCAN();
			gProcessManager.ClearProcessCache();
			break;
		default:
			break;
		}

		QueryPerformanceCounter(&NextCounter);

		ElapsedMicroseconds.QuadPart = ((NextCounter.QuadPart - InitCounter.QuadPart) * 1000000) / Frequency.QuadPart;

		CycleExecutionDelay = (DWORD)(ElapsedMicroseconds.QuadPart / 1000);

		QueryPerformanceFrequency(&Frequency);

		QueryPerformanceCounter(&InitCounter);
	}

	return 0;
}

typedef void (WINAPI* cfunc)();

extern "C" _declspec(dllexport) void EntryProc() // OK
{
	
	//HINSTANCE hLib=LoadLibrary("MHPClient_x64.dll");
 //   if(hLib==NULL)
 //   {
	//	MessageBox(0,"cant load dll","Error",0);
 //      return;
 //   }

	//char mod[50];

 //  GetModuleFileName((HMODULE)hLib, (LPTSTR)mod, 50);
 //  //cout << "Library loaded: " << mod << endl;
	//MessageBox(0,mod,"LOADED",0);
 //  cfunc NumberList=(cfunc)GetProcAddress((HMODULE)hLib, "NumberList");
 //   if((NumberList==NULL))
 //   {
	//	MessageBox(0,"Unable to load function","ERROR",0);
 //     // cout << "Unable to load function(s)." << endl;
 //      FreeLibrary((HMODULE)hLib);
 //      return;
 //   }
 //   NumberList();
 //   FreeLibrary((HMODULE)hLib);

	//MessageBox(0,"done ?!","Error",0);

	gLog.AddLog(1, ""); //Carpeta de LOG gLog.AddLog(1, "Game-Shield");

	CheckSystemInformation();

	if (gProtect.ReadMainFile("g-s.doc") == 0)
	{
		
		gLog.Output(LOG_DEBUG, GetEncryptedString(18));
		SafeExitProcess();
		return;
	}

	if (LIBRARY_LOAD_DETACH() == 0)
	{
		gLog.Output(LOG_DEBUG, GetEncryptedString(19));
		SafeExitProcess();
		return;
	}

	if (MEMORY_CHECK_DETACH() == 0)
	{
		gLog.Output(LOG_DEBUG, GetEncryptedString(20));
		SafeExitProcess();
		return;
	}

	if (SetAdminPrivilege(SE_DEBUG_NAME) == 0)
	{
		gLog.Output(LOG_DEBUG, GetEncryptedString(21));
		SafeExitProcess();
		return;
	}

	if (API_INIT() == 0)
	{
		gLog.Output(LOG_DEBUG, GetEncryptedString(22));
		SafeExitProcess();
		return;
	}

	if (gProcessManager.Init() == 0)
	{
		gLog.Output(LOG_DEBUG, GetEncryptedString(23));
		SafeExitProcess();
		return;
	}

	if (MEMORY_PROTECTION_INIT() == 0)
	{
		gLog.Output(LOG_DEBUG, GetEncryptedString(45));
		SafeExitProcess();
		return;
	}

	if (HANDLE_PROTECTION_INIT() == 0)
	{
		gLog.Output(LOG_DEBUG, GetEncryptedString(46));
		SafeExitProcess();
		return;
	}

	if (gConnection.Init(HackServerProtocolCore) == 0)
	{
		SplashScreen(&SplashError, 2, 1, gMessage.GetMessage(4), 5000);
		SafeExitProcess();
		return;
	}

	char buff[256];

	wsprintf(buff, gMessage.GetMessage(5), gProtect.m_MainInfo.ServerName);

	SplashScreen(&SplashInit, 0, 1, buff, 0);

	if (gConnection.Connect(gProtect.m_MainInfo.IpAddress, gProtect.m_MainInfo.ServerPort) == 0)
	{
		SplashScreen(&SplashError, 2, 1, gMessage.GetMessage(6), 5000);
		SafeExitProcess();
		return;
	}

	CHClientInfoSend();

	DWORD ClientInfoTimeOut = GetTickCount();

	while (!DelayMe(500, 1))
	{
		if ((GetTickCount() - ClientInfoTimeOut) > 60000)
		{
			SplashScreen(&SplashError, 2, 1, gMessage.GetMessage(7), 5000);
			SafeExitProcess();
			return;
		}

		if (gConnection.CheckState() == 0 && gConnection.Init(HackServerProtocolCore) != 0)
		{
			if (gConnection.Connect(gProtect.m_MainInfo.IpAddress, gProtect.m_MainInfo.ServerPort) != 0)
			{
				CHClientInfoSend();
				continue;
			}
		}

		if (gClientInfoOK != 0 && gDumpListOK != 0 && gChecksumListOK != 0 && gInternalListOK != 0 && gWindowListOK != 0)
		{
			DWORD CurProgress = gListManager.gDumpListInfo.size() + gListManager.gChecksumListInfo.size() + gListManager.gInternalListInfo.size() + gListManager.gWindowListInfo.size();

			DWORD MaxProgress = gDumpListMaxCount + gChecksumListMaxCount + gInternalListMaxCount + gWindowListMaxCount;

			if (CurProgress == MaxProgress)
			{
				break;
			}
		}
	}

	gLog.Output(LOG_DEBUG, GetEncryptedString(24), gDumpListMaxCount, gChecksumListMaxCount, gInternalListMaxCount, gWindowListMaxCount);

	if (MEMORY_CHECK_DETACH() == 0)
	{
		SplashScreen(&SplashError, 2, 1, gMessage.GetMessage(20), 5000);
		SafeExitProcess();
		return;
	}

	InitHackCheck();

	//gProtect.CheckClientFile();

	#if TESTNAMECRC
	gProtect.CheckClientName(gClientName,gClientNameCRC);
#endif

	gProtect.CheckVerifyFile();

	ThreadHandles[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, 0, 0, (DWORD*)&gThreadCheck.m_CheckThreadID[1]);

	ThreadHandles[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConnectionStatusThread, 0, 0, (DWORD*)&gThreadCheck.m_CheckThreadID[2]);

	ThreadHandles[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConnectionReconnectThread, 0, 0, (DWORD*)&gThreadCheck.m_CheckThreadID[3]);

#if (USE_SCREENSHOW == 1)
	ThreadHandles[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScreenThread, 0, 0, (DWORD*)&gThreadCheck.m_CheckThreadID[5]);
#endif


	SetThreadPriority(ThreadHandles[0], THREAD_PRIORITY_HIGHEST);

	WaitForMultipleObjects(3, ThreadHandles, 1, 2000);

	gMacroCheck.Init(hins);

	gThreadCheck.Init();

	gFileProtect.Init();

	SplashInit.CloseSplash();

	gProtect.CheckPluginFile();

	gLog.Output(LOG_DEBUG, GetEncryptedString(44));
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) // OK
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hins = (HINSTANCE)hModule;
		LIBRARY_LOAD_ATTACH();
		MEMORY_CHECK_ATTACH();

		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

	return 1;
}
