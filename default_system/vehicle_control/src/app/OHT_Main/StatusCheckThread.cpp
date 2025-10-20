/**
@file    StatusCheckThread.cpp
@version OHT 7.0
@brief   StatusCheckThread CPP File
*/
#include <math.h>
#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
//20.06.29 JHJ
#include <psapi.h>
#pragma link "Psapi.lib"
#include <dir.h>

#include "OHTMain.h"
#include "StatusCheckThread.h"
#include "NetworkAdapterControl.h"
#include "LogHandler.h" // Log 작성 목적
#include "MyTools.h"
// ---------------------------------------------------------------------------
#define ADD_LOG_EXTRAJOB(...)    WRITE_LOG_UNIT(pLogExtraJobThread, __VA_ARGS__)
#define LOG_USER(...)    WRITE_LOG_UNIT(pLogUser, __VA_ARGS__)
#define LOG_ERR_HID(...)     WRITE_LOG_UNIT(pLogErrHID, __VA_ARGS__)
#define CPUNMEM_LOG(...)     WRITE_LOG_UNIT(pLogCPUandMemInfo, __VA_ARGS__)
#define WirelessNetwork_LOG(...)     WRITE_LOG_UNIT(pLogWirelessNet, __VA_ARGS__)
//#define ADD_MCC_LOG(usemode, ...)      WRITE_LOG_UNIT(pLogMCC, __VA_ARGS__)
#define ADD_MCC_LOG(usemode, ...)							\
	if(usemode)												\
	{										        		\
		try{									    		\
			WRITE_LOG_UNIT(pLogMCC, __VA_ARGS__)    		\
		}                                           		\
		catch(...)                                  		\
		{                                           		\
			WRITE_LOG_UNIT(pLogMCC, "Status Exception")		\
		}                                           		\
	}
#define ADD_MD_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\MonitoringDetail.log", __VA_ARGS__) //AOHC-253 Monitoring 항목 추가 2
#define ADD_FDC_LOG(...)	writeFDCUnit(pFDCUnit, __VA_ARGS__)
#define SEND_WARN(...)		writeFDCUnit_WARN(pFDCUnit, __VA_ARGS__)
#define	FDC_Manager_Off_Network()			FDCManagerOff_()

#define STATUS_NORMAL					0
#define STATUS_OBS_STOP					1
#define STATUS_PBS_STOP					2
#define STATUS_OHT_PAUSED    			3

/**
@brief   StatusCheckThread 생성자
@author  zzang9un
@param   pHWSet : HWSet 클래스 포인터
@param   pOHTMainStatus : OHTMainStatus 클래스 포인터
*/
StatusCheckThread::StatusCheckThread() : ThreadCtrl()
{
	pLogExtraJobThread = getLogUnit("D:\\log\\ExtraJobThread.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogUser = getLogUnit("D:\\log\\User.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogErrHID = getLogUnit("D:\\log\\HIDError.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogCPUandMemInfo = getLogUnit("D:\\log\\CPUandMemInfo.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogWirelessNet = getLogUnit("D:\\log\\WirelessNet.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogMCC = getLogUnit("D:\\log\\MCC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

    m_pHWSet = OHTMainForm->GetHWSet();
    m_pTransControl = OHTMainForm->GetTransControl();

	m_OHTMainStatus = OHTMainForm->GetOHTStatus();
	m_ExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_NetworkDll = OHTMainForm->GetNetworkDll();

	m_pTaskControl = NULL;
    m_bPlayingSound = false;
	m_PlaySoundNum = SETSOUND_NONE;

	nCommfailCount = 0;
	m_OCSAPSignal = 0;   // AOHC-292

	m_CheckLampYF = false;
	m_TPESTOP = false;

	m_DiagManager = DiagManager::getInstance();

   	m_HID = new HID_VII_ZR(COM_HID);     //위치 옮기기
	m_IPCLoadLib = new IPCLibrary();
	//m_NetworkDll = new NetworkSinalLib();
	m_CPUandMemInfo = new CPUandMemInfo();

	if(m_NetworkDll != NULL)
		m_NetworkDll->NETWORKInit();

	m_defualtparam = OHTMainForm->GetDefaultParam();
	m_pAutoPatchParamSet =  OHTMainForm-> GetAutoPatchParam(); //AOHC-154
	m_LineParamSet = OHTMainForm->GetLineParam();  //AOHC-285
#if(WirelessClient_TYPE == FXE3000_WIRELESS_CLIENT || WirelessClient_TYPE == FXE2000_WIRELESS_CLIENT)
    memset(szWirelessClientIP, 0, 20);
    strcpy(szWirelessClientIP, m_defualtparam->WirelessClientIP);
    m_UdpClient = new UdpClient(szWirelessClientIP, WirelessClientStatusPort);
#endif

	m_PassPermit = OHTMainForm->GetPassPermit();
	m_nLCDVersion = LCD_PRE_VERSION;
	m_nLCDRetry = 0;
	//LCD
	if(m_defualtparam->IsLCDAndLED)
	{
		ComOpenForLCD();
#if((OHT_NAME == OHT_NAME_STD_V82)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
		LCDFirmwareVersionCheck();
#endif
	}

	m_DistanceSensorType = DISTANCE_SENSOR_DISABLE;
	m_mccparam.nMem = 0;
	m_mccparam.nCPU = 0;
	bUseAVS = m_defualtparam->bUseAVS;
	m_mccparam.nRSSI = 0;
	memset(m_mccparam.cClientMAC, 0, 18);
	bRegulatorCheck = m_defualtparam->bRegulatorCheck;

	m_mccparam.nLogTtime = 0;
	m_mccparam.nTaskTtime = 0;
	m_mccparam.nStatusTtime = 0;
	m_mccparam.nExcuteTtime = 0;
	m_mccparam.nExtraTtime = 0;
	m_mccparam.nAMCTtime = 0;
	m_mccparam.nHandPIOCount = 0;
	m_mccparam.nUseFDCCount = 0;

	m_mccparam.bAMCcomm = false;
	m_mccparam.nFanFail = 0;

	memset(m_mccparam.cCarrierType, 0, 10);
	sprintf(m_mccparam.cCarrierType,"None");

	memset(m_mccparam.cOHTType, 0, 10);
#if(OHT_NAME == OHT_NAME_STD_V82)
#if(USE_MAC == 0)
	sprintf(m_mccparam.cOHTType,"V82_FOHT");
#else
	sprintf(m_mccparam.cOHTType,"V82_MOHT");
#endif
#elif(OHT_NAME == OHT_NAME_STD_V86)
	sprintf(m_mccparam.cOHTType,"V86_ROHT");
#elif(OHT_NAME == OHT_NAME_STD_V85S)
	sprintf(m_mccparam.cOHTType,"V85S_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
	sprintf(m_mccparam.cOHTType,"V80_ROHT");
#elif(OHT_NAME == OHT_NAME_STD_V80)
	sprintf(m_mccparam.cOHTType,"V80_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V81)
	sprintf(m_mccparam.cOHTType,"V81_FOHT");
	if(m_defualtparam->bSOM6869)
	{
		memset(m_mccparam.cOHTType, 0, 10);
		sprintf(m_mccparam.cOHTType, "V81A_FOHT");
	}
#elif(OHT_NAME == OHT_NAME_STD_V81S)
	sprintf(m_mccparam.cOHTType,"V81S_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V85)
	sprintf(m_mccparam.cOHTType,"V85_ROHT");
	if(m_defualtparam->bSOM6869)
	{
		memset(m_mccparam.cOHTType, 0, 10);
		sprintf(m_mccparam.cOHTType, "V85A_ROHT");
	}
#else
	sprintf(m_mccparam.cOHTType,"Unknown");
#endif

	m_mccparam.fAxis1Torque = 0.0;
	m_mccparam.fAxis2Torque = 0.0;
	m_mccparam.fAxis3Torque = 0.0;
	m_mccparam.fAxis4Torque = 0.0;

	m_mccparam.dDrivingFollowingError = 0.0;
	m_mccparam.dHoistFollowingError = 0.0;
	m_mccparam.dShiftFollowingError = 0.0;

	m_mccparam.dDrivingMaxPos = 0.0;

	m_mccparam.nPathSteering = 0;

	memset(m_mccparam.nUBGDetectTime_N, 0, sizeof(int)*9);
	memset(m_mccparam.nUBGDetectTime_L, 0, sizeof(int)*9);
	memset(m_mccparam.nUBGDetectTime_R, 0, sizeof(int)*9);
	memset(m_mccparam.nUBGDetectTime_S, 0, sizeof(int)*9);
	memset(m_mccparam.nUBGDetectTime_B, 0, sizeof(int)*9);
	memset(m_mccparam.nUBGDetectTime_V, 0, sizeof(int)*9);
	memset(m_mccparam.nUBGDetectTime_P, 0, sizeof(int)*9);

	m_MoniVANode=0;
	m_MoniVDAUNode =0 ;
	m_MoniVBNode=0;
	m_MoniVDBUNode=0;
	m_MoniVCNode=0;
	m_MoniVDCUNode=0;
	m_MoniAPNode=0;

	m_nResetworkingCount = 0;
	m_dwResetTime = timeGetTime();
	m_dwSOSLABFrameTime = timeGetTime();
	m_bRSSICheck =true; // AOHC-299

	memset(&m_lcd_flag, 0, sizeof(m_lcd_flag));
	memset(&HandyTP_AP_Info, 0, sizeof(APSIGNAL_DATA));

	m_bAMCAutoPatchErrorCheck = false;

	m_HID->SetRegulatorCheck(bRegulatorCheck);

    m_fAxis1Torque = 0.0;
	m_fAxis2Torque = 0.0;
}

/**
@brief   StatusCheckThread Class 소멸자
@author  LSJ
@date    2012.11.04
*/
StatusCheckThread::~StatusCheckThread()
{
	delete m_HID;
	delete m_IPCLoadLib;
	delete m_NetworkDll;
	delete m_CPUandMemInfo;
#if(WirelessClient_TYPE == FXE3000_WIRELESS_CLIENT || WirelessClient_TYPE == FXE2000_WIRELESS_CLIENT)
    delete m_UdpClient;
#endif
}

/**
@brief   RealThreadProc Thread 함수(StatusDataCheck 함수 호출, Check_Error 함수 호출)
@author  LSJ
@date    2012.11.09
*/
void StatusCheckThread::RealThreadProc()
{
	static DWORD dwIntervalOfLamp = timeGetTime();
	static DWORD dwIntervalOfLampYF = timeGetTime();
	static DWORD dwIntervalOfAmcResponse = timeGetTime();
	static DWORD dwIntervalOfLCDRenew = timeGetTime();

	int nError = NO_ERR;


    // ---- Place thread code here ----
    while(!m_bThreadBreaker)
    {
        if(m_bThreadBreaker == true)
            break;  // Thread 종료를 위해 필수로 추가

		//2014.04.15 doori.shin Thread Check를 위한 Tick값 증가
		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;

		try{

		static TimeChk timeChk;

		// Step 2. Lamp Control
		// Lamp Control (대략 1s 마다 Y_F제외한 상태 출력 500MS마다 Y_F상태 출력)

		if(OHTUTILITY::PreventOverflowGetTimeMs(dwIntervalOfLampYF) >= 250)
		{
			if((m_defualtparam->IsLCDAndLED) && (m_CheckLampYF == true) && (m_TPESTOP == false))
			{
					ControlYellowLamp();
					dwIntervalOfLampYF = timeGetTime();
			}
		}

		if(OHTUTILITY::PreventOverflowGetTimeMs(dwIntervalOfLamp) >= 1000)
		{
				ControlLamp();
				dwIntervalOfLamp = timeGetTime();
				//dwIntervalOfLampYF = timeGetTime();
		}


		timeChk.Start(1);

		//LCD
		if (m_defualtparam->IsLCDAndLED)
		{
			if((OHTUTILITY::PreventOverflowGetTimeMs(dwIntervalOfLCDRenew) >= 1000))
			{
#if((OHT_NAME == OHT_NAME_STD_V82)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
				if(m_nLCDVersion != LCD_NEW_VERSION)       // 3번 시도했는데도 Pre Version이면 버전 체크 미시도.. com0com 버그
				{
					LCDFirmwareVersionCheck();            // New Version 체크를 위해서
				}
#endif
				GetReadLCDData();
				dwIntervalOfLCDRenew = timeGetTime();
			}
		}
		timeChk.End(1);
		CycleOverCheck(1, timeChk.Get(1));

		// Step 3. Sound Control
		timeChk.Start(2);
#if((OHT_NAME == OHT_NAME_STD_V82)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
		ControlSound_ATLAS();
#else
		ControlSound();
#endif

		timeChk.End(2);
		CycleOverCheck(2, timeChk.Get(2));

		// Step 4. Update Common Status
		timeChk.Start(3);

		UpdateCommonStatus();

		timeChk.End(3);
		CycleOverCheck(3, timeChk.Get(3));


		timeChk.Start(4);

		UpdateStandardStatus(); //puting

		timeChk.End(4);
		CycleOverCheck(4, timeChk.Get(4));


		timeChk.Start(5);

		UpdateStatusForTP();

		timeChk.End(5);
		CycleOverCheck(5, timeChk.Get(5));


//		if(m_OHTMainStatus->ConnectedTP || m_pHWSet->m_pRemocon->IsConnect())
//		{
 //			UpdateStatusForTP();
			//2014.06.09 ji7412.lee amc update 완료/실패에 대한 일정시간 지난후 초기화

			if(OHTUTILITY::PreventOverflowGetTimeMs(dwIntervalOfAmcResponse) > 400)
			{
//				LOG_USER("AMC Update - Time Out");

				if((m_OHTMainStatus->StatusTP.AMCResponse == 0x02) || (m_OHTMainStatus->StatusTP.AMCResponse == 0x03))
				{
//					LOG_USER("AMC Update - State Reset");

					m_pHWSet->m_AMCResponse = 0x00;

					m_OHTMainStatus->StatusTP.AMCResponse = 0x00;

				}
				dwIntervalOfAmcResponse = timeGetTime();
			}
  //		}

		//진단에게 보낼 데이터 업데이트
		timeChk.Start(6);

		UpdateDiagData();

		timeChk.End(6);
		CycleOverCheck(6, timeChk.Get(6));


		timeChk.Start(7);

		UpdateStatusForMonitoring();

		timeChk.End(7);
		CycleOverCheck(7, timeChk.Get(7));

		timeChk.Start(8);

		AddUserLog();

		timeChk.End(8);
		CycleOverCheck(8, timeChk.Get(8));

		timeChk.Start(9);

		CheckTraceLog();

		timeChk.End(9);
		CycleOverCheck(9, timeChk.Get(9));

		timeChk.Start(10);
		if(OHTMainForm->GetTaskControl() != NULL)
		{
			OHTMainForm->GetTaskControl()->CheckUBGSequence(m_DistanceSensorType);
		}
		timeChk.End(10);
		CycleOverCheck(10, timeChk.Get(10));

        MCCStatusData();

		timeChk.End(0);	// Event 0   스레드 주기 감시용도
		//CycleOverCheck(0, timeChk.Get(0));

		static ThreadTimeMonitor tmon;
		tmon.Execute("StChkT", timeChk.Get(0));

		timeChk.Start(0);	// Event 0

		}catch(...)
		  LOG_USER("Status Exception!!!!");

        Sleep(40);
    }
}

// 2018.12.04 SHCHO
// 특정 구간에서 Block이 걸릴경우를 대비
// TimeChk 클래스로 구간별로 감시. 블럭이 감지되면 m_bCycleOver 값을 true로 함. User 로그 기록 후 원복 됨
void StatusCheckThread::CycleOverCheck(int nEventNo, DWORD nChkTime)
{
	int nCycleLimit = 100;

//	if(nEventNo == 0)
//		nCycleLimit = 500;

	if(nChkTime > nCycleLimit)
	{
		LOG_USER("Cycle Time Over!!!! Event:%d, Time:%d", nEventNo, nChkTime);
	}

}
void StatusCheckThread::CycleMonitoring(int nEventNo, DWORD nChkTime)
{
	int nCycleLimit = 10*1000;

	if(nChkTime > nCycleLimit)
	{
		ADD_MD_LOG("[UpdateStatusForMonitoring] Time Over!!!! Event:%d, Time:%d", nEventNo, nChkTime);
	}

}
/**
@brief   Standart OHT Status를 Update하는 함수
@author  puting
@date    2015.11.10
*/
void StatusCheckThread::UpdateStandardStatus()
{
	static bool IsGripFoup = false;
	int nIsOriginPos = 0;
	int TmepNode = 0;
	int Tmepstation= 0;
	int TmepStopstation= 0;

	int nError = NO_ERR;

	static DWORD dwOHTDetectTrigerTime = timeGetTime();     //함수 진입 시간.
	static DWORD dwOHTDataTime = timeGetTime();     //함수 진입 시간.

	static DWORD dwHIDTime = timeGetTime();     //함수 진입 시간.

	static DWORD dwCRTime = timeGetTime();     //함수 진입 시간.
	static DWORD dwFoupTime = timeGetTime();     //함수 진입 시간.

	//DWORD dwStartTime = timeGetTime();

	static int nPreMode = 0x00;
	static int nOCSReportError = NO_ERR;

	static char cPreStatus_Trans = 0;

    BYTE CurMode    = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
	BYTE CurStatus  = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status;

	int nDetectStatus = m_pHWSet->m_pOHTDetect->GetStatus(); //DETECT_AND_STOP
	bool nDetectSensorStatus = m_pHWSet->m_pOHTDetect->GetSensorStatus();
	int nOBSDetectStatus = m_pHWSet->m_pObstacle->GetStatus(); //DETECT_AND_STOP    DETECT_AND_STOP

	int AMC_Pause_Flag = m_pHWSet->GetAMCPauseStatus();
	nError =  m_OHTMainStatus->ErrCode_StatusChk;

	AnsiString TempHexErrorCode;
	int TempErrorCode =0;

	int nTempCheckHID = NO_ERR;

    int nNodeIndex = 0;
	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	bool bSlopeCheck = false;


	//Check Commfail!!
	if(OHTUTILITY::PreventOverflowGetTimeMs(m_OHTMainStatus->CheckCommfailtime) > 20000)
	{
		nCommfailCount++;
		m_OHTMainStatus->CheckCommfailtime = timeGetTime();
    }

	if((((nPreMode == MODE_AUTO) && (CurMode == MODE_MANUAL)) || (m_OHTMainStatus->InnerErrCode == NO_ERR)) && (m_bAMCAutoPatchErrorCheck == false))
	{
		nOCSReportError = m_OHTMainStatus->InnerErrCode;
		nPreMode = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
	}
//AOHC-154
  else if((CurMode == MODE_MANUAL) && ( m_pAutoPatchParamSet->bAutoPatchControl == true )&&(m_OHTMainStatus->InnerErrCode != NO_ERR)&&(m_bAMCAutoPatchErrorCheck== false)) //auto patch에서 파라미터 1로 변경 -> patch 완료 후 오토 전환 시 파라미터 0으로 변경 필요
	{
		nOCSReportError = m_OHTMainStatus->InnerErrCode;
		nPreMode = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
	}
	else if((CurStatus ==STS_AUTORECOVERY) && (m_LineParamSet->bUseE84TimeoutErrorCode ==true))
	{
     	nOCSReportError = m_OHTMainStatus->InnerErrCode;
	}

	//오버런처리
	if(m_ExecuteInfo->FlagOverRunInfo.nFinalNode !=0)
	{

//		 ADD_LOG_EXTRAJOB("OverRun---! %d %d",m_ExecuteInfo->ExecutePositionInfo.CurrStation, m_ExecuteInfo->FlagOverRunInfo.nFinalNode);

		if(m_ExecuteInfo->FlagOverRunInfo.OverrunType ==0x01)
			m_ExecuteInfo->ExecutePositionInfo.CurrNode = m_ExecuteInfo->FlagOverRunInfo.nFinalNode;

		else if(m_ExecuteInfo->FlagOverRunInfo.OverrunType ==0x02)
			m_ExecuteInfo->ExecutePositionInfo.CurrStation = m_ExecuteInfo->FlagOverRunInfo.nFinalNode;
	}

	m_OHTMainStatus->bIsOHTStoped = !(m_pHWSet->m_pDrivingAxis->IsRunning());
	//상태값 --------------------------------------------------------------


	m_OHTMainStatus->StandardStatusOHT.PauseStatus.LastOHTDetect = nDetectSensorStatus;
	if(nDetectStatus == DETECT_AND_STOP)
	{
		m_OHTMainStatus->StandardStatusOHT.PauseStatus.OHT_Detect = ON;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CrashAvoidWait = ON;//보고용
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningVHLStatus.OHTDetect = ON;//보고용
	}
	else
	{
		m_OHTMainStatus->StandardStatusOHT.PauseStatus.OHT_Detect = OFF;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CrashAvoidWait = OFF;  //보고용
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningVHLStatus.OHTDetect = OFF;//보고용
	   //	dwOHTDetectTrigerTime = timeGetTime();
	}

	//상태값 --------------------------------------------------------------
	if(nOBSDetectStatus == DETECT_AND_STOP)
	{
		m_OHTMainStatus->StandardStatusOHT.PauseStatus.OBS_Detect = ON;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningVHLStatus.OBSDetect = ON;//보고용
	}
	else
	{
		m_OHTMainStatus->StandardStatusOHT.PauseStatus.OBS_Detect = OFF;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningVHLStatus.OBSDetect = OFF;//보고용
	}

	if((m_OHTMainStatus->StandardStatusOHT.PauseStatus.OBS_Detect ==OFF) &&
	   (m_OHTMainStatus->StandardStatusOHT.PauseStatus.OHT_Detect ==OFF ))
	   {
			dwOHTDetectTrigerTime = timeGetTime();
	   }

	//현재 및 대기 사이클
	//동기화

	if(m_OHTMainStatus->WaitCycle == CYCLE::CycleNotExist)
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningCycle = m_OHTMainStatus->CurCycle;
		Tmepstation =	m_OHTMainStatus->TargetStationNID;
    }
	else
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningCycle = m_OHTMainStatus->WaitCycle;
		Tmepstation =	m_OHTMainStatus->WaitTargetStationNID;
    }


	TmepNode =	m_ExecuteInfo->ExecutePositionInfo.CurrNode;
	TmepStopstation = m_ExecuteInfo->ExecutePositionInfo.CurrStation;


//	ADD_LOG_EXTRAJOB("----TmepNode-TmepStopstation-- %d %d %d %d %d", TmepNode, TmepStopstation,Tmepstation, m_OHTMainStatus->WaitTargetStationNID,m_OHTMainStatus->TargetStationNID);

    //동기화
	for(int Index =0; Index< 3; Index++)
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CurrentAddr[2-Index] = (TmepNode >> (Index * 8))&0xFF;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.StopNowAddr[2-Index] = (TmepStopstation >> (Index * 8))&0xFF;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.DestinationAddr[2-Index] = (Tmepstation >> (Index * 8))&0xFF;
	}

     //기동중 상태
     //	m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning = 0;

	// Step 1. HID Check

	nTempCheckHID = CheckHID();



	if(((CurStatus !=STS_GOING) && (CurStatus != STS_UNLOADING) && (CurStatus !=STS_LOADING)&& (CurStatus !=STS_RUNNING))
		&& (CurMode == MODE_MANUAL)
//		&& (bSlopeCheck ==false)
		&& (nTempCheckHID == ERR_TEMPER_OVERTEMPERATURE))
		{
			if(m_ExecuteInfo->ExecutePositionInfo.Marked == true)
			{
				nNodeIndex =	OHTMainForm->GetMap()->FindNodeIndex(m_ExecuteInfo->ExecutePositionInfo.CurrNode);
				pNODE_INFO = &(OHTMainForm->GetMap()->m_NodeArray[nNodeIndex]);		// Node 정보

				if(pNODE_INFO->InOutInfo ==SLOPE_NTYPE)
				{
					bSlopeCheck = true;

					//보완 필요시 넣을것.
				}

			}

			if((OHTUTILITY::PreventOverflowGetTimeMs(dwHIDTime) > 5000)
				&& (bSlopeCheck ==false))
			{
				if(m_pHWSet->m_pDrivingAxis->IsSlaveAMPEnable() == true)
				{
					m_pHWSet->m_pDrivingAxis->SetSlaveAMPEnable(false);
				}
			}

		}
		else
		{
			//시간 초기화



			dwHIDTime = timeGetTime();
		}


		//HID 보호회로 동작 관련 에러시 에러처리
		if((nTempCheckHID != NO_ERR) && (nTempCheckHID != ERR_TEMPER_OVERTEMPERATURE))
		{
			m_OHTMainStatus->ErrCode_StatusChk = nTempCheckHID;
		}


	//이상 상태
	//정위치 오버런    STS_AUTORECOVERY_OVERRUN




	if(CurStatus ==STS_AUTORECOVERY_OVERRUN)
	   {
	   	   //	ADD_LOG_EXTRAJOB("OverRun--- %d",m_ExecuteInfo->ExecutePositionInfo.CurrNode);
			//정위치 오버런
//            ADD_LOG_EXTRAJOB("OverRun--- %d %d",m_ExecuteInfo->ExecutePositionInfo.CurrNode, m_ExecuteInfo->FlagOverRunInfo.nFinalNode);



			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionOvrRun = ON;
			//이상
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.AbnormalTermination = OFF;
			//E84 Timeout
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.E84Timeout = OFF;
			//이중반입
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.DoubleInput = OFF;
			//공출하
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.BlankUnload = OFF;

	   }
	else if(CurStatus ==STS_AUTORECOVERY)
	{
		//이상
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.AbnormalTermination = OFF;
		//E84 Timeout
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.E84Timeout = ON;
		//이중반입
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.DoubleInput = OFF;
		//공출하
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.BlankUnload = OFF;
		//정위치 오버런
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionOvrRun = OFF;

	}
	else if(CurStatus ==STS_AUTORECOVERY_BLANKTRANS)
	{
		//이상
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.AbnormalTermination = OFF;
		//E84 Timeout
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.E84Timeout = OFF;
		//이중반입
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.DoubleInput = OFF;
		//공출하
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.BlankUnload = ON;
		//정위치 오버런
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionOvrRun = OFF;
	}
	else if(CurStatus ==STS_AUTORECOVERY_DOUBLETRANS)
	{
		//이상
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.AbnormalTermination = OFF;
		//E84 Timeout
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.E84Timeout = OFF;
		//이중반입
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.DoubleInput = ON;
		//공출하
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.BlankUnload = OFF;
		//정위치 오버런
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionOvrRun = OFF;
	}
	else if((nOCSReportError !=NO_ERR) &&(CurStatus ==STS_ERROR))
	{
//		ADD_LOG_EXTRAJOB("----Error--- %d %d", m_OHTMainStatus->InnerErrCode, nOCSReportError);
		//이상
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.AbnormalTermination = ON;
		//E84 Timeout
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.E84Timeout = OFF;
		//이중반입
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.DoubleInput = OFF;
		//공출하
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.BlankUnload = OFF;

		//정위치 오버런
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionOvrRun = OFF;
	}
	else
	{
    	//이상
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.AbnormalTermination = OFF;
		//E84 Timeout
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.E84Timeout = OFF;
		//이중반입
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.DoubleInput = OFF;
		//공출하
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.BlankUnload = OFF;
		//정위치 오버런
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionOvrRun = OFF;
    }

	if((nOCSReportError != NO_ERR) &&
	  ((m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.AbnormalTermination == ON) ||
	  ((m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.E84Timeout==true)&& (m_LineParamSet->bUseE84TimeoutErrorCode ==true))))
	{
		//OCS에 보고용은 10진수 숫자 그대로 16진수로 변환하여 보고 puting 2017.08.02
		TempHexErrorCode.sprintf("0x%d",nOCSReportError);
		TempErrorCode = TempHexErrorCode.ToInt();
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.ErrorCode = TempErrorCode;
	}
	else
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.ErrorCode = NO_ERR;


	//자동/수동 상태
	if(CurMode == MODE_MANUAL)
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.Auto = OFF;
	else
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.Auto = ON;


	  //BZ 대기중
	 if(((CurStatus ==STS_ARRIVED) || (CurStatus==STS_INIT) ) &&
	 	(m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.PassConfirmGet == OFF) &&
		(m_OHTMainStatus->bRealGoCycleing == true))// &&
	 //	((dwStartTime - dwOHTDataTime) > 2000))
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.BzWait = ON;
	 else
	 {
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.BzWait = OFF;
			dwOHTDataTime = timeGetTime();
	 }

	//분기 불가
	 if(OHTUTILITY::PreventOverflowGetTimeMs(dwOHTDataTime) > 15000)
		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnImpossible = ON;      ///통과허가하는 곳에서 처리할것.
		}
	 else
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnImpossible = OFF;      ///통과허가하는 곳에서 처리할것.


	//재하1
//  	if((CurStatus !=STS_LOADING) && (CurStatus !=STS_UNLOADING))

	// Foup 유무           if(m_pHWSet->m_pInnerFoup->IsFoupDetect())
#if(OHT_NAME == OHT_NAME_P4_MASK)
	if(m_pHWSet->m_pHand->IsHomingState() == false)
	{
		if((m_pHWSet->m_pHand->IsLeftGripOn() == true) && (m_pHWSet->m_pHand->IsRightGripOn() == true))
		{

			if((m_pHWSet->m_pHand->IsHome() == true) && (m_pHWSet->m_pHand->IsWork() == false))
			{
					//채터링
				if(IsGripFoup == OFF)
				{
					if (OHTUTILITY::PreventOverflowGetTimeMs(dwCRTime) > 500)
					{
						IsGripFoup = ON;
					}
				}
				else
					dwCRTime =  timeGetTime();

			}else
				dwCRTime =  timeGetTime();

		}
		else  if((m_pHWSet->m_pHand->IsLeftGripOn() == false) && (m_pHWSet->m_pHand->IsRightGripOn() == false))
		{
			//if((m_pHWSet->m_pHand->IsHome() == false) && (m_pHWSet->m_pHand->IsWork() == true))
			if((m_pHWSet->m_pHand->IsHome() == true) && (m_pHWSet->m_pHand->IsWork() == false))
			{
			  //채티링
				if(IsGripFoup == ON)
				{
					if (OHTUTILITY::PreventOverflowGetTimeMs(dwCRTime) > 500)
					{
						IsGripFoup = OFF;
					}
				}
				else
					dwCRTime =  timeGetTime();
			}
			else
				dwCRTime =  timeGetTime();
		}
		else
			dwCRTime =  timeGetTime();
	}
#else
	if(m_pHWSet->m_pHand->IsHomingState() == false)
	{
		if((m_pHWSet->m_pHand->IsLeftGripOn() == true) && (m_pHWSet->m_pHand->IsRightGripOn() == true))// || (m_pHWSet->m_pInnerFoup->IsFoupDetect()))
		{

			if((m_pHWSet->m_pHand->IsClose() == true) && (m_pHWSet->m_pHand->IsOpen() == false))
			{
				//채터링
				if(IsGripFoup == OFF)
				{
					if (OHTUTILITY::PreventOverflowGetTimeMs(dwCRTime) > 500)
					{
						IsGripFoup = ON;
					}
				}
				else
					dwCRTime =  timeGetTime();


//				ADD_LOG_EXTRAJOB("IsHomingState_2 %d",IsGripFoup);
			}else
				dwCRTime =  timeGetTime();

		}
		else  if((m_pHWSet->m_pHand->IsLeftGripOn() == false) && (m_pHWSet->m_pHand->IsRightGripOn() == false))
		{
			if((m_pHWSet->m_pHand->IsClose() == false) && (m_pHWSet->m_pHand->IsOpen() == true))
			{
			  //채티링
				if(IsGripFoup == ON)
				{
					if (OHTUTILITY::PreventOverflowGetTimeMs(dwCRTime) > 500)
					{
						IsGripFoup = OFF;
					}
				}
				else
					dwCRTime =  timeGetTime();


//				ADD_LOG_EXTRAJOB("IsHomingState_2_1 %d",IsGripFoup);
			}
			else
				dwCRTime =  timeGetTime();
//			else if((m_pHWSet->m_pHand->IsClose() == false) && (m_pHWSet->m_pHand->IsOpen() == false))
//			{
//				if(m_pHWSet->m_pInnerFoup->IsFoupDetect() == false) //채터링 추가.
//				{
//					IsGripFoup = OFF;
//				}
//			}
		}
		else
			dwCRTime =  timeGetTime();
	}
#endif

	if(bUseAVS)
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1 = 0;
	else
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1 = IsGripFoup;

	//이재기 Hp
#if(OHT_NAME==OHT_NAME_P4_MASK)
		if((m_pHWSet->m_pHand->IsHome() == true) && (m_pHWSet->m_pHand->IsWork() == false) &&
			(m_pHWSet->m_pShutter->IsClose(FRONT) == true) &&(m_pHWSet->m_pShutter->IsOpen(FRONT) == false) &&
			(fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < INPOS_LIMIT))
		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.LoadUnloadHP = ON;
			dwFoupTime = timeGetTime();
		}
		else
		{
			if (OHTUTILITY::PreventOverflowGetTimeMs(dwFoupTime) > 300)
			{
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.LoadUnloadHP = OFF;
			}
		}
#else
	if(!(m_defualtparam->PreShutter))
	{
		if ((((m_pHWSet->m_pHand->IsClose() == true) && (m_pHWSet->m_pHand->IsOpen() == false) &&
			(m_pHWSet->m_pShutter->IsClose(FRONT) == true) &&(m_pHWSet->m_pShutter->IsClose(REAR) == true)) ||
			((m_pHWSet->m_pHand->IsClose() == false) && (m_pHWSet->m_pHand->IsOpen() == true) &&
			(m_pHWSet->m_pShutter->IsOpen(FRONT) == true) &&(m_pHWSet->m_pShutter->IsOpen(REAR) == true))) &&
	// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))
			( (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) < (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			|| ((fabs(ROTATE_POS_90 - m_pHWSet->m_pRotate->GetCurrentPositionDegree())) < (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			|| ((fabs(ROTATE_POS_180 - m_pHWSet->m_pRotate->GetCurrentPositionDegree())) < (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180)))) &&
#endif
			((fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) < HOIST_HOME_INPOS_LIMIT) && (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < INPOS_LIMIT)))

		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.LoadUnloadHP = ON;
			dwFoupTime = timeGetTime();
		}
		else
		{
			if (OHTUTILITY::PreventOverflowGetTimeMs(dwFoupTime) > 300)
			{
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.LoadUnloadHP = OFF;
			}
		}

	}
	else //셔터 사전동작
	{
		if ((((m_pHWSet->m_pHand->IsClose() == true) && (m_pHWSet->m_pHand->IsOpen() == false)) ||
			((m_pHWSet->m_pHand->IsClose() == false) && (m_pHWSet->m_pHand->IsOpen() == true))) &&

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))
			( (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) < (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			|| ((fabs(ROTATE_POS_90 - m_pHWSet->m_pRotate->GetCurrentPositionDegree())) < (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			|| ((fabs(ROTATE_POS_180 - m_pHWSet->m_pRotate->GetCurrentPositionDegree())) < (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180)))) &&
#endif
			((fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) < HOIST_HOME_INPOS_LIMIT) && (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < INPOS_LIMIT)))
		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.LoadUnloadHP = ON;
			dwFoupTime = timeGetTime();
		}
		else
		{
			if (OHTUTILITY::PreventOverflowGetTimeMs(dwFoupTime) > 300)
			{
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.LoadUnloadHP = OFF;
			}
		}
	}
