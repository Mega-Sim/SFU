// ---------------------------------------------------------------------------
/**
@file    AMCDllLibQueue.cpp
$Rev:: 1           $:  Revision of last commit
$Author:: 임태웅   $:  Author of last commit
$Date:: 2013-01-21 0#$:  Date of last commit
@version OHT 7.0
@brief   AMCDllLibQueue Class 소스 File
*/
// ---------------------------------------------------------------------------

//#include "stdafx.h"
#include "LogUnit.h"
//#include "LogManager.h"

#define	INITIALIZE_CRITICAL_SECTION		InitializeCriticalSection(&m_CS)
#define	ENTER_CRITICAL_SECTION			EnterCriticalSection(&m_CS)
#define	LEAVE_CRITICAL_SECTION			LeaveCriticalSection(&m_CS)
#define	DELETE_CRITICAL_SECTION			DeleteCriticalSection(&m_CS)

#define	MIN_LOG_BUFFER_SIZE	1024

#define	ERR_LOG_BUFFER_IS_FULL	930	///@todo: 임의로 정한 에러 번호 - 추후 변경 필요 - 에러 통합 관리, 분산 관리등 논의 필요 

#define	SIZE_OF_INT_PLUS_DWORD	8	// 길이 정보(int:4) + 시간 정보(DWORD:4)

#define	DEFAULT_LOG_NAME	"LOG.log"


//extern LogManager *gm_LogManager;


/**
@brief   LogUnit 초기화
@author  임태웅
@date    2013.01.21
*/
LogUnit::LogUnit(char *szLogName, unsigned int nMaxSizeOfLog)
{

	// 크리티컬 섹션 초기화
	INITIALIZE_CRITICAL_SECTION;

	// TAG 초기화
	m_nTag = 0;

	// 파일 이름 대입

	char szFullSavePath[_MAX_PATH]={0,};
	char* pFullPathResult = NULL;
	pFullPathResult = _fullpath(szFullSavePath, szLogName, _MAX_PATH);
	
	
	int nLen = strlen(szFullSavePath);
	if(nLen!=0 && pFullPathResult)
	{
		m_szLogName = new char[nLen+1];
		memset(m_szLogName, 0, nLen+1);
		strcpy(m_szLogName, szFullSavePath);
//		m_szLogName[nLen]=0;	// 마지막 변수 0(null)로 설정
	}
	else
	{
		nLen = strlen(DEFAULT_LOG_NAME);
		memset(m_szLogName, 0, nLen+1);
		m_szLogName = new char[nLen+1];
		strcpy(m_szLogName, DEFAULT_LOG_NAME);
//		m_szLogName[nLen]=0;	// 마지막 변수 0(null)로 설정
	}

	// Buffer MAX 사이즈 설정
	if(nMaxSizeOfLog>MIN_LOG_BUFFER_SIZE)
	{
		m_nMaxSizeOfLog = nMaxSizeOfLog;
	}
	else
	{
		m_nMaxSizeOfLog = MIN_LOG_BUFFER_SIZE;
	}

	// 손실된 Log의 개수
	m_nCountOfLostLog = 0;
	m_nSize = 0;

//	gm_LogManager->connectLogUnit(this);

}


/**
@brief   LogUnit 소멸자
@author  임태웅
@date    2013.01.21
*/
LogUnit::~LogUnit()
{

	// File Name 소멸
	delete[]	m_szLogName;
	m_szLogName = NULL;

	// List 비우기
	LOG_PACKET *pLogPacket = NULL;
	int nTempCount = 0;

	ENTER_CRITICAL_SECTION;
	nTempCount = m_LList.getCount();	// 현재까지의 Count 를 받아온다.
	for(int n=0; n<nTempCount; n++)
	{
		pLogPacket = (LOG_PACKET*)m_LList.popFront();
		delete [] pLogPacket->pcValue;
		delete pLogPacket;
	}
	LEAVE_CRITICAL_SECTION;

	// 크리티컬 섹션 소멸
	DELETE_CRITICAL_SECTION;
	
}

/**
@brief   User가 Buffer에 Log를 입력할 때 사용
@author  임태웅
@date    2013.01.21
@param   QueuingAmcCmd : 입력되는 명령 정보
@param   nError : 실패 시 반환하는 에러값
@return	 정상 처리되면 true 반환(Buffer에 아무 값이 없어 명령을 보내지 않았다면 이것도 정상처리이다)
*/
bool LogUnit::put(int nLength, char *sLog, int *nError)
{

	bool bReturn = true;	
	int nTempError = 0;
	char *pLog = NULL;
	LOG_PACKET *pLogPacket;

	pLog				= new char[nLength];
	pLogPacket			= new LOG_PACKET();
	pLogPacket->Tag		= LOG_TAG_WRITE;
	memcpy(pLog, sLog, sizeof(char)*nLength);	
	pLogPacket->nLength	= nLength;

    // by zzang9un 2013.03.20 : 시간 값 저장 변경
	//pLogPacket->dwDrivingTime	= timeGetTime();
    GetLocalTime(&pLogPacket->LocalTime);
    
	pLogPacket->pcValue	= pLog;	
	
	ENTER_CRITICAL_SECTION;
	// m_nSize가 Buffer 최대값보다 클 경우, 전부 제거하고 에러 발생: AMC Buffer 용량 초과
	if(m_nSize+nLength+SIZE_OF_INT_PLUS_DWORD>m_nMaxSizeOfLog)
	{
		bReturn = false;
		nTempError = ERR_LOG_BUFFER_IS_FULL;
		m_nCountOfLostLog++;
		delete[] pLog;
		delete pLogPacket;
	}
	else
	{
		m_LList.insertBack((void*)pLogPacket);
		m_nSize+=(nLength+SIZE_OF_INT_PLUS_DWORD);	// 들어온 문자열 + 길이 정보(int:4) + 시간 정보(DWORD:4)
	}
	LEAVE_CRITICAL_SECTION;

	*nError = nTempError;
	return bReturn;
	
}

