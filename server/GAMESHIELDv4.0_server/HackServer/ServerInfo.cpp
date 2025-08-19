// ServerInfo.cpp: implementation of the CServerInfo class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ServerInfo.h"
#include "CCRC32.H"
#include "ReadFiles.h"
#include "BlackList.h"
#include "Util.h"

CServerInfo gServerInfo;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerInfo::CServerInfo() // OK
{

}

CServerInfo::~CServerInfo() // OK
{

}

void CServerInfo::ReadInit() // OK
{
	CCRC32 CRC32;

	this->ReadConfig();

	this->ReadDumpList();

	this->ReadChecksumList();

	this->ReadInternalList();

	this->ReadWindowList();

	this->ReadBlackList();

	if(CRC32.FileCRC(".\\Data\\info.xtm",&this->m_ClientFileCRC,1024) == 0)
	{
		this->m_ClientFileCRC = 0;
	}
}

void CServerInfo::ReadConfig() // OK
{
	this->ReadConfig("GameShield",".\\Configs.ini");

		LogAdd(LOG_PURPLE,"[Configs] Config loaded successfully");
		LogAdd(LOG_BLUE,"[Configs] Memory Module loaded successfully");
		LogAdd(LOG_BLUE,"[Configs] Injeccion Module loaded successfully");
		LogAdd(LOG_BLUE,"[Configs] Packets Module loaded successfully");
		LogAdd(LOG_BLUE,"[Configs] Class Module loaded successfully");
		LogAdd(LOG_GREEN,"[Configs] Library Module loaded successfully");
		LogAdd(LOG_BLUE,"[Configs] Graphic Speed Module loaded successfully");
		LogAdd(LOG_BLUE,"[Configs] Phisical Speed Module loaded successfully");
		LogAdd(LOG_BLUE,"[Configs] Memory Edition Module loaded successfully");
		LogAdd(LOG_BLUE,"[Configs] Anti-DDoS Module loaded successfully");
		LogAdd(LOG_BLUE,"[Configs] Anti-Flood Module loaded successfully");
		LogAdd(LOG_BLUE,"[Configs] Threads Module loaded successfully");
}

void CServerInfo::ReadDumpList() // OK
{
	gReadFiles.DumpList(".\\Data\\Dump.List.db");

	LogAdd(LOG_ORANGE,"[Configs] HackList loaded successfully");
}

void CServerInfo::ReadChecksumList() // OK
{
	gReadFiles.ChecksumList(".\\Data\\Archivos.txt");

	LogAdd(LOG_ORANGE,"[Configs] FilesProtection loaded successfully");
}

void CServerInfo::ReadInternalList() // OK
{
	gReadFiles.InternalList(".\\Data\\ListaInterna.sys");

	LogAdd(LOG_ORANGE,"[Configs] InternalList loaded successfully");
}

void CServerInfo::ReadWindowList() // OK
{
	gReadFiles.WindowList(".\\Data\\Window.List.db");

	LogAdd(LOG_ORANGE,"[Configs] NameList loaded successfully");
}

void CServerInfo::ReadBlackList() //OK
{
	gBlackList.SetList(".\\BanList.txt");

	LogAdd(LOG_RED,"[Configs] BANLIST loaded successfully");
}

void CServerInfo::ReadStartupInfo(char* section,char* path) // OK
{
	char HackServerInfo[100] = {0};

	GetPrivateProfileString(section,"CustomerName","",this->m_CustomerName,sizeof(this->m_CustomerName),path);

	GetPrivateProfileString(section,"CustomerHardwareId","",this->m_CustomerHardwareId,sizeof(this->m_CustomerHardwareId),path);

	this->m_HackServerPort = GetPrivateProfileInt(section,"MHPServerPort",0,path);

	this->m_MaxIpConnection = GetPrivateProfileInt(section,"MaxIpConnection",0,path);

	this->m_EncDecKey1 = GetPrivateProfileInt(section,"EncDecKey1",0,path);

	this->m_EncDecKey2 = GetPrivateProfileInt(section,"EncDecKey2",0,path);

	this->m_EncDecSetNumber = GetPrivateProfileInt(section,"EncDecSetNumber",0,path);

	this->m_MaxHardwareIdConnection = GetPrivateProfileInt(section, "MultiSesionHardwareId", 0, path);

	EnableFloodSystem = GetPrivateProfileInt(section,"EnableFloodSystem",1, path);

	IpLimitConnectionTime = GetPrivateProfileInt(section,"IpLimitConnectionTime",0, path);

	IpConnectionBlockedTime = GetPrivateProfileInt(section,"IpConnectionBlockedTime",0, path);

	IpMaxAttemptsConnectionTime = GetPrivateProfileInt(section,"IpMaxAttemptsConnectionTime",0, path);
}