#endif

	//반송가능 상태
	//에러가 없고, 현재상태가 이재가능 상태이며 등등.
//	if((CurStatus !=STS_ERROR)&&
//	   (m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.LoadUnloadHP ==ON))

	if(CurMode != MODE_MANUAL)   //사양 변경 20180303 puting
	   {
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.ReturnPossible = ON;
	   }
	else
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.ReturnPossible = OFF;


	//HT 조작 정지중 (HT Pause 및 TP 조작후 정지할 경우 )

	if((CurStatus == STS_COMPLETE) || (m_ExecuteInfo->bTpPause == true) || (m_ExecuteInfo->bOcsPause == true))
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.HtControlStopped = ON;
	else
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.HtControlStopped = OFF;





	//재하 종류 체크중
	if(m_OHTMainStatus->StatusCommon.ExistFoup)
	{
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
        if(m_pHWSet->m_pInnerFoup->IsEuvDetect())
		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CarryType = CARRIER_EUV;
		}
		else if(m_pHWSet->m_pInnerFoup->IsFoupDetect())
		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CarryType = CARRIER_POD;
		}
#elif(OHT_NAME == OHT_NAME_P4_MASK)
		if(m_pHWSet->m_pShutter->IsPodPlacement3On() && m_pHWSet->m_pShutter->IsPodPlacement4On())
		{
			if(m_pHWSet->m_pHoistAxis->IsHoistRSP150Home() == true)
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CarryType = CARRIER_POD;
			else if(m_pHWSet->m_pHoistAxis->IsHoistRSP200Home() == true)
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CarryType = CARRIER_RSP200;
			else if(m_pHWSet->m_pHoistAxis->IsHoistEUVHome() == true)
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CarryType = CARRIER_EUV;
			else
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CarryType = CARRIER_NOTDEFINED;
		}
#else
		if(m_pHWSet->m_pInnerFoup->IsFoupDetect())
		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CarryType = CARRIER_FOUP; //m_pTransControl->m_pParamSet->CarrierType;
//            ADD_LOG_EXTRAJOB("Carrirer Type: %d", m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CarryType);

		}
#endif
		else
		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CarryType = CARRIER_NOTDEFINED;
		}
	}
	else
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.CarryType = CARRIER_NOTDEFINED;
	}


	//OCS 정보 변경 APSignal AOHC-292
	m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.APSinalLevel = m_OCSAPSignal;     //nAPSignalLevel

	// 이동 거리 및 속도
	m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningDistance = m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset; //m_pHWSet->m_pDrivingAxis->GetCurrentPosition();
	m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningSpeed = m_pHWSet->m_pDrivingAxis->GetCurrentVelocity() * 1000;



	//주행중
	if((CurStatus ==STS_GOING) || ( m_pHWSet->m_pDrivingAxis->IsRunning()==true))
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.Running = ON;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionStop = OFF;

		//감속 정지중
		if(m_ExecuteInfo->bSpeedDownFlag == true)
		{
			if(((m_ExecuteInfo->nFinishOfNode !=0)  && (m_ExecuteInfo->ExecutePositionInfo.CurrCommandedNode == m_ExecuteInfo->nFinishOfNode)) ||
			   ((m_ExecuteInfo->nFinishOfStation !=0)  && (m_ExecuteInfo->ExecutePositionInfo.CurrCommandedStation == m_ExecuteInfo->nFinishOfStation)))
			{
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.SlowDownStop = ON;
			}
			else
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.SlowDownStop = OFF;
		}
		else
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.SlowDownStop = OFF;

	}
	else
	{
		//문영민K, 장재호K 요청내용 2018/1/25 최종 타겟 위치일 경우만 정위치 플래그 올려줄 것.
		if(m_OHTMainStatus->bRealGoCycleing == false) //싸이클 중이 아닐 경우만 정위치 플래그 설정함.
		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionStop = m_ExecuteInfo->ExecutePositionInfo.Marked;
		}
		else
		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionStop = OFF;

		}


		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.Running = OFF;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.SlowDownStop = OFF;



    }



//	ADD_LOG_EXTRAJOB("TestLog: %d %d", m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.Running,
//	m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionStop);

	if(OHTUTILITY::PreventOverflowGetTimeMs(dwOHTDetectTrigerTime) > 15000)  //30sec -> 15sec
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.Traffic_Sapche = ON;
	}
	else
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.Traffic_Sapche = OFF;
	}

	if(OHTUTILITY::PreventOverflowGetTimeMs(dwOHTDetectTrigerTime) > 30000)  //60sec -> 30sec
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.Traffic_Jungche = ON;
	}
	else
	{
        m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.Traffic_Jungche = OFF;
	}


	//정체

	if(m_ExecuteInfo->nCheckDivInfo == PATH_DIVERGENCE_LEFT)
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnRight = OFF;    //하위의 Map 정보에 의해 처리할 것.
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnLeft = ON;
		//m_OHTMainStatus->TurnOffInfo.VHL_Div = 0x02;

	}
	else if(m_ExecuteInfo->nCheckDivInfo == PATH_DIVERGENCE_RIGHT)
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnRight = ON;    //하위의 Map 정보에 의해 처리할 것.
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnLeft = OFF;
		//m_OHTMainStatus->TurnOffInfo.VHL_Div = 0x02;

	}else
	{
		//왼쪽 분기   ----처리 필요
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnRight = OFF;    //하위의 Map 정보에 의해 처리할 것.
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnLeft = OFF;
//		m_OHTMainStatus->TurnOffInfo.VHL_Div = 0x02;
	}


	if(m_OHTMainStatus->StandardStatusOHT.StandardOrderStatus.DataTransfer ==ON)
	{
       	ADD_LOG_EXTRAJOB("OCS DataTransFer Complete Flag On");
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataSuccess = OFF;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail = OFF;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID = OFF;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataType = OFF;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataStatus = OFF;
	}


	//뺴냄 OK 	//뺴냄 NG
	if(m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningCycle !=  CYCLE::TakeOut)
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.TakeOutOK = OFF;
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.TakeOutNG = OFF;
    }

	if(m_OHTMainStatus->StandardStatusOHT.VHL_Insert != INIT)
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.Rail_1_InputReq = OFF;
	 	m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.Rail_2_InputReq = OFF;
	}

	if(m_OHTMainStatus->StandardStatusOHT.VHL_FloorTransfer != INIT)
	{
		//층간이동확인 OK
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.FloorMoveOk = OFF;
		//층간이동확인 NG
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.FloorMoveNg = OFF;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////추가 사양 확인 필요 내용 ///////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///추후 Data 관련 내용 사양 정의 이후에 진행 ----puting

	///<레이아웃 변경용 주행중
	if(m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningCycle  ==CYCLE::TP_RunningLearning)
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.LayoutChangeRun = ON;      ///Map Making 상태값 확인하여 플래그 설정.
	else
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.LayoutChangeRun = OFF;      ///Map Making 상태값 확인하여 플래그 설정.

	//후퇴 주행중 (현재 미사용)
	m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.BackMovingNow = OFF;

	//대기 사이클  (현재 미사용)
	m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.WaitingCycle = 0;


	m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningVHLStatus.CID_Stop = m_ExecuteInfo->FlagCIDReport.bCIDStop;

	if(m_ExecuteInfo->FlagCIDReport.bCIDInterlock == true)
	{
		if(m_ExecuteInfo->nHIDPathInfo == EXECUTE_HIDPATH_SLOOP)
		{
			m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningVHLStatus.CID_Interlock = true;
        }
		else
		{
			if(m_PassPermit->GetPassPermitCount() !=0) // m_pHWSet->m_pCID->GetCIDPause() ==true
			   m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningVHLStatus.CID_Interlock = true;
			else
			   m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningVHLStatus.CID_Interlock = false;
        }

	}
	else
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningVHLStatus.CID_Interlock = false;

    m_OHTMainStatus->bOCSReportFlag =true;
	memmove(&(m_OHTMainStatus->StandardOHTStatusReport),&(m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus),sizeof(PROTOCOL_STATUS_OHT));
    m_OHTMainStatus->bOCSReportFlag =false;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
									////////////////기존 내용////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// StopStationOffset 정보 누락
	//m_OHTMainStatus->StatusCommon.StopStationOffset;

	int nInCount = 0, nOutCount = 0;
    int InputBuff[4] = {0,};
    int OutputBuff[4] = {0,};


    ///@ todo ExecuteThread에서 구현
// int nTmpNode = 0;
//
// if ((m_OHTMainStatus->cMode == MODE_MANUAL) ||
// ((m_OHTMainStatus->cMode == MODE_AUTO) && (gm_OHTStatus->nTargetNode == 0)))
// nTmpNode = gm_OHTStatus->nCurNode;
// else
// nTmpNode = gm_OHTStatus->nTargetNode;
//
// int nPauseStatus = 0;
//
// if (m_OHTMainStatus->cMode == MODE_MANUAL)
// nPauseStatus = 0;
// else if (gm_OHTStatus->AMCValue->strAMCObsStatus == "Q")
// {
// // 1:TP Command 2:OCS Command -> 2일때만 ocs가 pause된것으로 인식함
// nPauseStatus = gm_OHTStatus->AMCValue->nIsPauseSet;
//
// // OCS Pause 인 경우 2:OHT 또는 장애물이 감지된 경우, 3:OHT 또는 장애물이 감지되지 않은 경우로 구분함.
// // OCS 패치시 아래 풀고 패치 진행

// if (gm_OHTStatus->AMCValue->nIsPauseSet == 2)
// {
// int nTmepLevel = gm_IOControl->Get_UBG_Level(gm_OHTStatus->IOInput->nOHTDetect_Sen_Long,
// gm_OHTStatus->IOInput->nOHTDetect_Sen_Middle, gm_OHTStatus->IOInput->nOHTDetect_Sen_Short,
// gm_OHTStatus->IOInput->nOHTDetect_Sen_Fail);
// if ((nTmepLevel > 0) && (nTmepLevel <= 4)) // Level 4이하일때 Pause가 걸림 -> 4이하일때 OHT가 있는것으로 인식
// nPauseStatus = 2;
// else // OHT가 없는것으로 인식하고 Pause 해제함.
// nPauseStatus = 3;
// }
//
// }
// else if ((gm_OHTStatus->AMCValue->strAMCObsStatus == "P") || (gm_OHTStatus->AMCValue->strAMCObsStatus == "O"))
// nPauseStatus = 1;
// else
// nPauseStatus = 0;

    // by zzang9un 2013.05.16 : OHT Type 추가
	///@todo 정의 안된 값들에는 default 값으로 처리했으나 나머지 처리 필요
	// UBG 관련
	m_OHTMainStatus->StatusTP.IsUBGSen = 1;
	m_OHTMainStatus->StatusTP.UBGDetectLevel = 0;

	m_OHTMainStatus->StatusTP.IsFCPause = m_pHWSet->GetAMCPauseStatus();
	m_OHTMainStatus->StatusTP.TPCommandType = 0x00000000;
	m_OHTMainStatus->StatusTP.Cmd_Count = 0;

	// by zzang9un 2013.03.19 : ErrorString 삭제 예정
//    if(m_OHTMainStatus->ErrCode_StatusChk)
//    {
//        // 에러가 있는 경우
//        strcpy(m_OHTMainStatus->StatusTP.ErrorString, "Error");
//    }

	//2014.06.09 ji7412.lee AMC update 진행보고
	m_OHTMainStatus->StatusTP.AMCResponse = m_pHWSet->m_AMCResponse;
	//m_OHTMainStatus->StatusTP.AMCResponse = 0x00;


	m_OHTMainStatus->StatusTP.DownPercent = 0x00;
	m_OHTMainStatus->StatusTP.CPUTemperature = m_OHTMainStatus->m_cpMonitorData.IPCData.dCPUTemperature;

    ///@todo AP 부분 구현을 위해 수정 필요
    //m_OHTMainStatus->StatusTP.APSignalLevel = ;
//  memcpy(m_OHTMainStatus->StatusTP.AMCVer, m_MonitorDataArray->APSignalData.szAPMacAddress, sizeof(m_MonitorDataArray->APSignalData.szAPMacAddress));
	m_OHTMainStatus->StatusTP.APSignalLevel = m_OHTMainStatus->m_cpMonitorData.APSignalData.nAPSignalLevel;

    // 2016-12-03, Add by Labeler, ROHT의 경우 Rotate 정보 수집 전, Motor에 정보 요청 처리
#if((OHT_NAME==OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))
	//AOHC-215 Rotate 에러 개선(비동기 방식 동작 및 Critical Section 보호)
	if(m_pHWSet->m_pRotate != NULL)
    {
		m_pHWSet->m_pRotate->ExecuteStep();	// 아래 내용 비동기처리

        //if(m_pHWSet->m_pRotate->GetReqABSMoveStatus() == false)
		//{
		//	m_pHWSet->m_pRotate->ReqPosition();
		//	m_pHWSet->m_pRotate->ReadPosition();
		//	m_pHWSet->m_pRotate->ReqVelocity();
		//	m_pHWSet->m_pRotate->ReadVelocity();
		//	m_pHWSet->m_pRotate->ReqMotorStatus();
		//	m_pHWSet->m_pRotate->ReadMotorStatus();
        //}
    }
#endif

	// VMS Status Update
	BYTE SteerFront = 0, SteerRear = 0;

	if(m_pHWSet->m_pSteer->IsLeftPos(FRONT) == true)   	    SteerFront = 1;
	else if(m_pHWSet->m_pSteer->IsRightPos(FRONT) == true)  SteerFront = 2;
	else                                                    SteerFront = 0;

	if(m_pHWSet->m_pSteer->IsLeftPos(REAR) == true)         SteerRear = 1;
	else if(m_pHWSet->m_pSteer->IsRightPos(REAR) == true)   SteerRear = 2;
	else                                                    SteerRear = 0;

	m_OHTMainStatus->StatusVMS.CurMode    = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
	m_OHTMainStatus->StatusVMS.CurStatus  = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status;
	m_OHTMainStatus->StatusVMS.CurNodeID = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
	m_OHTMainStatus->StatusVMS.CurStationID = m_ExecuteInfo->ExecutePositionInfo.CurrStation;
	m_OHTMainStatus->StatusVMS.CurNodeOffset = m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset;
	m_OHTMainStatus->StatusVMS.SteerFront = SteerFront;
	m_OHTMainStatus->StatusVMS.SteerRear = SteerRear;
	m_OHTMainStatus->StatusVMS.OHTDetectLevel	= (BYTE) m_pHWSet->m_pOHTDetect->GetSensorLevel_MCC();
	m_OHTMainStatus->StatusVMS.OBSDetectLevel	= (BYTE) m_pHWSet->m_pObstacle->GetSensorLevel_MCC();
	m_OHTMainStatus->StatusVMS.DrivingSpeed = (float) m_pHWSet->m_pDrivingAxis->GetCurrentVelocity();
	m_OHTMainStatus->StatusVMS.DrivingPosition = (float) m_pHWSet->m_pDrivingAxis->GetCurrentPosition();
	m_OHTMainStatus->StatusVMS.HoistSpeed = (float) m_pHWSet->m_pHoistAxis->GetCurrentVelocity();
	m_OHTMainStatus->StatusVMS.HoistPosition = (float) m_pHWSet->m_pHoistAxis->GetCurrentPosition();
	m_OHTMainStatus->StatusVMS.SlideSpeed = (float) m_pHWSet->m_pShiftAxis->GetCurrentVelocity();
	m_OHTMainStatus->StatusVMS.SlidePosition = (float) m_pHWSet->m_pShiftAxis->GetCurrentPosition();

	m_OHTMainStatus->StatusBLACKBOX.CurMode    = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
	m_OHTMainStatus->StatusBLACKBOX.CurStatus  = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status;
	m_OHTMainStatus->StatusBLACKBOX.CurNodeID = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
	m_OHTMainStatus->StatusBLACKBOX.TargetNodeID = m_ExecuteInfo->ExecutePositionInfo.CurrCommandedNode;
	m_OHTMainStatus->StatusBLACKBOX.CurNodeOffset = m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset;
	m_OHTMainStatus->StatusBLACKBOX.DrivingSpeed =  m_pHWSet->m_pDrivingAxis->GetCurrentVelocity();
	m_OHTMainStatus->StatusBLACKBOX.Edc = m_OHTMainStatus->m_cpMonitorData.HIDData.Edc;
	m_OHTMainStatus->StatusBLACKBOX.Iout_sign = m_OHTMainStatus->m_cpMonitorData.HIDData.Iout_sign;
	m_OHTMainStatus->StatusBLACKBOX.Iout = m_OHTMainStatus->m_cpMonitorData.HIDData.Iout;
	m_OHTMainStatus->StatusBLACKBOX.IoT_Torque_Driving_F = (float) m_fAxis1Torque;//m_pHWSet->m_pDrivingAxis->GetCurrentVelocity();
	m_OHTMainStatus->StatusBLACKBOX.IoT_Torque_Driving_R = (float) m_fAxis2Torque;//m_pHWSet->m_pDrivingAxis->GetCurrentVelocity();
	m_OHTMainStatus->StatusBLACKBOX.ErrorCode = m_OHTMainStatus->InnerErrCode;;//m_OHTMainStatus->StatusCommon.ErrorCode;
	m_OHTMainStatus->StatusBLACKBOX.OHTDetectLevel	= (BYTE) m_pHWSet->m_pOHTDetect->GetSensorLevel_MCC();
	m_OHTMainStatus->StatusBLACKBOX.OBSDetectLevel	= (BYTE) m_pHWSet->m_pObstacle->GetSensorLevel_MCC();
}

/**
@brief   OHT Common Status를 Update하는 함수
@author  zzang9un
@date    2014.02.03
*/
void StatusCheckThread::UpdateCommonStatus()
{
    bool IsGripFoup = false;
    int nIsOriginPos = 0;

    static bool IsPreGripFoup = false;
    static char cPreStatus_Trans = 0;

    BYTE CurMode    = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
	BYTE CurStatus  = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status;

	int AMC_Pause_Flag = m_pHWSet->GetAMCPauseStatus();

	// Mode, Status, Cmd ID는 생략

    // Node 정보
	m_OHTMainStatus->StatusCommon.CurNodeID = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
	m_OHTMainStatus->StatusCommon.CurNodeOffset = m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset;

	// Station 정보
	m_OHTMainStatus->StatusCommon.CurStationID = m_ExecuteInfo->ExecutePositionInfo.CurrStation;
	m_OHTMainStatus->StatusCommon.StopNodeID = m_ExecuteInfo->ExecutePositionInfo.CurrCommandedNode;
	m_OHTMainStatus->StatusCommon.StopStationID = m_ExecuteInfo->ExecutePositionInfo.CurrCommandedStation;

	// StopStationOffset 정보 누락
	//m_OHTMainStatus->StatusCommon.StopStationOffset;



    // -------------------------------------------
    // [BEGIN] by zzang9un 2014.02.04 : ExistFoup 판단
	switch(CurMode)
    {
    case MODE_MANUAL: // Manual Mode
        cPreStatus_Trans = STS_INIT;
        break;

    case MODE_AUTO: // Auto Mode
        if(CurStatus == STS_UNLOADED)
			cPreStatus_Trans = STS_UNLOADED;
#if(OHT_NAME != OHT_NAME_P4_MASK)
		else if ((m_pHWSet->m_pHand->IsGoSigOn() == true) && (m_pHWSet->m_pHand->IsRightGripOn() == true) &&
			(m_pHWSet->m_pHand->IsLeftGripOn() == true) && (m_pHWSet->m_pHand->IsClose() == true) &&
			(m_pHWSet->m_pHand->IsOpen() == false) && (m_pHWSet->m_pShutter->IsClose(FRONT) == true) &&
			(m_pHWSet->m_pShutter->IsClose(REAR) == true) &&
			((CurStatus == STS_GOING) || (CurStatus == STS_ARRIVED)))
#else
		else if ((m_pHWSet->m_pHand->IsGoSigOn() == true) &&
			(m_pHWSet->m_pHand->IsRightGripOn() == true) && (m_pHWSet->m_pHand->IsLeftGripOn() == true) &&
			(m_pHWSet->m_pHand->IsHome() == true) && (m_pHWSet->m_pHand->IsWork() == false) &&
			(m_pHWSet->m_pShutter->IsClose(FRONT) == true) &&
			((m_pHWSet->m_pHoistAxis->IsHoistEUVHome() == true) || (m_pHWSet->m_pHoistAxis->IsHoistRSP200Home() == true) ||
			 (m_pHWSet->m_pHoistAxis->IsHoistRSP150Home() == true)) &&
			(m_pHWSet->m_pShutter->IsPodPlacement3On() == true) && (m_pHWSet->m_pShutter->IsPodPlacement4On() == true) &&
			((CurStatus == STS_GOING) || (CurStatus == STS_ARRIVED)))
#endif
            cPreStatus_Trans = STS_UNLOADED;
        else if((CurStatus != STS_GOING) && (CurStatus != STS_ARRIVED))
			cPreStatus_Trans = STS_INIT;
#if(OHT_NAME != OHT_NAME_P4_MASK)
        else if((m_pHWSet->m_pHand->IsGoSigOn() == true) && (m_pHWSet->m_pHand->IsRightGripOn() == false) &&
			(m_pHWSet->m_pHand->IsLeftGripOn() == false) && (m_pHWSet->m_pHand->IsClose() == false) &&
			(m_pHWSet->m_pHand->IsOpen() == true) && (m_pHWSet->m_pHand->IsInposition() == true))
#else
		else if((m_pHWSet->m_pHand->IsGoSigOn() == true) &&
			(m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false) &&
			(m_pHWSet->m_pHand->IsHome() == true) && (m_pHWSet->m_pHand->IsWork() == false) &&
			(m_pHWSet->m_pHoistAxis->IsHoistEUVHome() == false) && (m_pHWSet->m_pHoistAxis->IsHoistRSP200Home() == false) &&
			(m_pHWSet->m_pHoistAxis->IsHoistRSP150Home() == false) &&
			(m_pHWSet->m_pShutter->IsPodPlacement3On() == false) && (m_pHWSet->m_pShutter->IsPodPlacement4On() == false) &&
			(m_pHWSet->m_pHand->IsInposition() == true))
#endif
            cPreStatus_Trans = STS_INIT;

        break;
    }

    if(m_pHWSet->m_pHand->IsHomingState() == false)
	{
#if(OHT_NAME != OHT_NAME_P4_MASK)
		if((m_pHWSet->m_pHand->IsLeftGripOn() == true) || (m_pHWSet->m_pHand->IsRightGripOn() == true))
#else
        if((m_pHWSet->m_pHand->IsLeftGripOn() == true) || (m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pInnerFoup->IsPODDetect() == true))
#endif
		{
#if(OHT_NAME != OHT_NAME_P4_MASK)
			if((m_pHWSet->m_pHand->IsClose() == false) && (m_pHWSet->m_pHand->IsOpen() == false))
#else
			if((m_pHWSet->m_pHand->IsWork() == false) && (m_pHWSet->m_pHand->IsHome() == false) )
#endif
                IsGripFoup = IsPreGripFoup;
            else
				IsGripFoup = ON;
        }
        else
			IsGripFoup = OFF;

        // 주행중에 Hand Alarm 발생시 -> Hoist align sensor가 정위치일때는 이전 data사용
        if((CurMode != MODE_MANUAL) && (CurStatus == STS_GOING))
		{
#if(OHT_NAME != OHT_NAME_P4_MASK)
			if((m_pHWSet->m_pHand->IsAlarm() == true) || (m_pHWSet->m_pHand->IsGoSigOn() == true) ||
				((m_pHWSet->m_pHand->IsClose() == true) && (m_pHWSet->m_pHand->IsOpen() == true)))
#else
			if((m_pHWSet->m_pHand->IsAlarm() == true) || (m_pHWSet->m_pHand->IsGoSigOn() == true) ||
				((m_pHWSet->m_pHand->IsWork() == true) && (m_pHWSet->m_pHand->IsHome() == true)))
#endif
			{
				IsGripFoup = IsPreGripFoup;
            }
        }
        else if (cPreStatus_Trans == STS_UNLOADED)
            IsGripFoup = true;
    }
    else
        IsGripFoup = IsPreGripFoup;

    // Foup 유무
	m_OHTMainStatus->StatusCommon.ExistFoup = IsGripFoup;

    // 이전 Grip 상태에 반영
	IsPreGripFoup = IsGripFoup;
    // [END] by zzang9un 2014.02.04 : ExistFoup 판단
    // -------------------------------------------

    // -------------------------------------------
    // [BEGIN] by zzang9un 2014.02.04 : Carrier Type 판단
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82))
	if(IsGripFoup)
	{
        if(m_pHWSet->m_pInnerFoup->IsFoupDetect())
		{
			if(m_defualtparam->nCarrierType == MAC_OHT)
				m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_MAC;
			else
				m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_FOUP;//m_pTransControl->m_pParamSet->CarrierType;
       	}
        else
       	{
            m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_NOTDEFINED;
       	}
    }
    else
   	{
        m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_NOTDEFINED;
   	}
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
	if(IsGripFoup)
	{
		if(m_pHWSet->m_pInnerFoup->IsFoupDetect())
		{
			m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_POD; //m_pTransControl->m_pParamSet->CarrierType;
		}
		else
		{
			m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_NOTDEFINED;
		}
	}
	else
	{
		m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_NOTDEFINED;
	}
