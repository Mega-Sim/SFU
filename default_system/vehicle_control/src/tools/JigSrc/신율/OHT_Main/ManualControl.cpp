// ---------------------------------------------------------------------------
/**
@file    ManualControl.cpp
@version OHT 7.0
@brief   ManualControl Class CPP File
*/

#include <windows.h>
#include <mmsystem.h>
#include "OHTMain.h"
#include "ManualControl.h"
#include "LogHandler.h" // Log 작성 목적
#include "OHTMainStatus.h"

//---------------------------------------------------------------------------
#define MAX_BYTE_SIZE_OF_MANUALCONTROL_LOG    1024*10

// 로그 작성 함수
#define ADD_LOG(...)	WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)

#define ERR_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\Log\\Error.log", __VA_ARGS__);\
                        ADD_LOG(__VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)
//---------------------------------------------------------------------------


#if 1   // 임시로 사용 --> 추후 Trans로 이동 필요: 임태웅
#define SHIFT_SPEED_RATIO      1.0
#define SHIFT_MAX_SPEED        0.475*SHIFT_SPEED_RATIO ///< Shift 이적재 최고 속도(mm/msec) : 0.475

#if((OHT_NAME == OHT_NAME_STD_V81S) ||(OHT_NAME == OHT_NAME_STD_V85) ||(OHT_NAME == OHT_NAME_STD_V86)|| (OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	#define HOIST_SPEED_RATIO      0.8573       // OHT V7.0 감속비 12.86:1
#endif
#ifdef USE_TRANS_PROFILE_VER2
#if((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V86))
#define HOIST_MAX_SPEED	2.75
#else
#define HOIST_MAX_SPEED        2.14*HOIST_SPEED_RATIO ///< Hoist 이적재 최고 속도(mm/msec) : 02.14
#endif
#else
#define HOIST_MAX_SPEED        2.14*HOIST_SPEED_RATIO ///< Hoist 이적재 최고 속도(mm/msec) : 02.14
#endif
#endif

#if(OHT_NAME == OHT_NAME_STD_V81S)
#define MANUAL_DRIVING_ESTOP_SPEED  0.0032  // ms/msec2
#else
#define MANUAL_DRIVING_ESTOP_SPEED  0.0035  // ms/msec2
#endif
#define MANUAL_DRIVING_TURNOFF_SPEED  0.0030  // ms/msec2
#define MANUAL_DRIVING_MOVE_SPEED_LIMIT 0.3

// by zzang9un 2013.07.25 : Hoist, Shift 최대감속도
#ifdef USE_TRANS_PROFILE_VER2
#if((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V86))
#define MANUAL_HOIST_ESTOP_SPEED       (HOIST_MAX_SPEED /400.0)    // msec^2
#else
#define MANUAL_HOIST_ESTOP_SPEED       (HOIST_MAX_SPEED /280.0)    // msec^2
#endif
#else
#define MANUAL_HOIST_ESTOP_SPEED       (HOIST_MAX_SPEED /280.0)    // msec^2
#endif

#define MANUAL_SHIFT_ESTOP_SPEED       (SHIFT_MAX_SPEED /280.0)    // msec^2

//1 임시


#define MANUAL_SHIFT_SPEED_RATIO      1.0
#define MANUAL_SHIFT_MAX_SPEED        0.475*MANUAL_SHIFT_SPEED_RATIO ///< Shift 이적재 최고 속도(mm/msec) : 0.475

#define MANUAL_MULTIPLY_34_10P        0.1         // UNIT:10%
#define MANUAL_MULTIPLY_34_20P        0.2         // UNIT:10%

#define MANUAL_SHIFT_ACCTIME          550   //msec


#define MANUAL_HOIST_MAX_SPEED        2.67*0.5 //2.67 ///< Hoist 이적재 최고 속도(mm/msec)


#define MANUAL_MULTIPLY_2_15P         0.15        // UNIT:15%
#define MANUAL_MULTIPLY_2_6P          0.06        // UNIT:6%
#define MANUAL_MULTIPLY_2_5P          0.05        // UNIT:2%
#define MANUAL_MULTIPLY_2_3P          0.03        // UNIT:2%
#define MANUAL_MULTIPLY_2_2P          0.02        // UNIT:2%

#define MANUAL_HOIST_ACCTIME_50       50 //0.05     // UNIT:50MSEC
#define MANUAL_HOIST_ACCTIME_150      150 //0.15     // UNIT:150MSEC
#define MANUAL_HOIST_ACCTIME_300      300 //0.3     // UNIT:300MSEC
#define MANUAL_HOIST_ACCTIME_500      500 //0.5     // UNIT:500MSEC
//#define MANUAL_HOIST_ACCTIME_600      600 //0.6     // UNIT:600MSEC

// by zzang9un 2013.07.05 : TransControl.cpp에 있는 변수지만 include가 나눠져 있어서 중복 선언함
#define DIST_HOIST_HOME_MARGIN      1.0     // mm



/**
@brief   ManualControl Class 생성자
@author  LSJ
@date    2012.11.14
@param   pHWSet : HWSet Class의 포인터
@param   pHWCommon : HWCommon Class의 포인터
@param   pDrivingcontrol : DrivingControll Class의 포인터
@param   pStationDataManager : StationDataManager Class의 포인터
*/
ManualControl::ManualControl()
{
    m_pVirtualDrivingControl    = OHTMainForm->GetDrivingControl();
    m_pTransControl             = OHTMainForm->GetTransControl();
    m_pHWCommon         = OHTMainForm->GetHWCommon();
    m_pHWSet            = OHTMainForm->GetHWSet();
	m_pExecuteInfo		= OHTMainForm->GetExecuteInfo();
	m_defualtparam = OHTMainForm->GetDefaultParam();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
	
    pLogUnit = getLogUnit("D:\\Log\\ManualControl.log", MAX_BYTE_SIZE_OF_MANUALCONTROL_LOG);
	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

    memset(&m_ManualCmd, 0x00, sizeof(m_ManualCmd));
    
    m_nIOControlOutIndex        = 0;
    m_dJogDistance_mm           = 0;
    m_dAcceleration_mmpermsec2  = 0.0;
    m_dDeceleration_mmpermsec2  = 0.0;
    m_dVel_mmpermsec            = 0.0;

    m_UseIndex = 0;
	m_bUseOnOff = false;
	m_bNew = false;
}

/**
@brief   ManualControl Class 소멸자
@author  LSJ
@date    2012.11.14
*/
ManualControl::~ManualControl()
{

}

/**
@brief   내부 변수를 초기화
@author  LSJ
@date    2012.11.14
*/
void ManualControl::Init()
{
	m_StatusSet.byStatus=STS_INIT;
	m_StatusSet.uError=0;
    m_StatusSet.bIsOnProcess=false;
    m_QuickCmd=EXECUTE_QUICK_CMD_NONE;
    m_StatusSet.uStep=0;
}

/**
@brief   긴급 정지
@author  ehoto
@date    2012.11.27
@return  EXECUTE_ACK_NO_ERR     // 명령 실행 완료 응답
*/
EXECUTE_ACK ManualControl::Stop(void)
{
	m_StatusSet.byStatus = STS_ERROR;
	m_StatusSet.uStep = DRIVING_CTRL_STEP_NONE;
	m_StatusSet.bIsOnProcess = false;

	if(m_pExecuteInfo->QuickCmdEStopCommander == ESTOP_BY_TP)
	{
		m_StatusSet.uError = ERR_TP_ESTOP;  // - E_Stop 호출은 OCS 도 가능하므로 내용 추가 필요
	}
	else
	{
		if(m_pExecuteInfo->QuickCmdEStopType == EXIT_ROUTE_OCS_PATH)		    	m_StatusSet.uError = ERR_OCS_ESTOP1;
		else if(m_pExecuteInfo->QuickCmdEStopType == OHT_EXIST_IN_COLLISON_AREA)	m_StatusSet.uError = ERR_OCS_ESTOP2;
		else if(m_pExecuteInfo->QuickCmdEStopType == CARRIER_EXIST_4_UNLOADING)	    m_StatusSet.uError = ERR_OCS_ESTOP3;
		else if(m_pExecuteInfo->QuickCmdEStopType == NOT_CARRIER_4_LOADING)		    m_StatusSet.uError = ERR_OCS_ESTOP4;
		else if(m_pExecuteInfo->QuickCmdEStopType == TRUNOFF_STOP)					m_StatusSet.uError = NO_ERR;
		else if(m_pExecuteInfo->QuickCmdEStopType == TRUNON)						m_StatusSet.uError = NO_ERR;
		else																		m_StatusSet.uError = ERR_OCS_ESTOP5;

		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					m_StatusSet.uError,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}
	
	if( m_pHWSet->m_pDrivingAxis->GetCurveEstop() == false) m_pHWSet->m_pDrivingAxis->MoveStop((double)MANUAL_DRIVING_ESTOP_SPEED);
	else m_pHWSet->m_pDrivingAxis->SetCurveEstop(false);
    m_pHWSet->m_pHoistAxis->MoveStop(MANUAL_HOIST_ESTOP_SPEED);
    m_pHWSet->m_pShiftAxis->MoveStop((unsigned long)MANUAL_SHIFT_ESTOP_SPEED);
// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V86))
    m_pHWSet->m_pRotate->Stop();
#endif

    return EXECUTE_ACK_NO_ERR;
}

/**
@brief   기동 Off
@author  ehoto
@date    2012.11.27
@return  EXECUTE_ACK_NO_ERR     // 명령 실행 완료 응답
*/
EXECUTE_ACK ManualControl::TurnON(void)
{
	m_StatusSet.byStatus=STS_INIT;
    m_StatusSet.uError=0;
	m_StatusSet.bIsOnProcess=false;
	m_StatusSet.uStep=0;
}

/**
@brief   기동 Off
@author  ehoto
@date    2012.11.27
@return  EXECUTE_ACK_NO_ERR     // 명령 실행 완료 응답
*/
EXECUTE_ACK ManualControl::TurnOff(void)
{
    m_StatusSet.byStatus = STS_TURN_OFF;

	if(m_pExecuteInfo->QuickCmdEStopCommander == ESTOP_BY_TP)
	{
		m_StatusSet.uError = ERR_TP_ESTOP;  // - E_Stop 호출은 OCS 도 가능하므로 내용 추가 필요
	}
	else
	{
		if(m_pExecuteInfo->QuickCmdEStopType == EXIT_ROUTE_OCS_PATH)		    	m_StatusSet.uError = ERR_OCS_ESTOP1;
		else if(m_pExecuteInfo->QuickCmdEStopType == OHT_EXIST_IN_COLLISON_AREA)	m_StatusSet.uError = ERR_OCS_ESTOP2;
		else if(m_pExecuteInfo->QuickCmdEStopType == CARRIER_EXIST_4_UNLOADING)	    m_StatusSet.uError = ERR_OCS_ESTOP3;
		else if(m_pExecuteInfo->QuickCmdEStopType == NOT_CARRIER_4_LOADING)		    m_StatusSet.uError = ERR_OCS_ESTOP4;
		else if(m_pExecuteInfo->QuickCmdEStopType == TRUNOFF_STOP)					m_StatusSet.uError = NO_ERR;
		else if(m_pExecuteInfo->QuickCmdEStopType == TRUNON)						m_StatusSet.uError = NO_ERR;
		else																		m_StatusSet.uError = ERR_OCS_ESTOP5;

		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					m_StatusSet.uError,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}


	m_pHWSet->m_pDrivingAxis->MoveStop((double)MANUAL_DRIVING_TURNOFF_SPEED);
	m_pHWSet->m_pHoistAxis->MoveStop(MANUAL_HOIST_ESTOP_SPEED);
    m_pHWSet->m_pShiftAxis->MoveStop((unsigned long)MANUAL_SHIFT_ESTOP_SPEED);
// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V86))
	m_pHWSet->m_pRotate->Stop();
#endif
    return EXECUTE_ACK_NO_ERR;
}


/**
@brief   초기화
@author  ehoto
@date    2012.11.27
@return  EXECUTE_ACK_NO_ERR     // 명령 실행 완료 응답
*/
EXECUTE_ACK ManualControl::Reset(void)
{
    // Bumper Reset
	if(m_pHWSet->m_pBumper->IsBumperPress())
    {
        m_pHWSet->m_pBumper->ResetBumperPress();
	}

	// OBS Reset
	if(m_pHWSet->m_pObstacle->CheckError())
	{
		m_pHWSet->m_pObstacle->ErrorReset();
	}

#if(UBG_SENSOR_ENABLE==ON)
	// OHT Detect Reset
	if(m_pHWSet->m_pOHTDetect->CheckError())
	{
		m_pHWSet->m_pOHTDetect->ErrorReset();
	}
#endif

#if (((OHT_NAME == OHT_NAME_STD_V81S) ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE)) && (SAMSUNG_COMMON_SPEC==ON))
	if(m_StatusSet.byStatus == STS_ERROR)
	{
		m_pHWSet->m_pPowerControl->ResetController();
	}
#endif

	Init();

    return EXECUTE_ACK_NO_ERR;
}
//---------------------------------------------------------------------------

