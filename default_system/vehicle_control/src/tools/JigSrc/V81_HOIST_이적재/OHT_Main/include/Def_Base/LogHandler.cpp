// ---------------------------------------------------------------------------
/**
@file    LogHandler.cpp
@version OHT 7.0
@brief   LogHandler Class 소스 File
*/
// ---------------------------------------------------------------------------

//#include "stdafx.h"
//#include <stdlib.h>
//#include <exception>
//#include <windows.h>

#include <iostream>
#include <map>
#include <string>
#include "LogHandler.h"

using namespace std;

//---------------------------------------------------------------------------
LogManager *gm_LogManager = NULL;	///< 로그 매니저
FDCManager *gm_FDCManager = NULL;   ///< 이상감지 매니저
//---------------------------------------------------------------------------

/**
@brief   Log Manager 시작
@author  임태웅
@date    2013.04.18
*/
bool startLogManager(unsigned int unSize, bool checkLogFolder)
{
	if(gm_LogManager==NULL)
	{
		gm_LogManager = new LogManager(unSize, checkLogFolder);
		gm_FDCManager = new FDCManager(unSize, checkLogFolder);
		return true;
	}
	else
	{
		return false;
	}
};
//---------------------------------------------------------------------------

/**
@brief   Log Manager 끝
@author  임태웅
@date    2013.04.18
*/
bool endLogManager()
{
	if(gm_LogManager!=NULL)
	{
		delete gm_LogManager;
		gm_LogManager = NULL;

		delete gm_FDCManager;

		return true;
	}
	else
	{
		return false;
	}
};

//---------------------------------------------------------------------------

/**
@brief   Log 처리
@author  임태웅
@date    2013.04.18
*/
bool manageLog()
{
	if(gm_LogManager!=NULL)
	{
		gm_LogManager->handleLogSum();
		gm_FDCManager->handleFDCSum();
		return true;
	}
	else
	{
		return false;
	}
};

//---------------------------------------------------------------------------

/**
@brief   Log 처리
@author  임태웅
@date    2013.04.18
*/
LogUnit*	getLogUnit(char *szSavePath, unsigned int nMaxSizeOfLog)
{
	if(gm_LogManager!=NULL)
	{
		return gm_LogManager->getLogUnit(szSavePath, nMaxSizeOfLog);
	}
	else
	{
		return NULL;
	}
}

//---------------------------------------------------------------------------

/**
@brief   Log 작성
@author  임태웅
@date    2013.04.18
*/
bool	writeLogUnit(LogUnit* pLogUnit, char *pStr, ...)
{
	if(gm_LogManager==NULL)
	{
		return false;
	}

    va_list     argp;
    char        szBuf[1024];
	
	memset(&szBuf, 0, sizeof(szBuf));
 
    va_start(argp, pStr);
    sprintf(szBuf, pStr, argp);
    va_end(argp);
	
	return pLogUnit->put(strlen(szBuf), szBuf);

}

//---------------------------------------------------------------------------


/**
@brief   이상감지 처리
@author  문현성
@date    2018.06.25
*/

FDCUnit*	getFDCUnit(char *szSavePath, unsigned int nMaxSizeOfLog)
{
	if(gm_FDCManager!=NULL)
	{
		return gm_FDCManager->getFDCUnit(szSavePath, nMaxSizeOfLog);
	}
	else
	{
		return NULL;
	}
}
