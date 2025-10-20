/**
@file    Rotate.cpp
@version OHT 7.0
@brief   Rotate Class CPP File
*/

#include "Rotate.h"
#define LOG_JIG(...)   WRITE_LOG_UNIT(pLogJig, __VA_ARGS__)

/**
@brief   Rotate Class 생성자
@author
@date
@param   pHWCommon : HWCommon 클래스 포인터

*/
Rotate::Rotate(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
	pLogJig = getLogUnit("D:\\log\\jig.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	Init();
}

/**
@brief   Rotate Class 소멸자
@author
@date
*/
Rotate::~Rotate()
{
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
void Rotate::Init()
{
	bReadyOperation = false;
	dwStartTime = 0;
	nSendCommand	 = ROTATE_NO_COMMAND;
	dwOriginLimitTime = ROTATE_ORIGIN_LIMIT_TIME;
	dwMotionLimitTime = ROTATE_MOTION_LIMIT_TIME;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Rotate_0_Manual(void)
{
	int nError=0;
	if(bReadyOperation == false) nError = ERR_ROTATE_NOTORIGIN;
	if(nError==NO_ERR) nError = Enable();

	if(nError==NO_ERR)
	{
		nError = Rotate_IO_Init();
		if(nError == NO_ERR) nError = Alarm_Check_Reset();
		if(nError == NO_ERR) m_pHWCommon->SetBit(OUT_ROTATE_0, ON);

		if(nError) nError = ERR_SEND_ROTATE_0_COMMAND;
		else
		{
			dwStartTime  = timeGetTime();
			nSendCommand = ROTATE_0_COMMAND;
			while((nError = CheckMotionComplete()) == NEXT_CHECK)
			{
			  Sleep(10);
			}
		 }
	}
	return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Rotate_0_Auto(void)
{
	int nError=0;
    if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;
    if(nError==NO_ERR) nError = Enable();
	if(nError==NO_ERR) nError = Rotate_IO_Init();
	if(nError==NO_ERR) nError = Alarm_Check_Reset();

    // alarm check
	if(nError==NO_ERR)
	{
		if(IsAlarm() == true)
		{
			 nError = Alarm_Check_Reset();
			 if(IsAlarm() == true) nError = ERR_ROTATE_ALARM;
		}
	}

	// limit sensor check
	if(nError==NO_ERR)
	{
		if(Is0Pos() == true)
		{
		//??????Limit 위치가 달라지는지 확인 필요

			if((Is90Pos() == true)||(Is180Pos() == true)||(Is270Pos() == true))

				nError=ERR_ROTATE_LIMIT_ON_ERR;
		}

		else if((Is90Pos() == false)&&(Is180Pos() == false)&&(Is270Pos() == false))
			 nError=ERR_ROTATE_LIMIT_OFF_ERR;
		else
		{
			nError = m_pHWCommon->SetBit(OUT_ROTATE_0, ON);
			if(nError) nError = ERR_SEND_ROTATE_0_COMMAND;
		}
	}

    if(nError==NO_ERR)
	{
		dwStartTime  = timeGetTime();
		nSendCommand = ROTATE_0_COMMAND;
		while((nError = CheckMotionComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}

	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Send_Rotate_0_AutoCmd(void)
{
	int nError=0;

	if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;
	if(nError==NO_ERR) nError = Enable();
	if(nError==NO_ERR) nError = Rotate_IO_Init();

	// alarm check
	if(IsAlarm() == true)
	{
			nError = ERR_ROTATE_ALARM;
			LOG_JIG("0도 Rotate Alarm");
	}

	// limit sensor check
	if(nError==NO_ERR)
	{
		if(Is0Pos() == true)
		{

			if((Is90Pos() == true)||(Is180Pos() == true)||(Is270Pos() == true))

				nError=ERR_ROTATE_LIMIT_ON_ERR;
		}

		else if((Is90Pos() == false)&&(Is180Pos() == false)&&(Is270Pos() == false))
			 nError=ERR_ROTATE_LIMIT_OFF_ERR;
		else
		{
			nError = m_pHWCommon->SetBit(OUT_ROTATE_0, ON);
			if(nError) nError = ERR_SEND_ROTATE_0_COMMAND;
		}
	}

	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Rotate_90_Manual(void)
{
	int nError=0;

	if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;
	if(nError==NO_ERR) nError = Enable();
	if(nError==NO_ERR)
	{
		nError = Rotate_IO_Init();
		if(nError == NO_ERR) nError = Alarm_Check_Reset();
		if(nError == NO_ERR) nError = m_pHWCommon->SetBit(OUT_ROTATE_90, ON);
		if(nError) nError = ERR_SEND_ROTATE_90_COMMAND;
		else
		{
			dwStartTime  = timeGetTime();
			nSendCommand = ROTATE_90_COMMAND;
			while((nError = CheckMotionComplete()) == NEXT_CHECK)
			{
				Sleep(10);
			}
		}
	}

	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Rotate_90_Auto(void)
{
	int nError=0;
#if((OHT_NAME == OHT_NAME_MAGAZINE) || (OHT_NAME == OHT_NAME_CASSETTE) ||   		\
	(OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD) ||        \
	(OHT_NAME == OHT_NAME_CASSETTE_2WD) || (OHT_NAME == OHT_NAME_NEO_SSDTRAY) ||    \
	(OHT_NAME == OHT_NAME_MODULETRAY))

    if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;

  	if(nError==NO_ERR) nError = Enable();
	if(nError==NO_ERR) nError = Rotate_IO_Init();
	//if(nError==NO_ERR) nError = Alarm_Check_Reset();

	// alarm check
	if(nError==NO_ERR)
	{
		if(IsAlarm() == true)
		{
			nError = Origin_forRotateAuto();
			Sleep(200);
		}
	}

	// limit sensor check
	if(nError==NO_ERR)
	{
		if(Is90Pos() == true)
		{
	#if((OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD)	\
	|| (OHT_NAME == OHT_NAME_NEO_SSDTRAY))
			if((Is0Pos() == true) || (Is18Pos() == true) || (Is180Pos() == true) ||(Is270Pos() == true))
#else
			if((Is0Pos() == true)|| (Is180Pos() == true)||(Is270Pos() == true))
#endif
				nError=ERR_ROTATE_LIMIT_ON_ERR;
		}
	#if((OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD)	\
	|| (OHT_NAME == OHT_NAME_NEO_SSDTRAY))
		else if((Is0Pos() == false)  && (Is18Pos() == false) && (Is180Pos() == false)&&(Is270Pos() == false))
#else
		else if((Is0Pos() == false) && (Is180Pos() == false)&&(Is270Pos() == false))
#endif
			nError=ERR_ROTATE_LIMIT_OFF_ERR;
		else
		{
			nError = m_pHWCommon->SetBit(OUT_ROTATE_90, ON);
			if(nError) nError = ERR_SEND_ROTATE_90_COMMAND;
		}
	}

	if(nError==NO_ERR)
	{
		dwStartTime  = timeGetTime();
		nSendCommand = ROTATE_90_COMMAND;
		while((nError = CheckMotionComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
#endif
	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Send_Rotate_90_AutoCmd(void)
{
	int nError=0;

	if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;

	if(nError==NO_ERR) nError = Enable();

	if(nError==NO_ERR) nError = Rotate_IO_Init();


	// alarm check
	if(IsAlarm() == true)
	{
			nError = ERR_ROTATE_ALARM;
			LOG_JIG("90도 Rotate Alarm");
	}

	// limit sensor check
	if(nError==NO_ERR)
	{
		if(Is90Pos() == true)
		{
			if((Is0Pos() == true)|| (Is180Pos() == true)||(Is270Pos() == true))
				nError=ERR_ROTATE_LIMIT_ON_ERR;
		}

		else if((Is0Pos() == false) && (Is180Pos() == false)&&(Is270Pos() == false))
		{

			nError=ERR_ROTATE_LIMIT_OFF_ERR;
		}
		else
		{
			nError = m_pHWCommon->SetBit(OUT_ROTATE_90, ON);

			if(nError) nError = ERR_SEND_ROTATE_90_COMMAND;

		}
	}

	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Rotate_180_Manual(void)
{
	int nError=0;

	if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;
	if(nError==NO_ERR) nError = Enable();
	if(nError==NO_ERR)
	{
		nError = Rotate_IO_Init();
		if(nError == NO_ERR) nError = Alarm_Check_Reset();
		if(nError == NO_ERR) nError = m_pHWCommon->SetBit(OUT_ROTATE_180, ON);
		if(nError) nError = ERR_SEND_ROTATE_180_COMMAND;
		else
		{
			dwStartTime  = timeGetTime();
			nSendCommand = ROTATE_180_COMMAND;
			while((nError = CheckMotionComplete()) == NEXT_CHECK)
			{
				Sleep(10);
			}
		}
	}
	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Rotate_180_Auto(void)
{
	int nError=0;
#if((OHT_NAME == OHT_NAME_MAGAZINE) || (OHT_NAME == OHT_NAME_CASSETTE) ||   		\
	(OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD) ||        \
	(OHT_NAME == OHT_NAME_CASSETTE_2WD) || (OHT_NAME == OHT_NAME_NEO_SSDTRAY) ||    \
	(OHT_NAME == OHT_NAME_MODULETRAY))
    if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;

    if(nError==NO_ERR) nError = Enable();
	if(nError==NO_ERR) nError = Rotate_IO_Init();
	//if(nError==NO_ERR) nError = Alarm_Check_Reset();

	// alarm check
	if(nError==NO_ERR)
	{
		if(IsAlarm() == true)
	    {
			 nError = Origin_forRotateAuto();
			 Sleep(200);
		}
	}

	// limit sensor check
	if(nError==NO_ERR)
	{
		if(Is180Pos() == true)
		{
	#if((OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD)	\
	 || (OHT_NAME == OHT_NAME_NEO_SSDTRAY))
			if((Is0Pos() == true) || (Is18Pos() == true)  || (Is90Pos() == true) || (Is270Pos() == true))
#else
			if((Is0Pos() == true) || (Is90Pos() == true) || (Is270Pos() == true))
#endif
				nError=ERR_ROTATE_LIMIT_ON_ERR;
		}
	#if((OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD)	\
	 || (OHT_NAME == OHT_NAME_NEO_SSDTRAY))
		else if((Is0Pos() == false)&& (Is18Pos() == false)  && (Is90Pos() == false) && (Is270Pos() == false))
#else
		else if((Is0Pos() == false) && (Is90Pos() == false) && (Is270Pos() == false))
#endif
			nError=ERR_ROTATE_LIMIT_OFF_ERR;
		else
		{
			nError = m_pHWCommon->SetBit(OUT_ROTATE_180,ON);
			if(nError) nError = ERR_SEND_ROTATE_180_COMMAND;
		}
	}

	if(nError==NO_ERR)
	{
		dwStartTime  = timeGetTime();
		nSendCommand = ROTATE_180_COMMAND;
		while((nError = CheckMotionComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
#endif
	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Send_Rotate_180_AutoCmd(void)
{
	int nError=0;

    if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;

    if(nError==NO_ERR) nError = Enable();
	if(nError==NO_ERR) nError = Rotate_IO_Init();

	// alarm check
	if(IsAlarm() == true)
	{

			nError = ERR_ROTATE_ALARM;
			LOG_JIG("180도 Rotate Alarm");

	}

	// limit sensor check
	if(nError==NO_ERR)
	{
		if(Is180Pos() == true)
		{
			if((Is0Pos() == true) || (Is90Pos() == true) || (Is270Pos() == true))
				nError=ERR_ROTATE_LIMIT_ON_ERR;
		}

		else if((Is0Pos() == false) && (Is90Pos() == false) && (Is270Pos() == false))
			nError=ERR_ROTATE_LIMIT_OFF_ERR;
		else
		{
			nError = m_pHWCommon->SetBit(OUT_ROTATE_180,ON);
			if(nError) nError = ERR_SEND_ROTATE_180_COMMAND;
		}
	}

	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Rotate_270_Manual(void)
{
	int nError=0;

	if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;
	if(nError==NO_ERR) nError = Enable();

	if(nError==NO_ERR)
	{
		nError = Rotate_IO_Init();
		if(nError == NO_ERR) nError = Alarm_Check_Reset();
		if(nError == NO_ERR) nError =  m_pHWCommon->SetBit(OUT_ROTATE_270, ON);
		if(nError) nError = ERR_SEND_ROTATE_270_COMMAND;
		else
		{
			dwStartTime  = timeGetTime();
			nSendCommand = ROTATE_270_COMMAND;
			while((nError = CheckMotionComplete()) == NEXT_CHECK)
			{
			   //Application->ProcessMessages();
			  Sleep(10);
			}
		}
	}
	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Rotate_270_Auto(void)
{
	int nError=0;
#if((OHT_NAME == OHT_NAME_MAGAZINE) || (OHT_NAME == OHT_NAME_CASSETTE) ||   		\
	(OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD) ||        \
	(OHT_NAME == OHT_NAME_CASSETTE_2WD) || (OHT_NAME == OHT_NAME_NEO_SSDTRAY) ||    \
	(OHT_NAME == OHT_NAME_MODULETRAY))
	if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;
	if(nError==NO_ERR) nError = Enable();
	if(nError==NO_ERR) nError = Rotate_IO_Init();

	//if(nError==NO_ERR) nError = Alarm_Check_Reset();

	// alarm check
	if(nError==NO_ERR)
	{
		if(IsAlarm() == true)
		{
			 nError = Origin_forRotateAuto();
			 Sleep(200);
		}
	}

	// limit sensor check
	if(nError==NO_ERR)
	{
		if(Is270Pos() == true)
		{
	#if((OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD)	\
	 || (OHT_NAME == OHT_NAME_NEO_SSDTRAY))
			if((Is0Pos() == true) || (Is18Pos() == true) || (Is90Pos() == true) || (Is180Pos() == true))
			#else
			if((Is0Pos() == true) || (Is90Pos() == true) || (Is180Pos() == true))
			#endif
				nError=ERR_ROTATE_LIMIT_ON_ERR;
		}
	#if((OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD)	\
	 || (OHT_NAME == OHT_NAME_NEO_SSDTRAY))
		else if((Is0Pos() == false) && (Is18Pos() == false)  && (Is90Pos() == false) && (Is180Pos() == false))
#else
		else if((Is0Pos() == false) && (Is90Pos() == false) && (Is180Pos() == false))
#endif
			nError=ERR_ROTATE_LIMIT_OFF_ERR;
		else
		{
			nError = nError =  m_pHWCommon->SetBit(OUT_ROTATE_270, ON);
		    if(nError) nError = ERR_SEND_ROTATE_270_COMMAND;
		}
	}

	if(nError==NO_ERR)
	{
		dwStartTime  = timeGetTime();
		nSendCommand = ROTATE_270_COMMAND;
		while((nError = CheckMotionComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
#endif
	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Send_Rotate_270_AutoCmd(void)
{
	int nError=0;

	if(!bReadyOperation) nError = ERR_ROTATE_NOTORIGIN;
	if(nError==NO_ERR) nError = Enable();
	if(nError==NO_ERR) nError = Rotate_IO_Init();


	// alarm check
	if(IsAlarm() == true)
	{
			nError = ERR_ROTATE_ALARM;
			LOG_JIG("270도 Rotate Alarm");
	}

	// limit sensor check
	if(nError==NO_ERR)
	{
		if(Is270Pos() == true)
		{
			if((Is0Pos() == true) || (Is90Pos() == true) || (Is180Pos() == true))
				nError=ERR_ROTATE_LIMIT_ON_ERR;
		}

		else if((Is0Pos() == false) && (Is90Pos() == false) && (Is180Pos() == false))

			nError=ERR_ROTATE_LIMIT_OFF_ERR;
		else
		{
			nError =  m_pHWCommon->SetBit(OUT_ROTATE_270, ON);
			if(nError) nError = ERR_SEND_ROTATE_270_COMMAND;
		}
	}

	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Origin_forRotateAuto(void)
{
	int nError=0;

	bReadyOperation = false;
    nError = Alarm_Check_Reset();

	if(nError == NO_ERR) nError = m_pHWCommon->SetBit(OUT_ROTATE_ORIGIN, ON);
	Sleep(500);
	if(nError) nError = ERR_SEND_ROTATE_ORIGIN_COMMAND;
//	else                      // Sleep을 넣어도 다른 쓰레드가 안죽는지 확인 필요 timy
//	{
//		dwStartTime  = timeGetTime();
//		nSendCommand = ROTATE_ORIGIN_COMMAND;
//		while((nError = CheckMotionComplete()) == NEXT_CHECK)
//		{
//			Sleep(10);
//		}
//	}
	if(nError==NO_ERR) bReadyOperation = true;

	return nError;

}


/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Origin(void)
{
	int nError=0;

	bReadyOperation = false;
	if(nError==NO_ERR) nError = Enable();
	if(nError==NO_ERR) nError = Rotate_IO_Init();
	if(nError == NO_ERR) nError = Alarm_Check_Reset();
	if(nError == NO_ERR) nError = m_pHWCommon->SetBit(OUT_ROTATE_ORIGIN, ON);

	if((nError == NO_ERR)&&(Is0Pos() == true)) Sleep(500);

	if(nError) nError = ERR_SEND_ROTATE_ORIGIN_COMMAND;
//	else                         // Sleep을 넣어도 다른 쓰레드가 안죽는지 확인 필요 timy
//	{
//		dwStartTime = timeGetTime();
//		nSendCommand = ROTATE_ORIGIN_COMMAND;
//		while((nError = CheckMotionComplete()) == NEXT_CHECK)
//		{
//			Sleep(10);
//		}
	//if(nError == NO_ERR && bReadyOperation == false) bReadyOperation = true;
//	}
	if(nError==NO_ERR) bReadyOperation = true;

	  return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::CheckMotionComplete(void)
{
#if((OHT_NAME == OHT_NAME_MAGAZINE) || (OHT_NAME == OHT_NAME_CASSETTE) ||   		\
	(OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD) ||        \
	(OHT_NAME == OHT_NAME_CASSETTE_2WD) || (OHT_NAME == OHT_NAME_NEO_SSDTRAY) ||    \
	(OHT_NAME == OHT_NAME_MODULETRAY))
	//if((timeGetTime() - dwStartTime) >= ((nSendCommand == ROTATE_ORIGIN_COMMAND) ? dwOriginLimitTime : dwMotionLimitTime))

	if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= ((nSendCommand == ROTATE_ORIGIN_COMMAND) ? dwOriginLimitTime : dwMotionLimitTime))
    {
		switch(nSendCommand)
		{
			case ROTATE_0_COMMAND:
				Rotate_IO_Init();
				return ERR_ROTATE_0_TIMEOVER;

			case ROTATE_90_COMMAND:
				Rotate_IO_Init();
				return ERR_ROTATE_90_TIMEOVER;

			case ROTATE_180_COMMAND:
				Rotate_IO_Init();
				return ERR_ROTATE_180_TIMEOVER;

			case ROTATE_270_COMMAND:
				Rotate_IO_Init();
				return ERR_ROTATE_270_TIMEOVER;

			 case ROTATE_ORIGIN_COMMAND:
				Rotate_IO_Init();
				return ERR_ROTATE_ORIGIN_TIMEOVER;
		  }
	}

	if(m_pHWCommon->m_Input.ROTATE_Alarm == ON)
	{
		Rotate_IO_Init();
		return ERR_ROTATE_ALARM;
	}

	switch(nSendCommand)
	{

		case ROTATE_90_COMMAND:
			if((Is90Pos() == true) &&(IsInposition() == true))
			{
				Rotate_IO_Init();
				return NO_ERR;
			}
			break;

		case ROTATE_180_COMMAND:
			if((Is180Pos() == true) &&(IsInposition() == true))
			{
				Rotate_IO_Init();
				return NO_ERR;
			}
			break;

		case ROTATE_270_COMMAND:
			if((Is270Pos() == true) &&(IsInposition() == true))
			{
				Rotate_IO_Init();
				return NO_ERR;
			}
			break;

		case ROTATE_ORIGIN_COMMAND:
			#if(OHT_NAME == OHT_NAME_MODULETRAY)
			if((Is90Pos() == true) &&(IsInposition() == true))
			{
				Rotate_IO_Init();
				return NO_ERR;
			}
			break;
			#endif

		case ROTATE_0_COMMAND:
			if((Is0Pos() == true) &&(IsInposition() == true))
			{
				Rotate_IO_Init();
				return NO_ERR;
			}
			break;
	}
#endif
	return NEXT_CHECK;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::ReadyToOperation(void)
{
	if ((bReadyOperation == true) && (IsInposition() == true))
		return NO_ERR;
	else
		return ERR_ROTATE_NOTORIGIN;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Rotate_IO_Init(void)
{
	int nError=0;

	nError = m_pHWCommon->SetBit(OUT_ROTATE_0, OFF);
	if(nError==NO_ERR) nError = m_pHWCommon->SetBit(OUT_ROTATE_90, OFF);
	if(nError==NO_ERR) nError = m_pHWCommon->SetBit(OUT_ROTATE_180, OFF);
	if(nError==NO_ERR) nError = m_pHWCommon->SetBit(OUT_ROTATE_270, OFF);
	if(nError==NO_ERR) nError = m_pHWCommon->SetBit(OUT_ROTATE_ORIGIN, OFF);
	if(nError==NO_ERR) nError = m_pHWCommon->SetBit(OUT_ROTATE_ALARM_RESET, OFF);
	if(nError==NO_ERR) nError = m_pHWCommon->SetBit(OUT_ROTATE_FREE, OFF);

	return nError;

}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Alarm_Check_Reset(void)
{
	int nError=0;

	if(IsAlarm() == true)
	{
		nError = m_pHWCommon->SetBit(OUT_ROTATE_ALARM_RESET, ON);
		if(nError == NO_ERR)
		{
			Sleep(500);
			nError = m_pHWCommon->SetBit(OUT_ROTATE_ALARM_RESET, OFF);
		}
		Sleep(100);
	}

	return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Free(void)
{
	int nError=0;

	bReadyOperation = false;
	nError = m_pHWCommon->SetBit(OUT_ROTATE_FREE, ON);

	return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Enable(void)
{
	int nError=0;

	if(IsFree() == true) nError = m_pHWCommon->SetBit(OUT_ROTATE_FREE, OFF);

	return nError;

}

/**
@brief   Rotate가 0 위치인지를 확인하는 함수
@author  LSJ
@date    2014.06.18
@return  Rotate가 0도 Position 신호가 On이면 true, Off면 False 값 리턴
*/
bool Rotate::Is0Pos()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.ROTATE_0 == ON)
		bResult = true;
	else
		bResult = false;


	return bResult;
}

/**
@brief   Rotate가 90 위치인지를 확인하는 함수
@author  LSJ
@date    2014.06.18
@return  Rotate가 90도 Position 신호가 On이면 true, Off면 False 값 리턴
*/
bool Rotate::Is90Pos()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.ROTATE_90 == ON)
		bResult = true;
	else
		bResult = false;


	return bResult;
}

/**
@brief   Rotate가 180 위치인지를 확인하는 함수
@author  LSJ
@date    2014.06.18
@return  Rotate가 180도 Position 신호가 On이면 true, Off면 False 값 리턴
*/
bool Rotate::Is180Pos()
{
	bool bResult = false;


	if (m_pHWCommon->m_Input.ROTATE_180 == ON)
		bResult = true;
	else
		bResult = false;


	return bResult;
}

/**
@brief   Rotate가 270 위치인지를 확인하는 함수
@author  LSJ
@date    2014.06.18
@return  Rotate가 270도 Position 신호가 On이면 true, Off면 False 값 리턴
*/
bool Rotate::Is270Pos()
{
	bool bResult = false;


	if (m_pHWCommon->m_Input.ROTATE_270 == ON)
		bResult = true;
	else
		bResult = false;


	return bResult;
}

/**
@brief   Rotate Inposition 신호가 On인지를 확인하는 함수
@author  LSJ
@date    2014.06.18
@return  Rotate Inposition 신호가 On이면 true, Off면 False 값 리턴
*/
bool Rotate::IsInposition()
{
	bool bResult = false;


	if (m_pHWCommon->m_Input.ROTATE_Inposition == ON)
		bResult = true;
	else
		bResult = false;


	return bResult;
}

/**
@brief   Rotate Alarm 신호가 On인지를 확인하는 함수
@author  LSJ
@date    2014.06.18
@return  Rotate Alarm 신호가 On이면 true, Off면 False 값 리턴
*/
bool Rotate::IsAlarm()
{
	bool bResult = false;


	if (m_pHWCommon->m_Input.ROTATE_Alarm == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Rotate가 Free 상태인지를 확인하는 함수
@author  LSJ
@date    2014.06.18
@return  Rotate Free 출력이 On이면 true, Off면 False 값 리턴
*/
bool Rotate::IsFree()
{
	bool bResult = false;


	if (m_pHWCommon->m_Output.ROTATE_Free == ON)
		bResult = true;
	else
		bResult = false;


	return bResult;
}

/**
@brief   Rotate가 18 위치인지를 확인하는 함수
@author  LSJ
@date    2014.06.18
@return  Rotate가 18도 Position 신호가 On이면 true, Off면 False 값 리턴
*/
bool Rotate::Is18Pos()
{
	bool bResult = false;

#if((OHT_NAME == OHT_NAME_NEO_MAGAZINE) || (OHT_NAME == OHT_NAME_NEO_SSD))
	if (m_pHWCommon->m_Input.ROTATE_18 == ON)
		bResult = true;
	else
		bResult = false;
#endif
	return bResult;
}