/**
@brief   TP에서 보낸 명령 중 IO 출력을 직접 Control하는 함수
@author  LSJ
@date    2012.11.14
@return  명령 수행에 문제 없으면 0, 문제 발생 시 해당 에러 코드 리턴
*/
int ManualControl::setOutput()
{
    int nError = 0;
    int nOutBit = 0;

    if ((m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_SET_OUTPUT) || (m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_RESET_OUTPUT))
    {
        nOutBit = m_nIOControlOutIndex;
        if (nOutBit != -1)
        {
            if (m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_SET_OUTPUT)
                nError = m_pHWCommon->SetBit(nOutBit, ON);
            else
                nError = m_pHWCommon->SetBit(nOutBit, OFF);
        }
        m_StatusSet.bIsOnProcess = false;

        if(nError==NO_ERR)
        {
            m_StatusSet.byStatus = STS_COMPLETE;
            m_StatusSet.uError==NO_ERR;
        }
        else
        {
            m_StatusSet.byStatus = STS_ERROR;
            m_StatusSet.uError==nError;
        }
        
    }
    return nError;
}

/**
@brief   TP에서 보낸 명령 중 축의 Jog 명령을 수행하는 함수
@author  LSJ
@date    2012.11.14
@return  명령 수행에 문제 없으면 0, 문제 발생 시 해당 에러 코드 리턴
*/
int ManualControl::JogCommand()
{
    int nError = 0;
	double dVel_mmpermsec = 0.0, dPos_mm = 0.0, dAcceleration_mmpermsec2 = 0.0, dDeceleration_ctspersec2 = 0.0, dAcceleration_sec = 0.0;

    switch (m_ManualCmd.uCmdTag.Info.CmdData)
    {
         // *** Hoist (OHT)***
    case TPCMD_JOG_HOIST_UP:
		dPos_mm = m_pHWSet->m_pHoistAxis->GetCurrentPosition() - 20.0;
		dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_6P;// * MANUAL_MULTIPLY_2_2P;
		nError = m_pHWSet->m_pHoistAxis->MoveDistance((DWORD)MANUAL_HOIST_ACCTIME_500, (DWORD)MANUAL_HOIST_ACCTIME_500, dVel_mmpermsec, dPos_mm);
		/*
		dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_6P;//MANUAL_MULTIPLY_2_2P;
		nError = m_pHWSet->m_pHoistAxis->MoveNegative((DWORD)MANUAL_HOIST_ACCTIME_500, dVel_mmpermsec);
		*/
		m_pHWSet->m_pHoistAxis->SetHoistOriginFromSmalladd(false);
		break;
	case TPCMD_JOG_HOIST_DOWN:
		dPos_mm = m_pHWSet->m_pHoistAxis->GetCurrentPosition() + (double)20.0;
		dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_6P;// * MANUAL_MULTIPLY_2_2P;
		nError = m_pHWSet->m_pHoistAxis->MoveDistance((DWORD)MANUAL_HOIST_ACCTIME_500, (DWORD)MANUAL_HOIST_ACCTIME_500, dVel_mmpermsec, dPos_mm);
		/*
		dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_6P;//MANUAL_MULTIPLY_2_2P;
        nError = m_pHWSet->m_pHoistAxis->MovePositive((DWORD)MANUAL_HOIST_ACCTIME_500, dVel_mmpermsec);
		*/
		m_pHWSet->m_pHoistAxis->SetHoistOriginFromSmalladd(false);
        break;
	case TPCMD_JOG_HOIST_HOME:

		if((m_pHWSet->m_pHand->IsOpen() == true)&&((m_pHWSet->m_pHand->IsClose() == true)||(m_pHWSet->m_pHand->IsLeftGripOn() == true) || (m_pHWSet->m_pHand->IsRightGripOn() == true)))
        {
            if(m_pHWSet->m_pHand->IsClose() == true)
            {
                nError = ERR_HAND_LIMIT_SEN_ALL_ON;
            }
            else
                nError = ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;

        }
        if((m_pHWSet->m_pHand->IsOpen() == false)&&(m_pHWSet->m_pHand->IsClose() == false))
            nError = ERR_HAND_LIMIT_SEN_ALL_OFF;
        if(m_pHWSet->m_pHand->IsLeftGripOn() != m_pHWSet->m_pHand->IsRightGripOn())
            nError = ERR_GRIP_SEN_MISMATCH;
        if (nError == NO_ERR)
        {

			dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_6P * 3;
            nError = m_pHWSet->m_pHoistAxis->MoveDistance((DWORD)MANUAL_HOIST_ACCTIME_500, MANUAL_HOIST_ACCTIME_500, dVel_mmpermsec, 0.0);
        }
        break;
    case TPCMD_JOG_HOIST_STOP:
        nError = m_pHWSet->m_pHoistAxis->MoveStop((DWORD)MANUAL_HOIST_ACCTIME_500);
        break;

    case TPCMD_JOG_HOIST_ABS:
        dPos_mm = (double)m_dJogDistance_mm;
        // by zzang9un 2013.07.17 : 절대값으로 이동 시 속도는 Main에서 정함
        //dVel_mmpermsec = m_dVel_mmpermsec;        
		dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_15P;// * MANUAL_MULTIPLY_2_2P;
		nError = m_pHWSet->m_pHoistAxis->MoveDistance((DWORD)MANUAL_HOIST_ACCTIME_500, (DWORD)MANUAL_HOIST_ACCTIME_500, dVel_mmpermsec, dPos_mm);
        m_pHWSet->m_pHoistAxis->SetHoistOriginFromSmalladd(false);
		break;
        
	case TPCMD_JOG_HOIST_UP_INC:
        dPos_mm = m_pHWSet->m_pHoistAxis->GetCurrentPosition() - (double)m_dJogDistance_mm;
		dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_6P;// * MANUAL_MULTIPLY_2_2P;
		nError = m_pHWSet->m_pHoistAxis->MoveDistance((DWORD)MANUAL_HOIST_ACCTIME_500, (DWORD)MANUAL_HOIST_ACCTIME_500, dVel_mmpermsec, dPos_mm);
        m_pHWSet->m_pHoistAxis->SetHoistOriginFromSmalladd(false);
		break;
	case TPCMD_JOG_HOIST_DOWN_INC:
        dPos_mm = m_pHWSet->m_pHoistAxis->GetCurrentPosition() + (double)m_dJogDistance_mm;
		dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_6P;// * MANUAL_MULTIPLY_2_2P;
        nError = m_pHWSet->m_pHoistAxis->MoveDistance((DWORD)MANUAL_HOIST_ACCTIME_500, (DWORD)MANUAL_HOIST_ACCTIME_500, dVel_mmpermsec, dPos_mm);
        m_pHWSet->m_pHoistAxis->SetHoistOriginFromSmalladd(false);
		break;
         // *** Shift ***
    case TPCMD_JOG_SHIFT_LEFT:
/*		
        dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
        nError = m_pHWSet->m_pShiftAxis->MoveNegative((unsigned long)MANUAL_SHIFT_ACCTIME, dVel_mmpermsec);
*/        

		if(m_pHWSet->m_pShiftAxis->GetCurrentPosition() < SHIFT_MOVELIMIT_N)
		{
		   nError = ERR_SHIFT_OUT_OF_N_LIMIT;
		}

		if (nError == NO_ERR)
		{
			dPos_mm = m_pHWSet->m_pShiftAxis->GetCurrentPosition() - (double)15.0;

			if(dPos_mm < SHIFT_MOVELIMIT_N)
				dPos_mm = SHIFT_MOVELIMIT_N - 0.5;
        }

		if(nError == NO_ERR)
		{
			dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
			nError = m_pHWSet->m_pShiftAxis->MoveDistance((unsigned long)MANUAL_SHIFT_ACCTIME, (unsigned long)MANUAL_SHIFT_ACCTIME, dVel_mmpermsec, dPos_mm);
		}
		break;
	case TPCMD_JOG_SHIFT_RIGHT:
/*
		dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
		nError = m_pHWSet->m_pShiftAxis->MovePositive((unsigned long)MANUAL_SHIFT_ACCTIME, dVel_mmpermsec);
*/

		if(m_pHWSet->m_pShiftAxis->GetCurrentPosition() > SHIFT_MOVELIMIT_P)
		{
		   nError = ERR_SHIFT_OUT_OF_P_LIMIT;
		}

		if (nError == NO_ERR)
		{
			dPos_mm = m_pHWSet->m_pShiftAxis->GetCurrentPosition() + (double)15.0;

			if(dPos_mm >  SHIFT_MOVELIMIT_P)
				dPos_mm = SHIFT_MOVELIMIT_P + 0.5;
		}

		if(nError == NO_ERR)
		{
			dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
			nError = m_pHWSet->m_pShiftAxis->MoveDistance((unsigned long)MANUAL_SHIFT_ACCTIME, (unsigned long)MANUAL_SHIFT_ACCTIME, dVel_mmpermsec, dPos_mm);
		}
		break;
    case TPCMD_JOG_SHIFT_HOME:
        dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
        nError = m_pHWSet->m_pShiftAxis->MoveDistance((unsigned long)MANUAL_SHIFT_ACCTIME, (unsigned long)MANUAL_SHIFT_ACCTIME,
            dVel_mmpermsec, 0.0);
        break;
	case TPCMD_JOG_SHIFT_STOP:
        nError = m_pHWSet->m_pShiftAxis->MoveStop((unsigned long)MANUAL_SHIFT_ACCTIME);
        break;
        
    case TPCMD_JOG_SHIFT_ABS:
        dPos_mm = (double)m_dJogDistance_mm;

		if(dPos_mm > SHIFT_MOVELIMIT_P)
		{
		   nError = ERR_SHIFT_OUT_OF_P_LIMIT;
		}
		else if(dPos_mm < SHIFT_MOVELIMIT_N)
		{
		   nError = ERR_SHIFT_OUT_OF_N_LIMIT;
		}

		if (nError == NO_ERR)
		{
			// by zzang9un 2013.07.17 : 절대값으로 이동 시 속도는 Main에서 정함
			//dVel_mmpermsec = m_dVel_mmpermsec;
			dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
			nError = m_pHWSet->m_pShiftAxis->MoveDistance((unsigned long)MANUAL_SHIFT_ACCTIME, (unsigned long)MANUAL_SHIFT_ACCTIME,
				dVel_mmpermsec, dPos_mm);
        }

        break;
        
	case TPCMD_JOG_SHIFT_LEFT_INC:
		if(m_pHWSet->m_pShiftAxis->GetCurrentPosition() < SHIFT_MOVELIMIT_N)
		{
		   nError = ERR_SHIFT_OUT_OF_N_LIMIT;
		}

		if (nError == NO_ERR)
		{
			dPos_mm = m_pHWSet->m_pShiftAxis->GetCurrentPosition() - (double)m_dJogDistance_mm;

			if(dPos_mm < SHIFT_MOVELIMIT_N)
				dPos_mm = SHIFT_MOVELIMIT_N-0.5;
		}

		if (nError == NO_ERR)
		{
			dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
			nError = m_pHWSet->m_pShiftAxis->MoveDistance((unsigned long)MANUAL_SHIFT_ACCTIME, (unsigned long)MANUAL_SHIFT_ACCTIME,
			dVel_mmpermsec, dPos_mm);
		}
        break;
	case TPCMD_JOG_SHIFT_RIGHT_INC:
		if(m_pHWSet->m_pShiftAxis->GetCurrentPosition() > SHIFT_MOVELIMIT_P)
		{
		   nError = ERR_SHIFT_OUT_OF_P_LIMIT;
		}

		if (nError == NO_ERR)
		{
			dPos_mm = m_pHWSet->m_pShiftAxis->GetCurrentPosition() + (double)m_dJogDistance_mm;

			if(dPos_mm >  SHIFT_MOVELIMIT_P)
				dPos_mm = SHIFT_MOVELIMIT_P+0.5;
		}

		if (nError == NO_ERR)
		{
			dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
			nError = m_pHWSet->m_pShiftAxis->MoveDistance((unsigned long)MANUAL_SHIFT_ACCTIME, (unsigned long)MANUAL_SHIFT_ACCTIME,
				dVel_mmpermsec, dPos_mm);
        }

        break;
        ///@todo Rotate 추가 시 구현 필요

// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate 추가로 인한 구현
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) ||(OHT_NAME == OHT_NAME_STD_V85))
     //*** Rotate ***
     case TPCMD_JOG_ROTATE_CW :
     case TPCMD_JOG_ROTATE_CCW :
     case TPCMD_JOG_ROTATE_HOME :
     case TPCMD_JOG_ROTATE_ABS :
     case TPCMD_JOG_ROTATE_CW_INC :
     case TPCMD_JOG_ROTATE_CCW_INC :
        if(m_pHWSet->m_pHoistAxis->IsHoistHome() != true)
        {
            nError = ERR_HOISTALIGNSEN_NOT_ON;
        }
        else if((m_pHWSet->m_pShutter->IsOpen(FRONT) == false) || (m_pHWSet->m_pShutter->IsOpen(REAR) == false))
        {
            nError = ERR_SHUTTER_NOTORIGIN;
        }
        else if(m_pHWSet->m_pHand->IsRightGripOn() !=  m_pHWSet->m_pHand->IsLeftGripOn())
        {
            nError = ERR_GRIP_SEN_MISMATCH;
        }
        else
        {
            switch(m_ManualCmd.uCmdTag.Info.CmdData)
            {
                 case TPCMD_JOG_ROTATE_CW :
                    nError = m_pHWSet->m_pRotate->Move_Jog_CW();
                    break;

                 case TPCMD_JOG_ROTATE_CCW :
                     nError = m_pHWSet->m_pRotate->Move_Jog_CCW();
                    break;

                 case TPCMD_JOG_ROTATE_HOME :
                     nError = m_pHWSet->m_pRotate->GoHome();
                    break;

                 case TPCMD_JOG_ROTATE_ABS :
	 #if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) ||(OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
                    OHTMainForm->GetTransControl()->Set_CmdEuvRotateMove(m_dJogDistance_mm);
     #else
                    nError = m_pHWSet->m_pRotate->MoveAbsolute(m_dJogDistance_mm);
     #endif
                    break;

                 case TPCMD_JOG_ROTATE_CW_INC :
                    nError = m_pHWSet->m_pRotate->MoveIncremental_CW(m_dJogDistance_mm);
                    break;

                 case TPCMD_JOG_ROTATE_CCW_INC :
                    nError = m_pHWSet->m_pRotate->MoveIncremental_CCW(m_dJogDistance_mm);
                    break;

                 default:
                    break;
            }
        }
        break;

     case TPCMD_JOG_ROTATE_STOP :
            nError = m_pHWSet->m_pRotate->Stop();
        break;

