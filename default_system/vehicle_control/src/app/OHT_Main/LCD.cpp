#pragma hdrstop

#include "LCD.h"
#include "OHTMain.h"
#include "crc16.h"



/**
@brief   LCD Class 생성자
@author  kyh
@date    2017.12.12
@param   pHWCommon : HWCommon 클래스 포인터
*/
LCD::LCD(HWCommon *pHWCommon)
{
	m_nComPortNum = 15;   			//COM 23
	m_nBaudRate = 115200;
	m_pCommPort = NULL;
	bConnect = false;
}


/**
@brief   LCD Class 소멸자
@author  kyh
@date    2017.12.12
*/
LCD::~LCD()
{
	CloseCom();
}


/**
@brief   LCD Comm Port를 Open하는 함수
@author  kyh
@date    2017.12.12
@return  Open 하는데 문제 없으면 0, 문제가 발생하면 ERR_OPEN 리턴
*/
int LCD::OpenCom()
{
	int nError = 0;

	if(m_pCommPort==NULL)
	{
		m_pCommPort = new Comm_Port(m_nComPortNum, m_nBaudRate);
		if(m_pCommPort==NULL)
		{
			nError=ERR_BCR_OPEN;
		}
		else
		{
			bConnect = true;
		}
	}
	else if(m_pCommPort!=NULL)
	{
		nError = ERR_BCR_OPEN;
		bConnect = false;
	}
	return nError;
}


/**
@brief   LCD 접속 유무를 체크하는 함수
@author  kyh
@date    2017.12.15
@return  LCD이 접속인 경우 True, 비접속인 경우 False를 Return
*/
bool LCD::IsConnect()
{
	return bConnect;
}


/**
@brief   LCD Comm Port를 Close하는 함수
@author  kyh
@date    2017.12.15
*/
int LCD::CloseCom()
{
	int nError = 0;

	if(m_pCommPort!=NULL)
	{
		delete m_pCommPort;
		m_pCommPort = NULL;
	}
	bConnect = false;
	return nError;
}


/**
@brief   LCD Comm Port를 ReOpen하는 함수
@author  kyh
@date    2017.12.15
*/
int LCD::ReOpenCom()
{
	if(m_pCommPort!=NULL)
	{
		CloseCom();	// 통신이 연결 시 종료하고 재연결
	}
	return OpenCom();
}



int LCD::SendTriger(unsigned char* ackData, int Length)
{
	unsigned char strBCR[50] = {0,};
	int Error = 0;
	unsigned short cCRC16_header;

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_BCR_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		memcpy(strBCR+1, ackData, Length);

		//CRC 생성
		cCRC16_header = crc16_ccitt((const char *)&strBCR+1, Length);

		memcpy(strBCR+Length+1, (const char *)&cCRC16_header, 2);

		strBCR[0]=0x02;//Linker 수신 시작 바이트
		strBCR[Length+3]=0x03;//Linker 수신 종료 바이트
	}

	//strlen으로 전송시 데이터 0을 전송 못함
	//if(strlen(strBCR) != m_pCommPort->WriteCommBlock(strBCR, strlen(strBCR)))
	int LCD_Data_Length = (int)m_pCommPort->WriteCommBlock(strBCR, (Length+4));
	if(LCD_Data_Length != (Length+4))
	{
		Error=ERR_BCR_WRITE;
	}
	return Error;
}


/**
@brief   LCD Read 명령을 주고 그에 대한 응답을 받는 함수
@author  yunho.kim
@date    2017.12.24
@param   nID : 읽은 결과 값을 저장하는 변수, 실패 시 0 대입
@return  Read하는데 문제 없으면 0
*/
int LCD::GetID(unsigned char* pReadBuff)
{
	int Error = NO_ERR;
	DWORD dwLen = 0;
	int nReadBuffLength = 0;
	unsigned char szReadBuff[1024] = {0,};

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_BCR_OPEN;
	}
#if((OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
        dwLen = m_pCommPort->ReadLCDUart(szReadBuff);
#else
	dwLen = m_pCommPort->ReadLCDUart(szReadBuff, LCD_ETX);
#endif

	if(dwLen>0)
	{
		memcpy(pReadBuff, szReadBuff, 26);
	}
	else
	{
		Error = ERR_HANDTP_NO_DATA;
	}
	return Error;
}
