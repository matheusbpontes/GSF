#include "StdAfx.h"
#include "START.h"

CARREGAR carrega;

HANDLE m_DllModule;

extern  "C"  __declspec(dllexport) void __cdecl GasMask_5()  // Função API (init) da dll
{
//Splash messages
	carrega.Nome_das_Janelas			= "ANetworkMU AntiHack"; // // [CHOICE!] Splash message Server name
	carrega.Message_Warning_En          = 4; // [CHOICE!] Hack detect splash message: [Options: 0,1,2,3,4]  0 = silent, 1 = Engish, 2 = Portuguese 3 = Open php webpage with splash image and make a Log.txt file with Date / ip, 4 = Message with hack name detected (for configuration purposes only)
	carrega.HackSplash_WebSite          = "http://localhost:8090/Codex/HackSplash/Splash.php";

	carrega.Log_Txt_Hack                = 0; // On/Off - Creates a hack log ("MHP_LOG/Hack.txt")
//////////////////////////////////////////////////////////////////////////
	//virtual memorty protection /////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////	
	carrega.Memory_Buffer_P             = 0;     // On/Off - [CONFIGURATION!] Message box with address buffer for configuration! Need to configure values on: Detecta_Virtual_Memory.cpp
	carrega.Virtual_Mem_occours         = 935;   // Occours every X miliseconds. [ ATENTION! If antikill by Scans active this value canot be more than 1000 ]
//Rebuild main data (Watch Video Tutorial to understand this function)
}