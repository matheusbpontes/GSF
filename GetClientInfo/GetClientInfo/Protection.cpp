#include "stdafx.h"
#include "Protection.h"
#include "RESTfulAPI.h"

CProtection gProtection;

CProtection::CProtection()
{
}

CProtection::~CProtection()
{
}

void CProtection::SafeExitProcess()
{
	while (true)
	{
		TerminateProcess(GetCurrentProcess(), 0);
		CRASH_APPLICATION_MACRO
	}
	Sleep(300);
}

bool CProtection::GetAccessToken(char CustomerName[32])
{
	VMProtectBeginUltra("Protection: GetAccessToken");

	string URL;
	URL.resize(_MAX_PATH);
	URL = URL_API;

	URL += "auth";
	if (strlen(CustomerName) < 3)
	{
		return false;
	}
	// GET API
	string DATA = "name=";
	DATA += CustomerName;

	// GET API
	string readBuffer = gRESTfulAPI.Request(GET_METHOD, URL, DATA, false);

	StringStream s(readBuffer.c_str());
	Document json;
	json.ParseStream(s);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	json.Accept(writer);

	if (json.Empty())
	{
		this->SafeExitProcess();
		return false;
	}

	bool success = json["success"].GetBool();
	if (!success)
	{
		cout << json["message"].GetString() << endl;
		return false;
	}

	strcpy_s(this->m_AccessToken, json["accessToken"].GetString()); //AccessToken

	VMProtectEnd();

	return true;
}

bool CProtection::Auth()
{
	VMProtectBeginUltra("Protection: Auth");

	string URL;
	URL.resize(_MAX_PATH);
	URL = URL_API;

	URL += "getip";

	// GET API
	string readBuffer = gRESTfulAPI.Request(GET_METHOD, URL, "", true);

	StringStream s(readBuffer.c_str());
	Document json;
	json.ParseStream(s);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	json.Accept(writer);

	if (json.Empty())
	{
		this->SafeExitProcess();
	}

	bool success = json["success"].GetBool();
	if (!success)
	{
		cout << json["message"].GetString() << endl;
		return false;
	}

	strcpy_s(this->m_IpAddress1, json["ip1"].GetString());
	strcpy_s(this->m_IpAddress2, json["ip2"].GetString());
	strcpy_s(this->m_ExpireAt, json["expireAt"].GetString());

	VMProtectEnd();

	return true;
}