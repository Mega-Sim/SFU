/**
@file    OHTDetect.cpp
@version OHT 7.0
@brief   OHTDetect Class CPP File
*/
#include <stdlib.h>
#include <math.h>
#include "OHTDetect.h"
#include "ErrorCode.h"
#include "LogHandler.h"	// Log 작성 목적
#include "DiagManager.h"
#include "Utility.h"
#include <windows.h>
#include "OHTMain.h"


#define DETECT_LOG(...)    WRITE_LOG_UNIT(pLogDetect, __VA_ARGS__)
#define ADD_UBG_ABNORMAL_CHK_LOG(...)   WRITE_LOG_UNIT(pLogUBGAbnormalChk, __VA_ARGS__)

/**
@brief   OHTDetect Class 생성자
@author  임태웅
@date    2013.02.22
@param   pHWCommon : HWCommon Class의 포인터
*/
OHTDetect::OHTDetect(HWCommon *pHWCommon):VirtualDetect()
{
	m_nComPortNum = UBG_ComPort;
	m_nBaudRate = UBG_Baudrate;
	m_pCommPort = NULL;
	bConnect = false;
	m_pHWCommon	= pHWCommon;
	m_nStartEnableTime = timeGetTime();
	pLogDetect = getLogUnit("D:\\log\\DETECT.log", 1024*10);
	pLogUBGAbnormalChk = getLogUnit("D:\\log\\UBGAbnormalCheck.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	CheckUBGConnect(true);
	UBGSeriInfoOuput = 0x00;
	nStepCheckUBGSeriInfo = 0;
	m_bSOSVersionCheck = false;
	m_dwSOSLABCheckTime = timeGetTime();
	m_nRcvCount = 0;
	m_bSOSlabVer2Monitoring = false;
	m_bStartOHTDetectIOCheck = false;
	SOSLABRetryCount = 0;
	m_bInputCheckResult = false;
	m_bOutputCheckResult = false;
	nCheckSOSInputLevel = 0;
	nCheckSOSOutputLevel = 0;

	m_bSOSLABFrameFlag = false; //CheckOSLABFramEnableFlag();
	memset(&m_SoslabFramData, 0x00, sizeof(SOSLAB_FRAMEDATA));
}

/**
@brief   OHTDetect Class 소멸자
@author  임태웅
@date    2013.02.22
*/
OHTDetect::~OHTDetect()
{
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
    CheckUBGConnect(false);
#else
	if(GetSOSLABFramEnableFlag() ==true)
	{
       SOSLABFrameDataEnableCommand(false);
    }
	CheckUBGConnect(false);
	m_bSOSVersionCheck = false;
#endif


}

/**
@brief   OHT Detect 센서의 전원을 ON
@author  임태웅
@date    2013.02.22
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int OHTDetect::Enable()
{
#if(UBG_SENSOR_ENABLE == ON)
	return m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, OFF);
#else
	return NO_ERR;
#endif
}

/**
@brief	 OHT Detect 센서의 전원을 OFF
@author  임태웅
@date	 2013.02.22
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int	OHTDetect::Disable()
{
#if(UBG_SENSOR_ENABLE == ON)
	return m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, ON);
#else
	return NO_ERR;
#endif
}
int OHTDetect::OpenCom()
{
	int nError = 0;
	if(m_pCommPort==NULL)
	{
		//m_pCommPort = new Comm_Port(m_nComPortNum, m_nBaudRate);
		//m_nBaudRate = 38400;
		m_pCommPort = new Comm_Port(m_nComPortNum, 115200, 8 ,NOPARITY, 1);
		DETECT_LOG("OpenCom() %d", m_pCommPort);
		if(m_pCommPort==NULL)
		{
            bConnect = false;
			nError=ERR_UBG_OPEN;
		}
		else
		{
			bConnect = true;
		}
	}
	else
	{
        bConnect = false;
		CloseCom();
	}
	return nError;
}
int OHTDetect::CloseCom()
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

int OHTDetect::Send_UBGData(unsigned char* ackData, int Length)
{
	unsigned char strUBG[50] = {0,};
	int Error=0;

	if(m_pCommPort==NULL)
	{
		DETECT_LOG("Send_UBGData() ERR_UBG_OPEN");
		return ERR_UBG_OPEN;
	}
	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		memcpy(strUBG, ackData, Length);
	}

	if(Length != m_pCommPort->WriteCommBlock(strUBG, Length))
	{
		DETECT_LOG("Send_UBGData() ERR_UBG_WRITE");
		Error=ERR_UBG_WRITE;
	}
	return Error;
}

int OHTDetect::Recieve_UBGData(unsigned char* pReadBuff)
{
	int Error = NO_ERR;
	DWORD dwLen = 0;
	int nReadBuffLength = 0;
	unsigned char szReadBuff[1024] = {0,};

	if(m_pCommPort==NULL)
	{
		return ERR_UBG_OPEN;
	}

	dwLen = m_pCommPort->ReadUntilCountOHTDetect(szReadBuff,UBG_Final_Flag,UBG_Target);

	if(dwLen>0)
	{
		memcpy(pReadBuff, szReadBuff, 512);
		return dwLen;
	}
	else
	{
		DETECT_LOG("Recieve_UBGData() ERR_UBG_WRITE");
		Error = ERR_UBG_WRITE;
	}
	return Error;
}

int OHTDetect::Recieve_UBGData_Fine(unsigned char* pReadBuff)
{
	int Error = NO_ERR;
	DWORD dwLen = 0;
	int nReadBuffLength = 0;
	unsigned char szReadBuff[1024] = {0,};

	if(m_pCommPort==NULL)
	{
		return ERR_UBG_OPEN;
	}

	dwLen = m_pCommPort->ReadUntilCountOHTDetect(szReadBuff,UBG_Final_Flag_Fine,UBG_Target_Fine);

	if(dwLen>0)
	{
		memcpy(pReadBuff, szReadBuff, 512);
		return dwLen;
	}
	else
	{
		DETECT_LOG("Recieve_UBGData() ERR_UBG_WRITE");
		Error = ERR_UBG_WRITE;
	}
	return Error;
}

/**
@brief	 OHT Detect 센서의 전원을 컨트롤 하는 함수
@author  임태웅
@date	 2013.02.22
@param	 nMode(Power On, Off, Reset, Enable)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/

bool	OHTDetect::GetSensorStatus()					// 상태를 받아오는 함수
{
#if(UBG_SENSOR_ENABLE == ON)
	bool	bFail	= 	m_pHWCommon->m_Input.OHTDETECT_Fail;
	int 	nValue	= 	1 * m_pHWCommon->m_Input.OHTDETECT_Long
					+	2 * m_pHWCommon->m_Input.OHTDETECT_Middle
					+	4 * m_pHWCommon->m_Input.OHTDETECT_Short;

    bool breturn = false;
	if(bFail)
	{
//		DETECT_LOG("OHTDetect:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OHTDETECT_Fail
//			, m_pHWCommon->m_Input.OHTDETECT_Long
//			, m_pHWCommon->m_Input.OHTDETECT_Middle
//			, m_pHWCommon->m_Input.OHTDETECT_Short
//			, Status);
		return true;
	}

	switch(nValue)
	{
	case 7: //111
	case 5: //101
         breturn = true;
		break;
	}

	return breturn;
#else

	return false;
#endif

}
/**
@brief	 OHT Detect 센서의 전원을 컨트롤 하는 함수
@author  임태웅
@date	 2013.02.22
@param	 nMode(Power On, Off, Reset, Enable)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/

bool	OHTDetect::GetCheckStatus()					// 상태를 받아오는 함수
{
#if(UBG_SENSOR_ENABLE == ON)
	bool	bFail	= 	m_pHWCommon->m_Input.OHTDETECT_Fail;
	int 	nValue	= 	1 * m_pHWCommon->m_Input.OHTDETECT_Long
					+	2 * m_pHWCommon->m_Input.OHTDETECT_Middle
					+	4 * m_pHWCommon->m_Input.OHTDETECT_Short;

    bool breturn = false;
	if(bFail)
	{
//		DETECT_LOG("OHTDetect:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OHTDETECT_Fail
//			, m_pHWCommon->m_Input.OHTDETECT_Long
//			, m_pHWCommon->m_Input.OHTDETECT_Middle
//			, m_pHWCommon->m_Input.OHTDETECT_Short
//			, Status);
		return true;
	}

	switch(nValue)
	{

	case 2:	//010
	case 3: //011
	case 6: //110
	case 4:	//100
	case 7: //111
	case 5: //101
         breturn = true;
		break;
	}

	return breturn;
#else

	return false;
#endif

}
int	OHTDetect::GetStatusLevel()					// 상태를 받아오는 함수
{

	bool	bFail	= 	m_pHWCommon->m_Input.OHTDETECT_Fail;
	int 	nValue	= 	1 * m_pHWCommon->m_Input.OHTDETECT_Long
					+	2 * m_pHWCommon->m_Input.OHTDETECT_Middle
					+	4 * m_pHWCommon->m_Input.OHTDETECT_Short;
	if(bFail)
	{
		return 5;
	}

//	switch(nValue)
//	{
//
//	case 2:	//010
//	case 3: //011
//	case 6: //110
//	case 4:	//100
//	case 7: //111
//	case 5: //101
//         breturn = true;
//		break;
//	}

	return nValue;


}
/**
@brief	 OHT Detect 센서의 전원을 컨트롤 하는 함수
@author  임태웅
@date	 2013.02.22
@param	 nMode(Power On, Off, Reset, Enable)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/

DETECT_STATUS	OHTDetect::GetStatus()					// 상태를 받아오는 함수
{
#if(UBG_SENSOR_ENABLE == ON)
	bool	bFail	= 	m_pHWCommon->m_Input.OHTDETECT_Fail;
	int 	nValue	= 	1 * m_pHWCommon->m_Input.OHTDETECT_Long
					+	2 * m_pHWCommon->m_Input.OHTDETECT_Middle
					+	4 * m_pHWCommon->m_Input.OHTDETECT_Short;
	DETECT_STATUS	Status;

	if(bFail)
	{
//		DETECT_LOG("OHTDetect:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OHTDETECT_Fail
//			, m_pHWCommon->m_Input.OHTDETECT_Long
//			, m_pHWCommon->m_Input.OHTDETECT_Middle
//			, m_pHWCommon->m_Input.OHTDETECT_Short
//			, Status);
		return DETECT_UNIT_FAIL;
	}
#if(OHT_NAME == OHT_NAME_STD_V81S)
	if(OHTMainForm->CheckBox_ignoreOHT->Checked == true)
	{
		return DETECT_NONE;
	}
#endif
	switch(nValue)
	{
	case 0:	//000
		Status = DETECT_NONE;
		break;
	case 1:	//001
	case 3: //011
	case 2:	//010
	case 6: //110
#if(OHT_NAME == OHT_NAME_STD_V81S)
	case 5: //101
#else
	case 4:	//100
#endif

	case 7: //111
		Status = DETECT_AND_MOVE;
//		DETECT_LOG("OHTDetect:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OHTDETECT_Fail
//			, m_pHWCommon->m_Input.OHTDETECT_Long
//			, m_pHWCommon->m_Input.OHTDETECT_Middle
//			, m_pHWCommon->m_Input.OHTDETECT_Short
//			, Status);
		break;
#if(OHT_NAME == OHT_NAME_STD_V81S)
	case 4:	//100
#else
	case 5:	//101
#endif
		Status = DETECT_AND_STOP;
//		DETECT_LOG("OHTDetect:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OHTDETECT_Fail
//			, m_pHWCommon->m_Input.OHTDETECT_Long
//			, m_pHWCommon->m_Input.OHTDETECT_Middle
//			, m_pHWCommon->m_Input.OHTDETECT_Short
//			, Status);
		break;
	}

	return Status;
#else

	return NO_ERR;
#endif

}

/**
@brief   OHT Detect 센서의 전원을 컨트롤 하는 함수
@author  임태웅
@date    2013.02.22
@param   nMode(Power On, Off, Reset, Enable)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
void	OHTDetect::SetArea(DETECT_AREA_TYPE Type) // 영역을 설정하는 함수
{

}

/**
@brief   OHT Detect 센서의 전원을 컨트롤 하는 함수
@author  임태웅
@date    2013.02.22
@param   nMode(Power On, Off, Reset, Enable)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
double	OHTDetect::GetMovableDist()			// 이동 가능한 거리를 받아오는 함수
{
	return 0;
}

/**
@brief   OHT Detect 센서의 전원을 컨트롤 하는 함수
@author  임태웅
@date    2013.02.22
@param   nMode(Power On, Off, Reset, Enable)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int OHTDetect::PowerControl(int nMode)
{
	int nError = NO_ERR;

#if(UBG_SENSOR_ENABLE == ON)
	switch (nMode)
	{
	case PWR_OFF:
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, ON);
		break;
	case PWR_ON:
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, OFF);
		break;
	case PWR_RST:
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, ON);
		if (nError == NO_ERR)
		{
			Sleep(500);
			nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, OFF);
		}
		break;
	case PWR_ENABLE:
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA1_SEL, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA2_SEL, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA3_SEL, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA4_SEL, OFF);
		break;
	}
#endif

	return nError;
}

/**
@brief   OHT Detect 센서가 알람 발생했을 때 전원을 리셋하는 함수
@author  LSJ
@date    2012.10.31
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int OHTDetect::ErrorReset()
{
    int nError = NO_ERR;

#if(UBG_SENSOR_ENABLE == ON)
	if (m_pHWCommon->m_Input.OHTDETECT_Fail == ON)
    {
        nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, ON);
        if (nError == NO_ERR)
        {
            Sleep(500);
            nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, OFF);
        }
    }
#endif
    return nError;
}

/**
@brief   OHT Detect 센서의 Area를 비트 조합을 통해 선택하는 함수
@author  LSJ
@date    2012.10.31
@param   nArea(비트 조합을 통해 총 16가지의 Area를 선택할 수 있음.)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int OHTDetect::AreaSelect(int nArea)
{
    int nError = NO_ERR;
#if(UBG_SENSOR_ENABLE == ON)
	int i = 0, nDetectOut[4] = {0, };

    for (i = 0; i < 4; i++)
    {
        nDetectOut[i] = ((nArea >> i) & 1) ? 0 : 1;
    }

    /*
    nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA1_SEL, nDetectOut[0]);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA2_SEL, nDetectOut[1]);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA3_SEL, nDetectOut[2]);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA4_SEL, nDetectOut[3]);
	*/

	nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA1_SEL, nDetectOut[0], true, true);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA2_SEL, nDetectOut[1], true, true);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA3_SEL, nDetectOut[2], true, true);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA4_SEL, nDetectOut[3], true, true);