#elif(((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	if(IsGripFoup)
	{
		if(m_pHWSet->m_pInnerFoup->IsEuvDetect())
		{
			m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_EUV;
		}
		else if(m_pHWSet->m_pInnerFoup->IsFoupDetect())
		{
			//m_OHTMainStatus->StatusCommon.CarrierType = m_pTransControl->m_pParamSet->CarrierType;
            m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_POD;
		}
		else
		{
			m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_NOTDEFINED;
		}
	}
	else
	{
		m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_NOTDEFINED;
	}
#elif(OHT_NAME == OHT_NAME_P4_MASK)
	if(IsGripFoup)
	{
		if(m_pHWSet->m_pShutter->IsPodPlacement3On() && m_pHWSet->m_pShutter->IsPodPlacement4On())
		{
			if(m_pHWSet->m_pHoistAxis->IsHoistRSP150Home() == true)
				m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_POD;
			else if(m_pHWSet->m_pHoistAxis->IsHoistRSP200Home() == true)
				m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_RSP200;
			else if(m_pHWSet->m_pHoistAxis->IsHoistEUVHome() == true)
				m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_EUV;
			else
				m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_NOTDEFINED;
		}
		else
		{
			m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_NOTDEFINED;
		}
	}
	else
	{
		m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_NOTDEFINED;
	}
#else
    m_OHTMainStatus->StatusCommon.CarrierType = CARRIER_NOTDEFINED;
#endif
    // [END] by zzang9un 2014.02.04 : Carrier Type 판단
    // -------------------------------------------

    // -------------------------------------------
    // [BEGIN] by zzang9un 2014.02.04 : Pause Status 판단
    if((m_pHWSet->m_pOHTDetect->GetStatus() == DETECT_AND_STOP) || (m_pHWSet->m_pObstacle->GetStatus() == DETECT_AND_STOP))
    {
        m_OHTMainStatus->StatusCommon.PauseStatus = 0x01;
    }
    else if((AMC_Pause_Flag == STATUS_OHT_PAUSED)
		&& ((m_pHWSet->m_pOHTDetect->GetStatus() == DETECT_AND_MOVE) || (m_pHWSet->m_pObstacle->GetStatus() == DETECT_AND_MOVE)))
   	{
        m_OHTMainStatus->StatusCommon.PauseStatus = 0x02;
   	}
    else if((AMC_Pause_Flag == STATUS_OHT_PAUSED)
		&& ((m_pHWSet->m_pOHTDetect->GetStatus() == DETECT_NONE) || (m_pHWSet->m_pObstacle->GetStatus() == DETECT_NONE)))
   	{
		m_OHTMainStatus->StatusCommon.PauseStatus = 0x03;
   	}
    else
    {
        m_OHTMainStatus->StatusCommon.PauseStatus = 0x00;
    }
    // [END] by zzang9un 2014.02.04 : Pause Status 판단
    // -------------------------------------------

    // -------------------------------------------
    // [BEGIN] by zzang9un 2014.02.04 : Steering Position 판단
	char cSteerPos = STEERING_NONE;
    if ((m_pHWSet->m_pSteer->IsLeftPos(FRONT) == true) && (m_pHWSet->m_pSteer->IsLeftPos(REAR) == true))
        cSteerPos = STEERING_LEFT; // left
    else if ((m_pHWSet->m_pSteer->IsRightPos(FRONT) == true) && (m_pHWSet->m_pSteer->IsRightPos(REAR) == true))
		cSteerPos = STEERING_RIGHT; // right
    else
        cSteerPos = STEERING_MOVING; // moving

    m_OHTMainStatus->StatusCommon.SteerPosition = cSteerPos;
    // [END] by zzang9un 2014.02.04 : Steering Position 판단
    // -------------------------------------------

    // Error 정보
    // by zzang9un 2012.12.02 : 에러 코드 출력을 위해 Execute 에러코드로 변경
    m_OHTMainStatus->StatusCommon.ErrorCode = m_OHTMainStatus->ErrCode_StatusChk;

    // -------------------------------------------
    // [BEGIN] by zzang9un 2014.02.04 : Origin Info 판단

// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate Origin 신호 처리 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))
	m_OHTMainStatus->StatusCommon.OriginInfo.Rotate = m_pHWSet->m_pRotate->IsOrigin();
#else
	m_OHTMainStatus->StatusCommon.OriginInfo.Rotate = 0;
#endif

    // by zzang9un 2012.11.24 : Hand Origin을 알아오는 방법 확인 필요
    if(m_pHWSet->m_pHand->ReadyToOperation() == NO_ERR)
		m_OHTMainStatus->StatusCommon.OriginInfo.Hand = true;
    else
        m_OHTMainStatus->StatusCommon.OriginInfo.Hand = false;

    // by zzang9un 2012.11.24 : Shutter Origin을 알아오는 방법 확인 필요
    if(m_pHWSet->m_pShutter->ReadyToOperation() == NO_ERR)
        m_OHTMainStatus->StatusCommon.OriginInfo.Shutter = true;
    else
        m_OHTMainStatus->StatusCommon.OriginInfo.Shutter = false;
    // [END] by zzang9un 2014.02.04 : Origin Info 판단
	// -------------------------------------------

	//2022.12.27 sseung  분기 정보와 노드정보 동기화를 위해 Common에서도 전송
	if(m_ExecuteInfo->nCheckDivInfo == PATH_DIVERGENCE_LEFT)
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnRight = OFF;    //하위의 Map 정보에 의해 처리할 것.
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnLeft = ON;
		//m_OHTMainStatus->TurnOffInfo.VHL_Div = 0x02;
	}
	else if(m_ExecuteInfo->nCheckDivInfo == PATH_DIVERGENCE_RIGHT)
	{
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnRight = ON;    //하위의 Map 정보에 의해 처리할 것.
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnLeft = OFF;
		//m_OHTMainStatus->TurnOffInfo.VHL_Div = 0x02;
	}
	else
	{
		//왼쪽 분기   ----처리 필요
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnRight = OFF;    //하위의 Map 정보에 의해 처리할 것.
		m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnLeft = OFF;
//		m_OHTMainStatus->TurnOffInfo.VHL_Div = 0x02;
	}
}

/**
@brief   HID의 데이터를 Check하여 내부 에러 코드가 있는지, 픽업 온도가 리미트가 넘지 않는지 체크하는 함수
@author  LSJ
@date    2012.11.09
*/
int StatusCheckThread::CheckHID()
{
	int nError = NO_ERR;
    static int nPickupErrCount = 0;
	static int nPickupErrCount_FDC = 0;
//    static int nErrCount = 0;
	static unsigned long nPrePickupErrTime = 0;
	static unsigned long nPrePickupErrTime_FDC = 0;
	static int preErrorcode = 0;

	if (m_OHTMainStatus->m_cpMonitorData.HIDData.ErrorCode != 0)
	{
		if(preErrorcode != m_OHTMainStatus->m_cpMonitorData.HIDData.ErrorCode)
		{
			LOG_ERR_HID("HID Error Code :[%d]",m_OHTMainStatus->m_cpMonitorData.HIDData.ErrorCode);
		}

		switch(m_OHTMainStatus->m_cpMonitorData.HIDData.ErrorCode)
		{
			case ERR_EDC_UNDERVOLTAGE:
			case ERR_EDC_LOWVOLTAGE:
			case ERR_EOUT_UNDERVOLTAGE:
			case ERR_EOUT_LOWVOLTAGE:
			case ERR_ECAP_UNDERVOLTAGE:
			case ERR_ECAP_LOWERVOLTAGE:
			case ERR_PICKUP_OVERTEMP:
			case ERR_IGBT_OVERTEMP:
			case ERR_ECAP_OVERTEMP:
			case ERR_SEG_GEN_SIGNAL:
			case ERR_MULTI_SMPS_FAULT:
			case ERR_PEAKCURRENT_WARN:
			case ERR_REGUL_OPEN:
				nError = NO_ERR;
				break;
			case ERR_CAP_BANK_DISCONNECT:
			{
				if(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO)
					nError = NO_ERR;    //2025.04.28 sseung Cap Bank Error는 Manual에서만 나도록 요청 - 강동필 프로
				else
					nError = m_OHTMainStatus->m_cpMonitorData.HIDData.ErrorCode;
				break;
			}
			default:
				nError = m_OHTMainStatus->m_cpMonitorData.HIDData.ErrorCode;
				break;
		}

		preErrorcode = m_OHTMainStatus->m_cpMonitorData.HIDData.ErrorCode;

		return nError;
	}

	///@todo 파라미터 어디다 넣어야 되나??
        // 그냥 5번 이상이면 에러다 ? 5분 안에 5번 체크되면에러 발생시킴
	if ((float)m_OHTMainStatus->m_cpMonitorData.HIDData.PickupTemperature > PICKUPTEMPERLIMIT)
	{

        if (nPickupErrCount == 0)
		{
            nPrePickupErrTime = timeGetTime();
            nPickupErrCount++;
        }
        else
		{
            if (OHTUTILITY::PreventOverflowGetTimeMs(nPrePickupErrTime) > 3000)
                nPickupErrCount++;
        }

        if (nPickupErrCount > 5)
        {

			ADD_LOG_EXTRAJOB("PICKUPTEMPER:%d",m_OHTMainStatus->m_cpMonitorData.HIDData.PickupTemperature);
			nError = ERR_TEMPER_OVERTEMPERATURE;
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_TEMPER_OVERTEMPERATURE,
						m_OHTMainStatus->StatusCommon.CurNodeID,
						m_ExecuteInfo->ExecutePositionInfo.NextNode,
						m_OHTMainStatus->StatusCommon.CurNodeOffset,
						m_OHTMainStatus->StatusCommon.ExistFoup,
						m_OHTMainStatus->StatusCommon.StopStationID);
		}
	}else
	{
     	nPickupErrCount = 0;
    }

	if ((float)m_OHTMainStatus->m_cpMonitorData.HIDData.PickupTemperature > 50.0)
	{
		if (nPickupErrCount_FDC == 0)
		{
			nPrePickupErrTime_FDC = timeGetTime();
			nPickupErrCount_FDC++;
		}
		else
		{
			if (OHTUTILITY::PreventOverflowGetTimeMs(nPrePickupErrTime_FDC) < 60000)
				nPickupErrCount_FDC++;
            else
				nPickupErrCount_FDC = 0;
		}

		if (nPickupErrCount_FDC > 5)
		{
			nPickupErrCount_FDC = 0;

			SEND_WARN("WARN/39/6/%06d/%06d/%d/%d/%06d/%f",
						m_OHTMainStatus->StatusCommon.CurNodeID,
						m_ExecuteInfo->ExecutePositionInfo.NextNode,
						m_OHTMainStatus->StatusCommon.CurNodeOffset,
						m_OHTMainStatus->StatusCommon.ExistFoup,
						m_OHTMainStatus->StatusCommon.StopStationID,
						(float)m_OHTMainStatus->m_cpMonitorData.HIDData.PickupTemperature);

			SEND_WARN("WARN/40/6/%06d/%06d/%d/%d/%06d/%f",
						m_OHTMainStatus->StatusCommon.CurNodeID,
						m_ExecuteInfo->ExecutePositionInfo.NextNode,
						m_OHTMainStatus->StatusCommon.CurNodeOffset,
						m_OHTMainStatus->StatusCommon.ExistFoup,
						m_OHTMainStatus->StatusCommon.StopStationID,
						(float)m_OHTMainStatus->m_cpMonitorData.HIDData.PickupTemperature);
		}
	}


	return nError;
}


/**
@brief   OHT의 Status에 따라서 Lamp 상태를 바꾸는 함수
@author  LSJ
@date    2012.11.09
@return  nLampStatus 리턴
@Modify  P-PJT P1_2L OHT I/O추가에 따른 Lamp동작 변경 #JRespect 2018.01.03
*/
void StatusCheckThread::ControlLamp()
{
    int nLampStatus = 0;
	int nRedOut = 0, nGreenOut = 0;
	//int nYellowOut = 0;
	int nYellowRightOut = 0,  nYellowLeftOut = 0;
	static int nPreLampStatus = -1;

#if(OHT_NAME != OHT_NAME_P4_MASK)
	nRedOut     = m_pHWSet->m_pLamp->IsLampOn(DEF_RED);
	nGreenOut   = m_pHWSet->m_pLamp->IsLampOn(DEF_GREEN);
#else
	nRedOut = (m_pHWSet->m_pLamp->IsLampOn(DEF_RED_RIGHT)||m_pHWSet->m_pLamp->IsLampOn(DEF_RED_LEFT));
	nGreenOut = (m_pHWSet->m_pLamp->IsLampOn(DEF_GREEN_RIGHT)||m_pHWSet->m_pLamp->IsLampOn(DEF_GREEN_LEFT));
#endif

	nYellowLeftOut = m_pHWSet->m_pLamp->IsLampOn(DEF_YELLOW_LEFT);
	nYellowRightOut = m_pHWSet->m_pLamp->IsLampOn(DEF_YELLOW_RIGHT);

    m_TPESTOP = false;
	m_CheckLampYF = false;

	if((m_ExecuteInfo->bTpPause == true) &&
	   (m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status !=STS_ERROR))
	{
		m_TPESTOP = true;
		nLampStatus =  SETLAMP_G_FLICKER_Y_FLICKER;
		//DETECT_LOG("bTpPause");
    }
	else if(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL)
	{
		switch(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status)
		{
		case STS_ERROR:
			nLampStatus = SETLAMP_R_FLICKER;
			break;
		case STS_INIT:
			if (m_OHTMainStatus->ErrCode_StatusChk == 0)
			   {
				nLampStatus = SETLAMP_Y_FLICKER;
			   }
			else
				nLampStatus = SETLAMP_R_FLICKER;
			break;
        default:
            nLampStatus = SETLAMP_Y_FLICKER;
            break;
        }
    }
    else
    {
        switch(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status)
		{
		case STS_ARRIVED:
		case STS_INIT:
		case STS_LOADED:
		case STS_UNLOADED:
		case STS_AUTO_TAUGHT:
			if(m_OHTMainStatus->bRealGoCycleing == true)
			{
				//nLampStatus = SETLAMP_G_FLICKER;
				if(m_pHWSet->m_pSteer->IsFrontLeftOn() && m_pHWSet->m_pSteer->IsRearLeftOn())
				{
					nLampStatus = SETLAMP_G_FLICKER_Y_L_FLICKER;
					//DETECT_LOG("SETLAMP_G_FLICKER_Y_L_FLICKER");
				}
				else if(m_pHWSet->m_pSteer->IsFrontRightOn() && m_pHWSet->m_pSteer->IsRearRightOn())
				{
					nLampStatus = SETLAMP_G_FLICKER_Y_R_FLICKER;
					//DETECT_LOG("SETLAMP_G_FLICKER_Y_R_FLICKER");
                }
				else nLampStatus = SETLAMP_Y_FLICKER;
					//DETECT_LOG("SETLAMP_Y_FLICKER");

				m_CheckLampYF= true;
			}
			else
				nLampStatus = SETLAMP_Y_ON;
		break;

 		case STS_AUTOPOSITION:
		case STS_TURN_OFF: // 기동오프
		case STS_AUTORECOVERY:
		case STS_AUTORECOVERY_DOUBLETRANS:
		case STS_AUTORECOVERY_BLANKTRANS:
		case STS_AUTORECOVERY_OVERRUN:
			nLampStatus = SETLAMP_Y_ON;
			break;
        case STS_GOING:
				//nLampStatus = SETLAMP_G_FLICKER;
				if(m_pHWSet->m_pSteer->IsFrontLeftOn() && m_pHWSet->m_pSteer->IsRearLeftOn())
					nLampStatus = SETLAMP_G_FLICKER_Y_L_FLICKER;
				else if(m_pHWSet->m_pSteer->IsFrontRightOn() && m_pHWSet->m_pSteer->IsRearRightOn())
					nLampStatus = SETLAMP_G_FLICKER_Y_R_FLICKER;
				else nLampStatus = SETLAMP_Y_FLICKER;

				m_CheckLampYF = true;

			break;

        case STS_LOADING:
		case STS_UNLOADING:
		case STS_AUTO_TEACHING:
		case STS_AUTO_TEACHING_NULLTYPE:
		case STS_AUTO_TRANS_HOMING:
            nLampStatus = SETLAMP_G_ON;
            break;
		case STS_ERROR:
            nLampStatus = SETLAMP_R_FLICKER;
            break;
        }
    }

    switch (nLampStatus)
    {
    case SETLAMP_ALL_OFF:
		//if ((nRedOut == 1) || (nYellowOut == 1) || (nGreenOut == 1))
		if ((nRedOut == 1) || (nYellowLeftOut == 1) || (nGreenOut == 1) || (nYellowRightOut == 1))
            m_pHWSet->m_pLamp->LampOff();
        break;
    case SETLAMP_Y_ON:
		//if ((nYellowOut == 0) || (nPreLampStatus != nLampStatus))
		if ((nYellowRightOut == 0) || (nYellowLeftOut == 0) || (nPreLampStatus != nLampStatus))
        {
            m_pHWSet->m_pLamp->LampOff();
            m_pHWSet->m_pLamp->LampOn(nLampStatus);
        }
        break;
    case SETLAMP_G_ON:
        if ((nGreenOut == 0) || (nPreLampStatus != nLampStatus))
        {
			m_pHWSet->m_pLamp->LampOff();
            m_pHWSet->m_pLamp->LampOn(nLampStatus);
        }
		break;
	case SETLAMP_Y_FLICKER:
		//if ((nYellowOut == 0) && (nPreLampStatus == nLampStatus))
		if ((nYellowRightOut == 0) && (nYellowLeftOut == 0) && (nPreLampStatus == nLampStatus))
			m_pHWSet->m_pLamp->LampOn(nLampStatus);
		else
			m_pHWSet->m_pLamp->LampOff();
		break;
	case SETLAMP_G_FLICKER:
		if ((nGreenOut == 0) && (nPreLampStatus == nLampStatus))
            m_pHWSet->m_pLamp->LampOn(nLampStatus);
        else
            m_pHWSet->m_pLamp->LampOff();
        break;

	case SETLAMP_R_FLICKER:

        if ((nRedOut == 0) && (nPreLampStatus == nLampStatus))
            m_pHWSet->m_pLamp->LampOn(nLampStatus);
        else
            m_pHWSet->m_pLamp->LampOff();
		break;

	case SETLAMP_G_FLICKER_Y_FLICKER:
		//if ((nGreenOut == 0) && (nYellowOut == 0) && (nPreLampStatus == nLampStatus))
		if ((nGreenOut == 0) && (nYellowLeftOut == 0) && (nYellowRightOut == 0) && (nPreLampStatus == nLampStatus))
			m_pHWSet->m_pLamp->LampOn(nLampStatus);
        else
			m_pHWSet->m_pLamp->LampOff();
		break;
	// Green 점멸, Yellow_LEFT 점멸
	case SETLAMP_G_FLICKER_Y_L_FLICKER:
		if ((nGreenOut == 0) && (nPreLampStatus == nLampStatus))
			m_pHWSet->m_pLamp->LampOn(nLampStatus);
		else
			m_pHWSet->m_pLamp->LampOff();
		break;
	// Green 점멸, Yellow_RIGHT 점멸
	case SETLAMP_G_FLICKER_Y_R_FLICKER:
		if ((nGreenOut == 0) && (nPreLampStatus == nLampStatus))
			m_pHWSet->m_pLamp->LampOn(nLampStatus);
		else
			m_pHWSet->m_pLamp->LampOff();
		break;
	}

    nPreLampStatus = nLampStatus;
}
//---------------------------------------------------------------------------

/**
@brief   OHT의 Status에 따라서 Lamp 상태를 바꾸는 함수
@author  LSJ
@date    2012.11.09
@return  nLampStatus 리턴
@Modify  P-PJT P1_2L OHT I/O추가에 따른 Lamp동작 변경 #JRespect 2018.01.03
*/
void StatusCheckThread::ControlYellowLamp()
{
    int nLampStatus = 0;
	int nRedOut = 0, nGreenOut = 0;
	//int nYellowOut = 0;
	int nYellowRightOut = 0,  nYellowLeftOut = 0;
	static int nPreLampStatus = -1;

	nYellowLeftOut = m_pHWSet->m_pLamp->IsLampOn(DEF_YELLOW_LEFT);
	nYellowRightOut = m_pHWSet->m_pLamp->IsLampOn(DEF_YELLOW_RIGHT);

	if(m_pHWSet->m_pSteer->IsFrontLeftOn() && m_pHWSet->m_pSteer->IsRearLeftOn())
		nLampStatus = SETLAMP_G_FLICKER_Y_L_FLICKER;

	else if(m_pHWSet->m_pSteer->IsFrontRightOn() && m_pHWSet->m_pSteer->IsRearRightOn())
		nLampStatus = SETLAMP_G_FLICKER_Y_R_FLICKER;


    switch (nLampStatus)
	{
	// Green 점멸, Yellow_LEFT 점멸
	case SETLAMP_G_FLICKER_Y_L_FLICKER:
		if ((nYellowLeftOut == 0) && (nPreLampStatus == nLampStatus))
			m_pHWSet->m_pLamp->LampYellowSteerOn(nLampStatus);
		else
			m_pHWSet->m_pLamp->LampOff_Yellow();
		break;
	// Green 점멸, Yellow_RIGHT 점멸
	case SETLAMP_G_FLICKER_Y_R_FLICKER:
		if ((nYellowRightOut == 0) && (nPreLampStatus == nLampStatus))
			m_pHWSet->m_pLamp->LampYellowSteerOn(nLampStatus);
		else
			m_pHWSet->m_pLamp->LampOff_Yellow();
		break;
    }

    nPreLampStatus = nLampStatus;
}
//---------------------------------------------------------------------------

void StatusCheckThread::ControlSound_ATLAS()
{
	AnsiString strLog = "";
	static AnsiString strPreLog = "";

	static int nCheckOBSSound = 0;

	static bool bDetectStartSound = false;

	if(m_ExecuteInfo->bTpPause == true)
	{
		if((m_bPlayingSound == false) || (m_PlaySoundNum != PAUSE_SOUND))
		{
			m_pHWSet->SetSoundCommand(PAUSE_SOUND);
			strLog = "TP Pause (UBG On)";
			// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
			if(strLog != strPreLog)
			{
				strPreLog = strLog;
			}
			m_bPlayingSound = true;
			m_PlaySoundNum = PAUSE_SOUND;
		}
	}
	else
	{
    	switch(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status)
		{
		case STS_ARRIVED:
		case STS_LOADED:
		case STS_UNLOADED:
		case STS_INIT:
		case STS_AUTOPOSITION:
		case STS_COMPLETE:
		case STS_TURN_OFF:
			if(true)
			{
				m_pHWSet->SetSoundCommand(NO_SOUND);

				m_bPlayingSound = false;
				m_PlaySoundNum = NO_SOUND;
			}
			break;

		case STS_GOING:
			if(m_pHWSet->m_pObstacle->GetStatus() == DETECT_AND_STOP)
			{
				// 2013.10.22 By LSJ OBS만 감지 되었을 때에만 OBS 소리 나도록 변경
				//if(m_pHWSet->m_pOHTDetect->GetStatus() == DETECT_AND_STOP)
				if(m_pHWSet->m_pOHTDetect->GetStatus() != DETECT_NONE)
				{
					// 장애물 & UBG 모두 감지된 경우
					m_pHWSet->SetSoundCommand(NO_SOUND);

					m_bPlayingSound = false;
					m_PlaySoundNum = NO_SOUND;
					strLog = "Sound Off(PBS, UBG On)";
					// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
					if(strLog != strPreLog)
					{
						strPreLog = strLog;
					}

					nCheckOBSSound = 0;
				}
				else
				{
					// 장애물만 감지된 경우

					nCheckOBSSound++;
					if(nCheckOBSSound > 5)
					{
						if((m_bPlayingSound == false) || (m_PlaySoundNum != OBSTACLE_SOUND))
						{
							 // 장애물 사운드
						 	if(m_pTransControl->m_pParamSet->TransUsePart.ObsDetectSound == true)
							{
								m_pHWSet->SetSoundCommand(OBSTACLE_SOUND);

								strLog = "OBS Sound On(PBS On)";
								// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
								if(strLog != strPreLog)
								{
									strPreLog = strLog;
								}
								m_bPlayingSound = true;
								m_PlaySoundNum = OBSTACLE_SOUND;
							}
							else
							{
								// 주행 Sound 옵션이 off된 경우는 Sound를 Off한다.

								m_pHWSet->SetSoundCommand(NO_SOUND);

								strLog = "Sound Off(DrivingSound Off)";
								// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
								if(strLog != strPreLog)
								{
									strPreLog = strLog;
								}
								m_bPlayingSound = false;
								m_PlaySoundNum = NO_SOUND;
							}
						}
                    }

				}
			}
			else
			{
				nCheckOBSSound = 0;

				if(m_pHWSet->m_pOHTDetect->GetStatus() == DETECT_AND_STOP)
				{
					// UBG만 감지된 경우
					if((m_bPlayingSound == false) || (m_PlaySoundNum != OHT_DETECT_SOUND))
					{
						if(m_pTransControl->m_pParamSet->TransUsePart.OhtDetectSound == true)
						{
							m_pHWSet->SetSoundCommand(OHT_DETECT_SOUND);
							strLog = "OHT DETECT Sound On(UBG On)";

							// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
							if(strLog != strPreLog)
							{
								strPreLog = strLog;
							}
							m_bPlayingSound = true;
							m_PlaySoundNum = OHT_DETECT_SOUND;
						}
						else
						{
							// 주행 Sound 옵션이 off된 경우는 Sound를 Off한다.

							m_pHWSet->SetSoundCommand(NO_SOUND);

							strLog = "Sound Off(DrivingSound Off)";
							// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
							if(strLog != strPreLog)
							{
								strPreLog = strLog;
							}
							m_bPlayingSound = false;
							m_PlaySoundNum = NO_SOUND;
						}
					}
				}
				else
				{
						// 장애물, UBG 모두 감지되지 않은 경우, 일반 주행 Sound
					if((m_bPlayingSound == false) || (m_PlaySoundNum != DRIVE_SOUND))
					{
						if(m_pTransControl->m_pParamSet->TransUsePart.DrivingSound == true)
						{
							if (m_pHWSet->m_pDrivingAxis->IsRunning() == true){

								m_pHWSet->SetSoundCommand(DRIVE_SOUND);
								strLog = "Driving Sound On";
								// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
								if(strLog != strPreLog)
								{
									strPreLog = strLog;
								}
								m_bPlayingSound = true;
								m_PlaySoundNum = DRIVE_SOUND;
							}
							else
							{
								strLog = "Driving Not Start";
								if(strLog != strPreLog)
								{
									strPreLog = strLog;
								}
							}
						}
						else
						{
							// 주행 Sound 옵션이 off된 경우는 Sound를 Off한다.

							m_pHWSet->SetSoundCommand(NO_SOUND);

							strLog = "Sound Off(DrivingSound Off)";
							// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
							if(strLog != strPreLog)
							{
								strPreLog = strLog;
							}
							m_bPlayingSound = false;
							m_PlaySoundNum = NO_SOUND;
						}
					}
				}
			}

			break;

		case STS_LOADING:
		case STS_UNLOADING:
			if((m_bPlayingSound == false) || (m_PlaySoundNum != TRANS_SOUND))
			{
				if(m_pTransControl->m_bTransSound_Use == true)
				{
					m_pHWSet->SetSoundCommand(TRANS_SOUND);

					strLog = "Trans Sound On";
					// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
					if(strLog != strPreLog)
					{
						strPreLog = strLog;
					}
					m_bPlayingSound = true;
					m_PlaySoundNum = TRANS_SOUND;
					m_pTransControl->m_bTransSound_Use = false;
				}
			}
			break;

		case STS_ERROR:
			if((m_bPlayingSound == false) || (m_PlaySoundNum != ERROR_SOUND))
			{
				m_pHWSet->SetSoundCommand(ERROR_SOUND);
				strLog = "Error Sound On";
				// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
				if(strLog != strPreLog)
				{
					strPreLog = strLog;
				}
				m_bPlayingSound = true;
				m_PlaySoundNum = ERROR_SOUND;
			}
			break;

		default:
			if(m_bPlayingSound) //AOHT-0002 : Sound
			{
				m_pHWSet->SetSoundCommand(NO_SOUND);
				m_bPlayingSound = false;
				m_PlaySoundNum = NO_SOUND;
			}
		}
	}
}

