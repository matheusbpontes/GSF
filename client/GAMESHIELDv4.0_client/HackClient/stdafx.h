#pragma once

typedef unsigned __int64 QWORD;

#define WIN32_LEAN_AND_MEAN

#define _WIN32_WINNT _WIN32_WINNT_WINXP

#define HACKCLIENT_VERSION "3.3" // Esta es la version del lado cliente tiene que tener formato {NUMERO}{PUNTO FINAL}{NUMERO}, el server valida que este numero sea mayor que versiones
								 //anteriores para descartar clientes falsos o viejos, super recomendado borrar este comentario al compilar la dll del cliente

#define PSAPI_VERSION 1
#define TESTNAMECRC 1
#define USE_SCREENSHOW 1

#define ENCRYPT_LOG 1

// System Include
#include <windows.h>
#include <iostream>
#include <map>
#include <math.h>
#include <stdlib.h>
#include <winsock2.h>
#include <vector>
#include <Psapi.h>
#include "detours.h"
#include <Shlwapi.h>
#include <Dbghelp.h>
#include <wchar.h>
#include <Rpc.h>
#include <winternl.h>
#include <winioctl.h>
#include <fstream>
//NewProcess
#include <tlhelp32.h>
#include <atlimage.h>

#include <commctrl.h>


//vm
#define NewProtocolS6Start 1
#include "START.h"
#include "Classe.h"
#include <fstream>
//#include "CRC.h"
#include <tchar.h>    //to php POST
// NUEVO MELO920 XD
#include "time.h"
 #pragma warning (disable: 4996 4091 4101 4018 4309)
 using namespace std;
extern  "C"  __declspec(dllexport) void __cdecl GasMask_5(); //API (init da dll)


#pragma comment(lib, "comctl32.lib")

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"detours.lib")
#pragma comment(lib,"Rpcrt4.lib")
