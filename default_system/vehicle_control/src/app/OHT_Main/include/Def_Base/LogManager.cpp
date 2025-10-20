// ---------------------------------------------------------------------------
/**
@file    AMCDllLibQueue.cpp
@version OHT 7.0
@brief   AMCDllLibQueue Class 소스 File
*/
// ---------------------------------------------------------------------------




#include "LogManager.h"
#include "Def_OHT.h"

#define	INITIALIZE_CRITICAL_SECTION		InitializeCriticalSection(&m_CS)
#define	ENTER_CRITICAL_SECTION			EnterCriticalSection(&m_CS)
#define	LEAVE_CRITICAL_SECTION			LeaveCriticalSection(&m_CS)
#define	DELETE_CRITICAL_SECTION			DeleteCriticalSection(&m_CS)

#define	MIN_TOTAL_LOG_BUFFER_BYTE_SIZE	4096

#define	ERR_LOG_BUFFER_IS_FULL	930	///@todo: 임의로 정한 에러 번호 - 추후 변경 필요 - 에러 통합 관리, 분산 관리등 논의 필요 


/**
@brief   LogManager 초기화
@author  임태웅
@date    2013.01.21
*/
LogManager::LogManager(unsigned int nMaxByteSizeOfLogSum, bool checkLogFolder)
{
	// 크리티컬 섹션 초기화
	INITIALIZE_CRITICAL_SECTION;
	m_nByteSizeOfLogSum		= 0;
	m_nMaxByteSizeOfLogSum	= 0;
	m_nLostLogCnt = 0;
	m_nLogCnt = 0;
	m_bLogfileCheck = false;
	m_checkLogFolder = checkLogFolder;
	
	m_LogUnitMap.clear();

	// Buffer MAX 사이즈 설정
	if( nMaxByteSizeOfLogSum > MIN_TOTAL_LOG_BUFFER_BYTE_SIZE )
	{
		m_nMaxByteSizeOfLogSum = nMaxByteSizeOfLogSum;
	}
	else
	{
		m_nMaxByteSizeOfLogSum = MIN_TOTAL_LOG_BUFFER_BYTE_SIZE;
	}
	pClientSocket = new TCPCLIENT;
}

/**
@brief   LogManager 소멸자
@author  임태웅
@date    2013.01.21
*/
LogManager::~LogManager()
{
	// 생성된 로그 유닛 소멸
	LogUnit* pLogUnit = NULL;
	int nCountOfLogUnit = m_LList.getCount();
	m_LogUnitMap.clear();

	Sleep(3000);

	for(int n=0; n<nCountOfLogUnit; n++)
	{
		pLogUnit = (LogUnit*)m_LList.popFront();
		delete pLogUnit;
		pLogUnit = NULL;
	}
	delete pClientSocket;

	// 크리티컬 섹션 소멸
	DELETE_CRITICAL_SECTION;
}

/**
@brief   User가 로그 기록하는 class를 생성해서 포인터를 가져가는 함수
@author  임태웅
@date    2013.01.21
@param   pList : Manager 가 로그를 담아가는 구조체
@param   nSize : 담아가는 로그의 총 용량
@param   nError : 실패 시 반환하는 에러값
@return	 정상 처리되면 LogUnit의 포인터 반환, 비정상 시 NULL 시 반환 
*/
bool	LogManager::connectLogUnit(LogUnit *pLogUnit)
{
	bool	bReturn = false;

	
	ENTER_CRITICAL_SECTION;
	if((m_nByteSizeOfLogSum + pLogUnit->m_nMaxSizeOfLog)>m_nMaxByteSizeOfLogSum)	// 로그 용량이 관리 총용량을 초과할 경우 미생성
	{
		bReturn = false;
	}
	else
	{
		bReturn = true;
		m_LList.insertBack((void *) pLogUnit);
		m_nByteSizeOfLogSum+=pLogUnit->m_nMaxSizeOfLog;
	}
	LEAVE_CRITICAL_SECTION;
	
	return bReturn;
}

