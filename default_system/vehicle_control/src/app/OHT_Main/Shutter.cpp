/**
@file    Shutter.cpp
@version OHT 7.0
@brief   Shutter Class CPP File
*/

#include "Shutter.h"
#include "DiagManager.h"
#include "OHTMain.h"


// ---------------------------------------------------------------------------
// 로그 작성 함수
#define MAX_BYTE_SIZE_OF_SHUTTER_LOG 	1024 * 10
#define ADD_LOG_SHUTTER(...)        WRITE_LOG_UNIT(pLogShutter, __VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)

// ---------------------------------------------------------------------------


/**
@brief   Shutter Class 생성자
@author  LSJ
@date    2012.11.01
@param   pHWCommon : HWCommon Class의 포인터
*/
Shutter::Shutter(HWCommon * pHWCommon)
{
	m_pHWCommon = pHWCommon;
#if(OHT_NAME == OHT_NAME_P4_MASK)
	m_pHoistC = new Hoist(m_pHWCommon, 2);
	m_pShiftC = new Shift(m_pHWCommon, 3, 1000.0000000);
#endif
	pLogShutter = getLogUnit("D:\\log\\Shutter.log", MAX_BYTE_SIZE_OF_SHUTTER_LOG);
	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	Init();
	InitializeCriticalSection(&m_MovCntCS);

	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
}

/**
@brief   Shutter Class 소멸자
@author  LSJ
@date    2012.11.01
*/
Shutter::~Shutter()
{
	DeleteCriticalSection(&m_MovCntCS);
}

/**
@brief   Shutter Class에서 사용하는 변수 초기화
@author  LSJ
@date    2012.11.01
*/
void Shutter::Init()
{
	bLongRun = false;
	m_nMovingCount = 0;
	dwMaxMovingTime = 0;
	bReadyOperation = false;
    nSendCommand = 0;
    dwStartTime = 0;
    dwOriginLimitTime = SHUTTER_PWRRST_LIMIT_TIME;
    dwMotionLimitTime = SHUTTER_MOTION_LIMIT_TIME;
	nAutoRecoveryClose = SHUTTER_AR_CLOSE_NONE;
	dwAutoRecoveryTime = timeGetTime();
}