/**
@brief   OHT의 Status에 따라서 sound상태를 바꾸는 함수
@author  zzang9un
@date    2013.09.02
@return  nLampStatus 리턴
*/
void StatusCheckThread::ControlSound()
{
	AnsiString strLog = "";
	static AnsiString strPreLog = "";

	static int nCheckOBSSound = 0;

	static bool bDetectStartSound = false;

	if(m_ExecuteInfo->bTpPause == true)
	{
		if((m_bPlayingSound == false) || (m_PlaySoundNum != SETSOUND_PAUSE))
		{
			m_pHWSet->Sound_On(SETSOUND_PAUSE);

			strLog = "TP Pause (UBG On)";
			// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
			if(strLog != strPreLog)
			{
				strPreLog = strLog;
			}
			m_bPlayingSound = true;
			m_PlaySoundNum = SETSOUND_PAUSE;
		}
	}
	else
	{
    	switch(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status)
		{
		case STS_ARRIVED:
		case STS_LOADED:
		case STS_UNLOADED:
		case STS_INIT:
		case STS_AUTOPOSITION:
		case STS_COMPLETE:
		case STS_TURN_OFF:
			if(m_bPlayingSound)
			{
				m_pHWSet->Sound_Off();
				m_bPlayingSound = false;
				m_PlaySoundNum = SETSOUND_NONE;
			}
			break;

		case STS_GOING:
			if(m_pHWSet->m_pObstacle->GetStatus() == DETECT_AND_STOP)
			{
				// 2013.10.22 By LSJ OBS만 감지 되었을 때에만 OBS 소리 나도록 변경
				//if(m_pHWSet->m_pOHTDetect->GetStatus() == DETECT_AND_STOP)
				if(m_pHWSet->m_pOHTDetect->GetStatus() != DETECT_NONE)
				{
					// 장애물 & UBG 모두 감지된 경우
					m_pHWSet->Sound_Off();

					m_bPlayingSound = false;
					m_PlaySoundNum = SETSOUND_NONE;
					strLog = "Sound Off(PBS, UBG On)";
					// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
					if(strLog != strPreLog)
					{
						strPreLog = strLog;
					}

					nCheckOBSSound = 0;
				}
				else
				{
					// 장애물만 감지된 경우

					nCheckOBSSound++;
					if(nCheckOBSSound > 5)
					{
						if((m_bPlayingSound == false) || (m_PlaySoundNum != SETSOUND_OBS))
						{
						 // 장애물 사운드
						 	if(m_pTransControl->m_pParamSet->TransUsePart.ObsDetectSound == true)
							{
								m_pHWSet->Sound_On(SETSOUND_OBS);

								strLog = "OBS Sound On(PBS On)";
								// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
								if(strLog != strPreLog)
								{
									strPreLog = strLog;
								}
								m_bPlayingSound = true;
								m_PlaySoundNum = SETSOUND_OBS;
							}
							else
							{
								// 주행 Sound 옵션이 off된 경우는 Sound를 Off한다.

								m_pHWSet->Sound_Off();

								strLog = "Sound Off(DrivingSound Off)";
								// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
								if(strLog != strPreLog)
								{
									strPreLog = strLog;
								}
								m_bPlayingSound = false;
								m_PlaySoundNum = SETSOUND_NONE;
							}
						}
                    }

				}
			}
			else
			{
				nCheckOBSSound = 0;

				if(m_pHWSet->m_pOHTDetect->GetStatus() == DETECT_AND_STOP)
				{
					// UBG만 감지된 경우
					if((m_bPlayingSound == false) || (m_PlaySoundNum != SETSOUND_DETECT))
					{
						if(m_pTransControl->m_pParamSet->TransUsePart.OhtDetectSound == true)
						{
							m_pHWSet->Sound_On(SETSOUND_DETECT);            // Sound Off로 변경
							strLog = "OHT DETECT Sound On(UBG On)";

							// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
							if(strLog != strPreLog)
							{
								strPreLog = strLog;
							}
							m_bPlayingSound = true;
							m_PlaySoundNum = SETSOUND_DETECT;
						}
						else
						{
							// 주행 Sound 옵션이 off된 경우는 Sound를 Off한다.

							m_pHWSet->Sound_Off();

							strLog = "Sound Off(DrivingSound Off)";
							// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
							if(strLog != strPreLog)
							{
								strPreLog = strLog;
							}
							m_bPlayingSound = false;
							m_PlaySoundNum = SETSOUND_NONE;
						}
					}
				}
				else
				{
						// 장애물, UBG 모두 감지되지 않은 경우, 일반 주행 Sound
					if((m_bPlayingSound == false) || (m_PlaySoundNum != SETSOUND_DRIVING))
					{
						if(m_pTransControl->m_pParamSet->TransUsePart.DrivingSound == true)
						{
							if (m_pHWSet->m_pDrivingAxis->IsRunning() == true){
								m_pHWSet->Sound_On(SETSOUND_DRIVING);
								strLog = "Driving Sound On";
								// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
								if(strLog != strPreLog)
								{
									strPreLog = strLog;
								}
								m_bPlayingSound = true;
								m_PlaySoundNum = SETSOUND_DRIVING;
							}
							else
							{
								strLog = "Driving Not Start";
								if(strLog != strPreLog)
								{
//									SOUND_LOG("%s", strLog.c_str());

									strPreLog = strLog;
								}
							}
						}
						else
						{
							// 주행 Sound 옵션이 off된 경우는 Sound를 Off한다.

							m_pHWSet->Sound_Off();

							strLog = "Sound Off(DrivingSound Off)";
							// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
							if(strLog != strPreLog)
							{
								strPreLog = strLog;
							}
							m_bPlayingSound = false;
							m_PlaySoundNum = SETSOUND_NONE;
						}
					}
				}
			}

			break;

		case STS_LOADING:
		case STS_UNLOADING:
			if((m_bPlayingSound == false) || (m_PlaySoundNum != SETSOUND_TRANS))
			{
				if(m_pTransControl->m_bTransSound_Use == true)
				{
					m_pHWSet->Sound_On(SETSOUND_TRANS);
					strLog = "Trans Sound On";
					// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
					if(strLog != strPreLog)
					{
						strPreLog = strLog;
					}
					m_bPlayingSound = true;
					m_PlaySoundNum = SETSOUND_TRANS;
					m_pTransControl->m_bTransSound_Use = false;
				}
			}
			break;

		case STS_ERROR:
			if((m_bPlayingSound == false) || (m_PlaySoundNum != SETSOUND_ERROR))
			{
				m_pHWSet->Sound_On(SETSOUND_ERROR);
				strLog = "Error Sound On";
				// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
				if(strLog != strPreLog)
				{
					strPreLog = strLog;
				}
				m_bPlayingSound = true;
				m_PlaySoundNum = SETSOUND_ERROR;
			}
			break;

		default:
			if(m_bPlayingSound) //AOHT-0002 : Sound
			{
				m_pHWSet->Sound_Off();
				m_bPlayingSound = false;
				m_PlaySoundNum = SETSOUND_NONE;
			}
		}
	}
}

#define LOG_FOLDER_CHECK_PERIOD 60000
/**
@brief   현재 OHT의 상태를 TP에 보낼 구조체에 Update한다.
@author  zzang9un
@date    2012.11.23
*/
void StatusCheckThread::UpdateStatusForTP()
{
	int nInCount = 0, nOutCount = 0;
    int InputBuff[4] = {0,};
	int OutputBuff[4] = {0,};


	int PreNode = 0;
	BYTE CurModeTP    = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
	BYTE CurStatusTP  = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status;
	int nNodeIndexTP = 0;
	NODE_INFO	*pNODE_INFOTP		= NULL;	// Node 정보

	if(((CurStatusTP ==STS_RUNNING) || (CurStatusTP == STS_INIT) || (CurStatusTP ==STS_ARRIVED)|| (CurStatusTP ==STS_COMPLETE))
		&& (CurModeTP == MODE_MANUAL))
		{
			if(m_ExecuteInfo->ExecutePositionInfo.CurrNode != PreNode)
			{
				if(m_ExecuteInfo->ExecutePositionInfo.CurrNode != 0)
				{
					nNodeIndexTP = OHTMainForm->GetMap()->FindNodeIndex(m_ExecuteInfo->ExecutePositionInfo.CurrNode);
					pNODE_INFOTP = &(OHTMainForm->GetMap()->m_NodeArray[nNodeIndexTP]);		// Node 정보

					if(pNODE_INFOTP->InOutInfo ==EmergencyNodeMTL_NTYPE)
						m_OHTMainStatus->StatusTP.IsWarningState.EmergencyNodeMTL = true;
					else
						m_OHTMainStatus->StatusTP.IsWarningState.EmergencyNodeMTL = false;
				}

				PreNode = m_ExecuteInfo->ExecutePositionInfo.CurrNode ;
			}
		}

    if(OHTUTILITY::PreventOverflowGetTimeMs(m_OHTMainStatus->dwLogThreadTime) > 30000)
		m_OHTMainStatus->StatusTP.IsWarningState.IswarningLogThreadCycleTime =true;
	else
		m_OHTMainStatus->StatusTP.IsWarningState.IswarningLogThreadCycleTime =false; //flase

	m_OHTMainStatus->StatusTP.Data = m_OHTMainStatus->StatusCommon;


    // HID 정보
	m_OHTMainStatus->StatusTP.HID_Erec = m_OHTMainStatus->m_cpMonitorData.HIDData.Eout;
	m_OHTMainStatus->StatusTP.HID_Edc = m_OHTMainStatus->m_cpMonitorData.HIDData.Edc;
	m_OHTMainStatus->StatusTP.HID_Idc = m_OHTMainStatus->m_cpMonitorData.HIDData.Iout;
	m_OHTMainStatus->StatusTP.HID_Ecap = m_OHTMainStatus->m_cpMonitorData.HIDData.Ecap;
	m_OHTMainStatus->StatusTP.HID_Kw = (m_OHTMainStatus->m_cpMonitorData.HIDData.Iout * m_OHTMainStatus->m_cpMonitorData.HIDData.Edc);
	m_OHTMainStatus->StatusTP.HID_ErrorCode = m_OHTMainStatus->m_cpMonitorData.HIDData.ErrorCode;
	m_OHTMainStatus->StatusTP.HID_PickupTemper = m_OHTMainStatus->m_cpMonitorData.HIDData.PickupTemperature;

    // IO 정보
	m_pHWSet->GetIOSet(InputBuff, nInCount, OutputBuff, nOutCount);
	m_OHTMainStatus->StatusTP.Input[0] = InputBuff[0];
    m_OHTMainStatus->StatusTP.Input[1] = InputBuff[1];
    m_OHTMainStatus->StatusTP.Input[2] = InputBuff[2];
    m_OHTMainStatus->StatusTP.Output[0] = OutputBuff[0];
    m_OHTMainStatus->StatusTP.Output[1] = OutputBuff[1];
	m_OHTMainStatus->StatusTP.Output[2] = OutputBuff[2];

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.08.18 : Use 정보(Trans에 있는 Use만 설정됨)
    m_OHTMainStatus->StatusTP.UseInfo.Info.EQ_PIO = m_pTransControl->m_pParamSet->TransUsePart.EQPIO;
    m_OHTMainStatus->StatusTP.UseInfo.Info.GripperSen = m_pTransControl->m_pParamSet->TransUsePart.GripperSen;
	m_OHTMainStatus->StatusTP.UseInfo.Info.OscillationSensor = m_pTransControl->m_pParamSet->TransUsePart.OscillationSen;
#if(OHT_NAME != OHT_NAME_P4_MASK)
	m_OHTMainStatus->StatusTP.UseInfo.Info.HandPushSensorCheck = m_pTransControl->m_pParamSet->TransUsePart.HandPushSen;
#endif
    m_OHTMainStatus->StatusTP.UseInfo.Info.LookDownSensor = m_pTransControl->m_pParamSet->TransUsePart.LookdownSen;
    m_OHTMainStatus->StatusTP.UseInfo.Info.ShutterControl = m_pTransControl->m_pParamSet->TransUsePart.ShutterControl;
    m_OHTMainStatus->StatusTP.UseInfo.Info.HandControl = m_pTransControl->m_pParamSet->TransUsePart.HandSensor;
    m_OHTMainStatus->StatusTP.UseInfo.Info.HoistAlignSensor = m_pTransControl->m_pParamSet->TransUsePart.HoistHomeSen;
    m_OHTMainStatus->StatusTP.UseInfo.Info.STBSensor = m_pTransControl->m_pParamSet->TransUsePart.STBSen;
	m_OHTMainStatus->StatusTP.UseInfo.Info.UTBSensor = m_pTransControl->m_pParamSet->TransUsePart.UTBSen;
    m_OHTMainStatus->StatusTP.UseInfo.Info.UTBFoupCheck = m_pTransControl->m_pParamSet->TransUsePart.UTBFoupCheck;
    m_OHTMainStatus->StatusTP.UseInfo.Info.IgnorePIOError = m_pTransControl->m_pParamSet->TransUsePart.IgnorePIOError;
    m_OHTMainStatus->StatusTP.UseInfo.Info.TransSound = m_pTransControl->m_pParamSet->TransUsePart.TransSound;
    m_OHTMainStatus->StatusTP.UseInfo.Info.DrivingSound = m_pTransControl->m_pParamSet->TransUsePart.DrivingSound;
	m_OHTMainStatus->StatusTP.UseInfo.Info.ObsDetectSound = m_pTransControl->m_pParamSet->TransUsePart.ObsDetectSound;
	m_OHTMainStatus->StatusTP.UseInfo.Info.OhtDetectSound = m_pTransControl->m_pParamSet->TransUsePart.OhtDetectSound;
	m_OHTMainStatus->StatusTP.UseInfo.Info.TransEQSound = m_pTransControl->m_pParamSet->TransUsePart.TransEQSound;
	m_OHTMainStatus->StatusTP.UseInfo.Info.TransSTKSound = m_pTransControl->m_pParamSet->TransUsePart.TransSTKSound;
	m_OHTMainStatus->StatusTP.UseInfo.Info.TransSTBSound = m_pTransControl->m_pParamSet->TransUsePart.TransSTBSound;
    // [END] by zzang9un 2013.08.18 : Use 정보
    // -------------------------------------------

    // 모터 관련 정보
    ///@todo State Code와 Source Code는 어떻게 가져오는 지 협의 필요
    m_OHTMainStatus->StatusTP.Driving_F.Position = m_pHWSet->m_pDrivingAxis->GetCurrentPosition();
    m_OHTMainStatus->StatusTP.Driving_F.Speed = m_pHWSet->m_pDrivingAxis->GetCurrentVelocity()*1000;
    m_OHTMainStatus->StatusTP.Driving_F.ErrorCode = m_pHWSet->m_pDrivingAxis->GetError();
    m_OHTMainStatus->StatusTP.Driving_F.AMPEnable = m_pHWSet->m_pDrivingAxis->IsAMPEnable();
    m_OHTMainStatus->StatusTP.Driving_F.IsRunning = m_pHWSet->m_pDrivingAxis->IsRunning();

    m_OHTMainStatus->StatusTP.Driving_R.Position = m_pHWSet->m_pDrivingAxis->GetSlaveCurrentPosition();
    m_OHTMainStatus->StatusTP.Driving_R.Speed = m_pHWSet->m_pDrivingAxis->GetSlaveCurrentVelocity()*1000;
    m_OHTMainStatus->StatusTP.Driving_R.ErrorCode = m_pHWSet->m_pDrivingAxis->GetSlaveError();
    m_OHTMainStatus->StatusTP.Driving_R.AMPEnable = m_pHWSet->m_pDrivingAxis->IsSlaveAMPEnable();
    m_OHTMainStatus->StatusTP.Driving_R.IsRunning = m_pHWSet->m_pDrivingAxis->IsSlaveRunning();

	// by zzang9un 2013.09.09 : 단위 변경에 따른 변경 : mm(1/1000m) -> mm/10(1/10000m)
	m_OHTMainStatus->StatusTP.Hoist.Position = m_pHWSet->m_pHoistAxis->GetCurrentPosition() * 10;
	m_OHTMainStatus->StatusTP.dHoist_pulse = m_pHWSet->m_pHoistAxis->GetHoistPulseValue();

	m_OHTMainStatus->StatusTP.Hoist.Speed = m_pHWSet->m_pHoistAxis->GetCurrentVelocity()*1000;
	m_OHTMainStatus->StatusTP.Hoist.ErrorCode = m_pHWSet->m_pHoistAxis->GetError();
	m_OHTMainStatus->StatusTP.Hoist.AMPEnable = m_pHWSet->m_pHoistAxis->IsAMPEnable();
    m_OHTMainStatus->StatusTP.Hoist.IsRunning = m_pHWSet->m_pHoistAxis->IsRunning();

    // by zzang9un 2013.09.09 : 단위 변경에 따른 변경 : mm(1/1000m) -> mm/10(1/10000m)
    m_OHTMainStatus->StatusTP.Shift.Position = m_pHWSet->m_pShiftAxis->GetCurrentPosition() * 10;

    m_OHTMainStatus->StatusTP.Shift.Speed = m_pHWSet->m_pShiftAxis->GetCurrentVelocity()*1000;
    m_OHTMainStatus->StatusTP.Shift.ErrorCode = m_pHWSet->m_pShiftAxis->GetError();
    m_OHTMainStatus->StatusTP.Shift.AMPEnable = m_pHWSet->m_pShiftAxis->IsAMPEnable();
    m_OHTMainStatus->StatusTP.Shift.IsRunning = m_pHWSet->m_pShiftAxis->IsRunning();


    // 2016-11-28, Add by Labeler, ROHT의 경우, Rotate 관련 정보 Update
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))
    m_OHTMainStatus->StatusTP.Rotate.Position = m_pHWSet->m_pRotate->GetCurrentPositionDegree();
    m_OHTMainStatus->StatusTP.Rotate.Speed = m_pHWSet->m_pRotate->GetCurrentVelocity();
    m_OHTMainStatus->StatusTP.Rotate.ErrorCode = m_pHWSet->m_pRotate->GetError();
    m_OHTMainStatus->StatusTP.Rotate.AMPEnable = m_pHWSet->m_pRotate->IsAMPEnable();
    m_OHTMainStatus->StatusTP.Rotate.IsRunning = m_pHWSet->m_pRotate->IsRunning();
    m_OHTMainStatus->StatusTP.Rotate.IsOrigin = m_pHWSet->m_pRotate->IsOrigin();
#endif

    ///@todo 정의 안된 값들에는 default 값으로 처리했으나 나머지 처리 필요
    // UBG 관련
    m_OHTMainStatus->StatusTP.IsUBGSen = 1;
    m_OHTMainStatus->StatusTP.UBGDetectLevel = 0;

	m_OHTMainStatus->StatusTP.IsFCPause = m_pHWSet->GetAMCPauseStatus();
    m_OHTMainStatus->StatusTP.TPCommandType = 0x00000000;
    m_OHTMainStatus->StatusTP.Cmd_Count = 0;



	//2014.06.09 ji7412.lee AMC update 진행보고
	m_OHTMainStatus->StatusTP.AMCResponse = m_pHWSet->m_AMCResponse;
	//m_OHTMainStatus->StatusTP.AMCResponse = 0x00;

	m_OHTMainStatus->StatusTP.MapMakedLinkCount = OHTMainForm->GetMapMakeProcess()->GetCount();
	m_OHTMainStatus->StatusTP.MapMakedLinkDrivingCheckCount = OHTMainForm->GetMapMakeProcess()->GetCountMapMakeDriving();
	m_OHTMainStatus->StatusTP.DownPercent = 0x00;
	m_OHTMainStatus->StatusTP.CPUTemperature = m_OHTMainStatus->m_cpMonitorData.IPCData.dCPUTemperature;

    ///@todo AP 부분 구현을 위해 수정 필요
    //m_OHTMainStatus->StatusTP.APSignalLevel = ;
//  memcpy(m_OHTMainStatus->StatusTP.AMCVer, m_MonitorDataArray->APSignalData.szAPMacAddress, sizeof(m_MonitorDataArray->APSignalData.szAPMacAddress));
	m_OHTMainStatus->StatusTP.APSignalLevel = m_OHTMainStatus->m_cpMonitorData.APSignalData.nAPSignalLevel;

    for(int i = 0; i<6; i++)
    {
        m_OHTMainStatus->StatusTP.APMacAddress[i] = m_OHTMainStatus->m_cpMonitorData.APSignalData.szAPMacAddress[i];
    }

	static DWORD dwStTime = timeGetTime();

	//OCS 보고용 Satus 갱신 지연 현상 수
	// 2019.03 이곳에서 간헐적으로 SD카드 상태에 따라 딜레이가 심하게 걸리는 것 의심
	// 실행할때 한번만 하는 것으로
   //	if((OHTUTILITY::PreventOverflowGetTimeMs(dwStTime) > LOG_FOLDER_CHECK_PERIOD))
  //	{
  //		OHTMainForm->Set_LogFolder(OHTUTILITY::Check_LogFolder());
   //		dwStTime = timeGetTime();
   //	}

	m_OHTMainStatus->StatusTP.VolatageMonitor[0] = m_pHWSet->GetVolatageMonitor(0);
  	m_OHTMainStatus->StatusTP.VolatageMonitor[1] = m_pHWSet->GetVolatageMonitor(1);
	// 기타 정보
    m_OHTMainStatus->StatusTP.STBDirection = 0;
// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate Exist 처리
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))
    m_OHTMainStatus->StatusTP.IsRotateExist = 1;
#else
	m_OHTMainStatus->StatusTP.IsRotateExist = 0;
#endif

	//OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.DataType = OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.DataType | 0x02;
	//m_OHTMainStatus->StatusTP.IsRFPIORS23 = 0;
	m_OHTMainStatus->StatusTP.AdjustMapLoadInfo.Map_Adjust = ((OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.DataType & bit_0)==bit_0)? 1:0;
	m_OHTMainStatus->StatusTP.AdjustMapLoadInfo.Map_Adjust_Modify = ((OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.DataType & bit_1)==bit_1)? 1:0;
	m_OHTMainStatus->StatusTP.AdjustMapLoadInfo.Station_Adjust = ((OHTMainForm->GetStationDataManager_STD()->m_StationFileHeader.DataType & bit_0)==bit_0)? 1:0;
	m_OHTMainStatus->StatusTP.AdjustMapLoadInfo.Station_Adjust_Modify = ((OHTMainForm->GetStationDataManager_STD()->m_StationFileHeader.DataType & bit_1)==bit_1)? 1:0;
	m_OHTMainStatus->StatusTP.AdjustMapLoadInfo.Teaching_Adjust = ((OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.DataType & bit_0)==bit_0)? 1:0;
	m_OHTMainStatus->StatusTP.AdjustMapLoadInfo.Teaching_Adjust_Modify = ((OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.DataType & bit_1)==bit_1)? 1:0;
	m_OHTMainStatus->StatusTP.AdjustMapLoadInfo.CID_Adjust = ((OHTMainForm->GetCIDDataManager_STD()->m_CIDFileHeader.DataType & bit_0)==bit_0)? 1:0;
	m_OHTMainStatus->StatusTP.AdjustMapLoadInfo.CID_Adjust_Modify = ((OHTMainForm->GetCIDDataManager_STD()->m_CIDFileHeader.DataType & bit_1)==bit_1)? 1:0;

//	m_OHTMainStatus->StatusTP.IsRFPIO6Digit = 0;

	if(m_OHTMainStatus->CurCycle != CYCLE::CycleNotExist)
		m_OHTMainStatus->StatusTP.MapMakeState.IsCycling = 1;
	else
        m_OHTMainStatus->StatusTP.MapMakeState.IsCycling = 0;

	m_OHTMainStatus->StatusTP.IsErrorIgnolState = OHTMainForm->GetManualControl()->GetFoupCoverFlag();   //AOHT-0001 : FOUP COVER
//    m_OHTMainStatus->StatusTP.IsWarningState = 0;    //AOHC-285

	m_OHTMainStatus->StatusTP.IsWarningState.IsblackboxState = m_defualtparam->bUseblackbox;    //AOHC-285
	m_OHTMainStatus->StatusTP.IsWarningState.IsAutoTeachingVision = m_defualtparam->bUseVisionTeaching;
	memcpy(m_OHTMainStatus->StatusTP.DistanceSensorData.nOBS, m_OHTMainStatus->m_cpMonitorData.DistanceSensorData.nOBS, sizeof(m_OHTMainStatus->StatusTP.DistanceSensorData.nOBS));

    // by zzang9un 2013.03.26 : OHT Version 정보를 추가로 전달
    m_OHTMainStatus->StatusTP.OHTVer.Data = OHTMainForm->GetOHTProgramVersion().Data;

	// by zzang9un 2013.10.07 : AMC 버전 정보 전달
	m_OHTMainStatus->StatusTP.AMCVer.Data = m_pHWSet->GetAMCVersion();

	// by puting 2013.10.07 : Map 버전 정보 전달
	m_OHTMainStatus->StatusTP.MapVer.Data = OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.Version;

	// by zzang9un 2013.03.29 : Mark 관련 정보 전달
    switch(m_ExecuteInfo->ExecutePositionInfo.StationType)
    {
    case MARK_TYPE_NODE: // Driving Tag
        m_OHTMainStatus->StatusTP.MarkInfo.Info.DrivingTag = m_ExecuteInfo->ExecutePositionInfo.Marked;
        break;

	case MARK_TYPE_TAG_STATION: // Stop Tag
	case MARK_TYPE_TAG_STATION_BACK: // Stop Tag
        m_OHTMainStatus->StatusTP.MarkInfo.Info.StopTag = m_ExecuteInfo->ExecutePositionInfo.Marked;
        break;

    case MARK_TYPE_QR_STATION_LEFT: // QR Left Tag
	case MARK_TYPE_QR_LEFT_TOP:
	case MARK_TYPE_QR_LEFT_BOTTOM:
	case MARK_TYPE_QR_LEFT_BOTTOM_S:
	case MARK_TYPE_QR_PSTB_LEFT:
        m_OHTMainStatus->StatusTP.MarkInfo.Info.QRLTag_L = m_ExecuteInfo->ExecutePositionInfo.Marked;
        break;

    case MARK_TYPE_QR_STATION_RIGHT: // QR Right Tag
	case MARK_TYPE_QR_RIGHT_TOP:
	case MARK_TYPE_QR_RIGHT_BOTTOM:
	case MARK_TYPE_QR_RIGHT_BOTTOM_S:
	case MARK_TYPE_QR_PSTB_RIGHT:
		m_OHTMainStatus->StatusTP.MarkInfo.Info.QRLTag_R = m_ExecuteInfo->ExecutePositionInfo.Marked;
        break;

    default:
        m_OHTMainStatus->StatusTP.MarkInfo.Data = 0x00;
        break;
	}

	// 2017-01-09, Add by Labeler, 통과허가 상태 정보 전달
	m_OHTMainStatus->StatusTP.IsPassPermit = m_PassPermit->GetPassPermitCount();
	// 2017-01-09, Add by Labeler, pause 상태 정보 전달
	m_OHTMainStatus->StatusTP.IsOCSPause = m_ExecuteInfo->bOcsPause;
	m_OHTMainStatus->StatusTP.IsTPPause = m_ExecuteInfo->bTpPause;
	m_OHTMainStatus->StatusTP.IsCIDPause = m_pHWSet->m_pCID->GetCIDPause();
    m_OHTMainStatus->StatusTP.IsAutoTeachingFail = m_pTransControl->GetAutoTeachingFail();

}

