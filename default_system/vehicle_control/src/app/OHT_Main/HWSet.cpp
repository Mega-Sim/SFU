/**
@file	 HWSet.cpp
@version OHT 7.0
@brief	 HWSet Class CPP File
*/

#include "HWSet.h"
#include "OHTMain.h"

/**
@brief	 HWSet Class 생성자 함수
@author  LSJ
@date	 2012.11.07
@param	 pHWCommon : HWCommon 클래스 포인터
*/
HWSet::HWSet()
{
	m_pHWCommon = OHTMainForm->GetHWCommon();
	m_defualtparam = OHTMainForm->GetDefaultParam();

	CreateHWSet();
	nIs_SounON = SETSOUND_NONE;
	m_AMCResponse = 0x00;
}

/**
@brief	 HWSet Class 소멸자 함수
@author  LSJ
@date	 2012.11.07
*/
HWSet::~HWSet()
{
	DeleteHWSet();
}

/**
@brief	 OHT Hardware 관련된 클래스에 대한 생성
@author  LSJ
@date	 2012.11.07
*/
void HWSet::CreateHWSet()
{
	m_pTransBCR				= new BCR(m_pHWCommon, TRANS_SIDE);
	m_pDrivingBCR			= new BCR(m_pHWCommon, DRIVING_SIDE);
	m_pBumper				= new Bumper(m_pHWCommon);

	m_pDrivingAxis			= new Driving(m_pHWCommon, 0, 1, m_defualtparam->DrivingDefault.dIdealGearRatio);  //113.3736777 주행 기본 기어비

	m_pDrivingTag			= new DrivingTag(m_pHWCommon);
	m_pLeftEQPIO			= new EQPIO(m_pHWCommon, LEFT_SIDE);
	m_pFan					= new Fan(m_pHWCommon);

#if(OHT_NAME != OHT_NAME_P4_MASK)
	m_pHand					= new Hand(m_pHWCommon);
#else
	m_pFfu					= new Ffu(m_pHWCommon);
	m_pHand					= new HandRMR(m_pHWCommon);
	m_pHWCommon->SetBit(OUT_HAND_PIO_SEL, ON);
#endif

	m_pHoistAxis			= new Hoist(m_pHWCommon, 2);
	m_pInnerFoup			= new InnerFoup(m_pHWCommon);
	m_pLamp					= new Lamp(m_pHWCommon);
	m_pLookdown				= new Lookdown(m_pHWCommon);
	m_pObstacle				= new Obstacle(m_pHWCommon);
	m_pOcsillation			= new Ocsillation(m_pHWCommon);

#if(((OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)) && (SAMSUNG_COMMON_SPEC==ON))
	m_pFoupCoverOpenDetect	= new FoupCoverOpenDetect(m_pHWCommon);
#endif
	
	m_pOHTDetect			= new OHTDetect(m_pHWCommon);
	m_pPowerControl			= new PowerControl(m_pHWCommon);

	m_pLeftQRR				= new QR_Reader(m_pHWCommon, COM_LEFT_QRR, true);
	m_pRightQRR 			= new QR_Reader(m_pHWCommon, COM_RIGHT_QRR, false);
//#else
//	m_pLeftQRR				= new QR_Reader(m_pHWCommon, COM_LEFT_QRR, false);
//	m_pRightQRR				= new QR_Reader(m_pHWCommon, COM_RIGHT_QRR, true);
//#endif	
//	m_pRailSupport			= new RailSupport(m_pHWCommon);

	if(m_defualtparam->bUseVisionTeaching == true)
	{
		m_pLeftSTBQRR			= new STB_Reader(m_pHWCommon, COM_LCD, true);
		m_pLeftSTBQRR->SendTriggerOn();

		m_pRightSTBQRR 			= new STB_Reader(m_pHWCommon, COM_HANDTP, false);
		m_pRightSTBQRR->SendTriggerOn();
	}
	else
	{
		m_pRemocon				= new Remocon(m_pHWCommon);
		m_pLCD					= new LCD(m_pHWCommon);
	}

// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate Class 생성
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))
	m_pRotate               = new Rotate(m_pHWCommon);