#endif
    return nError;
}

/**
@brief   OHT Detect 센서의 Level을 얻어오는 함수
@author  LSJ
@date    2012.10.31
@param   bLong : Long 입력 On/Off 상태, bMiddle : Middle 입력 On/Off 상태, bShort : Short 입력 On/Off 상태, bFail : Fail 입력 On/Off 상태
@return  OHT Detect 센서 Level을 리턴

int OHTDetect::GetSensorLevel(bool bLong, bool bMiddle, bool bShort, bool bFail)
{
    int nLeveld = 0;

#if(UBG_SENSOR_ENABLE == ON)
	if (bFail == ON)
        nLeveld = -1; // fail
    else if ((bLong == OFF) && (bMiddle == ON) && (bShort == ON))
        nLeveld = 7; // UBG Output1(제일 먼거리 감지)
    else if ((bLong == OFF) && (bMiddle == OFF) && (bShort == ON))
        nLeveld = 6; // UBG Output2(제일 먼거리 감지)
    else if ((bLong == ON) && (bMiddle == OFF) && (bShort == ON))
        nLeveld = 5; // UBG Output3(제일 먼거리 감지)
    else if ((bLong == ON) && (bMiddle == OFF) && (bShort == OFF))
        nLeveld = 4; // UBG Output4(제일 먼거리 감지)
    else if ((bLong == OFF) && (bMiddle == OFF) && (bShort == OFF))
        nLeveld = 3; // UBG Output5(제일 먼거리 감지)
    else if ((bLong == OFF) && (bMiddle == ON) && (bShort == OFF))
        nLeveld = 2; // UBG Output6(제일 먼거리 감지)
    else if ((bLong == ON) && (bMiddle == ON) && (bShort == ON))
        nLeveld = 0; // 미감지
    else
        nLeveld = -1; // Fail ==>재확인필요(ON ON OFF)
#endif
    return nLeveld;
}
*/

