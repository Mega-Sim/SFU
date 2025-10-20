//---------------------------------------------------------------------------


#pragma hdrstop

#include "HandRMR.h"
#include "DiagManager.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
#if((OHT_NAME == OHT_NAME_P4_MASK) || (OHT_NAME == OHT_NAME_MASK_JIG))

/**
@file    HandRMR.cpp
@version OHT 7.0
@brief   HandRMR Class CPP File
*/

#include "HandRMR.h"

#define ORIGIN_POSE  0
#define HOME_POSE  1
#define WORK_POSE  2

#define MANUAL_M 0
#define AUTO_M	1

/**
@brief   HandRMR Class 생성자
@author  mgchoi
@date    2022.08.22
@param   pHWCommon : HWCommon Class의 포인터
*/
HandRMR::HandRMR(HWCommon * pHWCommon)
{
	m_pHWCommon = pHWCommon;
	m_pShutterC	= new Shutter(m_pHWCommon);
	m_pHoistC = new Hoist(m_pHWCommon, 2);
	Init();

	InitializeCriticalSection(&m_MovCntCS);
}

/**
@brief   HandRMR Class 소멸자
@author  mgchoi
@date    2022.08.22
*/
HandRMR::~HandRMR()
{;
	DeleteCriticalSection(&m_MovCntCS);
}

/**
@brief   HandRMR Class에서 사용하는 변수 초기화
@author  mgchoi
@date    2022.08.22
*/
void HandRMR::Init()
{
    bReadyOperation = false;
	nSendCommand = HANDRMR_NO_COMMAND;

	dwOriginLimitTime = HANDRMR_ORIGIN_LIMIT_TIME;
	dwMotionLimitTime = HANDRMR_MOTION_LIMIT_TIME;

    bLongRun = false;
    m_nMovingCount = 0;
	dwMaxMovingTime = 0;
	nCount_HandReset = 0;
    bHomingState = false;
	dwStartTime = 0;
}

/**
@brief   HandRMR 부 출력을 모두 OFF 상태로 초기화시키는 함수
@author  mgchoi
@date    2022.08.22
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int HandRMR::IO_Init()
{
    int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_HAND_WORK, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_WORK_INTERLOCK, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_HOME, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_HOME_INTERLOCK, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_ORIGIN, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_RESET, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_FREE, OFF);
	return nError;
}

/**
@brief   HandRMR을 Manual Mode로 Work위치로 이동하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR가 문제 없이 WORK 되면 0, 에러 발생 시 에러 코드 리턴.
*/
int HandRMR::ManualWork(int withIL)
{
    int nError = NO_ERR;
	int nExec = 0;

    if (bReadyOperation == false)
		nError = ERR_HANDRMR_NOTORIGIN;

	if(nError == NO_ERR)   // RMR이 이동불가 상태 체크
		nError = IsHandRMRMoveAvailableState(MANUAL_M);

	if (nError == NO_ERR)  // RMR 가동 상태 만듦
		nError = Enable();

	if (nError == NO_ERR)
	{
		nError = IO_Init();
		if (nError == NO_ERR)
			nError = AlarmReset();
	}
	if (nError == NO_ERR)
	{
		if(withIL == 0)  // WORK
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_HAND_WORK, ON);
			if (nError)
				nError = ERR_SEND_HANDRMR_WORK_COMMAND;
			else
			{
				dwStartTime = timeGetTime();
				nSendCommand = HANDRMR_WORK_COMMAND;
				while ((nError = CheckComplete()) == NEXT_CHECK)
				{
					Sleep(10);
				}
			}
		}
		else    // WORK_INTERLOCK
		{
            // 2016-11-14. Add by Labeler
            // Work_IL 명령 발생 시, Flange Sensor Check 수행 토록 추가
            if(IsFlangeCheck() == false)
                return ERR_SEND_HANDRMR_WORK_IL_COMMAND;
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_HAND_WORK_INTERLOCK, ON);
			if (nError)
				nError = ERR_SEND_HANDRMR_WORK_IL_COMMAND;
			else
			{
				dwStartTime = timeGetTime();
				nSendCommand = HANDRMR_WORK_IL_COMMAND;
				while ((nError = CheckComplete()) == NEXT_CHECK)
				{
					Sleep(10);
				}
			}
		}
	}
    return nError;
}