/**
@brief   Shutter 부 출력을 모두 OFF 상태로 초기화시키는 함수
@author  LSJ
@date    2012.11.01
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::IO_Init()
{
    int nError = 0;
    nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_RESET, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_RESET, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_OPEN, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_OPEN, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_ORIGIN, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_ORIGIN, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_FREE, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_FREE, OFF);

// shkim. EUV의 경우 반만 닫음 I/O가 있어 이것도 처리해 줘야 한다 
#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, OFF);
#endif

    return nError;
}

/**
@brief   Shutter Unit을 Open 시키는 함수
@author  LSJ
@date    2012.11.01
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::Open()
{
    int nError = NO_ERR;
	
    if (bReadyOperation == false)
	{
        nError = ERR_SHUTTER_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHUTTER_NOTORIGIN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
	}
    if (nError == NO_ERR)
        nError = Enable();

	if (nError == NO_ERR)
	{
#if(OHT_NAME != OHT_NAME_P4_MASK)
		nError = CheckSensor(SHUTTER_NO_POS);
#else
		nError = CheckSensor(SHUTTER_NO_POS,1);	// 0 : Origin, 1 : Open, 2 : Close
#endif
	}

// 셔터 열때 EUV 센서 체크 인터락하는 로직 제거 - 센서 의도와 인터락 맞지 않음

    if (nError == NO_ERR)
    {
		if((bReadyOperation == true) && (IsOpen(BOTH) == true) && (IsInposition(BOTH) == true))
		{
			return	nError;
		}    
		
        if ((IsClose(FRONT) == true) || (IsClose(REAR) == true))
        {
            nError = IO_Init();
            Sleep(200);
        }
        else
            nError = IO_Init();

        if (nError == NO_ERR)
            nError = AlarmReset();
        if (nError)
            nError = AlarmReset();
#if(OHT_NAME == OHT_NAME_P4_MASK)
		// By mgchoi 2024.05.21 Inposition Off 시 Origin 하도록 기능 추가. Close동일 사양
		if ( (nError == NO_ERR) && (IsInposition(BOTH) != true) )
		{
			nError = Origin();
			if(nError)
				return nError;
		}
#endif

        if (nError == NO_ERR)
        {
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_OPEN, ON);
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_OPEN, ON);
			if (nError == NO_ERR)
				m_nMovingCount++;

            if (nError)
			{
                nError = ERR_SEND_SHUTTER_OPEN_COMMAND;
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_OPEN_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
			}
            else
            {
                dwStartTime = timeGetTime();
               	nSendCommand = SHUTTER_OPEN_COMMAND;

				bool bIsTimeChecked = false;

                while ((nError = CheckComplete()) == NEXT_CHECK)
				{

					DiagManager::getInstance()->IncreasePreviosDiagMax(PREVIOS_DIAG_SHUT_MAX,OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));//AOHC-196 사전 로그 추가
					//진단모니터링 항목(Shutter move timeover)
					int nMoveTimeover = DiagManager::getInstance()->GetDiagParam().nRefAbnormTimeoverMoveShutter;

					if((OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= nMoveTimeover) && (bIsTimeChecked == false))
					{
						DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_SHUTTER);
						bIsTimeChecked = true;
					}

                    Sleep(10);
                }
            }

            if (nSendCommand == SHUTTER_ORIGIN_COMMAND)
            {
                if (nError == NO_ERR)
                    nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_ORIGIN, OFF);
                if (nError == NO_ERR)
                    nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_ORIGIN, OFF);
            }
            else
            {
                if (nError == NO_ERR)
                    nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_OPEN, OFF);
                if (nError == NO_ERR)
                    nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_OPEN, OFF);
            }

        }
    }
    return nError;
}

/**
@brief   Shutter Unit을 Close 시키는 함수
@author  LSJ
@date    2012.11.01
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::Close()
{
	int nError = NO_ERR;

	if (bReadyOperation == false)
	{
		nError = ERR_SHUTTER_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHUTTER_NOTORIGIN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
	}
	if (nError == NO_ERR)
		nError = Enable();

	if (nError == NO_ERR)
#if(OHT_NAME != OHT_NAME_P4_MASK)
		nError = CheckSensor(SHUTTER_NO_POS);
#else
		nError = CheckSensor(SHUTTER_NO_POS,2);	// 0 : Origin, 1 : Open, 2 : Close
#endif

	if (nError == NO_ERR)
	{
		if((bReadyOperation == true) && (IsClose(BOTH) == true) && (IsInposition(BOTH) == true))
		{
			return	nError;
		}

		if ((IsOpen(FRONT) == true) || (IsOpen(REAR) == true))
		{
			nError = IO_Init();
			Sleep(200);
		}
		else
			nError = IO_Init();

		if (nError == NO_ERR)
			nError = AlarmReset();

		// By elibeli2 2014.02.25 Inposition Off 시 Origin 하도록 기능 추가
		if ( (nError == NO_ERR) && (IsInposition(BOTH) != true) )
		{
			nError = Origin();
			if(nError)
				return nError;
		}

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
		// EUV foup이 감지된 경우 - 반만 닫는다
		if((m_pHWCommon->m_Input.POD_EUV_1 == ON)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == ON))		// EUV Middle Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, ON);
		}
		else if((m_pHWCommon->m_Input.POD_EUV_1 == OFF)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == OFF))	// POD Full Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
		}
		else
		{
			// 하나라도 ON이면 에러 발생
			return ERR_EUVROHT_DIFF_EUVSENSINGS;
		}
#else
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
#endif

		if (nError == NO_ERR)
			m_nMovingCount++;

		if (nError)
		{
			nError = ERR_SEND_SHUTTER_CLOSE_COMMAND;
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_CLOSE_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
		}
		else
		{
			dwStartTime = timeGetTime();
			nSendCommand = SHUTTER_CLOSE_COMMAND;
			while ((nError = CheckComplete()) == NEXT_CHECK)
			{
				Sleep(10);
			}
		}

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	if((m_pHWCommon->m_Input.POD_EUV_1 == ON)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == ON))		// EUV Middle Close
	{
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, OFF);
	}
	else if((m_pHWCommon->m_Input.POD_EUV_1 == OFF)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == OFF))	// POD Full Close
	{
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, OFF);
	}
	else
	{
		// 하나라도 ON이면 에러 발생
		return ERR_EUVROHT_DIFF_EUVSENSINGS;
	}
#else
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, OFF);
#endif

	}
	return nError;
}


/**
@brief   Shutter Unit을 Close 시키는 함수
@author  LSJ
@date    2012.11.01
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::Close_Classifying_Material()
{
	int nError = NO_ERR;
	ADD_LOG_SHUTTER("Close_Classifying_Material Type : %d", GetMaterialType());
	if (bReadyOperation == false)
	{
        nError = ERR_SHUTTER_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHUTTER_NOTORIGIN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
	}
    if (nError == NO_ERR)
        nError = Enable();

	if (nError == NO_ERR)
	{
#if(OHT_NAME != OHT_NAME_P4_MASK)
		nError = CheckSensor(SHUTTER_NO_POS);
#else
		nError = CheckSensor(SHUTTER_NO_POS,2);	// 0 : Origin, 1 : Open, 2 : Close
#endif
	}

    if (nError == NO_ERR)
    {
		if((bReadyOperation == true) && (IsClose(BOTH) == true) && (IsInposition(BOTH) == true))
		{
			return	nError;
		}

		if ((IsOpen(FRONT) == true) || (IsOpen(REAR) == true))
        {
			nError = IO_Init();
			Sleep(200);
        }
        else
            nError = IO_Init();

		if (nError == NO_ERR)
			nError = AlarmReset();

		// By elibeli2 2014.02.25 Inposition Off 시 Origin 하도록 기능 추가
		if ( (nError == NO_ERR) && (IsInposition(BOTH) != true) )
		{
			nError = Origin();
			if(nError)
				return nError;
		}

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
		// EUV foup이 감지된 경우 - 반만 닫는다
#if(OHT_NAME == OHT_NAME_STD_V86)
		if ((m_pHWCommon->m_Input.POD_EUV_1 == ON)&& (GetMaterialType() == MATERIALTYPE_RSP200 || GetMaterialType() == MATERIALTYPE_PODEUV)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == ON))       // EUV Middle Close  //재하 정보 define 하기
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, ON);
		}
		else if((m_pHWCommon->m_Input.POD_EUV_1 == OFF)&& (GetMaterialType() == MATERIALTYPE_RSP200 || GetMaterialType() == MATERIALTYPE_PODEUV))
		{
			return ERR_EUVROHT_DIFF_EUVSENSINGS;
		}
		else if ((m_pHWCommon->m_Input.POD_EUV_1 == OFF)&&(GetMaterialType() == MATERIALTYPE_RSP150 )) //&& (m_pHWCommon->m_Input.POD_EUV_2 == OFF))  // POD Full Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
		}
		else           // 하나라도 ON이면 에러 발생
			return ERR_EUVROHT_DIFF_EUVSENSINGS;
#else
		if((m_pHWCommon->m_Input.POD_EUV_1 == ON)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == ON))		// EUV Middle Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, ON);
		}
		else if((m_pHWCommon->m_Input.POD_EUV_1 == OFF)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == OFF))	// POD Full Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
		}
		else
		{
			// 하나라도 ON이면 에러 발생
			return ERR_EUVROHT_DIFF_EUVSENSINGS;
		}
#endif
#else
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
#endif
		
        if (nError == NO_ERR)
			m_nMovingCount++;
        
		if (nError)
		{
            nError = ERR_SEND_SHUTTER_CLOSE_COMMAND;
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_CLOSE_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
		}
		else
        {
			dwStartTime = timeGetTime();
			nSendCommand = SHUTTER_CLOSE_COMMAND;
			while ((nError = CheckComplete()) == NEXT_CHECK)
            {
				Sleep(10);
			}
		}

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	if((m_pHWCommon->m_Input.POD_EUV_1 == ON)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == ON))		// EUV Middle Close
	{
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, OFF);
	}
	else if((m_pHWCommon->m_Input.POD_EUV_1 == OFF)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == OFF))	// POD Full Close
	{
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, OFF);
	}
    else
    {
        // 하나라도 ON이면 에러 발생
		return ERR_EUVROHT_DIFF_EUVSENSINGS;
    }
#else
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, OFF);
#endif

    }
    return nError;
}

// Close동작과는 달리 이 MidClse는 항상 Mid로 닫기 위한 것으로
// auto상태일때에 사용하는 목적이 아니다
// 매뉴얼 모드일때 Mid close하는 전용 동작함수임
int Shutter::Mid_Close()
{
	int nError = NO_ERR;
#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	if (bReadyOperation == false)
		nError = ERR_SHUTTER_NOTORIGIN;
	if (nError == NO_ERR)
		nError = Enable();

	if (nError == NO_ERR)
		nError = CheckSensor(SHUTTER_NO_POS);

	if (nError == NO_ERR)
	{
		if((bReadyOperation == true) && (IsClose(BOTH) == true) && (IsInposition(BOTH) == true))
		{
			return	nError;
		}
		
        if ((IsOpen(FRONT) == true) || (IsOpen(REAR) == true))
        {
            nError = IO_Init();
            Sleep(200);
        }
        else
            nError = IO_Init();

		if (nError == NO_ERR)
			nError = AlarmReset();

		// By elibeli2 2014.02.25 Inposition Off 시 Origin 하도록 기능 추가
		if ( (nError == NO_ERR) && (IsInposition(BOTH) != true) )
		{
			nError = Origin();
			if(nError)
				return nError;
		}

		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, ON);

		if (nError == NO_ERR)
			m_nMovingCount++;
		
		if (nError)
		{
			nError = ERR_SEND_SHUTTER_CLOSE_COMMAND;
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_CLOSE_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
		}
		else
		{
			dwStartTime = timeGetTime();
			nSendCommand = SHUTTER_MIDDLE_COMMAND;
			while ((nError = CheckComplete()) == NEXT_CHECK)
			{
				Sleep(10);
			}
		}

	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, OFF);

	}
#endif
	return nError;
}


/**
@brief   Shutter Unit을 Open 시키는 함수
@author  LSJ
@date    2012.11.01
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::Open_Cmd()
{
    int nError = NO_ERR;

    if (bReadyOperation == false)
	{
        nError = ERR_SHUTTER_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHUTTER_NOTORIGIN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
	}
    if (nError == NO_ERR)
        nError = Enable();

	if (nError == NO_ERR)
	{
#if(OHT_NAME != OHT_NAME_P4_MASK)
		nError = CheckSensor(SHUTTER_NO_POS);
#else
		nError = CheckSensor(SHUTTER_NO_POS,1);	// 0 : Origin, 1 : Open, 2 : Close
#endif
	}

    if (nError == NO_ERR)
    {
		if((bReadyOperation == true) && (IsOpen(BOTH) == true) && (IsInposition(BOTH) == true))
		{
			return	nError;
		}

        if ((IsClose(FRONT) == true) || (IsClose(REAR) == true))
        {
            nError = IO_Init();
            Sleep(200);
        }
        else
            nError = IO_Init();

        if (nError == NO_ERR)
            nError = AlarmReset();
        if (nError)
            nError = AlarmReset();

        if (nError == NO_ERR)
        {
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_OPEN, ON);
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_OPEN, ON);
            if (nError == NO_ERR)
                m_nMovingCount++;

            if (nError)
			{
                nError = ERR_SEND_SHUTTER_OPEN_COMMAND;
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_OPEN_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
			}
            else
            {
                dwStartTime = timeGetTime();
                nSendCommand = SHUTTER_OPEN_COMMAND;

			}

        }
    }
    return nError;
}

/**
@brief   Shutter Unit을 Close 시키는 함수
@author  LSJ
@date    2012.11.01
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return

int Shutter::Close_Cmd()
{
	int nError = NO_ERR;

    if (bReadyOperation == false)
	{    nError = ERR_SHUTTER_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
						ERR_SHUTTER_NOTORIGIN,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						IsOpen(FRONT),
						IsClose(FRONT),
						IsOpen(REAR),
						IsClose(REAR));
	}
    if (nError == NO_ERR)
        nError = Enable();

    if (nError == NO_ERR)
		nError = CheckSensor(SHUTTER_NO_POS);

    if (nError == NO_ERR)
    {
		if((bReadyOperation == true) && (IsClose(BOTH) == true) && (IsInposition(BOTH) == true))
		{
			return	nError;
		}

        if ((IsOpen(FRONT) == true) || (IsOpen(REAR) == true))
        {
            nError = IO_Init();
            Sleep(10);
        }
        else
            nError = IO_Init();

		if (nError == NO_ERR)
			nError = AlarmReset();

		// By elibeli2 2014.02.25 Inposition Off 시 Origin 하도록 기능 추가
		if ( (nError == NO_ERR) && (IsInposition(BOTH) != true) )
		{
			nError = Origin();
			if(nError)
				return nError;
		}

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))&& (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
		// EUV foup이 감지된 경우 - 반만 닫는다
		if ((m_pHWCommon->m_Input.POD_EUV_1 == ON)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == ON))       // EUV Middle Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, ON);
		}
		else if ((m_pHWCommon->m_Input.POD_EUV_1 == OFF)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == OFF))  // POD Full Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
		}
		else           // 하나라도 ON이면 에러 발생
			return ERR_EUVROHT_DIFF_EUVSENSINGS;
#else
		if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
        if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
#endif

        if (nError == NO_ERR)
            m_nMovingCount++;

		if (nError)
		{
            nError = ERR_SEND_SHUTTER_CLOSE_COMMAND;
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_CLOSE_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
		}
        else
        {
            dwStartTime = timeGetTime();
			nSendCommand = SHUTTER_CLOSE_COMMAND;

		}

    }
    return nError;
}
*/

