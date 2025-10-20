// ---------------------------------------------------------------------------
/**
@file    RailSupport.cpp
@version OHT 7.0
@brief   RailSupport Class CPP File
*/

#include "RailSupport.h"

#define NOOUTTIRRGER 1

/**
@brief   RailSupport 생성자

 direction선택 -> torque 선택 (output은 반대임)
 Motor Free : Tr1=1 Tr0=1
 Traction Torque : Tr1=1 Tr0=0
 Stop Torque : Tr1=0 Tr0=1
 Moving Torque : Tr1=0 Tr0=0
@author	 LSJ
@date    2012.11.01
@param   pHWCommon : HWCommon Class의 포인터
*/
RailSupport::RailSupport(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
    Init();
}

/**
@brief   RailSupport 소멸자
@author	 LSJ
@date    2012.11.01
*/
RailSupport::~RailSupport()
{

}

/**
@brief   RailSupport의 입출력 및 각종 변수 초기화
@author	 LSJ
@date    2012.11.01
@return	 정상인 경우 0, 비정상인 경우 ErrorCode Return
*/
void RailSupport::Init()
{
    bReadyOperation = false;
	bLongRun = false;

	nMovingCount = 0;
    dwMaxMovingTime = 0;
    nSendCommand = 0;
	dwStartTime = 0;
	nDelayTime = 0;

	m_nCheckOuttiggerCount = 0;

	dwOriginLimitTime = RAILSUPPORT_PWRRST_LIMIT_TIME;
	dwMotionLimitTime = RAILSUPPORT_MOTION_LIMIT_TIME;

}

/**
@brief   RailSupport Unit을 Work Position으로 이동시킴.
@author	 LSJ
@date    2012.11.01
@return	 정상인 경우 0, 비정상인 경우 ErrorCode Return
*/
int RailSupport::MoveWorkPos()
{
	int nError = NO_ERR;

#if((OHT_NAME == OHT_NAME_STD_V80))
	if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
			  //	m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
			 //	m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
			   //	m_pHWCommon->SetBit(OUT_Outtrigger_R_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
			  //	m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, OFF);
		   }

		m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
		Sleep(50);

		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
		Sleep(50);

        if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
		//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, ON);
		Sleep(10);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);

		if (nError == NO_ERR)
			nMovingCount++;
	}

	if (nError)
		nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = RAILSUPPORT_WORK_COMMAND;
		m_nCheckOuttiggerCount= 0;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
#elif(OHT_NAME == OHT_NAME_STD)
  if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_78, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_77, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_79, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_76, OFF);
		   }
		Sleep(10);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, ON);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_77, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_76, ON);

		if (nError == NO_ERR)
			nMovingCount++;
	}

	if (nError)
		nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = RAILSUPPORT_WORK_COMMAND;
		m_nCheckOuttiggerCount= 0;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
    if (nError == NO_ERR)
    {
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
    else
    {
        dwStartTime = timeGetTime();
		nSendCommand = RAILSUPPORT_WORK_COMMAND;
		m_nCheckOuttiggerCount = 0;
        while ((nError = CheckComplete()) == NEXT_CHECK)
        {
            Sleep(10);
        }
    }
#endif
	return nError;
}

/**
@brief   RailSupport Unit을 Home Position으로 이동시킴.
@author	 LSJ
@date    2012.11.01
@return	 정상인 경우 0, 비정상인 경우 ErrorCode Return
*/
int RailSupport::MoveHomePos()
{
	int nError = NO_ERR;
#if(  (OHT_NAME == OHT_NAME_STD_V80))
	if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
			//	m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
			//	m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
			 //	m_pHWCommon->SetBit(OUT_Outtrigger_R_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
			 //	m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, OFF);
		   }

		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
		Sleep(50);

		//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, ON);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
		Sleep(50);

		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, ON);

        Sleep(50);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

	if (nError)
		nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
	else
	{
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
		nSendCommand = RAILSUPPORT_HOME_COMMAND;

		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
#elif(OHT_NAME == OHT_NAME_STD)
    if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_78, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_77, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_79, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_76, OFF);
		   }
		   Sleep(10);

		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, ON);

		if (nError == NO_ERR)
			nMovingCount++;
	}

	if (nError)
		nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = RAILSUPPORT_HOME_COMMAND;
		m_nCheckOuttiggerCount = 0;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
#else
    if (nError == NO_ERR)
    {
		nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);

		if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
    else
    {
        dwStartTime = timeGetTime();
		nSendCommand = RAILSUPPORT_HOME_COMMAND;
		m_nCheckOuttiggerCount= 0;
        while ((nError = CheckComplete()) == NEXT_CHECK)
        {
            Sleep(10);
        }
    }
