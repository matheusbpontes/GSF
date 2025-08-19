#include "StdAfx.h"

char* RemoveChar(char* Input, char Character) //OK
{
	for(int i=0; i<=strlen(Input); i++)
	{
		if(Input[i] == Character)
		{
			for(int n=i; n<=strlen(Input); n++)
			{
				Input[n] = Input[n+1];
			}
		}
	}
	return Input;
}

void GetHID()
{
	HW_PROFILE_INFO hwProfileInfo;
	DWORD HDSerial;
	char HardID[50];
	TCHAR DriverLetterBuf[30];
	char DriverName[5];

    GetSystemWindowsDirectory(DriverLetterBuf,30);
	sprintf(DriverName,"%c://",DriverLetterBuf[0]);
	GetVolumeInformationA(DriverName, NULL, 12, &HDSerial, NULL, NULL, NULL, 10);


	UUID uuid;

	UuidCreateSequential(&uuid);

	SYSTEM_INFO SystemInfo;

	GetSystemInfo(&SystemInfo);

	DWORD ComputerHardwareId1 = HDSerial ^ 0x12B586FE;

	DWORD ComputerHardwareId2 = *(DWORD *)(&uuid.Data4[2]) ^ 0x5D78A569;

	DWORD ComputerHardwareId3 = ((*(WORD *)(&uuid.Data4[6]) & 0xFFFF) | (SystemInfo.wProcessorArchitecture << 16)) ^ 0xF45BC123;

	DWORD ComputerHardwareId4 = ((SystemInfo.wProcessorLevel & 0xFFFF) | (SystemInfo.wProcessorRevision << 16)) ^ 0xB542D8E1;

	sprintf(HardID, "%08X-%08X-%08X-%08X", ComputerHardwareId1, ComputerHardwareId2, ComputerHardwareId3, ComputerHardwareId4);

	FILE * pFile;

	fopen_s(&pFile,".\\HardwareId.txt","w");

	if (pFile!=NULL)
	{
		fputs (HardID,pFile);
		fclose (pFile);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{	
	GetHID();
	return 0;
}