/**
/**
@brief   Shutter Unit을 Open 명령을 AMC에 Send하는 함수
@author  LSJ
@date    2013.04.23
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::SendOpenCmd()
{
    int nError = NO_ERR;
    if (bReadyOperation == false)
	{
        nError = ERR_SHUTTER_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
						ERR_SHUTTER_NOTORIGIN,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						IsOpen(FRONT),
						IsClose(FRONT),
						IsOpen(REAR),
						IsClose(REAR));
	}
    if (nError == NO_ERR)
        nError = Enable();

	if (nError == NO_ERR)
	{
#if(OHT_NAME != OHT_NAME_P4_MASK)
		nError = CheckSensor(SHUTTER_NO_POS);
#else
		nError = CheckSensor(SHUTTER_NO_POS,1);	// 0 : Origin, 1 : Open, 2 : Close
#endif
	}

    if (nError == NO_ERR)
    {
//        if ((IsClose(FRONT) == true) || (IsClose(REAR) == true))
//        {
//            nError = IO_Init();
//            Sleep(200);
//        }
//        else
            nError = IO_Init();

        if (nError == NO_ERR)
            nError = AlarmReset();
        if (nError)
            nError = AlarmReset();

        if (nError == NO_ERR)
        {
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_OPEN, ON);
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_OPEN, ON);
            if (nError == NO_ERR)
                m_nMovingCount++;

            if (nError)
			{
				nError = ERR_SEND_SHUTTER_OPEN_COMMAND;
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_OPEN_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
			}
			else
			{
				dwStartTime = timeGetTime();
                nSendCommand = SHUTTER_OPEN_COMMAND;
            }
        }
    }
    return nError;
}

/**
@brief   Shutter Unit을 Close 명령을 AMC에 Send하는 함수
@author  
@date    2013.04.23
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::SendCloseCmd()
{
    int nError = NO_ERR;

    if (bReadyOperation == false)
	{
        nError = ERR_SHUTTER_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
						ERR_SHUTTER_NOTORIGIN,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						IsOpen(FRONT),
						IsClose(FRONT),
						IsOpen(REAR),
						IsClose(REAR));
	}
    if (nError == NO_ERR)
        nError = Enable();

	if (nError == NO_ERR)
	{
#if(OHT_NAME != OHT_NAME_P4_MASK)
		nError = CheckSensor(SHUTTER_NO_POS);
#else
		nError = CheckSensor(SHUTTER_NO_POS,2);	// 0 : Origin, 1 : Open, 2 : Close
#endif
	}

    if (nError == NO_ERR)
    {
//        if ((IsOpen(FRONT) == true) || (IsOpen(REAR) == true))
//        {
//            nError = IO_Init();
//            Sleep(200);
//        }
//        else
            nError = IO_Init();

        if (nError == NO_ERR)
            nError = AlarmReset();

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
		if ((m_pHWCommon->m_Input.POD_EUV_1 == ON)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == ON))       // EUV Middle Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, ON);
		}
		else if ((m_pHWCommon->m_Input.POD_EUV_1 == OFF)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == OFF))  // POD Full Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
		}
		else           // 하나라도 ON이면 에러 발생
			return ERR_EUVROHT_DIFF_EUVSENSINGS;
#else
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
#endif
		
        if (nError == NO_ERR)
            m_nMovingCount++;

        if (nError)
		{
			nError = ERR_SEND_SHUTTER_CLOSE_COMMAND;
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_CLOSE_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
		}
		else
        {
            dwStartTime = timeGetTime();
			nSendCommand = SHUTTER_CLOSE_COMMAND;

		}

    }
    return nError;
}

/**
@brief   Shutter Unit을 Close 명령을 AMC에 Send하는 함수, Material Type 구분하여 Shutter Close
@author
@date    2013.04.23
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::SendCloseCmd_Classifying_Material()
{
	int nError = NO_ERR;
	ADD_LOG_SHUTTER("SendCloseCmd_Classifying_Material Type : %d", GetMaterialType());
	if (bReadyOperation == false)
	{
        nError = ERR_SHUTTER_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
						ERR_SHUTTER_NOTORIGIN,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						IsOpen(FRONT),
						IsClose(FRONT),
						IsOpen(REAR),
						IsClose(REAR));
	}
    if (nError == NO_ERR)
        nError = Enable();

	if (nError == NO_ERR)
	{
#if(OHT_NAME != OHT_NAME_P4_MASK)
		nError = CheckSensor(SHUTTER_NO_POS);
#else
		nError = CheckSensor(SHUTTER_NO_POS,2);	// 0 : Origin, 1 : Open, 2 : Close
#endif
     }
    if (nError == NO_ERR)
	{
			nError = IO_Init();

		if (nError == NO_ERR)
			nError = AlarmReset();

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
#if(OHT_NAME == OHT_NAME_STD_V86)
		if ((m_pHWCommon->m_Input.POD_EUV_1 == ON)&& (GetMaterialType() == MATERIALTYPE_RSP200 || GetMaterialType() == MATERIALTYPE_PODEUV)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == ON))       // EUV Middle Close  //재하 정보 define 하기
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, ON);
		}
		else if((m_pHWCommon->m_Input.POD_EUV_1 == OFF)&& (GetMaterialType() == MATERIALTYPE_RSP200 || GetMaterialType() == MATERIALTYPE_PODEUV))
		{
        	return ERR_EUVROHT_DIFF_EUVSENSINGS;
		}
		else if ((m_pHWCommon->m_Input.POD_EUV_1 == OFF)&&(GetMaterialType()==MATERIALTYPE_RSP150)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == OFF))  // POD Full Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
		}
		else           // 하나라도 ON이면 에러 발생
			return ERR_EUVROHT_DIFF_EUVSENSINGS;
#else
		if ((m_pHWCommon->m_Input.POD_EUV_1 == ON)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == ON))       // EUV Middle Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_MIDDLE_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_MIDDLE_CLOSE, ON);
		}
		else if ((m_pHWCommon->m_Input.POD_EUV_1 == OFF)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == OFF))  // POD Full Close
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
		}
		else           // 하나라도 ON이면 에러 발생
			return ERR_EUVROHT_DIFF_EUVSENSINGS;
#endif
#else
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
#endif

		if (nError == NO_ERR)
			m_nMovingCount++;

		if (nError)
		{
			nError = ERR_SEND_SHUTTER_CLOSE_COMMAND;
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_CLOSE_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
		}
		else
		{
			dwStartTime = timeGetTime();
			nSendCommand = SHUTTER_CLOSE_COMMAND;

		}

	}
	return nError;
}

/**
@brief   출력 명령에 맞는 동작을 제한 동작 시간 내에 완료했는지 확인하는 함수
@author  LSJ
@date    2012.11.01
@return  문제 없이 동작 시 0, 문제 발생시 해당 에러 코드 리턴.
*/
int Shutter::CheckComplete()
{
    int nError = 0;
    DWORD dwShutterMovingTime = 0;

    if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= ((nSendCommand == SHUTTER_ORIGIN_COMMAND) ?
        dwOriginLimitTime : dwMotionLimitTime))
    {
        switch (nSendCommand)
        {
        case SHUTTER_OPEN_COMMAND:
            IO_Init();
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SHUTTER_OPEN_EXEC_TIMEOVER,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
            return ERR_SHUTTER_OPEN_EXEC_TIMEOVER;
        case SHUTTER_CLOSE_COMMAND:
            IO_Init();
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SHUTTER_CLOSE_EXEC_TIMEOVER,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
            return ERR_SHUTTER_CLOSE_EXEC_TIMEOVER;
        case SHUTTER_ORIGIN_COMMAND:
            IO_Init();
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SHUTTER_PWRRST_EXEC_TIMEOVER,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
            return ERR_SHUTTER_PWRRST_EXEC_TIMEOVER;
        }
    }

    if (nSendCommand != SHUTTER_ORIGIN_COMMAND)
    {
        if (IsAlarm(FRONT) == true)
        {
            IO_Init();
            if (nSendCommand == SHUTTER_OPEN_COMMAND)
            {
				nError = AlarmReset();
#if (OHT_NAME == OHT_NAME_P4_MASK)
				if(nError == NO_ERR)
					nError = CheckSensor(SHUTTER_NO_POS, 0);  // 0: Origin, 1: open, 2: close
#endif
                if (nError == NO_ERR)
                    nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_ORIGIN, ON);
                if (nError)
                {
                    nError = ERR_SEND_SHUTTER_ORIGIN_COMMAND;
					ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_ORIGIN_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
                    return nError;
                }
                else
                {
                    dwStartTime = timeGetTime();
                    nSendCommand = SHUTTER_ORIGIN_COMMAND;
                }
            }
            else
            {
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d",
							ERR_F_SHUTTER_ALARM,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsAlarm(FRONT),
							IsOpen(FRONT),
							IsClose(FRONT));
                return ERR_F_SHUTTER_ALARM;
            }
        }