/**
@brief   동적배열 포인터와 동적배열 포인터를 가르키는 char형 포인타 연결 함수
@date    2013.06.03
@author  puting
@note   구조체와 char형 포인터연결(첫번째 주소값)
*/
/*
void StatusCheckThread::InitSTRCUTInfo()
{
   if(m_MonitorDataArray != NULL)
    {
        delete  m_MonitorDataArray;
        m_MonitorDataArray = new MONITOR_DATA_INFO;
    }
    else
        m_MonitorDataArray = new MONITOR_DATA_INFO;

        memset(m_MonitorDataArray, 0x00, sizeof(MONITOR_DATA_INFO));
        m_OHTMainStatus->m_cpMonitorData = (char *)m_MonitorDataArray;

   if(m_NODEArray != NULL)
    {
        delete  m_NODEArray;
        m_NODEArray = new HID_NODE_INFO;
    }
    else
        m_NODEArray = new HID_NODE_INFO;

        memset(m_NODEArray, 0x00, sizeof(HID_NODE_INFO));
        m_OHTMainStatus->m_cpNODE = (char *)m_NODEArray;

}
*/
//---------------------------------------------------------------------------

/**
@brief   동적배열 포인터와 동적배열 포인터를 가르키는 char형 포인타 해제 함수
@date    2013.06.03
@author  puting
@note   구조체와 char형 포인터연결(첫번째 주소값)
*/
/*
void StatusCheckThread::CloseSTRCUTinfo()
{
    if(m_MonitorDataArray != NULL)
    {
        delete  m_MonitorDataArray;
        m_MonitorDataArray = NULL;
    }

    if(m_OHTMainStatus->m_cpMonitorData != NULL)
    {
     // delete m_OHTMainStatus->m_cpMonitorData;
        m_OHTMainStatus->m_cpMonitorData = NULL;
    }

    if(m_NODEArray != NULL)
    {
        delete  m_NODEArray;
        m_NODEArray = NULL;
    }

    if(m_OHTMainStatus->m_cpNODE != NULL)
    {
      //    delete m_OHTMainStatus->m_cpNODE;
        m_OHTMainStatus->m_cpNODE = NULL;
    }
}
*/
//---------------------------------------------------------------------------

/**
@brief   User를 위한 로그 생성 함수
@date    2013.09.13
@author  zzang9un
@note    유저가 요구하는 내용을 계속 추가해서 로그 남기도록 함
@todo    내부 변환 로직을 함수로 따로 빼는 것이 좋을 것 같음
*/
void StatusCheckThread::AddUserLog()
{
	static DWORD dwUserTrigerTime = timeGetTime();     //함수 진입 시간.
	static int n5VCTCount = 0;
	static int n5VCTUpCount = 0;

	AnsiString strLog = "";
	static AnsiString strPreLog = "";

    // Mode
	AnsiString strMode = "";
    switch(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode)
    {
    case MODE_AUTO:
        strMode = "A";
        break;

    case MODE_MANUAL:
        strMode = "M";
        break;

    default:
        strMode = "";
        break;
    }

    // Status
	String strStatus = GetStatusToString(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status);


    // Speed
    double Speed = m_pHWSet->m_pDrivingAxis->GetCurrentVelocity() * 1000.0;

    // Steering
	AnsiString strSteering = "N";
	if((m_pHWSet->m_pSteer->IsLeftPos(FRONT) == true) && (m_pHWSet->m_pSteer->IsLeftPos(REAR) == true))
        strSteering = "L"; // left
    else if((m_pHWSet->m_pSteer->IsRightPos(FRONT) == true) && (m_pHWSet->m_pSteer->IsRightPos(REAR) == true))
        strSteering = "R"; // right
	else
		strSteering = "M"; // moving


	sIoT_Data iotData;

	if(m_pTaskControl == NULL)
		m_pTaskControl = OHTMainForm->GetTaskControl();

	if(m_pTaskControl != NULL)
		m_pTaskControl->GetIotData(&iotData);

	strLog.sprintf("%s%s/%06d/%06d/%d/%.2f/%s/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%ld/%d/%d/%x/%d",
				strMode.c_str(),
				strStatus.c_str(),
				m_ExecuteInfo->ExecutePositionInfo.CurrNode,
				m_ExecuteInfo->ExecutePositionInfo.CurrStation,
				m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset,
				Speed,
				strSteering.c_str(),
				m_OHTMainStatus->m_cpMonitorData.APSignalData.nAPSignalLevel,
				m_OHTMainStatus->m_cpMonitorData.HIDData.Edc,
				m_OHTMainStatus->m_cpMonitorData.HIDData.Ecap,
				m_OHTMainStatus->m_cpMonitorData.HIDData.Eout,
				m_OHTMainStatus->m_cpMonitorData.HIDData.Iout,
				m_OHTMainStatus->m_cpMonitorData.HIDData.Iac,
				m_OHTMainStatus->m_cpMonitorData.HIDData.ErrorCode,
				m_OHTMainStatus->m_cpMonitorData.HIDData.PickupTemperature,
				m_OHTMainStatus->m_cpMonitorData.HIDData.IGBTTemperature,
				m_OHTMainStatus->m_cpMonitorData.HIDData.ECap1Temperature,
				m_ExecuteInfo->nCheckDivInfo,
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnLeft,
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.TurnRight,
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1,
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.LoadUnloadHP,
				nCommfailCount,
				m_OHTMainStatus->m_cpMonitorData.pValue.nCPU,
				m_OHTMainStatus->m_cpMonitorData.pValue.lMem,
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.SlowDownStop,
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionStop,
				m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.ErrorCode,
				m_OHTMainStatus->bTourMode
//				iotData.Info1.nIotHubState,
//				iotData.Info1.fXAxisACC,
//				iotData.Info1.fYAxisACC,
//				iotData.Info1.fZAxisACC,
//				iotData.Info1.fGyroRoll,
//				iotData.Info1.fGyroPitch,
//				iotData.Info1.fGyroYaw,
//				iotData.Info1.fXAxisDegree,
//				iotData.Info1.fYAxisDegree,
//				iotData.Info1.f1AxisTorque,
//				iotData.Info1.f2AxisTorque,
//				iotData.Info1.f3AxisTorque,
//				iotData.Info1.f4AxisTorque,
//				iotData.Info2.nCountShockDefect,
//				iotData.Info2.nCountParticleDefect,
//				iotData.Info2.nDegreeOfSlope,
//				iotData.Info2.bDetectSlope

				//UBGCheck,
				//OBSCheck
				//strOHTDetect.c_str(),
				//strObsatacle.c_str()
				);


	//사전 로그
	int TempVoltageMonitor1 =  m_pHWSet->GetVolatageMonitor(0);
	int TempVoltageMonitor2 =  m_pHWSet->GetVolatageMonitor(1);
	int TempCheck = (TempVoltageMonitor1>>16)&0xFFFF;
	int TempCheck1 = TempVoltageMonitor2&0xFFFF;
	int TempCheck2 = (TempVoltageMonitor2>>16)&0xFFFF;

	//AOHC-196 사전 로그 추가
    if(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode==MODE_AUTO)
    {

        if(TempCheck < 490)
		{
			n5VCTCount++;
			if(n5VCTCount  > 3)
			{
				if(m_MoniVANode != m_ExecuteInfo->ExecutePositionInfo.CurrNode )
				{
					m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_VD);
					ADD_MD_LOG("VA Cur N : %06d, Offset : %06d",m_ExecuteInfo->ExecutePositionInfo.CurrNode,m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset );
				}
				n5VCTCount = 0;
				m_MoniVANode = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
			}
		}else
			n5VCTCount = 0;

		if(TempCheck > 520)
		{
			n5VCTUpCount++;
			if(n5VCTUpCount  > 3)
			{
				if(m_MoniVDAUNode != m_ExecuteInfo->ExecutePositionInfo.CurrNode )
				{
					m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_VD_U);
					ADD_MD_LOG("VD_A_U Cur N : %06d, Offset : %06d",m_ExecuteInfo->ExecutePositionInfo.CurrNode,m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset );
				}
				n5VCTUpCount = 0;
				m_MoniVDAUNode = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
			}
		}else
			n5VCTUpCount = 0;


		if((TempCheck1 < 1164)  || (TempCheck2 < 1164))     //상향
        {
			if(m_MoniVBNode != m_ExecuteInfo->ExecutePositionInfo.CurrNode )
			{
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_VD_B);
				ADD_MD_LOG("VB Cur N : %06d, Offset : %06d",m_ExecuteInfo->ExecutePositionInfo.CurrNode,m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset );
			}
				m_MoniVBNode = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
		}
		else if((TempCheck1 > 1236)  || (TempCheck2 >1236))     //상향
		{
			if(m_MoniVDBUNode != m_ExecuteInfo->ExecutePositionInfo.CurrNode )
			{
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_VD_B_U);
				ADD_MD_LOG("VD_B_U Cur N : %06d, Offset : %06d",m_ExecuteInfo->ExecutePositionInfo.CurrNode,m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset );
			}
			m_MoniVDBUNode = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
		}



        if(m_OHTMainStatus->m_cpMonitorData.HIDData.Edc < 240)
        {
			if(m_MoniVCNode != m_ExecuteInfo->ExecutePositionInfo.CurrNode )
			{
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_VD_C);
				ADD_MD_LOG("VC Cur N : %06d, Offset : %06d",m_ExecuteInfo->ExecutePositionInfo.CurrNode,m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset );
			}
			m_MoniVCNode = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
		}else if(m_OHTMainStatus->m_cpMonitorData.HIDData.Edc > 350)// 0812발생 보호회로 동작 건으로355->350으로 수정
        {
			if(m_MoniVDCUNode != m_ExecuteInfo->ExecutePositionInfo.CurrNode )
			{
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_VD_C_U);
				ADD_MD_LOG("VD_C_U Cur N : %06d, Offset : %06d",m_ExecuteInfo->ExecutePositionInfo.CurrNode,m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset );
			}
			m_MoniVDCUNode =  m_ExecuteInfo->ExecutePositionInfo.CurrNode;
		}

		if(m_OHTMainStatus->m_cpMonitorData.APSignalData.nAPSignalLevel < 4)
		{
			if(m_MoniAPNode != m_ExecuteInfo->ExecutePositionInfo.CurrNode )
			{
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_AP);
				ADD_MD_LOG("AT Drop Node: %d, Offset : %d, AP Lv : %d ",m_ExecuteInfo->ExecutePositionInfo.CurrNode,m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset,m_OHTMainStatus->m_cpMonitorData.APSignalData.nAPSignalLevel );
			}
			m_MoniAPNode = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
		}
    }

	// by zzang9un 2013.10.30 : 디버그를 위해서 중복 처리 안함
	// 같은 로그 내용을 중복으로 기록하는 것을 막기 위한 부분
	if(strLog != strPreLog)
	{
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwUserTrigerTime) > 200)
		{
			LOG_USER("%s", strLog.c_str());
			dwUserTrigerTime = timeGetTime();
		}
		strPreLog = strLog;
	}
}
//---------------------------------------------------------------------------

/**
@brief   TraceLog를 기록해야 하는 상황인지 체크하는 함수
@author  zzang9un
@date    2013.10.22
*/
void StatusCheckThread::CheckTraceLog()
{
    // Mode
    static UINT PreMode = MODE_MANUAL;
    static DWORD dwTimeOfAuto2Manual = timeGetTime();

    UINT CurrMode = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
    UINT CurrStatus = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status;

    if((PreMode == MODE_AUTO) && (CurrMode == MODE_MANUAL))
    {
        dwTimeOfAuto2Manual = timeGetTime();
    }

    // 상태가 Auto 에서 Manual 에러 상태로 변경 될 경우 Trace 작성
    if(((PreMode == MODE_AUTO) || (OHTUTILITY::PreventOverflowGetTimeMs(dwTimeOfAuto2Manual) < 200)) &&  // 이전 상태가 Auto 이거나 Auto에서 Manual 로 떨어진지 200ms 이내인 경우
        (CurrMode == MODE_MANUAL) && (CurrStatus == STS_ERROR) &&
        (m_OHTMainStatus->bTraceStart == false))
    {
		m_OHTMainStatus->bTraceStart = true;

        ADD_LOG_EXTRAJOB("[StatusCheckThread]Trace flag On");

		if((m_OHTMainStatus->StatusCommon.ErrorCode == ERR_PIO_GO_OFF)
			|| (m_OHTMainStatus->StatusCommon.ErrorCode >= ERR_PIO_READY_ABNORMAL_ON_TD1 && m_OHTMainStatus->StatusCommon.ErrorCode <= ERR_PASSBOX_PIO_PB_AVBL_SIGNAL_CHECK_ABNORMAL_OFF))
		{
			m_pHWSet->m_pLeftEQPIO->Save_HybridPIO_Data();
		}
    }

    // 모드 저장
    PreMode = CurrMode;
}
//---------------------------------------------------------------------------

String StatusCheckThread::GetStatusToString(const BYTE status)
{
	String strStatus = L"";

	switch(status)
    {
	case STS_INIT:
        strStatus = "I";    //"Initialized";
        break;
    case STS_GOING:
        strStatus = "G";       //"Going";
		break;
    case STS_ARRIVED:
        strStatus = "A";            //"Arrived";
		break;
    case STS_UNLOADING:
        strStatus = "U";        //"Unloading";
		break;
    case STS_UNLOADED:
		strStatus = "N";            //"Unloaded";
		break;
    case STS_LOADING:
        strStatus = "L";        //"Loading";
		break;
    case STS_LOADED:
        strStatus = "O";        //"Loaded";
		break;
    case STS_AUTOPOSITION:
        strStatus = "P";        //"AutoPosition";
		break;
    case STS_RUNNING:
        strStatus = "R";            //"Running";
		break;
	case STS_COMPLETE:
	case STS_AUTO_COMPLETE:
        strStatus = "C";            //"Complete";
		break;
    case STS_AUTORECOVERY:
        strStatus = "V";            //"AutoRecovery";
		break;
	case STS_AUTORECOVERY_BLANKTRANS:
		strStatus = "B";            //"공출하";
		break;
	case STS_AUTORECOVERY_DOUBLETRANS:
		strStatus = "D";            //"이중반입";
		break;
    case STS_ERROR:
        strStatus = "E";            //"Error";
		break;
	case STS_AUTORECOVERY_OVERRUN:
		strStatus = "Z";        //"OverRun";
		break;
	case STS_TURN_OFF:
		strStatus = "F";        //"Turn Off";
		break;
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
		strStatus = "T";        //"오토티칭";
		break;
	case STS_AUTO_TAUGHT:
		strStatus = "H";
		break;
	case STS_AUTO_TRANS_HOMING:
		strStatus = "M";
		break;
	default:
        strStatus = "K";  //Unknown
        break;
	}

	return strStatus;
}
//진단
void StatusCheckThread::UpdateDiagData()
{
	static int nUpdateInterval = 0;
   // static DWORD dwPreviosDiagUpdateInterval = timeGetTime();
	static DWORD dwDiagStatUpdateTime = timeGetTime();
	static int nLoggingCount = 0;


	nUpdateInterval++;
	if(nUpdateInterval > 20)
	{

		if(m_pHWSet != NULL)
		{
			double dRunningPosition = 0.0;
			int nRevCount = 0;
			int nMoveCount;
			static DWORD dwUpdateInterval = timeGetTime();
			static int nTimeCount = 0;

			//1. 주행부
			dRunningPosition = m_pHWSet->m_pDrivingAxis->GetRunningPosition(true, ON);
			m_DiagManager->UpdateOverhaulData(DIAG_DATA_MOVEDISTANCE_F, dRunningPosition);
			m_DiagManager->UpdateOverhaulData(DIAG_DATA_MOVEDISTANCE_R, dRunningPosition);
			m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_MOVEDISTANCE, dRunningPosition);

			//2. Hoist
			//2-1. Hoist 모터 Revolution
			 dRunningPosition = m_pHWSet->m_pHoistAxis->GetRunningPosition(false, OFF);
			nRevCount = dRunningPosition / 8192;
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_HOISTREV, nRevCount);
			 m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_REVHOIST, nRevCount);


			//2-1. Hoist 구동 거리
			dRunningPosition = m_pHWSet->m_pHoistAxis->GetRunningPosition(true, ON);
			m_DiagManager->UpdateOverhaulData(DIAG_DATA_HOISTDIST, dRunningPosition);

			//2-2. Hoist 구동횟수
			nMoveCount = m_pHWSet->m_pHoistAxis->GetTotalMoveCount(true);
			m_DiagManager->UpdateOverhaulData(DIAG_DATA_HOISTCOUNT,nMoveCount);
			m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_MOVEHOIST,nMoveCount);


			//3. Shift
			//3-1. Shift 모터 Revolution
			 dRunningPosition = m_pHWSet->m_pShiftAxis->GetRunningPosition(false, OFF);
			nRevCount = dRunningPosition / 8192;
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_SHIFTREV, nRevCount);
			 m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_REVSHIFT, nRevCount);

			//3-1. Shift 구동 거리
			 dRunningPosition = m_pHWSet->m_pShiftAxis->GetRunningPosition(true, ON);
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_SHIFTDIST,dRunningPosition);

			//3-2. Shift 구동횟수
			 nMoveCount = m_pHWSet->m_pShiftAxis->GetTotalMoveCount(true);
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_SHIFTCOUNT, nMoveCount);
			 m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_MOVESHIFT, nMoveCount);

			//4. Steer
			nMoveCount = m_pHWSet->m_pSteer->GetMovingCount(true);
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_MOVESTEERING_F, nMoveCount);
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_MOVESTEERING_R, nMoveCount);
			 m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_MOVESTEERING, nMoveCount);

			//5. Hand
			 nMoveCount = m_pHWSet->m_pHand->GetMovingCount(true);
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_HANDCOUNT, nMoveCount);
			 m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_MOVEHAND, nMoveCount);

			//6. Shutter
			nMoveCount = m_pHWSet->m_pShutter->GetMovingCount(true);
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_SHUTTER_COUNT_F, nMoveCount);
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_SHUTTER_COUNT_R, nMoveCount);

			//7. Rotate
			//없음

			//8. Driving BCR 동작 횟수
			 nMoveCount = m_pHWSet->m_pDrivingBCR->GetTriggerCount(true);
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_DRVBCRCOUNT, nMoveCount);
			 m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_COUNTDRVBCR, nMoveCount);

			//9. Trans BCR 동작 횟수
			 nMoveCount = m_pHWSet->m_pTransBCR->GetTriggerCount(true);
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_STABCRCOUNT, nMoveCount);
			 m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_COUNTSTABCR, nMoveCount);

			//10. QR 동작 횟수
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_QR_COUNT_L,
											m_pHWSet->m_pLeftQRR->GetTriggerCount(true));
			 m_DiagManager->UpdateOverhaulData(DIAG_DATA_QR_COUNT_R,
											m_pHWSet->m_pRightQRR->GetTriggerCount(true));

			//한 시간 단위로 동작하는 부분
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwUpdateInterval) >= 60*60*1000)
			{
				DWORD dwWorkingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwUpdateInterval);

				//11. 가동시간 업데이트
				 m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_TIMELOOKDOWN, dwWorkingTime);
				 m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_TIMEUBG, dwWorkingTime);
				 m_DiagManager->UpdateStatDiagData(DIAG_DATA_ID_TIMEPBS, dwWorkingTime);

				// 구동이력
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_TIMEDRIVING_F, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_TIMEDRIVING_R, dwWorkingTime);

				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_TIMESTEERING_F, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_TIMESTEERING_R, dwWorkingTime);

				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_SHIFTTIME, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_HOISTTIME, dwWorkingTime);

				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_HANDTIME, dwWorkingTime);

				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_SHUTTER_TIME_F, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_SHUTTER_TIME_R, dwWorkingTime);

				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_DRVBCRTIME, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_STABCRTIME, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_QR_TIME_L, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_QR_TIME_R, dwWorkingTime);

				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_LOOKDOWNTIME, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_UBGTIME, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_PBSTIME, dwWorkingTime);

				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_SMPSTIME, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_REGULATORTIME, dwWorkingTime);
				 m_DiagManager->UpdateOverhaulData(DIAG_DATA_PICKUPTIME, dwWorkingTime);

				//11. OHT Status정보 업데이트
//				m_DiagManager->CheckOHTStatus();
				m_DiagManager->IncreaseDiagCheckCount();

				//12. 24시간마다 구동되는 부분
				if(m_DiagManager->GetDiagCheckCount() >= DIAG_SEND_INTERVAL)//24시간 일때,
				{
					m_DiagManager->ResetAbnormalDrvDiffCount();
					m_DiagManager->ResetAbnormalHandResetCount();
					//LogThread 시간 초기화
				}

				//Interval 초기화
				dwUpdateInterval = timeGetTime();

			}

			//13. Abnormal Retry 확인
			if((m_pHWSet->m_pDrivingBCR->GetRetryCount() > m_DiagManager->GetDiagParam().nRefAbnormRetryBCR)
			||(m_pHWSet->m_pTransBCR->GetRetryCount() > m_DiagManager->GetDiagParam().nRefAbnormRetryBCR))
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(m_DiagManager->GetDiagEventFlag().dwReferenceAbnormalRetryBCR) > 24*60*60*1000)
				{
					m_DiagManager->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_RETRY_BCR);
					m_DiagManager->GetDiagEventFlag().dwReferenceAbnormalRetryBCR = timeGetTime();

					m_pHWSet->m_pDrivingBCR->ResetRetryCount();
					m_pHWSet->m_pTransBCR->ResetRetryCount();



				}
			}

		}


		nUpdateInterval = 0;
	}

	// 구동 이력 데이터 로깅
	if(OHTUTILITY::PreventOverflowGetTimeMs(dwDiagStatUpdateTime) >= 10*60*1000)
	{


		char logStr[2048];
		memset(logStr,0,sizeof(logStr));

		m_DiagManager->GetOverhaulLogStr(logStr);

		ADD_MCC_LOG(m_ExecuteInfo->ExecuteMode,",%s,STS,COUNT 10M,%s,%06d,%d,%3.3lf,Log Ver,%.3f,%s",
		m_defualtparam->VHLName,m_mccparam.cCarrierType,
		m_ExecuteInfo->ExecutePositionInfo.CurrNode,
		m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset,
		m_pHWSet->m_pDrivingAxis->GetCurrentVelocity(),
		2.3,
		logStr);

		nLoggingCount++;
		if(nLoggingCount == 6)
		{
			mkdir("C:\\User\\OHT\\Diag");
			ofstream fs;
			fs.open("C:\\User\\OHT\\Diag\\DiagStat.txt");

			fs.write(logStr, strlen(logStr));

			fs.close();
			nLoggingCount = 0;
		}

		dwDiagStatUpdateTime = timeGetTime();
	}


	//AOHC-196 사전 로그 추가
	// 기본 60분 타이머를 만족할때 로깅
	// 시간 조절시 define PREVIOUS_LOG_WRITE_INTERVAL_MIN 만 수정하면 됩니다.
	if(m_DiagManager->PrevLogTimeChk())
	{
		if(m_bRSSICheck  == false)
		{
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_RSSI);
			m_bRSSICheck = true;
		}
		else
		{
            m_bRSSICheck = false;
		}
		m_DiagManager->LogginPreviosDiagCount();
		m_DiagManager->ResetPreviosDiagCount();
	}

	//한 시간 단위로 동작하는 부분
	//1시간에 1파일에 1줄 남기는 부분 시스템 시간도 같이 비교
	//Data Manager는 파일을 Open해서 기준치 이상이 넘으면 점검 필요.  크리티컬 섹션

