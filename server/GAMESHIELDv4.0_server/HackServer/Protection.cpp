#include "stdafx.h"
#include "Protection.h"
#include "ServerInfo.h"
#include "resource.h"
#include "wininet.h"
#include "Util.h"
//#include <curl/curl.h>
#include "../../../Util/curl/curl.h"
#include "Log.h"
//#include "hosts_parcer.h"

#include <fstream>
//#include <iomanip>
//#include <stdlib.h>
#include <list>
#include <string>

CProtection gProtection;

LRESULT CALLBACK LicenseErrorBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			{
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), gProtection.m_ComputerHardwareId);//strupr(gLicense.m_Data.Hash1));
				return TRUE;
			}

		case WM_COMMAND:
			if ( ( wParam & 0xFFFF & 0xFFFF ) == IDOK || (wParam & 0xFFFF & 0xFFFF ) == IDCANCEL || (wParam & 0xFFFF & 0xFFFF) == IDC_BUTTON1) 
			{
				EndDialog(hDlg, (wParam& 0xFFFF & 0xFFFF ));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

CProtection::CProtection()
{
	srand((DWORD)time(0));

	memset(this->m_ComputerHardwareId,0,sizeof(this->m_ComputerHardwareId));

	this->m_EncKey = rand()%256;

	this->m_LicenseActive = 0;

	this->m_Debt = 0;
}

CProtection::~CProtection()
{
	
}

void CProtection::CheckHosts(char* File)
{
	char path[MAX_PATH];
	GetWindowsDirectoryA(path, MAX_PATH);
	strcat_s(path,File);

	std::fstream F;
	F.open(path);

	std::string str;
	std::list<std::string> Strings;

	if (F)
	{
		while (!F.eof())
		{
			std::getline(F, str);
			Strings.push_back(str);
		}

		F.close();

		for(auto it = Strings.begin(); it != Strings.end(); it++)
		{
			size_t pos = it->find(gProtection.GetEncryptedString(gHostsString1,sizeof(gHostsString1)));
			size_t pos2 = it->find(gProtection.GetEncryptedString(gHostsString2,sizeof(gHostsString2)));
			size_t pos3 = it->find(gProtection.GetEncryptedString(gHostsString3,sizeof(gHostsString3)));
			if (pos != std::string::npos || pos2 != std::string::npos || pos3 != std::string::npos)
			{
				gProtection.SafeExitProcess();
			}
		}
	}
	else
	{
		//gProtection.SafeExitProcess();
	}
}


void CProtection::GetComputerHardwareId()
{
	DWORD VolumeSerialNumber;

	if(GetVolumeInformation(this->GetEncryptedString(gProtectString1,sizeof(gProtectString1)),0,0,&VolumeSerialNumber,0,0,0,0) == 0)
	{
		return;
	}

	UUID uuid;

	UuidCreateSequential(&uuid);

	SYSTEM_INFO SystemInfo;

	GetSystemInfo(&SystemInfo);

	DWORD ComputerHardwareId1 = VolumeSerialNumber ^ 0xF45BC123;;

	DWORD ComputerHardwareId2 = *(DWORD*)(&uuid.Data4[2]) ^ 0x13B686FE;

	DWORD ComputerHardwareId3 = ((*(WORD*)(&uuid.Data4[6]) & 0xFFFF) | (SystemInfo.wProcessorArchitecture << 16)) ^ 0x6D19A614;

	DWORD ComputerHardwareId4 = ((SystemInfo.wProcessorLevel & 0xFFFF) | (SystemInfo.wProcessorRevision << 16)) ^ 0xA921E11D;

	wsprintf(this->m_ComputerHardwareId,this->GetEncryptedString(gProtectString2,sizeof(gProtectString2)),ComputerHardwareId1,ComputerHardwareId2,ComputerHardwareId3,ComputerHardwareId4);
}

size_t CProtection::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void code(char* lpName, char* lpPassword, char* lpResult)
{
    unsigned int a = 0, b = 0;
    a = 0;
	
    // пока существует строка lpResult
    while (lpResult[a])
    {
        b = 0;
	
	// пока существует строка lpPassword
        while (lpPassword[b]) 
        {
	    // хэшируем простым XOR'ом	
            lpResult[a] ^= (lpPassword[b] + (a*b)); 
	    // идем к следующему символу lpPassword
            b++; 
        }
	// идем к следующему символу lpResult
        a++; 
    }
}

void CProtection::Auth(HINSTANCE hIns, HWND hWnd)
{
	this->CheckHosts(gProtection.GetEncryptedString(gHostsFile1,sizeof(gHostsFile1)));
	this->CheckHosts(gProtection.GetEncryptedString(gHostsFile2,sizeof(gHostsFile2)));
	if (this->Auth(hIns,hWnd,0) == false)
	{
		if (this->Auth(hInst,hWnd,1) == false)
		{
			if (this->Auth(hInst,hWnd,2) == false)
			{
				this->InfoBox(this->GetEncryptedString(gProtectString8, sizeof(gProtectString8))); // Выводим сообщение о том что нет подключения к серверу
				this->SafeExitProcess(); // Закрываем процесс.
			}
		}
	}
}

bool CProtection::Auth(HINSTANCE hIns, HWND hWnd, int type)
{
	this->hInst = hIns;
	this->ghWnd = hWnd;
	// ----
	this->GetComputerHardwareId(); // Получаем HWID.
	// ----
	CURL *curl;
	CURLcode res1;

	std::string URL;
	URL.resize(_MAX_PATH);

	if (type == 0)
	{
		URL = this->GetEncryptedString(gProtectString3, sizeof(gProtectString3));
	}
	else if (type == 1)
	{
		URL = this->GetEncryptedString(gProtectString9, sizeof(gProtectString9));
	}
	else if (type == 2)
	{
		URL = this->GetEncryptedString(gProtectString10, sizeof(gProtectString10));
	}

	URL += this->m_ComputerHardwareId;

	std::string readBuffer;

	// Открываем и читаем в буффер XML файл.
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res1 = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	// Загружаем файл из буффера в pugixml.
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load(readBuffer.c_str());

	if(res.status != pugi::status_ok)
	{
		return false;
	}

	if (file.child(this->GetEncryptedString(gProtectAttr1, sizeof(gProtectAttr1))).empty() != 0) // Если файл не содержит главный узел, значит файл пуст.
	{
		//if( !strcmp(this->GetEncryptedString(gRootHWID,sizeof(gRootHWID)), this->m_ComputerHardwareId) ) //
		//{
		//	for (int i=0;i<CUSTOMS_COUNT;i++)
		//	{
		//		this->SetCustomState(i, 1); // Устанавливаем статус кастому.
		//	}
		//	return true;
		//}

		//this->InfoBox(this->GetEncryptedString(gProtectString8, sizeof(gProtectString8))); // Выводим сообщение о том что нет подключения к серверу
		//this->SafeExitProcess(); // Закрываем процесс.
		return false;
	}
	
	pugi::xml_node LicenseInfo = file.child(this->GetEncryptedString(gProtectAttr1, sizeof(gProtectAttr1))); // Открываем главный узел.
	pugi::xml_node Node;
	Node = LicenseInfo.child(this->GetEncryptedString(gProtectAttr2, sizeof(gProtectAttr2))); // Открываем узел с основной информацией о лицензиях.
	int NotFound = Node.attribute(this->GetEncryptedString(gProtectAttr3, sizeof(gProtectAttr3))).as_int(); // Если в узле есть аттрибут "NotFound", значит лицензия не найдена.

	if (NotFound == 1) // Если NotFound найден, то он всегда равен 1.
	{
		DialogBoxA(this->hInst, (LPCTSTR)IDD_LICENSE_BOX, this->ghWnd, (DLGPROC)LicenseErrorBox); // Открываем диалог с HWID-ом, и информацией что лицензия не найдена.
		this->SafeExitProcess(); // Закрываем процесс
	}

	this->m_Index = Node.attribute(this->GetEncryptedString(gProtectAttr10, sizeof(gProtectAttr10))).as_int(); // Читаем индекс пользователя.

	strcpy_s(this->CustomerName,Node.attribute(this->GetEncryptedString(gProtectAttr4, sizeof(gProtectAttr4))).as_string()); // Читаем имя пользователя из главного узла.

	this->m_LicenseActive = Node.attribute(this->GetEncryptedString(gProtectAttr5, sizeof(gProtectAttr5))).as_int(); // Читаем активен ли пользователь.

	if (this->m_LicenseActive != 1) // Если нет, шлем нахуй.
	{
		this->InfoBox(this->GetEncryptedString(gProtectString5, sizeof(gProtectString5)),this->m_Debt); // Отправляем сам посыл.
		this->SafeExitProcess(); // Завершаем процесс.
	}

	char LicenseCode[36]; 
	strcpy_s(LicenseCode,Node.attribute(this->GetEncryptedString(gProtectAttr6, sizeof(gProtectAttr6))).as_string()); // Получаем HWID из узла

	if( strcmp(LicenseCode, this->m_ComputerHardwareId) ) // Сравниваем HWID полученный из узла, с HWID-ом полученным в GS-e. Не знаю зачем это сделал, но лишним не будет.
	{
		this->SafeExitProcess(); // Завершаем процесс.
	}

	this->m_Debt = Node.attribute(this->GetEncryptedString(gProtectAttr7, sizeof(gProtectAttr7))).as_int(); // Читаем инфофрмацию о долге.

	if (this->m_Debt > 0) // Если долг больше 0, то напоминаем пользователю вернуть долг.
	{
		this->InfoBox(this->GetEncryptedString(gProtectString6, sizeof(gProtectString6)),this->m_Debt); // Само напоминание.
	}

	return true;
}



void CProtection::ErrorMessageBox1()
{
	memset(this->m_ComputerHardwareId,0,sizeof(this->m_ComputerHardwareId));

	//MessageBox(0,"Authorization failed.\n\nVisit: ltp-team.ru","Error",MB_ICONSTOP|MB_OK);

	this->SafeExitProcess();
}

void CProtection::ConnectionErrorBox()
{
	memset(this->m_ComputerHardwareId,0,sizeof(this->m_ComputerHardwareId));

	MessageBox(0,this->GetEncryptedString(gProtectString4, sizeof(gProtectString4)),"Error",MB_ICONSTOP|MB_OK);

	this->SafeExitProcess();
}

void CProtection::InfoBox(char* message,...)
{
	char buff[256];

	memset(buff,0,sizeof(buff));

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	MessageBox(0,buff,"Info",MB_OK | MB_ICONWARNING);
}

void CProtection::SafeExitProcess()
{
	while(true)
	{
		TerminateProcess(GetCurrentProcess(),0);
		CRASH_APPLICATION_MACRO
	}
}

char* CProtection::GetEncryptedString(BYTE* string, int size)
{
	static char buff[256] = {0};

	memset(buff,0,sizeof(buff));

	for(int n=0;n < size;n++)
	{
		buff[n] = string[n]^0xB0;
	}

	return buff;
}