#endif
	return nError;
}

/**
@brief   RailSupport Unit을 Work Position으로 이동시키는 명령만 보냄.
@author	 LSJ
@date    2013.04.23
@return	 정상인 경우 0, 비정상인 경우 ErrorCode Return
*/
int RailSupport::SendMoveWorkPosCmd()
{
    int nError = NO_ERR;

#if((OHT_NAME == OHT_NAME_STD_V80))
    if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, OFF);
		   }

		m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
		Sleep(10);

		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
		Sleep(10);

		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);

		Sleep(10);
//		//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);


        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
        nSendCommand = RAILSUPPORT_WORK_COMMAND;
	}
#elif(OHT_NAME == OHT_NAME_STD)
      if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_78, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_77, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_79, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_76, OFF);
		   }
		   Sleep(10);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, ON);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
        nSendCommand = RAILSUPPORT_WORK_COMMAND;
	}
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
#else
    if (nError == NO_ERR)
    {
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
        nSendCommand = RAILSUPPORT_WORK_COMMAND;
    }
#endif
    return nError;
}

/**
@brief   RailSupport Unit을 Home Position으로 이동시키는 명령만 보냄.
@author	 LSJ
@date    2013.04.23
@return	 정상인 경우 0, 비정상인 경우 ErrorCode Return
*/
int RailSupport::SendMoveHomePosCmd()
{
    int nError = NO_ERR;

#if((OHT_NAME == OHT_NAME_STD_V80))
    if (nError == NO_ERR)
    {
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, OFF);
		   }


        nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
		Sleep(10);

		//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, ON);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
		Sleep(10);

		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, ON);

        Sleep(10);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
        nSendCommand = RAILSUPPORT_HOME_COMMAND;
	}
#elif(OHT_NAME == OHT_NAME_STD)
      if (nError == NO_ERR)
    {
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_78, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_77, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_79, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_76, OFF);
		   }
		   Sleep(10);
 // nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
        nSendCommand = RAILSUPPORT_HOME_COMMAND;
	}
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
	if (nError == NO_ERR)
    {
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
		nSendCommand = RAILSUPPORT_HOME_COMMAND;
    }
#endif
    return nError;
}

/**
@brief   출력 명령에 맞는 동작을 제한 동작 시간 내에 완료했는지 확인하는 함수
@author  LSJ
@date    2012.11.01
@return  문제 없이 동작 시 0, 문제 발생시 해당 에러 코드 리턴.
*/
int RailSupport::CheckComplete()
{
    DWORD dwRailSupportMovingTime = 0;

   // *** Error Occur : Timeover ****
    if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >=
		((nSendCommand == RAILSUPPORT_ORIGIN_COMMAND) ? dwOriginLimitTime :
		dwMotionLimitTime))
	{
        switch (nSendCommand)
        {
		case RAILSUPPORT_HOME_COMMAND:
            Stop();
            return ERR_RAILSUPPORT_MOVE_HOME_TIMEOVER;
        case RAILSUPPORT_WORK_COMMAND:
			Stop();
			return ERR_RAILSUPPORT_MOVE_WORK_TIMEOVER;
        }
    }
		
#if(OHT_NAME == OHT_NAME_STD)
	m_nCheckOuttiggerCount++;

	switch (nSendCommand)
	{
	case RAILSUPPORT_ORIGIN_COMMAND:
		 // left상태에서 Pwr_Rst시 alarm상태에서 바로 return되므로 alarm check해야 함...
        if (IsHomePos() == true)
        {
            Stop();
            bReadyOperation = true;
            return NO_ERR;
        }
        break;
    case RAILSUPPORT_HOME_COMMAND:
        if (IsHomePos() == true)
        {
            Stop();

            dwRailSupportMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
            dwStartTime = 0;
            if (dwMaxMovingTime < dwRailSupportMovingTime)
				dwMaxMovingTime = dwRailSupportMovingTime;

            bReadyOperation = true;
            return NO_ERR;
        }
        break;
    case RAILSUPPORT_WORK_COMMAND:
		if ((IsWorkPos() == true) && (m_nCheckOuttiggerCount >=40))
		{
            Stop();

            dwRailSupportMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
            dwStartTime = 0;
			if (dwMaxMovingTime < dwRailSupportMovingTime)
                dwMaxMovingTime = dwRailSupportMovingTime;

            bReadyOperation = true;
            return NO_ERR;
        }
        break;
	}