//	if(CurTime.wMinute >1)
//	{
//		if(bIsPreviosDiagUpdate ==false)
//		{
//			if((m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status !=STS_GOING) &&
//			(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status != STS_UNLOADING) &&
//			(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status !=STS_LOADING)&&
//			(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status !=STS_RUNNING))
//			{
//				 m_DiagManager->LogginPreviosDiagCount();
//				 m_DiagManager->ResetPreviosDiagCount();
//
//				 //Interval 초기화
//				 bIsPreviosDiagUpdate = true;
//			}
//		}
//
//	}
//	else
//	{
//		if(bIsPreviosDiagUpdate ==true)
//			bIsPreviosDiagUpdate = false;
//	}
	/*
	if(OHTUTILITY::PreventOverflowGetTimeMs(dwPreviosDiagUpdateInterval) > 60*60*1000)
	{
		if((m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status !=STS_GOING) &&
			(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status != STS_UNLOADING) &&
			(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status !=STS_LOADING)&&
			(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status !=STS_RUNNING))
			{
				 m_DiagManager->LogginPreviosDiagCount();
				 m_DiagManager->ResetPreviosDiagCount();

				 //Interval 초기화
				 dwPreviosDiagUpdateInterval = timeGetTime();
			}

	}
     */
	// 기타 모니터링 정보


}
void StatusCheckThread::UpdateStatusForMonitoring()
{
	// 2018.12.04 SHCHO : V30055 호기 E3011에러 발생. StatusCheckThread 7초간 멈춤 발생
	// OHT 150 버전 패치후 발생. 의심되는 부분 주석처리
	//
	static CStepTimer timerIPCStat;
	static TimeChk timeChk;
	int nError = NO_ERR;
	// 2018.10.11 SHCHO : IPC 상태 정보 로깅
	char szInfo[512]={0,};
	bool bIPCLoadLib = false;
	bool bCPUMem = false;
	if(timerIPCStat.OnDelay(true, 1000))
	{
		timerIPCStat.OnDelay(false);
		 //3. IPC 정보 얻기
		if(m_IPCLoadLib !=NULL)
		{

#if((OHT_NAME == OHT_NAME_STD_V82) ||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
			timeChk.Start(0);
			m_IPCLoadLib->GetEZUNEMonitoringData(&m_OHTMainStatus->m_cpMonitorData.IPCData);
			timeChk.End(0);
			CycleMonitoring(0, timeChk.Get(0));
#else
			timeChk.Start(0);
			m_IPCLoadLib->GetIPCData(&m_OHTMainStatus->m_cpMonitorData.IPCData);
//			Get_IPCSysInfo(&m_OHTMainStatus->m_cpMonitorData.MonitorOperStatus.IPCSysInfo);
            timeChk.End(0);
			CycleMonitoring(0, timeChk.Get(0));
			// IPC 정보 string으로 얻기
			bool bRet = m_IPCLoadLib->GetInfo_String(szInfo, sizeof(szInfo));
			if(bRet)
				bIPCLoadLib = true;
//				RapidLog(IPC_STAT, szInfo);

#endif

 		}
	}

	static int HIDUpdateInterval = 0;
 	static int nUpdateInterval = 0;
	static int nMemoryCheckCount = 0;

	static nCPUCheckCount_FDC = 0;
	static nMemoryCheckCount_FDC = 0;

	static double dPreSpeed =0.0;
    double TempData = 0.0;
	static int nADWResult = 0;  //0 대기,  가속 1, 감속 2, 등속 3
    int nOBSData[OBS_Sensor_Data_Num] = {0,};
	int nUBGData[UBG_Sensor_Data_Num] = {0,};
 //	int nPBSData[NUM_PBS_DATA] = {0,};
	int nPBSData[LookDown_Data_Num] = {0,};

	char FineUBG[UBG_Sensor_Data_Num] = {0,};

	static int nPreDistanceSensorType = DISTANCE_SENSOR_DISABLE;

	static DWORD m_dwTime = timeGetTime();

    static BYTE PreMode = 0x00;
	static int Fine_CheckCount = 0;
	BYTE CurMode    = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
	BYTE CurStatus  = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status;

	nUpdateInterval++;
	HIDUpdateInterval++;

    if(PreMode != CurMode)
    {
        // Manual -> Auto
//        if(CurMode == MODE_AUTO)
//        {
//            if(m_pHWSet != NULL)
//            {
//                 m_pHWSet->m_pOHTDetect->CloseCom();
//                 m_pHWSet->m_pObstacle->CloseCom();
////                 m_pHWSet->m_pLookdown->PBSDetect_ReadStop();
//            }
//        }
	}
    else
    {
		if((CurMode == MODE_MANUAL) && (CurStatus !=STS_GOING) && (CurStatus !=STS_ERROR))
        {
			if((nPreDistanceSensorType != m_DistanceSensorType) && (m_pHWSet != NULL))
            {

				if(m_DistanceSensorType == UBG_OHTDETECT_SENSOR)
				{
				   if(m_defualtparam->OhtDetectSensorType != OHTDETECT_SOSLAB)
				   {
						m_pHWSet->m_pOHTDetect->DataReadStart(nUBGData);
						nPreDistanceSensorType = m_DistanceSensorType;
				   }else
                     m_DistanceSensorType = DISTANCE_SENSOR_DISABLE;
                }
                else if(m_DistanceSensorType == UBG_OBSTACLE_SENSOR)
				{
				   if(m_defualtparam->OhtDetectSensorType != OHTDETECT_SOSLAB)
				   {
						m_pHWSet->m_pObstacle->DataReadStart(nOBSData);
						nPreDistanceSensorType = m_DistanceSensorType;
				   }else
                     m_DistanceSensorType = DISTANCE_SENSOR_DISABLE;
				}
				else if(m_DistanceSensorType == UBG_OHTDETECT_SOSLAB_SENSOR)
				{
					if((nError == NO_ERR) && (m_defualtparam->OhtDetectSensorType == OHTDETECT_SOSLAB))
					{
						ADD_LOG_EXTRAJOB("[SOSLAB OHT Detect Sensor TP Check] START !!");
						m_pHWSet->m_pOHTDetect->SOSLABFrameDataEnableCommand(true);

						if(m_pHWSet->m_pObstacle->GetSOSLABFramEnableFlag() == true)
						{
							m_pHWSet->m_pObstacle->SOSLABFrameDataGetCommand();  //쓰레기 버퍼 제거
							m_pHWSet->m_pObstacle->SOSLABFrameDataEnableCommand(false);
						}
					}else
				        m_DistanceSensorType = DISTANCE_SENSOR_DISABLE;

				}
				else if(m_DistanceSensorType == UBG_OBSTACLE_SOSLAB_SENSOR)
				{
					if((nError == NO_ERR) && (m_defualtparam->OhtDetectSensorType == OHTDETECT_SOSLAB))
					{
						ADD_LOG_EXTRAJOB("[SOSLAB OHT Detect Sensor TP Check] START !!");
						m_pHWSet->m_pObstacle->SOSLABFrameDataEnableCommand(true);

						if(m_pHWSet->m_pOHTDetect->GetSOSLABFramEnableFlag() == true)
						{
							m_pHWSet->m_pOHTDetect->SOSLABFrameDataGetCommand();  //쓰레기 버퍼 제거
							m_pHWSet->m_pOHTDetect->SOSLABFrameDataEnableCommand(false);
						}
					}else
						m_DistanceSensorType = DISTANCE_SENSOR_DISABLE;

				}
				//timy. Lookdown 부분
				else if(m_DistanceSensorType == PBS_LOOKDOWN_SENSOR)
				{
					  //m_pHWSet->m_pLookdown->DataReadStart(nPBSData);
					  m_pHWSet->m_pLookdown->PBSDetect_ReadStart();

					  nPreDistanceSensorType = m_DistanceSensorType;
				}

//				else
//				{
////                    if(nPreDistanceSensorType == UBG_OHTDETECT_SENSOR) m_pHWSet->m_pOHTDetect->CloseCom();
////					else if(nPreDistanceSensorType == UBG_OBSTACLE_SENSOR) m_pHWSet->m_pObstacle->CloseCom();
////                    else if(nPreDistanceSensorType == PBS_LOOKDOWN_SENSOR) m_pHWSet->m_pLookdown->PBSDetect_ReadStop();
//				}
                nPreDistanceSensorType = m_DistanceSensorType;
            }
            else if((m_DistanceSensorType == PBS_LOOKDOWN_SENSOR) && (m_pHWSet->m_pLookdown != NULL))
			{
//                m_pHWSet->m_pLookdown->UpdataData(nPBSData);
				m_pHWSet->m_pLookdown->UpdataDate(nPBSData);
				memcpy(m_OHTMainStatus->m_cpMonitorData.DistanceSensorData.nPBS, nPBSData, sizeof(nPBSData));
			}
            else if((m_DistanceSensorType == UBG_OHTDETECT_SENSOR) && (m_pHWSet->m_pOHTDetect != NULL)&& (m_defualtparam->OhtDetectSensorType != OHTDETECT_SOSLAB))
            {

                    m_pHWSet->m_pOHTDetect->UpdataData(nUBGData);
                    memcpy(m_OHTMainStatus->m_cpMonitorData.DistanceSensorData.nUBG, nUBGData, sizeof(nUBGData));
            }
			else if((m_DistanceSensorType == UBG_OBSTACLE_SENSOR) && (m_pHWSet->m_pObstacle != NULL) && (m_defualtparam->OhtDetectSensorType != OHTDETECT_SOSLAB))
            {
                m_pHWSet->m_pObstacle->UpdataData(nOBSData);
                memcpy(m_OHTMainStatus->m_cpMonitorData.DistanceSensorData.nOBS, nOBSData, sizeof(nOBSData));
			}
			else if((m_DistanceSensorType == UBG_OHTDETECT_SOSLAB_SENSOR) && (m_pHWSet->m_pOHTDetect != NULL) && (m_defualtparam->OhtDetectSensorType == OHTDETECT_SOSLAB))
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwSOSLABFrameTime) > 100)  //100msec
				{
					m_pHWSet->m_pOHTDetect->SOSLABFrameDataGetCommand();
					memmove(m_OHTMainStatus->m_cpMonitorData.DistanceSensorData.nSOSLABUBG, m_pHWSet->m_pOHTDetect->m_SoslabFramData.DTn, sizeof(m_pHWSet->m_pOHTDetect->m_SoslabFramData.DTn));

//					for(int i= 0; i<1000; i++)        //특이사항없음
//					{
//                      	ADD_LOG_EXTRAJOB("[SOSLAB Check i=%d/%x %x ",i,m_OHTMainStatus->m_cpMonitorData.DistanceSensorData.nSOSLABUBG[i], m_OHTMainStatus->m_cpMonitorData.DistanceSensorData.nSOSLABUBG[500+i]);
//					}
					m_dwSOSLABFrameTime = timeGetTime();
				}


			}else if((m_DistanceSensorType == UBG_OBSTACLE_SOSLAB_SENSOR) && (m_pHWSet->m_pObstacle != NULL) && (m_defualtparam->OhtDetectSensorType == OHTDETECT_SOSLAB))
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwSOSLABFrameTime) > 100)  //100msec
				{
					m_pHWSet->m_pObstacle->SOSLABFrameDataGetCommand();
					memmove(m_OHTMainStatus->m_cpMonitorData.DistanceSensorData.nSOSLABOBS, m_pHWSet->m_pObstacle->m_SoslabFramData.DTn, sizeof(m_pHWSet->m_pObstacle->m_SoslabFramData.DTn));
                	m_dwSOSLABFrameTime = timeGetTime();
				}
			}
			else
			{
				if(m_defualtparam->OhtDetectSensorType == OHTDETECT_SOSLAB)
				{
					if(OHTMainForm->GetUBGCheckComplete() ==true)
					{
					   	if(m_pHWSet->m_pOHTDetect->GetSOSLABFramEnableFlag() == true)
						{
							m_pHWSet->m_pOHTDetect->SOSLABFrameDataGetCommand();  //쓰레기 버퍼 제거
							m_pHWSet->m_pOHTDetect->SOSLABFrameDataEnableCommand(false);
							m_DistanceSensorType =  DISTANCE_SENSOR_DISABLE;
						}
					}

					if(m_pHWSet->m_pObstacle->GetSOSLABFramEnableFlag() == true)
					{
						m_pHWSet->m_pObstacle->SOSLABFrameDataGetCommand();  //쓰레기 버퍼 제거
						m_pHWSet->m_pObstacle->SOSLABFrameDataEnableCommand(false);
						m_DistanceSensorType =  DISTANCE_SENSOR_DISABLE;
					}
				}
			}

		}else
		{
			if(m_defualtparam->OhtDetectSensorType == OHTDETECT_SOSLAB)
			{
				if(OHTMainForm->GetUBGCheckComplete() ==true)
				{
					if(m_pHWSet->m_pOHTDetect->GetSOSLABFramEnableFlag() == true)
					{
						m_pHWSet->m_pOHTDetect->SOSLABFrameDataGetCommand();  //쓰레기 버퍼 제거
						m_pHWSet->m_pOHTDetect->SOSLABFrameDataEnableCommand(false);
						m_DistanceSensorType =  DISTANCE_SENSOR_DISABLE;
					}
				}

				if(m_pHWSet->m_pObstacle->GetSOSLABFramEnableFlag() == true)
				{
					m_pHWSet->m_pObstacle->SOSLABFrameDataGetCommand();  //쓰레기 버퍼 제거
					m_pHWSet->m_pObstacle->SOSLABFrameDataEnableCommand(false);
					m_DistanceSensorType =  DISTANCE_SENSOR_DISABLE;
				}
			}

		}
    }

	PreMode = CurMode;


	 if(nUpdateInterval >= 4)
	 {      timeChk.Start(1);
			if((nUpdateInterval % 4) ==0)
			{
				TempData = (fabs(m_pHWSet->m_pDrivingAxis->GetCurrentVelocity() * 1000) - dPreSpeed)/100;

				if(fabs(TempData) < 0.5)
				{
					if(fabs(m_pHWSet->m_pDrivingAxis->GetCurrentVelocity()) < 0.05)
						nADWResult = 0;
					else
						nADWResult = 3;
				}else if(TempData >= 0.5)
				{
					nADWResult = 1;
				}else
				{
				   nADWResult = 2;
				}

				dPreSpeed  =fabs(m_pHWSet->m_pDrivingAxis->GetCurrentVelocity() * 1000);
//				nADWResult = m_pHWSet->m_pDrivingAxis->GetSpeedMode();
			}
			timeChk.End(1);
			CycleMonitoring(1, timeChk.Get(1));
#if((OHT_NAME == OHT_NAME_STD_V82)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))

		timeChk.Start(2);
		if(HIDUpdateInterval > 7)     //그린파워 요청, 주기 300ms
		{
//			LOG_ERR_HID("HID ADData %f (CurrSP:%f, PreSP: %f, Result:%d)",TempData,(m_pHWSet->m_pDrivingAxis->GetCurrentVelocity() * 1000),dPreSpeed ,nADWResult );
			//1. HID 정보 Updata  및 HID 정보 얻기
			if(m_HID != NULL)
			{
				if(m_HID->UpdateData(nADWResult, m_ExecuteInfo->nHIDPathInfo) == NO_ERR)
				{
					m_OHTMainStatus->m_cpMonitorData.HIDData = m_HID->GetHIDData();
				}
			}
			HIDUpdateInterval = 0;
		}
		timeChk.End(2);
		CycleMonitoring(2, timeChk.Get(2));
#endif
		if(nUpdateInterval > 20)
		{

//			LOG_ERR_HID("HID ADData %f (CurrSP:%f, PreSP: %f, Result:%d)",TempData,(m_pHWSet->m_pDrivingAxis->GetCurrentVelocity() * 1000),dPreSpeed ,nADWResult );
			//1. HID 정보 Updata  및 HID 정보 얻기
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
			if(m_HID != NULL)
			{
				timeChk.Start(2);
				if(m_HID->UpdateData(nADWResult, m_ExecuteInfo->nHIDPathInfo) == NO_ERR)
				{
					m_OHTMainStatus->m_cpMonitorData.HIDData = m_HID->GetHIDData();
				}
				timeChk.End(2);
				CycleMonitoring(2, timeChk.Get(2));
			}
#endif
			//2. NetWork Data 얻기
			if(m_NetworkDll!=NULL)
			{
#if(WirelessClient_TYPE == XR5_WIRELESS_CLIENT)
				if(m_NetworkDll->DisconnectFlag == false)
					m_NetworkDll->Get_WLanInfo(&m_OHTMainStatus->m_cpMonitorData.APSignalData);
#else
				static int iDelay = 0;

				// FXE-3000 무선랜 카드 상태값 요청 구현 필요



				if(iDelay == m_defualtparam->nWlanCheckCount)
				{
					if(m_NetworkDll->DisconnectFlag == false)
					{
						timeChk.Start(3);
						Get_WLanInfo_FXE_MODEL((char*)&m_defualtparam->WirelessClientIP, &m_OHTMainStatus->m_cpMonitorData.APSignalData);
						timeChk.End(3);
						CycleMonitoring(3, timeChk.Get(3));
					}
					iDelay = 0;
				}
				else
				{
					iDelay++;
				}
#endif

			}

			// Handy TP - Info add
			memmove(HandyTP_AP_Info.szAPMacAddress, &m_OHTMainStatus->m_cpMonitorData.APSignalData,sizeof(HandyTP_AP_Info.szAPMacAddress));
			HandyTP_AP_Info.nAPSignalLevel = m_OHTMainStatus->m_cpMonitorData.APSignalData.nAPSignalLevel;

			//4. 메모리 및 CPU 점유률 정보 얻기
			if(m_CPUandMemInfo !=NULL)
			{
				CPU_INFO* pValue = new CPU_INFO;

				timeChk.Start(4);
				bool result = m_CPUandMemInfo->Get_CPUandMem(pValue);
				timeChk.End(4);
				CycleMonitoring(4, timeChk.Get(4));

				if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwResetTime) > 24*1000*60*60)  //1시간
				//if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwResetTime) > 5000)  //1시간
				{
//					SIZE_T dwMemorySize = CheckMemoryInfo(); //메모리리셋
					m_OHTMainStatus->fMemoryReset = ResetWorkingSetSize(m_OHTMainStatus->nfMemorySize);

					if(m_OHTMainStatus->fMemoryReset == true)
					{
						CPUNMEM_LOG("MEMSIZE/%lu", m_OHTMainStatus->nfMemorySize);
						m_nResetworkingCount++;
                    }
					else
						m_nResetworkingCount = 0;



					m_dwResetTime =  timeGetTime();
                }

				if(m_nResetworkingCount ==10)
				{
					m_OHTMainStatus->ErrCode_StatusChk = ERR_WINDOWEXCEPTION;
					m_nResetworkingCount = 0;
				}


				if(result == true)
				{
					if((m_OHTMainStatus->m_cpMonitorData.pValue.lMem != pValue->lMem) ||
					   (m_OHTMainStatus->m_cpMonitorData.pValue.nCPU != pValue->nCPU))
					   {
							bCPUMem = true;

							//AOHC-196 사전 로그 추가
							if(pValue->nCPU > 50)
								 m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_CPU);

							if(pValue->lMem > 45)
								m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_MEM);


                            m_OHTMainStatus->m_cpMonitorData.pValue.lMem =  pValue->lMem;
							m_OHTMainStatus->m_cpMonitorData.pValue.nCPU =  pValue->nCPU;
					   }

					//진단항목(메모리 사용량 검사)

					if(pValue->lMem > m_DiagManager->GetDiagParam().nRefAbnormUsingMemoryRate)
					{
						if(nMemoryCheckCount > m_DiagManager->GetDiagParam().nRefAbnormUsingMemoryCount)
						{
							if(OHTUTILITY::PreventOverflowGetTimeMs(m_DiagManager->GetDiagEventFlag().dwReferenceAbnormalMemoryUsingCount) > 24*60*60*1000)
							{
								m_DiagManager->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_USING_MEMORY);
								m_DiagManager->GetDiagEventFlag().dwReferenceAbnormalMemoryUsingCount = timeGetTime();
                            	nMemoryCheckCount = 0;
							}

						}else
							nMemoryCheckCount++;
					}
				}

				if(pValue->nCPU > 50)
				{
					nCPUCheckCount_FDC++;

					if(nCPUCheckCount_FDC > 50)
					{
						SEND_WARN("WARN/41/1/6/%06d/%06d/%d/%d/%06d/%ld",
									m_OHTMainStatus->StatusCommon.CurNodeID,
									m_ExecuteInfo->ExecutePositionInfo.NextNode,
									m_OHTMainStatus->StatusCommon.CurNodeOffset,
									m_OHTMainStatus->StatusCommon.ExistFoup,
									m_OHTMainStatus->StatusCommon.StopStationID,
									pValue->nCPU);
						nCPUCheckCount_FDC = 0;
					}
				}


				if(pValue->lMem > 80)
				{
					nMemoryCheckCount_FDC++;

					if(nMemoryCheckCount_FDC > 50)
					{
						SEND_WARN("WARN/42/1/6/%06d/%06d/%d/%d/%06d/%ld",
									m_OHTMainStatus->StatusCommon.CurNodeID,
									m_ExecuteInfo->ExecutePositionInfo.NextNode,
									m_OHTMainStatus->StatusCommon.CurNodeOffset,
									m_OHTMainStatus->StatusCommon.ExistFoup,
									m_OHTMainStatus->StatusCommon.StopStationID,
									pValue->lMem);
						nMemoryCheckCount_FDC = 0;
					}
				}

				if(pValue->nCPU < 50)
				{
					nCPUCheckCount_FDC = 0;
				}

				if(pValue->lMem < 80)
				{
					nMemoryCheckCount_FDC = 0;
				}
				delete pValue;
			}

			nUpdateInterval = 0;
		}
	}

	if(bIPCLoadLib ==true)
	{
		CPUNMEM_LOG("%ld/%ld/%s",  m_OHTMainStatus->m_cpMonitorData.pValue.nCPU, m_OHTMainStatus->m_cpMonitorData.pValue.lMem,szInfo);
	}
	else if(bCPUMem == true)
	{
		CPUNMEM_LOG("%ld/%ld",  m_OHTMainStatus->m_cpMonitorData.pValue.nCPU, m_OHTMainStatus->m_cpMonitorData.pValue.lMem);
	}
}
//---------------------------------------------------------------------------
bool StatusCheckThread::ResetWorkingSetSize(SIZE_T dwMemorySize)
{
	if((int)dwMemorySize < m_DiagManager->GetDiagParam().nRefMemoryReset)
		return false;
	else
		return true;

}
int StatusCheckThread::Get_WLanInfo_FXE_MODEL(char* chAddr, APSIGNAL_DATA* APSignalData)
{

	int iResult = SNMP_GET_OK;
	int iRdx;

	unsigned char packet[MAX_PACKET] = { 0, };
	unsigned char data[MAX_PACKET] = { 0, };
	unsigned char clientMac[MAX_MAC] = { 0, };
	unsigned char clientSsid[MAC_SSID] = { 0, };
	unsigned char mib[MIB_CNT][MAX_PACKET] = { 0, };

#if(WirelessClient_TYPE == FXE3000_WIRELESS_CLIENT)
	// Model : FXE-3000
	char *pMib[MIB_CNT] = { "1.3.6.1.4.1.672.65.3.3.1.11.7.0",
							"1.3.6.1.4.1.672.65.3.3.1.11.2.0",
							"1.3.6.1.4.1.672.65.3.3.1.11.3.0" };		// RSSI, MAC, SSID
#else
	// Model : FXE-2000
	char *pMib[MIB_CNT] = { "1.3.6.1.4.1.672.63.3.3.1.11.7.0",
							"1.3.6.1.4.1.672.63.3.3.1.11.2.0",
							"1.3.6.1.4.1.672.63.3.3.1.11.3.0" };		// RSSI, MAC, SSID
#endif

	bool bmibOK[MIB_CNT] = { false, false, false };
	bool bRcvSuccess = true;
    int rssi, rssi_level, snmplen, mibType, nError, i, j, k, strLen, curNode, curStation;
    int mibLen[MIB_CNT] = { 0, };

	// 변수 초기화
	rssi = 0;
    rssi_level = 0;
	snmplen = 0;
	i = j = k = 0;
	mibType = 0;
    curNode = curStation = 0;
	nError = SNMP_NO_ERR;

    SNMP SnmpData;

    // 정상적으로 UDP Socket이 Open되어 있는 경우
    if(m_UdpClient->SocketStatus())
    {
        // --------------------------------------------------
        //	실제 정보 읽기 (SNMP)
        // --------------------------------------------------
        for (i = 0; i<MIB_CNT; i++)
        {
            SnmpData.MakeSnmp(packet, pMib[i], &snmplen);
            //strLen = send(sock, (char *)packet, snmplen, 0);
            strLen = m_UdpClient->Send((char *)packet, snmplen);
            if (strLen == -1)
            {
                bRcvSuccess = false;
                nError = SNMP_SEND_ERR;
            }

            // ----- fast receive (대기시간 최소화) ----
            if (nError == SNMP_NO_ERR)
            {
                for(iRdx = 0; iRdx < 10; iRdx++)
                {
                    //strLen = recv(sock, (char *)packet, sizeof(packet), 0);
                    strLen = m_UdpClient->Receive((char *)packet, sizeof(packet));
                    // MIB Parsing
                    if (strLen != SOCKET_ERROR)
                    {
                        for (j = 0; j < MIB_CNT; j++)
                        {
                            SnmpData.MibParsing(mib[j], pMib[j], &mibLen[j]);
                            bmibOK[j] = true;
                        }

                        for (j = 0; j < MIB_CNT; j++)
                        {
                            for (k = 2; k < mibLen[j]; k++)
                            {
                                if (mib[j][k] != packet[30 + k])
                                {
                                    bmibOK[j] = false;
                                }
                            }
                        }

                        if (bmibOK[0] == true)			mibType = MIB_RSSI;
                        else if (bmibOK[1] == true)		mibType = MIB_MAC;
                        else if (bmibOK[2] == true)		mibType = MIB_SSID;
                        else                            mibType = MIB_NONE;

                        if (mibType == MIB_RSSI)
                        {
                            // MIB Data is Null
                            if ((strLen == 47) && (packet[45] == 5) && (packet[46] == 0))
                            {
                                bRcvSuccess = false;
                                nError = SNMP_RSSI_NULL_ERR;
                            }
                            else
                            {
								rssi = SnmpData.IntParse(packet);

								m_OCSAPSignal = rssi; //AOHC-292

								rssi = rssi - 96;

								if(rssi > -40)
									rssi = -40;
								else if(rssi < -90)
									rssi = -90;

								rssi_level = (int)(((double)(rssi - (-90))) / ((-40) - (-90)) * 10 + 0.5);
								///////////////////////////////////////////////////////////////////////////

								APSignalData->nAPSignal = rssi;
								APSignalData->nAPSignalLevel = rssi_level;
								APSignalData->nChannel = 1;		                                            // 추가필요

                            }
                        }
                        else if (mibType == MIB_MAC)
                        {
                            // MIB Data is Null
                            if ((strLen == 47) && (packet[45] == 5) && (packet[46] == 0))
                            {
                                bRcvSuccess = false;
                                nError = SNMP_MAC_NULL_ERR;
                            }
                            else
                            {
                                strLen = SnmpData.CharParse(packet, data);
                                sprintf((char *)clientMac, "%02x:%02x:%02x:%02x:%02x:%02x", data[0], data[1], data[2], data[3], data[4], data[5]);
                                clientMac[MAX_MAC - 1] = '\0';
                                memcpy(&APSignalData->szAPMacAddress[0], &data[0], 1);
                                memcpy(&APSignalData->szAPMacAddress[1], &data[1], 1);
                                memcpy(&APSignalData->szAPMacAddress[2], &data[2], 1);
                                memcpy(&APSignalData->szAPMacAddress[3], &data[3], 1);
                                memcpy(&APSignalData->szAPMacAddress[4], &data[4], 1);
                                memcpy(&APSignalData->szAPMacAddress[5], &data[5], 1);
                            }
                        }
                        else if (mibType == MIB_SSID)
                        {
                            // MIB Data is Null
                            if ((strLen == 47) && (packet[45] == 5) && (packet[46] == 0))
                            {
                                bRcvSuccess = false;
                                nError = SNMP_SSID_NULL_ERR;
                            }
                            else
                            {
								strLen = SnmpData.CharParse(packet, data);
                                for (j = 0; j < strLen; j++)
                                {
									clientSsid[j] = data[j];
                                }
								clientSsid[strLen] = '\0';
								memcpy(&APSignalData->szSSID, &clientSsid, sizeof(APSignalData->szSSID));   //넣어줄 곳은 6바이트
							}
                        }
                        else
                        {
                            bRcvSuccess = false;
                            nError = SNMP_TYPE_ERR;
                        }
                        break;  	// 세번째 값을 읽으면 loop를 빠져 나간다
                    }
                    Sleep(1);
                }
            }
            // SNMP 송수신 에러 발생시 loop를 빠져 나감
            if(nError != SNMP_NO_ERR)
                break;
        }

        // 정상 수신한 경우 로그 기록
        if (bRcvSuccess)
        {
            // Position
            curNode = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
            curStation = m_ExecuteInfo->ExecutePositionInfo.CurrStation;

            // Status
            String strStatus = GetStatusToString(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status);
			m_bRSSICheck = true;
            // Mode
            AnsiString strMode = "";
            switch(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode)
            {
                case MODE_AUTO:
                    strMode = "A";
                    break;

                case MODE_MANUAL:
                    strMode = "M";
                    break;

                default:
                    strMode = "";
                    break;
            }

            WirelessNetwork_LOG("[WlanInfo] Mode[%s], Status[%s], Node[%06d(%06d)], Rssi[%d], Rssi Level[%d], MAC[%s], SSID[%s]", strMode.c_str(), strStatus.c_str(), curNode, curStation, rssi, rssi_level, clientMac, clientSsid);
			m_mccparam.nRSSI = rssi;
			strcpy(m_mccparam.cClientMAC,clientMac);



			if(strcmp(LINETYPE_SEMES_1st, clientSsid) == 0 ||
			   strcmp(LINETYPE_SEMES_2nd, clientSsid) == 0 ||
			   strcmp(LINETYPE_SEMES_3rd, clientSsid) == 0)
			{
				m_mccparam.nUseFDCCount = 0;
			}
			else
			{
				m_mccparam.nUseFDCCount++;
			}
			if(m_mccparam.nUseFDCCount > 10)
			{
             	FDC_Manager_Off_Network();
			}
        }
        else
		{
			iResult = SNMP_GET_ERROR;

		}
    }

	if(iResult != SNMP_GET_OK)	// 비정상일때만 저장
		WirelessNetwork_LOG("[WlanInfo] SNMP Func result : %d [%d]", iResult, nError);

	// --- Close ----
    //if(iResult != SNMP_GET_WSA_ERR)
	//    closesocket(sock);

	return iResult;
}


int StatusCheckThread::SaveHID_Log(void)
{
	return m_HID->SaveErrorDataLog();
}


/**
 @brief   LCD의 Com통신을 Open하는 함수
 @author  kyh
 @date    2017.12.12
 */
int StatusCheckThread::ComOpenForLCD()
{
	int nError = NO_ERR;

	if(m_defualtparam->bUseVisionTeaching == false)
	{
		String strLog = NULL;
		m_pLCD = NULL;

		m_pLCD = OHTMainForm->GetHWSetLCD();
		if (m_pLCD != NULL) {
			nError = m_pLCD->OpenCom();
			if (nError == NO_ERR) {
				bOpenLCD = true;
				ADD_LOG_EXTRAJOB("[ComOpenForLCD]LCD Open");
			}
			else {
				bOpenLCD = false;
				ADD_LOG_EXTRAJOB("[ComOpenForLCD]LCD NotOpen");
			}
		}
		else
			nError = -1;
	}

	return nError;
}

//LCD New Version인지 Check하는 함수
int StatusCheckThread::LCDFirmwareVersionCheck()
{
	if(m_defualtparam->bUseVisionTeaching == false)
	{
		if(m_nLCDRetry > 3)             //3번째 New Version 아니면 Pre Version 사용
			return 0;

		unsigned char szReadBuff[1024];
		unsigned char ackData[3] = {0x20, 0x01, 0x25};
		int Length = 3;
		m_pHWSet->m_pLCD->SendTriger(ackData, Length);
		Sleep(700);     //버전 정보 받기까지 700mmsec 필요

		m_pLCD->GetID(szReadBuff);
		ADD_LOG_EXTRAJOB(
		"[LCD]:%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x",
			szReadBuff[0], szReadBuff[1], szReadBuff[2], szReadBuff[3],
			szReadBuff[4], szReadBuff[5], szReadBuff[6], szReadBuff[7],
			szReadBuff[8], szReadBuff[9], szReadBuff[10],szReadBuff[11],
			szReadBuff[12],szReadBuff[13],szReadBuff[14],szReadBuff[15],
			szReadBuff[16],szReadBuff[17],szReadBuff[18],szReadBuff[19],
			szReadBuff[20],szReadBuff[21],szReadBuff[22],szReadBuff[23],
			szReadBuff[24],szReadBuff[25],szReadBuff[26]);


		if((szReadBuff[3] == 0x07) && (szReadBuff[4] == 0x03) && (szReadBuff[5] == 0x18) && (szReadBuff[6] == 0x20))
		{
			ADD_LOG_EXTRAJOB("TestLCDData - New");
			m_nLCDVersion = LCD_NEW_VERSION;
		}
		else
		{
			ADD_LOG_EXTRAJOB("TestLCDData - Pre");
			m_nLCDVersion = LCD_PRE_VERSION;
		}

		//2023.01.03 sseung 버전관리 시스템 LCD Version 저장
		if(!(szReadBuff[3]=='0' && szReadBuff[4]=='0' && szReadBuff[5]=='0' && szReadBuff[6]=='0'))
		{
			OHTMainForm->Version_Info.LCD_ver.sprintf("%x.%x.%x.%x", szReadBuff[3], szReadBuff[4], szReadBuff[5], szReadBuff[6]);     //sseung 2022-11-03 LCD FW Ver. 입력
			OHTMainForm->Version_Info.LCD_Date.sprintf(OHTMainForm->strVersion_Date.c_str());
		}

		m_nLCDRetry++;

		return m_nLCDVersion;
	}
	else
		return 0;
}

/**
 @brief   LCD의 Data를 Read하는 함수
 @author  Yunho.Kim
 @date    2017.12.24
 @note    LCD ReceiveData
 */
