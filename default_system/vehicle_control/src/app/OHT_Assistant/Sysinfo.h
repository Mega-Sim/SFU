//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/**
@file    SysInfo.h
$Rev:: 467           $:  Revision of last commit
$Author:: puting   $:  Author of last commit
$Date:: 2013-03-27 1#$:  Date of last commit
@version OHT
@brief   SysInfo Class Header File
*/
#ifndef SysinfoH
#define SysinfoH
//---------------------------------------------------------------------------
#include <dir.h>


#include "Define.h"
#include "../Common/Def_DataType.h"

/***********************************************************************************/
// Function Name : Get_IPCSysInfo
// Release Date: 2013.03.28, 박태욱
/***********************************************************************************/

int Get_IsLogDriveAlive(TCHAR *sysinfo);
unsigned long PreventOverflowGetTimeMs(unsigned long precalc);
int Get_OHTDriveType(void);
String Get_FileAccessTime(String);
wchar_t* CharToWChar(const char* pstrSrc);  //char를 wchar로 컨버트
char* WCharToChar(const wchar_t* pwstrSrc); //wchar를 char로  컨버트


#endif