#endif

#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	if(m_defualtparam->bUseRSA==true)
		m_pRSAMonitoring        = new RSAMonitoring(m_pHWCommon);
	else
		m_pServoMonitoring		= new ServoMonitoring(m_pHWCommon);
#endif

	m_pShiftAxis			= new Shift(m_pHWCommon, 3, m_defualtparam->ShiftDefault.dIdealGearRatio);   // 기본 기어비
	m_pShutter				= new Shutter(m_pHWCommon);
	m_pSteer				= new Steer(m_pHWCommon);
	m_pTrackBufferDetect	= new TrackBufferDetect(m_pHWCommon);
	m_pTransStopTag			= new TransStopTag(m_pHWCommon);
	m_pPassBoxPIO			= new PassBoxPIO(m_pHWCommon);
	m_pCID                  = new CID(m_pHWCommon);
	m_pMTLPIO               = new MTLPIO(m_pHWCommon);
	// 제품들 활성화
	m_pDrivingBCR->Enable();
	m_pTransBCR->Enable();

#if (((OHT_NAME == OHT_NAME_STD_V80)  || (OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V85S)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86) || (OHT_NAME == OHT_NAME_P4_MASK)) && (SAMSUNG_COMMON_SPEC==ON))
	m_pPowerControl->ResetController();
#else
	m_pPowerControl->MCControl(true);
#endif

	m_pSteer->Adjust();
//	m_pRailSupport->Adjust();

	//	m_pDrivingBCR->Open();
}

/**
@brief	 OHT Hardware 관련된 클래스에 대한 소멸
@author  LSJ
@date	 2012.11.07
*/
void HWSet::DeleteHWSet()
{
	delete m_pTransBCR;
	m_pTransBCR = NULL;
	delete m_pDrivingBCR;
	m_pDrivingBCR = NULL;
	delete m_pBumper;
	m_pBumper = NULL;
	delete m_pDrivingAxis;
	m_pDrivingAxis = NULL;
	delete m_pDrivingTag;
	m_pDrivingTag = NULL;
	delete m_pLeftEQPIO;
	m_pLeftEQPIO = NULL;
#if(OHT_NAME == OHT_NAME_P4_MASK)
	delete m_pFfu;
	m_pFfu = NULL;
#endif
	delete m_pFan;
	m_pFan = NULL;
	delete m_pHand;
	m_pHand = NULL;

//	Sleep(2000);

	delete m_pHoistAxis;	//<-- 순수 가상 함수 호출 
	m_pHoistAxis = NULL;

//	Sleep(2000);
	
	delete m_pInnerFoup;
	m_pInnerFoup = NULL;
	delete m_pLamp;
	m_pLamp = NULL;
	delete m_pLookdown;
	m_pLookdown = NULL;
	delete m_pObstacle;
	m_pObstacle = NULL;
	delete m_pOcsillation;
	m_pOcsillation = NULL;
	delete m_pOHTDetect;
	m_pOHTDetect = NULL;
#if (((OHT_NAME == OHT_NAME_STD_V80) ||  (OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)) && (SAMSUNG_COMMON_SPEC==ON))
	delete m_pFoupCoverOpenDetect;
	m_pFoupCoverOpenDetect = NULL;
#endif
	delete m_pPowerControl;
	m_pPowerControl = NULL;
	delete m_pLeftQRR;
	m_pLeftQRR = NULL;
	delete m_pRightQRR;
	m_pRightQRR = NULL;
//	delete m_pRailSupport;
//	m_pRailSupport = NULL;

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
	delete m_pRotate;
	m_pRotate = NULL;
#endif

#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	if(m_defualtparam->bUseRSA==true)
	{
		delete m_pRSAMonitoring;
		m_pRSAMonitoring = NULL;
	}
	else
	{
		delete m_pServoMonitoring;
		m_pServoMonitoring = NULL;
	}
#endif

	delete m_pShiftAxis;	//<-- 순수 가상 함수 호출 
	m_pShiftAxis = NULL;

	delete m_pShutter;
	m_pShutter = NULL;
	delete m_pSteer;
	m_pSteer = NULL;
	delete m_pTrackBufferDetect;
	m_pTrackBufferDetect = NULL;
	delete m_pTransStopTag;
	m_pTransStopTag = NULL;

	delete m_pPassBoxPIO;
	m_pPassBoxPIO = NULL;
  	delete m_pCID;
	m_pCID = NULL;
	delete m_pMTLPIO;
	m_pMTLPIO = NULL;
	
	if(m_defualtparam->bUseVisionTeaching == true)
	{
		delete m_pLeftSTBQRR;
		m_pLeftSTBQRR = NULL;

		delete m_pRightSTBQRR;
		m_pRightSTBQRR = NULL;
	}
	else
	{
        delete m_pRemocon;
		m_pRemocon = NULL;

		delete m_pLCD;
		m_pLCD = NULL;
    }
	
}

