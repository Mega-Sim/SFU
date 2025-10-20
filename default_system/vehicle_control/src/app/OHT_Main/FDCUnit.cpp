

#include "FDCUnit.h"
//#include "OHTMain.h"

#define	SIZE_OF_FDC				24
#define	MIN_FDC_BUFFER_SIZE		1024
#define	DEFAULT_LOG_NAME		"LOG.log"
#define MAX_FDC_DATA_LENGTH		23                 //<Message 뒤의 Data 갯수를 맞춰줌 23 -> Data 20개

FDCUnit::FDCUnit(char *szFDCName, unsigned int nMaxSizeOfFDC)
{

	char szFullSavePath[_MAX_PATH]={0,};
	char* pFullPathResult = NULL;
	pFullPathResult = _fullpath(szFullSavePath, szFDCName, _MAX_PATH);

	int nLen = strlen(szFullSavePath);
	if(nLen!=0 && pFullPathResult)
	{
		m_szFDCName = new char[nLen+1];
		memset(m_szFDCName, 0, nLen+1);
		strcpy(m_szFDCName, szFullSavePath);
	}
	else
	{
		nLen = strlen(DEFAULT_LOG_NAME);
		memset(m_szFDCName, 0, nLen+1);
		m_szFDCName = new char[nLen+1];
		strcpy(m_szFDCName, DEFAULT_LOG_NAME);
	}

	// Buffer MAX 사이즈 설정
	if(nMaxSizeOfFDC>MIN_FDC_BUFFER_SIZE)
	{
		m_nMaxSizeOfFDC = nMaxSizeOfFDC;
	}
	else
	{
		m_nMaxSizeOfFDC = MIN_FDC_BUFFER_SIZE;
	}

	m_bIsAutoMode = false;

	m_nOCSErrorcode = 0;

	m_nCountOfLostLog = 0;

	m_bUseFDC = false;

//	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
}


FDCUnit::~FDCUnit()
{

	// File Name 소멸
	delete[]	m_szFDCName;
	m_szFDCName = NULL;

	// List 비우기
	FDC_PACKET *pFDCPacket = NULL;
	int nTempCount = 0;

	nTempCount = m_FDC_List.getCount();
	for(int n=0; n<nTempCount; n++)
	{
		pFDCPacket = (FDC_PACKET*)m_FDC_List.popFront();
		delete [] pFDCPacket->pcValue;
		delete pFDCPacket;
	}

}



	//20180607 FDC 추가
/**
@brief   User가 Buffer에 Log를 입력할 때 사용
@author  문현성
@date    2018.06.07
@param   QueuingAmcCmd : 입력되는 명령 정보
@param   nError : 실패 시 반환하는 에러값
@return	 정상 처리되면 true 반환(Buffer에 아무 값이 없어 명령을 보내지 않았다면 이것도 정상처리이다)
*/
bool FDCUnit::FDC_put(int nLength, char *sFDC)
{

	bool bReturn = true;
	char *pFDC = NULL;
	FDC_PACKET *pFDCPacket;
	static int nPreErrorcode = 0;
	static SYSTEMTIME nPreErrorcodeTime;
	int Errorcode = 0;
	bool bIsWARN = false;

	if(CheckTransmitted(&nPreErrorcode,sFDC,&nPreErrorcodeTime, &Errorcode, &bIsWARN))
		return false;

	//여기에서 직접 Extension 하기
	nLength = ExtensionData(nLength,sFDC);

	pFDC				= new char[nLength];
	pFDCPacket			= new FDC_PACKET();
	pFDCPacket->nTransmit_Count = 0;
	memcpy(pFDC, sFDC, sizeof(char)*nLength);
	pFDCPacket->nLength	= nLength;
	pFDCPacket->dwTransmitTime	= 0;
	pFDCPacket->nErrorcode = Errorcode;

	// by zzang9un 2013.03.20 : 시간 값 저장 변경
	//pLogPacket->dwDrivingTime	= timeGetTime();
	GetLocalTime(&pFDCPacket->LocalTime);

	pFDCPacket->pcValue	= pFDC;

	if(bIsWARN == true)
	{
		if(Check_WARN(pFDCPacket))
			m_WARN_List.insertBack((void*)pFDCPacket);
		else
		{
            bReturn = false;

			delete[] pFDC;
			delete pFDCPacket;
        }
	}
	else
	{
		// m_nSize가 Buffer 최대값보다 클 경우, 전부 제거하고 에러 발생: AMC Buffer 용량 초과
		if((m_nSize+nLength+SIZE_OF_FDC>m_nMaxSizeOfFDC) || (pFDCPacket->pcValue == NULL))
		{
			bReturn = false;
			m_nCountOfLostLog++;

			delete[] pFDC;
			delete pFDCPacket;
		}
//		else
//		{
//			m_FDC_List.insertBack((void*)pFDCPacket);
//			m_nSize+=(nLength+SIZE_OF_FDC);	// 들어온 문자열 + 길이 정보(int:4) + 시간 정보(DWORD:4)
//		}
	}
	return bReturn;
}

