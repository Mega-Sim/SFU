/**
@file    BCR.cpp
@version OHT 7.0
@brief   BCR Class CPP File
*/

#include<windows.h>
#include <stdlib.h>
#include <math.h>

#include "BCR.h"
#include "Utility.h"
#include "ErrorCode.h"

#define TRANS          0 ///< Trans BCR
#define DRIVING        1 ///< Driving BCR

#define	CR					0x0D
#define	LF					0x0A
#define	KEIENCE_BCR_TRIGGER_ON		"LON"
#define	KEIENCE_BCR_TRIGGER_OFF		"LOFF"
#define KEIENCE_BCR_ERROR			"ERROR"

#define	LEUZE_BCR_TRIGGER_ON		"+"
#define	LEUZE_BCR_TRIGGER_OFF		"-"
#define LEUZE_BCR_ERROR				"?"

#define STX					0x02
#define ETX					0x03



#define BCR_BAUDRATE    38400

//#define BCRRAW_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\BCRRawData.log", __VA_ARGS__)
//수정
#define BCRRAW_LOG(...)         WRITE_LOG_UNIT(pLogBCRRaw, __VA_ARGS__)

/**
@brief   BCR Setting 을 위한 구조체

    추후 생성자에서 받아서 처리 예정
@author  임태웅
@date    2012.12.06
*/
//typedef struct _BCR_SETTING_
//{
//    int nPortNum;
//    unsigned int nIndexOfReadOK;
//    unsigned int nIndexOfReadNG;
//} BCR_SETTING;