/**
@brief   HandRMR을 Auto Mode로 이적재 시 Work위치로 이동하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR이 문제 없이 이동되면 0, 에러 발생 시 에러 코드 리턴.
*/
int HandRMR::AutoWork(int withIL)
{
	int nError = NO_ERR;

    if (bReadyOperation == false)
		nError = ERR_HANDRMR_NOTORIGIN;

	if(nError == NO_ERR)   // RMR이 이동불가 상태 체크
		nError = IsHandRMRMoveAvailableState(AUTO_M);

	if (nError == NO_ERR)
		nError = Enable();

	if (nError == NO_ERR)
		nError = IO_Init();

   // alarm check
	if (IsAlarm() == true)
    {
		nError = Origin();
		nCount_HandReset++;
		//Sleep(200);
		Sleep(3500); // E1335 다발로 인해 시간 늘리기
	}
   // limit sensor check
	if (nError == NO_ERR)
	{
		if (IsWork() == true)
		{
			if (IsHome() == true)
				nError = ERR_HAND_LIMIT_SEN_ALL_ON;    // 기존 에러 이용
			else
				return nError;
		}
		else
		{
			if (IsHome() == false)
				nError = ERR_HAND_LIMIT_SEN_ALL_OFF;
			else       // normal case : Home에 있을때
			{
				if(withIL == 0)
				{
					nError = m_pHWCommon->SetBit(OUT_HAND_WORK, ON);
					if (nError)
						nError = ERR_SEND_HANDRMR_WORK_COMMAND;
				}
				else
				{
                    // 2016-11-14. Add by Labeler
                    // Work_IL 명령 발생 시, Flange Sensor Check 수행 토록 추가
                    if(IsFlangeCheck() == false)
                        return ERR_SEND_HANDRMR_WORK_IL_COMMAND;

					nError = m_pHWCommon->SetBit(OUT_HAND_WORK_INTERLOCK, ON);
					if (nError)
						nError = ERR_SEND_HANDRMR_WORK_IL_COMMAND;
				}

				if (nError == NO_ERR)
					m_nMovingCount++;
			}
		}
	}
	if (nError == NO_ERR)
	{
		dwStartTime = timeGetTime();
		if(withIL == 0)
			nSendCommand = HANDRMR_WORK_COMMAND;
		else
			nSendCommand = HANDRMR_WORK_IL_COMMAND;

		bool bIsTimeChecked = false;

		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			//진단모니터링 항목(Hand move timeover)
			int nMoveTimeOver = DiagManager::getInstance()->GetDiagParam().nRefAbnormTimeoverMoveHand;
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= nMoveTimeOver && bIsTimeChecked == false)
			{
				DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_HAND);

				bIsTimeChecked = true;
			}

			Sleep(10);
		}
	}
	return nError;
}

/**
@brief   HandRMR를 Manual Mode로 Home 하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR이 문제 없이 Home 되면 0, 에러 발생 시 에러 코드 리턴.
*/
int HandRMR::ManualHome(int withIL)
{
	int nError = NO_ERR;
	if (bReadyOperation == false)
		nError = ERR_HANDRMR_NOTORIGIN;

	if(nError == NO_ERR)   // RMR이 이동불가 상태 체크
		nError = IsHandRMRMoveAvailableState(MANUAL_M);

	if (nError == NO_ERR)
		nError = Enable();

	if (nError == NO_ERR)
		nError = IO_Init();

	if (nError == NO_ERR)
		nError = AlarmReset();

	if (nError == NO_ERR)
	{
		if(withIL==0)
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_HAND_HOME, ON);

			if (nError)
			nError = ERR_SEND_HANDRMR_HOME_COMMAND;
			else
			{
				dwStartTime = timeGetTime();
				nSendCommand = HANDRMR_HOME_COMMAND;
				while ((nError = CheckComplete()) == NEXT_CHECK)
				{
					Sleep(10);
				}
			}
		}
		else
		{
		   // 2016-11-14. Add by Labeler
		   // Work_IL 명령 발생 시, Flange Sensor Check 수행 토록 추가
		  if(IsFlangeCheck() == false)
			  return ERR_SEND_HANDRMR_HOME_IL_COMMAND;

			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_HAND_HOME_INTERLOCK, ON);

			if (nError)
				nError = ERR_SEND_HANDRMR_HOME_IL_COMMAND;
			else
			{
				dwStartTime = timeGetTime();
				nSendCommand = HANDRMR_HOME_IL_COMMAND;
				while ((nError = CheckComplete()) == NEXT_CHECK)
				{
					Sleep(10);
				}
			}
		}
	}
    return nError;
}