/**
@brief	 OHT의 Sound를 켜는 함수
@author  LSJ
@date	 2012.11.07
@param	 nKindOfSound(SETSOUND_ERROR, SETSOUND_DRIVING, SETSOUND_TRANS, SETSOUND_OBS, SETSOUND_DETECT, SETSOUND_PAUSE, SETSOUND_OBSWARNNING)
*/
void HWSet::Sound_On(int nKindOfSound)
{
   String strOHTPath = "", strTmpFileName = "";
   bool bRtnValue = false;   //AOHT-0002_SOUND

   if(nIs_SounON==nKindOfSound) return;
	if(nKindOfSound == SETSOUND_NONE) return;

   Sound_Off();

   strOHTPath = ExtractFileDir(Application->ExeName);
   strTmpFileName = String(strOHTPath+STR_WAVE_PATH);
   //AOHT-0002_SOUND
   switch (nKindOfSound)
   {
	  case SETSOUND_ERROR :
		 strTmpFileName += AnsiString(NAME_WAV_ERROR);
		 bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 if(bRtnValue==false) bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 nIs_SounON=SETSOUND_ERROR;
		 break;
	  case SETSOUND_DRIVING :
		 strTmpFileName += AnsiString(NAME_WAV_DRIVING);
		 bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 if(bRtnValue==false) bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 nIs_SounON=SETSOUND_DRIVING;
		 break;
	  case SETSOUND_TRANS :
		 strTmpFileName += AnsiString(NAME_WAV_TRANS);
		 bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 if(bRtnValue==false) bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 nIs_SounON=SETSOUND_TRANS;
		 break;
	  case SETSOUND_OBS :
		 strTmpFileName += AnsiString(NAME_WAV_OBS);
		 bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 if(bRtnValue==false) bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 nIs_SounON=SETSOUND_OBS;
		 break;
	  case SETSOUND_DETECT :
		 strTmpFileName += AnsiString(NAME_WAV_DETECT);
		 bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 if(bRtnValue==false) bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 nIs_SounON=SETSOUND_DETECT;
		 break;
	  case SETSOUND_PAUSE :
		 strTmpFileName += AnsiString(NAME_WAV_PAUSE);
		 bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 if(bRtnValue==false) bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 nIs_SounON=SETSOUND_PAUSE;
		 break;
	  case SETSOUND_OBSWARNNING :
		 strTmpFileName += AnsiString(NAME_WAV_OBSWARNNING);
		 bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 if(bRtnValue==false) bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 nIs_SounON=SETSOUND_OBSWARNNING;
		 break;
	  //20190512
	  case SETSOUND_LOOKDOWN :
		 strTmpFileName += AnsiString(NAME_WAV_LOOKDOWN);
		 bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 if(bRtnValue==false) bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 nIs_SounON=SETSOUND_LOOKDOWN;
		 break;
	  case SETSOUND_OSCILLATION :
		 strTmpFileName += AnsiString(NAME_WAV_OSCILLATION);
		 bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 if(bRtnValue==false) bRtnValue = sndPlaySound(strTmpFileName.c_str(), SND_ASYNC|SND_LOOP);
		 nIs_SounON=SETSOUND_OSCILLATION;
		 break;
   }

}

