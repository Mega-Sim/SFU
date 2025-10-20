// ---------------------------------------------------------------------------
/**
@file    Obstacle.cpp
@version OHT 7.0
@brief   Obstacle Class CPP File
*/

#pragma hdrstop

#include "Obstacle.h"
#include "LogHandler.h"	// Log 작성 목적
#include "Utility.h"
#include "DiagManager.h"
#include "MyTools.h"
#include "OHTMain.h"


#define OBS_LOG(...)    WRITE_LOG_UNIT(m_pLogObstacle, __VA_ARGS__)
#define OBS_AREA_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\OBSAreaCheck.log", __VA_ARGS__)


/**
@brief   Obstacle 생성자
@author  LSJ
@date    2012.10.13
@param   pHWCommon : HWCommon Class의 포인터
@note    Input, Ouput을 사용하기 위해 HWCommon의 Pointer를 가져옴
*/
Obstacle::Obstacle(HWCommon *pHWCommon):VirtualDetect()
{
	m_pHWCommon = pHWCommon;
	m_nStartEnableTime = timeGetTime();
    m_nComPortNum = COM_OBSTACLE;                // Com Port 9
	m_nBaudRate = OBS_Baudrate;                 // 38400
	m_pCommPort = NULL;

    m_pLogObstacle = getLogUnit("D:\\log\\Obstacle.log", 1024*10);
	bConnect = false;
	CheckOBSConnect(true);
	m_bSOSLABFrameFlagOBS = false; //CheckOSLABFramEnableFlag();
	memset(&m_SoslabFramData, 0x00, sizeof(SOSLAB_FRAMEDATA));

}

/**
@brief   Obstacle 소멸자
@author  LSJ
@date    2012.10.13
*/
Obstacle::~Obstacle()
{
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
    CloseCom ();
#else
	if(GetSOSLABFramEnableFlag() ==true)
	{
	   SOSLABFrameDataEnableCommand(false);
    }
    CheckOBSConnect(false);
#endif

}