#if(OHT_NAME != OHT_NAME_P4_MASK)
		if (IsAlarm(REAR) == true)
		{
            IO_Init();
            if (nSendCommand == SHUTTER_OPEN_COMMAND)
            {
                nError = AlarmReset();
                if (nError == NO_ERR)
                    nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_ORIGIN, ON);
                if (nError)
                {
                    nError = ERR_SEND_SHUTTER_ORIGIN_COMMAND;
					ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_ORIGIN_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
                    return nError;
                }
                else
                {
                    dwStartTime = timeGetTime();
                    nSendCommand = SHUTTER_ORIGIN_COMMAND;
                }
            }
            else
            {
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d",
							ERR_R_SHUTTER_ALARM,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsAlarm(FRONT),
							IsOpen(FRONT),
							IsClose(FRONT));
                return ERR_R_SHUTTER_ALARM;
            }
		}
#endif
    }

    switch (nSendCommand)
	{
	case SHUTTER_ORIGIN_COMMAND:
        if (IsOpen(BOTH) == true)
        {
            if (IsInposition(BOTH) == true)
            {
                IO_Init();
                return NO_ERR;
            }
        }
        break;
    case SHUTTER_OPEN_COMMAND:
        if (IsOpen(BOTH) == true)
        {
            if (IsInposition(BOTH) == true)
            {
                dwShutterMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
                dwStartTime = 0;
                if (dwMaxMovingTime < dwShutterMovingTime)
                    dwMaxMovingTime = dwShutterMovingTime;

                IO_Init();
                return NO_ERR;
            }
        }
        break;
    case SHUTTER_CLOSE_COMMAND:
	case SHUTTER_MIDDLE_COMMAND:
        if (IsClose(BOTH) == true)
        {
            if (IsInposition(BOTH) == true)
            {
                dwShutterMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
                dwStartTime = 0;
                if (dwMaxMovingTime < dwShutterMovingTime)
                    dwMaxMovingTime = dwShutterMovingTime;

                IO_Init();
                return NO_ERR;
            }
        }
        break;
    }
    return NEXT_CHECK;
}