void CServerInfo::ReadConfig(char* section,char* path) // OK
{
	char HackServerInfo[100] = {0};

	this->LauncherType = GetPrivateProfileInt(section, "LauncherType", 0, path);

	GetPrivateProfileString(section, "LauncherInstance", "", this->WindowsNameLauncher, sizeof(this->WindowsNameLauncher), path);

	GetPrivateProfileString(section, "LauncherCRC", "", this->WindowsCRCLauncher, sizeof(this->WindowsCRCLauncher), path);

	GetPrivateProfileString(section, "LauncherName", "", this->NameLauncher, sizeof(this->NameLauncher), path);
#if TESTNAMECRC
	GetPrivateProfileString(section,"ClientName","",this->ClientName,sizeof(this->ClientName),path);

	GetPrivateProfileString(section,"ClientNameCRC","",this->ClientNameCRC,sizeof(this->ClientNameCRC),path);
#endif

	GetPrivateProfileString(section,"ClientVersion","",this->m_ClientVersion,sizeof(this->m_ClientVersion),path);

	GetPrivateProfileString(section,"ClientSerial","",this->m_ClientSerial,sizeof(this->m_ClientSerial),path);

	this->m_DetectionLockTime = GetPrivateProfileInt(section,"DetectionLockTime",0,path);

	this->m_InternalVersion = GetPrivateProfileInt(section,"InternalVersion",0,path);

	this->m_ReconnectSwitch = GetPrivateProfileInt(section,"ReconnectSwitch",0,path);

	this->m_MemoryGuardSwitch = GetPrivateProfileInt(section,"MemoryGuardSwitch",0,path);

	this->m_MemoryGuardNumber = GetPrivateProfileInt(section,"MemoryGuardNumber",0,path);

	this->m_MacroProtectSwitch = GetPrivateProfileInt(section,"MacroProtectSwitch",0,path);

	this->m_MacroProtectNumber = GetPrivateProfileInt(section,"MacroProtectNumber",0,path);

	this->m_MacroProtectS6Switch = GetPrivateProfileInt(section,"MacroProtectS6",0,path);

	GetPrivateProfileString(section,"MemoryAddress1","",HackServerInfo,sizeof(HackServerInfo),path);

	this->m_MemoryAddress1 = (((this->m_MemoryAddress1=strtoul(HackServerInfo,NULL,0))>0x10000000)?(this->m_MemoryAddress1^0xC47A0E9F):this->m_MemoryAddress1);

	GetPrivateProfileString(section,"MemoryAddress2","",HackServerInfo,sizeof(HackServerInfo),path);

	this->m_MemoryAddress2 = (((this->m_MemoryAddress2=strtoul(HackServerInfo,NULL,0))>0x10000000)?(this->m_MemoryAddress2^0xC47A0E9F):this->m_MemoryAddress2);

	GetPrivateProfileString(section,"MemoryAddress3","",HackServerInfo,sizeof(HackServerInfo),path);

	this->m_MemoryAddress3 = (((this->m_MemoryAddress3=strtoul(HackServerInfo,NULL,0))>0x10000000)?(this->m_MemoryAddress3^0xC47A0E9F):this->m_MemoryAddress3);

	GetPrivateProfileString(section,"MemoryAddress4","",HackServerInfo,sizeof(HackServerInfo),path);

	this->m_MemoryAddress4 = (((this->m_MemoryAddress4=strtoul(HackServerInfo,NULL,0))>0x10000000)?(this->m_MemoryAddress4^0xC47A0E9F):this->m_MemoryAddress4);

	GetPrivateProfileString(section,"MemoryAddress5","",HackServerInfo,sizeof(HackServerInfo),path);

	this->m_MemoryAddress5 = (((this->m_MemoryAddress5=strtoul(HackServerInfo,NULL,0))>0x10000000)?(this->m_MemoryAddress5^0xC47A0E9F):this->m_MemoryAddress5);

	GetPrivateProfileString(section,"MemoryAddress6","",HackServerInfo,sizeof(HackServerInfo),path);

	this->m_MemoryAddress6 = (((this->m_MemoryAddress6=strtoul(HackServerInfo,NULL,0))>0x10000000)?(this->m_MemoryAddress6^0xC47A0E9F):this->m_MemoryAddress6);

	GetPrivateProfileString(section,"MemoryAddress7","",HackServerInfo,sizeof(HackServerInfo),path);

	this->m_MemoryAddress7 = (((this->m_MemoryAddress7=strtoul(HackServerInfo,NULL,0))>0x10000000)?(this->m_MemoryAddress7^0xC47A0E9F):this->m_MemoryAddress7);

	GetPrivateProfileString(section,"MemoryAddress8","",HackServerInfo,sizeof(HackServerInfo),path);

	this->m_MemoryAddress8 = (((this->m_MemoryAddress8=strtoul(HackServerInfo,NULL,0))>0x10000000)?(this->m_MemoryAddress8^0xC47A0E9F):this->m_MemoryAddress8);
}