/**
@brief   각각의 생성된 log 기록 class의 log를 취합해서 기록하는 함수
@author  임태웅
@date    2013.01.21
@param   pList : Manager 가 로그를 담아가는 구조체
@param   nSize : 담아가는 로그의 총 용량
@param   nError : 실패 시 반환하는 에러값
@return	 정상 처리되면 true 반환(Buffer에 아무 값이 없어 로그를 담아가지 않았다면 이것도 정상처리이다)
*/
bool  LogManager::handleLogSum()
{
	int nCountOfLogUnit = 0;
	LogUnit *pLogUnit = NULL;

	// Log Unit의 총 개수를 확인
	ENTER_CRITICAL_SECTION;
	nCountOfLogUnit = m_LList.getCount();
	LEAVE_CRITICAL_SECTION;

	// 각각의 Log Unit 에서 Log를 확인하여 남김
	for(int nIndex=0; nIndex<nCountOfLogUnit; nIndex++)
	{
		pLogUnit = (LogUnit*)m_LList.refer(nIndex);
		
		// Log 기록을 남김

		//bool bRet = handleLogUnitTime(pLogUnit); // Line by Line 기록 방식
		bool bRet = handleLogUnitTimeEx(pLogUnit); // 메모리 버퍼에 쌓아서 한번에 Writing 방식


		//if(bRet)
		//	Sleep(1);	// Context Switching
	}

     return m_bLogfileCheck;
}

LogUnit*	LogManager::getLogUnit(char *szSavePath, unsigned int nMaxSizeOfLog)
{

	char szFullSavePath[_MAX_PATH]={0,};
	char* pFullPathResult = NULL;
	pFullPathResult = _fullpath(szFullSavePath, szSavePath, _MAX_PATH);

	LogUnit* pLogUnit = NULL;
	std::string strSavePath = std::string(szFullSavePath);
	std::map<std::string, LogUnit*>::iterator IterPos;

	if(pFullPathResult!=NULL)
	{
		IterPos = m_LogUnitMap.find(strSavePath);

		if(IterPos == m_LogUnitMap.end())	// 정보 없으면
		{
			pLogUnit = new LogUnit(szFullSavePath, nMaxSizeOfLog);
			connectLogUnit(pLogUnit);
			m_LogUnitMap[strSavePath] = pLogUnit;		//m_LogUnitMap.insert(std::map<std::string, LogUnit*>::value_type(strSavePath, pLogUnit));
		}
		else
		{
			pLogUnit = (*IterPos).second;
		}
	}
	
	return pLogUnit;	
}