/**
@brief   HandRMR를 Auto Mode로 이적재 시 Home 하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR이 문제 없이 Home 되면 0, 에러 발생 시 에러 코드 리턴.
*/
int HandRMR::AutoHome(int withIL)
{
	int nError = NO_ERR;
	if (bReadyOperation == false)
		nError = ERR_HANDRMR_NOTORIGIN;

	if(nError == NO_ERR)   // RMR이 이동불가 상태 체크
		nError = IsHandRMRMoveAvailableState(AUTO_M);

	if (nError == NO_ERR)
		nError = Enable();

	if (nError == NO_ERR)
		nError = IO_Init();

   // alarm check
    if (IsAlarm() == true)
	{
		nError = Origin();
		nCount_HandReset++;
		//Sleep(200);
		Sleep(3500); // E1335 다발로 인해 시간 늘리기
    }

	if (nError == NO_ERR)
	{
		if (IsHome() == true)
		{
			if (IsWork() == true)
				nError = ERR_HAND_LIMIT_SEN_ALL_ON;
			else
				return nError;
		}
		else
		{
			if (IsWork() == false)
				nError = ERR_HAND_LIMIT_SEN_ALL_OFF;
			else     // Normal Case
			{
				if(withIL==0)
				{
					nError = m_pHWCommon->SetBit(OUT_HAND_HOME, ON);
					if (nError)
						nError = ERR_SEND_HANDRMR_HOME_COMMAND;
				}
				else
				{
                    // 2016-11-14. Add by Labeler
                    // Work_IL 명령 발생 시, Flange Sensor Check 수행 토록 추가
                    if(IsFlangeCheck() == false)
                        return ERR_SEND_HANDRMR_HOME_IL_COMMAND;

					nError = m_pHWCommon->SetBit(OUT_HAND_HOME_INTERLOCK, ON);
					if (nError)
						nError = ERR_SEND_HANDRMR_HOME_IL_COMMAND;
				}

				if (nError == NO_ERR)
					m_nMovingCount++;
			}
		}
	}
	if (nError == NO_ERR)
	{
		dwStartTime = timeGetTime();
		if(withIL==0)
			nSendCommand = HANDRMR_HOME_COMMAND;
		else
			nSendCommand = HANDRMR_HOME_IL_COMMAND;

		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
            Sleep(10);
        }
    }
    return nError;
}