#else
	switch (nSendCommand)
    {
	case RAILSUPPORT_ORIGIN_COMMAND:
		 // left상태에서 Pwr_Rst시 alarm상태에서 바로 return되므로 alarm check해야 함...
        if (IsHomePos() == true)
        {
            Stop();
            bReadyOperation = true;
            return NO_ERR;
        }
        break;
    case RAILSUPPORT_HOME_COMMAND:
		if ((IsHomePos() == true) || (m_nCheckOuttiggerCount > 3))
		{
			m_nCheckOuttiggerCount++;
            Stop();

            dwRailSupportMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
            dwStartTime = 0;
            if (dwMaxMovingTime < dwRailSupportMovingTime)
				dwMaxMovingTime = dwRailSupportMovingTime;

            bReadyOperation = true;
            return NO_ERR;
        }
        break;
    case RAILSUPPORT_WORK_COMMAND:
		if ((IsWorkPos(1) == true) || (m_nCheckOuttiggerCount > 3))
		{
			m_nCheckOuttiggerCount++;
            Stop();

            dwRailSupportMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
            dwStartTime = 0;
			if (dwMaxMovingTime < dwRailSupportMovingTime)
                dwMaxMovingTime = dwRailSupportMovingTime;

            bReadyOperation = true;
            return NO_ERR;
        }
        break;
	}
#endif
    return NEXT_CHECK;
}

/**
@brief   RailSupport 토크 값이 구동 토크를 유지하고 있는지 확인하여 결과를 리턴하는 함수
@author  LSJ
@date    2014.02.10
@return  구동 토크 상태이면 true, 이외이면 false를 Return
*/
bool RailSupport::IsMovingTorque()
{
	bool bStatus = false;
#if((OHT_NAME == OHT_NAME_STD_V80))
	if ((m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == 1)) //&& (m_pHWCommon->m_Output.Outtrigger_R_Start_OUT == 1))
	{
		bStatus = true;
    }
    else
	{
		bStatus = false;
	}
#elif(OHT_NAME == OHT_NAME_STD)
	if ((m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == 1) && (m_pHWCommon->m_Output.Outtrigger_R_Start_OUT == 1))
	{
		bStatus = true;
    }
    else
	{
		bStatus = false;
	}
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
#else
	if ((m_pHWCommon->m_Output.RailSupport_TRQ_0 == 1) && (m_pHWCommon->m_Output.RailSupport_TRQ_1 == 1))
	{
		bStatus = true;
    }
    else
	{
		bStatus = false;
    }
#endif

	return bStatus;
}