// AMC_RECV 로그 방식이 기존10진수에서 91진수로 바뀜에 따라
// 로그 파일 최초 생성할때 상단 부에 로그 버전을 기록해야 나중에 Converting 할때 구별할 수 있다
// 이에 로그 파일이 최초 생성된 amc recv 로그파일인지 확인해 주는 함수
// 시간 loss가 있는 함수는 아니다
// name 값은 "AMC_Recv" 혹은 "AMC_Send_Periodic" 문자열이 오기를 기대
bool LogManager::IsNewLogFile(char *path, char *name)
{
	bool bNewFlag = false;

	try
	{
		bNewFlag = !FileExists(path);

		if(bNewFlag)
		{
			if(name == NULL)
				return false;

			int nNameLen = strlen(name);
			if(nNameLen < 1 || nNameLen > 100)
				return false;

			char szFileName[512];
			memset(szFileName, 0, sizeof(szFileName));

			String str = ExtractFileName(path);
			wcstombs(szFileName, str.c_str() , sizeof(szFileName));

			// 일치하는지 확인
			if(strlen(szFileName) == nNameLen + 14)//  [2019031912.log] <- 14자
			{
				bool bMatch = true;
				for(int i=0; i<nNameLen ; i++)
					bMatch &= (szFileName[i] == name[i]);

				return bMatch;
			}
		}
	}
	catch(...){}

	return false;
}
// SHCHO : 로그 기록 함수 handleLogUnitTime
// Line by Line 기록
// 로그 항목의 시간 기준으로 파일 이름을 생성하기 위함
// 이전에는  현재시간 기준으로 파일을 생성하여 실재 로그 시간과 파일명의 규칙이 깨질 수 있다.
// ex) 1월1일 00:00:00 로그가 12월31일 23:59:58 에 찍히게되는 것 방지.
bool LogManager::handleLogUnitTime(LogUnit* pLogUnit)
{
	int nCountOfLostLog	= 0;
	LList TmpList;
	int nTmpError = 0;	// NO_ERR;

	// 로그 리스트를 얻어온다
	if(!pLogUnit->get(&TmpList, &nCountOfLostLog ,&nTmpError))
		return false;

	if(TmpList.getCount() == 0)
		return false;


	while(TmpList.getCount() > 0)
	{
		LOG_PACKET *pLogPacket = (LOG_PACKET*)TmpList.popFront();

		//날짜 시간을 넣는다.
		char szFullPath[256];
		char szTmpPath[256];
		memset(szFullPath, 0, sizeof(szFullPath));
		memset(szTmpPath, 0, sizeof(szTmpPath));

		sprintf(szTmpPath,"%s", pLogUnit->getLogName());
		int len = strlen(szTmpPath);
		if(len < 6)
			continue;  // exception case

		//확장자 제거
		for(int i=0; i<4; i++)
			szTmpPath[len-i-1]='\0';

		sprintf(szFullPath, "%s%04d%02d%02d%02d.log", szTmpPath, pLogPacket->LocalTime.wYear, pLogPacket->LocalTime.wMonth, pLogPacket->LocalTime.wDay, pLogPacket->LocalTime.wHour);

		// SHCHO 2019.03
		// 현재 로그파일이 새로 생성해야하는 AMC_RECV or AmcPeriodic 로그인지 확인
		// 맞다면 로그 상단에 로그 버전 기록해야함. 진법 변환하는 버전을 명시하기 위함
		bool bShowLogVer = IsNewLogFile(szFullPath, "AMC_Recv");
			bShowLogVer |= IsNewLogFile(szFullPath, "AMC_Send_Periodic");

		FILE *fp = NULL;
		try
		{	fp = fopen(szFullPath, "a");
			if(fp == NULL)
				continue;

			if(bShowLogVer)
				fprintf(fp, "LOG_VERSION = 2.0\n");	// 로그 버전 기록

			// 손실된 로그가 있다면 개수를 기록한다
			if(nCountOfLostLog > 0)
			{
				fprintf(fp, ">>> LOG OVERFLOW <<< %u of %d\n", TmpList.getCount(), nCountOfLostLog);
				nCountOfLostLog = 0;
				//Sleep(0);	// Context Switching
			}

			char szContents[1024];
			memset(szContents, 0, sizeof(szContents));
			memcpy(szContents, pLogPacket->pcValue, pLogPacket->nLength);

			fprintf(fp, "[%02d:%02d:%02d.%03d]%s\n", pLogPacket->LocalTime.wHour, pLogPacket->LocalTime.wMinute, pLogPacket->LocalTime.wSecond, pLogPacket->LocalTime.wMilliseconds, szContents);

			//Sleep(0);	// Context Switching

			// 파일을 닫는다
			fflush(fp);
			//Sleep(0);	// Context Switching
			fclose(fp);
			//Sleep(0);	// Context Switching

		}catch(...)
		{
			if(fp != NULL)
				fclose(fp);
		}

		// 객체 삭제
		delete[] pLogPacket->pcValue;
		delete pLogPacket;
	}

	return true;

}