// 위 함수 정리
int OHTDetect::GetSensorLevel2(bool bLong, bool bMiddle, bool bShort, bool bFail)
{
	// Long Mid Shor 순서도
	// 000 -> 100 -> 110 -> 010 -> 011 -> 111 -> 101
	//	8  -> 7   -> 6   -> 5    -> 4   -> 3  ->  2  (로 하기로 하자)

	if(bFail)
		return -1;

	if(!bLong && !bMiddle && !bShort)  //000
		return 8;

	if(bLong && !bMiddle && !bShort)  //100
		return 7;

	if(bLong && bMiddle && !bShort)   //110
		return 6;

	if(!bLong && bMiddle && !bShort)    //010
		return 5;

	if(!bLong && bMiddle && bShort)     //011
		return 4;

	if(bLong && bMiddle && bShort)       //111
		return 3;

	if(bLong && !bMiddle && bShort)     //101
		return 2;

	return -1;
}

int OHTDetect::GetOutputSensorLevel(bool bArea1, bool bArea2, bool bArea3, bool bArea4)
{
	//Area1 , Area2, Area3, Area4 순서
	int nError = -1;

	if(bArea1 && bArea2 && bArea3 && bArea4)  //1111은 명령 x
		nError = 0;

	if(!bArea1 && bArea2 && bArea3 && bArea4)  //1000  -> 01111
		nError = 16;

	if(bArea1 && !bArea2 && bArea3 && bArea4)  //0100 -> 1011
		nError =17;

	if(!bArea1 && !bArea2 && bArea3 && bArea4)  //1100 -> 0011
		nError =18;

	if(bArea1 && bArea2 && !bArea3 && bArea4)  //0010 -> 1101
		nError =19;

	if(!bArea1 && bArea2 && !bArea3 && bArea4)  //1010  -> 0101
		nError =20;

	if(bArea1 && !bArea2 && !bArea3 && bArea4)  //0110 -> 1001
		nError =21;

	if(!bArea1 && !bArea2 && !bArea3 && bArea4)  //1110 -> 0001
		nError =22;

	return nError;
}


//AOHC-298
bool OHTDetect::GetOutputSensorStright()
{
	bool breturn = false;

	bool	bArea1	= m_pHWCommon->m_Output.OHTDETECT_Area1_Sel;
	bool	bArea2	= m_pHWCommon->m_Output.OHTDETECT_Area2_Sel;
	bool	bArea3	= m_pHWCommon->m_Output.OHTDETECT_Area3_Sel;
	bool	bArea4	= m_pHWCommon->m_Output.OHTDETECT_Area4_Sel;

	if(!bArea1 && bArea2 && bArea3 && bArea4) //직선 이동하는 경우만 true
		breturn = true;

	return breturn;

}

int OHTDetect::GetSensorLevel_MCC()
{
	// Long Mid Shor 순서도
	// 000 -> 100 -> 110 -> 010 -> 011 -> 111 -> 101
	//	8  -> 7   -> 6   -> 5    -> 4   -> 3  ->  2  (로 하기로 하자)

	bool	bFail	= 	m_pHWCommon->m_Input.OHTDETECT_Fail;
	bool	bLong	=  	m_pHWCommon->m_Input.OHTDETECT_Long;
	bool	bMiddle	=  	m_pHWCommon->m_Input.OHTDETECT_Middle;
	bool	bShort	= 	m_pHWCommon->m_Input.OHTDETECT_Short;

	if(bFail)
		return -1;

	if(!bLong && !bMiddle && !bShort)  //000
		return 8;

	if(bLong && !bMiddle && !bShort)  //100
		return 7;

	if(bLong && bMiddle && !bShort)   //110
		return 6;

	if(!bLong && bMiddle && !bShort)    //010
		return 5;

	if(!bLong && bMiddle && bShort)     //011
		return 4;

	if(bLong && bMiddle && bShort)       //111
		return 3;

	if(bLong && !bMiddle && bShort)     //101
		return 2;

	return -1;
}