bool FDCUnit::FDC_put_WARN(int nLength, char *sFDC)
{

	bool bReturn = true;
	char *pFDC = NULL;
	FDC_PACKET *pFDCPacket;
	int Errorcode = 0;

	//여기에서 직접 Extension 하기
	nLength = ExtensionData(nLength,sFDC);

	pFDC				= new char[nLength];
	pFDCPacket			= new FDC_PACKET();
	pFDCPacket->nTransmit_Count = 0;
	memcpy(pFDC, sFDC, sizeof(char)*nLength);
	pFDCPacket->nLength	= nLength;
	pFDCPacket->dwTransmitTime	= 0;
	pFDCPacket->nErrorcode = Errorcode;

	// by zzang9un 2013.03.20 : 시간 값 저장 변경
	//pLogPacket->dwDrivingTime	= timeGetTime();
	GetLocalTime(&pFDCPacket->LocalTime);

	pFDCPacket->pcValue	= pFDC;


	// m_nSize가 Buffer 최대값보다 클 경우, 전부 제거하고 에러 발생: AMC Buffer 용량 초과
	if((m_nSize+nLength+SIZE_OF_FDC>m_nMaxSizeOfFDC) || (pFDCPacket->pcValue == NULL))
	{
		bReturn = false;
		m_nCountOfLostLog++;

		delete[] pFDC;
		delete pFDCPacket;
	}
	else
	{
		m_FDC_List.insertBack((void*)pFDCPacket);
		m_nSize+=(nLength+SIZE_OF_FDC);	// 들어온 문자열 + 길이 정보(int:4) + 시간 정보(DWORD:4)
	}

	return bReturn;
}


bool  FDCUnit::FDC_get(LList *pList, int *nError, int*pnCountOfLostLog)
{

	// 변수 초기화
	bool bReturn = true;
	int nTempError = 0;
	int nCmd = 0;
	FDC_PACKET *pFDCPacket = NULL;
	int nTempCount = 0;
	m_nSize = 0;

	*pnCountOfLostLog = m_nCountOfLostLog;
	m_nCountOfLostLog = 0;

	nTempCount = m_FDC_List.getCount();	// 현재까지의 Count 를 받아온다.
	// 내부 버퍼의 로그를 Manager의 List로 전달
	for(int n=0; n<nTempCount; n++)
	{
        if(m_FDC_List.getCountsize() > 0)
		{
			pFDCPacket = (FDC_PACKET*)m_FDC_List.popFront();
			pList->insertBack((void*)pFDCPacket);
		}
	}

	*nError = nTempError;
	return bReturn;

}


void FDCUnit::Transmitted(FDC_PACKET *pFDCPacket)
{
	pFDCPacket->nTransmit_Count++;
}


void FDCUnit::Transmit_Insert(FDC_PACKET *pFDCPacket)
{
	// m_nSize가 Buffer 최대값보다 클 경우, 전부 제거하고 에러 발생: AMC Buffer 용량 초과
	if((m_nSize+pFDCPacket->nLength+SIZE_OF_FDC>m_nMaxSizeOfFDC) || (pFDCPacket->pcValue == NULL))
	{
		m_nCountOfLostLog++;

		delete[] pFDCPacket->pcValue;
		delete pFDCPacket;
	}
	else
	{
		m_FDC_List.insertBack((void*)pFDCPacket);
		m_nSize+=(pFDCPacket->nLength+SIZE_OF_FDC);	// 들어온 문자열 + 길이 정보(int:4) + 시간 정보(DWORD:4)
	}
}


int FDCUnit::Transmit_Count(FDC_PACKET *pFDCPacket)
{
	return pFDCPacket->nTransmit_Count;
}
/*
void FDCUnit::Transmit_pop(FDC_PACKET *pFDCPacket, unsigned int nIndex)
{
	m_FDC_List.pop(nIndex);
}
*/

int FDCUnit::ExtensionData(int nLength, char *pFDC)
{
	int TempSize;
	int SlashCount = 0;
	int SlashPlusCount;
	char szTemp[256];

	for(TempSize=0 ; TempSize < 256 ; TempSize++)
	{
		if(pFDC[TempSize] == '\0')
			break;
		if(pFDC[TempSize] == '/')
			SlashCount++;
	}

	SlashPlusCount = MAX_FDC_DATA_LENGTH - SlashCount;

	for(TempSize=0 ; TempSize < SlashPlusCount ; TempSize++)
	{
		sprintf(szTemp, "%s/0",pFDC);
		nLength += 2;
		memcpy(pFDC, szTemp, nLength);
	}

	return nLength;
}


