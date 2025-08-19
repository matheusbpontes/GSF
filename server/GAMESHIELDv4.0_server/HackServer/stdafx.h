#pragma once

typedef unsigned __int64 QWORD;

#define WIN32_LEAN_AND_MEAN

#define _WIN32_WINNT _WIN32_WINNT_WINXP

#define HACKSERVER_VERSION_DUMMY "PREMIUM"
#define HACKSERVER_VERSION "3.3"	// Esta es la version del lado cliente tiene que tener formato {NUMERO}{PUNTO FINAL}{NUMERO}, el server valida que este numero sea mayor que versiones
									//anteriores para descartar clientes falsos o viejos, super recomendado borrar este comentario al compilar la dll del cliente

#define HACKSERVER_CLIENT "[GameShield v4.0 + Desktop + Memory Protect]" //TITULO DE LA VENTANA Y DEL RECUADRO PRINCIPAL

#define URL_LICENCIA "http://66.com.ar/licencia/licencia.php" //NO SOPORTA HTTPS PARA EVITAR PROBLEMAS POR CERTIFICADOS EN LOS SERVIDORES
#define TESTNAMECRC 1

// Tamaño de ventana
#define ANCHO_VENTANA 800
#define ALTO_VENTANA 600

#define PROTECT_STATE 0

#define USE_SCREENSHOW 0

// System Include
#include <windows.h>
#include <winsock2.h>
#include <process.h>
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <Rpc.h>
#include <dbghelp.h>
#include <Psapi.h>



//#include "detours.h"
#include <Shlwapi.h>
#include <wchar.h>
#include <winternl.h>
#include <winioctl.h>

#include <commctrl.h>


#include <atlimage.h>

//#include "..\\..\\..\\Util\\pugixml\\pugixml.hpp"

#pragma comment(lib, "comctl32.lib")

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"Shlwapi.lib")
//#pragma comment(lib,"detours.lib")
#pragma comment(lib,"Rpcrt4.lib")

#include <Gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#pragma comment(lib,"dbghelp.lib")
//#pragma comment(lib, "libcurl.lib")

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib,"wldap32.lib")
#pragma comment(lib,"Normaliz.lib")