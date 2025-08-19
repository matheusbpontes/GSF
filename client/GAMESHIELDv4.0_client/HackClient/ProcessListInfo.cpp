#include "stdafx.h"
#include "ProcessListInfo.h"
#include "Util.h"
#include "Connection.h"
#include "CCRC32.h"

CProcessListInfo gProcessListInfo;

//void CProcessListInfo::RecvProcessListReq()
//{
//	this->mData.clear();
//	//std::vector<PROCESS_INFO_DATA> mData;
//
//	//this->GetProcessList();
//	if (!this->GetProcessList())
//	{
//		return;
//	}
//
//	BYTE send[8192];
//
//	//std::vector<BYTE> data;
//
//	PMSG_PROCESS_INFO_DATA_SEND pMsg;
//
//	pMsg.header.set(0x12,0);
//
//	pMsg.count = 0;
//
//	int size = sizeof(pMsg);
//
//	PROCESS_INFO_DATA info;
//
//	char test[10];
//	sprintf(test,"count = %d",mData.size());
//	MessageBox(0,test,test,0);
//
//	for(auto it = mData.begin(); it != mData.end(); ++it)
//	{
//		info.PID = it->PID;
//		memcpy(info.name,it->name,sizeof(info.name));
//		memcpy(info.path,it->path,sizeof(info.path));
//
//		memcpy(&send[size],&info,sizeof(info));
//
//		//send.emplace_back(&info);
//
//
//		/*for(int i=0;i<sizeof(info);i++)
//		{
//			data.emplace_back(((BYTE*)&info)[i]);
//		}*/
//
//		size += sizeof(info);
//
//		pMsg.count++;
//	}
//
//	pMsg.header.size[0] = SET_NUMBERHB(size);
//
//	pMsg.header.size[1] = SET_NUMBERLB(size);
//
//	memcpy(send,&pMsg,sizeof(pMsg));
//	gConnection.DataSend(send,size);
//
//	/*for	(int i=0; i < data.size(); i++)
//	{
//		pMsg.count = 0;
//		pMsg.BufferSize = data.size();
//
//		for (int j=0;j<8192-20;j++)
//		{
//			if (i>=data.size())
//			{
//				break;
//			}
//
//
//
//			send[j] = data[i];
//		}
//	}*/
//}

void CProcessListInfo::RecvProcessListReq()
{
	this->mData.clear();
	//std::vector<PROCESS_INFO_DATA> mData;

	//this->GetProcessList();
	if (!this->GetProcessList())
	{
		return;
	}

	BYTE send[8192];

	PMSG_PROCESS_INFO_DATA_SEND pMsg;

	pMsg.header.set(0x12,0);

	pMsg.count = 0;

	pMsg.isFirst = 1;

	int size = sizeof(pMsg);

	PROCESS_INFO_DATA info;

	/*char test[10];
	sprintf(test,"count = %d",mData.size());
	MessageBox(0,test,test,0);*/

	const int ITEMS_PER_SEND = 25;

	int counter = 0;

	for (int i=0; i < mData.size(); i++)
	{
		/*if (i == mData.size() - 1)
		{
			MessageBoxA(0,"END","END",0);
		}*/

		info.PID = mData[i].PID;
		memcpy(info.name,mData[i].name,sizeof(info.name));
		memcpy(info.path,mData[i].path,sizeof(info.path));

		memcpy(&send[size],&info,sizeof(info));

		size += sizeof(info);

		pMsg.count++;

		counter++;

		if (counter >= ITEMS_PER_SEND || i == mData.size() - 1)
		{
			pMsg.header.size[0] = SET_NUMBERHB(size);

			pMsg.header.size[1] = SET_NUMBERLB(size);

			memcpy(send,&pMsg,sizeof(pMsg));

			while(true)
			{
				if (gConnection.CheckSendSize() > 0)
				{
					Sleep(1);
				}
				else
				{
					break;
				}
			}


			gConnection.DataSend(send,size);

			counter = 0;

			pMsg.count = 0;

			pMsg.isFirst = 0;

			size = sizeof(pMsg);

			memset(send,0,sizeof(send));
		}
	}

	
}