// SHCHO 2019.03 : 로그 기록 함수 handleLogUnitTimeEx(..)
// 기존 로그 기록 방식은 Line by Line 기록
// 매 Line 마다 파일 Open -> fprintf -> close 반복하여 시간 오버헤드가 매우 큼
// SD카드에 자주 접근하여 성능 저하 유발 가능.
// 로그 기록시간이 길어서 로그 씹히는 경우도 간혹발생됨
//
// 개선된 방식: 메모리 버퍼에 로그를 모아서 한번에 기록
// 스레드 한 사이클 내에서
// 로그 시간대가 바뀌거나, 버퍼크기(8Kb)를 내에서 File Writing
// 버퍼가 꽉찰때 까지 기다리는게 아니고, 한 사이클에 버퍼가 꽉 안차도 무조건 File Writing
bool LogManager::handleLogUnitTimeEx(LogUnit* pLogUnit)
{
	LList TmpList;
	int nTmpError = 0;	// NO_ERR;
	m_nLostLogCnt = 0;
	m_nLogCnt = 0;

	// 로그 리스트를 얻어온다
	if(!pLogUnit->get(&TmpList, &m_nLostLogCnt ,&nTmpError))
		return false;


	if(TmpList.getCount() <= 0 && m_nLostLogCnt == 0)
		return false;

	char dumpBuf[1024*8];    // 8Kb
	memset(dumpBuf, 0, sizeof(dumpBuf));
	int nLogCnt = TmpList.getCount();
	m_nLogCnt = nLogCnt;

	LOG_PACKET *pObj = NULL;  // 기준로그
	int nBufLen = 0;
    // Lost 로그 파일 기록

	try
	{
		// 로그 리스트를 돌면서, 연속하는 타입의 로그는 메모리에 쌓다가
		// 다른 타입의 로그가 나오면 메모리에 쌓인 로그를 파일에 Wrting 한다
		for(int h=0 ; h<nLogCnt ; h++)
		{
			LOG_PACKET *pCur = (LOG_PACKET*)TmpList.refer(h);

			if(pObj == NULL)
				pObj = pCur;

			if(pCur->nLength <= 0)
				continue;

			bool bSizeChk = sizeof(dumpBuf) > (strlen(dumpBuf) + pCur->nLength + 256); // 시간표시까지 합쳐서 넉넉하게 용량계산

			// 이전 로그와 시간대 체크.
			//그리고 버퍼 용량이 넘치지 않으면 메모리에 쌓아놓는다.
			//한번에 파일 쓰기 위함
			if(IsSameLogHour(pObj, pCur) && bSizeChk)
			{
				nBufLen = LogMemCopy(pCur, dumpBuf, nBufLen);
				continue;   // 다음 로그로...
			}

			// 여기 왔다는건 시간대가 바뀌었거나, 버퍼가 꽉 찼을 경우. 파일 기록을 한다
			// 버퍼를 파일에 WRITE 한다.
			LogFileWrite(pLogUnit, pObj, dumpBuf, nBufLen);

			memset(dumpBuf, 0, sizeof(dumpBuf));
			nBufLen = 0;

			// 기준로그 변경
			nBufLen = LogMemCopy(pCur, dumpBuf, nBufLen);
			pObj = pCur;
		}

		// 마지막 dumpBuf Flush
		if(pObj != NULL && nBufLen > 0)
			LogFileWrite(pLogUnit, pObj, dumpBuf, nBufLen);

	}catch(...) { }

	// 객체 삭제
	while(TmpList.getCount() > 0)
	{
		LOG_PACKET *pLogPacket = (LOG_PACKET*)TmpList.popFront();
		delete[] pLogPacket->pcValue;
		delete pLogPacket;
	}

	return true;
}

bool LogManager::Get_LogFolder()
{
	return this->m_checkLogFolder;
}

bool LogManager::IsSameLogHour(void *pvA, void *pvB)
{
	LOG_PACKET *pA = (LOG_PACKET*)pvA;
	LOG_PACKET *pB = (LOG_PACKET*)pvB;

	if(pA == NULL || pB == NULL)
		return false;

	if(pA == pB)	// 같은 객체
		return true;

	bool bSame = true;
	bSame &= pA->LocalTime.wYear 	== pB->LocalTime.wYear;
	bSame &= pA->LocalTime.wMonth  	== pB->LocalTime.wMonth;
	bSame &= pA->LocalTime.wDay 	== pB->LocalTime.wDay;
	bSame &= pA->LocalTime.wHour 	== pB->LocalTime.wHour;

	return bSame;
}

int LogManager::LogMemCopy(void* pvLog, char *logBuf, int nBufSize)
{
	LOG_PACKET* pLog = (LOG_PACKET*)pvLog;

	if(pLog == NULL)
		return 0;

	// 로그 줄의 시간 꾸미기
	char szTime[64];
	memset(szTime, 0, sizeof(szTime));
	sprintf(szTime, "[%02d:%02d:%02d.%03d]", pLog->LocalTime.wHour, pLog->LocalTime.wMinute, pLog->LocalTime.wSecond, pLog->LocalTime.wMilliseconds);
	int nTimeLen = strlen(szTime);

	char lineFeed = '\n';

	// strcpy나 strcat, strlen 은 함수는 성능이 느린 함수이므로 사용을 지양한다
	// 정확한 memcpy를 사용하면 훨신 빠르다

	// 시간 복사
	memcpy(logBuf + nBufSize, szTime, nTimeLen);
	nBufSize += nTimeLen;

	// 로그 내용 복사
	memcpy(logBuf + nBufSize, pLog->pcValue, pLog->nLength);
	nBufSize += pLog->nLength;

	// 줄바꿈 char 복사
	memcpy(logBuf + nBufSize, &lineFeed, 1);
	nBufSize += 1;

	return nBufSize;
}