/**
@brief   Shutter Unit을 원점 복귀 시키는 함수
@author  LSJ
@date    2012.11.01
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::Origin()
{
    int nError = NO_ERR;

    bReadyOperation = false;
    if (nError == NO_ERR)
		nError = Enable();

#if (OHT_NAME == OHT_NAME_P4_MASK)
	if(nError == NO_ERR)
		nError = CheckSensor(SHUTTER_NO_POS, 0);  // 0: Origin, 1: open, 2: close
#endif

    if (nError == NO_ERR)
    {
        nError = IO_Init();
        if (nError == NO_ERR)
			nError = AlarmReset();

		if (nError == NO_ERR)      //sensor check  timy.
		{
		   if((m_pHWCommon->m_Input.SHUTTER_F_Open == ON && m_pHWCommon->m_Input.SHUTTER_F_Close == ON)
			  || (m_pHWCommon->m_Input.SHUTTER_R_Open == ON && m_pHWCommon->m_Input.SHUTTER_R_Close == ON))
				nError = ERR_SEND_SHUTTER_ORIGIN_COMMAND;
		}

		if (nError == NO_ERR)
        {
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_ORIGIN, ON);
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_ORIGIN, ON);

			if (nError)
			{
				nError = ERR_SEND_SHUTTER_ORIGIN_COMMAND;
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_ORIGIN_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
			}
            else
            {
				dwStartTime = timeGetTime();
                nSendCommand = SHUTTER_ORIGIN_COMMAND;
                while ((nError = CheckComplete()) == NEXT_CHECK)
                {
                    Sleep(10);
                }
            }
			if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_ORIGIN, OFF);
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_ORIGIN, OFF);
        }
    }
    if (nError == NO_ERR)
        bReadyOperation = true;
    else
        bReadyOperation = false;

    return nError;
}

/**
@brief   Shutter가 알람 상태이면 알람을 리셋하는 함수
@author  LSJ
@date    2012.10.30
@return  알람 리셋에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::AlarmReset()
{
    int nError = 0;

    if ((IsAlarm(FRONT) == true) || (IsAlarm(REAR) == true))
	{
		//AOHC-196 사전 로그 추가
		//사전로그 항목(Shutter Alarm reset)
		DiagManager* pDiagManager = DiagManager::getInstance();
		pDiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_SHU_ALAM);

		if (IsAlarm(FRONT) == true)
            nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_RESET, ON);
        if (IsAlarm(REAR) == true)
            nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_RESET, ON);

        if (nError == NO_ERR)
        {
            Sleep(500);
            nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_RESET, OFF);
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_RESET, OFF);
            if (nError == NO_ERR)
                Sleep(200);
        }
    }

    if (nError == NO_ERR)
    {
        if (IsAlarm(FRONT) == true)
			nError = ERR_F_SHUTTER_ALARM;
        else if (IsAlarm(REAR) == true)
            nError = ERR_R_SHUTTER_ALARM;
    }
    return nError;
}

/**
@brief   Shutter 모터를 Eable 상태로 만드는 함수
@author  LSJ
@date    2012.11.01
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::Enable()
{
    int nError = 0;
    if (IsFree(FRONT) == true)
    {
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_FREE, OFF);
		ADD_LOG_SHUTTER("Enable() - IsFree(FRONT) == true, nError : %d", nError);
    }
	
    if (nError == NO_ERR)
        if (IsFree(REAR) == true)
        {
            nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_FREE, OFF);
			ADD_LOG_SHUTTER("Enable() - IsFree(REAR) == true, nError : %d", nError);
        }
    return nError;
}

/**
@brief   Shutter 모터를 Free 상태로 만드는 함수
@author  LSJ
@date    2012.11.01
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::Free()
{
    int nError = 0;

    bReadyOperation = false;
    nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_FREE, ON);

	ADD_LOG_SHUTTER("Free() - Shutter Front Free : ON");
		
    if (nError == NO_ERR)
    {
        nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_FREE, ON);
		ADD_LOG_SHUTTER("Free() - Shutter Rear Free : ON");
    }

    return nError;
}

/**
@brief   Shutter 위치가 Open 인지 확인하는 함수
@author  LSJ
@date    2012.11.01
@param   nFrontRear(Front Shutter 볼지, Rear Shutter 볼지, 둘 다 볼지 결정)
@return  Open 리미트 센서가 On 되어 있으면 true, Off되어 있으면 false 리턴.
*/
bool Shutter::IsOpen(int nFrontRear)
{
    bool bResult = false;

#if (OHT_NAME == OHT_NAME_P4_MASK)
	if(nFrontRear>=0 && nFrontRear<=2) nFrontRear = FRONT;
#endif

    switch (nFrontRear)
    {
    case FRONT:
        if (m_pHWCommon->m_Input.SHUTTER_F_Open == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case REAR:
        if (m_pHWCommon->m_Input.SHUTTER_R_Open == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case BOTH:
        if ((m_pHWCommon->m_Input.SHUTTER_F_Open == ON) && (m_pHWCommon->m_Input.SHUTTER_R_Open == ON))
            bResult = true;
        else
            bResult = false;
        break;
    }

    return bResult;
}

/**
@brief   Shutter 위치가 Close 인지 확인하는 함수
@author  LSJ
@date    2012.11.01
@param   nFrontRear(Front Shutter 볼지, Rear Shutter 볼지, 둘 다 볼지 결정)
@return  Close 리미트 센서가 On 되어 있으면 true, Off되어 있으면 false 리턴.
*/
bool Shutter::IsClose(int nFrontRear)
{
	bool bResult = false;

#if (OHT_NAME == OHT_NAME_P4_MASK)
	if(nFrontRear>=0 && nFrontRear<=2) nFrontRear = FRONT;
#endif

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	if ((m_pHWCommon->m_Input.POD_EUV_1 == ON)) //|| (m_pHWCommon->m_Input.POD_EUV_2 == ON))
	{
	    switch (nFrontRear)
	    {
			case FRONT:
		        if (m_pHWCommon->m_Input.SHUTTER_F_Middle_Close == ON)
		            bResult = true;
		        else
		            bResult = false;
		        break;
			case REAR:
		        if (m_pHWCommon->m_Input.SHUTTER_R_Middle_Close == ON)
		            bResult = true;
		        else
		            bResult = false;
		        break;
		    case BOTH:
				if ((m_pHWCommon->m_Input.SHUTTER_F_Middle_Close == ON) && (m_pHWCommon->m_Input.SHUTTER_R_Middle_Close == ON))
		            bResult = true;
		        else
		            bResult = false;
		        break;
	    }
	}
	else
	{
	    switch (nFrontRear)
		{
			case FRONT:
		        if (m_pHWCommon->m_Input.SHUTTER_F_Close == ON)
		            bResult = true;
		        else
		            bResult = false;
				break;
			case REAR:
				if (m_pHWCommon->m_Input.SHUTTER_R_Close == ON)
		            bResult = true;
				else
					bResult = false;
				break;
			case BOTH:
				if ((nSendCommand == SHUTTER_MIDDLE_COMMAND) &&
					(m_pHWCommon->m_Input.SHUTTER_F_Middle_Close == ON) && (m_pHWCommon->m_Input.SHUTTER_R_Middle_Close == ON))
					bResult = true;
				else if ((m_pHWCommon->m_Input.SHUTTER_F_Close == ON) && (m_pHWCommon->m_Input.SHUTTER_R_Close == ON))
					bResult = true;
				else
					bResult = false;
				break;
		}
	}
#else
	    switch (nFrontRear)
	    {
			case FRONT:
		        if (m_pHWCommon->m_Input.SHUTTER_F_Close == ON)
		            bResult = true;
		        else
		            bResult = false;
		        break;
		    case REAR:
		        if (m_pHWCommon->m_Input.SHUTTER_R_Close == ON)
		            bResult = true;
		        else
		            bResult = false;
		        break;
		    case BOTH:
		        if ((m_pHWCommon->m_Input.SHUTTER_F_Close == ON) && (m_pHWCommon->m_Input.SHUTTER_R_Close == ON))
		            bResult = true;
		        else
		            bResult = false;
		        break;
	    }
#endif

    return bResult;
}


/**
@brief   Shutter 명령이 Open 인지 확인하는 함수
@author  LSJ
@date    2012.11.01
@param   nFrontRear(Front Shutter 볼지, Rear Shutter 볼지, 둘 다 볼지 결정)
@return  Open 리미트 센서가 On 되어 있으면 true, Off되어 있으면 false 리턴.

bool Shutter::IsCmdOpen(int nFrontRear)
{
    bool bResult = false;

    switch (nFrontRear)
    {
    case FRONT:
        if (m_pHWCommon->m_Output.SHUTTER_F_Open == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case REAR:
        if (m_pHWCommon->m_Output.SHUTTER_R_Open == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case BOTH:
        if ((m_pHWCommon->m_Output.SHUTTER_F_Open == ON) && (m_pHWCommon->m_Output.SHUTTER_R_Open == ON))
            bResult = true;
        else
            bResult = false;
        break;
    }

    return bResult;
}
*/

/**
@brief   Shutter 명령이 Close 인지 확인하는 함수
@author  LSJ
@date    2012.11.01
@param   nFrontRear(Front Shutter 볼지, Rear Shutter 볼지, 둘 다 볼지 결정)
@return  Close 리미트 센서가 On 되어 있으면 true, Off되어 있으면 false 리턴.

bool Shutter::IsCmdClose(int nFrontRear)
{
    bool bResult = false;

#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	if ((m_pHWCommon->m_Input.POD_EUV_1 == ON)) //|| (m_pHWCommon->m_Input.POD_EUV_2 == ON))
	{
		switch (nFrontRear)
		{
			case FRONT:
				if (m_pHWCommon->m_Output.SHUTTER_F_Middle_Close == ON)
					bResult = true;
				else
					bResult = false;
				break;
			case REAR:
				if (m_pHWCommon->m_Output.SHUTTER_R_Middle_Close == ON)
					bResult = true;
				else
					bResult = false;
				break;
			case BOTH:
				if ((m_pHWCommon->m_Output.SHUTTER_F_Middle_Close == ON) && (m_pHWCommon->m_Output.SHUTTER_R_Middle_Close == ON))
					bResult = true;
				else
					bResult = false;
				break;
		}
	}
	else
	{
		switch (nFrontRear)
		{
			case FRONT:
				if (m_pHWCommon->m_Output.SHUTTER_F_Close == ON)
					bResult = true;
				else
					bResult = false;
				break;
			case REAR:
				if (m_pHWCommon->m_Output.SHUTTER_R_Close == ON)
					bResult = true;
				else
					bResult = false;
				break;
			case BOTH:
				if ((m_pHWCommon->m_Output.SHUTTER_F_Close == ON) && (m_pHWCommon->m_Output.SHUTTER_R_Close == ON))
					bResult = true;
				else
					bResult = false;
				break;
		}
	}
#else
    switch (nFrontRear)
    {
	    case FRONT:
	        if (m_pHWCommon->m_Output.SHUTTER_F_Close == ON)
	            bResult = true;
	        else
	            bResult = false;
	        break;
	    case REAR:
	        if (m_pHWCommon->m_Output.SHUTTER_R_Close == ON)
	            bResult = true;
	        else
	            bResult = false;
	        break;
	    case BOTH:
	        if ((m_pHWCommon->m_Output.SHUTTER_F_Close == ON) && (m_pHWCommon->m_Output.SHUTTER_R_Close == ON))
	            bResult = true;
	        else
	            bResult = false;
	        break;
    }
#endif

    return bResult;
}
*/


/**
@brief   Shutter 모터 Inposition 신호 상태를 확인하는 함수
@author  LSJ
@date    2012.11.01
@param   nFrontRear(Front Shutter 볼지, Rear Shutter 볼지, 둘 다 볼지 결정)
@return  Shutter 모터 Inposition 신호가 On이면 true, Off면 False 값 리턴
*/
bool Shutter::IsInposition(int nFrontRear)
{
	bool bResult = false;

#if (OHT_NAME == OHT_NAME_P4_MASK)
	if(nFrontRear>=0 && nFrontRear<=2) nFrontRear = FRONT;
#endif

    switch (nFrontRear)
	{
    case FRONT:
        if (m_pHWCommon->m_Input.SHUTTER_F_Inposition == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case REAR:
        if (m_pHWCommon->m_Input.SHUTTER_R_Inposition == ON)
            bResult = true;
        else
            bResult = false;
        break;
	case BOTH:
		if ((m_pHWCommon->m_Input.SHUTTER_F_Inposition == ON) && (m_pHWCommon->m_Input.SHUTTER_R_Inposition == ON))
			bResult = true;
        else
            bResult = false;
        break;
    }

    return bResult;
}

/**
@brief   Shutter가 Alarm상태인지 확인하는 함수
@author  LSJ
@date    2012.11.01
@param   nFrontRear(Front Shutter 볼지, Rear Shutter 볼지, 둘 다 볼지 결정)
@return  Alarm 신호가 On 되어 있으면 true, Off되어 있으면 false 리턴.
*/
bool Shutter::IsAlarm(int nFrontRear)
{
	bool bResult = false;

#if (OHT_NAME == OHT_NAME_P4_MASK)
	if(nFrontRear>=0 && nFrontRear<=2) nFrontRear = FRONT;
#endif

    switch (nFrontRear)
    {
    case FRONT:
        if (m_pHWCommon->m_Input.SHUTTER_F_Alarm == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case REAR:
        if (m_pHWCommon->m_Input.SHUTTER_R_Alarm == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case BOTH:
        if ((m_pHWCommon->m_Input.SHUTTER_F_Alarm == ON) && (m_pHWCommon->m_Input.SHUTTER_R_Alarm == ON))
            bResult = true;
        else
            bResult = false;
        break;
    }

    return bResult;
}

/**
@brief   Shutter의 Free 출력 상태를 확인하는 함수
@author  LSJ
@date    2012.11.01
@param   nFrontRear(Front Shutter 볼지, Rear Shutter 볼지, 둘 다 볼지 결정)
@return  Free 신호가 On 되어 있으면 true, Off되어 있으면 false 리턴.
*/
bool Shutter::IsFree(int nFrontRear)
{
    bool bResult = false;

#if (OHT_NAME == OHT_NAME_P4_MASK)
	if(nFrontRear>=0 && nFrontRear<=2) nFrontRear = FRONT;
#endif

    switch (nFrontRear)
    {
    case FRONT:
        if (m_pHWCommon->m_Output.SHUTTER_F_Free == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case REAR:
        if (m_pHWCommon->m_Output.SHUTTER_R_Free == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case BOTH:
        if ((m_pHWCommon->m_Output.SHUTTER_F_Free == ON) && (m_pHWCommon->m_Output.SHUTTER_R_Free == ON))
            bResult = true;
        else
            bResult = false;
        break;
    }

    return bResult;
}

/**
@brief   Shutter 리미트 센서를 각 상태에 따라 체크하는 함수
@author  LSJ
@date    2012.11.01
@param   nShutterPos(원래 감지 되어야 할 위치)
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::CheckSensor(int nShutterPos)
{
    int nError = 0;
    unsigned int nCheckData = 0;
	bool bEuvModel = false;

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
//    if((m_pHWCommon->m_Input.HOIST_Home == true) &&
//            (m_pHWCommon->m_Input.POD_EUV_1 != m_pHWCommon->m_Input.POD_EUV_2))
//    {
//        nCheckData = 0x90;  // ERR_SHR_MISMATCH_FOUP_SENSING(736)
//    }
//    else
//    {
		nCheckData = (m_pHWCommon->m_Input.SHUTTER_F_Middle_Close << 5) +
		             (m_pHWCommon->m_Input.SHUTTER_R_Middle_Close << 4) +
					 (m_pHWCommon->m_Input.SHUTTER_F_Open << 3) +
					 (m_pHWCommon->m_Input.SHUTTER_R_Open << 2) +
                     (m_pHWCommon->m_Input.SHUTTER_F_Close << 1) +
                     (m_pHWCommon->m_Input.SHUTTER_R_Close);
//    }
#else
	    nCheckData = (m_pHWCommon->m_Input.SHUTTER_F_Open << 3) + 
	        (m_pHWCommon->m_Input.SHUTTER_R_Open << 2) +
	        (m_pHWCommon->m_Input.SHUTTER_F_Close << 1) +
			(m_pHWCommon->m_Input.SHUTTER_R_Close);
#endif

    switch (nCheckData)
    {
    case 0x00: // Front & Rear Shutter No Sensing at Both Pos.
	{
        nError = ERR_SHR_F_NO_SEN_AND_R_NO_SEN;
		ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_F_NO_SEN_AND_R_NO_SEN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
	}
        break;
	case 0x01: // Front  No Sensing at Both Pos. & Rear Close Pos.  0001
		nError = ERR_SHR_F_NO_SEN_AND_R_CLOSE_POS;
                ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_F_NO_SEN_AND_R_CLOSE_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
		break;
	case 0x02: // Front Close Pos. & Rear No Sensing at Both Pos. 0010
		nError = ERR_SHR_F_CLOSE_POS_AND_R_NO_SEN;
                ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_F_CLOSE_POS_AND_R_NO_SEN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
		break;
	case 0x03: // Front Close Pos. & Rear Close Pos.  0011
		if (nShutterPos == SHUTTER_OPEN_POS)
		{
			nError = ERR_SHR_F_CLOSE_POS_AND_R_CLOSE_POS;
                     ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_F_CLOSE_POS_AND_R_CLOSE_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
		}
        else
            nError = NO_ERR;
        break;
	case 0x04: // Front No Sensing at Both Pos. & Rear Open Pos.   0100
		nError = ERR_SHR_F_NO_SEN_AND_R_OPEN_POS;
                ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_F_NO_SEN_AND_R_OPEN_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
		break;
    case 0x06: // Front Close Pos. & Rear Open Pos.           0110
		nError = ERR_SHR_F_CLOSE_POS_AND_R_OPEN_POS;
                ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_F_CLOSE_POS_AND_R_OPEN_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
        break;
    case 0x08: // Front Open Pos. & Rear No Sensing at Both Pos.     1000
        nError = ERR_SHR_F_OPEN_POS_AND_R_NO_SEN;
        ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_F_OPEN_POS_AND_R_NO_SEN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
        break;
	case 0x09: // Front Open Pos. & Rear Close Pos.               1001
		nError = ERR_SHR_F_OPEN_POS_AND_R_CLOSE_POS;
                ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_F_OPEN_POS_AND_R_CLOSE_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
		break;
	case 0x0C: // Front Open Pos. & Rear Open Pos.
		if (nShutterPos == SHUTTER_CLOSE_POS)
		{
			nError = ERR_SHR_F_OPEN_POS_AND_R_OPEN_POS;
                      ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_F_OPEN_POS_AND_R_OPEN_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
                }
		else
			nError = NO_ERR;
		break;
	case 0x10: // Front Open Pos. & Rear Close Pos.      // 0001000
		nError = ERR_SHR_F_MIDDLE_CLOSE_POS_AND_R_OPEN_POS;
		break;
	case 0x20: // Front Close Pos. & Rear No Sensing at Both Pos. 0010 00 00
		nError = ERR_SHR_F_MIDDLE_CLOSE_POS_AND_R_NO_SEN;
		break;
	case 0x30: // Front Close Pos. & Rear No Sensing at Both Pos. 00110000
		if (nShutterPos == SHUTTER_OPEN_POS)
			nError = ERR_SHR_F_MIDDLE_CLOSE_POS_AND_R_MIDDLE_CLOSE_POS;
		else
			nError = NO_ERR;
		break;
	case 0x24: // Front Close Pos. & Rear Open Pos.           10 01 00
		nError = ERR_SHR_F_MIDDLE_CLOSE_POS_AND_R_OPEN_POS;
		break;
	case 0x18: // Front Open Pos. & Rear Close Pos.            1 10 00
		nError = ERR_SHR_F_OPEN_POS_AND_R_MIDDLE_CLOSE_POS;
		break;

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	case 0x90:
#if((OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))
            nError = ERR_SHR_MISMATCH_FOUP_SENSING;
            ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_MISMATCH_FOUP_SENSING,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
#endif
        break;
#endif
    default: // Front or Rear Sensing at Boat Pos.
        nError = ERR_SHR_BOTH_POS_SENING;
        ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHR_BOTH_POS_SENING,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
    }
    return nError;
}


/**
@brief   Shutter Long Run 상태인지 확인하는 함수
@author  LSJ
@date    2012.11.01
@return  Shutter Long Run Flag 값 리턴
*/
bool Shutter::IsLongRun()
{
    return bLongRun;
}

/**
@brief   Shutter Long Run을 시작하는 함수
@author  LSJ
@date    2012.11.01

void Shutter::StartLongRun()
{
    if (IsLongRun() == false)
        bLongRun = true;
}
*/

/**
@brief   Shutter Long Run을 멈추는 함수
@author  LSJ
@date    2012.11.01
*/
/*
void Shutter::StopLongRun()
{
    if (IsLongRun() == true)
        bLongRun = false;
}
*/

/**
@brief   Shutter Moving 횟수를 확인하는 함수
@author  LSJ
@date    2012.11.01
@return  Shutter Moving Count 리턴
*/
int Shutter::GetMovingCount(bool bInit)
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
@brief   Shutter Max Moving Time을 확인하는 함수
@author  LSJ
@date    2012.11.01
@return  Shutter Max Moving Time 리턴

DWORD Shutter::GetMaxMovingTime()
{
    return dwMaxMovingTime;
}
*/

/**
@brief   Shutter 부가 동작할 준비가 되어 있는지 확인하는 함수
@author  LSJ
@date    2012.11.15
@return  준비가 되어 있으면 0, 되어 있지 않으면 ERR_SHUTTER_NOTORIGIN 리턴.
*/
int Shutter::ReadyToOperation()
{
    int nError = NO_ERR;

#if(OHT_NAME == OHT_NAME_P4_MASK)
	nError = CheckSensor(SHUTTER_NO_POS, 4);
#else
	nError = CheckSensor(SHUTTER_NO_POS);
#endif

    if (nError)
		nError = ERR_SHUTTER_NOTORIGIN;
    else if (bReadyOperation == false)
        nError = ERR_SHUTTER_NOTORIGIN;
    return nError;
}

#if(OHT_NAME == OHT_NAME_P4_MASK)
/**
@brief   Shutter Long Run 상태인지 확인하는 함수
@author  mgchoi
@date    2022.09.24
@param   nShutterPos(원래 감지 되어야 할 위치)
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Shutter::CheckSensor(int nShutterPos, int mode)
{
	int nError = NO_ERR;
	unsigned int nCheckData = 0;
	if(mode >= 1)  // 0: orgin 제외  // 0: Origin, 1: open, 2: close, 3: , 4: ReadyToOperation
	{
		nCheckData = (IsOpen(FRONT) << 1) + (IsClose(FRONT) << 0);
		switch (nCheckData)
		{
		case 0x00: // Front Shutter No Sensing at Both Pos.
			nError = ERR_SHR_F_NO_SEN_AND_R_NO_SEN;
			break;
		case 0x01: // Front Close Pos.
			if (nShutterPos == SHUTTER_OPEN_POS)
				nError = ERR_SHUTTER_CLOSE_POS;
			else
				nError = NO_ERR;
			break;
		case 0x02: // Front Open Pos.
			if (nShutterPos == SHUTTER_CLOSE_POS)
				nError = ERR_SHUTTER_OPEN_POS;
			else
				nError = NO_ERR;
			break;
		default: // Front or Rear Sensing at Boat Pos.
			nError = ERR_SHR_BOTH_POS_SENING;
		}
	}
	// Common Check
	if((nError == NO_ERR) && (mode <3))         // moving
	{
		//if(m_pHoistC->IsHoistHome() == OFF)
		//	nError = ERR_SHR_NOTMOVE_HOISTHOME_OFF;

		///AntiTemp..Pod임시 주석처리. mgchoi
		if((IsPodPlacement3On() == true) || (IsPodPlacement4On() == true))
			nError = ERR_SHR_NOTMOVE_PODPLACEMENT_ON;
		else
		;
	}
	if((nError == NO_ERR) && ((mode == 0) || (mode == 2)))    // origin & close
	{
		if(m_pShiftC->IsShiftHome() == false)
			nError = ERR_SHIFT_NOTORIGIN;
		else
			;
	}
	return nError;
}

/**
@brief   3개의 Detect Sensor를 차레로 체크하여 알맞은 POD Type을 Check하는 함수.
@author  mgchoi
@date    2022.08.18
@return  Detect된 POD Type을 Return
*/
int Shutter::CheckPodType()
{
	MaskPodType nResult = MaskPodType::None;

	if (m_pHWCommon->m_Input.EUVPod_Detect == ON)
		nResult = MaskPodType::EUVPod;
	else if(m_pHWCommon->m_Input.RSP150_Detect == ON)
		nResult = MaskPodType::RSP150;
	else if(m_pHWCommon->m_Input.RSP200_Detect == ON)
		nResult = MaskPodType::RSP200;

	return nResult;
}

/**
@brief   POD displacement1 센서 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.19
@return  POD displacement1 신호가 On이면 true, Off면 False 값 리턴
*/
bool Shutter::IsPodPlacement1On()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.ANTIDROP_POD_Placement1 == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   POD displacement2 센서 상태를 확인하는 함수
@author  mgchoi
@date    2022.08.19
@return  POD displacement2 신호가 On이면 true, Off면 False 값 리턴
*/
bool Shutter::IsPodPlacement2On()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.ANTIDROP_POD_Placement2 == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}
/**
@brief   POD displacement3 센서 상태를 확인하는 함수
@author  mgchoi
@date    2023.01.31
@return  POD displacement3 신호가 On이면 true, Off면 False 값 리턴
*/
bool Shutter::IsPodPlacement3On()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.ANTIDROP_POD_Placement3 == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   POD displacement4 센서 상태를 확인하는 함수
@author  mgchoi
@date    2023.01.31
@return  POD displacement4 신호가 On이면 true, Off면 False 값 리턴
*/
bool Shutter::IsPodPlacement4On()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.ANTIDROP_POD_Placement4 == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}
#endif

void Shutter::SetMaterialType(BYTE set)
{
	m_nSetMaterialType = set;
}
BYTE Shutter::GetMaterialType()
{
	return m_nSetMaterialType;
}

int Shutter::Open_AutoRecovery()
{
	int nError = NO_ERR;

	if (bReadyOperation == false)
	{
        nError = ERR_SHUTTER_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHUTTER_NOTORIGIN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
	}
    if (nError == NO_ERR)
        nError = Enable();

    if (nError == NO_ERR)
	{
		if((bReadyOperation == true) && (IsOpen(BOTH) == true) && (IsInposition(BOTH) == true))
		{
			return	nError;
		}

        if ((IsClose(FRONT) == true) || (IsClose(REAR) == true))
        {
			nError = IO_Init();
        }
        else
            nError = IO_Init();

        if (nError == NO_ERR)
            nError = AlarmReset();
        if (nError)
			nError = AlarmReset();

        if (nError == NO_ERR)
        {
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_OPEN, ON);
            if (nError == NO_ERR)
                nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_OPEN, ON);
            if (nError == NO_ERR)
				m_nMovingCount++;

			if (nError)
			{
                nError = ERR_SEND_SHUTTER_OPEN_COMMAND;
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_OPEN_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
			}
            else
			{
				dwStartTime = timeGetTime();
				nSendCommand = SHUTTER_OPEN_COMMAND;

				bool bIsTimeChecked = false;

                while ((nError = CheckComplete()) == NEXT_CHECK)
				{

					DiagManager::getInstance()->IncreasePreviosDiagMax(PREVIOS_DIAG_SHUT_MAX,OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));//AOHC-196 사전 로그 추가
					//진단모니터링 항목(Shutter move timeover)
					int nMoveTimeover = DiagManager::getInstance()->GetDiagParam().nRefAbnormTimeoverMoveShutter;

					if((OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= nMoveTimeover) && (bIsTimeChecked == false))
					{
						DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_SHUTTER);
						bIsTimeChecked = true;
					}

                    Sleep(10);
                }
            }

            if (nSendCommand == SHUTTER_ORIGIN_COMMAND)
            {
                if (nError == NO_ERR)
                    nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_ORIGIN, OFF);
                if (nError == NO_ERR)
                    nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_ORIGIN, OFF);
            }
            else
            {
                if (nError == NO_ERR)
                    nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_OPEN, OFF);
                if (nError == NO_ERR)
                    nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_OPEN, OFF);
			}
        }
    }
    return nError;
}