int StatusCheckThread::GetReadLCDData(bool OHTMainOff)
{
	int Error = NO_ERR;

	if(m_defualtparam->bUseVisionTeaching == false)
	{
		unsigned char szReadBuff[1024];

		if (OHTMainOff == true)
		{
			m_lcd_flag.LCDOHTMainOff = 1;
			//ADD_LOG_EXTRAJOB("[KYH] LCDOHTMainOff");
		}

		if(m_pTaskControl == NULL)
		{
			m_pTaskControl = OHTMainForm->GetTaskControl();
		}

		if (m_pTaskControl->m_bSetCheckCommfail == true)
		{
			if(m_lcd_flag.LCDCommfail_WaitTimeCheckFlag == false)
			{
				m_lcd_flag.LCDCommfail_WaitTime = timeGetTime();
				m_lcd_flag.LCDCommfail_WaitTimeCheckFlag = true;
			}

			if (OHTUTILITY::PreventOverflowGetTimeMs(m_lcd_flag.LCDCommfail_WaitTime) >= 10000)
			{
				m_lcd_flag.LCDCommfail = 1;
			}
		}
		else
		{
			m_lcd_flag.LCDCommfail_WaitTimeCheckFlag = false;
			m_lcd_flag.LCDCommfail = 0;
		}

		memset(szReadBuff, 0x00, sizeof(szReadBuff));

		if (bOpenLCD)
		{
			try
			{
				if (m_pLCD != NULL)
				{
					Error = m_pLCD->GetID(szReadBuff);
					if(szReadBuff[0] != 0 )
					{
					ADD_LOG_EXTRAJOB(
						"[LCD]:%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x result : %d",
						szReadBuff[0], szReadBuff[1], szReadBuff[2], szReadBuff[3],
						szReadBuff[4], szReadBuff[5], szReadBuff[6], szReadBuff[7],
						szReadBuff[8], szReadBuff[9], szReadBuff[10],szReadBuff[11],
						szReadBuff[12],szReadBuff[13],szReadBuff[14],szReadBuff[15],
						szReadBuff[16],szReadBuff[17],szReadBuff[18],szReadBuff[19],
						szReadBuff[20],szReadBuff[21],szReadBuff[22],szReadBuff[23],
						szReadBuff[24],szReadBuff[25],szReadBuff[26],Error);
					}
				}
			}
			catch (Exception &e)
			{
				ADD_LOG_EXTRAJOB("[LCD]:LCD Exception[%s]", e.ToString());
			}

			//LCD Pack 전송 준비 확인
			if (m_lcd_flag.LCDStatus == LCD_Satus_Compare)
			{
				m_lcd_flag.LCDError = m_OHTMainStatus->InnerErrCode;
				//OHT 에러 및 모드 비교
				if(m_OHTMainStatus->InnerErrCode != 0)
				{
					m_lcd_flag.LCDData = m_lcd_flag.LCDError;
					//ADD_LOG_EXTRAJOB("[KYH] LCDError");
				}
				else
				{
					m_lcd_flag.LCDData = m_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
				}

				//OHT 상태, 명령 비교
				if( m_lcd_flag.LCDOHTMainOff == 1 )
				{
					m_lcd_flag.LCDAction = LCD_ACTION_OHTOFF;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_OHTOFF");
				}
				else if( m_lcd_flag.LCDCommfail == 1 )
				{
					m_lcd_flag.LCDAction = LCD_ACTION_COMFAIL;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_COMFAIL");
				}
				else if( m_OHTMainStatus->StandardStatusOHT.PauseStatus.OHT_Detect == ON )
				{
					m_lcd_flag.LCDAction = LCD_ACTION_UBGSTOP;
				}
				else if( m_OHTMainStatus->StandardStatusOHT.PauseStatus.OBS_Detect == ON )
				{
					m_lcd_flag.LCDAction = LCD_ACTION_OBSSTOP;
				}
				else if( m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.BzWait == ON )
				{
					m_lcd_flag.LCDAction = LCD_ACTION_BZSTOP;
				}
				else if( m_OHTMainStatus->StatusTP.IsOCSPause == true )
				{
					m_lcd_flag.LCDAction = LCD_ACTION_TPPAUSE;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_TPPAUSE");
				}
				else if( m_OHTMainStatus->StatusTP.IsCIDPause == true && m_pHWSet->m_pCID->IsInterlock_ON() == true )
				{
					m_lcd_flag.LCDAction = LCD_ACTION_CIDINTERLOCK;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_CIDINTERLOCK");
				}
				else if( m_OHTMainStatus->StatusTP.IsCIDPause == true )
				{
					m_lcd_flag.LCDAction = LCD_ACTION_CIDDECEL;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_CIDDECEL");
				}
				else if ( m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningCycle == CYCLE::CraneDown ||
						  m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningCycle == CYCLE::Tour_Deposit)
				{
					m_lcd_flag.LCDAction = LCD_ACTION_DEPOSIT;
				}
				else if ( m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningCycle == CYCLE::CraneUp ||
						  m_OHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningCycle == CYCLE::Tour_Acquire)
				{
					m_lcd_flag.LCDAction = LCD_ACTION_ACQUIRE;
				}
				else
				{
					m_lcd_flag.LCDAction = LCD_ACTION_NOMAL;
				}

				if( m_OHTMainStatus->FinalStationOfPathSearch != 0)
				{
					m_lcd_flag.LCDNode = m_OHTMainStatus->FinalStationOfPathSearch;
				}
				else if (m_OHTMainStatus->FinalNodeOfPathSearch  != 0)
				{
					m_lcd_flag.LCDNode = m_OHTMainStatus->FinalNodeOfPathSearch;
				}
				else if (m_ExecuteInfo->ExecutePositionInfo.CurrStation != 0)
				{
					m_lcd_flag.LCDNode = m_ExecuteInfo->ExecutePositionInfo.CurrStation;
				}
				else
				{
					m_lcd_flag.LCDNode = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
				}

				if(m_lcd_flag.PreLCDData == m_lcd_flag.LCDData)	//앞 내용과 확인하여 동일하면 LCD에 정보를 새로 출력하지 않음
				{

					if(m_lcd_flag.LCDData == MODE_AUTO)
					{
						if (m_lcd_flag.PreLCDAction == m_lcd_flag.LCDAction)
						{
						}
						else
						{
							m_lcd_flag.LCDStatus = LCD_First_Print;
						}
					}
					else
					{
						if (m_lcd_flag.LCDAction == LCD_ACTION_OHTOFF && m_lcd_flag.PreLCDAction != m_lcd_flag.LCDAction) // OHTOFF의 경우 Manual에서 상태변화라 추가
						{
							m_lcd_flag.LCDStatus = LCD_First_Print;
						}
						else
						{
						}
					}

					if((m_lcd_flag.preLCDNode != m_lcd_flag.LCDNode)&&(m_lcd_flag.LCDError == 0))
					{
						m_lcd_flag.LCDStatus = LCD_Second_Print;
					}
					else
					{
					}
				}
				else
				{
					m_lcd_flag.LCDStatus = LCD_First_Print;
				}
			}
#if((OHT_NAME == OHT_NAME_STD_V82)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
			if(m_nLCDVersion == LCD_NEW_VERSION)
				 SendLCDDataToNewVersion();
			else
				SendLCDDataToPreVersion();
#else
                         SendLCDDataToPreVersion();
#endif
		}
	}
	return Error;
}

//LCD New Version Data Send 함수
int StatusCheckThread::SendLCDDataToNewVersion()
{
	int Error = 0;

	if(m_defualtparam->bUseVisionTeaching == false)
	{
  //	ADD_LOG_EXTRAJOB("SendLCDDataToNewVersion");
		if (m_lcd_flag.LCDStatus == LCD_First_Print)  //첫줄
		{
			if (m_lcd_flag.LCDAction == LCD_ACTION_OHTOFF)
			{
				unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY, 0x0C,0x00,0x05,0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x03,LCD_TextOut,0x06,'O','H','T','O','F','F'};
				int Length = 21;
				m_pHWSet->m_pLCD->SendTriger(ackData, Length);
				m_lcd_flag.PreLCDAction = LCD_ACTION_OHTOFF;
				m_lcd_flag.LCDStatus = LCD_Second_Delay;
				//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_OHTOFF_1");
			}
			else if(m_lcd_flag.LCDData == m_lcd_flag.LCDError)
			{
				unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY,0x16, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x03,LCD_TextOut,0x05,'E','R','R','O','R'};
				int Length = 20;
				m_pHWSet->m_pLCD->SendTriger(ackData, Length);
				m_lcd_flag.LCDStatus = LCD_First_Delay;
			}
			else if (m_lcd_flag.LCDData == MODE_MANUAL)
			{
				unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x16, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x05,'M','A','I','N','T'};
				int Length = 20;
				m_pHWSet->m_pLCD->SendTriger(ackData, Length);
				m_lcd_flag.LCDStatus = LCD_First_Delay;
			}
			else
			{
				if (m_lcd_flag.LCDAction == LCD_ACTION_COMFAIL)
				{
					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY,0x0C,0x00,0x05,0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x03,LCD_TextOut,0x06,'C','O','M','F','A','L'};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_COMFAIL;
					m_lcd_flag.LCDStatus = LCD_First_Delay;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_COMFAIL_1");
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_UBGSTOP)
				{
					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY,0x0C,0x00,0x05,0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x03,LCD_TextOut,0x06,'V','H','L','S','T','P'};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_UBGSTOP;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_OBSSTOP)
				{
					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY,0x0C,0x00,0x05,0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x03,LCD_TextOut,0x06,'O','B','S','S','T','P'};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_OBSSTOP;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_BZSTOP)
				{
					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY,0x16, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x05,'B','Z','S','T','P'}; //안짤림
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_BZSTOP;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_TPPAUSE)
				{
					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY,0x08, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x05,'P','A','U','S','E'};  //안짤림  오른쪽 치우침
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_TPPAUSE;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_TPPAUSE_1");
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_CIDINTERLOCK)
				{
					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY,0x0C,0x00,0x05,0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x03,LCD_TextOut,0x06,'C','I','D','S','T','P'};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_CIDINTERLOCK;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_CIDINTERLOCK_1");
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_CIDDECEL)
				{
					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY,0x0C,0x00,0x05,0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x03,LCD_TextOut,0x06,'C','I','D','D','C','L'};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_CIDDECEL;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_CIDDECEL_1");
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_DEPOSIT)
				{
					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x08, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x05,' ','L','O','A','D'}; //안짤림 오른쪽 치우침
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_DEPOSIT;
					m_lcd_flag.LCDStatus = LCD_First_Delay;
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_ACQUIRE)
				{
					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY,0x0C,0x00,0x05,0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x03,LCD_TextOut,0x06,'U','N','L','O','A','D'}; //A 짤림
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_ACQUIRE;
					m_lcd_flag.LCDStatus = LCD_First_Delay;
				}
				else
				{
					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x08, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x05,' ','A','U','T','O'}; //안짤림 오른쪽 치우침
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_NOMAL;
					m_lcd_flag.LCDStatus = LCD_First_Delay;
				}
			}
			m_lcd_flag.dwIntervalOfLCD = timeGetTime();
		}
		else if (m_lcd_flag.LCDStatus == LCD_First_Delay)
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_lcd_flag.dwIntervalOfLCD) > 1000)
			{
				m_lcd_flag.LCDStatus = LCD_Second_Print;
			}
			else
			{
				return Error;
			}
		}
		else if (m_lcd_flag.LCDStatus == LCD_Second_Print) //두번째줄
		{
			if (m_lcd_flag.LCDData == m_lcd_flag.LCDError)
			{
				if (int(m_lcd_flag.LCDData) < 0)
				{
					unsigned char ackData[27] = {LCD_Text, 0x19, LCD_TextXY, 0x00, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x07,LCD_TextOut,0x0C,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
					int Length = 27;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
				}
				else
				{
					unsigned char LCDErrorCode[4] ={};
					LCDErrorCode[0] = (m_lcd_flag.LCDData / 1000) + 0x30;
					LCDErrorCode[1] = ((m_lcd_flag.LCDData / 100) % 10) + 0x30;
					LCDErrorCode[2] = ((m_lcd_flag.LCDData / 10) % 10) + 0x30;
					LCDErrorCode[3] = (m_lcd_flag.LCDData % 10) + 0x30;

					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY, 0x20, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x07,LCD_TextOut,0x06,' ',LCDErrorCode[0],LCDErrorCode[1],LCDErrorCode[2],LCDErrorCode[3],' '};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
				}
			}
			else if(m_lcd_flag.LCDData == MODE_AUTO)
			{
				if(m_lcd_flag.LCDAction == LCD_ACTION_COMFAIL)
				{
					unsigned char LCDStation[6] ={};
					unsigned char LCDCommfail[3] ={}; // 16진수를 아스키 코드로 변환 작업
					LCDCommfail[0] = m_OHTMainStatus->m_cpMonitorData.APSignalData.szAPMacAddress[3];
					LCDCommfail[1] = m_OHTMainStatus->m_cpMonitorData.APSignalData.szAPMacAddress[4];
					LCDCommfail[2] = m_OHTMainStatus->m_cpMonitorData.APSignalData.szAPMacAddress[5];

					if(((LCDCommfail[0] / 0x10) % 0x10) >= 0x0a)
					{
						LCDStation[0] = ((LCDCommfail[0] / 0x10) % 0x10) + 0x57;
					}
					else
					{
						LCDStation[0] = ((LCDCommfail[0] / 0x10) % 0x10) + 0x30;
					}
					if((LCDCommfail[0] % 0x10) >= 0x0a)
					{
						LCDStation[1] = (LCDCommfail[0] % 0x10) + 0x57;
					}
					else
					{
						LCDStation[1] = (LCDCommfail[0] % 0x10) + 0x30;
					}

					if(((LCDCommfail[1] / 0x10) % 0x10) >= 0x0a)
					{
						LCDStation[2] = ((LCDCommfail[1] / 0x10) % 0x10) + 0x57;
					}
					else
					{
						LCDStation[2] = ((LCDCommfail[1] / 0x10) % 0x10) + 0x30;
					}
					if((LCDCommfail[1] % 0x10) >= 0x0a)
					{
						LCDStation[3] = (LCDCommfail[1] % 0x10) + 0x57;
					}
					else
					{
						LCDStation[3] = (LCDCommfail[1] % 0x10) + 0x30;
					}

					if(((LCDCommfail[2] / 0x10) % 0x10) >= 0x0a)
					{
						LCDStation[4] = ((LCDCommfail[2] / 0x10) % 0x10) + 0x57;
					}
					else
					{
						LCDStation[4] = ((LCDCommfail[2] / 0x10) % 0x10) + 0x30;
					}
					if((LCDCommfail[2] % 0x10) >= 0x0a)
					{
						LCDStation[5] = (LCDCommfail[2] % 0x10) + 0x57;
					}
					else
					{
						LCDStation[5] = (LCDCommfail[2] % 0x10) + 0x30;
					}

					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY, 0x00, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode; // 구동하지 않기 때문에 동일 값 사용됨
				}
				else if (m_OHTMainStatus->FinalStationOfPathSearch != 0 )
				{
					unsigned char LCDStation[6] ={};
					LCDStation[0] = (m_lcd_flag.LCDNode / 100000) + 0x30;
					LCDStation[1] = ((m_lcd_flag.LCDNode / 10000) % 10) + 0x30;
					LCDStation[2] = ((m_lcd_flag.LCDNode / 1000) % 10)+ 0x30;
					LCDStation[3] = ((m_lcd_flag.LCDNode / 100)% 10 )+ 0x30;
					LCDStation[4] = ((m_lcd_flag.LCDNode/ 10) % 10) + 0x30;
					LCDStation[5] = (m_lcd_flag.LCDNode % 10) + 0x30;

					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY, 0x00, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode;
				}
				else if (m_OHTMainStatus->FinalNodeOfPathSearch != 0)
				{
					unsigned char LCDStation[6] ={};
					LCDStation[0] = (m_lcd_flag.LCDNode / 100000) + 0x30;
					LCDStation[1] = ((m_lcd_flag.LCDNode / 10000) % 10) + 0x30;
					LCDStation[2] = ((m_lcd_flag.LCDNode / 1000) % 10)+ 0x30;
					LCDStation[3] = ((m_lcd_flag.LCDNode / 100)% 10 )+ 0x30;
					LCDStation[4] = ((m_lcd_flag.LCDNode/ 10) % 10) + 0x30;
					LCDStation[5] = (m_lcd_flag.LCDNode % 10) + 0x30;

					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY, 0x00, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode;
				}
				else
				{
					unsigned char LCDStation[6] ={};
					LCDStation[0] = (m_lcd_flag.LCDNode / 100000) + 0x30;
					LCDStation[1] = ((m_lcd_flag.LCDNode / 10000) % 10) + 0x30;
					LCDStation[2] = ((m_lcd_flag.LCDNode / 1000) % 10)+ 0x30;
					LCDStation[3] = ((m_lcd_flag.LCDNode / 100)% 10 )+ 0x30;
					LCDStation[4] = ((m_lcd_flag.LCDNode/ 10) % 10) + 0x30;
					LCDStation[5] = (m_lcd_flag.LCDNode % 10) + 0x30;

					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY, 0x00, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode;
				}
			}
			else
			{
				if (m_ExecuteInfo->ExecutePositionInfo.CurrStation != 0)
				{
					unsigned char LCDStation[6] ={};
					LCDStation[0] = (m_lcd_flag.LCDNode / 100000) + 0x30;
					LCDStation[1] = ((m_lcd_flag.LCDNode / 10000) % 10) + 0x30;
					LCDStation[2] = ((m_lcd_flag.LCDNode / 1000) % 10)+ 0x30;
					LCDStation[3] = ((m_lcd_flag.LCDNode / 100)% 10 )+ 0x30;
					LCDStation[4] = ((m_lcd_flag.LCDNode/ 10) % 10) + 0x30;
					LCDStation[5] = (m_lcd_flag.LCDNode % 10) + 0x30;

					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY, 0x00, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode;
				}
				else
				{
					unsigned char LCDStation[6] ={};
					LCDStation[0] = (m_lcd_flag.LCDNode / 100000) + 0x30;
					LCDStation[1] = ((m_lcd_flag.LCDNode / 10000) % 10) + 0x30;
					LCDStation[2] = ((m_lcd_flag.LCDNode / 1000) % 10)+ 0x30;
					LCDStation[3] = ((m_lcd_flag.LCDNode / 100)% 10 )+ 0x30;
					LCDStation[4] = ((m_lcd_flag.LCDNode/ 10) % 10) + 0x30;
					LCDStation[5] = (m_lcd_flag.LCDNode % 10) + 0x30;

					unsigned char ackData[21] = {LCD_Text, 0x13, LCD_TextXY, 0x00, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x04,LCD_TextOut,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 21;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode;
				}
			}
			m_lcd_flag.dwIntervalOfLCD = timeGetTime();
			m_lcd_flag.LCDStatus = LCD_Second_Delay;
		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_lcd_flag.dwIntervalOfLCD) > 1000)
			{
				m_lcd_flag.LCDStatus = LCD_Satus_Compare;
				m_lcd_flag.PreLCDData = m_lcd_flag.LCDData;
			}
			else
			{
			}
		}
	}
}
//LCD _Pre Version Data Send 함수
int StatusCheckThread::SendLCDDataToPreVersion()
{
	int Error = 0;
	if(m_defualtparam->bUseVisionTeaching == false)
	{
   //	ADD_LOG_EXTRAJOB("SendLCDDataToPreVersion");
		if (m_lcd_flag.LCDStatus == LCD_First_Print)  //첫줄
		{
			if (m_lcd_flag.LCDAction == LCD_ACTION_OHTOFF)
			{
				unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,'O','H','T','O','F','F'};
				int Length = 20;
				m_pHWSet->m_pLCD->SendTriger(ackData, Length);
				m_lcd_flag.PreLCDAction = LCD_ACTION_OHTOFF;
				m_lcd_flag.LCDStatus = LCD_Second_Delay;
				//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_OHTOFF_1");
			}
			else if(m_lcd_flag.LCDData == m_lcd_flag.LCDError)
			{
				unsigned char ackData[19] = {LCD_Text, 0x11, LCD_TextXY,  0x32, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x05,'E','R','R','O','R'};
				int Length = 19;
				m_pHWSet->m_pLCD->SendTriger(ackData, Length);
				m_lcd_flag.LCDStatus = LCD_First_Delay;
			}
			else if (m_lcd_flag.LCDData == MODE_MANUAL)
			{
				unsigned char ackData[19] = {LCD_Text, 0x11, LCD_TextXY, 0x32, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x05,'M','A','I','N','T'};
				int Length = 19;
				m_pHWSet->m_pLCD->SendTriger(ackData, Length);
				m_lcd_flag.LCDStatus = LCD_First_Delay;
			}
			else
			{
				if (m_lcd_flag.LCDAction == LCD_ACTION_COMFAIL)
				{
					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,'C','O','M','F','A','L'};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_COMFAIL;
					m_lcd_flag.LCDStatus = LCD_First_Delay;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_COMFAIL_1");
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_UBGSTOP)
				{
					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,'V','H','L','S','T','P'};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_UBGSTOP;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_OBSSTOP)
				{
					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,'O','B','S','S','T','P'};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_OBSSTOP;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_BZSTOP)
				{
					unsigned char ackData[19] = {LCD_Text, 0x11, LCD_TextXY, 0x32, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x05,'B','Z','S','T','P'};
					int Length = 19;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_BZSTOP;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_TPPAUSE)
				{
					unsigned char ackData[19] = {LCD_Text, 0x11, LCD_TextXY, 0x32, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x05,'P','A','U','S','E'};
					int Length = 19;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_TPPAUSE;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_TPPAUSE_1");
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_CIDINTERLOCK)
				{
					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,'C','I','D','S','T','P'};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_CIDINTERLOCK;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_CIDINTERLOCK_1");
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_CIDDECEL)
				{
					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,'C','I','D','D','C','L'};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_CIDDECEL;
					m_lcd_flag.LCDStatus = LCD_Second_Delay;
					//ADD_LOG_EXTRAJOB("[KYH] LCD_ACTION_CIDDECEL_1");
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_DEPOSIT)
				{
					unsigned char ackData[19] = {LCD_Text, 0x11, LCD_TextXY, 0x0F, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x05,' ','L','O','A','D'};
					int Length = 19;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_DEPOSIT;
					m_lcd_flag.LCDStatus = LCD_First_Delay;
				}
				else if (m_lcd_flag.LCDAction == LCD_ACTION_ACQUIRE)
				{
					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,'U','N','L','O','A','D'};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_ACQUIRE;
					m_lcd_flag.LCDStatus = LCD_First_Delay;
				}
				else
				{
					unsigned char ackData[19] = {LCD_Text, 0x11, LCD_TextXY, 0x0F, 0x00, 0x05, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x05,' ','A','U','T','O'};
					int Length = 19;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
					m_lcd_flag.PreLCDAction = LCD_ACTION_NOMAL;
					m_lcd_flag.LCDStatus = LCD_First_Delay;
				}
			}
			m_lcd_flag.dwIntervalOfLCD = timeGetTime();
		}
		else if (m_lcd_flag.LCDStatus == LCD_First_Delay)
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_lcd_flag.dwIntervalOfLCD) > 1000)
			{
				m_lcd_flag.LCDStatus = LCD_Second_Print;
			}
			else
			{
				return Error;
			}
		}
		else if (m_lcd_flag.LCDStatus == LCD_Second_Print) //두번째줄
		{
			if (m_lcd_flag.LCDData == m_lcd_flag.LCDError)
			{
				if (int(m_lcd_flag.LCDData) < 0)
				{
					unsigned char ackData[26] = {LCD_Text, 0x18, LCD_TextXY, 0x00, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x07,0x0C,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
					int Length = 26;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
				}
				else
				{
					unsigned char LCDErrorCode[4] ={};
					LCDErrorCode[0] = (m_lcd_flag.LCDData / 1000) + 0x30;
					LCDErrorCode[1] = ((m_lcd_flag.LCDData / 100) % 10) + 0x30;
					LCDErrorCode[2] = ((m_lcd_flag.LCDData / 10) % 10) + 0x30;
					LCDErrorCode[3] = (m_lcd_flag.LCDData % 10) + 0x30;

					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x20, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x07,0x06,' ',LCDErrorCode[0],LCDErrorCode[1],LCDErrorCode[2],LCDErrorCode[3],' '};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);
				}
			}
			else if(m_lcd_flag.LCDData == MODE_AUTO)
			{
				if(m_lcd_flag.LCDAction == LCD_ACTION_COMFAIL)
				{
					unsigned char LCDStation[6] ={};
					unsigned char LCDCommfail[3] ={}; // 16진수를 아스키 코드로 변환 작업
					LCDCommfail[0] = m_OHTMainStatus->m_cpMonitorData.APSignalData.szAPMacAddress[3];
					LCDCommfail[1] = m_OHTMainStatus->m_cpMonitorData.APSignalData.szAPMacAddress[4];
					LCDCommfail[2] = m_OHTMainStatus->m_cpMonitorData.APSignalData.szAPMacAddress[5];

					if(((LCDCommfail[0] / 0x10) % 0x10) >= 0x0a)
					{
						LCDStation[0] = ((LCDCommfail[0] / 0x10) % 0x10) + 0x57;
					}
					else
					{
						LCDStation[0] = ((LCDCommfail[0] / 0x10) % 0x10) + 0x30;
					}
					if((LCDCommfail[0] % 0x10) >= 0x0a)
					{
						LCDStation[1] = (LCDCommfail[0] % 0x10) + 0x57;
					}
					else
					{
						LCDStation[1] = (LCDCommfail[0] % 0x10) + 0x30;
					}

					if(((LCDCommfail[1] / 0x10) % 0x10) >= 0x0a)
					{
						LCDStation[2] = ((LCDCommfail[1] / 0x10) % 0x10) + 0x57;
					}
					else
					{
						LCDStation[2] = ((LCDCommfail[1] / 0x10) % 0x10) + 0x30;
					}
					if((LCDCommfail[1] % 0x10) >= 0x0a)
					{
						LCDStation[3] = (LCDCommfail[1] % 0x10) + 0x57;
					}
					else
					{
						LCDStation[3] = (LCDCommfail[1] % 0x10) + 0x30;
					}

					if(((LCDCommfail[2] / 0x10) % 0x10) >= 0x0a)
					{
						LCDStation[4] = ((LCDCommfail[2] / 0x10) % 0x10) + 0x57;
					}
					else
					{
						LCDStation[4] = ((LCDCommfail[2] / 0x10) % 0x10) + 0x30;
					}
					if((LCDCommfail[2] % 0x10) >= 0x0a)
					{
						LCDStation[5] = (LCDCommfail[2] % 0x10) + 0x57;
					}
					else
					{
						LCDStation[5] = (LCDCommfail[2] % 0x10) + 0x30;
					}

					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode; // 구동하지 않기 때문에 동일 값 사용됨
				}
				else if (m_OHTMainStatus->FinalStationOfPathSearch != 0 )
				{
					unsigned char LCDStation[6] ={};
					LCDStation[0] = (m_lcd_flag.LCDNode / 100000) + 0x30;
					LCDStation[1] = ((m_lcd_flag.LCDNode / 10000) % 10) + 0x30;
					LCDStation[2] = ((m_lcd_flag.LCDNode / 1000) % 10)+ 0x30;
					LCDStation[3] = ((m_lcd_flag.LCDNode / 100)% 10 )+ 0x30;
					LCDStation[4] = ((m_lcd_flag.LCDNode/ 10) % 10) + 0x30;
					LCDStation[5] = (m_lcd_flag.LCDNode % 10) + 0x30;

					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode;
				}
				else if (m_OHTMainStatus->FinalNodeOfPathSearch != 0)
				{
					unsigned char LCDStation[6] ={};
					LCDStation[0] = (m_lcd_flag.LCDNode / 100000) + 0x30;
					LCDStation[1] = ((m_lcd_flag.LCDNode / 10000) % 10) + 0x30;
					LCDStation[2] = ((m_lcd_flag.LCDNode / 1000) % 10)+ 0x30;
					LCDStation[3] = ((m_lcd_flag.LCDNode / 100)% 10 )+ 0x30;
					LCDStation[4] = ((m_lcd_flag.LCDNode/ 10) % 10) + 0x30;
					LCDStation[5] = (m_lcd_flag.LCDNode % 10) + 0x30;

					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode;
				}
				else
				{
					unsigned char LCDStation[6] ={};
					LCDStation[0] = (m_lcd_flag.LCDNode / 100000) + 0x30;
					LCDStation[1] = ((m_lcd_flag.LCDNode / 10000) % 10) + 0x30;
					LCDStation[2] = ((m_lcd_flag.LCDNode / 1000) % 10)+ 0x30;
					LCDStation[3] = ((m_lcd_flag.LCDNode / 100)% 10 )+ 0x30;
					LCDStation[4] = ((m_lcd_flag.LCDNode/ 10) % 10) + 0x30;
					LCDStation[5] = (m_lcd_flag.LCDNode % 10) + 0x30;

					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode;
				}
			}
			else
			{
				if (m_ExecuteInfo->ExecutePositionInfo.CurrStation != 0)
				{
					unsigned char LCDStation[6] ={};
					LCDStation[0] = (m_lcd_flag.LCDNode / 100000) + 0x30;
					LCDStation[1] = ((m_lcd_flag.LCDNode / 10000) % 10) + 0x30;
					LCDStation[2] = ((m_lcd_flag.LCDNode / 1000) % 10)+ 0x30;
					LCDStation[3] = ((m_lcd_flag.LCDNode / 100)% 10 )+ 0x30;
					LCDStation[4] = ((m_lcd_flag.LCDNode/ 10) % 10) + 0x30;
					LCDStation[5] = (m_lcd_flag.LCDNode % 10) + 0x30;

					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode;
				}
				else
				{
					unsigned char LCDStation[6] ={};
					LCDStation[0] = (m_lcd_flag.LCDNode / 100000) + 0x30;
					LCDStation[1] = ((m_lcd_flag.LCDNode / 10000) % 10) + 0x30;
					LCDStation[2] = ((m_lcd_flag.LCDNode / 1000) % 10)+ 0x30;
					LCDStation[3] = ((m_lcd_flag.LCDNode / 100)% 10 )+ 0x30;
					LCDStation[4] = ((m_lcd_flag.LCDNode/ 10) % 10) + 0x30;
					LCDStation[5] = (m_lcd_flag.LCDNode % 10) + 0x30;

					unsigned char ackData[20] = {LCD_Text, 0x12, LCD_TextXY, 0x0C, 0x00, 0x8C, 0x00,LCD_TextColor,0xFF,0xFF,0xFF,LCD_TextFont,0x01,0x06,LCDStation[0],LCDStation[1],LCDStation[2],LCDStation[3],LCDStation[4],LCDStation[5]};
					int Length = 20;
					m_pHWSet->m_pLCD->SendTriger(ackData, Length);

					m_lcd_flag.preLCDNode = m_lcd_flag.LCDNode;
				}
			}
			m_lcd_flag.dwIntervalOfLCD = timeGetTime();
			m_lcd_flag.LCDStatus = LCD_Second_Delay;
		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_lcd_flag.dwIntervalOfLCD) > 1000)
			{
				m_lcd_flag.LCDStatus = LCD_Satus_Compare;
				m_lcd_flag.PreLCDData = m_lcd_flag.LCDData;
			}
			else
			{
			}
		}
	}
}
void StatusCheckThread::SetDistanceSensorType(int cmd)
{
	m_DistanceSensorType = cmd;
}