/**
@brief   RailSupport 솔레노이드를 Free 시키는 함수
@author  LSJ
@date    2012.11.01
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int RailSupport::Free()
{
    int nError = NO_ERR;
#if((OHT_NAME == OHT_NAME_STD)|| (OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
#else
    nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, OFF);
#endif
    return nError;
}

/**
/**
@brief   RailSupport 솔레노이드를 정지 토크 상태로 Enable 시키는 함수
@author  LSJ
@date    2012.11.01
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int RailSupport::Enable()
{
    int nError = NO_ERR;
#if((OHT_NAME == OHT_NAME_STD)|| (OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
#else
    nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);
#endif
    return nError;
}

/**
@brief   솔레노이드를 정지 토크로 바꾸는 함수
@author  LSJ
@date    2012.11.01
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int RailSupport::Stop()
{
    int nError = NO_ERR;

#if((OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
	//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, ON);
	Sleep(10);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
#elif(OHT_NAME == OHT_NAME_STD)
   	nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
	//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, ON);
	Sleep(50);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
	if (nError == NO_ERR)
	//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, OFF);

#else
    nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);

#endif
	return nError;
}

/**
@brief   Rail Support 위치가 Work 위치 인지 확인하는 함수
@author	 LSJ
@date    2012.11.01
@return  Work 리미트 센서가 On 되어 있으면 true, Off되어 있으면 false 리턴.
*/
bool RailSupport::IsWorkPos(int nCheck)
{
	bool bResult = false;
#if((OHT_NAME == OHT_NAME_STD_V80))

	if(nCheck == 0)
	{
		if ((m_pHWCommon->m_Input.Outtrigger_F_Alarm_IN == ON) || (m_pHWCommon->m_Input.Outtrigger_F_Work_IN == ON)) // || (m_pHWCommon->m_Input.Outtrigger_F_Home_IN == OFF))
			bResult = true;
		else
			bResult = false;
	}else if(nCheck == 1)
	{
		if ((m_pHWCommon->m_Input.Outtrigger_F_Alarm_IN == ON))
			bResult = true;
		else
			bResult = false;
	}else
	{
		if ((m_pHWCommon->m_Input.Outtrigger_F_Work_IN == ON))
			bResult = true;
		else
			bResult = false;
    }

#if NOOUTTIRRGER
		bResult = false;
#endif

#elif(OHT_NAME == OHT_NAME_STD)
	if ((m_pHWCommon->m_Input.Outtrigger_F_Home_IN == ON) //&& (m_pHWCommon->m_Input.Outtrigger_R_Home_IN == ON))
		bResult = true;
	else
		bResult = false;
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
    if (m_pHWCommon->m_Input.Railsupport_Work == ON)
        bResult = true;
    else
        bResult = false;

#endif
	return bResult;
}

/**
@brief   Rail Support 위치가 Home 위치 인지 확인하는 함수
@author	 LSJ
@date    2012.11.01
@return  Home 리미트 센서가 On 되어 있으면 true, Off되어 있으면 false 리턴.
*/
bool RailSupport::IsHomePos()
{
	bool bResult = false;
#if((OHT_NAME == OHT_NAME_STD_V80))
	if ((m_pHWCommon->m_Input.Outtrigger_F_Home_IN == ON))// && (m_pHWCommon->m_Input.Outtrigger_R_Home_IN == OFF))
		bResult = true;
    else
		bResult = false;
#if NOOUTTIRRGER
		bResult = true;
#endif

#elif(OHT_NAME == OHT_NAME_STD)
	if ((m_pHWCommon->m_Input.Outtrigger_F_Home_IN == OFF) && (m_pHWCommon->m_Input.Outtrigger_R_Home_IN == OFF))
		bResult = true;
    else
		bResult = false;
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
    if (m_pHWCommon->m_Input.RailSupport_Home == ON)
        bResult = true;
    else
        bResult = false;
#endif
    return bResult;
}

/**
@brief   Rail Support Long Run 상태인지 확인하는 함수
@author  LSJ
@date    2012.11.01
@return  Rail Support Long Run Flag 값 리턴
*/
bool RailSupport::IsLongRun()
{
    return bLongRun;
}

/**
@brief   Rail Support Long Run을 시작하는 함수
@author  LSJ
@date    2012.11.01
*/
void RailSupport::StartLongRun()
{
    if (IsLongRun() == false)
        bLongRun = true;
}

/**
@brief   Rail Support Long Run을 멈추는 함수
@author  LSJ
@date    2012.11.01
*/
void RailSupport::StopLongRun()
{
    if (IsLongRun() == true)
        bLongRun = false;
}

/**
@brief   Rail Support Moving 횟수를 확인하는 함수
@author  LSJ
@date    2012.11.01
@return  Rail Support Moving Count 리턴
*/
int RailSupport::GetMovingCount()
{
    return nMovingCount;
}

/**
@brief   Rail Support Max Moving Time을 확인하는 함수
@author  LSJ
@date    2012.11.01
@return  Rail Support Max Moving Time 리턴
*/
DWORD RailSupport::GetMaxMovingTime()
{
    return dwMaxMovingTime;
}

/**
@brief   RailSupport 부가 동작할 준비가 되어 있는지 확인하는 함수
@author  LSJ
@date    2012.11.15
@return  준비가 되어 있으면 0, 되어 있지 않으면 ERR_STEER_NOT_READY 리턴.
*/
int RailSupport::ReadyToOperation(void)
{
    int nError = NO_ERR;
    if (IsHomePos() != true)
        nError = ERR_RAILSUPPORT_NOT_READY;

    return nError;
}

/**
@brief   RailSupport 부의 상태에 따라 Origin 명령을 주고 난 후 동작할 준비가 되어 있는지 확인하는 함수
@author  LSJ
@date    2012.11.15
@return  준비가 되어 있으면 0, 되어 있지 않으면 ERR_STEER_NOT_READY 리턴.
*/
int RailSupport::AutoInitialize()
{
    int nError = 0;
   // AnsiString strLogData;
   // strLogData.sprintf("Steering Auto Origin Start[E%d]", nRcvError);
   // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile(strLogData);

    //nError = MoveHomePos();

    if (IsHomePos() == true)
        bReadyOperation = true;
    else
        bReadyOperation = false;
   // strLogData.sprintf("Steering Auto Origin End..[E%d]", nError);
   // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile(strLogData);
   nError = ReadyToOperation();

    return nError;
}

/**
@brief	 RailSupport의 동작 출력과 리미트 센서가 틀릴 경우 정지 토크로 RailSupport를 움직이는 함수
@author  LSJ
@date	 2014.02.06
@return  문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int RailSupport::Adjust()
{
	int nError = 0;

// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD) ||  (OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_2))
#else
	if (IsHomePos() == true)
	{
		nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, ON);
	}
	else if (IsWorkPos() == true)
	{
		nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, OFF);
	}

	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);
#endif
	return nError;
}

// ---------------------------------------------------------------------------

