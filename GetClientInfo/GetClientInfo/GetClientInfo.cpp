#include "stdafx.h"
#include "CCRC32.H"
#include "Message.h"
#include "AntiCrack.h"
#include "Protection.h"

struct MAIN_FILE_INFO
{
	char CustomerName[32];
	char IpAddress[32];
	WORD ServerPort;
	char ServerName[32];
	char ClientName[32];
	char PluginName[32];
	char VerifyName[32];
	char RegistryHWID[32];
	DWORD ClientCRC32;
	DWORD PluginCRC32;
	DWORD VerifyCRC32;
	MESSAGE_INFO EngMessageInfo[MAX_MESSAGE];
	MESSAGE_INFO PorMessageInfo[MAX_MESSAGE];
	MESSAGE_INFO SpnMessageInfo[MAX_MESSAGE];
};

int _tmain(int argc, _TCHAR *argv[]) // OK
{
	VMProtectBeginUltra("Main");
	//VMProtectBeginUltra("Main");

cout << "==============================================================" << endl;
cout << "#######          #        #####  #     # ### ####### #       ######" << endl;  
cout << "#               # #      #     # #     #  #  #       #       #     #" << endl; 
cout << "#              #   #     #       #     #  #  #       #       #     #" << endl; 
cout << "#####   ##### #     #     #####  #######  #  #####   #       #     #" << endl; 
cout << "#             #######          # #     #  #  #       #       #     #" << endl; 
cout << "#             #     #    #     # #     #  #  #       #       #     #" << endl; 
cout << "#######       #     #     #####  #     # ### ####### ####### ######" << endl;                                                               


	//cout << "==============================================================" << endl;
	//cout << "   _________    __  _________   ________  _____    ____  ____ " << endl;
	//cout << "  / ____/   |  /  |/  / ____/  / ____/ / / /   |  / __ \\/ __ \\" << endl;
	//cout << " / / __/ /| | / /|_/ / __/    / / __/ / / / /| | / /_/ / / / /" << endl;
	//cout << "/ /_/ / ___ |/ /  / / /___   / /_/ / /_/ / ___ |/ _, _/ /_/ / " << endl;
	//cout << "\\____/_/  |_/_/  /_/_____/   \\____/\\____/_/  |_/_/ |_/_____/  " << endl;

	cout << "==============================================================" << endl;
	cout << "Credits: AlexRios      Website: https://EA-Gaming.com" << endl;
	cout << "==============================================================" << endl;

#if (ANTI_CRACK == 1)
	MainProtection();
#endif

	MAIN_FILE_INFO info;

	memset(&info, 0, sizeof(info));

	char TempInfo[100];

	GetPrivateProfileString("ClientInfo", "CustomerName", "", info.CustomerName, sizeof(info.CustomerName), ".\\ClientInfo.ini");

	cout << "[License] CustomerName:\t\t\t" << info.CustomerName << endl;

	/*if (gProtection.GetAccessToken(info.CustomerName))
	{
		if (!gProtection.Auth())
		{
			cout << "[License] Check fails......" << endl
				 << endl;
			getchar();
			gProtection.SafeExitProcess();
		}*/

		char type;
	again:
		cout << "[License] Use IP Proxy/FW? (Y/N):\t";

		cin >> type;
		getchar();
		switch (type)
		{
		case 'y':
		case 'Y':

			strcpy_s(info.IpAddress, gProtection.m_IpAddress2);
			info.IpAddress[32] = GetPrivateProfileString("ClientInfo", "IpAddress", "", info.IpAddress, sizeof(info.IpAddress), ".\\ClientInfo.ini");
			break;
		case 'n':
		case 'N':

			strcpy_s(info.IpAddress, gProtection.m_IpAddress1);
			
			break;
		default:
			goto again;
			break;
		}

		cout << "[License] IpAddress:\t\t\t" << info.IpAddress << endl;

		info.ServerPort = GetPrivateProfileInt("ClientInfo", "ServerPort", 0, ".\\ClientInfo.ini");

		cout << "[License] ServerPort:\t\t\t" << info.ServerPort << endl;

		GetPrivateProfileString("ClientInfo", "ServerName", "", info.ServerName, sizeof(info.ServerName), ".\\ClientInfo.ini");

		cout << "[License] ServerName:\t\t\t" << info.ServerName << endl;

		GetPrivateProfileString("ClientInfo", "ClientName", "", info.ClientName, sizeof(info.ClientName), ".\\ClientInfo.ini");

		strcpy_s(info.RegistryHWID, "SYSTEM\\HardwareConfig\\Current");

		strcpy_s(info.PluginName, "Data/Custom/EA-Shield.bmd");

		strcpy_s(info.VerifyName, "Data/Custom/EA-Shield4.0.bmd");

		gMessage.Load("Message.txt");

		memcpy(info.EngMessageInfo, gMessage.m_EngMessageInfo, sizeof(info.EngMessageInfo));

		memcpy(info.PorMessageInfo, gMessage.m_PorMessageInfo, sizeof(info.PorMessageInfo));

		memcpy(info.SpnMessageInfo, gMessage.m_SpnMessageInfo, sizeof(info.SpnMessageInfo));

		CCRC32 CRC32;

		if (CRC32.FileCRC(info.ClientName, &info.ClientCRC32, 1024) == 0)
		{
			info.ClientCRC32 = 0;
		}

		if (CRC32.FileCRC(info.PluginName, &info.PluginCRC32, 1024) == 0)
		{
			info.PluginCRC32 = 0;
		}

		if (CRC32.FileCRC(info.VerifyName, &info.VerifyCRC32, 1024) == 0)
		{
			info.VerifyCRC32 = 0;
		}

		for (int n = 0; n < sizeof(MAIN_FILE_INFO); n++)
		{
			((BYTE *)&info)[n] ^= 0x7D;
			((BYTE *)&info)[n] -= 0xF5;
		}

		HANDLE file = CreateFile("Data/Custom/EA-SHIELD/EA-LicenseV2.dll", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);

		if (file == INVALID_HANDLE_VALUE)
		{
			return 0;
		}

		DWORD OutSize = 0;

		if (WriteFile(file, &info, sizeof(MAIN_FILE_INFO), &OutSize, 0) == 0)
		{
			CloseHandle(file);
			return 0;
		}

		CloseHandle(file);
		cout << "[License] Expire at:\t\t\t" << gProtection.m_ExpireAt << endl;

		cout << "==============================================================" << endl;
		cout << "[Result] Success" << endl;
		cout << "==============================================================" << endl;
		cout << "[Tutorial] Copy the EA-LicenseV2.dll to Client/Data/Custom/EA-SHILD FOLDER" << endl;
		cout << "==============================================================" << endl;
		cout << "Press any key to exit......." << endl;

		getchar();
	/*}
	else
	{
		cout << "==============================================================" << endl;
		cout << "[Result] Failed......" << endl;
		cout << "==============================================================" << endl;
		cout << "Press any key to exit......." << endl;

		getchar();
		gProtection.SafeExitProcess();
	}*/

	VMProtectEnd();

	return 0;
}
