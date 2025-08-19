#define CRC 1
#if(CRC==1)
#include "stdafx.h"
///////////////////////////////////////////////////////////////////

void InitCrcTable();
ULONG Reflect(ULONG ref, char ch);
int Get_CRC(unsigned char* buffer, ULONG bufsize);
long FileSize(FILE* input);
int  GetCheckSumFromMain(const char* file);

char  GetCheckSumFromMain2(const char* file);
#endif