/**
@brief   OHT Detect 센서의 어떤 단계가 감지 되었는지 확인하는 함수
@author  LSJ
@date    2012.10.31
@param   nLevel(Long, Middle, Short.)
@return  감지가 되어 있으면 true, 감지되지 않으면 false Return.
*/
bool OHTDetect::IsDetect(int nLevel)
{
	bool bResult = false;

#if(UBG_SENSOR_ENABLE == ON)
	switch (nLevel)
	{
	case DEF_LONG:
        if (m_pHWCommon->m_Input.OHTDETECT_Long == ON)
            bResult = true;
        else
            bResult = false;
        break;
	case DEF_MIDDLE:
		if (m_pHWCommon->m_Input.OHTDETECT_Middle == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case DEF_SHORT:
		if (m_pHWCommon->m_Input.OHTDETECT_Short == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case DEF_FAIL:
		if (m_pHWCommon->m_Input.OHTDETECT_Fail == ON)
			bResult = true;
		else
			bResult = false;
		break;
    }
#endif
	return bResult;
}

bool OHTDetect::IsRegion(int nLevel)
{
	bool bResult = false;

	switch (nLevel)
	{
	case AREA_SEL1:
		if (m_pHWCommon->m_Output.OHTDETECT_Area1_Sel == ON)
			bResult = true;
        else
            bResult = false;
        break;
	case AREA_SEL2:
		if (m_pHWCommon->m_Output.OHTDETECT_Area2_Sel == ON)
            bResult = true;
        else
            bResult = false;
		break;
	case AREA_SEL3:
		if (m_pHWCommon->m_Output.OHTDETECT_Area3_Sel == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case AREA_SEL4:
		if (m_pHWCommon->m_Output.OHTDETECT_Area4_Sel == ON)
			bResult = true;
		else
			bResult = false;
		break;
    }

	return bResult;
}

/**
@brief   OHT Detect 센서가 알람 상태인지 확인하는 함수
@author  LSJ
@date    2012.10.31
@return  Fail 신호가 Off이면 0, ON이면 에러 코드 Return.
*/
bool OHTDetect::CheckError()
{
    int nError = NO_ERR;

#if(OHT_NAME == OHT_NAME_STD_V81S)
	if(OHTMainForm->CheckBox_ignoreOHT->Checked == true)
	{
		return nError;
	}
#endif


#if(UBG_SENSOR_ENABLE == ON)
	if (m_pHWCommon->m_Input.OHTDETECT_Fail == ON)
		nError = ERR_DETECT_SENSOR_FAIL;
#endif
	return nError;
}
/*
int OHTDetect::GetEnableTime(bool bInit)
{
	int nEnableTime = OHTUTILITY::PreventOverflowGetTimeMs(m_nStartEnableTime);

	if(bInit == true)
	{
		m_nStartEnableTime = 0;
	}

	return nEnableTime;
}
*/
/**
@brief   OHT Detect 센서가 off 상태인지 확인하는 함수
@author  puting
@date    2017.08.27
@return  true면 Output 모두 1(Off), 그외 false
*/
bool OHTDetect::CheckSensorOff()
{
	bool breturn = false;

	if((m_pHWCommon->m_Output.OHTDETECT_Area1_Sel ==ON) &&
		(m_pHWCommon->m_Output.OHTDETECT_Area2_Sel ==ON) &&
		(m_pHWCommon->m_Output.OHTDETECT_Area3_Sel ==ON) &&
		(m_pHWCommon->m_Output.OHTDETECT_Area4_Sel ==ON))
	{
		breturn = true;
	}
	else
	{
		breturn = false;
	}
	return breturn;
}

void OHTDetect::RequestData(void)
{
	unsigned char Request_Data[UBG_Command_Packet_Size] = {0x24, 0x47, 0x0D, 0x0A};

    if(bConnect == false)
    {
		DETECT_LOG("RD1: %d", bConnect);
		OpenCom();
		if(bConnect == false)
		{
			DETECT_LOG("RD2 : %d", bConnect);
			return;
        }
    }

    if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

    if(m_pCommPort->WriteCommBlock(Request_Data, UBG_Command_Packet_Size) != UBG_Command_Packet_Size)
    {
        DETECT_LOG("UBG RequestData Send Fail");
    }
}

int OHTDetect::ReceiveData(char *ReadBuff)
{
    int nError = NO_ERR;
	DWORD dwLen = 0;
	int nReadBuffLength = 0;
	unsigned char szReadBuff[2048] = {0,};

	if((m_pCommPort==NULL) || (bConnect==false))
	{
		DETECT_LOG("CommClose");
	}
    else
    {
        // 0x0A 4번 수신될 때 까지
	    dwLen = m_pCommPort->ReadUntilCountOHTDetect2(szReadBuff,UBG_Final_Flag,UBG_Area_Target);

	    if(dwLen>0)
	    {
			memcpy(ReadBuff, szReadBuff, UBG_ReceiveDataSize);
			DETECT_LOG("UBG Receive : %d", dwLen);
		    nError = dwLen;
		}
	    else
	    {
		    nError = 0;
            DETECT_LOG("UBG Data Receive Fail");
	    }

        if( m_pCommPort != NULL)
		    PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    }
    return nError;
}

//Data 감지된 영역 확인.
int OHTDetect::CheckUbgSensorData(int *Buff)
{
    int nError = NO_ERR;
    int nReceiveDataSize = 0;
    int nSensorData = 0;
    int nStart_Pos = 0;
    unsigned char Receive_Data[2048] = {0, };
    AnsiString strLog, strTmp;

    nReceiveDataSize = ReceiveData(Receive_Data);
	DETECT_LOG("CheckUbgSensorData");

	if(nReceiveDataSize == UBG_ReceiveDataSize)
	{
		if((Receive_Data[0] == UBG_Start_Flag_1) && (Receive_Data[1] == UBG_Start_Flag_2))
		{
			strLog.sprintf("%02d", GetUbgAreaStatus());
			DETECT_LOG("UBG Sensor Area : %s", strLog);
			strLog = "";
			ADD_UBG_ABNORMAL_CHK_LOG("%s", Receive_Data);

            for(int i = 0; i < UBG_Sensor_Data_Num; i++)
            {
                nStart_Pos = 3 * i + UBG_Sensor_Data_StartPacketSize;
                //Buff[UBG_Sensor_Data_Num-i-1] = Decode(nStart_Pos, Receive_Data, UBG_Sensor_Data);
                Buff[i] = Decode(nStart_Pos, Receive_Data, UBG_Sensor_Data);

                strTmp.sprintf("/%05d", Buff[i]);
                strLog = strLog + strTmp;

                if(i == 100 || i == 200 || i == 300 || i == 400 || i == 512)
                {
                    DETECT_LOG("UBG Data[%d] : %s", i, strLog);
                    strLog = "";
                }
            }
        }
    }
    else
    {
        nError = -1;
        DETECT_LOG(" UBG Receive Data Size is not Correct");

	}

    return nError;
}

int OHTDetect::Decode(int start, char *code, int byte)
{
	int value = 0;
	int i;
	for (i = start; i < start + byte; ++i)
    {
		value <<= 6;
		value &= ~0x3f;
		value |= code[i] - 0x30;
	}
	return value;
}

//UBG 영역 확인.
int OHTDetect::UpdataData(int *Buff, bool bIsStart)
{
    static DWORD dwUBGTrigerTime = timeGetTime();
    DWORD dwStartTime = timeGetTime();

    int nError = NO_ERR;
    static int Pre_Buff[UBG_Sensor_Data_Num] = {0,};

    if(bIsStart)
    {
		RequestData();
        dwUBGTrigerTime = timeGetTime();
        DETECT_LOG("[Update]bIsStart");
    }
    else
    {
        // 센서 최소 응답 시간 : 240ms
        //if(dwStartTime - dwUBGTrigerTime > UBG_Sensor_Request_Data_Interval)
        if(OHTUTILITY::PreventOverflowGetTimeMs(dwUBGTrigerTime) > UBG_Sensor_Request_Data_Interval)
        {
            // Receive
            DETECT_LOG("[Update]Recieved");
            nError = CheckUbgSensorData(Buff);

            // Send
            if(nError == NO_ERR)
            {

                for(int i=0; i<UBG_Sensor_Data_Num; i++)
                {
                    Pre_Buff[i] = Buff[i];
                }
                RequestData();
            }
            else
            {
                DETECT_LOG("error");
				RequestData();
            }

            dwUBGTrigerTime = timeGetTime();
        }
        else
        {
            for(int i=0; i<UBG_Sensor_Data_Num; i++)
            {
                Buff[i] = Pre_Buff[i];
            }
        }
    }
    return nError;
}

void OHTDetect::DataReadStart(int *Buff)
{
   //	OpenCom();  //AOHC-256
	if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	DETECT_LOG("DataReadStart");
	UpdataData(Buff, true);
}

int OHTDetect::OriginDataReadStart(char *Buff)
{
//	static DWORD dwUBGTrigerTime = timeGetTime();
//	static CStepTimer timerFineStat;
	char SendBuf[10] = "[CH/7F]";
	unsigned char szReadBuff[2048] = {0,};
	char temp_checksum[10]= {0,};
	DWORD dwLen = 0;
	int nError = NO_ERR;

	if(m_pCommPort==NULL)
	{
		DETECT_LOG("[Origin Data] Send_UBGData() ERR_UBG_OPEN");
		return ERR_UBG_OPEN;
	}

	m_pCommPort->WriteCommBlock(SendBuf, StrLen(SendBuf));   // 명령 전송

	DETECT_LOG("[Origin Data] FINE Origin DataReadStart");

//	 sleep(300);
	 // 센서 최소 응답 시간 : 300ms
//	if(timerFineStat.OnDelay(true, 300))
//	{
//		timerFineStat.OnDelay(false);

		dwLen = m_pCommPort->ReadUntilCountOHTDetect(szReadBuff,UBG_Final_Flag_Fine,UBG_Target_Fine);
		DETECT_LOG("FINE UBG Receive");

		if(dwLen>0)
		{
			memcpy(Buff, szReadBuff, 512);
			DETECT_LOG("UBG Receive : %d", dwLen);
		}
		else
		{
			DETECT_LOG("[Origin Data] Recieve_UBGData() ERR_UBG_WRITE");
			nError = ERR_UBG_WRITE;
		}


		// Check Sum
		DETECT_LOG("[Origin Data] Recieved");
		DETECT_LOG("[Origin Data] Recv : %s", szReadBuff);

		int	checksum_xor = szReadBuff[0];

		for (int i = 1; i < dwLen - 3 ; i++)
		{
			checksum_xor = checksum_xor ^ szReadBuff[i];    // 받아드린 버퍼를 계산한 Check sum 값
		}

		temp_checksum[0] = szReadBuff[dwLen - 3];
		temp_checksum[1] = szReadBuff[dwLen - 2];

		int checksum_recv = StrToHex(temp_checksum,StrLen(temp_checksum));


		if(checksum_recv != checksum_xor)
		{
			DETECT_LOG("[Origin Data] Check Sum Fail");
			nError = ERR_UBG_WRITE;
		}
		DETECT_LOG("[Origin Data] Check Sum OK");

//		dwUBGTrigerTime = timeGetTime();
//	}


	return nError;
}

int OHTDetect::GetUbgAreaStatus()
{
	int nValue = 0;

	nValue	= 	1 * m_pHWCommon->m_Output.OHTDETECT_Area1_Sel
			+	2 * m_pHWCommon->m_Output.OHTDETECT_Area2_Sel
			+	4 * m_pHWCommon->m_Output.OHTDETECT_Area3_Sel
			+	8 * m_pHWCommon->m_Output.OHTDETECT_Area4_Sel;

	return nValue;
}

bool OHTDetect::IsConnect()
{
	if(m_pCommPort!=NULL)
	return true;
	else
	return false;
}
//AOHC-256
int OHTDetect::CheckUBGConnect(bool bConnect)
{
	int nError = NO_ERR;

	if(bConnect)
	{
		if(IsConnect() == false)
		   {
				OpenCom();
				DETECT_LOG("CheckUBGConnect -> OpenCom");
				if(IsConnect() == false)
				{
					DETECT_LOG("CheckUBGConnect -> OpenCom NG");
				   nError = ERR_UBG_WRITE;
				   return nError;
				}
				else
				{
				   return nError;       //open com 완료면 return
				}

		   }
		   else
		   {
			   return nError;
		   }
	}
	else
	{    if(IsConnect() == true)
		   {
			   CloseCom();
			  DETECT_LOG("CheckUBGConnect -> Closecom");
				if(IsConnect() == true)
				{  	DETECT_LOG("CheckUBGConnect -> Closecom NG");
					nError = ERR_UBG_WRITE;
					return  nError;
				}
				else
				return nError;  //close com 완료면 return

		   }
		   else
		   {
				return nError;
			}
	}

}

int OHTDetect::GetAreaDataTest()
{
	int nError = NO_ERR;
	unsigned char AreaIndexTest[15] = {0,};
	unsigned char GetArea[2200] = {0,};
	unsigned char GetParam[2200] = {0,};


	nError = AreaIndexCommand();

	Sleep(100);
	if(m_pCommPort != NULL)
	   nError = AreaIndexRecieve(/*AreaIndexTest*/);   //현재 영역 읽어오기

	DETECT_LOG("AreaIndexCommand -------------->GetAreaDataCommand");

	nError = GetAreaDataCommand();

	Sleep(1000);
	if(m_pCommPort != NULL)
	   nError = GetAreaData(/*GetArea*/);   //현재 영역 읽어오기
	 // 에러 조건 추가
	   nError = LoadSOSData(/*GetParam*/);

	   nError = CompData();

}
unsigned char OHTDetect::SOS_CheckSum(unsigned char *buf, int Length)
{
	unsigned char reValue = 0;

	for(int i =0; i< Length;i++)
	{
	  reValue = reValue ^ buf[i];
	}

	return reValue;
}
int OHTDetect::AreaLevelCaptureCommand()
{
	int nError = NO_ERR;
	unsigned char crc_value ;
	unsigned char header_data[SOS_UBG_Command_Data_Size] =  {0xC3,0x51,0xA1,0xF8,0x0f,0x00,0x00,0x01,0x00,0x21,0x01,0x07,0x01,0xC2};
	unsigned char command_data[SOS_UBG_Command_Data_Size] = {0,};

	crc_value = SOS_CheckSum(header_data,SOS_UBG_Command_Data_Size-1 );
	memcpy(command_data,header_data, SOS_UBG_Command_Data_Size-1);
	command_data[SOS_UBG_Command_Data_Size-1] = crc_value;

	nError = Send_UBGData(command_data,SOS_UBG_Command_Data_Size);

	return nError;
}

 void OHTDetect::SetOHTDetectIOCheck(bool start)
{
	m_bStartOHTDetectIOCheck = start;
}
 bool OHTDetect::ResultOHTDetectInputCheck()
{
	static int CurInputUBGData, PreInputUBGData;
	static int Inputcount = 0;

	CurInputUBGData = GetSensorLevel2(m_pHWCommon->m_Input.OHTDETECT_Long, m_pHWCommon->m_Input.OHTDETECT_Middle,m_pHWCommon->m_Input.OHTDETECT_Short,m_pHWCommon->m_Input.OHTDETECT_Fail);

	if(m_bStartOHTDetectIOCheck == true)
	{
		if(CurInputUBGData != PreInputUBGData)
		{
			Inputcount++;
			DETECT_LOG("[ResultOHTDetectInputCheck-1]CurInputUBGData : %d,PreInputUBGData :%d, count:%d ",CurInputUBGData,PreInputUBGData,Inputcount);
		}
		else
		{
			m_bInputCheckResult = true;
		}

		if(Inputcount > 0)
		{
			m_bInputCheckResult = false;
		}


	}
	else
	{
		Inputcount = 0;
		if(CurInputUBGData != PreInputUBGData)
		{
			m_bInputCheckResult = false;
			DETECT_LOG("[ResultOHTDetectInputCheck-2]CurInputUBGData : %d,PreInputUBGData :%d, count:%d ",CurInputUBGData,PreInputUBGData,Inputcount);
		}

	}

	PreInputUBGData = CurInputUBGData;

	return m_bInputCheckResult;

}

 bool OHTDetect::ResultOHTDetectOutputCheck()
{
	static int CurOutputUBGData, PreOutputUBGData;
	static int Outputcount = 0;

	CurOutputUBGData = GetOutputSensorLevel(m_pHWCommon->m_Output.OHTDETECT_Area1_Sel,m_pHWCommon->m_Output.OHTDETECT_Area2_Sel,m_pHWCommon->m_Output.OHTDETECT_Area3_Sel,m_pHWCommon->m_Output.OHTDETECT_Area4_Sel);

	if(m_bStartOHTDetectIOCheck == true)
	{
		if(CurOutputUBGData != PreOutputUBGData)
		{
			Outputcount++;
			DETECT_LOG("[ResultOHTDetectOutputCheck-1]CurOutputUBGData : %d,PreOutputUBGData :%d, count:%d ",CurOutputUBGData,PreOutputUBGData,Outputcount);
		}
		else
		{
			m_bOutputCheckResult = true;
		}

		if(Outputcount > 0)
		{
			m_bOutputCheckResult = false;
		}
	}
	else
	{
		Outputcount = 0;
		if(CurOutputUBGData != PreOutputUBGData)
		{
			m_bOutputCheckResult = false;
			DETECT_LOG("[ResultOHTDetectOutputCheck-2]CurOutputUBGData : %d,PreOutputUBGData :%d, count:%d ",CurOutputUBGData,PreOutputUBGData,Outputcount);
		}

	}

	PreOutputUBGData = CurOutputUBGData;

	return m_bOutputCheckResult;

}
void OHTDetect::SetRetryCountInfo(bool set)
{
	 if(set == true)
		SOSLABRetryCount++;
	 else
		SOSLABRetryCount =0;
}
int OHTDetect::GetRetryCountInfo()
{
	return SOSLABRetryCount;
}
 int OHTDetect::GetAreaLevelCapture()
{
	int nError = NO_ERR;
	int nTmpError = NO_ERR;
	unsigned char SOS_Area_Level[SOS_UBG_Recieve_Data_Size] = {0,};
	int dwlen = 0;
	AnsiString strSOS_Area = "", strSOS_Level="";
	int nSOS_Area=0, nSOS_Level =0;
	int InputUBGData = 0,OutputUBGData = 0;
	static int retry = 0;
	static DWORD dwSOSLABTrigerTime = timeGetTime();
	static bool CaptureStart = true;
	static int count=0;
	if(m_pCommPort==NULL)
	{
		return ERR_UBG_OPEN;
	}


	if(OHTUTILITY::PreventOverflowGetTimeMs(dwSOSLABTrigerTime) > 100)
	{
		nTmpError = AreaIndexRecieve(SOS_Area_Level); // rcv

		InputUBGData = GetSensorLevel2(m_pHWCommon->m_Input.OHTDETECT_Long, m_pHWCommon->m_Input.OHTDETECT_Middle,m_pHWCommon->m_Input.OHTDETECT_Short,m_pHWCommon->m_Input.OHTDETECT_Fail);
		OutputUBGData = GetOutputSensorLevel(m_pHWCommon->m_Output.OHTDETECT_Area1_Sel,m_pHWCommon->m_Output.OHTDETECT_Area2_Sel,m_pHWCommon->m_Output.OHTDETECT_Area3_Sel,m_pHWCommon->m_Output.OHTDETECT_Area4_Sel);

		if(nTmpError != NO_ERR)
		{
			retry++;
			SetRetryCountInfo(true);
			DETECT_LOG("[GetAreaLevelCapture Retry] nError : %d, retry count : %d ", nTmpError,retry);
		}
		else
		{
			dwlen= sizeof(SOS_Area_Level);
			if(dwlen > 0)
			{
				strSOS_Area = SOS_Area_Level[12];
				strSOS_Level = SOS_Area_Level[13];
			}
			else
			{
				strSOS_Area = 0;
				strSOS_Level = 0;
			}
			nSOS_Area =  StrToIntDef(strSOS_Area.c_str(), 0);
			nSOS_Level = StrToIntDef(strSOS_Level.c_str(),0);

			SetOHTDetectIOCheck(false);

			if(ResultOHTDetectInputCheck() == true)
			{
//				InputUBGData = GetSensorLevel2(m_pHWCommon->m_Input.OHTDETECT_Long, m_pHWCommon->m_Input.OHTDETECT_Middle,m_pHWCommon->m_Input.OHTDETECT_Short,m_pHWCommon->m_Input.OHTDETECT_Fail);
				nError = CheckSOSLevel(8-nSOS_Level,InputUBGData);
			}

			if((nError == NO_ERR) &&( (ResultOHTDetectOutputCheck() == true)))
			{
//				OutputUBGData = GetOutputSensorLevel(m_pHWCommon->m_Output.OHTDETECT_Area1_Sel,m_pHWCommon->m_Output.OHTDETECT_Area2_Sel,m_pHWCommon->m_Output.OHTDETECT_Area3_Sel,m_pHWCommon->m_Output.OHTDETECT_Area4_Sel);
				nError = CheckSOSArea(nSOS_Area,OutputUBGData);
			}

			AreaLevelCaptureCommand();
			m_bInputCheckResult = false; // ResultOHTDetectIOCheck() IOCheck 시작 전 변수 초기화
			m_bOutputCheckResult = false;
			SetOHTDetectIOCheck(true); // ResultOHTDetectIOCheck() IOCheck 시작
			dwSOSLABTrigerTime = timeGetTime();
		}
	}

	if(retry > 5)
	{
		CaptureStart = true;
		retry = 0;
		SetOHTDetectIOCheck(false);
	}

	if(CaptureStart == true)
	{
		DETECT_LOG("CheckSOSLevel Start, Retry");
		AreaLevelCaptureCommand();
		dwSOSLABTrigerTime = timeGetTime();
		nCheckSOSInputLevel = 0; //통신 불량 시 초기화.
		nCheckSOSOutputLevel = 0;
		CaptureStart = false;
		m_bInputCheckResult = false; // ResultOHTDetectIOCheck() IOCheck 시작 전 변수 초기화
		m_bOutputCheckResult = false;
		SetOHTDetectIOCheck(true); // ResultOHTDetectIOCheck() IOCheck 시작
	}

	return nError;

}
int OHTDetect::CheckSOSLevel(int SensorLevel, int InputLevel)
{
	int nError = NO_ERR;
	int OutputArea;

	OutputArea = GetOutputSensorLevel(m_pHWCommon->m_Output.OHTDETECT_Area1_Sel,m_pHWCommon->m_Output.OHTDETECT_Area2_Sel,m_pHWCommon->m_Output.OHTDETECT_Area3_Sel,m_pHWCommon->m_Output.OHTDETECT_Area4_Sel);

	if(OutputArea == 0)
	{
		nCheckSOSInputLevel = 0;
		return nError;
	}
	DETECT_LOG("SL/%d/IL/%d",SensorLevel,InputLevel);
	if(SensorLevel != InputLevel)
	{
		nCheckSOSInputLevel++;
		DETECT_LOG("[CheckSOSLevel]SensorLevel : %d InputLevel : %d, count : %d",SensorLevel,InputLevel,nCheckSOSInputLevel);
		if(nCheckSOSInputLevel > 5) //5회 retry , 일반적으로 레벨 감지 유지 시간 1sec 이상,
		{
			nError = ERR_DETECT_SENSOR_FAIL;
			nCheckSOSInputLevel = 0;
			DETECT_LOG("[CheckSOSLevel]ERR_DETECT_SENSOR_FAIL-Input, SensorLevel:%d, InputLevel:%d",SensorLevel,InputLevel);
			return nError;
		}
	}
	else
	{
		nCheckSOSInputLevel = 0;
	}

	return nError;

}
int OHTDetect::CheckSOSArea(int SensorArea, int OutputArea)
{
	int nError = NO_ERR;

	if(OutputArea == 0)
	{
		nCheckSOSOutputLevel = 0;
		return nError;
	}
	DETECT_LOG("SA/%d/OA/%d",SensorArea,OutputArea);
	if(SensorArea != OutputArea )
	{
		nCheckSOSOutputLevel++;
		DETECT_LOG("SensorArea : %d, OutputArea : %d, count : %d",SensorArea,OutputArea,nCheckSOSOutputLevel );
		if(nCheckSOSOutputLevel> 5)  //5회 retry , 일반적으로 레벨 감지 유지 시간 1sec 이상,
		{
			nError = ERR_DETECT_SENSOR_FAIL;
			nCheckSOSOutputLevel = 0;
			DETECT_LOG("[CheckSOSLevel]ERR_DETECT_SENSOR_FAIL-Output,SensorArea:%d, OutputArea:%d",SensorArea,OutputArea);
			return nError;
		}
	}
	else
	{
		nCheckSOSOutputLevel = 0;
	}

	return nError;

}
bool OHTDetect::GetSOSVersion()
{
	return m_bSOSVersionCheck;
}
bool OHTDetect::GetVersion2Monitoring()
{
	return m_bSOSlabVer2Monitoring;
}
AnsiString OHTDetect::SOS_FW_VersionCheck()//함수에 인자 추가 여부 확인
{
	int nError = NO_ERR;
	unsigned char SOS_FW_Version[SOS_UBG_Recieve_Data_Size] = {0,};
	int dwlen = 0;
	AnsiString fw_year, fw_month, fw_day;
	char strMsg[1024]={0,};
	int year, month, day;
	static DWORD dwVersionCheckRetryTime = timeGetTime();

	if(m_pCommPort==NULL)
	{
		return ERR_UBG_OPEN;
	}
	//버전 체크는 20초 안에만 완료되면 정상 처리
	if(OHTUTILITY::PreventOverflowGetTimeMs(dwVersionCheckRetryTime)< 20*1000)
	{
		nError = SOS_FW_VersionCheckCommand();

		Sleep(50);
		if(nError == NO_ERR)
		{
			nError = AreaIndexRecieve(SOS_FW_Version);
		}
		dwlen = sizeof(SOS_FW_Version);
		if(dwlen > 0)
		{
			fw_year = SOS_FW_Version[12];
			fw_month = SOS_FW_Version[13];
			fw_day = SOS_FW_Version[14];

			m_bSOSVersionCheck = true;
		}
		else
		{
			fw_year = 0;
			fw_month = 0;
			fw_day = 0;
		}
		//year 비교
		m_bSOSlabVer2Monitoring = CheckYearVersion(StrToIntDef(fw_year.c_str(), 0),StrToIntDef(fw_month.c_str(), 0),StrToIntDef(fw_day.c_str(), 0));
		DETECT_LOG("[Version Check]fw_year : %s, fw_month : %s, fw_day : %s", fw_year, fw_month, fw_day);
		if(m_bSOSlabVer2Monitoring ==  true)
		{
			sprintf(strMsg, "SOSLAB[Ver %s.%s.%s]", fw_year, fw_month, fw_day);
			return strMsg;
		}
		else
		return -1;
	}
	else
	{
		DETECT_LOG("[Error]fw_year : %s, fw_month : %s, fw_day : %s", fw_year, fw_month,fw_day);
		return  ERR_UBG_OPEN;
	}
}
bool OHTDetect::CheckYearVersion(int year_version, int month_version, int day_version)
{
	//22.12.21버전 이후 해당 기능 사용 가능
	//year 22 이//, month 1~12 까지만, day는 1~31 까지만
	if(year_version >= 22 &&(month_version >=1 && month_version <=12)&&(day_version >= 1 &&day_version<=31))
	return true;
	else
	return false;
}
int OHTDetect::SOS_FW_VersionCheckCommand()
{
	int nError = NO_ERR;
	unsigned char crc_value ;
	unsigned char header_data[SOS_UBG_Command_Data_Size] =  {0xC3,0x51,0xA1,0xF8,0x03,0x00,0x00,0x01,0x01,0x21,0x02,0x0C,0x01,0xC2};
	unsigned char command_data[SOS_UBG_Command_Data_Size] = {0,};


	crc_value = SOS_CheckSum(header_data,SOS_UBG_Command_Data_Size-1 );
	memcpy(command_data,header_data, SOS_UBG_Command_Data_Size-1);
	command_data[SOS_UBG_Command_Data_Size-1] = crc_value;

	nError = Send_UBGData(command_data,SOS_UBG_Command_Data_Size);

	DETECT_LOG("SOS_FW_VersionCheckCommand : %d",nError);

	return nError;
}
int OHTDetect::AreaIndexCommand()
{
	int nError = NO_ERR;
	unsigned char crc_value ;
	unsigned char header_data[SOS_UBG_Command_Data_Size] =  {0xC3,0x51,0xA1,0xF8,0x0F,0x00,0x00,0x01,0x00,0x21,0x01,0x04,0x10/*area index*/,0xC2};
	unsigned char command_data[SOS_UBG_Command_Data_Size] = {0,};


	crc_value = SOS_CheckSum(header_data,SOS_UBG_Command_Data_Size-1 );
	memcpy(command_data,header_data, SOS_UBG_Command_Data_Size-1);
	command_data[SOS_UBG_Command_Data_Size-1] = crc_value;

	nError = Send_UBGData(command_data,SOS_UBG_Command_Data_Size);

	DETECT_LOG("AreaIndexCommand : %d",nError);

	return nError;
}



//AMOHTV80F-952 Frame Data Stream Enable/Disalbe 함수
int OHTDetect::SOSLABFrameDataEnableCommand(bool bEnable)
{
	int nError = NO_ERR;
	unsigned char crc_value ;
	int nLen = 0;
												  //Ps1, Ps2, P23, Ps4, TL_L TL_H  PI  PL   SM   BI  CAT0  CAT1 DTn
	unsigned char header_dataEnable[SOS_UBG_Command_Data_Size] =  {0xC3,0x51,0xA1,0xF8,0x00,0x00,0x00,0x01,0x00,0x21,0x01,0x03,0x01/*area index*/,0xC2};
	unsigned char header_dataDisable[SOS_UBG_Command_Data_Size] =  {0xC3,0x51,0xA1,0xF8,0x00,0x00,0x00,0x01,0x00,0x21,0x01,0x03,0x00/*area index*/,0xC2};
	unsigned char command_data[SOS_UBG_Command_Data_Size] = {0,};
	unsigned char szReadBuff[SOS_UBG_Command_Data_Size] = {0,};

	if( m_pCommPort == NULL){ return -1;}

	if(bEnable == true)
	{
		crc_value = SOS_CheckSum(header_dataEnable,SOS_UBG_Command_Data_Size-1 );
		memcpy(command_data,header_dataEnable, SOS_UBG_Command_Data_Size-1);
	}
	else
	{
		crc_value = SOS_CheckSum(header_dataDisable,SOS_UBG_Command_Data_Size-1 );
		memcpy(command_data,header_dataDisable, SOS_UBG_Command_Data_Size-1);
    }

	command_data[SOS_UBG_Command_Data_Size-1] = crc_value;

	nLen = m_pCommPort->WriteCommBlock(command_data,SOS_UBG_Command_Data_Size);
	if(nLen != SOS_UBG_Command_Data_Size)
	{
		nError = -1;
		DETECT_LOG("SOSLABFrameDataEnableCommand1 : %d %d %d",nError, SOS_UBG_Command_Data_Size, nLen);
	}

	Sleep(500);
	if(nError ==NO_ERR)
	{
		if( m_pCommPort != NULL)
		{
			nLen = m_pCommPort->ReadUntilCount_SOS_AreaIndex(szReadBuff);
			if(nLen<=0){   nError =  -1; }
			PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
        }

		if(nError == NO_ERR)
		{
			if(szReadBuff[SOS_UBG_CHECK_FRAMEENABLE] != 0x01)
			nError =  -1;
		}
	}

	if(nError == NO_ERR)
	{

		if(bEnable == true) m_bSOSLABFrameFlag = true;
		else m_bSOSLABFrameFlag = false;
    }
    memset(&m_SoslabFramData, 0x00, sizeof(SOSLAB_FRAMEDATA));
	DETECT_LOG("SOSLABFrameDataEnableCommand : %d/%d",nError,m_bSOSLABFrameFlag);

	return nError;
}
//AMOHTV80F-952 Frame Data Stream에서 데이터 확인 함수  ... puting
int OHTDetect::SOSLABFrameDataGetCommand()//(unsigned char *ReadBuff)     // ReadBuff size : SOS_UBG_Frame_Data_Size
{
  //주기적으로 받아야됨.
  	int nError = NO_ERR;
	DWORD dwLen = 0;
	unsigned char szReadBuff[SOS_UBG_Frame_Data_Size] = {0,};

	if(m_pCommPort==NULL)
	{
		return 0;
	}

	dwLen = m_pCommPort->ReadUntilCount_SOS_GetFrame(szReadBuff, SOS_UBG_Frame_Data_Size);

	DETECT_LOG("SOSLABFrameDataGetCommand() size : %d, %x %x %x %x /%x %x %x %x /%x %x",dwLen,szReadBuff[0],szReadBuff[1],szReadBuff[2],szReadBuff[3],szReadBuff[801],szReadBuff[802],szReadBuff[803],szReadBuff[804], szReadBuff[SOS_UBG_Frame_Data_Size-1],szReadBuff[SOS_UBG_Frame_Data_Size-2]);
	if(dwLen == SOS_UBG_Frame_Data_Size)
	{
//		memmove(ReadBuff, szReadBuff, dwLen);
        memmove(&m_SoslabFramData, szReadBuff, sizeof(SOSLAB_FRAMEDATA));
	}else
	{
        memset(&m_SoslabFramData, 0x00, sizeof(SOSLAB_FRAMEDATA));
    }


	 if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	return dwLen;


}


//AMOHTV80F-952 Frame Data Stream에서 데이터 enable 상태 확인  ... puting
bool OHTDetect::GetSOSLABFramEnableFlag()
{
    return  m_bSOSLABFrameFlag;
}


int OHTDetect::AreaIndexRecieve()
{
	int nError = NO_ERR;
	DWORD dwLen = 0;
	unsigned char szReadBuff[SOS_UBG_Recieve_Data_Size] = {0,};

	if(m_pCommPort==NULL)
	{
		return ERR_UBG_OPEN;
	}

	dwLen = m_pCommPort->ReadUntilCount_SOS_AreaIndex(szReadBuff);     //dwLen DWORD 아니고 int 로 변경해서 테스트해보기

	if(dwLen>0)
	{
		DETECT_LOG("AreaIndexRecieve %d", dwLen);
	}
	else
	{
		DETECT_LOG("AreaIndexRecieve() ERR_UBG_WRITE-1");
		return ERR_UBG_WRITE;
	}

	 if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	return nError;

}
int OHTDetect::AreaIndexRecieve(unsigned char *ReadBuff)
{
	int nError = NO_ERR;
	DWORD dwLen = 0;
	unsigned char szReadBuff[SOS_UBG_Recieve_Data_Size] = {0,};

	if(m_pCommPort==NULL)
	{
		return ERR_UBG_OPEN;
	}


	dwLen = m_pCommPort->ReadUntilCount_SOS_AreaIndex(szReadBuff);

	if(dwLen>0)
	{
		memcpy(ReadBuff, szReadBuff, dwLen);
	}
	else
	{
		nError = ERR_UBG_WRITE;
		DETECT_LOG("AreaIndexRecieve() ERR_UBG_WRITE-2 nError : %d",nError);
		return nError;

	}

	 if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	return nError;

}
int OHTDetect::GetAreaDataCommand()
{
	int nError = NO_ERR;
	unsigned char crc_value ;
	unsigned char header_data[SOS_UBG_Command_Data_Size] =  {0xC3,0x51,0xA1,0xF8,0x0F,0x00,0x00,0x01,0x01,0x21,0x02,0x03,0x01/*byte 12*/,0xC2};
	unsigned char command_data[SOS_UBG_Command_Data_Size] = {0,};


	crc_value = SOS_CheckSum(header_data,SOS_UBG_Command_Data_Size-1 );
	memcpy(command_data,header_data, SOS_UBG_Command_Data_Size-1);
	command_data[SOS_UBG_Command_Data_Size-1] = crc_value;

	nError = Send_UBGData(command_data,SOS_UBG_Command_Data_Size);

	DETECT_LOG("GetAreaData : %d",nError);

	return nError;
}
int OHTDetect::GetAreaData()
{
	int nError = NO_ERR;
	DWORD dwLen = 0;
	unsigned char m_szReadBuff[2200] = "";
	if(m_pCommPort==NULL)
	{
		return ERR_UBG_OPEN;
	}

	dwLen = m_pCommPort->ReadUntilCount_SOS_GetArea(m_szReadBuff);     //dwLen DWORD 아니고 int 로 변경해서 테스트해보기

	if(dwLen>0)
	{
		DETECT_LOG("AreaIndexRecieve %d", dwLen);
		memcpy(m_szGetBuff, m_szReadBuff, sizeof(m_szReadBuff));
	}
	else
	{
		DETECT_LOG("AreaIndexRecieve() ERR_UBG_WRITE-3");
		return ERR_UBG_WRITE;
	}

	 if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	return nError;
}
int OHTDetect::StrToHex(char* szHex, int nDigit)
{
	int nHex = 0;
	int nNum = 0;

	for(int i=0; i<nDigit; i++)
	{
		switch(szHex[i])
        {
			case 'a': case 'A':    nNum = 10; break;
			case 'b': case 'B': nNum = 11; break;
			case 'c': case 'C': nNum = 12; break;
			case 'd': case 'D':    nNum = 13; break;
			case 'e': case 'E':    nNum = 14; break;
			case 'f': case 'F': nNum = 15; break;
            default:
				if(szHex[i]>='0' && szHex[i]<='9')
					nNum = szHex[i]-48;
				else
					return    -1;
			break;
		}

		nHex+=nNum*std::pow(16, nDigit-i-1);
	}


	return nHex;
}

bool OHTDetect::LoadSOSData()
{
	AnsiString strTmp;
	AnsiString strTmp1;
	AnsiString strTmp2 = "";
	char szTmpValue[2200]="";
	int i =0;
	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile("DataFiles\\OHTDetectAreaInfo_SOS.ini");
	if(!pIniFile->GetIniFileExist())
	{
		delete pIniFile;
		pIniFile = NULL;
	}
	if(pIniFile != NULL)
	{
		//파일 경로에서 데이터 받아옴
		for(i=0;i<2200;i++)
		{
			strTmp.sprintf("sosRcv%d",i);
			pIniFile->ReadString("SOSRecvPattern",strTmp.c_str(),"00",szTmpValue);
			m_szLoadBuff[i] = StrToHex(szTmpValue,strlen(szTmpValue));
			memset(szTmpValue, 0x00, sizeof(szTmpValue));
		}

	}

	delete pIniFile;

	return true;
}

bool OHTDetect::CompData()
{

	bool bsame = (memcmp(m_szGetBuff, m_szLoadBuff, sizeof(m_szGetBuff)) == 0);         //이게 1 이면 다른거 , 0이면 같은거

	if(bsame == true)
	{
		DETECT_LOG("SOS Pattern Comp SUCCESS!!!");
		return true;
	}
	else
	{
		DETECT_LOG("SOS Pattern Comp FAIL...");
		return false;
	}

}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void OHTDetect::CheckUBGSeriInfo(int nStatus)
{
	static DWORD dwTimeUBGSeriInfo = timeGetTime();

	switch(nStepCheckUBGSeriInfo)
	{
		case 0:
			if(nStatus == STS_GOING)
			{
				char SendBuf[40] = {'$', 'I', 'R', 0x0D, '\0'};
				int nTmpRet = 0;
				nTmpRet = Send_UBGData(SendBuf,6);
				dwTimeUBGSeriInfo  = timeGetTime();
				nStepCheckUBGSeriInfo = 1;
			}
		break;

		case 1:
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwTimeUBGSeriInfo)>150)
			{

				nStepCheckUBGSeriInfo = 2;
			}
		break;
		case 2:
			char RcvBuf[1024] = {0,};
			ZeroMemory(RcvBuf, sizeof(RcvBuf));
			int nReadBytes = Recieve_UBGData(RcvBuf);	//recv
			DETECT_LOG("CheckUBGSerialInfo:%s,%c",RcvBuf, RcvBuf[7]);
			UBGSeriInfoOuput = RcvBuf[7];
			nStepCheckUBGSeriInfo = 0;
		break;
	}
}
/*     //UBG RESET 삭제로 정합성 기능 삭제
int OHTDetect::CheckUBGIOSeriInfo()   //UBG IO Check
{
	static DWORD dwTimeUBGSeriCheck  = timeGetTime();

	switch(nStepCheckUBGIOSeriInfo)
	{
		case 0:
		{
			char SendBuf[40] = {'$', 'I', 'R', 0x0D, '\0'};
			int nTmpRet = 0;

			AreaSelect(1);      //직선 패턴 ON
			nTmpRet = Send_UBGData(SendBuf,6);
			dwTimeUBGSeriCheck  = timeGetTime();
			nStepCheckUBGIOSeriInfo = 1;
		break;
		}


		case 1:
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwTimeUBGSeriCheck)>300)   //Data Recive 300ms Delay
			{
				nStepCheckUBGIOSeriInfo = 2;
			}
		break;
		}
		case 2:
		{
			char RcvBuf[1024] = {0,};
			ZeroMemory(RcvBuf, sizeof(RcvBuf));
			int nReadBytes = Recieve_UBGData(RcvBuf);	//recv
			DETECT_LOG("UBG IO CheckUBGSerialInfo:%s,%c",RcvBuf, RcvBuf[7]);
			nStepCheckUBGIOSeriInfo = 0;

			AreaSelect(0);    // 패턴 리셋


			if (RcvBuf[7] =='1')     //직선 패턴은 1
			{
				DETECT_LOG("UBG 패턴 체크 OK :RcvBuf[7]:%d, %c ,  ::%c",RcvBuf[7],RcvBuf[7],'1');
				bStepCheckUBGInfo = false;
				return 0;
			}
			else                    // 다른 패턴일 경우 ERROR
			{
				DETECT_LOG("UBG 패턴 체크 FAIL:RcvBuf[7]:%d, %c ,  ::%c",RcvBuf[7],RcvBuf[7],'1');
				if(nStepCheckUBGInfo >1)
				{
					bStepCheckUBGInfo = false;
				}
				nStepCheckUBGInfo++;
			}

		break;
		}
	}

	return -1;
}
*/
//모니터링 케이블 연결 감시 함수
bool OHTDetect::CheckUBGOuputInfo()
{
	bool breturn = false;

	bool	bArea1	= m_pHWCommon->m_Output.OHTDETECT_Area1_Sel;
	bool	bArea2	= m_pHWCommon->m_Output.OHTDETECT_Area2_Sel;
	bool	bArea3	= m_pHWCommon->m_Output.OHTDETECT_Area3_Sel;
	bool	bArea4	= m_pHWCommon->m_Output.OHTDETECT_Area4_Sel;


	bool	bArea1Input	= m_pHWCommon->m_Input.Rotate_90_Sensor;
	bool	bArea2Input	= m_pHWCommon->m_Input.Rotate_180_Sensor;
	bool	bArea3Input	= m_pHWCommon->m_Input.Rotate_270_Sensor;
#if(OHT_NAME == OHT_NAME_STD_V80)
	bool	bArea4Input	= !m_pHWCommon->m_Input.UTB_Detect;

	if((bArea1== bArea1Input) &&
	   (bArea2== bArea2Input) &&
	   (bArea3== bArea3Input) &&
	   (bArea4== bArea4Input))
	   {
			breturn = true;

	   }
#endif

	return  breturn;
}

//대차센서 설정 I/O 값 확인 함수
int OHTDetect::GetUBGSelectStatus()
{
	int 	nValue	= 	1 * (!m_pHWCommon->m_Output.OHTDETECT_Area1_Sel)
					+	2 * (!m_pHWCommon->m_Output.OHTDETECT_Area2_Sel)
					+	4 * (!m_pHWCommon->m_Output.OHTDETECT_Area3_Sel)
					+	8 * (!m_pHWCommon->m_Output.OHTDETECT_Area4_Sel);

	return nValue;
}
