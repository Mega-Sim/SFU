#ifndef LogManagerH
#define LogManagerH

#include <stdio.h>
#include <windows.h>
#include <map>
#include <string>

#include "List.h"			// List를 사용하기 위한 참조
#include "LogUnit.h"		// User에게 Log class의 I/F를 제공하기 위한 참조
#include "TCPClient.h"
#include "LogTLV.h"

#define MAX_BYTE_SIZE_OF_LOG    1024*1024
#define MAX_BYTE_SIZE_OF_MAIN_LOG   1024*10     //10->20 증가 AOHC-287 Log 사이즈 증가 -> 원복

class LogUnit;

class LogManager
{
private:
	CRITICAL_SECTION	m_CS;						///< LogManager가 LogUnit을 처리할 때와 LogUnit class를 연결하는 부분과의 충돌을 막기위한 Critical Section
	LList 				m_LList;						///< Log를 저장하는 List
	unsigned int		m_nMaxByteSizeOfLogSum;		///< LogFile의 최대 용량
	unsigned int		m_nByteSizeOfLogSum;			///< LogFile의 현재 용량
	bool 				m_checkLogFolder;

	std::map<std::string, LogUnit*> m_LogUnitMap;	///< 저장경로로 LogUnit을 찾기위한 map

public:
	TCPCLIENT*	pClientSocket;


public:
	LogManager(unsigned int nMaxByteSizeOfLogSum, bool checkLogFolder);	///< 생성자(nTag:인식자, sLogName:로그 이름, nMaxByteSizeOfLogSum: Log저장 최대 Size~Byte)
	virtual ~LogManager(void);

	// 1. 로그 기록하는 class를 생성해서 포인터를 전달하는 함수
	bool	connectLogUnit(LogUnit *pLogUnit);	///< 1. 로그 기록하는 class를 생성해서 포인터를 전달하는 함수

	// 2. 생성된 log 기록 class의 log를 취합해서 기록하는 함수
	bool 	handleLogSum(void);	///< 2. 생성된 log 기록 class의 log를 취합해서 기록하는 함수

	// 3. Log Unit을 생성하거나, 이미 존재하는 객체의 포인터를 받아오는 함수
	LogUnit*	getLogUnit(char *szSavePath, unsigned int nMaxSizeOfLog);

	bool Get_LogFolder();	
	bool IsNewLogFile(char *path, char *name);
	bool m_bLogfileCheck;
private:
		int LogMemCopy(void* pLog, char *logBuf, int nBufSize);
		bool LogFileWrite(LogUnit *pLogUnit, void* pLog, char *buffer, int nBufSize);
		bool IsSameLogHour(void *pA, void *pB);

		int m_nLostLogCnt;
		int m_nLogCnt;

protected:



	virtual bool handleLogUnitTimeEx(LogUnit* pLogUnit);
	virtual bool handleLogUnitTime(LogUnit* pLogUnit);		///< 로그를 시간대별로 남김


	friend class LogUnit;

};



#endif