#endif
         // ***
    case TPCMD_JOG_ALLROBOT_HOME:
        dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_6P;
        nError = m_pHWSet->m_pHoistAxis->MoveDistance   (
                                                            (unsigned long)MANUAL_HOIST_ACCTIME_500,
                                                            (unsigned long)MANUAL_HOIST_ACCTIME_500,
                                                            dVel_mmpermsec,
                                                            0.0
                                                        );
        if (nError == NO_ERR)
        {
            dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
            nError = m_pHWSet->m_pShiftAxis->MoveDistance   (
                                                                (unsigned long)MANUAL_SHIFT_ACCTIME,
                                                                (unsigned long)MANUAL_SHIFT_ACCTIME,
                                                                dVel_mmpermsec,
                                                                0.0
                                                            );
        }
// 2016-11-28, Add by Labeler, ROHT의 경우, Rotate 추가로 인한 구현
// 2016-12-05, Add by Labeler, Rotate Axis의 경우, Hoist 축이 원점 도착 상태에서만 조작해야 함
// Hoist Axis가 Home 상태일 때만 Home 처리 하도록 변경
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) ||(OHT_NAME == OHT_NAME_STD_V85))
        if(nError == NO_ERR)
        {
            if(m_pHWSet->m_pHoistAxis->IsHoistHome() != true)
            {
                nError = ERR_HOISTALIGNSEN_NOT_ON;
            }
            else if((m_pHWSet->m_pShutter->IsOpen(FRONT) == false) || (m_pHWSet->m_pShutter->IsOpen(REAR) == false))
            {
                nError = ERR_SHUTTER_NOTORIGIN;
            }
            else
            {
                nError = m_pHWSet->m_pRotate->GoHome();
            }
        }
#endif
        break;

    case TPCMD_JOG_ALLROBOT_STOP:
        nError = m_pHWSet->m_pHoistAxis->MoveStop((DWORD)MANUAL_HOIST_ACCTIME_500);
        if (nError == NO_ERR)
            nError = m_pHWSet->m_pShiftAxis->MoveStop((unsigned long)MANUAL_SHIFT_ACCTIME);
// 2016-11-28, Add by Labeler, ROHT의 경우, Rotate 추가로 인한 구현
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  ||(OHT_NAME == OHT_NAME_STD_V85))
        if(nError == NO_ERR)
        {
            nError = m_pHWSet->m_pRotate->Stop();
        }
#endif
        break;
	case TPCMD_JOG_HOISTSHIFT_HOME:
		dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_6P;
        nError = m_pHWSet->m_pHoistAxis->MoveDistance((unsigned long)MANUAL_HOIST_ACCTIME_500,
            (unsigned long)MANUAL_HOIST_ACCTIME_500, dVel_mmpermsec, 0.0);
        if (nError == NO_ERR)
        {
            dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
            nError = m_pHWSet->m_pShiftAxis->MoveDistance((unsigned long)MANUAL_SHIFT_ACCTIME, (unsigned long)MANUAL_SHIFT_ACCTIME,
                dVel_mmpermsec, 0.0);
        }
        break;
    case TPCMD_JOG_SHIFTROTATE_HOME:
        dVel_mmpermsec = MANUAL_SHIFT_MAX_SPEED * MANUAL_MULTIPLY_34_10P;
        nError = m_pHWSet->m_pShiftAxis->MoveDistance((unsigned long)MANUAL_SHIFT_ACCTIME, (unsigned long)MANUAL_SHIFT_ACCTIME,
            dVel_mmpermsec, 0.0);
// 2016-11-28, Add by Labeler, ROHT의 경우, Rotate 추가로 인한 구현
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
        if(nError == NO_ERR)
		{
            if(m_pHWSet->m_pHoistAxis->IsHoistHome() != true)
            {
                nError = ERR_HOISTALIGNSEN_NOT_ON;
            }
            else if((m_pHWSet->m_pShutter->IsOpen(FRONT) == false) || (m_pHWSet->m_pShutter->IsOpen(REAR) == false))
            {
                nError = ERR_SHUTTER_NOTORIGIN;
            }
            else
            {
                nError = m_pHWSet->m_pRotate->GoHome();
            }
        }
#endif
        break;

	case TPCMD_JOG_HOIST_DIST:
        // by zzang9un 2012.11.29 : TP에서 받은 거리값 적용
        dPos_mm                     = m_dJogDistance_mm;
        dAcceleration_mmpermsec2    = m_dAcceleration_mmpermsec2;
        dDeceleration_ctspersec2    = m_dDeceleration_mmpermsec2;
        dVel_mmpermsec              = m_dVel_mmpermsec;

		nError = m_pHWSet->m_pHoistAxis->MoveDistance (
                                                            dAcceleration_mmpermsec2,
                                                            dDeceleration_ctspersec2,
                                                            dVel_mmpermsec,
                                                            dPos_mm
                                                        );

//        nError = m_pHWSet->m_pHoistAxis->MoveDistance(m_dAcceleration_mmpermsec2, m_dDeceleration_mmpermsec2,
//            m_dVel_mmpermsec, dPos_mm);
        break;
    }
    m_StatusSet.bIsOnProcess = false;

    if(nError==NO_ERR)
    {
        m_StatusSet.byStatus = STS_COMPLETE;
        m_StatusSet.uError==NO_ERR;
    }
    else
    {
        m_StatusSet.byStatus = STS_ERROR;
        m_StatusSet.uError==nError;
    }

    return nError;
}

 
int	ManualControl::ControlRFPIOSet()
{
	int m_nTransStep;
	int nError = 0;
	// 내용 추가.

//	int nPIOID = Load_RFPIOParameter_ID();
//	int nPIOChannel = Load_RFPIOParameter_CH();

	int nPIOID = m_ManualCmd.uTPCmd.TPCmd.Data1;
	int nPIOChannel = m_ManualCmd.uTPCmd.TPCmd.Data2;
	int nPIOCS = 0;
	nError = m_pHWSet->m_pLeftEQPIO->PIODisable(false);
	
	DWORD dwCheckTime = timeGetTime();

	while(OHTUTILITY::PreventOverflowGetTimeMs(dwCheckTime) < 5000)
	{
		  if(OHTMainForm->Get_EQPIO_Type() > TYPE_PIO_IR)
		  {
				if(OHTMainForm->Get_EQPIO_Type() == TYPE_PIO_HYBRID)
				{
					//LimJY_20160119 : For IR+RF PIO m_nPIOType 인자 추가

					nError = m_pHWSet->m_pLeftEQPIO->Set_RFPIO_Data_RS232_Hybrid(TYPE_PIO_HYBRID, nPIOID, nPIOChannel, nPIOCS, m_defualtparam->VHLName);

					if(m_pHWSet->m_pLeftEQPIO->GetRFPIOConnectStep()==8)
					{
						break;
					}
				}
				else
				{
					// RF-PIO ID, Channel Setting
					nError = m_pHWSet->m_pLeftEQPIO->Set_RFPIO_Data_RS232_RF(nPIOID, nPIOChannel, nPIOCS);

					if(m_pHWSet->m_pLeftEQPIO->GetRFPIOConnectStep()==8)
					{
						break;
					}
				}
		  }
	  
		  if(nError != NO_ERR)  break;


	  	Sleep(10);
	  
	}
		                            	  
    m_StatusSet.bIsOnProcess = false;
	///
	if(nError==NO_ERR)
    {
        m_StatusSet.byStatus = STS_COMPLETE;
        m_StatusSet.uError==NO_ERR;
    }
    else
    {
        m_StatusSet.byStatus = STS_ERROR;
        m_StatusSet.uError==nError;
    }

    return nError;
}


/**
@brief   TP에서 보낸 명령 중 축의 원점이나 SW 리미트를 셋팅하는 함수
@author  LSJ
@date    2012.11.14
@return  명령 수행에 문제 없으면 0, 문제 발생 시 해당 에러 코드 리턴
*/
int ManualControl::SetOriginAndLimit()
{
    int nError = 0;
	char szTmpLog[1024] ={0,};



	switch (m_ManualCmd.uCmdTag.Info.CmdData)
    {
	case TPCMD_HOIST_ORIGIN_SETTING:
		sprintf(szTmpLog, "/%s/%s/CurPos:%f",
		OACISType(3),
		OACISResult(0),
		m_pHWSet->m_pHoistAxis->GetCurrentPosition());
		OACISLog(OACIS_LOG_HOISTHOME, szTmpLog);
		nError = m_pHWSet->m_pHoistAxis->SetHome();
		m_pHWSet->m_pHoistAxis->SetHoistOriginFromSmalladd(false);
        break;

	case TPCMD_SHIFT_ORIGIN_SETTING:
		sprintf(szTmpLog, "/%s/%s/CurPos:%f",
		OACISType(3),
		OACISResult(0),
		m_pHWSet->m_pShiftAxis->GetCurrentPosition());
		OACISLog(OACIS_LOG_SHIFTHOME, szTmpLog);
		nError = m_pHWSet->m_pShiftAxis->SetHome();
        break;

// 2016-11-28, Add by Labeler, ROHT의 경우, Rotate 추가로 인한 구현
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  ||(OHT_NAME == OHT_NAME_STD_V85))
    case TPCMD_ROTATE_ORIGIN_SETTING :
        nError = m_pHWSet->m_pRotate->SetOrigin();
        break;

    case TPCMD_ROTATE_GO_ORIGIN :
        if(m_pHWSet->m_pHoistAxis->IsHoistHome() != true)
        {
            nError = ERR_HOISTALIGNSEN_NOT_ON;
        }
        else if((m_pHWSet->m_pShutter->IsOpen(FRONT) == false) || (m_pHWSet->m_pShutter->IsOpen(REAR) == false))
        {
            nError = ERR_SHUTTER_NOTORIGIN;
        }
        else if(m_pHWSet->m_pHand->IsRightGripOn() !=  m_pHWSet->m_pHand->IsLeftGripOn())
        {
            nError = ERR_GRIP_SEN_MISMATCH;
        }
        else
        {
	#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) ||(OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
            OHTMainForm->GetTransControl()->Set_CmdEuvRotateOrigin();
	#else
            nError = m_pHWSet->m_pRotate->Zeroing();
	#endif
        }
        break;
#endif

    case TPCMD_HOIST_SETLIMIT_NEG:
        nError = m_pHWSet->m_pHoistAxis->SetNegativeSWLimit((double)m_dJogDistance_mm);
        break;
    case TPCMD_HOIST_SETLIMIT_POSI:
        nError = m_pHWSet->m_pHoistAxis->SetPositiveSWLimit((double)m_dJogDistance_mm);
        break;
    case TPCMD_SHIFT_SETLIMIT_NEG:
        nError = m_pHWSet->m_pShiftAxis->SetNegativeSWLimit((double)m_dJogDistance_mm);
        break;
    case TPCMD_SHIFT_SETLIMIT_POSI:
        nError = m_pHWSet->m_pShiftAxis->SetPositiveSWLimit((double)m_dJogDistance_mm);
        break;
    }
    m_StatusSet.bIsOnProcess = false;

    if(nError==NO_ERR)
    {
        m_StatusSet.byStatus = STS_COMPLETE;
        m_StatusSet.uError==NO_ERR;
    }
    else
    {
        m_StatusSet.byStatus = STS_ERROR;
        m_StatusSet.uError==nError;
    }


    return nError;
}