int StatusCheckThread::GetDistanceSensorType()
{
	return m_DistanceSensorType;
}

void StatusCheckThread::MCCStatusData()
{
	static DWORD dwMCCUpdateTime_1s = timeGetTime();
	static DWORD dwMCCUpdateTime_5s = timeGetTime();
	static DWORD dwMCCUpdateTime_5m = timeGetTime();
	static DWORD dwMCCUpdateTime_10m = timeGetTime();

	static UINT nCurNode = 0;
	static UINT nNextNode = 0;

	int    timeerror_1s = 0;
	int    timeerror_5s = 0;
	int    timeerror_5m = 0;
	int	   timeerror_10m = 0;
	sIoT_Data iotData;

	int JobO_NAK_Count = 0;
	int JobC_NAK_Count = 0;
	int JobD_NAK_Count = 0;
	int OCS_Comm = 0;

	double dvoltage3V = 0.0;
	double dvoltage5V = 0.0;
	double dvoltageP12V = 0.0;
	double dvoltageM12V = 0.0;

	int nvoltage3V = 0;
	int nvoltage5V = 0;
	int nvoltageP12V = 0;
	int nvoltageM12V = 0;

	bool fResult = false;

	char *pszDrive_C  = "C:";
	char *pszDrive_D  = "D:";

	UINT64 uFreeBytesToCaller_C = 0;
	UINT64 uTotalBytes_C = 0;
	UINT64 uFreeBytes_C = 0;

	UINT64 uFreeBytesToCaller_D = 0;
	UINT64 uTotalBytes_D = 0;
	UINT64 uFreeBytes_D = 0;

	if(m_pTaskControl != NULL)
		m_pTaskControl->GetIotData(&iotData);

	switch(m_OHTMainStatus->StatusCommon.CarrierType)
	{
		case CARRIER_NOTDEFINED:
			sprintf(m_mccparam.cCarrierType,"None");
			break;
		case CARRIER_FOUP:
			sprintf(m_mccparam.cCarrierType,"FOUP");
			break;
		case CARRIER_POD:
			sprintf(m_mccparam.cCarrierType,"POD");
			break;
		case CARRIER_EUV:
			sprintf(m_mccparam.cCarrierType,"EUV");
			break;
		case CARRIER_MAC:
			sprintf(m_mccparam.cCarrierType,"MAC");
			break;
		case CARRIER_FOSB:
			sprintf(m_mccparam.cCarrierType,"FOSB");
			break;
	}

	if(m_mccparam.nMem < m_OHTMainStatus->m_cpMonitorData.pValue.lMem)
	{
		m_mccparam.nMem = m_OHTMainStatus->m_cpMonitorData.pValue.lMem;
	}
	if(m_mccparam.nCPU < m_OHTMainStatus->m_cpMonitorData.pValue.nCPU)
	{
		m_mccparam.nCPU = m_OHTMainStatus->m_cpMonitorData.pValue.nCPU;
	}

	if(m_pHWSet->m_pFan->IsFanFail() == true)
	{
		m_mccparam.nFanFail++;
	}

	if((m_pHWSet->m_pHand->IsGoSigOn() == true) && (m_mccparam.bpreHandPIO == false))
	{
		m_mccparam.nHandPIOCount++;
	}
	m_mccparam.bpreHandPIO = m_pHWSet->m_pHand->IsGoSigOn();


	if(fabs(m_mccparam.fAxis1Torque) < fabs(iotData.Info1.f1AxisTorque))
	{
		m_mccparam.fAxis1Torque = iotData.Info1.f1AxisTorque;
		m_fAxis1Torque = iotData.Info1.f1AxisTorque;
	}

	if(fabs(m_mccparam.fAxis2Torque) < fabs(iotData.Info1.f2AxisTorque))
	{
		m_mccparam.fAxis2Torque = iotData.Info1.f2AxisTorque;
		m_fAxis2Torque = iotData.Info1.f2AxisTorque;
	}

	if(fabs(m_mccparam.fAxis3Torque) < fabs(iotData.Info1.f3AxisTorque))
	{
		m_mccparam.fAxis3Torque = iotData.Info1.f3AxisTorque;
	}

	if(fabs(m_mccparam.fAxis4Torque) < fabs(iotData.Info1.f4AxisTorque))
	{
		m_mccparam.fAxis4Torque = iotData.Info1.f4AxisTorque;
	}

	if(fabs(m_mccparam.dDrivingMaxPos) < fabs(m_pHWSet->m_pDrivingAxis->GetCurrentPosition()))
	{
		m_mccparam.dDrivingMaxPos = m_pHWSet->m_pDrivingAxis->GetCurrentPosition();
	}

	double dTmpFollowingError = m_pHWSet->m_pDrivingAxis->GetFollowingError();
	double dTmpFollowingError_cts = m_pHWSet->m_pDrivingAxis->GetFollowingError_cts();
	if(fabs(m_mccparam.dDrivingFollowingError) < fabs(dTmpFollowingError_cts))
	{
		if((fabs(fabs(m_mccparam.dDrivingMaxPos) - fabs(dTmpFollowingError)) < 5)
			&& (fabs(m_pHWSet->m_pDrivingAxis->GetCurrentPosition()) < 5)
			&& fabs(m_mccparam.dDrivingMaxPos) > 10)
		{
			//21.07.30 JHJ Following Error 튐현상 개선
			//직전 최고 주행 엔코더와 현재 팔로잉에러가 동일하고,
			//현재 주행 엔코더가 초기화 되었으며,
			//직전 최고 주행 엔코더가 초기값이 아니면 MCC 팔로잉에러에 해당 값 인가하지 않음
//			ADD_MCC_LOG(m_ExecuteInfo->ExecuteMode,",%s,STS,DRV FOLLOWING ERROR TEST,-,%6.1lf,%6.1lf,%6.1lf,%6.1lf,%6.1lf",
//				m_defualtparam->VHLName,m_mccparam.dDrivingMaxPos,
//				m_pHWSet->m_pDrivingAxis->GetCurrentPosition(),
//				dTmpFollowingError,
//				dTmpFollowingError_cts,
//				m_mccparam.dDrivingFollowingError
//			);
		}
		else
		{
			m_mccparam.dDrivingFollowingError = dTmpFollowingError_cts;
		}
	}

	if(fabs(m_mccparam.dHoistFollowingError) < fabs(m_pHWSet->m_pHoistAxis->GetFollowingError_cts()))
	{
		m_mccparam.dHoistFollowingError = m_pHWSet->m_pHoistAxis->GetFollowingError_cts();
	}

	if(fabs(m_mccparam.dShiftFollowingError) < fabs(m_pHWSet->m_pShiftAxis->GetFollowingError_cts()))
	{
		m_mccparam.dShiftFollowingError = m_pHWSet->m_pShiftAxis->GetFollowingError_cts();
	}
	if(nCurNode != m_ExecuteInfo->ExecutePositionInfo.CurrNode || nNextNode != m_ExecuteInfo->ExecutePositionInfo.NextNode)
	{
		nCurNode = m_ExecuteInfo->ExecutePositionInfo.CurrNode;
		nNextNode = m_ExecuteInfo->ExecutePositionInfo.NextNode;
		if(nCurNode == 0 || nNextNode == 0)
		{
			m_mccparam.nPathSteering = 0;
		}
		else
		{
			m_mccparam.nPathSteering = OHTMainForm->GetPathSearch()->GetSteeringInfo(OHTMainForm->GetMap()
								,m_ExecuteInfo->ExecutePositionInfo.CurrNode
								,m_ExecuteInfo->ExecutePositionInfo.NextNode);
		}
	}

	if(OHTUTILITY::PreventOverflowGetTimeMs(dwMCCUpdateTime_1s) > 1 * 1000)
	{
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
		if(m_OHTMainStatus->m_cpMonitorData.HIDData.Eout == 0)
			m_OHTMainStatus->m_cpMonitorData.HIDData.Iout_sign = '_';
#else
		if(m_OHTMainStatus->m_cpMonitorData.HIDData.Iout_sign == 0x00)
			m_OHTMainStatus->m_cpMonitorData.HIDData.Iout_sign = '0';
#endif
		ADD_MCC_LOG(m_ExecuteInfo->ExecuteMode,",%s,STS,PERIODIC 1s,%s,%06d,%d,%3.3lf,Status,%s,Driving Front Torque,%g,Driving Rear Torque,%g,3V,%d,5V,%d,+12V,%d,-12V,%d,Edc,%d,Ecap,%d,Eout,%d,Iout Sign,%c,Iout,%d,Iac,%d,PickUp Temp,%d,IGB Temp,%d,ECapTemp,%d,Ecap2 Temp,%d,Ecap Round Temp,%d,IPC Temp,%f,IPC Voltage,%f,Driving Following Err,%6.3lf,Hoist Following Err,%6.3lf,Shift Following Err,%6.3lf,RSSI,%d,MAC,%s,OHT Detect Output Status,%d,OBS Output Status,%d,Path Steering,%d,Hoist Torque,%g,Shift Torque,%g",
					m_defualtparam->VHLName,m_mccparam.cCarrierType,
					m_ExecuteInfo->ExecutePositionInfo.CurrNode,
					m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset,
					m_pHWSet->m_pDrivingAxis->GetCurrentVelocity(),
					GetStatusToString(m_OHTMainStatus->StatusCommon.MSCmdID.Info.Status),
					m_mccparam.fAxis1Torque,
					m_mccparam.fAxis2Torque,
					m_OHTMainStatus->StatusTP.VolatageMonitor[0]&0xFFFF,
					(m_OHTMainStatus->StatusTP.VolatageMonitor[0]>>16)&0xFFFF,
					m_OHTMainStatus->StatusTP.VolatageMonitor[1]&0xFFFF,
					(m_OHTMainStatus->StatusTP.VolatageMonitor[1]>>16)&0xFFFF,
					m_OHTMainStatus->m_cpMonitorData.HIDData.Edc,
					m_OHTMainStatus->m_cpMonitorData.HIDData.Ecap,
					m_OHTMainStatus->m_cpMonitorData.HIDData.Eout,
					m_OHTMainStatus->m_cpMonitorData.HIDData.Iout_sign, //추가
					m_OHTMainStatus->m_cpMonitorData.HIDData.Iout,
					m_OHTMainStatus->m_cpMonitorData.HIDData.Iac,
					m_OHTMainStatus->m_cpMonitorData.HIDData.PickupTemperature,
					m_OHTMainStatus->m_cpMonitorData.HIDData.IGBTTemperature,
					m_OHTMainStatus->m_cpMonitorData.HIDData.ECap1Temperature,
					m_OHTMainStatus->m_cpMonitorData.HIDData.ECap2Temperature,
					m_OHTMainStatus->m_cpMonitorData.HIDData.ECapRoundTemperature,
					m_OHTMainStatus->m_cpMonitorData.IPCData.dCPUTemperature,
					m_OHTMainStatus->m_cpMonitorData.IPCData.dVoltage_Input,
					m_mccparam.dDrivingFollowingError,
					m_mccparam.dHoistFollowingError,
					m_mccparam.dShiftFollowingError,
					m_mccparam.nRSSI,
					m_mccparam.cClientMAC,
					m_pHWSet->m_pOHTDetect->GetUbgAreaStatus(),
					m_pHWSet->m_pObstacle->GetObsAreaStatus(),
					m_mccparam.nPathSteering,
					m_mccparam.fAxis3Torque,
					m_mccparam.fAxis4Torque
					);
		timeerror_1s = OHTUTILITY::PreventOverflowGetTimeMs(dwMCCUpdateTime_1s) - 1000;
		dwMCCUpdateTime_1s = timeGetTime() - timeerror_1s;

		m_mccparam.fAxis1Torque = 0.0;
		m_mccparam.fAxis2Torque = 0.0;
		m_mccparam.fAxis3Torque = 0.0;
		m_mccparam.fAxis4Torque = 0.0;

		m_mccparam.dDrivingFollowingError = 0.0;
		m_mccparam.dHoistFollowingError = 0.0;
		m_mccparam.dShiftFollowingError = 0.0;
		m_mccparam.dDrivingMaxPos = 0.0;
	}

	if(OHTUTILITY::PreventOverflowGetTimeMs(dwMCCUpdateTime_5s) > 5 * 1000)
	{
		ADD_MCC_LOG(m_ExecuteInfo->ExecuteMode,",%s,STS,PERIODIC 5s,%s,%06d,%d,%3.3lf,Memory,%ld,CPU,%ld,FanFail,%d,LogThread,%d,TaskControlThread,%d,StatusCheckThread,%d,ExcuteThread,%d,ExtraJobThread,%d,AMCDLLlibThread,%d,AMCcommfail,%d,Hand PIO Commfail Count,%d",
					m_defualtparam->VHLName,m_mccparam.cCarrierType,
					m_ExecuteInfo->ExecutePositionInfo.CurrNode,
					m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset,
					m_pHWSet->m_pDrivingAxis->GetCurrentVelocity(),
					m_mccparam.nMem,
					m_mccparam.nCPU,
					m_mccparam.nFanFail,
					m_mccparam.nLogTtime,
					m_mccparam.nTaskTtime,
					m_mccparam.nStatusTtime,
					m_mccparam.nExcuteTtime,
					m_mccparam.nExtraTtime,
					m_mccparam.nAMCTtime,
					m_mccparam.bAMCcomm,
					m_mccparam.nHandPIOCount);
		timeerror_5s = OHTUTILITY::PreventOverflowGetTimeMs(dwMCCUpdateTime_5s) - 5000;
		dwMCCUpdateTime_5s = timeGetTime() - timeerror_5s;
		m_mccparam.nMem = 0;
		m_mccparam.nCPU = 0;
		m_mccparam.nFanFail = 0;
		m_mccparam.nLogTtime = 0;
		m_mccparam.nTaskTtime = 0;
		m_mccparam.nStatusTtime = 0;
		m_mccparam.nExcuteTtime = 0;
		m_mccparam.nExtraTtime = 0;
		m_mccparam.nAMCTtime = 0;
		m_mccparam.bAMCcomm = false;
		m_mccparam.nHandPIOCount = 0;
	}

	if(OHTUTILITY::PreventOverflowGetTimeMs(dwMCCUpdateTime_5m) > 5 * 60 * 1000)
	{
		if(m_pTaskControl != NULL)
			m_pTaskControl->GetMCCCount(JobO_NAK_Count, JobC_NAK_Count, JobD_NAK_Count, OCS_Comm);

		nvoltage3V = m_OHTMainStatus->StatusTP.VolatageMonitor[0]&0xFFFF;
		nvoltage5V = (m_OHTMainStatus->StatusTP.VolatageMonitor[0]>>16)&0xFFFF;
		nvoltageP12V = m_OHTMainStatus->StatusTP.VolatageMonitor[1]&0xFFFF;
		nvoltageM12V = (m_OHTMainStatus->StatusTP.VolatageMonitor[1]>>16)&0xFFFF;

		dvoltage3V = ((double)nvoltage3V)/100;
		dvoltage5V = ((double)nvoltage5V)/100;
		dvoltageP12V = ((double)nvoltageP12V)/100;
		dvoltageM12V = ((double)nvoltageM12V)/100;

//		fResult = GetDiskFreeSpaceExA (pszDrive_C,(PULARGE_INTEGER)&uFreeBytesToCaller_C,(PULARGE_INTEGER)&uTotalBytes_C,(PULARGE_INTEGER)&uFreeBytes_C);
//		fResult = GetDiskFreeSpaceExA (pszDrive_D,(PULARGE_INTEGER)&uFreeBytesToCaller_D,(PULARGE_INTEGER)&uTotalBytes_D,(PULARGE_INTEGER)&uFreeBytes_D);

		uFreeBytes_C = 0;
		uFreeBytes_D = 0;

		ADD_MCC_LOG(m_ExecuteInfo->ExecuteMode,",%s,STS,PERIODIC 5M,%s,%06d,%d,%3.3lf,JobO Nack Count,%d,JobC Nack Count,%d,JobD Nack Count,%d,OCS Comm Count,%d,C: Free Space,%I64u,D: Free Space,%I64u",
					m_defualtparam->VHLName,m_mccparam.cCarrierType,
					m_ExecuteInfo->ExecutePositionInfo.CurrNode,
					m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset,
					m_pHWSet->m_pDrivingAxis->GetCurrentVelocity(),
					JobO_NAK_Count,
					JobC_NAK_Count,
					JobD_NAK_Count,
					OCS_Comm,
					uFreeBytes_C,
					uFreeBytes_D);
		SEND_WARN("MCC/13/1/12/%06d/%06d/%d/%d/%06d/%f/%f/%f/%f/%d/%d/%d",
					m_OHTMainStatus->StatusCommon.CurNodeID,
					m_ExecuteInfo->ExecutePositionInfo.NextNode,
					m_OHTMainStatus->StatusCommon.CurNodeOffset,
					m_OHTMainStatus->StatusCommon.ExistFoup,
					m_OHTMainStatus->StatusCommon.StopStationID,
					dvoltage3V,
					dvoltage5V,
					dvoltageP12V,
					dvoltageM12V,
					m_OHTMainStatus->m_cpMonitorData.HIDData.Edc,
					m_OHTMainStatus->m_cpMonitorData.HIDData.Ecap,
					m_OHTMainStatus->m_cpMonitorData.HIDData.Eout);
		timeerror_5m = OHTUTILITY::PreventOverflowGetTimeMs(dwMCCUpdateTime_5m) - 300000;
		dwMCCUpdateTime_5m = timeGetTime() - timeerror_5m;
	}

	if(OHTUTILITY::PreventOverflowGetTimeMs(dwMCCUpdateTime_10m) > 10 * 60 * 1000)
	{
		AnsiString strMainVer = "0.0.0.0";
		AnsiString strAMCVer = "0.0.0.0";
		AnsiString strMapVer = "0.0.0.0";
		AnsiString strIP = "127.0.0.1";    //임시 Default 값
		UINT Major, Minor;

		strMainVer.sprintf("%u.%u.%u.%u_%d",m_OHTMainStatus->StatusTP.OHTVer.Info.Major,
											m_OHTMainStatus->StatusTP.OHTVer.Info.Minor,
											m_OHTMainStatus->StatusTP.OHTVer.Info.Release,
											m_OHTMainStatus->StatusTP.OHTVer.Info.Build,
											INTERNAL_VER);

		// by puting 2013.10.07 : Map 버전 정보 전달
		int iMapVer = m_OHTMainStatus->StatusTP.MapVer.Data;
		Major = iMapVer & 0xFFFF0000;
		Major = Major >> 16;
		Minor = iMapVer & 0x0000FFFF;
		strMapVer.sprintf("%d.%d", Major, Minor);

		ADD_MCC_LOG(m_ExecuteInfo->ExecuteMode,",%s,STS,PERIODIC 10M,%s,%06d,%d,%3.3lf,Log Ver,%.3f,VHL Ver,%s,Main Ver,%s,MAP Ver,%s,VHL IP,%s,0P,%.1lf,0U,%.1lf,1P,%.1lf,1U,%.1lf,2P,%.1lf,2U,%.1lf,3P,%.1lf,3U,%.1lf,4P,%.1lf,4U,%.1lf,5P,%.1lf,5U,%.1lf,6P,%.1lf,6U,%.1lf,Rail2Jig,%.1f",
					m_defualtparam->VHLName,m_mccparam.cCarrierType,
					m_ExecuteInfo->ExecutePositionInfo.CurrNode,
					m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset,
					m_pHWSet->m_pDrivingAxis->GetCurrentVelocity(),
					2.3, //Log version 임시로 2로 넣음
					m_mccparam.cOHTType, //VHL Version
					strMainVer, //Main Version
					strMapVer, //Map Version
					//AMC, DLL, Param Version or HID Ver 추가 필요 시 작업  AMC Ver,%s,
					strIP, //OHT IP
					m_defualtparam->HoistDefault.PositionValue[0].dPulse,
					m_defualtparam->HoistDefault.PositionValue[0].dUserUnit,
					m_defualtparam->HoistDefault.PositionValue[1].dPulse,
					m_defualtparam->HoistDefault.PositionValue[1].dUserUnit,
					m_defualtparam->HoistDefault.PositionValue[2].dPulse,
					m_defualtparam->HoistDefault.PositionValue[2].dUserUnit,
					m_defualtparam->HoistDefault.PositionValue[3].dPulse,
					m_defualtparam->HoistDefault.PositionValue[3].dUserUnit,
					m_defualtparam->HoistDefault.PositionValue[4].dPulse,
					m_defualtparam->HoistDefault.PositionValue[4].dUserUnit,
					m_defualtparam->HoistDefault.PositionValue[5].dPulse,
					m_defualtparam->HoistDefault.PositionValue[5].dUserUnit,
					m_defualtparam->HoistDefault.PositionValue[6].dPulse,
					m_defualtparam->HoistDefault.PositionValue[6].dUserUnit,
					m_defualtparam->HoistDefault.dDeviceDifference
		);
		ADD_MCC_LOG(m_ExecuteInfo->ExecuteMode,",%s,STS,UBG 10M,%s,%06d,%d,%3.3lf,N,%d/%d/%d/%d/%d/%d/%d/%d,L,%d/%d/%d/%d/%d/%d/%d/%d,R,%d/%d/%d/%d/%d/%d/%d/%d,S,%d/%d/%d/%d/%d/%d/%d/%d,B,%d/%d/%d/%d/%d/%d/%d/%d,V,%d/%d/%d/%d/%d/%d/%d/%d,P,%d/%d/%d/%d/%d/%d/%d/%d",
					m_defualtparam->VHLName,m_mccparam.cCarrierType,
					m_ExecuteInfo->ExecutePositionInfo.CurrNode,
					m_ExecuteInfo->ExecutePositionInfo.CurrNodeOffset,
					m_pHWSet->m_pDrivingAxis->GetCurrentVelocity(),
					m_mccparam.nUBGDetectTime_N[0],m_mccparam.nUBGDetectTime_N[1],m_mccparam.nUBGDetectTime_N[2],m_mccparam.nUBGDetectTime_N[3],
					m_mccparam.nUBGDetectTime_N[4],m_mccparam.nUBGDetectTime_N[5],m_mccparam.nUBGDetectTime_N[6],m_mccparam.nUBGDetectTime_N[7],
					m_mccparam.nUBGDetectTime_L[0],m_mccparam.nUBGDetectTime_L[1],m_mccparam.nUBGDetectTime_L[2],m_mccparam.nUBGDetectTime_L[3],
					m_mccparam.nUBGDetectTime_L[4],m_mccparam.nUBGDetectTime_L[5],m_mccparam.nUBGDetectTime_L[6],m_mccparam.nUBGDetectTime_L[7],
					m_mccparam.nUBGDetectTime_R[0],m_mccparam.nUBGDetectTime_R[1],m_mccparam.nUBGDetectTime_R[2],m_mccparam.nUBGDetectTime_R[3],
					m_mccparam.nUBGDetectTime_R[4],m_mccparam.nUBGDetectTime_R[5],m_mccparam.nUBGDetectTime_R[6],m_mccparam.nUBGDetectTime_R[7],
					m_mccparam.nUBGDetectTime_S[0],m_mccparam.nUBGDetectTime_S[1],m_mccparam.nUBGDetectTime_S[2],m_mccparam.nUBGDetectTime_S[3],
					m_mccparam.nUBGDetectTime_S[4],m_mccparam.nUBGDetectTime_S[5],m_mccparam.nUBGDetectTime_S[6],m_mccparam.nUBGDetectTime_S[7],
					m_mccparam.nUBGDetectTime_B[0],m_mccparam.nUBGDetectTime_B[1],m_mccparam.nUBGDetectTime_B[2],m_mccparam.nUBGDetectTime_B[3],
					m_mccparam.nUBGDetectTime_B[4],m_mccparam.nUBGDetectTime_B[5],m_mccparam.nUBGDetectTime_B[6],m_mccparam.nUBGDetectTime_B[7],
					m_mccparam.nUBGDetectTime_V[0],m_mccparam.nUBGDetectTime_V[1],m_mccparam.nUBGDetectTime_V[2],m_mccparam.nUBGDetectTime_V[3],
					m_mccparam.nUBGDetectTime_V[4],m_mccparam.nUBGDetectTime_V[5],m_mccparam.nUBGDetectTime_V[6],m_mccparam.nUBGDetectTime_V[7],
					m_mccparam.nUBGDetectTime_P[0],m_mccparam.nUBGDetectTime_P[1],m_mccparam.nUBGDetectTime_P[2],m_mccparam.nUBGDetectTime_P[3],
					m_mccparam.nUBGDetectTime_P[4],m_mccparam.nUBGDetectTime_P[5],m_mccparam.nUBGDetectTime_P[6],m_mccparam.nUBGDetectTime_P[7]
		);

		memset(m_mccparam.nUBGDetectTime_N, 0, sizeof(int)*9);
		memset(m_mccparam.nUBGDetectTime_L, 0, sizeof(int)*9);
		memset(m_mccparam.nUBGDetectTime_R, 0, sizeof(int)*9);
		memset(m_mccparam.nUBGDetectTime_S, 0, sizeof(int)*9);
		memset(m_mccparam.nUBGDetectTime_B, 0, sizeof(int)*9);
		memset(m_mccparam.nUBGDetectTime_V, 0, sizeof(int)*9);
		memset(m_mccparam.nUBGDetectTime_P, 0, sizeof(int)*9);

		timeerror_10m = OHTUTILITY::PreventOverflowGetTimeMs(dwMCCUpdateTime_10m) - 600000;
		dwMCCUpdateTime_10m = timeGetTime() - timeerror_10m;
	}
}

void StatusCheckThread::SaveThreadTime(DWORD max, char* name)
{
	if(strcmp("LogT", name) == 0)
	{
		m_mccparam.nLogTtime = max;
		m_OHTMainStatus->dwLogThreadTime = timeGetTime();
	}
	else if(strcmp("TaskCtrl", name) == 0)
	{
		m_mccparam.nTaskTtime = max;
	}
	else if(strcmp("StChkT", name) == 0)
	{
		m_mccparam.nStatusTtime = max;
	}
	else if(strcmp("ExecT", name) == 0)
	{
		m_mccparam.nExcuteTtime = max;
	}
	else if(strcmp("ExtJobT", name) == 0)
	{
		m_mccparam.nExtraTtime = max;
	}
	else if(strcmp("AmcLibT", name) == 0)
	{
		m_mccparam.nAMCTtime = max;
	}
}

void StatusCheckThread::AMCCommfail()
{
	m_mccparam.bAMCcomm = true;
}

void StatusCheckThread::UBGDetectTimeCheck(int nOHTDetectArea, int nOHTDetectLevel, int nTmpnOHTDetectLevel)
{
	switch(nOHTDetectArea)
	{
		case 14: //N
			m_mccparam.nUBGDetectTime_N[nOHTDetectLevel-1] += nTmpnOHTDetectLevel;
			break;
		case 13: //L
			m_mccparam.nUBGDetectTime_L[nOHTDetectLevel-1] += nTmpnOHTDetectLevel;
			break;
		case 12: //R
			m_mccparam.nUBGDetectTime_R[nOHTDetectLevel-1] += nTmpnOHTDetectLevel;
			break;
		case 11: //S
			m_mccparam.nUBGDetectTime_S[nOHTDetectLevel-1] += nTmpnOHTDetectLevel;
			break;
		case 10: //B
			m_mccparam.nUBGDetectTime_B[nOHTDetectLevel-1] += nTmpnOHTDetectLevel;
			break;
		case 9:  //V
			m_mccparam.nUBGDetectTime_V[nOHTDetectLevel-1] += nTmpnOHTDetectLevel;
			break;
		case 8:  //P
			m_mccparam.nUBGDetectTime_P[nOHTDetectLevel-1] += nTmpnOHTDetectLevel;
			break;
		default:
			ADD_LOG_EXTRAJOB(" 예외 Pattern / OHTDetectArea:%d, OHTDetectLevel:%d", nOHTDetectArea, nOHTDetectLevel);
			break;
	}
}