bool LogManager::LogFileWrite(LogUnit *pLogUnit, void* pvLog, char *buffer, int nBufSize)
{
	LOG_PACKET *pLog = (LOG_PACKET*)pvLog;

	if(pLogUnit == NULL || pLog == NULL || buffer == NULL)
		return false;

	if(nBufSize == 0)
		return true;


	//날짜 시간을 넣는다.
	char szFullPath[256];
	char szTmpPath[256];
	memset(szFullPath, 0, sizeof(szFullPath));
	memset(szTmpPath, 0, sizeof(szTmpPath));

	sprintf(szTmpPath,"%s", pLogUnit->getLogName());

	int len = strlen(szTmpPath);
	if(len < 6)
		return false;  // exception case

	//확장자 제거
	for(int i=0; i<4; i++)
		szTmpPath[len-i-1]='\0';

	sprintf(szFullPath, "%s%04d%02d%02d%02d.log", szTmpPath, pLog->LocalTime.wYear, pLog->LocalTime.wMonth, pLog->LocalTime.wDay, pLog->LocalTime.wHour);

	// SHCHO 2019.03
	// 현재 로그파일이 새로 생성해야하는 AMC_RECV or AmcPeriodic 로그인지 확인
	// 맞다면 로그 상단에 로그 버전 기록해야함. 진법 변환하는 버전을 명시하기 위함
	bool bShowLogVer = IsNewLogFile(szFullPath, "AMC_Recv");
		 bShowLogVer |= IsNewLogFile(szFullPath, "AMC_Send_Periodic");

	bool bShowARLogVer = IsNewLogFile(szFullPath, "AutoRecovery");

	char	cOHTType[10];
	memset(cOHTType, 0, 10);
#if(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
	sprintf(cOHTType,"V80_ROHT");
#elif(OHT_NAME == OHT_NAME_STD_V80)
	sprintf(cOHTType,"V80_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V81)
	sprintf(cOHTType,"V81_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V81S)
	sprintf(cOHTType,"V81S_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V85)
	sprintf(cOHTType,"V85_ROHT");
#elif(OHT_NAME == OHT_NAME_STD_V82)
	sprintf(cOHTType,"V82_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V86)
	sprintf(cOHTType,"V86_ROHT");
#else
	sprintf(cOHTType,"Unknown");
#endif

	FILE *fp = NULL;
	int wbyte = 0;
	try
	{	fp = fopen(szFullPath, "a");
		if(fp == NULL)
		{
			m_bLogfileCheck = false;
			return false;
		}

		if(bShowLogVer)
			fprintf(fp, "LOG_VERSION = 2.0\n");	// LOG 버전 기록
		if(bShowARLogVer)
			fprintf(fp, "Log Ver,%.3f,VHL Ver,%s\n",1.5, cOHTType);	// 1.1 앞의 1은 AutoRecovery 에러 수 증가 시 올림, 소숫점은 AutoRecovery Lof 갱신 시 올림

		// 만약에 로그 씹힌거 있으면 남김
		if(m_nLostLogCnt > 0)
		{
			fprintf(fp, ">>> LOG OVERFLOW <<< %d of %d \n", m_nLostLogCnt, m_nLogCnt);
			m_nLostLogCnt = 0;
		}
		// 여러줄을 한방에 빡! Writing
		wbyte = fwrite(buffer, 1, nBufSize, fp);

		if(wbyte != nBufSize)
			m_bLogfileCheck = false;
		else
			m_bLogfileCheck = true;

		fclose(fp);

	}catch(...)
	{
		if(fp != NULL)
			fclose(fp);
	}

	return (wbyte == nBufSize);

}

//---------------------------------------------------------------------------

