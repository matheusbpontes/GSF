#ifndef _HIDMANAGER_H_
#define _HIDMANAGER_H_
#pragma once

struct HARDWARE_ID_INFO
{
	char HardwareId[36];
	DWORD LastDetectionTime;
};

struct HARDWARE_ID_INFO2
{
	char HardwareId2[36];
	DWORD HardwareIdCount;
};

class CHidManager
{
public:
	CHidManager();
	virtual ~CHidManager();
	bool CheckHardwareId(char HardwareId[36]);
	bool CheckHardwareId2(char HardwareId[36]);
	void InsertHardwareId(char HardwareId[36]);
	void InsertHardwareId2(char HardwareId[36]);
	void ClearHardwareId();
	void ClearHardwareId2(char HardwareId[36]);
	void DeleteAllHardwareId();
//private:
	std::vector<HARDWARE_ID_INFO> m_HardwareIdInfo;
	std::map<std::string, HARDWARE_ID_INFO2> m_HardwareIdInfo2;
};

extern CHidManager gHidManager;
#endif