/**
@brief   BCR Class 생성자
@author  LSJ
@date    2012.11.04
@param   pHWCommon : HWCommon Class의 포인터
*/
BCR::BCR(HWCommon * pHWCommon, int nIndex)
{

	pLogBCRRaw = getLogUnit("D:\\log\\BCRRawData.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_nLatestID = 0;
	m_pnResultID = NULL;
	m_pHWCommon = pHWCommon;

	if (nIndex == TRANS)
	{
		m_nPortNum = COM_TRANS_BCR;    // 6
		m_nIndex = TRANS;
	}
	else
	{
		m_nPortNum = COM_DRIVING_BCR; 	// 5
		m_nIndex = DRIVING;
    }

    m_pCommPort = NULL;
    
    Enable();

	m_nMovingCount = 0;
	m_nTriggerCount = 0;
	m_nRetryCount = 0;

	InitializeCriticalSection(&m_MovCntCS);

	nPreID = 0;
	ReadCheck=false;
	bUseLeuze = false;
	strErr = KEIENCE_BCR_ERROR;
}

/**
@brief   BCR Class 소멸자
@author  LSJ
@date    2012.11.04
*/
BCR::~BCR()
{
	Disable();
	DeleteCriticalSection(&m_MovCntCS);
}

/**
@brief   BCR 입력 중 리딩에 문제가 없을 때 들어오는 OK 신호를 확인하는 함수
@author  LSJ
@date    2012.11.13
@return  OK 신호가 On이면 true, OFF 면 false.

bool BCR::IsBCROK()
{
    bool bResult = false;

    switch (m_nIndex)
    {
    case DRIVING:
        if (m_pHWCommon->m_Input.BCR_Driving_OK == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case TRANS:
        if (m_pHWCommon->m_Input.BCR_Trans_OK == ON)
            bResult = true;
        else
            bResult = false;
        break;
    }

    return bResult;
}
*/

/**
@brief   BCR 입력 중 리딩에 문제가 있을 때 들어오는 NG 신호를 확인하는 함수
@author  LSJ
@date    2012.11.13
@return  NG 신호가 On이면 true, OFF 면 false.

bool BCR::IsBCRNG()
{
    bool bResult = false;

    switch (m_nIndex)
    {
    case DRIVING:
        if (m_pHWCommon->m_Input.BCR_Driving_NG == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case TRANS:
        if (m_pHWCommon->m_Input.BCR_Trans_NG == ON)
			bResult = true;
        else
            bResult = false;
        break;
    }

    return bResult;
}
*/

/**
@brief   BCR Read 명령을 주고 그에 대한 응답을 받는 함수
@author  LSJ
@date    2012.11.04
@param   nID : 읽은 결과 값을 저장하는 변수, 실패 시 0 대입 
@return  BCR Read하는데 문제 없으면 0, 문제 발생 시 해당 에러 코드 리턴
*/
int BCR::GetID(int *pnID)
{
	DWORD dwLen = 0;
	char szReadBuff[1024] = {0,};
	char szResult[1024] = {0,};
	int nTmpError = NO_ERR;
	int nID = 0;

	m_nLatestID = 0;
	if(m_pnResultID!=NULL)
	{
		*m_pnResultID = 0;
	}
	*pnID = 0;

	if(m_pCommPort==NULL)
	{
		nTmpError = ReStart();
		IncreaseRetryCount();
	}
	
	if(m_pCommPort==NULL)
	{
		return ERR_BCR_OPEN;
	}

	dwLen = m_pCommPort->ReadUntil(szReadBuff, LF);

	if(szReadBuff[0]!=0x02||szReadBuff[dwLen-2]!='\r'||szReadBuff[dwLen-1]!='\n')
	{
		if(dwLen!=0)
			BCRRAW_LOG("BCR NG :%s, Len:%d", szReadBuff,dwLen);

		nTmpError=ERR_BCR_DATAFORMAT;
		nID=0;
	}
	else if(strstr(szReadBuff, strErr) == NULL)
	{
		// 프로토콜에 의한 값 제거
		szReadBuff[dwLen-2]=0;
		szReadBuff[dwLen-1]=0;
		//ID 마지막 자리수 CheckSum 추후에 추가 필요.
#if(OHT_NAME != OHT_NAME_P4_MASK)
		szReadBuff[dwLen-3] = 0;
#endif
		strcpy(szResult, &szReadBuff[1]);

		try
		{
			nID = StrToIntDef(szResult, 0);	// hex 처리

			if(nPreID !=  nID)
				BCRRAW_LOG("BCR OK %s:%d,Len:%d",szReadBuff, nID,dwLen);

			nPreID = nID;

		}
		catch (...)
		{
         	nID = 0;
		}

	   //	nID = atoi(szResult);	// int 처리

		nTmpError = NO_ERR;
	}
#if(OHT_NAME != OHT_NAME_P4_MASK)
	else if(dwLen != 10)
	{
		BCRRAW_LOG("BCR Len NG :%s, Len:%d", szReadBuff,dwLen);
		nTmpError=ERR_BCR_DATAFORMAT;
		nID=0;
	}
#endif

	if(nTmpError == NO_ERR)
	{
		if(strstr(szReadBuff, strErr) != NULL)
		{
		
			BCRRAW_LOG("BCR ERROR");
			nTmpError = ERR_BCR_READ;                 //Read Fail
			nID=0;
		}
	}
	m_nLatestID = nID;
	if(m_pnResultID!=NULL)
	{
		*m_pnResultID = nID;
	}
	*pnID = nID;

	//진단모니터링 항목(BCR 동작 횟수)
	m_nMovingCount++;

	return  nTmpError;
}


/** AOHC-249 BCR Read Sequence 개선
@brief   BCR Read 명령을 주고 그에 대한 응답 Check 및 Data 획득
@author  Jrespect.im
@date    2019.09.09
@param   nID : 읽은 결과 값을 저장하는 변수, 실패 시 0 대입
@Param   bCheck : Data를 읽혔는지 유무를 저장하는 변수 (Read Data 있는 경우 true, Read Data 0 경우 False)
@return  BCR Read하는데 문제 없으면 0, 문제 발생 시 해당 에러 코드 리턴
*/
int BCR::GetID(int *pnID, bool *bCheck)
{
	DWORD dwLen = 0;
	char szReadBuff[1024] = {0,};
	char szResult[1024] = {0,};
	int nTmpError = NO_ERR;
	int nID = 0;


	m_nLatestID = 0;
	if(m_pnResultID!=NULL)
	{
		*m_pnResultID = 0;
	}
	*pnID = 0;
	*bCheck = 0;

	if(m_pCommPort==NULL)
	{
		nTmpError = ReStart();
		IncreaseRetryCount();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_BCR_OPEN;
	}

	dwLen = m_pCommPort->ReadUntil(szReadBuff, LF);
	if(dwLen == 0)
	{
		ReadCheck = true;
	}

	if(szReadBuff[0]!=0x02||szReadBuff[dwLen-2]!='\r'||szReadBuff[dwLen-1]!='\n')
	{
		if(dwLen!=0)
		{
			BCRRAW_LOG("BCR NG :%s, Len:%d", szReadBuff,dwLen);
            ReadCheck = false;
		}
		nTmpError=ERR_BCR_DATAFORMAT;
		nID=0;
	}
	else if(strstr(szReadBuff, strErr) == NULL)
	{
		// 프로토콜에 의한 값 제거
		szReadBuff[dwLen-2]=0;
		szReadBuff[dwLen-1]=0;
		//ID 마지막 자리수 CheckSum 추후에 추가 필요.
#if(OHT_NAME != OHT_NAME_P4_MASK)
		szReadBuff[dwLen-3] = 0;
#endif

#if(OHT_NAME == OHT_NAME_P4_MASK)
       int FirstChar = (int) szReadBuff[1];
		if(FirstChar < 48 || FirstChar > 57)
		{
			szReadBuff[1] = '0';
			strcpy(szResult, &szReadBuff[1]);
		}
		else
		{
			strcpy(szResult, &szReadBuff[1]);
		}
#else
		strcpy(szResult, &szReadBuff[1]);
#endif

		try
		{
			nID = StrToIntDef(szResult, 0);	// hex 처리

			if(nPreID !=  nID)
				BCRRAW_LOG("BCR OK %s:%d,Len:%d",szReadBuff, nID,dwLen);

			nPreID = nID;
		}
		catch (...)
		{
         	nID = 0;
		}

	   //	nID = atoi(szResult);	// int 처리
        ReadCheck = false;
		nTmpError = NO_ERR;
	}
#if(OHT_NAME != OHT_NAME_P4_MASK)
	else if(dwLen != 10)
	{
		BCRRAW_LOG("BCR Len NG :%s, Len:%d", szReadBuff,dwLen);
		nTmpError=ERR_BCR_DATAFORMAT;
		nID=0;
		ReadCheck = false;
	}
#endif
	if(	nTmpError == NO_ERR)
	{
		if(strstr(szReadBuff, strErr) != NULL)
		{
			BCRRAW_LOG("BCR ERROR");
			nTmpError = ERR_BCR_READ;                 //Read Fail
			nID=0;
			ReadCheck = false;
		}
	}
	m_nLatestID = nID;
	if(m_pnResultID!=NULL)
	{
		*m_pnResultID = nID;
	}
	//Driving으로 Data 전달
	*pnID = nID;
	*bCheck = ReadCheck;

	//진단모니터링 항목(BCR 동작 횟수)
	m_nMovingCount++;

	return  nTmpError;
}

/**
@brief   수신 Data를 확인함
@author 임태웅
@date    2013.07.02
@return Data 확인 시 에러 반납
*/
int BCR::ReadBuffer()
{
	return GetID(&m_nLatestID);
}

/**
@brief   수신된 ID를 읽음
@author 임태웅
@date    2013.07.02
@return 수신된 ID를 반납 
*/
int BCR::GetLatestID()
{
	return m_nLatestID;
}



/**
@brief   결과 값을 받을 ID를 설정
@author 임태웅
@date    2013.07.02
@return 문제 발생 시 에러 발생 
*/
int BCR::SetResultPointer(int* pnResultID, ...)
{
	m_pnResultID = pnResultID;
	return 0;
}

/**
@brief   BCR Read 명령을 Write하는 함수
@author  LSJ
@date    2012.11.04
@return  명령 Write에 문제 없으면 0, 문제 발생시 ERR_BCR_WRITE 리턴
*/
int BCR::SendTriggerOn()
{
	char strBCR[1024] = {0,};

	int nTmpError=0;

	m_nLatestID = 0;
   	if(m_pnResultID!=NULL)
	{
   		*m_pnResultID = 0;
   	}

   if(m_pCommPort==NULL)
   {
	   nTmpError = ReStart();
	   IncreaseRetryCount();
   }
   
   if(m_pCommPort==NULL)
   {
	   return ERR_BCR_OPEN;
   }

	if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
   
   if(nTmpError==NO_ERR && m_pCommPort!=NULL)
   {
	  if(!bUseLeuze)
			sprintf(strBCR, 	"%s%c%c",KEIENCE_BCR_TRIGGER_ON, CR, LF);
		else
			sprintf(strBCR, 	"%c%s%c%c",STX, LEUZE_BCR_TRIGGER_ON, CR, LF);

      if(strlen(strBCR) != m_pCommPort->WriteCommBlock(strBCR, strlen(strBCR)) )
		  nTmpError=ERR_BCR_WRITE;
   }
   m_nTriggerCount++;
   return nTmpError;
}

/**
@brief	 QR Trigger On 함수
@author  임태웅
@date
@return
*/
int BCR::SendTriggerOff()
{
	char strMsg[1024] = {0,};

	int nTmpError=0;

	if(m_pCommPort==NULL)
	{
		nTmpError = ReStart();
		IncreaseRetryCount();
	}
   
	if(m_pCommPort==NULL)
	{
		return ERR_BCR_OPEN;
	}
	   
	if(nTmpError==NO_ERR && m_pCommPort!=NULL)
	{
		if(!bUseLeuze)
			sprintf(strMsg, 	"%s%c%c",KEIENCE_BCR_TRIGGER_OFF, CR, LF);
		else
			sprintf(strMsg, 	"%c%s%c%c",STX, LEUZE_BCR_TRIGGER_OFF, CR, LF);

		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		nTmpError=ERR_BCR_WRITE;
	}
	return nTmpError;
}
/**
@brief   BCR Comm Port를 Open하는 함수
@author  LSJ
@date    2012.11.04
@return  Open 하는데 문제 없으면 0, 문제가 발생하면 ERR_BCR_OPEN 리턴
*/
int BCR::Enable()
{
	int nError=NO_ERR;

	if(m_pCommPort==NULL)
	{
		m_pCommPort = new Comm_Port(m_nPortNum, BCR_BAUDRATE);
		
		if(m_pCommPort==NULL)
		{
			nError=ERR_BCR_OPEN;
		}
	}
	return nError;
}

/**
@brief   BCR Comm Port를 Close하는 함수
@author  LSJ
@date    2012.11.04
*/
void BCR::Disable()
{
    if(m_pCommPort!=NULL)
    {
        delete m_pCommPort;
        m_pCommPort = NULL;
    }
//	m_pCommPort->Close();
}
/**AOHC-249 BCR Read Sequence 개선
@brief   BCR 연결여부 Check하는 함수
@author  Jrespect.im
@date    2019.09.05
@return  Open상태면 True, Close 상태면 False

bool BCR::isEnable()
{
	if(m_pCommPort==NULL)
	{
		return false;
	}
	else return true;
}
*/

int BCR::ReStart()
{
//	m_nRetryCount++;

	if(m_pCommPort!=NULL)
	{
		Disable();	// 통신이 연결 시 종료하고 재연결
	}

	return Enable();
}

int BCR::GetMovingCount(bool bInit)
{
	int nMovingCount = m_nMovingCount;

	if(bInit == true)
	{
		EnterCriticalSection(&m_MovCntCS);
		m_nMovingCount = 0;
		LeaveCriticalSection(&m_MovCntCS);
	}

	return nMovingCount;
}

void BCR::IncreaseRetryCount()
{
	this->m_nRetryCount++;
}
int BCR::GetRetryCount()
{
	return m_nRetryCount;
}

void BCR::ResetRetryCount()
{
    m_nRetryCount = 0;
}

void BCR::SetUseLeuze(bool bUse)
{
	bUseLeuze = bUse;
	if(bUseLeuze)
	{
		strErr = LEUZE_BCR_ERROR;
    }
}

int BCR::GetTriggerCount(bool bInit)
{
	int nTriggerCount = m_nTriggerCount;
	if(bInit == true)
	{
		EnterCriticalSection(&m_MovCntCS);
		m_nTriggerCount = 0;
		LeaveCriticalSection(&m_MovCntCS);
	}

	return nTriggerCount;

}
