// IpManager.h: interface for the CIpManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

//struct IP_ADDRESS_INFO
//{
//	char IpAddress[16];
//	WORD IpAddressCount;
//};

struct IP_ADDRESS_INFO
{
	char IpAddress[16];
	WORD IpAddressCount;
	WORD IpFloodAttemps;
	WORD IpBlocked;
	DWORD IpBlockedTime;
	DWORD IpTime;
	DWORD IpTime2;
	DWORD IpLastTime;
};

class CIpManager
{
public:
	CIpManager();
	virtual ~CIpManager();
	bool CheckIpAddress(char* IpAddress, int Enable);
	void InsertIpAddress(char* IpAddress);
	void RemoveIpAddress(char* IpAddress);
	int GetIpCount(char* IpAddress);
private:
	std::map<std::string,IP_ADDRESS_INFO> m_IpAddressInfo;
};

extern CIpManager gIpManager;
