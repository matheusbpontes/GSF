#pragma once

#define ANTI_CRACK 1
//#define URL_API "http://localhost:7000/v1/"
#define URL_API "https://api.muguard.net/v1/"
// System Include
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <winternl.h>
#include <winioctl.h>
#include "wininet.h"
using namespace std;

#include "..\\..\\Util\\VMProtect\\VMProtectSDK.h"

#include "..\\..\\Util\\rapidjson\\document.h"
#include "..\\..\\Util\\rapidjson\\writer.h"
#include "..\\..\\Util\\rapidjson\\stringbuffer.h"
using namespace rapidjson;

#define CURL_STATICLIB
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Normaliz.lib")

#ifdef _DEBUG
#pragma comment(lib, "..\\..\\Util\\curl\\libcurl_a_debug.lib")
#else
#pragma comment(lib, "..\\..\\Util\\curl\\libcurl_a.lib")
#endif