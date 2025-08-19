#include "StdAfx.h"
#include "HidManager.h"
#include "ServerInfo.h"
#include "Util.h"

CHidManager gHidManager;

CHidManager::CHidManager()
{
}

CHidManager::~CHidManager()
{
}

bool CHidManager::CheckHardwareId(char HardwareId[36]) // OK
{
	if(gServerInfo.m_DetectionLockTime == 0)
	{
		return 1;
	}

	this->ClearHardwareId();

	for(DWORD i = 0; i < m_HardwareIdInfo.size(); i++)
	{
		if(memcmp(m_HardwareIdInfo[i].HardwareId, HardwareId,sizeof(m_HardwareIdInfo[i].HardwareId)) == 0)
		{
			return 0;
		}
	}

	return 1;
}

bool CHidManager::CheckHardwareId2(char HardwareId[36]) // OK
{
	std::map<std::string, HARDWARE_ID_INFO2>::iterator it = this->m_HardwareIdInfo2.find(std::string(HardwareId));

	if (it == this->m_HardwareIdInfo2.end())
	{
		return ((gServerInfo.m_MaxHardwareIdConnection == 0) ? 1 : 1); // If config is 0 disabled detection replacing 0 : 1 by 1 : 1
	}
	else
	{
		return ((it->second.HardwareIdCount >= gServerInfo.m_MaxHardwareIdConnection) ? 0 : 1);
	}
}

void CHidManager::InsertHardwareId(char HardwareId[36])
{
	HARDWARE_ID_INFO info;

	memcpy(info.HardwareId,HardwareId,sizeof(info.HardwareId));

	info.LastDetectionTime = GetTickCount();

	this->m_HardwareIdInfo.push_back(info);
}

void CHidManager::InsertHardwareId2(char HardwareId[36])
{
	HARDWARE_ID_INFO2 info;

	memcpy(info.HardwareId2, HardwareId, sizeof(info.HardwareId2));

	info.HardwareIdCount = 1;//GetTickCount();

	std::map<std::string, HARDWARE_ID_INFO2>::iterator it = this->m_HardwareIdInfo2.find(std::string(HardwareId));

	if (it == this->m_HardwareIdInfo2.end())
	{
		this->m_HardwareIdInfo2.insert(std::pair<std::string, HARDWARE_ID_INFO2>(std::string(HardwareId), info));
	}
	else
	{
		it->second.HardwareIdCount++;
	}
}

void CHidManager::ClearHardwareId()
{
	for(std::vector<HARDWARE_ID_INFO>::iterator it=this->m_HardwareIdInfo.begin();it != this->m_HardwareIdInfo.end();it++)
	{
		if((GetTickCount()-it->LastDetectionTime) > (DWORD)(gServerInfo.m_DetectionLockTime*1000))
		{
			it = this->m_HardwareIdInfo.erase(it);
			break;
		}
	}
}

void CHidManager::ClearHardwareId2(char HardwareId[36])
{
	std::map<std::string, HARDWARE_ID_INFO2>::iterator it = this->m_HardwareIdInfo2.find(std::string(HardwareId));

	if (it != this->m_HardwareIdInfo2.end())
	{
		if ((--it->second.HardwareIdCount) == 0)
		{
			this->m_HardwareIdInfo2.erase(it);
			LogAdd(LOG_ORANGE, "Cleaning Session");
		}
	}
}

void CHidManager::DeleteAllHardwareId()
{
	int count = this->m_HardwareIdInfo.size();
	this->m_HardwareIdInfo.clear();
	int countnew = this->m_HardwareIdInfo.size();
	LogAdd(LOG_BLACK, "Temporary hardware bans successfully cleared! Total %d HwIds deleted.", count);
}