/**
@brief   HandRMR 모터를 원점 복귀시키는 함수
@author  mgchoi
@date    2022.08.22
@return  동작에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int HandRMR::Origin()
{
	int nError = NO_ERR;

	bReadyOperation = false;
	bHomingState = true;
	if (nError == NO_ERR)
		nError = Enable();

	if(nError == NO_ERR)   // RMR이 이동불가 상태 체크
		nError = IsHandRMRMoveAvailableState(MANUAL_M);  // orgin

	if (nError == NO_ERR)
		nError = IO_Init();

	if (nError == NO_ERR)
		nError = AlarmReset();

	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_ORIGIN, ON);

	Sleep(500);

	if (nError)
		nError = ERR_SEND_HAND_ORIGIN_COMMAND;
	else
    {
		dwStartTime = timeGetTime();
		nSendCommand = HANDRMR_ORIGIN_COMMAND;
        while ((nError = CheckComplete()) == NEXT_CHECK)
        {
            Sleep(10);
		}
		if (nError == NO_ERR && bReadyOperation == false)
            bReadyOperation = true;
	}
    bHomingState = false;
    return nError;
}

/**
@brief   HandRMR이 알람 상태이면 알람을 리셋하는 함수
@author  mgchoi
@date    2022.08.22
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int HandRMR::AlarmReset()
{
	int nError = NO_ERR;

	if(IsAlarm() == true)
	{
		//진단모니터링 항목(Hand Alarm reset)
		DiagManager* pDiagManager = DiagManager::getInstance();

		pDiagManager->IncreaseAbnormalHandResetCount();

		if(pDiagManager->GetAbnormalHandResetCount() > pDiagManager->GetDiagParam().nRefAbnormAlarmResetHand)
		{
			DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_ALARM_RESET_HAND);
			pDiagManager->ResetAbnormalHandResetCount();
		}

		nError = m_pHWCommon->SetBit(OUT_HAND_RESET, ON);
		if (nError == NO_ERR)
		{
			Sleep(500);
			nError = m_pHWCommon->SetBit(OUT_HAND_RESET, OFF);
		}
		Sleep(50);
	}
    return nError;
}

/**
@brief   HandRMR 모터를 Enable 상태로 만드는 함수
@author  mgchoi
@date    2022.08.22
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int HandRMR::Enable()
{
	int nError = NO_ERR;
	if (IsFree() == true)
		nError = m_pHWCommon->SetBit(OUT_HAND_FREE, OFF);

	return nError;
}

/**
@brief   Hand RMR 모터를 Free 상태로 만드는 함수
@author  mgchoi
@date    2022.08.22
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int HandRMR::Free()
{
    int nError = NO_ERR;
    bReadyOperation = false;

	nError = m_pHWCommon->SetBit(OUT_HAND_FREE, ON);

	return nError;
}

/**
@brief   출력 명령에 맞는 동작을 제한 동작 시간 내에 완료했는지 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  문제 없이 동작 시 0, 문제 발생시 해당 에러 코드 리턴.
*/
int HandRMR::CheckComplete()
{
	DWORD dwHandrmrMovingTime = 0;

	if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= ((nSendCommand == HANDRMR_ORIGIN_COMMAND) ?
		dwOriginLimitTime : dwMotionLimitTime))
	{
		switch (nSendCommand)
        {
		case HANDRMR_WORK_COMMAND:
			IO_Init();
			return ERR_HANDRMR_WORK_TIMEOVER;

		case HANDRMR_WORK_IL_COMMAND:
			IO_Init();
			return ERR_HANDRMR_WORK_IL_TIMEOVER;

		case HANDRMR_HOME_COMMAND:
			IO_Init();
			return ERR_HANDRMR_HOME_TIMEOVER;

		case HANDRMR_HOME_IL_COMMAND:
			IO_Init();
			return ERR_HANDRMR_HOME_IL_TIMEOVER;

		case HANDRMR_ORIGIN_COMMAND:
            IO_Init();
			return ERR_HAND_ORIGIN_TIMEOVER;           // 기존 Hand Error 사용
        }
    }
	if (IsAlarm() == true)
	{
        IO_Init();
		return ERR_HAND_ALARM;            // 기존 Hand Error 사용
	}

	switch (nSendCommand)
	{
		case HANDRMR_WORK_COMMAND:
		case HANDRMR_WORK_IL_COMMAND:
			if ((IsWork() == true) && (IsInposition() == true))
			{
				dwHandrmrMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
				dwStartTime = 0;
				if (dwMaxMovingTime < dwHandrmrMovingTime)
					dwMaxMovingTime = dwHandrmrMovingTime;

				// 2016-11-15
				Sleep(500);
				IO_Init();
				return NO_ERR;
			}
			break;
		case HANDRMR_HOME_COMMAND:
		case HANDRMR_HOME_IL_COMMAND:
			if ((IsHome() == true) && (IsInposition() == true))
			{
				dwHandrmrMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
				dwStartTime = 0;
				if (dwMaxMovingTime < dwHandrmrMovingTime)
					dwMaxMovingTime = dwHandrmrMovingTime;
				// 2016-11-15
				Sleep(500);
				IO_Init();
				return NO_ERR;
			}
			break;
		case HANDRMR_ORIGIN_COMMAND:
			if ((IsHome() == true) && (IsInposition() == true))
			{
				IO_Init();
				return NO_ERR;
			}
			break;
	}
	return NEXT_CHECK;
}

