#ifndef LogHandlerH
#define LogHandlerH

#include <stdlib.h>	// _MAX_PATH, _fullpath

#include "LogManager.h"		// List를 사용하기 위한 참조
#include "LogUnit.h"			// User에게 Log class의 I/F를 제공하기 위한 참조
#include "FDCManager.h"
#include "FDCUnit.h"
extern LogManager *gm_LogManager;

extern FDCManager *gm_FDCManager;


bool startLogManager(unsigned int unSize, bool checkLogFolder);
bool endLogManager();
bool manageLog();
LogUnit*	getLogUnit(char *szSavePath, unsigned int nMaxSizeOfLog);
bool	writeLogUnit(LogUnit* pLogUnit, char *pStr, ...);
FDCUnit*	getFDCUnit(char *szSavePath, unsigned int nMaxSizeOfFDC);



#define WRITE_LOG_UNIT(pLogUnit, ...)				\
{													\
	if(gm_LogManager!=NULL && pLogUnit!=NULL && gm_LogManager->Get_LogFolder())		\
	{												\
		char		szBuf[1024];						\
													\
		memset(&szBuf, 0, sizeof(szBuf));			\
		sprintf(szBuf, __VA_ARGS__);					\
													\
		pLogUnit->put(strlen(szBuf), szBuf);			\
	}												\
}	

#define WRITE_LOG_ON_THIS_FILE(szSavePath, ...)				\
{															\
	LogUnit* pLogUnit = getLogUnit(szSavePath, 1024*10);	\
	if(gm_LogManager!=NULL && pLogUnit!=NULL && gm_LogManager->Get_LogFolder())		\
	{														\
		char		szBuf[1024];								\
															\
		memset(&szBuf, 0, sizeof(szBuf));					\
		sprintf(szBuf, __VA_ARGS__);							\
															\
		pLogUnit->put(strlen(szBuf), szBuf);					\
	}														\
}	

#define CLEAR_LOG(pLogUnit)						\
{													\
	if(gm_LogManager!=NULL && pLogUnit!=NULL)		\
	{												\
		pLogUnit->clear();							\
	}												\
}

/**
@brief   이상감지 Put 함수
@author  MOON
@date    2018.06.25
*/

#define writeFDCUnit(pFDCUnit, ...)				\
{															\
	if(gm_FDCManager!=NULL && pFDCUnit!=NULL && pFDCUnit->IsAuto())		\
	{														\
		char		szBuf[1024];								\
															\
		memset(&szBuf, 0, sizeof(szBuf));					\
		sprintf(szBuf, __VA_ARGS__);							\
															\
		pFDCUnit->FDC_put(strlen(szBuf), szBuf);					\
								\
	}														\
}

#define writeFDCUnit_WARN(pFDCUnit, ...)				\
{															\
	if(gm_FDCManager!=NULL && pFDCUnit!=NULL && pFDCUnit->IsAuto())		\
	{														\
		char		szBuf[1024];								\
															\
		memset(&szBuf, 0, sizeof(szBuf));					\
		sprintf(szBuf, __VA_ARGS__);							\
															\
		pFDCUnit->FDC_put_WARN(strlen(szBuf), szBuf);					\
								\
	}														\
}


/**
@brief   이상감지 AutoMode 변경 함수
@author  MOON
@date    2018.06.25
*/


#define changeFDCAutoMode(pFDCUnit)            \
{                                  \
	pFDCUnit->changeAutoMode();        \
}                                       \

#define changeFDCManualMode(pFDCUnit, errorcode)            \
{                                  \
	pFDCUnit->changeManualMode(errorcode);           \
}

#endif