int Shutter::Close_AutoRecovery()
{
	int nError = NO_ERR;

	if (bReadyOperation == false)
	{
        nError = ERR_SHUTTER_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHUTTER_NOTORIGIN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsOpen(FRONT),
					IsClose(FRONT),
					IsOpen(REAR),
					IsClose(REAR));
	}
    if (nError == NO_ERR)
		nError = Enable();

    if (nError == NO_ERR)
	{
		if((bReadyOperation == true) && (IsClose(BOTH) == true) && (IsInposition(BOTH) == true))
		{
			return	nError;
		}

		if ((IsOpen(FRONT) == true) || (IsOpen(REAR) == true))
        {
			nError = IO_Init();
			Sleep(200);
		}
		else
			nError = IO_Init();

		if (nError == NO_ERR)
			nError = AlarmReset();

		if(nError == NO_ERR)
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_ORIGIN, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_ORIGIN, ON);
			if (nError)
			{
				nError = ERR_SEND_SHUTTER_ORIGIN_COMMAND;
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_ORIGIN_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
			}
			else
			{
				dwStartTime = timeGetTime();
				nSendCommand = SHUTTER_ORIGIN_COMMAND;

				bool bIsTimeChecked = false;

                while ((nError = CheckComplete()) == NEXT_CHECK)
				{

					DiagManager::getInstance()->IncreasePreviosDiagMax(PREVIOS_DIAG_SHUT_MAX,OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));//AOHC-196 사전 로그 추가
					//진단모니터링 항목(Shutter move timeover)
					int nMoveTimeover = DiagManager::getInstance()->GetDiagParam().nRefAbnormTimeoverMoveShutter;

					if((OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= nMoveTimeover) && (bIsTimeChecked == false))
					{
						DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_SHUTTER);
						bIsTimeChecked = true;
					}

					Sleep(10);
				}
			}
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_ORIGIN, OFF);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_ORIGIN, OFF);

		}

		if (nError == NO_ERR)
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, ON);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, ON);
			if (nError == NO_ERR)
				m_nMovingCount++;

			if (nError)
			{
				nError = ERR_SEND_SHUTTER_CLOSE_COMMAND;
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SEND_SHUTTER_CLOSE_COMMAND,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsOpen(FRONT),
							IsClose(FRONT),
							IsOpen(REAR),
							IsClose(REAR));
			}
			else
			{
				dwStartTime = timeGetTime();
				nSendCommand = SHUTTER_CLOSE_COMMAND;

				bool bIsTimeChecked = false;

                while ((nError = CheckComplete()) == NEXT_CHECK)
				{

					DiagManager::getInstance()->IncreasePreviosDiagMax(PREVIOS_DIAG_SHUT_MAX,OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));//AOHC-196 사전 로그 추가
					//진단모니터링 항목(Shutter move timeover)
					int nMoveTimeover = DiagManager::getInstance()->GetDiagParam().nRefAbnormTimeoverMoveShutter;

					if((OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= nMoveTimeover) && (bIsTimeChecked == false))
					{
						DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_SHUTTER);
						bIsTimeChecked = true;
					}

					Sleep(10);
				}
			}

			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_F_CLOSE, OFF);
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_SHUTTER_R_CLOSE, OFF);
		}

	}
	if(GetAutoRecoveryClose() != SHUTTER_AR_CLOSE_COMPLETE)
	{
		SetAutoRecoveryClose(SHUTTER_AR_CLOSE_COMPLETE);
	   	ADD_LOG_SHUTTER("[Close_AutoRecovery]SetAutoRecoveryClose(SHUTTER_AR_CLOSE_COMPLETE)");
	}
    return nError;
}

void Shutter::SetAutoRecoveryClose(int nFlag)
{
	nAutoRecoveryClose = nFlag;

	if(nAutoRecoveryClose == SHUTTER_AR_CLOSE_START)
		SetAutoRecoveryStartTime();

  ADD_LOG_SHUTTER("[SetAutoRecoveryClose]nAutoRecoveryClose : %d, nFlag : %d",nAutoRecoveryClose,nFlag );
}
int Shutter::GetAutoRecoveryClose()
{
   return nAutoRecoveryClose;
}

void Shutter::SetAutoRecoveryStartTime()
{
	dwAutoRecoveryTime = timeGetTime();
}

DWORD Shutter::GetAutoRecoveryStartTime()
{
	return dwAutoRecoveryTime;
}