/**
@brief   TP에서 보낸 명령 중 HWSet에 구성된 클래스의 명령을 수행하는 함수
@author  LSJ
@date    2012.11.14
@return  명령 수행에 문제 없으면 0, 문제 발생 시 해당 에러 코드 리턴
*/
int ManualControl::ControlHW()
{
    char chReadResult[1024] = {0, };
    int nError = 0;
    int nExec = 0;

    switch (m_ManualCmd.uCmdTag.Info.CmdData)
    {
    case TPCMD_STEER_FRONT_LEFT:
        nError = m_pHWSet->m_pSteer->FrontLeft();
        break;
    case TPCMD_STEER_FRONT_RIGHT:
        nError = m_pHWSet->m_pSteer->FrontRight();
        break;
    case TPCMD_STEER_REAR_LEFT:
        nError = m_pHWSet->m_pSteer->RearLeft();
        break;
    case TPCMD_STEER_REAR_RIGHT:
        nError = m_pHWSet->m_pSteer->RearRight();
        break;

    case TPCMD_HAND_CLOSE:
        nError = m_pHWSet->m_pHand->ManualClose();
        break;
    case TPCMD_HAND_OPEN: // 개발자 Mode시
    case TPCMD_HAND_OPEN_LIMIT: // 근무자 Mode시
        if((m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false))
			nExec = 1;

		else if((fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) > DIST_HOIST_HOME_MARGIN) &&
				m_pHWSet->m_pHand->IsPushOn() == true)
			nExec = 1;
		else
		{
            nExec = 0;
            nError = ERR_HAND_CANNOTOPEN_IN_HOIST_HOME;
        }
        if (nError == NO_ERR && nExec == 1)
            nError = m_pHWSet->m_pHand->ManualOpen();
        break;
	case TPCMD_HAND_HOMING:
        if((m_pHWSet->m_pHand->IsOpen()==true) && (m_pHWSet->m_pHand->IsClose()==true))
        {
            ADD_LOG("Hand Open/Close all On");
            nError = ERR_HAND_LIMIT_SEN_ALL_ON;
        }
        else
        {
            nError = m_pHWSet->m_pHand->Origin();
        }
        break;
    case TPCMD_HAND_RESET:
		nError = m_pHWSet->m_pHand->AlarmReset();
        break;
    case TPCMD_HAND_FREE:
        nError = m_pHWSet->m_pHand->Free();
        break;
    case TPCMD_HAND_ENABLE:
        nError = m_pHWSet->m_pHand->Enable();
        break;
    case TPCMD_HAND_PWR_ON:
        nError = m_pHWSet->m_pHand->HandPowerControl(PWR_ON);
        break;
    case TPCMD_HAND_PWR_OFF:
        nError = m_pHWSet->m_pHand->HandPowerControl(PWR_OFF);
        break;
    case TPCMD_HAND_PWR_RST:
        nError = m_pHWSet->m_pHand->HandPowerControl(PWR_RST);
        break;
         // ***
    case TPCMD_LEFT_PIO_SELECT_ON:
        nError = m_pHWSet->m_pLeftEQPIO->PIOEnable();
        break;
	case TPCMD_LEFT_PIO_SELECT_OFF:               
        nError = m_pHWSet->m_pLeftEQPIO->PIODisable(false);
		break;
	case TPCMD_SET_RFPIO_CHANNEL:
		nError = m_pHWSet->m_pLeftEQPIO->Set_RFPIO_Data_RS232_Hybrid(0,0,0,0,m_defualtparam->VHLName);
		ADD_LOG("[TPCMD_SET_RFPIO_CHANNEL]");
        break;
    case TPCMD_LEFT_PIO_VALID_ON:
        nError = m_pHWSet->m_pLeftEQPIO->ValidOnOff(ON);
		break;
	case TPCMD_LEFT_PIO_VALID_OFF:
        nError = m_pHWSet->m_pLeftEQPIO->ValidOnOff(OFF);
        break;
    case TPCMD_LEFT_PIO_CS_0_ON:
        nError = m_pHWSet->m_pLeftEQPIO->CS0OnOff(ON);
        break;
    case TPCMD_LEFT_PIO_CS_0_OFF:
        nError = m_pHWSet->m_pLeftEQPIO->CS0OnOff(OFF);
        break;
    case TPCMD_LEFT_PIO_CS_1_ON:
        nError = m_pHWSet->m_pLeftEQPIO->CS1OnOff(ON);
        break;
    case TPCMD_LEFT_PIO_CS_1_OFF:
        nError = m_pHWSet->m_pLeftEQPIO->CS1OnOff(OFF);
        break;
    case TPCMD_LEFT_PIO_TR_REQ_ON:
        nError = m_pHWSet->m_pLeftEQPIO->TRREQOnOff(ON);
        break;
    case TPCMD_LEFT_PIO_TR_REQ_OFF:
        nError = m_pHWSet->m_pLeftEQPIO->TRREQOnOff(OFF);
        break;
    case TPCMD_LEFT_PIO_BUSY_ON:
        nError = m_pHWSet->m_pLeftEQPIO->BusyOnOff(ON);
        break;
    case TPCMD_LEFT_PIO_BUSY_OFF:
        nError = m_pHWSet->m_pLeftEQPIO->BusyOnOff(OFF);
        break;
    case TPCMD_LEFT_PIO_COMPT_ON:
        nError = m_pHWSet->m_pLeftEQPIO->CompleteOnOff(ON);
        break;
    case TPCMD_LEFT_PIO_COMPT_OFF:
        nError = m_pHWSet->m_pLeftEQPIO->CompleteOnOff(OFF);
        break;
    case TPCMD_LEFT_PIO_ALL_OFF:
        nError = m_pHWSet->m_pLeftEQPIO->AllPIOOff();
        break;
         // ***
    case TPCMD_RIGHT_PIO_SELECT_ON:
//        nError = m_pHWSet->m_pRightEQPIO->PIOEnable();
		break;
    case TPCMD_RIGHT_PIO_SELECT_OFF:
//		nError = m_pHWSet->m_pRightEQPIO->PIODisable(false);
		break;
    case TPCMD_RIGHT_PIO_VALID_ON:
//		nError = m_pHWSet->m_pRightEQPIO->ValidOnOff(ON);
		break;
    case TPCMD_RIGHT_PIO_VALID_OFF:
//        nError = m_pHWSet->m_pRightEQPIO->ValidOnOff(OFF);
		break;
    case TPCMD_RIGHT_PIO_CS_0_ON:
//        nError = m_pHWSet->m_pRightEQPIO->CS0OnOff(ON);
		break;
    case TPCMD_RIGHT_PIO_CS_0_OFF:
//        nError = m_pHWSet->m_pRightEQPIO->CS0OnOff(OFF);
		break;
    case TPCMD_RIGHT_PIO_CS_1_ON:
//        nError = m_pHWSet->m_pRightEQPIO->CS1OnOff(ON);
		break;
    case TPCMD_RIGHT_PIO_CS_1_OFF:
//        nError = m_pHWSet->m_pRightEQPIO->CS1OnOff(OFF);
		break;
    case TPCMD_RIGHT_PIO_TR_REQ_ON:
//        nError = m_pHWSet->m_pRightEQPIO->TRREQOnOff(ON);
		break;
    case TPCMD_RIGHT_PIO_TR_REQ_OFF:
//		nError = m_pHWSet->m_pRightEQPIO->TRREQOnOff(OFF);
		break;
    case TPCMD_RIGHT_PIO_BUSY_ON:
//        nError = m_pHWSet->m_pRightEQPIO->BusyOnOff(ON);
        break;
    case TPCMD_RIGHT_PIO_BUSY_OFF:
//        nError = m_pHWSet->m_pRightEQPIO->BusyOnOff(OFF);
		break;
    case TPCMD_RIGHT_PIO_COMPT_ON:
//        nError = m_pHWSet->m_pRightEQPIO->CompleteOnOff(ON);
        break;
    case TPCMD_RIGHT_PIO_COMPT_OFF:
//        nError = m_pHWSet->m_pRightEQPIO->CompleteOnOff(OFF);
        break;
    case TPCMD_RIGHT_PIO_ALL_OFF:
	   //	nError = m_pHWSet->m_pRightEQPIO->AllPIOOff();
        break;
         // ***
	case TPCMD_STEER_LEFT:
		nError = m_pHWSet->m_pSteer->Left(false);
		// m_pHWSet->m_pCID->CID_Output_Clear();
		break;
	case TPCMD_STEER_RIGHT:
		nError = m_pHWSet->m_pSteer->Right(false);
		// m_pHWSet->m_pCID->CID_Output_Clear();
		break;

	case TPCMD_STEER_F_FREE:
		nError = m_pHWSet->m_pSteer->FrontFree();
		break;
	case TPCMD_STEER_F_ENABLE:
		nError = m_pHWSet->m_pSteer->FrontEnable();
		break;
	case TPCMD_STEER_R_FREE:
		nError = m_pHWSet->m_pSteer->RearFree();
        break;
    case TPCMD_STEER_R_ENABLE:
        nError = m_pHWSet->m_pSteer->RearEnable();
        break;
    case TPCMD_TRACTION_STEER_LEFT:
        nError = m_pHWSet->m_pSteer->Left(true);
        break;
    case TPCMD_TRACTION_STEER_RIGHT:
        nError = m_pHWSet->m_pSteer->Right(true);
        break;
         // ***
    case TPCMD_SHUTTER_ALARM_RST:
        nError = m_pHWSet->m_pShutter->AlarmReset();
        break;
    case TPCMD_SHUTTER_CLOSE:
// 2016-12-05, Add by Labeler, ROHT의 경우, Rotate 추가로 인해 Shutter Close 시 Interlock 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) ||(OHT_NAME == OHT_NAME_STD_V85))
		if(((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
		&& (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
		&& (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree() - ROTATE_POS_90)  > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
		&& (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree() - ROTATE_POS_180) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
        {
            nError = ERR_ROTATE_NOT_INPOSITION;
		}
		else if(m_pHWSet->m_pHoistAxis->IsHoistHome() != true)
		{
			nError = ERR_HOISTALIGNSEN_NOT_ON;
		}
        else
        {
#if((OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1)  ||(OHT_NAME == OHT_NAME_STD_V85))
            if(((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
#if(OHT_NAME == OHT_NAME_STD_V80_RETICLE) 
  		   && ((m_pHWCommon->m_Input.POD_EUV_1 == true) || (m_pHWCommon->m_Input.POD_EUV_2 == true)))
#else
           && ((m_pHWCommon->m_Input.POD_EUV_1 == true)))
#endif
			//&& ((m_pHWCommon->m_Input.POD_EUV_1 == true) || (m_pHWCommon->m_Input.POD_EUV_2 == true)))
            {
                nError = ERR_EUVROHT_EUVON_FULLCLOSE;
			}
            else
#endif
            {
                nError = m_pHWSet->m_pShutter->Close();
            }
        }
#else
        nError = m_pHWSet->m_pShutter->Close();
#endif
        break;

	case TPCMD_SHUTTER_MID_CLOSE:
// 셔터를 mid로 닫는 동작이므로 EUV에게만 의미가 있다
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) ||(OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
        if(((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
        && (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
       	&& (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree() - ROTATE_POS_90)  > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
		&& (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree() - ROTATE_POS_180) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
        {
            nError = ERR_ROTATE_NOT_INPOSITION;
		}
		else if(m_pHWSet->m_pHoistAxis->IsHoistHome() != true)
		{
			nError = ERR_HOISTALIGNSEN_NOT_ON;
		}
        else
        {
            nError = m_pHWSet->m_pShutter->Mid_Close();
		}
#endif
		break;

	case TPCMD_SHUTTER_OPEN:

		if ((nError == NO_ERR) && (m_pHWSet->m_pHand->IsClose() != true))
		{
            nExec = 0;
			nError = ERR_SHUTTER_CANTMOVE_HAND_OPEN;
        }
			  // *** Hand Open/Close를 만족해야 Shutter Open함

		if (((m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false)) ||
			((m_pHWSet->m_pHand->IsRightGripOn() == true) && (m_pHWSet->m_pHand->IsLeftGripOn() == true) &&
			(m_pHWSet->m_pHand->IsClose() == true) && (m_pHWSet->m_pHand->IsOpen() == false)))
			nExec = 1;
		else
		{
            nExec = 0;
			nError = ERR_SHR_MISMATCH_FOUP_SENSING;
		}

		// Foup 존재하는데 Inner Foup센서가 감지되지 않으면 ==> 에러 발생.

		if ((nError == NO_ERR) && ((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true)) &&
			(m_pHWSet->m_pHoistAxis->IsHoistHome() == true) && (m_pHWSet->m_pShiftAxis->IsShiftHome() == true) &&
			(m_pHWSet->m_pInnerFoup->IsFoupDetect() == false))
        {
            nExec = 0;
			nError = ERR_INNER_FOUP_NOT_DETECT;
		}
		// Foup 존재하지 않는데 Inner Foup센서가 감지되면 ==> 에러 발생.

		if ((nError == NO_ERR) && (m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false) &&
			(m_pHWSet->m_pHoistAxis->IsHoistHome() == true) && (m_pHWSet->m_pShiftAxis->IsShiftHome() == true) &&
			(m_pHWSet->m_pInnerFoup->IsFoupDetect() == true))
        {
            nExec = 0;
			nError = ERR_INNER_FOUP_DETECT;
		}

        if (nError == NO_ERR && nExec == 1)
            nError = m_pHWSet->m_pShutter->Open();
        break;
	case TPCMD_SHUTTER_ORIGIN:

		if ((nError == NO_ERR) && (m_pHWSet->m_pHand->IsClose() != true))
		{
            nExec = 0;
            nError = ERR_SHUTTER_CANTMOVE_HAND_OPEN;
		}
			  // *** Hand Open/Close를 만족해야 Shutter Open함

		if (((m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false)) ||
			((m_pHWSet->m_pHand->IsRightGripOn() == true) && (m_pHWSet->m_pHand->IsLeftGripOn() == true) &&
			(m_pHWSet->m_pHand->IsClose() == true) && (m_pHWSet->m_pHand->IsOpen() == false)))
			nExec = 1;
		else
		{
            nExec = 0;
			nError = ERR_SHR_MISMATCH_FOUP_SENSING;
		}

		// Foup 존재하는데 Inner Foup센서가 감지되지 않으면 ==> 에러 발생.

		if ((nError == NO_ERR) && ((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true)) &&
			(m_pHWSet->m_pHoistAxis->IsHoistHome() == true) && (m_pHWSet->m_pShiftAxis->IsShiftHome() == true) &&
			(m_pHWSet->m_pInnerFoup->IsFoupDetect() == false))
		{
			nExec = 0;
			nError = ERR_INNER_FOUP_NOT_DETECT;
		}

		// Foup 존재하지 않는데 Inner Foup센서가 감지되면 ==> 에러 발생.

		if ((nError == NO_ERR) && (m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false) &&
			(m_pHWSet->m_pHoistAxis->IsHoistHome() == true) && (m_pHWSet->m_pShiftAxis->IsShiftHome() == true) &&
			(m_pHWSet->m_pInnerFoup->IsFoupDetect() == true))
		{
			nExec = 0;
			nError = ERR_INNER_FOUP_DETECT;
		}
		
		if (nError == NO_ERR && nExec == 1)
        {
            nError = m_pHWSet->m_pShutter->Origin();
        }
        break;
    case TPCMD_SHUTTER_FREE:
        nError = m_pHWSet->m_pShutter->Free();
        break;
    case TPCMD_SHUTTER_ENABLE:
        nError = m_pHWSet->m_pShutter->Enable();
        break;
            // ***

// 2016-11-30, Labeler
	case TPCMD_RAILSUPPORT_WORK:
   	case TPCMD_RAILSUPPORT_HOME:
    case TPCMD_RAILSUPPORT_FREE:
    case TPCMD_RAILSUPPORT_ENABLE:
        break;

         // ***
    case TPCMD_MC_CONTROL_ON:
        nError = m_pHWSet->m_pPowerControl->MCControl(ON);
        break;
	case TPCMD_MC_CONTROL_OFF:
		nError = m_pHWSet->m_pPowerControl->MCControl(OFF);
		break;

		// *** CID

	case TPCMD_CID_LEFT_SEL:
		ADD_LOG("TPCMD_CID_LEFT_SEL");
		nError = m_pHWSet->m_pCID->CID_Out_Left_Select(ON);
		break;
	case TPCMD_CID_RIGHT_SEL:
		ADD_LOG("TPCMD_CID_RIGHT_SEL");
		nError = m_pHWSet->m_pCID->CID_Out_Right_Select(ON);
		break;
	case TPCMD_CID_OCCUPREQ:
		ADD_LOG("TPCMD_CID_OCCUPREQ");
		nError = m_pHWSet->m_pCID->CID_Out_Occup_Req(ON);
		break;
	case TPCMD_CID_PASSCOMPT:
		ADD_LOG("TPCMD_CID_PASSCOMPT");
		nError = m_pHWSet->m_pCID->CID_Out_PASS_Compt(ON);
		break;
	case TPCMD_CID_MANUAL:
		ADD_LOG("TPCMD_CID_MANUAL");
		nError = m_pHWSet->m_pCID->CID_Out_Manual(ON);
		break;
	case TPCMD_CID_OPERATION:
		ADD_LOG("TPCMD_CID_OPERATION");
		nError = m_pHWSet->m_pCID->CID_Out_Operation(ON);
		break;
	case TPCMD_CID_RESERVE_1:
		nError = m_pHWSet->m_pCID->CID_Out_Common_Tag(9, ON);
		break;
	case TPCMD_CID_RESERVE_2:
		nError = m_pHWSet->m_pCID->CID_Out_Reserved_2(ON);
		break;

	case TPCMD_CID_LEFT_SEL_OFF:
		nError = m_pHWSet->m_pCID->CID_Out_Left_Select(OFF);
		break;
	case TPCMD_CID_RIGHT_SEL_OFF:
		nError = m_pHWSet->m_pCID->CID_Out_Right_Select(OFF);
		break;
	case TPCMD_CID_OCCUPREQ_OFF:
		nError = m_pHWSet->m_pCID->CID_Out_Occup_Req(OFF);
		break;
	case TPCMD_CID_PASSCOMPT_OFF:
		nError = m_pHWSet->m_pCID->CID_Out_PASS_Compt(OFF);
		break;
	case TPCMD_CID_MANUAL_OFF:
		nError = m_pHWSet->m_pCID->CID_Out_Manual(OFF);
		break;
	case TPCMD_CID_OPERATION_OFF:
		nError = m_pHWSet->m_pCID->CID_Out_Operation(OFF);
		break;
	case TPCMD_CID_RESERVE_1_OFF:
		nError = m_pHWSet->m_pCID->CID_Out_Common_Tag(10, OFF);
		break;
	case TPCMD_CID_RESERVE_2_OFF:
		nError = m_pHWSet->m_pCID->CID_Out_Reserved_2(OFF);
		break;


         // ***
		 /*
	case TPCMD_STB_R_WORK_ON:
		nError = m_pHWSet->m_pSTBAccess->STBRWorkOn();
		break;
	case TPCMD_STB_R_WORK_OFF:
		nError = m_pHWSet->m_pSTBAccess->STBRWorkOff();
		break;
	case TPCMD_STB_L_WORK_ON:
		nError = m_pHWSet->m_pSTBAccess->STBLWorkOn();
		break;
	case TPCMD_STB_L_WORK_OFF:
		nError = m_pHWSet->m_pSTBAccess->STBLWorkOff();
		break;
		*/
         // ***
    case TPCMD_OBSTACLE_POWER_ON:
        nError = m_pHWSet->m_pObstacle->PowerControl(PWR_ON);
        break;
    case TPCMD_OBSTACLE_POWER_OFF:
        nError = m_pHWSet->m_pObstacle->PowerControl(PWR_OFF);
        break;
    case TPCMD_OBSTACLE_POWER_RESET:
        nError = m_pHWSet->m_pObstacle->PowerControl(PWR_RST);
        break;
         // ***
    case TPCMD_OHTDETECT_POWER_OFF:
        nError = m_pHWSet->m_pOHTDetect->PowerControl(PWR_OFF);
        break;
    case TPCMD_OHTDETECT_POWER_ON:
        nError = m_pHWSet->m_pOHTDetect->PowerControl(PWR_ON);
        break;
    case TPCMD_OHTDETECT_POWER_RESET:
        nError = m_pHWSet->m_pOHTDetect->PowerControl(PWR_RST);
        break;
    case TPCMD_OBSTACLE_ENABLE:
        nError = m_pHWSet->m_pObstacle->PowerControl(PWR_ENABLE);
        break;
    case TPCMD_OHTDETECT_ENABLE:
        nError = m_pHWSet->m_pOHTDetect->PowerControl(PWR_ENABLE);
        break;
    case TPCMD_OBSTACLE_AREA1:
    case TPCMD_OBSTACLE_AREA2:
    case TPCMD_OBSTACLE_AREA3:
    case TPCMD_OBSTACLE_AREA4:
    case TPCMD_OBSTACLE_AREA5:
    case TPCMD_OBSTACLE_AREA6:
    case TPCMD_OBSTACLE_AREA7:
    case TPCMD_OBSTACLE_AREA8:
    case TPCMD_OBSTACLE_AREA9:
    case TPCMD_OBSTACLE_AREA10:
    case TPCMD_OBSTACLE_AREA11:
    case TPCMD_OBSTACLE_AREA12:
    case TPCMD_OBSTACLE_AREA13:
    case TPCMD_OBSTACLE_AREA14:
    case TPCMD_OBSTACLE_AREA15:
        nError = m_pHWSet->m_pObstacle->AreaSelect(m_ManualCmd.uCmdTag.Info.CmdData - TPCMD_OBSTACLE_AREA1 + 1);
        break;

         // ** OHT Detect **
    case TPCMD_OHTDETECT_AREA1:
    case TPCMD_OHTDETECT_AREA2:
    case TPCMD_OHTDETECT_AREA3:
    case TPCMD_OHTDETECT_AREA4:
    case TPCMD_OHTDETECT_AREA5:
    case TPCMD_OHTDETECT_AREA6:
    case TPCMD_OHTDETECT_AREA7:
    case TPCMD_OHTDETECT_AREA8:
    case TPCMD_OHTDETECT_AREA9:
    case TPCMD_OHTDETECT_AREA10:
    case TPCMD_OHTDETECT_AREA11:
    case TPCMD_OHTDETECT_AREA12:
    case TPCMD_OHTDETECT_AREA13:
    case TPCMD_OHTDETECT_AREA14:
	case TPCMD_OHTDETECT_AREA15:

		ADD_LOG("SetVHLDetectIOSet! Mode:%d, Status:%d,IOStatus:%d",m_pExecuteInfo->ExecuteMode,m_StatusSet.byStatus,m_pHWSet->GetVHLDetectForceIOStatus() );

		if(m_pExecuteInfo->ExecuteMode == EXECUTE_MODE_MANUAL)
		{
			if((fabs(m_pHWSet->m_pDrivingAxis->GetCurrentVelocity() < MANUAL_DRIVING_MOVE_SPEED_LIMIT)) && (m_StatusSet.byStatus != STS_GOING))
			{
				if(m_pHWSet->GetVHLDetectForceIOStatus() ==0)
				{
					m_pHWSet->m_pDrivingAxis->SetVHLDetectIOSet(true);
					ADD_LOG("SetVHLDetectIOSet!");
				}
				nError = m_pHWSet->m_pOHTDetect->AreaSelect(m_ManualCmd.uCmdTag.Info.CmdData - TPCMD_OHTDETECT_AREA1 + 1);

			}
		}
		break;
         // ***
    case TPCMD_LOOKDOWN_ENABLE:
        nError = m_pHWSet->m_pLookdown->Enable();
        break;
    case TPCMD_LOOKDOWN_DISABLE:
        nError = m_pHWSet->m_pLookdown->Disable();
        break;
         // ***
    case TPCMD_SERVO_DRV_F_RESET:
    case TPCMD_SERVO_DRV_R_RESET:
        nError = m_pHWSet->m_pDrivingAxis->Reset();
        break;
    case TPCMD_SERVO_HOIST_RESET:
        nError = m_pHWSet->m_pHoistAxis->Reset();
        break;
    case TPCMD_SERVO_SHIFT_RESET:
        nError = m_pHWSet->m_pShiftAxis->Reset();
		break;

         ///@todo rotate 추가 후 구현 필요
         /*
         case TPCMD_SERVO_ROTATE_RESET :
            nError = gm_IOControl->ServoReset(AXIS_ROTATE);
            break;
 */
    case TPCMD_SERVO_RESET:
        nError = m_pHWSet->m_pDrivingAxis->Reset();
        if (nError == NO_ERR)
            nError = m_pHWSet->m_pHoistAxis->Reset();
        if (nError == NO_ERR)
            nError = m_pHWSet->m_pShiftAxis->Reset();
        break;
         // ***
    case TPCMD_DRIVING_BRK_RELEASE:
        nError = m_pHWSet->m_pDrivingAxis->SetBrakeRelease(true);
        break;
    case TPCMD_DRIVING_BRK_LOCK:
        nError = m_pHWSet->m_pDrivingAxis->SetBrakeRelease(false);
        break;
         // ***
    case TPCMD_SAFETY_BD_CLEAR:
        nError = m_pHWSet->m_pBumper->ResetBumperPress();
        break;
        
    case TPCMD_SOUND_ON:
        // by zzang9un 2013.09.02 : Data1에 재생해야 할 Sound 정보가 있음
        switch (m_ManualCmd.uTPCmd.TPCmd.Data1)
        {
        case 1:
            m_pHWSet->Sound_On(SETSOUND_ERROR);
            break;
        case 2:
            m_pHWSet->Sound_On(SETSOUND_DRIVING);
            break;
        case 3:
            m_pHWSet->Sound_On(SETSOUND_TRANS);
            break;
        case 4:
            m_pHWSet->Sound_On(SETSOUND_OBS);
            break;
        }
        break;

    case TPCMD_SOUND_OFF:
        m_pHWSet->Sound_Off();
        break;
    case TPCMD_TRANS_BCR_READ:
        nError = m_pHWSet->m_pTransBCR->SendTriggerOn();    // Manual 경우 ManualControl에서 확인하여 Update 함 //Read(chReadResult);
        break;
    case TPCMD_DRIVING_BCR_READ:
        nError = m_pHWSet->m_pDrivingBCR->SendTriggerOn(); // Manual 경우 ManualControl에서 확인하여 Update 함//Read(chReadResult);
        break;
    case TPCMD_LEFT_QRCODE_READ:
        nError = m_pHWSet->m_pLeftQRR->SendTriggerOn(); // Manual 경우 ManualControl에서 확인하여 Update 함//Read(chReadResult);
        break;
    case TPCMD_RIGHT_QRCODE_READ:
        nError = m_pHWSet->m_pRightQRR->SendTriggerOn(); // Manual 경우 ManualControl에서 확인하여 Update 함//Read(chReadResult);
        break;
	case TPCMD_DRIVING_F_SERVO_ON:
		nError = m_pHWSet->m_pDrivingAxis->SetAMPEnable(true);
		break;
    case TPCMD_DRIVING_R_SERVO_ON:
		nError = m_pHWSet->m_pDrivingAxis->SetSlaveAMPEnable(true);
        break;
	case TPCMD_DRIVING_F_SERVO_OFF:
		nError = m_pHWSet->m_pDrivingAxis->SetAMPEnable(false);
		break;
    case TPCMD_DRIVING_R_SERVO_OFF:
		nError = m_pHWSet->m_pDrivingAxis->SetSlaveAMPEnable(false);
		break;
	case TPCMD_HOIST_SERVO_ON:
		nError = m_pHWSet->m_pHoistAxis->SetAMPEnable(true);
#if((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V81S)|| (OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
		if (nError == NO_ERR)
			nError = m_pHWSet->m_pHoistAxis->HoistExternalBrakeReleaseOnOFF(true);
#endif
		break;
	case TPCMD_HOIST_SERVO_OFF:
			nError = m_pHWSet->m_pHoistAxis->SetAMPEnable(false);
        break;
    case TPCMD_SHIFT_SERVO_ON:
		nError = m_pHWSet->m_pShiftAxis->SetAMPEnable(true);
        break;
    case TPCMD_SHIFT_SERVO_OFF:
        nError = m_pHWSet->m_pShiftAxis->SetAMPEnable(false);
        break;

// 2016-11-28, Add by Labeler, ROHT의 경우, Rotate 추가로 인한 구현
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  ||(OHT_NAME == OHT_NAME_STD_V85))
    case TPCMD_ROTATE_SERVO_ON :
        nError = m_pHWSet->m_pRotate->Enable();
        break;
	case TPCMD_ROTATE_SERVO_OFF :
        nError = m_pHWSet->m_pRotate->Disable();
        break;
#endif

    case TPCMD_ALL_SERVO_ON:
		nError = m_pHWSet->m_pDrivingAxis->SetAMPEnable(true);
		if (nError == NO_ERR)
			nError = m_pHWSet->m_pHoistAxis->SetAMPEnable(true);
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) ||(OHT_NAME == OHT_NAME_STD_V85))
		if (nError == NO_ERR)
			nError = m_pHWSet->m_pHoistAxis->HoistExternalBrakeReleaseOnOFF(true);
#endif
		if (nError == NO_ERR)
            nError = m_pHWSet->m_pShiftAxis->SetAMPEnable(true);
// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) ||(OHT_NAME == OHT_NAME_STD_V85))
		if (nError == NO_ERR)
            nError = m_pHWSet->m_pRotate->Enable();
#endif
        break;
    case TPCMD_ALL_SERVO_OFF:
		nError = m_pHWSet->m_pDrivingAxis->SetAMPEnable(false);
		if (nError == NO_ERR)
			nError = m_pHWSet->m_pHoistAxis->SetAMPEnable(false);
        if (nError == NO_ERR)
            nError = m_pHWSet->m_pShiftAxis->SetAMPEnable(false);
// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  ||(OHT_NAME == OHT_NAME_STD_V85))
        if(nError == NO_ERR)
            nError = m_pHWSet->m_pRotate->Disable();
#endif
		break;
	case TPCMD_DISTANCE_SENSOR_REQUEST_STOP:
	case TPCMD_PBS_SENSOR_REQUEST_START:
	case TPCMD_UBG_SENSOR_REQUEST_START:
	case TPCMD_OBS_SENSOR_REQUEST_START:
        OHTMainForm->GetStatusCheckThread()->SetDistanceSensorType(m_ManualCmd.uCmdTag.Info.CmdData-TPCMD_DISTANCE_SENSOR_REQUEST_STOP);
	    break;
    }
    m_StatusSet.bIsOnProcess = false;

    if(nError==NO_ERR)
    {
        m_StatusSet.byStatus = STS_COMPLETE;
        m_StatusSet.uError = NO_ERR;
    }
    else
    {
        m_StatusSet.byStatus = STS_ERROR;
        m_StatusSet.uError = nError;
    }


    return nError;
}


/**
@brief   Driving 명령 함수
@author  임태웅
@date    2012.11.07
@return
*/
int ManualControl::ControlDriving()
{
	int nError = 0;
	MANUAL_DRIVING_CMD_INFO Info;
	Info.dAccel = m_dAcceleration_mmpermsec2;
	Info.dDecel = m_dDeceleration_mmpermsec2;
	Info.dSpeed = m_dVel_mmpermsec;
	Info.dDist  = m_dJogDistance_mm;
	Info.bNew   = m_bNew;
	m_bNew      = false;

	if((m_pExecuteInfo->ExecuteMode != EXECUTE_MODE_MANUAL) || (!m_pExecuteInfo->bJogInterlockIgnore))
	{
		if ((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
		{
			if ((m_pHWSet->m_pShutter->IsClose(FRONT) == false) || (m_pHWSet->m_pShutter->IsClose(REAR) == false))
			{
				nError = ERR_SHUTTER_NOT_CLOSE;
			}
		}
	}

    switch(m_ManualCmd.uCmdTag.Info.CmdData)
    {   
	case TPCMD_GO_MARK_FORWARD :
	case TPCMD_GO_TRANS_MARK_FORWARD :
	case TPCMD_GO_TRANS_MARK_BACKWARD :    //추가
    case TPCMD_GO_QR_LEFT_MARK_FORWARD :
    case TPCMD_GO_QR_RIGHT_MARK_FORWARD :
    case TPCMD_JOG_DRIVING_STOP:
    case TPCMD_JOG_DRIVING_FORWARD:
	case TPCMD_JOG_DRIVING_BACKWARD:
	case TPCMD_JOG_DRIVING_DIST:
		if(!m_pHWSet->m_pHoistAxis->IsHoistHome())
		{
			nError = ERR_HOIST_NOTORIGIN;
		}
		if(!m_pHWSet->m_pShiftAxis->IsShiftHome())
		{
			nError = ERR_SHIFT_NOTORIGIN;
		}

        // 2016-12-05, Add by Labeler, ROHT의 경우 Rotate 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) ||(OHT_NAME == OHT_NAME_STD_V85))
        if((nError == NO_ERR)
		&& (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
		&& (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree() - ROTATE_POS_90) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
		&& (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree() - ROTATE_POS_180) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
        {
            nError = ERR_ROTATE_NOT_INPOSITION;
        }
#endif
		break;
	}


    //1 1. 명령 분류 및 처리
    if(nError==NO_ERR)
    {
		switch(m_ManualCmd.uCmdTag.Info.CmdData)
        {   
		case TPCMD_GO_MARK_FORWARD :
			m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_NODE);
			break;
        case TPCMD_GO_TRANS_MARK_FORWARD :
            m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_STATION_STOP);
			break;
		///추가
		case TPCMD_GO_TRANS_MARK_BACKWARD :
			m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_STATION_STOP_BACK);
			break;
		////
        case TPCMD_GO_QR_LEFT_MARK_FORWARD :
            m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_STATION_QR_LEFT);
            break;
        case TPCMD_GO_QR_RIGHT_MARK_FORWARD :
            m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_STATION_QR_RIGHT);
            break;

        case TPCMD_JOG_DRIVING_STOP:
            Info.CmdType    = MANUAL_DRIVING_CMD_STOP;
            m_pVirtualDrivingControl->Manual_Driving(Info);
            break;
        case TPCMD_JOG_DRIVING_FORWARD:
            Info.CmdType    = MANUAL_DRIVING_CMD_FORWARD;
            m_pVirtualDrivingControl->Manual_Driving(Info);
            break;
        case TPCMD_JOG_DRIVING_BACKWARD:
            Info.CmdType    = MANUAL_DRIVING_CMD_BACKWARD;
            m_pVirtualDrivingControl->Manual_Driving(Info);
            break;
		case TPCMD_JOG_DRIVING_DIST:
			Info.CmdType    = MANUAL_DRIVING_CMD_DIST;
			ADD_LOG("[TPCMD_JOG_DRIVING_DIST] Data1:%f, Data2:%f, Data3:%f, Data4:%d",Info.dDecel,Info.dSpeed,Info.dDist,Info.bNew);
            m_pVirtualDrivingControl->Manual_Driving(Info);
            break;
        }
    }
    
    m_StatusSet.byStatus        = m_pVirtualDrivingControl->GetStatus();
    m_StatusSet.bIsOnProcess    = m_pVirtualDrivingControl->IsOnProcess();
	m_StatusSet.uError      = m_pVirtualDrivingControl->GetError();
	m_StatusSet.uStep		= m_pVirtualDrivingControl->GetStep();

    return nError;
}
// ---------------------------------------------------------------------------

/**
@brief   ManualControl 명령을 내부 변수로 복사하는 함수
@author  zzang9un
@date    2013.08.17
@return  무조건 0을 리턴
*/
EXECUTE_ACK ManualControl::InsertGeneralCmd(void *sManualControlInfo)
{
	if(m_StatusSet.uStep >= DRIVING_CTRL_STEP_MANUAL_MARK_INIT && m_StatusSet.uStep <= DRIVING_CTRL_STEP_MANUAL_MARK_COMPLETE)
	{
		return EXECUTE_ACK_ON_ERR;
	}

	memcpy(&m_ManualCmd, (EXECUTE_MANUALCONTROL_INFO *)sManualControlInfo, sizeof(EXECUTE_MANUALCONTROL_INFO));

    ADD_LOG("[InsertGeneralCmd] CmdData:%04x, Data1:%d, Data2:%d, Data3:%d, Data4:%d", 
        m_ManualCmd.uCmdTag.Info.CmdData,
        m_ManualCmd.uTPCmd.TPCmd.Data1,
        m_ManualCmd.uTPCmd.TPCmd.Data2,
        m_ManualCmd.uTPCmd.TPCmd.Data3,
        m_ManualCmd.uTPCmd.TPCmd.Data4);

    // by zzang9un 2013.08.17 : 이 함수에서 bIsOnPrecess를 true로 변경해야 한다.
    m_StatusSet.bIsOnProcess = true;
    m_StatusSet.byStatus = STS_RUNNING;
    m_StatusSet.uStep = 0;
    m_bNew  = true;

    return EXECUTE_ACK_NO_ERR;
}
//---------------------------------------------------------------------------

UINT ManualControl::ExecuteJob(void)
{
    int nError = NO_ERR;

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.08.17 : TP Command에 따른 분류
    switch(m_ManualCmd.uCmdTag.Info.CmdData)
    {
    case TPCMD_SET_OUTPUT:      // IO On 명령
    case TPCMD_RESET_OUTPUT:    // IO Off 명령
        m_nIOControlOutIndex = m_ManualCmd.uTPCmd.TPCmd.Data1;
        break;  

    case TPCMD_USE_ON:
        m_UseIndex = m_ManualCmd.uTPCmd.TPCmd.Data1;
        m_bUseOnOff = true;
        break;
        
    case TPCMD_USE_OFF:
        m_UseIndex = m_ManualCmd.uTPCmd.TPCmd.Data1;
        m_bUseOnOff = false;
        break;        

    case TPCMD_JOG_DRIVING_FORWARD:         // 주행 Jog 명령
    case TPCMD_JOG_DRIVING_BACKWARD:
		m_dAcceleration_mmpermsec2  = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_DRV.Accel / 1000000.0;  ///< 조그 주행 시 가속도 : iData1
        m_dDeceleration_mmpermsec2  = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_DRV.Decel / 1000000.0;  ///< 조그 주행 시 감속도 : iData2
        m_dVel_mmpermsec            = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_DRV.Velocity / 1000.0;  ///< 조그 주행 시 속도   : iData3
        break;
		
    case TPCMD_JOG_DRIVING_DIST:
    case TPCMD_JOG_DRIVING_STOP:
    case TPCMD_RFTPJOG_DRIVING_FORWARD:
	case TPCMD_RFTPJOG_DRIVING_BACKWARD:
    case TPCMD_RFTPJOG_DRIVING_STOP:    
        m_dAcceleration_mmpermsec2  = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_DIST.Accel / 1000000.0;  ///< 조그 주행 시 가속도 : iData1
        m_dDeceleration_mmpermsec2  = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_DIST.Decel / 1000000.0;  ///< 조그 주행 시 감속도 : iData2
        m_dVel_mmpermsec            = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_DIST.Velocity / 1000.0;  ///< 조그 주행 시 속도   : iData3
        m_dJogDistance_mm           = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_DIST.Distance;   ///< 조그 이동 시 거리값 : iData4
        break;

    case TPCMD_JOG_HOIST_UP:                // 이적재 Jog 명령
    case TPCMD_JOG_HOIST_DOWN:
    case TPCMD_JOG_HOIST_HOME:      
    case TPCMD_JOG_HOIST_STOP:      
    case TPCMD_JOG_HOIST_ABS:       
    case TPCMD_JOG_HOIST_UP_INC:
    case TPCMD_JOG_HOIST_DOWN_INC:  
    case TPCMD_JOG_HOIST_DIST:      
    case TPCMD_JOG_SHIFT_LEFT:      
    case TPCMD_JOG_SHIFT_RIGHT:     
    case TPCMD_JOG_SHIFT_HOME:      
    case TPCMD_JOG_SHIFT_STOP:      
	case TPCMD_JOG_SHIFT_ABS:
    case TPCMD_JOG_SHIFT_LEFT_INC:  
    case TPCMD_JOG_SHIFT_RIGHT_INC: 
		m_dAcceleration_mmpermsec2  = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_TR.AccelTime / 1000000.0;   ///< 조그 주행 시 가속도 : iData1
        m_dDeceleration_mmpermsec2  = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_TR.DecelTime / 1000000.0;   ///< 조그 주행 시 감속도 : iData2
        m_dVel_mmpermsec            = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_TR.Velocity / 1000.0;   ///< 조그 주행 시 속도   : iData3

        // by zzang9un 2013.09.10 : Hoist, Shift 단위 변경(1/1000m -> 1/10000m)
        m_dJogDistance_mm           = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_TR.Distance / 10.0;    ///< 조그 이동 시 거리값 : iData4
		break;

    case TPCMD_JOG_ROTATE_CW:       
    case TPCMD_JOG_ROTATE_CCW:      
    case TPCMD_JOG_ROTATE_HOME:     
	case TPCMD_JOG_ROTATE_STOP:
    case TPCMD_JOG_ROTATE_ABS:      
    case TPCMD_JOG_ROTATE_CW_INC:
    case TPCMD_JOG_ROTATE_CCW_INC:  
    case TPCMD_JOG_ALLROBOT_HOME:
    case TPCMD_JOG_ALLROBOT_STOP:   
    case TPCMD_JOG_HOISTSHIFT_HOME: 
    case TPCMD_JOG_SHIFTROTATE_HOME:
        m_dAcceleration_mmpermsec2  = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_TR.AccelTime / 1000000.0;   ///< 조그 주행 시 가속도 : iData1
        m_dDeceleration_mmpermsec2  = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_TR.DecelTime / 1000000.0;   ///< 조그 주행 시 감속도 : iData2
        m_dVel_mmpermsec            = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_TR.Velocity / 1000.0;   ///< 조그 주행 시 속도   : iData3
        m_dJogDistance_mm           = (double)m_ManualCmd.uTPCmd.CmdTP_Jog_TR.Distance;    ///< 조그 이동 시 거리값 : iData4
		break;
    }
    // [END] by zzang9un 2013.08.17 : TP Command에 따른 분류
    // -------------------------------------------

    ///@todo MainThread에서 구현 필요
    /*
   if(((nCommandType>=101)&&(nCommandType<=110))||(nCommandType==120)||((nCommandType>=TPCMD_CHANGE_MANUAL_LOAD_MAP)&&(nCommandType<=TPCMD_CHANGE_MANUAL_LOAD_CROSSRAIL)
      || nCommandType == TPCMD_CHANGE_MANUAL_LOAD_PIODELAYNODES))
   {
      TP_SetMode();
   }
    */

    if((m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_SET_OUTPUT) || 
        (m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_RESET_OUTPUT))
    {
        nError = setOutput();
    }
    else if((m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_USE_ON) || (m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_USE_OFF))
    {
        // by zzang9un 2013.08.18 : Use 변수 설정을 위해 TransControl 함수를 호출함
        nError = m_pTransControl->ChangeUse(m_UseIndex, m_bUseOnOff);

        if(nError == NO_ERR)
        {
            m_StatusSet.bIsOnProcess = false;
            m_StatusSet.byStatus = STS_COMPLETE;
            m_StatusSet.uError == NO_ERR;
        }
        else
        {
            m_StatusSet.bIsOnProcess = false;
            m_StatusSet.byStatus = STS_ERROR;
            m_StatusSet.uError == nError;
        }
    }
    
   ///@todo Rotate 추가 시 구현 필요
   /*
#if(IS_ROTATE_EXIST==1)
   else if(m_TPCmd==TPCMD_CMCONLINECMD)
   {
      TP_OnlineCommand();
   }
#endif
*/


   ///@todo MainThread에서 구현 필요
   /*
   else if((m_TPCmd==TPCMD_ASSISTANT_CLOSE)||(m_TPCmd==TPCMD_ASSISTANT_EXEC))
   {
      TP_FileControl();
   }
 */

   ///@todo MainThread에서 ExecuteThread에 줄 때 Trans 명령으로 줌
   /*
   else if(Check_Trans_CMD(m_TPCmd)==NO_ERR)
   {
         switch(m_TPCmd)
         {
            //*** Load ***
            case TPCMD_MANUAL_LOAD_P0_R0 :
            case TPCMD_MANUAL_LOAD_P0_R1 :

            case TPCMD_MANUAL_LOAD_P1_R0 :
            case TPCMD_MANUAL_LOAD_P1_R1 :

            case TPCMD_MANUAL_UTB_LOAD_R0 :
            case TPCMD_MANUAL_UTB_LOAD_R1 :

      #if((OHTVER_PROJ != TPOHTVER10_PROJ)&&(OHTVER_PROJ!=MZOHTVER20_PROJ))
         #if(IS_STB_EXIST==1)
            case TPCMD_MANUAL_STB_L_LOAD_R0 :
            case TPCMD_MANUAL_STB_L_LOAD_R1 :
			case TPCMD_MANUAL_STB_R_LOAD_R0 :
            case TPCMD_MANUAL_STB_R_LOAD_R1 :
         #endif
      #else
            case TPCMD_MANUAL_EQ_X_LOAD_P0_R0 :
            case TPCMD_MANUAL_EQ_X_LOAD_P1_R0 :
      #endif

            //*** Unload ***
            case TPCMD_MANUAL_UNLOAD_P0_R0 :

            case TPCMD_MANUAL_UNLOAD_P1_R0 :

            case TPCMD_MANUAL_UTB_UNLOAD_R0 :
            case TPCMD_MANUAL_UTB_UNLOAD_R1 :
            case TPCMD_MANUAL_SCAN_UTB :

      #if((OHTVER_PROJ != TPOHTVER10_PROJ)&&(OHTVER_PROJ!=MZOHTVER20_PROJ))
         #if(IS_STB_EXIST==1)
            case TPCMD_MANUAL_STB_L_UNLOAD_R0 :
            case TPCMD_MANUAL_STB_L_UNLOAD_R1 :
            case TPCMD_MANUAL_STB_R_UNLOAD_R0 :
            case TPCMD_MANUAL_STB_R_UNLOAD_R1 :
            case TPCMD_MANUAL_SCAN_STB_L :
            case TPCMD_MANUAL_SCAN_STB_R :
         #endif
      #else
            case TPCMD_MANUAL_EQ_X_UNLOAD_P0_R0 :
            case TPCMD_MANUAL_EQ_X_UNLOAD_P1_R0 :
      #endif
               if(Is_Motion_Start())
               {
                  TP_Trans();
               }
               break;

            //*** Move T-Point ***
            case TPCMD_MOVE_TEACHPOINT:
            case TPCMD_MOVE_23AXIS_TEACHPOINT:
			case TPCMD_MOVE_UTB_TEACHPOINT:
            case TPCMD_MOVE_UTB_23AXIS_TEACHPOINT:
      #if((OHTVER_PROJ != TPOHTVER10_PROJ)&&(OHTVER_PROJ!=MZOHTVER20_PROJ))
         #if(IS_STB_EXIST==1)
            case TPCMD_MOVE_STB_L_TEACHPOINT:
            case TPCMD_MOVE_STB_L_23AXIS_TEACHPOINT:
            case TPCMD_MOVE_STB_R_TEACHPOINT:
            case TPCMD_MOVE_STB_R_23AXIS_TEACHPOINT:
         #endif
      #else
            case TPCMD_MOVE_EQ_X_TEACHPOINT:
            case TPCMD_MOVE_EQ_X_23AXIS_TEACHPOINT:
      #endif
               if(Is_Motion_Start())
               {
                  TP_Move_TeachingPoint();
               }
               break;
         }
   }
 */

   ///@todo MainThread에서 구현 필요
   /*
   else if((m_TPCmd>=144)&&(m_TPCmd<=169))
   {
      TP_FileControl();
   }
   */

    else if ((m_ManualCmd.uCmdTag.Info.CmdData >= TPCMD_HOIST_ORIGIN_SETTING) && (m_ManualCmd.uCmdTag.Info.CmdData <= TPCMD_SHIFT_SETLIMIT_POSI))
    {
        nError = SetOriginAndLimit();
    }

    /*
    ///@todo 상위 Thread에서 구현 필요
    else if ((m_TPCmd >= TPCMD_MAP_USE_BCR) && (m_TPCmd <= TPCMD_MAP_USE_OBSIGNORESOUND))
    {
		TP_Para_UseData_Change();
    }
    */
    else if ((m_ManualCmd.uCmdTag.Info.CmdData >= TPCMD_JOG_DRIVING_FORWARD) && (m_ManualCmd.uCmdTag.Info.CmdData <= TPCMD_JOG_SHIFTROTATE_HOME)&&
                m_ManualCmd.uCmdTag.Info.CmdData != TPCMD_JOG_DRIVING_STOP        &&
                m_ManualCmd.uCmdTag.Info.CmdData != TPCMD_JOG_DRIVING_FORWARD     &&
                m_ManualCmd.uCmdTag.Info.CmdData != TPCMD_JOG_DRIVING_BACKWARD    &&
                m_ManualCmd.uCmdTag.Info.CmdData != TPCMD_JOG_DRIVING_DIST                        )
    {
        nError = JogCommand();
	}
	else if(m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_SET_RFPIO_CHANNEL)
	{
       nError = ControlRFPIOSet();
    }
	else if ((m_ManualCmd.uCmdTag.Info.CmdData >= TPCMD_IO_TPCMD_START) && (m_ManualCmd.uCmdTag.Info.CmdData <= IO_TPCMD_END))
	{
 		nError = ControlHW();
		ADD_LOG("[UpdateAMCDownloadFile]ControlHW [Error:%d]", nError);
    }
    else if (((m_ManualCmd.uCmdTag.Info.CmdData >= TPCMD_GO_SETCURNODE) && (m_ManualCmd.uCmdTag.Info.CmdData <= TPCMD_GO_QR_RIGHT_MARK_FORWARD))||
                m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_JOG_DRIVING_STOP        ||
                m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_JOG_DRIVING_FORWARD     ||
                m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_JOG_DRIVING_BACKWARD    ||
                m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_JOG_DRIVING_DIST                        )
    {
        nError = ControlDriving();
//        nError = TP_GoCommand();
	}
	//2014.02.10 ji7412.lee : AMC.out Update를 위한 처리
	else if(m_ManualCmd.uCmdTag.Info.CmdData == TPCMD_DOWN_AMCFILE)
	{
		nError = UpdateAMCDownloadFile();
		ADD_LOG("[UpdateAMCDownloadFile]UpdateComplete [Error:%d]", nError);
	}

    return nError;
}

/**
@brief   대기 중 이상 상태 확인 
@author  ehoto
@date    2013.02.07
@return  문제 있을 경우 에러 반환, 그 외에는 NO_ERR(0) 반환
*/
//
UINT ManualControl::CheckAbnormalOnWait(void)
{
//	m_pVirtualDrivingControl->Manual_DrivingAbnormalCheck();
		 //Slope에서 모터관련 에러일 경우 브레이크 동작

    return NO_ERR;
}


/**
@brief   대기 중 이상 상태 확인 
@author  ehoto
@date    2013.02.07
@return  문제 있을 경우 에러 반환, 그 외에는 NO_ERR(0) 반환
*/
//
UINT ManualControl::CheckAlways(void)
{
	int nError = NO_ERR;
	static int nOnEMOCount = 0;
	static int nBumperPressCount = 0;
	static int nFanFailCount = 0;
	static int nSMPSFailCount = 0;
	
#if ((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V81S) || (OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	static int nHoistBeltDetectCount = 0;
#endif

	if((m_pExecuteInfo->ExecuteMode == EXECUTE_MODE_MANUAL) && (!m_pExecuteInfo->bJogInterlockIgnore))
	{
		return NO_ERR;
	}



	//1 1. Emergency 확인
	if(m_pHWSet->m_pPowerControl->IsOnEMO() == false)
	{
		nOnEMOCount++;
		if(nOnEMOCount > 3)
		{
			ADD_LOG("[ERR_ESTOP_PRESS] IsOnEMO : %d, nOnEMOCount : %d", m_pHWSet->m_pPowerControl->IsOnEMO(), nOnEMOCount);
            ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_ESTOP_PRESS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
			nOnEMOCount = 0;
			return ERR_ESTOP_PRESS;
		}
	}		
	else
	{
		nOnEMOCount = 0;
	}
	//2018.10.11 Bumper Press Error 발생 시 대차 감지 유무에 따라 센서 이상 or 실제 추돌 여부 확인 로그 추가 #Jrespect.im
	if(m_pHWSet->m_pBumper->IsBumperPress() == true)
	{

		nBumperPressCount++;
		if(nBumperPressCount > 3)
		{
			if((m_pHWCommon->m_Input.OHTDETECT_Long == ON) && (m_pHWCommon->m_Input.OHTDETECT_Middle == ON) && (m_pHWCommon->m_Input.OHTDETECT_Short == ON))
			{
				ADD_LOG("[E464] : No Detect, IsBumper : %d, nCount : %d", m_pHWSet->m_pBumper->IsBumperPress(), nBumperPressCount);
				nBumperPressCount = 0;
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_BUMPER_PRESS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
				return ERR_BUMPER_PRESS;
			}
			else
			{
				ADD_LOG("[E464] : Detect, IsBumper : %d, nCount : %d", m_pHWSet->m_pBumper->IsBumperPress(), nBumperPressCount);
				nBumperPressCount = 0;
				ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
					ERR_BUMPER_PRESS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
				return ERR_BUMPER_PRESS;
			}
		}
	}
	else
	{
		nBumperPressCount = 0;
	}

	//1 2. FAN Fail 확인
	if(m_pHWSet->m_pFan->IsFanFail() == true)
	{
		nFanFailCount++;
		if(nFanFailCount > 3)
		{
			ADD_LOG("[ERR_FAN_FAIL] IsFanFail : %d, nFanFailCount : %d", m_pHWSet->m_pFan->IsFanFail(), nFanFailCount);								
			
			nFanFailCount = 0;
			return ERR_FAN_FAIL;
		}
	}
	else
	{
		nFanFailCount = 0;
	}

#if( (OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V81S)|| (OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
//	if(m_pHWSet->m_pHoistAxis->IsHoistBeltDetect() == true)
//	{
//		nHoistBeltDetectCount++;
//		if(nHoistBeltDetectCount > 3)
//		{
//			ADD_LOG("[ERR_HOIST_BELT_CUT] IsHoistBeltDetect : %d, nHoistBeltDetectCount : %d", m_pHWSet->m_pHoistAxis->IsHoistBeltDetect(), nHoistBeltDetectCount);
//
//			nHoistBeltDetectCount = 0;
//			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
//						ERR_HOIST_BELT_CUT,
//						m_pOHTMainStatus->StatusCommon.CurNodeID,
//						m_pExecuteInfo->ExecutePositionInfo.NextNode,
//						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//						m_pOHTMainStatus->StatusCommon.ExistFoup,
//						m_pOHTMainStatus->StatusCommon.StopStationID);
//			return ERR_HOIST_BELT_CUT;
//		}
//	}
//	else
//	{
//		nHoistBeltDetectCount = 0;
//	}
#endif



#if(((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V81S)|| (OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE)) && (SAMSUNG_COMMON_SPEC==ON))
	// SMPS Fault 확인
	if(m_pHWSet->m_pPowerControl->IsOnSMPSFault() == true)
	{
		nSMPSFailCount++;
		if(nSMPSFailCount > 3)
		{
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_SMPS_FAULT_ON,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			return ERR_SMPS_FAULT_ON;
        }

	}else
	{
		nSMPSFailCount = 0;
	}


#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) ||(OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
    // EUV POD는 회전 동작(Origin, Move) 시 간섭으로 Shift 10mm 안쪽 이동 후 동작이 필요.
    if(nError==NO_ERR)   nError = OHTMainForm->GetTransControl()->Seq_EuvRotate_Origin();
    if(nError==NO_ERR)   nError = OHTMainForm->GetTransControl()->Seq_EuvRotate_Move();
#endif

#endif



	return nError;
}



bool ManualControl::DeleteCmd(void *pCmd)
{
    bool bReturn = true;
    try
    {
        EXECUTE_MANUALCONTROL_INFO *sTmpManualControlInfo=(EXECUTE_MANUALCONTROL_INFO *)pCmd;
        delete sTmpManualControlInfo;
    }
    catch(...)
    {
        bReturn = false;
    }
    return bReturn;
}

/**
@brief   AMCDownloadFile Update기능
@author  ji7412.lee
@date    2014.02.10
@note    v6.5 기능함수를 통합(two Function->one Function)
*/
int ManualControl::UpdateAMCDownloadFile()
{

	//2014.06.09 ji7412.lee TP상태전송을 위해 추가 0x00대기, 0x01진행, 0x02완료, 0x03실패
	m_pHWSet->m_AMCResponse = 0x01;

	int nError = 0, nRtnValue = 0;
	AnsiString sDownFileName, sFullName, sLogMessage, sOHTPath, scoffbootPath;

	sOHTPath = ExtractFileDir(Application->ExeName);
	sDownFileName = sOHTPath + "\\DataFiles\\amc.out";      //Temp Path
	scoffbootPath = sOHTPath + "\\DataFiles\\coffboot.bin"; //Temp Path

	ADD_LOG("[UpdateAMCDownloadFile]AMC Upload Start");

	if(sDownFileName.Length() <= 0)
	{
		ADD_LOG("[UpdateAMCDownloadFile][%s] Error[Length<=0] Upload Fail",ExtractFileName(sDownFileName));
		nError = 1;
	}
	else if(!FileExists(sDownFileName))
	{
		ADD_LOG("[UpdateAMCDownloadFile][%s] Error[File Not Exist] Upload Fail",ExtractFileName(sDownFileName));
		nError = 2;
	}

	for(int i=0; i <4; i++)
	{
		m_pHWCommon->SetAMPEnable(i,false);
	}

#if 0
	Sleep(50);
	nError = m_pHWCommon->AMCFileSystemForamt();
	Sleep(50);
#else
	Sleep(100);
#endif

	nError = m_pHWCommon->SetAMCDriverOpenByCommon(false);
	nError = m_pHWCommon->ClearWatchDogStatus();
	nError = m_pHWCommon->DisableWatchDog();

	if(nError == NO_ERR) nError = m_pHWCommon->AMCFileSystemInstall(scoffbootPath.c_str(), sDownFileName.c_str(), &nRtnValue, 0, WM_DSP_MSGID);

	if(nRtnValue != NO_ERR)
	{
		ADD_LOG("[UpdateAMCDownloadFile]Flash Error[%d]", nRtnValue);
		if(nRtnValue <= -4 && nError == 0) nError = -1;
	}
	if(nError || nRtnValue)
		nError = m_pHWCommon->SetAMCDriverOpenByCommon(true);

	Sleep(50);
	if(nError == NO_ERR) nError = m_pHWCommon->DSP_Reboot();
	Sleep(5000);

	if(nError == NO_ERR)
	{
		ADD_LOG("[UpdateAMCDownloadFile][%s]:Upload Sucesses",ExtractFileName(sDownFileName));
		// TP상태전송을 위해 추가 0x00대기, 0x01진행, 0x02완료, 0x03실패
		m_pHWSet->m_AMCResponse = 0x02;
	}
	else
	{
		ADD_LOG("[UpdateAMCDownloadFile][%s]:Error[%d]:Upload Fail",ExtractFileName(sDownFileName), nError);
		// TP상태전송을 위해 추가 0x00대기, 0x01진행, 0x02완료, 0x03실패
		m_pHWSet->m_AMCResponse = 0x03;
	}

	return nError;
}

int ManualControl::SetOnOff(int m_UseIndex, int m_bUseOnOff)
{
	int nError = m_pTransControl->ChangeUse(m_UseIndex, m_bUseOnOff);

	if(nError == NO_ERR)
	{
		m_StatusSet.bIsOnProcess = false;
		m_StatusSet.byStatus = STS_COMPLETE;
		m_StatusSet.uError == NO_ERR;
	}
	else
	{
		m_StatusSet.bIsOnProcess = false;
		m_StatusSet.byStatus = STS_ERROR;
		m_StatusSet.uError == nError;
	}

	return nError;
}

int ManualControl::SetOutBit(int m_UseIndex, int m_bUseTrueFalse)
{
	int nError = m_pHWCommon->SetBit(m_UseIndex, m_bUseTrueFalse);

	return nError;
}

void ManualControl::SendCidMonitor(void)
{
   m_pHWSet->m_pCID->SendMonitoringCmd_Clear();
}

void ManualControl::SoundOn(int iSoundNum)
{
	switch (iSoundNum)
	{
	case 1:
		m_pHWSet->Sound_On(SETSOUND_ERROR);
		break;
	case 2:
		m_pHWSet->Sound_On(SETSOUND_DRIVING);
		break;
	case 3:
		m_pHWSet->Sound_On(SETSOUND_TRANS);
		break;
	case 4:
		m_pHWSet->Sound_On(SETSOUND_OBS);
		break;
	}
}

void ManualControl::SoundOff()
{
	m_pHWSet->Sound_Off();
}


void ManualControl::PowerOnOff(int iAxis, int iOnOff)
{
	int nError;
	
	if(iAxis == AXIS_HAND)
	{
		switch(iOnOff)
		{
			case PWR_ON:
				nError = m_pHWSet->m_pHand->HandPowerControl(PWR_ON);
			break;

			case PWR_OFF:
				nError = m_pHWSet->m_pHand->HandPowerControl(PWR_OFF);
			break;

			case PWR_RST:
				nError = m_pHWSet->m_pHand->HandPowerControl(PWR_RST);
			break;
		}
	}
	else if(iAxis == AXIS_FRONT)
	{
		switch(iOnOff)
		{
			case PWR_ON:
				nError = m_pHWSet->m_pSteer->FrontEnable();
			break;

			case PWR_OFF:
				nError = m_pHWSet->m_pSteer->FrontFree();
			break;
		}
	}
	else if(iAxis == AXIS_REAR)
	{
		switch(iOnOff)
		{
			case PWR_ON:
				nError = m_pHWSet->m_pSteer->RearEnable();
			break;

			case PWR_OFF:
				nError = m_pHWSet->m_pSteer->RearFree();
			break;
		}
	}
}

void ManualControl::MarkTry(void)
{
	if(m_pHWCommon->m_Input.STOP_Driving_Front == true)
	{
	   ADD_LOG("[Param Apply] Driving Mark Try...");
			
	   if(m_StatusSet.uError == NO_ERR)
	   {
	   	 ADD_LOG("[Param Apply] MANUAL_MARK_CMD_NODE");
		 m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_NODE);
	   }
	   else
       {
		 ADD_LOG("[Param Apply] Err. Mark Not Try");
	   }
	}
	else if((m_pHWCommon->m_Input.STOP_Trans_Front == true) && (m_pHWCommon->m_Input.STOP_Trans_Rear == true))
	{
	   if(m_StatusSet.uError == NO_ERR)
	   {
	     ADD_LOG("[Param Apply] MANUAL_MARK_CMD_STATION_STOP");
		 m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_STATION_STOP);
	   }
	   else
       {
		 ADD_LOG("[Param Apply] Err. Mark Not Try");
       }
	}
}

void ManualControl::MoveByPulse(double dblPulse, double dblMoveSpeed)
{
	int nError = 0;
	double dVel_mmpermsec = 0.0;

	// ShowMessage("ManualControl - MoveByPulse");

	dVel_mmpermsec = MANUAL_HOIST_MAX_SPEED * MANUAL_MULTIPLY_2_6P * dblMoveSpeed;

	nError = m_pHWSet->m_pHoistAxis->MoveDistance_Pulse
		((DWORD)MANUAL_HOIST_ACCTIME_500, (DWORD)MANUAL_HOIST_ACCTIME_500, dVel_mmpermsec, dblPulse);
}