/**
@brief   Hand 위치가 RMR을 움직일 수 있는지 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  Hand가 Antidrop위에 놓여 있거나, POD 감지센서나 Placement 센서가 감지되거나, POD를 들었는데 unload위치, Antidrop가 Close이면 이동 불가.
*/
int HandRMR::IsHandRMRMoveAvailableState(int mode)              // selpose: 0 Origine, 1: Home, 2 Work
{
	int nError = NO_ERR;

	///AntiTemp..Pod임시 주석처리. mgchoi
	if((m_pShutterC->IsPodPlacement3On() == true) || (m_pShutterC->IsPodPlacement4On() == true))  //Placement센서 오감지된 경우
			nError = ERR_HANDRMR_NOTMOVE_PLACEMENT_SEN_ON;
	else if(((m_pShutterC->IsClose(FRONT) == true) || (m_pShutterC->IsOpen(FRONT) == false))
		&& (m_pHoistC->IsHoistHome() == false) && (mode == MANUAL_M))         // Manual일때는 Home에서는 Shutter가 Close되있어도 Hand움직임
			nError = ERR_HANDRMR_NOTMOVE_ANTIDROPNOTOPEN;
	else if(((m_pShutterC->IsClose(FRONT) == true) || (m_pShutterC->IsOpen(FRONT) == false))
		&& (mode == AUTO_M))         // Antidrop Close 위치인 경우
			nError = ERR_HANDRMR_NOTMOVE_ANTIDROPNOTOPEN;
	else if(IsLeftGripOn() != IsRightGripOn())
		nError = ERR_GRIP_SEN_MISMATCH;
	else if((IsUnloadPosition() == true) && ((IsLeftGripOn()== true) || (IsRightGripOn()== true)))  // POD를 들었는데 Unload위치
		nError = ERR_POD_NOTMOVE_IN_UNLOADPOS;
	else
		;

	return nError;
}

/**
@brief   HandRMR HAND_UnloadPosition 센서 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR HAND_UnloadPosition On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsLoadPosition()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_LoadPosition == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR HAND_UnloadPosition 센서 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR HAND_UnloadPosition On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsUnloadPosition()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_UnloadPosition == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR Work Limit 센서 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR Work Limit 센서가 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsWork()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Work == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR Home Limit 센서 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR Home Limit 센서가 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsHome()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Home == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR Alarm 신호 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR Alarm 신호가 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsAlarm()
{
    bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Alarm == ON)
		bResult = true;
	else
		bResult = false;

    return bResult;
}

/**
@brief   HandRMR Free 츨력 값 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR Free 츨력 값이 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsFree()
{
	bool bResult = false;

	if (m_pHWCommon->m_Output.HAND_Free == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR가 Origin 되어 동작을 할 준비가 되어 있는지 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR Ready Operation Flag를 리턴.
*/
bool HandRMR::IsReady()
{
    return bReadyOperation;
}

/**
@brief   HandRMR 모터 Inposition 신호 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR 모터 Inposition 신호가 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsInposition()
{
    bool bResult = false;

	if(m_pHWCommon->m_Input.HAND_Alarm == ON)
		bResult = false;
	else if (m_pHWCommon->m_Input.HAND_Work == ON || m_pHWCommon->m_Input.HAND_Home == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Flange Sensor 간섭 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  Flange 감지 상태가 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsFlangeCheck()
{
    bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Flange_Check == ON)
		bResult = true;
	else
		bResult = false;
	return bResult;
}

/**
@brief   Hand PIO Go 신호 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  Hand PIO Go 신호가 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsGoSigOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_PIO_Go == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR Left Gripper 센서 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR Left Gripper 센서가 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsLeftGripOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Left_Grip == ON)
		bResult = true;
	else
		bResult = false;

    return bResult;
}

/**
@brief   HandRMR Right Gripper 센서 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  HandRMR Right Gripper 센서가 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsRightGripOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Right_Grip == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Hand Push 센서 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  Hand Close 출력 신호가 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsWorkOutputOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Output.HAND_Work == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Hand Push 센서 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  Hand Close 출력 신호가 On이면 true, Off면 False 값 리턴
*/
bool HandRMR::IsHomeOutputOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Output.HAND_Home == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Hand가 Homing 중인 상태인지 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  Hand Homing State Flag 값 리턴
*/
bool HandRMR::IsHomingState()
{
    return bHomingState;
}