/**
@brief   OHT Detect 센서의 전원을 ON
@author  임태웅
@date    2013.02.22
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Obstacle::Enable()
{
	return m_pHWCommon->SetBit(OUT_OBSTACLE_POWER_RESET, OFF);
}

/**
@brief	 Obstacle 센서의 전원을 OFF
@author  임태웅
@date	 2013.02.22
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int	Obstacle::Disable()
{
	return m_pHWCommon->SetBit(OUT_OBSTACLE_POWER_RESET, ON);
}

/**
@brief	 Obstacle 센서의 전원을 컨트롤 하는 함수
@author  임태웅
@date	 2013.02.22
@param	 nMode(Power On, Off, Reset, Enable)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/

DETECT_STATUS	Obstacle::GetStatus()					// 상태를 받아오는 함수
{
	bool	bFail	= 	m_pHWCommon->m_Input.OBSTACLE_Fail;
	int 	nValue	= 	1 * m_pHWCommon->m_Input.OBSTACLE_Long
					+	2 * m_pHWCommon->m_Input.OBSTACLE_Middle
					+	4 * m_pHWCommon->m_Input.OBSTACLE_Short;
	DETECT_STATUS	Status;
	static DWORD dwAbnormalCheckTime = 0;
	static bool bAbnormalCheck = false;

	if(bFail)
	{
//		OBS_LOG("OBS:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OBSTACLE_Fail
//			, m_pHWCommon->m_Input.OBSTACLE_Long
//			, m_pHWCommon->m_Input.OBSTACLE_Middle
//			, m_pHWCommon->m_Input.OBSTACLE_Short
//			, Status);
		return DETECT_UNIT_FAIL;
	}
#if((OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
	if(OHTMainForm->CheckBox_ignoreOBS->Checked == true)
	{
		return DETECT_NONE;
	}
#endif
	// -------------------------------------------
    // [BEGIN] by zzang9un 2013.12.05 : 탈조, Move, Stop 조건을 AMC랑 통일함
    switch(nValue)
	{
	case 0:	// 000
		Status = DETECT_NONE;
		bAbnormalCheck = false;
		break;
	case 5: // 101 : 탈조
		Status = DETECT_NONE;

		//진단모니터링 항목(PBS 탈조발생 Count)
		if(bAbnormalCheck == false)
		{
			dwAbnormalCheckTime = timeGetTime();
			bAbnormalCheck = true;
		}
		else
		{
			//Timeout시간동안 탈조가 발생한 경우 Abnormal 처리
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwAbnormalCheckTime) > DiagManager::getInstance()->GetDiagParam().nPBSOutOfRangeTimeout)
			{
				dwAbnormalCheckTime = 0;
				bAbnormalCheck = false;
				if(OHTUTILITY::PreventOverflowGetTimeMs(DiagManager::getInstance()->GetDiagEventFlag().dwPBSOutOfRangeTimeout) > 24*60*60*1000)
				{
					DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_OUT_OF_PHASE_PBS);
					DiagManager::getInstance()->GetDiagEventFlag().dwPBSOutOfRangeTimeout = timeGetTime();
				}
			}
		}
		break;

    case 2:	// 010 : 비정상(난반사)
	case 4:	// 100 : 비정상(난반사)        	
	case 6:	// 110 : 비정상(난반사)        	
	case 7:	// 111 : 정상 감지 정지
		Status = DETECT_AND_STOP;
		bAbnormalCheck = false;
		break;    

    case 1:	// 001
    case 3:	// 011
//		OBS_LOG("OBS:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OBSTACLE_Fail
//			, m_pHWCommon->m_Input.OBSTACLE_Long
//			, m_pHWCommon->m_Input.OBSTACLE_Middle
//			, m_pHWCommon->m_Input.OBSTACLE_Short
//			, Status);
		Status = DETECT_AND_MOVE;
		bAbnormalCheck = false;
		break;
	}
    // [END] by zzang9un 2013.12.05 : 탈조, Move, Stop 조건을 AMC랑 통일함
    // -------------------------------------------

	return Status;

}

/**
@brief   Obstacle 센서의 전원을 컨트롤 하는 함수
@author  임태웅
@date    2013.02.22
@param   nMode(Power On, Off, Reset, Enable)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
void Obstacle::SetArea(DETECT_AREA_TYPE Type) // 영역을 설정하는 함수
{

}


/**
@brief   Obstacle 센서의 전원을 컨트롤 하는 함수
@author  임태웅
@date    2013.02.22
@param   nMode(Power On, Off, Reset, Enable)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
double	Obstacle::GetMovableDist()			// 이동 가능한 거리를 받아오는 함수
{
 	return 0;
}


/**
@brief   Obstacle 센서의 Area를 획득하는 함수
@author  김경수
@date    2015.04.30
@param   
@return  Obstacle 센서의 Area
*/
int Obstacle::GetObsAreaStatus()
{
	int nValue = 0;

	nValue	= 	1 * m_pHWCommon->m_Output.OBSTACLE_Area1_Sel
			+	2 * m_pHWCommon->m_Output.OBSTACLE_Area2_Sel
			+	4 * m_pHWCommon->m_Output.OBSTACLE_Area3_Sel
			+	8 * m_pHWCommon->m_Output.OBSTACLE_Area4_Sel;

	return nValue;
}