void FDCUnit::changeAutoMode()
{
		m_bIsAutoMode = true;
		m_nOCSErrorcode = 0;
	}

void FDCUnit::changeManualMode(int error)
{
	m_bIsAutoMode = false;
	m_nOCSErrorcode = error;
}

bool FDCUnit::IsAuto()
{
	return m_bIsAutoMode;
}


bool  FDCUnit::CheckTransmitted(int *preErrorcode, char *pFDC, SYSTEMTIME *nPreErrorcodeTime, int *Errorcode, bool *bIsWARN)
{
	int TempSize;
	int SlashCount = 0;
	int CharCount = 0;
	char szTemp[4];
	SYSTEMTIME CurrentTime;
	SYSTEMTIME CurrentTime2 = *nPreErrorcodeTime;

	GetLocalTime(&CurrentTime);

	if((CurrentTime.wMinute - CurrentTime2.wMinute) > 1)
	{
		*preErrorcode = 0;
	}

	if((pFDC[0] == 'W') && (pFDC[1] == 'A') && (pFDC[2] == 'R') && (pFDC[3] == 'N'))
	{
		*bIsWARN = true;
	}
	for(TempSize = 0 ; TempSize < 256 ; TempSize++)
	{
		if(pFDC[TempSize] == '/')
			SlashCount++;

		if(SlashCount == 2)
		{
			CharCount = TempSize-1;
			break;
		}
	}

	for(TempSize = 3 ; TempSize > -1 ; TempSize--)
	{
		if((pFDC[CharCount] >= '0') && (pFDC[CharCount] <= '9'))
		{
			szTemp[TempSize] = pFDC[CharCount--];
		}
		else
			szTemp[TempSize] = '0';
	}

	*Errorcode = CharToInt(szTemp);

	if(*bIsWARN == false)
	{
		if(*preErrorcode == CharToInt(szTemp))
		{
			return true;
		}

		*preErrorcode = CharToInt(szTemp);


		GetLocalTime(nPreErrorcodeTime);
	}

	return false;

}

int FDCUnit::getOCSErrorcode()
{
	return m_nOCSErrorcode;
}

void FDCUnit::WARN_Insert()
{
    int 			nCountOfFDC;
	FDC_PACKET 		*tempFDCPacket = NULL;


	nCountOfFDC = m_WARN_List.getCount();

	if(nCountOfFDC > 0)
	{
		for(int nIndex = 0 ; nIndex < nCountOfFDC; nIndex++)
		{
			tempFDCPacket = (FDC_PACKET *)m_WARN_List.popFront();

			// m_nSize가 Buffer 최대값보다 클 경우, 전부 제거하고 에러 발생: AMC Buffer 용량 초과
			if(m_nSize+tempFDCPacket->nLength+SIZE_OF_FDC>m_nMaxSizeOfFDC)
			{
				m_nCountOfLostLog++;
				delete[] tempFDCPacket->pcValue;	//@todo: 문제없는지 확인 필요 delete[]
				delete tempFDCPacket;
			}
			else
			{
				m_FDC_List.insertBack((void*)tempFDCPacket);
				m_nSize+=(tempFDCPacket->nLength+SIZE_OF_FDC);	// 들어온 문자열 + 길이 정보(int:4) + 시간 정보(DWORD:4)
			}
		}
	}
}

bool FDCUnit::Check_WARN(FDC_PACKET *pFDCPacket)
{
	int 			nCountOfFDC;
	FDC_PACKET 		*tempFDCPacket = NULL;

	nCountOfFDC = m_WARN_List.getCount();

	if(nCountOfFDC > 0)
	{
		for(int nIndex = 0 ; nIndex < nCountOfFDC; nIndex++)
		{
			tempFDCPacket = (FDC_PACKET *)m_WARN_List.refer(nIndex);

			if(tempFDCPacket->nErrorcode == pFDCPacket->nErrorcode)
				return false;
		}

	}

	return true;
}
/*
int FDCUnit::getFDCSize()
{
	return m_nSize;
}
*/
/*
int FDCUnit::getFDCListSize()
{
	return m_FDC_List.getCountsize();
}
*/

void FDCUnit::changeUseMode()
{
	m_bUseFDC = true;
}

void FDCUnit::changeNoUseMode()
{
	m_bUseFDC = false;
}

bool FDCUnit::IsUseMode()
{
	return m_bUseFDC;
}
#pragma package(smart_init)