/**
@brief   Hand Long Run 상태인지 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  Hand Long Run Flag 값 리턴
*/
bool HandRMR::IsLongRun()
{
    return bLongRun;
}

/**
@brief   Hand Long Run을 시작하는 함수
@author  mgchoi
@date    2022.08.22
*/
void HandRMR::StartLongRun()
{
    if (IsLongRun() == false)
        bLongRun = true;
}

/**
@brief   Hand Long Run을 멈추는 함수
@author  mgchoi
@date    2022.08.22
*/
void HandRMR::StopLongRun()
{
    if (IsLongRun() == true)
        bLongRun = false;
}

/**
@brief   Hand Moving 횟수를 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  Hand Moving Count 리턴
*/
int HandRMR::GetMovingCount(bool bInit)
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

/**
@brief   Hand Max Moving Time을 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  Hand Max Moving Time 리턴
*/
DWORD HandRMR::GetMaxMovingTime()
{
    return dwMaxMovingTime;
}

/**
@brief   Hand 부 전원을 컨트롤 하는 함수
@author  mgchoi
@date    2022.08.22
@param   nCase(On/Off/Reset)을 선택할 수 있음.
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int HandRMR::HandPowerControl(int nMode)
{
    int nError = NO_ERR;

    switch (nMode)
    {
    case PWR_OFF:
        nError = m_pHWCommon->SetBit(OUT_HAND_POWER_RESET, ON);
        break;
    case PWR_ON:
        nError = m_pHWCommon->SetBit(OUT_HAND_POWER_RESET, OFF);
        break;
    case PWR_RST:
        nError = m_pHWCommon->SetBit(OUT_HAND_POWER_RESET, ON);
		if (nError == NO_ERR)
        {
            Sleep(300);
			nError = m_pHWCommon->SetBit(OUT_HAND_POWER_RESET, OFF);
        }
        break;
    }

    return nError;
}

/**
@brief   Hand 부 전원을 컨트롤 하는 함수
@author  mgchoi
@date    2022.08.22
@return  Power 출력 Return
*/
bool HandRMR::IsHandPowerOn()
{
	return !(m_pHWCommon->m_Output.HAND_Power_Reset);
}


/**
@brief   Hand 부가 동작할 준비가 되어 있는지 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  준비가 되어 있으면 0, 되어 있지 않으면 ERR_HAND_NOTORIGIN 리턴.
*/
int HandRMR::ReadyToOperation()
{
    int nError = NO_ERR;

    if (bReadyOperation == true)
        nError = NO_ERR;
    else
        nError = ERR_HAND_NOTORIGIN;
    return nError;
}

/**
@brief   Hand 부의 상태에 따라 Origin 명령을 주고 난 후 동작할 준비가 되어 있는지 확인하는 함수
@author  mgchoi
@date    2022.08.22
@return  준비가 되어 있으면 0, 되어 있지 않으면 ERR_HAND_NOTORIGIN 리턴.
*/
int HandRMR::AutoInitialize()
{
	int nError = 0;
	nError = IsHandRMRMoveAvailableState(AUTO_M);
	if (nError == NO_ERR)
    {
      // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile("FOUP Check : NO FOUP(Normal case) ===> Hand Origin Start...");
        nError = Origin();
        if (nError == NO_ERR)
        {
		 // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile("   ===> SUCCESS Hand Origin");
			bReadyOperation = true;
        }
        else
        {
			bReadyOperation = false;
		}
	}
	nError = ReadyToOperation();
	return nError;
}

unsigned int HandRMR::GetHandResetCount()
{
	return nCount_HandReset;
}

/**
@brief  ResetMovementData
@author  mgchoi
@date    2022.08.22
*/
void HandRMR::ResetMovementData()
{
	m_nMovingCount = 0;
	dwMaxMovingTime = 0;
	nCount_HandReset = 0;
}

bool HandRMR::IsOpen(int nLeftRight)
{
	return false;
}
bool HandRMR::IsClose(int nLeftRight)
{
	return false;
}
bool HandRMR::IsCloseOutputOn()
{
	return false;
}

bool HandRMR::IsOpenOutputOn()
{
	return false;
}
bool HandRMR::IsPushOn()
{
	return false;
}
#endif

