#ifndef LogUnitH
#define LogUnitH

#include <stdio.h>
#include <windows.h>
#include "List.h"
#include "LogManager.h"

typedef enum LOG_TAG_
{
	LOG_TAG_FILE_CLEAR	= 0,
	LOG_TAG_WRITE		= 1
}LOG_TAG;

typedef struct _LOG_PACKET
{
	LOG_TAG	Tag;
	int		nLength;

    // by zzang9un 2013.03.20 : 시간 변수 변경
	//DWORD	dwDrivingTime;
	SYSTEMTIME LocalTime;
    
	char	*pcValue;
}LOG_PACKET;

class LogUnit
{
private:
	int m_nTag; 					///< class의 Tag
	char *m_szLogName;			///< 로그 이름

	CRITICAL_SECTION	m_CS;	///< Manager(Log를 관리하는 class)가 USer(Log를 만드는 class)와의 충돌을 막기위한 Critical Section
	LList m_LList;				///< Log를 저장하는 List

	unsigned int m_nMaxSizeOfLog;		///< Log List에 저장가능한 Char(BYTE)의 총량(Length, Time 정보 포함)
	unsigned int m_nSize;				///< Log List에 저장된 char(BYTE)의 총량(Length, Time 정보 포함)
	unsigned int m_nCountOfLostLog;	///< Log List에 저장된 Char의 총량(Length 정보 포함)

public:
	LogUnit(char *szLogName, unsigned int nMaxSizeOfLog);	///< 생성자(nTag:인식자, sLogName:로그 이름, nMaxSizeOfLog: Log저장 최대 Size~Char개수)
	virtual ~LogUnit(void);		///< 소멸자
	
	bool	put(int nLength, char *sLog, int *nError);	///< Log를 입력하는 함수(nLength: sLog길이, dwDrivingTime:작성시간, sLog: 로그, nError: 에러) / 성공하면 true 리턴
	bool	put(int nLength, char *sLog);				///< Log를 입력하는 함수(nLength: sLog길이, dwDrivingTime:작성시간, sLog: 로그, nError: 에러) / 성공하면 true 리턴
	void	clear();	
	bool	copyLogNameTo(char* szLogName, int nSize);	///< LogUnit의 저장 파일 정보를 받아오기 위한 함수

protected:	
	bool	setTag(int nTag);		///< LogUnit의 Tag 정보를 받아오기 위한 함수
	int		getTag(void);			///< LogUnit의 Tag 정보를 받아오기 위한 함수
	char*	getLogName(void);	///< LogUnit의 저장 파일 정보를 받아오기 위한 함수
	bool	get(LList *pList, int*pnCountOfLostLog, int *nError);	///< Log를 가져가는 함수(pList: Log를 가져가는 List, nError: 에러) / 성공하면 true 리턴

	friend class LogManager;
};

#endif