/**
@brief	 OHT의 Sound를 끄는 함수
@author  LSJ
@date	 2012.11.07
*/
void HWSet::Sound_Off()
{
   bool bRtnValue = false;

   bRtnValue = sndPlaySound(NULL, SND_ASYNC);
   if(bRtnValue==false) bRtnValue = sndPlaySound(NULL, SND_ASYNC);
   nIs_SounON=SETSOUND_NONE;

}

// 통신으로 제어시에서 구현 시 사용 함수
/**
@brief	 HWSet I/O의 정보 획득
@author  ehoto
@date	 2012-11-19
@param	pInputBuff: Input 받는 버퍼
@param	nInputCount: Input 받는 버퍼 개수(int 단위)
@param	pOutputBuff: Output 받는 버퍼
@param	nOutCount: Output 받는 버퍼 개수(int 단위)
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int HWSet::GetIOSet(int* pInputBuff, int& nInputCount, int* pOutputBuff, int& nOutCount)
{
	return m_pHWCommon->GetIOSet(pInputBuff, nInputCount, pOutputBuff, nOutCount);
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 버전을 얻어오는 함수
@author  zzang9un
@date    2013.10.07
@return  AMC의 버전 정보(int)를 리턴
*/
int HWSet::GetAMCVersion()
{
    return m_pHWCommon->GetAMCVersion();
}
int HWSet::GetAMCDLLVersion()
{
	return m_pHWCommon->GetAMCDLLVersion();
}

int HWSet::GetFPGAVersion()
{
	return m_pHWCommon->GetFPGAVersion();
}
// AMOHTV80F-1703
int HWSet::GetRSAnSlaveVersion(int num)
{
	return m_pHWCommon->GetRSAnSlaveVersion(num);
}

//---------------------------------------------------------------------------

int HWSet::GetVolatageMonitor(int Index)
{
	return m_pHWCommon->GetVolatageMonitor(Index);
}

int HWSet::GetAMCPauseStatus()
{
    return m_pHWCommon->GetAMCPauseStatus();
}
int HWSet::GetAMCTorqueStatus()
{
	return m_pHWCommon->GetAMCTorqueStatus();
}

int HWSet::GetAMCVHLDetectStatus()
{
	return m_pHWCommon->GetAMCVHLDetectStatus();
}

int HWSet::GetAMCStopStatus()
{
    return m_pHWCommon->GetAMCStopStatus();
}

int HWSet::GetAMCDecelStatus()
{
	return m_pHWCommon->GetAMCDecelStatus();
}

int HWSet::GetAMCDetectStopDecelDownStatus()
{
	return m_pHWCommon->GetAMCDetectStopDecelDownStatus();
}

int HWSet::GetVHLDetectForceIOStatus()
{
	return m_pHWCommon->GetVHLDetectForceIOStatus();
}
//AOHC-285
int HWSet::GetLowVabraionStatus()
{
	return m_pHWCommon->GetLowVabraionStatus();
}
/**
@brief   TraceLog 작성하는 함수
@author  임태웅
@date    2013.10.22
*/
void HWSet::MakeTraceLog()
{
	m_pHWCommon->makeTraceLog();
}

int HWSet::SetSoundCommand(int nSound)
{
	m_pHWCommon->SetSoundCommand(nSound);
}
//---------------------------------------------------------------------------