/**
@brief   Obstacle 센서의 전원을 컨트롤 하는 함수
@author  LSJ
@date    2012.10.31
@param   nMode(Power On, Off, Reset, Enable)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Obstacle::PowerControl(int nMode)
{
    int nError = NO_ERR;

    switch (nMode)
    {
	case PWR_OFF:
		nError = this->Disable();
		break;
	case PWR_ON:
		nError = this->Enable();
        break;
    case PWR_RST:
//        nError = m_pHWCommon->SetBit(OUT_OBSTACLE_POWER_RESET, ON);
		nError = this->Disable();

		if (nError == NO_ERR)
		{
			Sleep(500);
//            nError = m_pHWCommon->SetBit(OUT_OBSTACLE_POWER_RESET, OFF);
			nError = this->Enable();
		}
    case PWR_ENABLE:
        nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA1_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA2_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA3_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA4_SEL, OFF);
        break;
    }
    return nError;
}

/**
@brief   Obstacle 센서가 알람 발생했을 때 전원을 리셋하는 함수
@author  LSJ
@date    2012.10.31
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Obstacle::ErrorReset()
{
    int nError = NO_ERR;

    if (m_pHWCommon->m_Input.OBSTACLE_Fail == ON)
    {
//        nError = m_pHWCommon->SetBit(OUT_OBSTACLE_POWER_RESET, ON);
		nError = this->Disable();
		if (nError == NO_ERR)
		{
			Sleep(500);
//			nError = m_pHWCommon->SetBit(OUT_OBSTACLE_POWER_RESET, OFF);
            nError = this->Enable();
		}
    }

    return nError;
}

/**
@brief   Obstacle 센서의 Area를 비트 조합을 통해 선택하는 함수
@author  LSJ
@date    2012.10.31
@param   nArea(비트 조합을 통해 총 16가지의 Area를 선택할 수 있음.)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Obstacle::AreaSelect(int nArea)
{
    int nError = NO_ERR;
    int i = 0, nObsOut[4] = {0, };

    for (i = 0; i < 4; i++)
	{
#if(PBS_TYPE == SICK_PBS)
		nObsOut[i] = ((nArea >> i) & 1) ? 1 : 0;
#else
		nObsOut[i] = ((nArea >> i) & 1) ? 0 : 1;
#endif
	}

	nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA1_SEL, nObsOut[0], true);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA2_SEL, nObsOut[1], true);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA3_SEL, nObsOut[2], true);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA4_SEL, nObsOut[3], true);

    return nError;
}

/**
@brief   Obstacle 센서의 어떤 단계가 감지 되었는지 확인하는 함수
@author  LSJ
@date    2012.10.31
@param   nLevel(Long, Middle, Short.)
@return  감지가 되어 있으면 true, 감지되지 않으면 false Return.
*/
bool Obstacle::IsDetect(int nLevel)
{
    bool bResult = false;

    switch (nLevel)
    {
    case DEF_LONG:
        if (m_pHWCommon->m_Input.OBSTACLE_Long == ON)
            bResult = true;
        else
            bResult = false;
        break;
	case DEF_MIDDLE:
		if (m_pHWCommon->m_Input.OBSTACLE_Middle == ON)
            bResult = true;
        else
            bResult = false;
        break;
	case DEF_SHORT:
		if (m_pHWCommon->m_Input.OBSTACLE_Short == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case DEF_FAIL:
		if (m_pHWCommon->m_Input.OBSTACLE_Fail == ON)
			bResult = true;
		else
			bResult = false;

    }

    return bResult;
}



bool Obstacle::IsRegion(int nLevel)
{
	bool bResult = false;

    switch (nLevel)
    {
	case AREA_SEL1:
		if (m_pHWCommon->m_Output.OBSTACLE_Area1_Sel == ON)
            bResult = true;
        else
            bResult = false;
        break;
	case AREA_SEL2:
		if (m_pHWCommon->m_Output.OBSTACLE_Area2_Sel == ON)
			bResult = true;
		else
            bResult = false;
        break;
	case AREA_SEL3:
		if (m_pHWCommon->m_Output.OBSTACLE_Area3_Sel == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case AREA_SEL4:
		if (m_pHWCommon->m_Output.OBSTACLE_Area4_Sel == ON)
			bResult = true;
		else
			bResult = false;
		break;
    }

    return bResult;
}
/**
@brief   Obstacle 센서가 알람 상태인지 확인하는 함수
@author  LSJ
@date    2012.10.31
@return  Fail 신호가 Off이면 0, ON이면 에러 코드 Return.
*/
bool Obstacle::CheckError()
{
    int nError = NO_ERR;
#if((OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
	if(OHTMainForm->CheckBox_ignoreOBS->Checked == true)
	{
		return DETECT_NONE;
	}
#endif
    if (m_pHWCommon->m_Input.OBSTACLE_Fail == ON)
        nError = ERR_OBS_SENSOR_FAIL;

    return nError;
}

/**
@brief   센서 가동시간을 측정
@author  doori.shin
@date    2016.04.07
@return  센서가 Enable되어있던 시간의 누적합을 리턴

int Obstacle::GetEnableTime(bool bInit)
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
@brief   OBS 센서의 Level을 얻어오는 함수
@author  soming
@date    2018.10.16
@param   bLong : Long 입력 On/Off 상태, bMiddle : Middle 입력 On/Off 상태, bShort : Short 입력 On/Off 상태, bFail : Fail 입력 On/Off 상태
@return  Obstacle 센서 Level을 리턴

int Obstacle::GetSensorLevel(bool bLong, bool bMiddle, bool bShort, bool bFail)
{
	int nLeveld = 0;

	if (bFail == ON)
		nLeveld = -1; // fail
	else if ((bLong == OFF) && (bMiddle == ON) && (bShort == ON))
		nLeveld = 7; // OBS 3.3m/s
	else if ((bLong == OFF) && (bMiddle == OFF) && (bShort == ON))
		nLeveld = 5; // OBS 1.0m/s
	else if ((bLong == OFF) && (bMiddle == OFF) && (bShort == OFF))
		nLeveld = 3; // OBS STOP

	else
		nLeveld = -1; // Fail ==>재확인필요(ON ON OFF)
	return nLeveld;
}
*/

int Obstacle::GetSensorLevel_MCC()
{
	// Long Mid Shor 순서도
	// 000 -> 100 -> 110 -> 010 -> 011 -> 111 -> 101
	//	8  -> 7   -> 6   -> 5    -> 4   -> 3  ->  2  (로 하기로 하자)

	bool	bFail	= 	m_pHWCommon->m_Input.OHTDETECT_Fail;
	bool	bLong	=  	m_pHWCommon->m_Input.OBSTACLE_Long;
	bool	bMiddle	=  	m_pHWCommon->m_Input.OBSTACLE_Middle;
	bool	bShort	= 	m_pHWCommon->m_Input.OBSTACLE_Short;

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
@brief   OBS 센서의 Output Level을 얻어오는 함수
@author  soming
@date    2018.10.16
@param   bLong : Long 입력 On/Off 상태, bMiddle : Middle 입력 On/Off 상태, bShort : Short 입력 On/Off 상태, bFail : Fail 입력 On/Off 상태
@return  OBS 센서 Output Level을 리턴


AnsiString Obstacle::GetOutputSensorLevel(bool bSel1, bool bSel2, bool bSel3, bool bSel4, bool bPower)
{
	AnsiString strLevel="";


	if (bPower == ON)
		strLevel = "PO"; // Power OFF
	else if ((bSel1 == OFF) && (bSel2 == ON) && (bSel3 == ON) && (bSel4 == ON))
		strLevel = "NN"; // OBS Normal 명령

	else if ((bSel1 == ON) && (bSel2 == OFF) && (bSel3 == ON) && (bSel4 == ON))
		strLevel = "LL"; // OBS Left 명령

	else if ((bSel1 == OFF) && (bSel2 == OFF) && (bSel3 == ON) && (bSel4 == ON))
		strLevel = "RR"; // OBS Right 명령

	else if ((bSel1 == ON) && (bSel2 == ON) && (bSel3 == OFF) && (bSel4 == ON))
		strLevel = "NS"; // OBS N-Diver 명령

	else if ((bSel1 == OFF) && (bSel2 == ON) && (bSel3 == OFF) && (bSel4 == ON))
		strLevel = "BO"; //OBS BOTH 명령

	else if ((bSel1 == ON) && (bSel2 == OFF) && (bSel3 == OFF) && (bSel4 == ON))
		strLevel = "VO"; // OBS Narrow 명령
	else if ((bSel1 == OFF) && (bSel2 == OFF) && (bSel3 == OFF) && (bSel4 == ON))
		strLevel = "SL"; // OBS STB_L 명령

	else if ((bSel1 == ON) && (bSel2 == ON) && (bSel3 == ON) && (bSel4 == OFF))
		strLevel = "SR"; // OBS STB_R 명령

	else if ((bSel1 == OFF) && (bSel2 == ON) && (bSel3 == ON) && (bSel4 == OFF))
		strLevel = "NR"; // OBS N_R 명령

	else if ((bSel1 == ON) && (bSel2 == OFF) && (bSel3 == ON) && (bSel4 == OFF))
		strLevel = "NL"; // OBS N_L 명령

	else if ((bSel1 == OFF) && (bSel2 == OFF) && (bSel3 == ON) && (bSel4 == OFF))
		strLevel = "UL"; // OBS U_L 명령

	else if ((bSel1 == ON) && (bSel2 == ON) && (bSel3 == OFF) && (bSel4 == OFF))
		strLevel = "UR"; // OBS U_R 명령

	else if ((bSel1 == OFF) && (bSel2 == ON) && (bSel3 == OFF) && (bSel4 == OFF))
		strLevel = "BA"; // OBS BAY 명령

	else
		strLevel = "FA"; // Fail ==>재확인필요(ON ON OFF)

	return strLevel;
}
*/

bool Obstacle::IsConnect()
{
	if(m_pCommPort!=NULL)
	return true;
	else
	return false;
}
 int Obstacle::CheckOBSConnect(bool bConnect)
{
	int nError = NO_ERR;

	if(bConnect)
	{
		if(IsConnect() == false)
		   {
				OpenCom();

				if(IsConnect() == false)
				{

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

				if(IsConnect() == true)
				{
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
// ---------------------------------------------------------------------------
// OBS Setting Code
int Obstacle::OpenCom(void)
{
    int nError = 0;

    if(m_pCommPort == NULL)
	{
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
        m_pCommPort = new Comm_Port(m_nComPortNum, m_nBaudRate);
#else
		m_pCommPort = new Comm_Port(m_nComPortNum, m_nBaudRate, 8 ,NOPARITY, 1);
#endif


        if(m_pCommPort == NULL)
        {
            OBS_LOG("CommPort Open Fail");
            bConnect = false;
        }
        else
        {
            OBS_LOG("CommPort Open Success");
            bConnect = true;
        }
    }
    else
    {
        OBS_LOG("CommPort is Opened");
        bConnect = true;     //AOHC-288
    }

    return nError;
}

int Obstacle::CloseCom(void)
{
    int nError = 0;
	if(m_pCommPort!=NULL)
	{
		delete (Comm_Port *) m_pCommPort;
		m_pCommPort = NULL;
        OBS_LOG("CommPort Close Success");
	}
    else
    {
        OBS_LOG("CommPort Closed before");
    }
	bConnect = false;
	return nError;
}

void Obstacle::RequestData(void)
{
    unsigned char Request_Data[OBS_Command_Packet_Size] = {0x24, 0x47, 0x0D, 0x0A};
    OBS_LOG("[request] first");
    if(bConnect == false)
    {
        OBS_LOG("[request]OpenCom try");
		OpenCom();
        if(bConnect == false)
            return;
    }

    if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

    if(m_pCommPort->WriteCommBlock(Request_Data, OBS_Command_Packet_Size) != OBS_Command_Packet_Size)
    {
        OBS_LOG("OBS RequestData Send Fail");
    }
}

int Obstacle::ReceiveData(char *ReadBuff)    //AOHC-288
{
    int dwLen = 0;
	int nReadBuffLength = 0;
	unsigned char szReadBuff[2048] = {0,};

	if((m_pCommPort==NULL) || (bConnect==false))
	{
		OBS_LOG("[532]CommPort is Closed");
    }
    else
    {
        // 0x0A 4번 수신될 때 까지
	    dwLen = m_pCommPort->ReadUntilCountOHTDetect2(szReadBuff,OBS_Final_Flag,OBS_Target);

	    if(dwLen>0)
	    {
		    memcpy(ReadBuff, szReadBuff, OBS_ReceiveDataSize);
            OBS_LOG("[542]OBS Data Received: %d", dwLen);

    	}
	    else
	    {
            OBS_LOG("[547]OBS Data Receive Fail");
            dwLen = 0;
	    }

        if( m_pCommPort != NULL)
		    PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    }
    //return nError;
    return dwLen;
}

int Obstacle::CheckObsSensorData(void)
{
    int nError = NO_ERR;
    int nReceiveDataSize = 0;
    int nSensorData = 0;
    int nStart_Pos = 0;
    int nArea = 0;
    unsigned char Receive_Data[2048] = {0, };

    nReceiveDataSize = ReceiveData(Receive_Data);

    if(nReceiveDataSize == OBS_ReceiveDataSize)
    {
        if((Receive_Data[0] == OBS_Start_Flag_1) && (Receive_Data[1] == OBS_Start_Flag_2))
        {
            nArea = GetObsAreaStatus(); // pattern 정보 필요 없으면 제거
            Receive_Data[nReceiveDataSize-1] = nArea/*현재 패턴*/+ASCII_ZERO/*0x30*/;
            Receive_Data[nReceiveDataSize] = NULL;
            OBS_AREA_LOG("%s", Receive_Data);
            nError = nReceiveDataSize;
        }
    }
    else
    {
        nError = -1;
		OBS_LOG("OBS Receive Data Size is not Correct. RecvSize=%d, Expect=%d", nReceiveDataSize, OBS_ReceiveDataSize);    //AOHC-288
    }
    CloseCom();
    return nError;
}

int Obstacle::CheckObsSensorData(int *Buff)
{
    int nError = NO_ERR;
    int nReceiveDataSize = 0;
    int nSensorData = 0;
    int nStart_Pos = 0;
    unsigned char Receive_Data[2048] = {0, };
    AnsiString strLog, strTmp;

    nReceiveDataSize = ReceiveData(Receive_Data);

    // test code
    /*
    for(int i = 0; i < OBS_Sensor_Data_Num; i++)
    {
        Buff[i] = Random(4000);
    }
    return 0;
    */

    if(nReceiveDataSize == OBS_ReceiveDataSize)
    {
        if((Receive_Data[0] == OBS_Start_Flag_1) && (Receive_Data[1] == OBS_Start_Flag_2))
        {
            strLog.sprintf("%02d", GetObsAreaStatus());
            OBS_LOG("OBS Sensor Area : %s", strLog);
            strLog = "";

            for(int i = 0; i < OBS_Sensor_Data_Num; i++)
            {
                nStart_Pos = 3 * i + OBS_Sensor_Data_StartPacketSize;
                //Buff[OBS_Sensor_Data_Num-i-1] = Decode(nStart_Pos, Receive_Data, OBS_Sensor_Data);
                Buff[i] = Decode(nStart_Pos, Receive_Data, OBS_Sensor_Data);

                /*
                strTmp.sprintf("/%05d", Buff[i]);
                strLog = strLog + strTmp;

                if(i == 100 || i == 200 || i == 300 || i == 400 || i == 512)
                {
                    OBS_LOG("OBS Data[%d] : %s", i, strLog);
                    strLog = "";
                }
                */
            }
        }
    }
    else
    {
        nError = -1;
        OBS_LOG("OBS Receive Data Size is not Correct");
        /*
        if(nReceiveDataSize <= OBS_ReceiveDataSize)
        {
            for(int j = 0; j < nReceiveDataSize; j++)
            {
                strTmp.sprintf("%x", Receive_Data[j]);
                strLog = strLog + strTmp;
            }
            OBS_LOG("OBS Data : %s", strLog);
        }
        else
        {
            OBS_LOG("OBS Data size is big");
        }
        */
    }

    return nError;
}



int Obstacle::Decode(int start, char *code, int byte)
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

int Obstacle::UpdataData(int *Buff, bool bIsStart)
{
    static DWORD dwOBSTrigerTime = timeGetTime();
    DWORD dwStartTime = timeGetTime();
    int nError = NO_ERR;
    static int Pre_Buff[OBS_Sensor_Data_Num] = {0,};

    if(bIsStart)
    {
        RequestData();
        dwOBSTrigerTime = timeGetTime();
    }
    else
    {
        // 센서 최소 응답 시간 : 320ms
		//if(dwStartTime - dwOBSTrigerTime > OBS_Sensor_Request_Data_Interval)
        if(OHTUTILITY::PreventOverflowGetTimeMs(dwOBSTrigerTime) > OBS_Sensor_Request_Data_Interval)
        {
            // Receive
            nError = CheckObsSensorData(Buff);

            // Send
            if(nError == NO_ERR)
            {
                RequestData();
                for(int i=0; i<OBS_Sensor_Data_Num; i++)
                {
                    Pre_Buff[i] = Buff[i];
                }
            }
            else
            {
				RequestData();
                OBS_LOG("error");
            }

            dwOBSTrigerTime = timeGetTime();
        }
        else
        {
            for(int i=0; i<OBS_Sensor_Data_Num; i++)
            {
                Buff[i] = Pre_Buff[i];
            }
        }
    }
    return nError;
}
// ---------------------------------------------------------------------------

void Obstacle::DataReadStart(int *Buff)
{
	OpenCom();
	if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	UpdataData(Buff, true);
}



//AMOHTV80F-952 Frame Data Stream Enable/Disalbe 함수
int Obstacle::SOSLABFrameDataEnableCommand(bool bEnable)
{
	int nError = NO_ERR;
	int nLen = 0;
	unsigned char crc_value ;
														  //Ps1, Ps2, P23, Ps4, TL_L TL_H  PI  PL   SM   BI  CAT0  CAT1 DTn
	unsigned char header_dataEnable[SOS_OBS_Command_Data_Size] =  {0xC3,0x51,0xA1,0xF8,0x00,0x00,0x00,0x01,0x00,0x21,0x01,0x03,0x01/*area index*/,0xC2};
	unsigned char header_dataDisable[SOS_OBS_Command_Data_Size] =  {0xC3,0x51,0xA1,0xF8,0x00,0x00,0x00,0x01,0x00,0x21,0x01,0x03,0x00/*area index*/,0xC2};
	unsigned char command_data[SOS_OBS_Command_Data_Size] = {0,};
	unsigned char szReadBuff[SOS_OBS_Command_Data_Size] = {0,};

	if( m_pCommPort == NULL){ return -1;}

	if(bEnable == true)
	{
		crc_value = SOS_CheckSum(header_dataEnable,SOS_OBS_Command_Data_Size-1 );
		memcpy(command_data,header_dataEnable, SOS_OBS_Command_Data_Size-1);
	}
	else
	{
		crc_value = SOS_CheckSum(header_dataDisable,SOS_OBS_Command_Data_Size-1 );
		memcpy(command_data,header_dataDisable, SOS_OBS_Command_Data_Size-1);
    }

	command_data[SOS_OBS_Command_Data_Size-1] = crc_value;

	nLen = m_pCommPort->WriteCommBlock(command_data,SOS_OBS_Command_Data_Size);
	if(nLen != SOS_OBS_Command_Data_Size)
	{
	    OBS_LOG("SOSLABFrameDataEnableCommand_OBS WriteCommBlock : %d/", nLen);
		nError = -1;
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
			if(szReadBuff[SOS_OBS_CHECK_FRAMEENABLE] != 0x01)
			{
				OBS_LOG("SOSLABFrameDataEnableCommand_OBS ReadUntilCount_SOS_AreaIndex : %x/", szReadBuff[SOS_OBS_CHECK_FRAMEENABLE]);
				nError =  -1;
            }
	    }
	}

	if(nError == NO_ERR)
	{

		if(bEnable == true) m_bSOSLABFrameFlagOBS = true;
		else m_bSOSLABFrameFlagOBS = false;
    }
    memset(&m_SoslabFramData, 0x00, sizeof(SOSLAB_FRAMEDATA));
    OBS_LOG("SOSLABFrameDataEnableCommand_OBS : %d/%d/%d",nError,m_bSOSLABFrameFlagOBS, nLen);

	return nError;
}
//AMOHTV80F-952 Frame Data Stream에서 데이터 확인 함수  ... puting
int Obstacle::SOSLABFrameDataGetCommand()//(unsigned char *ReadBuff)     // ReadBuff size : SOS_UBG_Frame_Data_Size
{
  //주기적으로 받아야됨.
  	int nError = NO_ERR;
	DWORD dwLen = 0;
	unsigned char szReadBuff[SOS_OBS_Frame_Data_Size] = {0,};

	if(m_pCommPort==NULL)
	{
		return 0;
	}

	dwLen = m_pCommPort->ReadUntilCount_SOS_GetFrame(szReadBuff, SOS_OBS_Frame_Data_Size);

  	OBS_LOG("SOSLABFrameDataGetCommand()_OBS size : %d, %x %x %x %x /%x %x",dwLen,szReadBuff[0],szReadBuff[1],szReadBuff[2],szReadBuff[3],szReadBuff[SOS_OBS_Frame_Data_Size-1],szReadBuff[SOS_OBS_Frame_Data_Size-2]);
	if(dwLen == SOS_OBS_Frame_Data_Size)
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
bool Obstacle::GetSOSLABFramEnableFlag()
{
    return  m_bSOSLABFrameFlagOBS;
}

unsigned char Obstacle::SOS_CheckSum(unsigned char *buf, int Length)
{
	unsigned char reValue = 0;

	for(int i =0; i< Length;i++)
	{
	  reValue = reValue ^ buf[i];
	}

	return reValue;
}

#pragma package(smart_init)