/**
@brief   User가 Buffer에 Log를 입력할 때 사용
@author  임태웅
@date    2013.01.21
@param   QueuingAmcCmd : 입력되는 명령 정보
@param   nError : 실패 시 반환하는 에러값
@return	 정상 처리되면 true 반환(Buffer에 아무 값이 없어 명령을 보내지 않았다면 이것도 정상처리이다)
*/
bool LogUnit::put(int nLength, char *sLog)
{

	int nError = 0;
	return put(nLength, sLog, &nError);

}


/**
@brief   Log저장하는 파일 초기화
@author  임태웅
@date    2013.02.25
*/
void LogUnit::clear()
{

	char *pLog = NULL;
	LOG_PACKET *pLogPacket;

	pLogPacket = new LOG_PACKET();
	pLogPacket->Tag		= LOG_TAG_FILE_CLEAR;
	pLogPacket->nLength	= 0;

    // by zzang9un 2013.03.20 : 시간 값 저장 변경
	//pLogPacket->dwDrivingTime	= timeGetTime();
    GetLocalTime(&pLogPacket->LocalTime);
    	
	pLogPacket->pcValue	= NULL;	
	
	ENTER_CRITICAL_SECTION;
	m_LList.insertBack((void*)pLogPacket);
	LEAVE_CRITICAL_SECTION;
	
}


/**
@brief   Mgr(Manager)가 Buffer의 Log를 가져갈 때 사용
@author  임태웅
@date    2013.01.21
@return	 정상 처리되면 Tag 반환, 비정상 시 NULL반납
*/
bool LogUnit::setTag(int nTag)
{

	bool bReturn = true;	
	m_nTag = nTag;
	return bReturn;
	
}

/**
@brief   Mgr(Manager)가 Buffer의 Log를 가져갈 때 사용
@author  임태웅
@date    2013.01.21
@return	 정상 처리되면 Tag 반환, 비정상 시 NULL반납
*/
int LogUnit::getTag()
{

	return m_nTag;
	
}

/**
@brief   Mgr(Manager)가 Buffer의 Log를 가져갈 때 사용
@author  임태웅
@date    2013.01.21
@return	 정상 처리되면 파일 이름 반환, 비정상 시 NULL반납
*/
char* LogUnit::getLogName()
{

	return m_szLogName;

}

/**
@brief   Mgr(Manager)가 Buffer의 Log를 가져갈 때 사용
@author  임태웅
@date    2013.01.21
@return	 정상 처리되면 파일 이름 반환, 비정상 시 NULL반납
*/
bool LogUnit::copyLogNameTo(char* szLogName, int nSize)
{
	bool bSuccess = false;
	int nLen = strlen(m_szLogName);

	if((nLen!=0) && ((nLen+1)<nSize))
	{
		memset(szLogName, 0, nLen+1);
		strcpy(szLogName, m_szLogName);
		bSuccess = true;
	}

	return bSuccess;
}

/**
@brief   Mgr(Manager)가 Buffer의 Log를 가져갈 때 사용
@author  임태웅
@date    2013.01.21
@param   pAmcCmd : AMC로 보내는 명령 구조체
@param   nError : 실패 시 반환하는 에러값
@return	 정상 처리되면 true 반환(Buffer에 아무 값이 없어 명령을 보내지 않았다면 이것도 정상처리이다)
*/
bool  LogUnit::get(LList *pList, int*pnCountOfLostLog, int *nError)
{

	// 변수 초기화
	bool bReturn = true;	
	int nTempError = 0;
	int nCmd = 0;
	LOG_PACKET *pLogPacket = NULL;
	int nTempCount = 0;

	// 버퍼의 로그 개수를 받아옮
	ENTER_CRITICAL_SECTION;
	nTempCount = m_LList.getCount();	// 현재까지의 Count 를 받아온다.
	*pnCountOfLostLog = m_nCountOfLostLog;
	m_nCountOfLostLog = 0;
	m_nSize = 0;
	LEAVE_CRITICAL_SECTION;

	// 내부 버퍼의 로그를 Manager의 List로 전달 
	for(int n=0; n<nTempCount; n++)
	{
		ENTER_CRITICAL_SECTION;
		pLogPacket = (LOG_PACKET*)m_LList.popFront();
		LEAVE_CRITICAL_SECTION;
		pList->insertBack((void*)pLogPacket);
	}

	*nError = nTempError;
	return bReturn;
	
}

//---------------------------------------------------------------------------

