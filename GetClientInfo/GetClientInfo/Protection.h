#pragma once

#define CRASH_APPLICATION_MACRO _asm _emit 0xFF _asm _emit 0x25 _asm _emit 0x00 _asm _emit 0x00 _asm _emit 0x00 _asm _emit 0x00

class CProtection
{
public:
	CProtection();
	virtual ~CProtection();
	inline void SafeExitProcess();

	bool Auth();
	bool GetAccessToken(char CustomerName[32]);

public:
	char m_AccessToken[1024];
	char m_IpAddress1[32];
	char m_IpAddress2[32];
	char m_ExpireAt[32];
};
extern CProtection gProtection;