bool CProcessListInfo::GetProcessList()
{
	DWORD ProcessIds[1024],BytesReturned=0,ProcessCount=0;

	if(EnumProcesses(ProcessIds,sizeof(ProcessIds),&BytesReturned) == 0)
	{
		return false;
	}

	//std::ofstream fout(".\\TEST.txt", std::ios::app); //открываем файл для записи в конец
	
//	fout << (int)Index << "\t\"" <<Name << "\"\t"<<"\""<<Text<<"\"" << std::endl;

	//fout << "Total processes: " << (BytesReturned/sizeof(DWORD)) << std::endl;
	

	for(int n=0,ProcessCount=(BytesReturned/sizeof(DWORD));n < ProcessCount;n++)
	{
		if(ProcessIds[n] != 0)
		{
			wchar_t szProcessName[MAX_PATH], sTemp[MAX_PATH];

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcessIds[n]);

			if (hProcess != NULL)
			{
				if(GetProcessImageFileNameW(hProcess,szProcessName,MAX_PATH) != 0)
				{
					char ProcessName[64];
					wsprintf(ProcessName,"%S",ConvertModuleFileName(szProcessName));
					if(ConvertProcessImageFileNameW(szProcessName,sTemp,MAX_PATH) != 0)
					{
						PROCESS_INFO_DATA info;

						info.PID = ProcessIds[n];
						memcpy(info.name,ProcessName,sizeof(info.name));


						//char ProcessDump[256];
						//wsprintf(ProcessDump,"%S",GetDump(szProcessName));
						
						DWORD type = 0;
						DWORD address;
						BYTE dump[32];

						if(FindEntryPointInfo(sTemp,&address,dump) == 0)
						{
							type = 1;

							CCRC32 CRC32;
							address = CRC32.FileCRC(sTemp);

							memset(dump,0,sizeof(dump));
						}

						char temp[400];

						//wcscpy_s(szProcessName,256,ConvertFileName(szProcessName));
						//filename[31] = ((wcslen(filename)>31)?0:filename[31]);

						wsprintf(temp,"%d	\"0x%.8X\"	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	\"%S\"\r\n",
	(DWORD)type, (DWORD)address, (BYTE)dump[0], (BYTE)dump[1], (BYTE)dump[2], (BYTE)dump[3], (BYTE)dump[4], (BYTE)dump[5], (BYTE)dump[6], (BYTE)dump[7], (BYTE)dump[8], (BYTE)dump[9], (BYTE)dump[10],
	(BYTE) dump[11], (BYTE)dump[12], (BYTE)dump[13], (BYTE)dump[14], (BYTE)dump[15], (BYTE)dump[16], (BYTE)dump[17], (BYTE)dump[18], (BYTE)dump[19], (BYTE)dump[20],
	(BYTE) dump[21], (BYTE)dump[22], (BYTE)dump[23], (BYTE)dump[24], (BYTE)dump[25], (BYTE)dump[26], (BYTE)dump[27], (BYTE)dump[28], (BYTE)dump[29], (BYTE)dump[30],
	(BYTE) dump[31], sTemp);
							//sprintf(test,"%s",GetDump(szProcessName));
							//MessageBox(0,"Caution",temp,0);

						//memcpy(info.temp,"HOLA MUNDO",sizeof(info.temp));
						//info.temp = GetDump(szProcessName);
						memcpy(info.path,temp,sizeof(info.path));
						//sprintf(info.path,"%ws",sTemp);

						//fout << "PID: " << ProcessIds[n] << " Name: " << ProcessName << " Path: " << sTemp << std::endl;

						mData.emplace_back(info);
					}
				}
			}

			CloseHandle(hProcess);
		}
	}

	//fout << "The end!" << std::endl;

	//fout.close();
	//MessageBox(0,"Done succesfull",0,0);
	return true;
}

char CProcessListInfo::GetDump(wchar_t* filename)
{
	DWORD type = 0;
	DWORD address;
	BYTE dump[32];

	if(FindEntryPointInfo(filename,&address,dump) == 0)
	{
		type = 1;

		CCRC32 CRC32;
		address = CRC32.FileCRC(filename);

		memset(dump,0,sizeof(dump));
	}

	char temp[256];

	wcscpy_s(filename,256,ConvertFileName(filename));

	filename[31] = ((wcslen(filename)>31)?0:filename[31]);

	wsprintf(temp,"%d	\"0x%.8X\"	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	%.3d	\"%S\"\r\n",
	(DWORD)type, (DWORD)address, (BYTE)dump[0], (BYTE)dump[1], (BYTE)dump[2], (BYTE)dump[3], (BYTE)dump[4], (BYTE)dump[5], (BYTE)dump[6], (BYTE)dump[7], (BYTE)dump[8], (BYTE)dump[9], (BYTE)dump[10],
	(BYTE) dump[11], (BYTE)dump[12], (BYTE)dump[13], (BYTE)dump[14], (BYTE)dump[15], (BYTE)dump[16], (BYTE)dump[17], (BYTE)dump[18], (BYTE)dump[19], (BYTE)dump[20],
	(BYTE) dump[21], (BYTE)dump[22], (BYTE)dump[23], (BYTE)dump[24], (BYTE)dump[25], (BYTE)dump[26], (BYTE)dump[27], (BYTE)dump[28], (BYTE)dump[29], (BYTE)dump[30],
	(BYTE) dump[31], filename);

	return temp[256];
	//WriteFile(temp,TYPE_DUMP);
}

wchar_t* CProcessListInfo::ConvertFileName(wchar_t* path)
{
	wchar_t *filename = wcsrchr(path, L'\\');
	(filename == NULL) ? filename = path : filename++;
	return filename;
}

bool CProcessListInfo::FindEntryPointInfo(wchar_t* filename,DWORD* address,BYTE* dump) // OK
{
	HANDLE file = CreateFileW(filename,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);

	if(file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	BY_HANDLE_FILE_INFORMATION bhfi;

	if(GetFileInformationByHandle(file,&bhfi) == 0)
	{
		CloseHandle(file);
		return 0;
	}

	HANDLE mapping = CreateFileMapping(file,0,PAGE_READONLY,bhfi.nFileSizeHigh,bhfi.nFileSizeLow,0);

	if(mapping == 0)
	{
		CloseHandle(file);
		return 0;
	}

	BYTE* lpBaseAddress = (BYTE*)MapViewOfFile(mapping,FILE_MAP_READ,0,0,bhfi.nFileSizeLow);

	if(lpBaseAddress == 0)
	{
		CloseHandle(mapping);
		CloseHandle(file);
		return 0;
	}

	IMAGE_DOS_HEADER* lpDosHeader = (IMAGE_DOS_HEADER*)lpBaseAddress;

	if(lpDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		UnmapViewOfFile((LPCVOID)lpBaseAddress);
		CloseHandle(mapping);
		CloseHandle(file);
		return 0;
	}

	IMAGE_NT_HEADERS* lpNtHeader = (IMAGE_NT_HEADERS32*)(lpBaseAddress+lpDosHeader->e_lfanew);

	if(lpNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		UnmapViewOfFile((LPCVOID)lpBaseAddress);
		CloseHandle(mapping);
		CloseHandle(file);
		return 0;
	}

	if(lpNtHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)
	{
		UnmapViewOfFile((LPCVOID)lpBaseAddress);
		CloseHandle(mapping);
		CloseHandle(file);
		return 0;
	}

	IMAGE_SECTION_HEADER* lpSectionHeader = ImageRvaToSection(lpNtHeader,(PVOID)lpNtHeader->OptionalHeader.ImageBase,lpNtHeader->OptionalHeader.AddressOfEntryPoint);

	if(lpSectionHeader == 0)
	{
		UnmapViewOfFile((LPCVOID)lpBaseAddress);
		CloseHandle(mapping);
		CloseHandle(file);
		return 0;
	}

	if(SetFilePointer(file,((lpNtHeader->OptionalHeader.AddressOfEntryPoint-lpSectionHeader->VirtualAddress)+lpSectionHeader->PointerToRawData),0,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		UnmapViewOfFile((LPCVOID)lpBaseAddress);
		CloseHandle(mapping);
		CloseHandle(file);
		return 0;
	}

	DWORD NumberOfBytesRead = 0;

	if(ReadFile(file,dump,32,&NumberOfBytesRead,0) == 0)
	{
		UnmapViewOfFile((LPCVOID)lpBaseAddress);
		CloseHandle(mapping);
		CloseHandle(file);
		return 0;
	}

	(*address) = lpNtHeader->OptionalHeader.ImageBase+lpNtHeader->OptionalHeader.AddressOfEntryPoint;

	UnmapViewOfFile((LPCVOID)lpBaseAddress);

	CloseHandle(mapping);

	CloseHandle(file);

	return 1;
}