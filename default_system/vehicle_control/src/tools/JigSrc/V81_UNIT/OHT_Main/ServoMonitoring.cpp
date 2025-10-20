//---------------------------------------------------------------------------
/**
@file    ServoMonitoring.cpp
@version OHT 7.0
@brief   ServoMonitoring Class CPP File
*/

#pragma hdrstop

#include "ServoMonitoring.h"
#include "OHTMain.h"

#define ADD_ERR_LOG(...)    WRITE_LOG_UNIT(pLogServoMonitoring, __VA_ARGS__)


/**
@brief   ServoMonitoring 생성자
@author  zzang9un
@date    2012.10.13
@param   pHWCommon : HWCommon Class의 포인터
@note    Comm Handler을 사용하기 위해 HWCommon의 Pointer를 가져옴
*/
ServoMonitoring::ServoMonitoring(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;

	m_wAddress			= 0;
	m_dwErrCode			= 0;
	m_lReadData         = 0;
	m_wMonitorPara 		= 0;
	ReUpdate			= false;
	m_cCOMPort = new Comm_Port(COM_SERVOSETUP, 19200, DATABIT_8, PARITYBIT_EVEN, STOPBIT_1);

	pLogServoMonitoring = getLogUnit("D:\\log\\AbnormalError.log",MAX_BYTE_SIZE_OF_MAIN_LOG);

	if(!IsCommOpen())
	{
		delete m_cCOMPort;
		m_cCOMPort = NULL;
	}else
	{
		SetInnerLimitTorque_90();
    }


	m_IsSetTorqueFront = 0x00;
	m_IsSetTorqueFront_2 = 0x00;
	m_IsSetTorqueRear = 0x00;
	m_IsSetTorqueRear_2 = 0x00;

	m_IsSetTorqueHoist = 0x00;
	m_IsSetTorqueShift = 0x00;

	SetFn009Step(SERVOPACK_FN009_STEP_INIT);

	m_Fn009FrontFlag = false;
	m_Fn009RearFlag = false;

//	SetFn009(1);
//	SetFn009(2);

}



/**
@brief   ServoMonitoring 소멸자
@author  zzang9un
@date    2012.10.13
*/
ServoMonitoring::~ServoMonitoring()
{

	m_wAddress			= 0;
	m_dwErrCode			= 0;
	m_lReadData         = 0;
//	SetInnerLimitTorque_90();

	if(m_cCOMPort!=NULL)
	{
		delete m_cCOMPort;
		m_cCOMPort			= NULL;
	}

}

DWORD ServoMonitoring::ReadMonitorData(WORD wAddress, WORD wMonitorPara, long* lpReadData)
{
	WORD	wRegNum = 0;
	static DWORD	dwErrCode = 0, dwReadData = 0;
	BOOL	bIsLongData = FALSE;

	if( wMonitorPara == Deviation_Counter || wMonitorPara == Reference_Pulse_Counter || wMonitorPara == Feedback_Pulse_Counter ||
		wMonitorPara == Fully_Closed_Feedback_Pulse_Counter || wMonitorPara == Rotate_Angle1 )
	{
		bIsLongData = TRUE;
		wRegNum = 2;
	}
	else
		wRegNum = 1;

	dwErrCode = SigmaCom((BYTE)wAddress, FUNC_CODE, REGISTER_READ, wMonitorPara, wRegNum, &dwReadData);
	if(dwErrCode != SCOM_RT_SUCCESS) return dwErrCode;

	if(lpReadData != NULL)
	{
		if(bIsLongData == TRUE)
			*lpReadData = (int)dwReadData;
		else
			*lpReadData = (short)dwReadData;
	}

	return SCOM_RT_SUCCESS;
}
//-----------------------------------------------------------------------------
//레지스터를 읽을때 사용함.

DWORD ServoMonitoring::SigmaCom(char chAddress, char chFuncCode, WORD wFuncSubCode, WORD wRegister, WORD wRegNum, DWORD* dwpReadData)
{
	static	DWORD	dwSIGMATrigerTime	= timeGetTime();     //함수 진입 시간.
	static	bool	bSIGMAOnTriger	= false;
	static  int		nErrorCount	= 0;
	int i = 0;
	int nError = 0;
	DWORD		dwStartTime = timeGetTime();

	char	szRecvBuff[16] = {0};
	char	szSendBuff[16] = {0};
	WORD	wReadDataCnt = 0, wCRCData = 0, wRecvDataLength = 0;
	DWORD	dwErrCode = 0, dwReadData = 0;

	int nRtnByteNo =0;
	int nSedByteNo = 0;
	DWORD	dwTimeOut = 0;

	if(m_cCOMPort == NULL) return SCOM_RT_COM_OPEN_FAIL;    //port open error

	szSendBuff[0]	= chAddress;
	szSendBuff[1]	= chFuncCode;
	szSendBuff[2]	= wFuncSubCode >> 8;
	szSendBuff[3]	= wFuncSubCode & 0xFF;
	szSendBuff[4]	= wRegister >> 8;
	szSendBuff[5]	= wRegister & 0xFF;
	szSendBuff[6]	= wRegNum >> 8;
	szSendBuff[7]	= wRegNum & 0xFF;

	wCRCData = CRCCheck(SEND_PK_LENGTH - 2, szSendBuff);

	szSendBuff[8]	= wCRCData & 0xFF;
	szSendBuff[9]	= wCRCData >> 8;
	
      for(i = 0; i < RETRY_MAX_COUNT; i++)
	{
		memset(szRecvBuff, 0, 16);

		// puring 동기형식으로 데이터 한번에 쓰기
           //2. 해당 병령의 경우 데이터 길이가 차이있음
		if( wRegister == Deviation_Counter || wRegister == Reference_Pulse_Counter || wRegister == Feedback_Pulse_Counter ||
			wRegister == Fully_Closed_Feedback_Pulse_Counter || wRegister == Rotate_Angle1 )
		{
			wRecvDataLength = RECV_PK_LENGTH2;
		}
		else
		{
			wRecvDataLength = RECV_PK_LENGTH;
		}

		m_cCOMPort->WriteCommBlock(szSendBuff,SEND_PK_LENGTH);


		if(nError==0)
		{
			 dwTimeOut = ::GetTickCount();

			 while(1)
			{

				nRtnByteNo = m_cCOMPort->ReadCommBlock(szRecvBuff, wRecvDataLength);

				if(nRtnByteNo != 0) break;
				if(::GetTickCount() - dwTimeOut > 1000)
				{
					nError = SCOM_RT_RCV_TIMEOUT_FAIL; //timeout error
					break;
				}
				Sleep(5);
			}
		 }

    	if(szRecvBuff[0] != chAddress)						{	dwErrCode = SCOM_RT_RECEIVE_INVALID_DATA;	continue;	}
		if(szRecvBuff[1] != chFuncCode)						{	dwErrCode = SCOM_RT_RECEIVE_INVALID_DATA;	continue;	}
		if(szRecvBuff[2] != ((char)(wFuncSubCode >> 8)))	{	dwErrCode = SCOM_RT_RECEIVE_INVALID_DATA;	continue;	}
		if(szRecvBuff[3] != ((char)(wFuncSubCode & 0xFF)))	{	dwErrCode = SCOM_RT_RECEIVE_INVALID_DATA;	continue;	}

		wCRCData = CRCCheck(wRecvDataLength - 2, szRecvBuff);

		if(szRecvBuff[wRecvDataLength - 2] != ((char)(wCRCData & 0xFF)))	{	dwErrCode = SCOM_RT_RECEIVE_INVALID_DATA;	continue;	}
		if(szRecvBuff[wRecvDataLength - 1] != ((char)(wCRCData >> 8)))		{	dwErrCode = SCOM_RT_RECEIVE_INVALID_DATA;	continue;	}

		break;
    }

    if(i >= RETRY_MAX_COUNT) return SCOM_RT_COM_RETRY_ABOVE_MAX_COUNT;

	wReadDataCnt = (szRecvBuff[4] << 8) | szRecvBuff[5];

	if(wReadDataCnt == 4)
	{
		dwReadData = (0xFF000000 & (szRecvBuff[8] << 24)) | (0x00FF0000 & (szRecvBuff[9] << 16)) |
					 (0x0000FF00 & (szRecvBuff[6] << 8))  | (0x000000FF & szRecvBuff[7]);
	}
	else
	{
		dwReadData = (0x0000FFFF & (szRecvBuff[6] << 8)) | (0x000000FF & szRecvBuff[7]);
	}

	if(dwpReadData != NULL)
		*dwpReadData = dwReadData;

	return SCOM_RT_SUCCESS;

}

DWORD ServoMonitoring::SigmaComWrite(char chAddress, char chFuncCode, WORD wFuncSubCode, WORD wRegister, WORD wWriteData)
{


	int		i = 0, j = 0;
	char	chErrCode = 0;
	int 	nError = 0;
	char	szRecvBuff[16] = {0};
	char	szSendBuff[16] = {0};
	WORD	wReadDataCnt = 0, wCRCData = 0, wRxDataLength = 0;
	DWORD	dwErrCode = 0, dwReadData = 0;
  //	char*	strData;
	int nRtnByteNo =0;
	int nSedByteNo = 0;
	DWORD	dwTimeOut = 0;

	AnsiString strTemp;

	if(m_cCOMPort == NULL) return SCOM_RT_COM_OPEN_FAIL;    //port open error

	szSendBuff[0]	= chAddress;
	szSendBuff[1]	= chFuncCode;
	szSendBuff[2]	= wFuncSubCode >> 8;
	szSendBuff[3]	= wFuncSubCode & 0xFF;
	szSendBuff[4]	= wRegister >> 8;
	szSendBuff[5]	= wRegister & 0xFF;
	szSendBuff[6]	= wWriteData >> 8;
	szSendBuff[7]	= wWriteData & 0xFF;

	wCRCData = CRCCheck(SEND_PK_LENGTH - 2, szSendBuff);

	szSendBuff[8]	= wCRCData & 0xFF;
	szSendBuff[9]	= wCRCData >> 8;

	nSedByteNo = Write(szSendBuff,SEND_PK_LENGTH);
	if(nSedByteNo !=0)	{ return SCOM_RT_WRITE_FAIL;  	}

	Sleep(200); // 200ms 인 경우 COM_RT_COM_RECEIVE_FAIL error 발생

	nRtnByteNo = Read(szRecvBuff, RECV_PK_LENGTH);
	if(nRtnByteNo == 0)			return SCOM_RT_COM_RECEIVE_FAIL;
	else
	{

	  //	Form1->Memo1->Lines->Add("1");
		if(szRecvBuff[0] != chAddress)						{	return SCOM_RT_RECEIVE_INVALID_DATA; }
	  //	Form1->Memo1->Lines->Add("2");
		if(szRecvBuff[1] != chFuncCode)						{	return SCOM_RT_RECEIVE_INVALID_DATA; }
	  //	Form1->Memo1->Lines->Add("3");
		if(szRecvBuff[2] != ((char)(wFuncSubCode >> 8)))	{	return SCOM_RT_RECEIVE_INVALID_DATA; }
		if(szRecvBuff[3] != ((char)(wFuncSubCode & 0xFF)))	{	return SCOM_RT_RECEIVE_INVALID_DATA; }

		wCRCData = CRCCheck(RECV_PK_LENGTH - 2, szRecvBuff);

		if(szRecvBuff[RECV_PK_LENGTH - 2] != ((char)(wCRCData & 0xFF)))		{	return SCOM_RT_RECEIVE_INVALID_DATA; }
		if(szRecvBuff[RECV_PK_LENGTH - 1] != ((char)(wCRCData >> 8)))		{	return SCOM_RT_RECEIVE_INVALID_DATA; }

	}


	return SCOM_RT_SUCCESS;



}

DWORD ServoMonitoring::SigmaComWrite(char chAddress, char chFuncCode, WORD wFuncSubCode, WORD wRegister, WORD wWriteData, int sleep)
{


	int		i = 0, j = 0;
	char	chErrCode = 0;
	int 	nError = 0;
	char	szRecvBuff[16] = {0};
	char	szSendBuff[16] = {0};
	WORD	wReadDataCnt = 0, wCRCData = 0, wRxDataLength = 0;
	DWORD	dwErrCode = 0, dwReadData = 0;
  //	char*	strData;
	int nRtnByteNo =0;
	int nSedByteNo = 0;
	DWORD	dwTimeOut = 0;

	AnsiString strTemp;

	if(m_cCOMPort == NULL) return SCOM_RT_COM_OPEN_FAIL;    //port open error

	szSendBuff[0]	= chAddress;
	szSendBuff[1]	= chFuncCode;
	szSendBuff[2]	= wFuncSubCode >> 8;
	szSendBuff[3]	= wFuncSubCode & 0xFF;
	szSendBuff[4]	= wRegister >> 8;
	szSendBuff[5]	= wRegister & 0xFF;
	szSendBuff[6]	= wWriteData >> 8;
	szSendBuff[7]	= wWriteData & 0xFF;

	wCRCData = CRCCheck(SEND_PK_LENGTH - 2, szSendBuff);

	szSendBuff[8]	= wCRCData & 0xFF;
	szSendBuff[9]	= wCRCData >> 8;

	nSedByteNo = Write(szSendBuff,SEND_PK_LENGTH);
	if(nSedByteNo !=0)	{ return SCOM_RT_WRITE_FAIL;  	}

	Sleep(sleep); // 200ms 인 경우 COM_RT_COM_RECEIVE_FAIL error 발생

	nRtnByteNo = Read(szRecvBuff, RECV_PK_LENGTH);
	if(nRtnByteNo == 0)			return SCOM_RT_COM_RECEIVE_FAIL;
	else
	{

	  //	Form1->Memo1->Lines->Add("1");
		if(szRecvBuff[0] != chAddress)						{	return SCOM_RT_RECEIVE_INVALID_DATA; }
	  //	Form1->Memo1->Lines->Add("2");
		if(szRecvBuff[1] != chFuncCode)						{	return SCOM_RT_RECEIVE_INVALID_DATA; }
	  //	Form1->Memo1->Lines->Add("3");
		if(szRecvBuff[2] != ((char)(wFuncSubCode >> 8)))	{	return SCOM_RT_RECEIVE_INVALID_DATA; }
		if(szRecvBuff[3] != ((char)(wFuncSubCode & 0xFF)))	{	return SCOM_RT_RECEIVE_INVALID_DATA; }

		wCRCData = CRCCheck(RECV_PK_LENGTH - 2, szRecvBuff);

		if(szRecvBuff[RECV_PK_LENGTH - 2] != ((char)(wCRCData & 0xFF)))		{	return SCOM_RT_RECEIVE_INVALID_DATA; }
		if(szRecvBuff[RECV_PK_LENGTH - 1] != ((char)(wCRCData >> 8)))		{	return SCOM_RT_RECEIVE_INVALID_DATA; }

	}


	return SCOM_RT_SUCCESS;



}


//-----------------------------------------------------------------------------
WORD ServoMonitoring::GetMonitorParaValue(int nTempSelIndex)
{
	int		nSel = 0;
	WORD	wParaValue = 0;


		switch(nTempSelIndex)
		{
		case 0:
			wParaValue = Motor_Speed;		break;

		case 1:
			wParaValue = Speed_Reference;	break;

		case 2:
			wParaValue = Torque_Reference;	break;

		case 3:
			wParaValue = Rotate_Angle1;		break;

		case 4:
			wParaValue = Rotate_Angle2;		break;

		case 5:
			wParaValue = Input_Signal_Monitor;			break;

		case 6:
			wParaValue = Output_Signal_Monitor;			break;

		case 7:
			wParaValue = Input_Reference_Pulse_Speed;	break;

		case 8:
			wParaValue = Deviation_Counter;				break;

		case 9:
			wParaValue = Cumulative_Load;				break;

		case 10:
			wParaValue = Regenerative_Load;				break;

		case 11:
			wParaValue = DB_Resister_Consumption_Power;	break;

		case 12:
			wParaValue = Reference_Pulse_Counter;		break;

		case 13:
			wParaValue = Feedback_Pulse_Counter;		break;

		case 14:
			wParaValue = Fully_Closed_Feedback_Pulse_Counter;	break;

		case 15:
			wParaValue = Current_Alarm_status;		break;

		}

	return wParaValue;
}
//--------------------------------------------------------------------------
WORD ServoMonitoring::CRCCheck(int nDataNum, char* szCheckBuff)
{
	WORD    wCalcValue = 0xFFFF, wBuff = 0;
	LONG	i = 0, j = 0;

	for(i = 0; i < nDataNum; i++)
	{
		wBuff = szCheckBuff[i] & 0x00FF;
		wCalcValue ^= wBuff;

		for(j = 0; j < CRC_SHIFT_COUNT; j++)
		{
			if((wCalcValue & 0x01) == 0)
			{
				wCalcValue	>>= 1;
				continue;
			}
			else
			{
				wCalcValue	>>= 1;
				wCalcValue  ^= 0xA001;
			}
		}
	}

	return wCalcValue;
}
//-------------------------------------------------------------------------
bool ServoMonitoring::SetParaValue(WORD wTempAddress, int nTempSelIndex)
{
	 //크리티컬섹션 시작

	 m_wAddress = wTempAddress;
	 m_wMonitorPara = GetMonitorParaValue(nTempSelIndex);
    //크리티컬센션 종료

}
//---------------------------------------------------------------------------
bool ServoMonitoring::GetRcvdData(DWORD* dwTempErrcode,long* lTempReadData)
{
	*dwTempErrcode =   m_dwErrCode;
	*lTempReadData =   m_lReadData;
}
//-------------------------------------------------------------------------
bool ServoMonitoring::IsCommOpen()
{
	bool bCheck= false;
	if(m_cCOMPort!=NULL)
	{
		if(m_cCOMPort->m_hCOM != INVALID_HANDLE_VALUE)
			bCheck = true;
	}
	return bCheck;
}

/**
@brief   시리얼에 명령을 보내는 함수
@date    2013.03.12
@param   chSndData : Write할 문자
@return  정상인 경우 0, 에러 발생한 경우 Error Code 리턴
*/
int ServoMonitoring::Write(char * chSndData, int Len)
{
   if(m_cCOMPort == NULL) return SCOM_RT_COM_OPEN_FAIL;
   PurgeComm(m_cCOMPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);

   if( Len == (int)(m_cCOMPort->WriteCommBlock(chSndData,Len)))
	  return NO_ERROR;
   else
	  return SCOM_RT_WRITE_FAIL;
}

/**
@brief   Buff에 읽기위한 함수
@date    2013.03.14
@param    buff, StopTarget
@return  길이 반환
*/
int ServoMonitoring::Read(char *RtnReadBuff, int Len)
{
   int i, index = 0;
   char chBuff;


	for(i=0; i<255; i++)
   {
		if(m_cCOMPort == NULL)
		{
			index=0;
			break;
		}
		if(m_cCOMPort->ReadCommBlock(&chBuff,1)==0)  // One Byte Reading
		{
			if(m_cCOMPort != NULL) PurgeComm(m_cCOMPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
		   //	index=0;
			break;
		}
		*(RtnReadBuff + index ) = chBuff;
		 index++;
		 if(index == Len) break;
	}
	return index;
}

bool ServoMonitoring::SetInnerLimitTorque_0()      //Test용
{
	bool bcheck = true;
	WORD	wParaData = 0;//200;
	DWORD	dwErrCode = NO_ERR;

	//1 축 Driving
	if(m_IsSetTorqueFront != 0x03)
	{
		dwErrCode = SigmaComWrite((BYTE)1, FUNC_CODE, REGISTER_WRITE, ForwardTorqe, wParaData);
		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueFront = 0x03;
		else return bcheck;


	}
	if(m_IsSetTorqueFront_2 != 0x03)
	{
		dwErrCode = SigmaComWrite((BYTE)1, FUNC_CODE, REGISTER_WRITE, BackwardTorqe, wParaData);
		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueFront_2 = 0x03;
		else return bcheck;
    }


	//2 축 Driving
	if(m_IsSetTorqueRear != 0x03)
	{
		dwErrCode = SigmaComWrite((BYTE)2, FUNC_CODE, REGISTER_WRITE, ForwardTorqe, wParaData);
		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueRear = 0x03;
		else return bcheck;

	}
	if(m_IsSetTorqueRear_2 != 0x03)
	{
		dwErrCode = SigmaComWrite((BYTE)2, FUNC_CODE, REGISTER_WRITE, BackwardTorqe, wParaData);
		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueRear_2 = 0x03;
		else return bcheck;
	}

	return bcheck;
}

bool ServoMonitoring::SetInnerLimitTorque_90()
{
#if(OHT_NAME == OHT_NAME_STD_V81S)
	int FRONT_SERVOPACK = OHTMainForm->Edit_frontName->Text.ToInt();
	int REAR_SERVOPACK = OHTMainForm->Edit_rearName->Text.ToInt();
#else
	int FRONT_SERVOPACK = 1;
	int REAR_SERVOPACK = 2;
#endif
	ADD_ERR_LOG("SetInnerLimitTorque_90 start");
	bool bcheck = true;
	WORD	wParaData = 90;//200;
	DWORD	dwErrCode = NO_ERR;

	ADD_ERR_LOG("para : %d, isSetFront : %d, isSetFront2 : %d", wParaData, m_IsSetTorqueFront, m_IsSetTorqueFront_2);

	//1 축 Driving
	if(m_IsSetTorqueFront != 0x01)
	{
		dwErrCode = SigmaComWrite((BYTE)FRONT_SERVOPACK, FUNC_CODE, REGISTER_WRITE, ForwardTorqe, wParaData);
		ADD_ERR_LOG("front foward error %d", dwErrCode);
//		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueFront = 0x01;
		else
		{
			m_IsSetTorqueFront = 0x00;
			ADD_ERR_LOG("[SetInnerLimitTorque_90_1] : %x , %d, %d", m_IsSetTorqueFront, bcheck);
			return bcheck;
		}


	}
	if(m_IsSetTorqueFront_2 != 0x01)
	{
		dwErrCode = SigmaComWrite((BYTE)FRONT_SERVOPACK, FUNC_CODE, REGISTER_WRITE, BackwardTorqe, wParaData);
		ADD_ERR_LOG("front backward error %d", dwErrCode);
//		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueFront_2 = 0x01;
		else
		{
			m_IsSetTorqueFront_2 = 0x00;
			ADD_ERR_LOG("[SetInnerLimitTorque_90_2] : %x , %d, %d", m_IsSetTorqueFront_2, bcheck);
			return bcheck;
		}
    }

	ADD_ERR_LOG("para : %d, isSetRear : %d, isSetRear2 : %d", wParaData, m_IsSetTorqueRear, m_IsSetTorqueRear_2);

	//2 축 Driving
	if(m_IsSetTorqueRear != 0x01)
	{
		dwErrCode = SigmaComWrite((BYTE)REAR_SERVOPACK, FUNC_CODE, REGISTER_WRITE, ForwardTorqe, wParaData);
		ADD_ERR_LOG("rear foward error %d", dwErrCode);
//		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueRear = 0x01;
		else
		{
			m_IsSetTorqueRear = 0x00;
			ADD_ERR_LOG("[SetInnerLimitTorque_90_3] : %x , %d, %d", m_IsSetTorqueRear, bcheck);
			return bcheck;
		}

	}
	if(m_IsSetTorqueRear_2 != 0x01)
	{
		dwErrCode = SigmaComWrite((BYTE)REAR_SERVOPACK, FUNC_CODE, REGISTER_WRITE, BackwardTorqe, wParaData);
		ADD_ERR_LOG("rear backward error %d", dwErrCode);
//		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueRear_2 = 0x01;
		else
		{
			m_IsSetTorqueRear_2 = 0x00;
			ADD_ERR_LOG("[SetInnerLimitTorque_90_4] : %x , %d, %d", m_IsSetTorqueRear_2, bcheck);
			return bcheck;
		}
	}
	ADD_ERR_LOG("SetInnerLimitTorque_90 finish\n");
	return bcheck;
}

bool ServoMonitoring::SetInnerLimitTorque_800()
{
#if(OHT_NAME == OHT_NAME_STD_V81S)
	int FRONT_SERVOPACK = OHTMainForm->Edit_frontName->Text.ToInt();
	int REAR_SERVOPACK = OHTMainForm->Edit_rearName->Text.ToInt();
#else
	int FRONT_SERVOPACK = 1;
	int REAR_SERVOPACK = 2;
#endif


	ADD_ERR_LOG("SetInnerLimitTorque_800 start");
	bool bcheck = true;
	WORD	wParaData = 800;
	DWORD	dwErrCode = NO_ERR;

	ADD_ERR_LOG("para : %d, isSetFront : %d, isSetFront2 : %d", wParaData, m_IsSetTorqueFront, m_IsSetTorqueFront_2);

	if(m_IsSetTorqueFront != 0x02)
	{
		dwErrCode = SigmaComWrite((BYTE)FRONT_SERVOPACK, FUNC_CODE, REGISTER_WRITE, ForwardTorqe, wParaData);
		ADD_ERR_LOG("front foward error %d", dwErrCode);
//		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueFront = 0x02;
		else
		{
			m_IsSetTorqueFront = 0x00;
			ADD_ERR_LOG("[SetInnerLimitTorque_800_1] : %x , %d, %d", m_IsSetTorqueFront, bcheck);
			return bcheck;
		}


	}
	if(m_IsSetTorqueFront_2 != 0x02)
	{
		dwErrCode = SigmaComWrite((BYTE)FRONT_SERVOPACK, FUNC_CODE, REGISTER_WRITE, BackwardTorqe, wParaData);
		ADD_ERR_LOG("front backward error %d", dwErrCode);
//		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueFront_2 = 0x02;
		else
		{
			m_IsSetTorqueFront_2 = 0x00;
			ADD_ERR_LOG("[SetInnerLimitTorque_800_2] : %x , %d, %d", m_IsSetTorqueFront_2, bcheck);
			return bcheck;
		}
    }

	//2 축 Driving
#if(OHT_NAME == OHT_NAME_STD_V81S)
	wParaData = OHTMainForm->Edit_rear_torque->Text.ToInt();
	ADD_ERR_LOG("para : %d, isSetRear : %d, isSetRear2 : %d", wParaData, m_IsSetTorqueRear, m_IsSetTorqueRear_2);
#endif
	if(m_IsSetTorqueRear != 0x02)
	{
		dwErrCode = SigmaComWrite((BYTE)REAR_SERVOPACK, FUNC_CODE, REGISTER_WRITE, ForwardTorqe, wParaData);
		ADD_ERR_LOG("rear foward error %d", dwErrCode);
//		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueRear = 0x02;
		else
		{
			m_IsSetTorqueRear = 0x00;
			ADD_ERR_LOG("[SetInnerLimitTorque_800_3] : %x , %d, %d", m_IsSetTorqueRear, bcheck);
			return bcheck;
		}

	}
	if(m_IsSetTorqueRear_2 != 0x02)
	{
		dwErrCode = SigmaComWrite((BYTE)REAR_SERVOPACK, FUNC_CODE, REGISTER_WRITE, BackwardTorqe, wParaData);
		ADD_ERR_LOG("rear backward error %d", dwErrCode);
//		if(dwErrCode != NO_ERR)	bcheck = false;
		if(bcheck==true)	m_IsSetTorqueRear_2 = 0x02;
		else
		{
			m_IsSetTorqueRear_2 = 0x00;
			ADD_ERR_LOG("[SetInnerLimitTorque_800_4] : %x , %d, %d", m_IsSetTorqueRear_2, bcheck);
			return bcheck;
		}
	}
	ADD_ERR_LOG("SetInnerLimitTorque_800 finish\n");
	return bcheck;
}
//---------------------------------------------------------------------------


bool ServoMonitoring::SetInnerLimitTorque_800_Trans()
{
	bool bcheck = true;
	WORD	wParaData = 800;
	DWORD	dwErrCode = NO_ERR;

	dwErrCode = SigmaComWrite((BYTE)3, FUNC_CODE, REGISTER_WRITE, ForwardTorqe, wParaData);
	if(dwErrCode != NO_ERR)	bcheck = false;
	if(bcheck==false)	return bcheck;



	dwErrCode = SigmaComWrite((BYTE)3, FUNC_CODE, REGISTER_WRITE, BackwardTorqe, wParaData);
	if(dwErrCode != NO_ERR)	bcheck = false;
	if(bcheck==false)	return bcheck;


	//2 축 Driving

	dwErrCode = SigmaComWrite((BYTE)4, FUNC_CODE, REGISTER_WRITE, ForwardTorqe, wParaData);
	if(dwErrCode != NO_ERR)	bcheck = false;
	if(bcheck==false)	return bcheck;


	dwErrCode = SigmaComWrite((BYTE)4, FUNC_CODE, REGISTER_WRITE, BackwardTorqe, wParaData);
	if(dwErrCode != NO_ERR)	bcheck = false;
	if(bcheck==false)	return bcheck;

	m_IsSetTorqueHoist = 0x02;
	m_IsSetTorqueShift = 0x02;

	return bcheck;
}

bool ServoMonitoring::IsSetInnerLimitTorque_90()
{
	if((m_IsSetTorqueFront==0x01) &&(m_IsSetTorqueRear==0x01) && (m_IsSetTorqueFront_2==0x01) &&(m_IsSetTorqueRear_2==0x01)) return true;
	else return false;
}
bool ServoMonitoring::IsSetInnerLimitTorque_800()
{
	if((m_IsSetTorqueFront==0x02) &&(m_IsSetTorqueRear==0x02) && (m_IsSetTorqueFront_2==0x02) &&(m_IsSetTorqueRear_2==0x02)) return true;
	else return false;
}
bool ServoMonitoring::IsSetInnerLimitTorque_0()
{
		if((m_IsSetTorqueFront==0x03) &&(m_IsSetTorqueRear==0x03) && (m_IsSetTorqueFront_2==0x03) &&(m_IsSetTorqueRear_2==0x03)) return true;
	else return false;
}

void ServoMonitoring::DisplayServoMonitorResult(WORD RegisterAddress, int readValue)
{
	AnsiString strTemp;

	if(RegisterAddress >= ERROR_TRACE_DATE0 && RegisterAddress <= CURRUNT_ALARM_STATE)
	{
		for(int i=0; i<SERVO_ALARM_COUNT; i++)
		{
			if(readValue == gScomAlarmList[i].alarm_number)
			{
				strTemp.sprintf("SERVOPACK alarm: 0x%x (%s)", readValue, gScomAlarmList[i].alarm_name.c_str());
				OHTMainForm->memoMain->Lines->Add(strTemp);
				ADD_ERR_LOG("[Axis Error] SERVOPACK alarm: 0x%x (%s)", readValue, gScomAlarmList[i].alarm_name.c_str());
			}
		}
	}
	else if(RegisterAddress == SERVO_STATUS_MONITOR)
	{
		for(int i=0; i<SERVO_STATUS_COUNT; i++)
		{
			if(readValue == gScomServoStatusList[i].status_number)
			{
				strTemp.sprintf("SERVOPACK status: 0x%x (%s)", readValue, gScomServoStatusList[i].status_name.c_str());
				OHTMainForm->memoMain->Lines->Add(strTemp);
			}
		}
	}
	/*
	strAlarm.sprintf("SERVOPACK alarm: 0x%x", alarmCode);
	if(alarmCode == 0)
	{
		strAlarm.sprintf("SERVOPACK alarm: 0x%x", alarmCode);
		OHTMainForm->memoMain->Lines->Add(strAlarm);
		ADD_ERR_LOG("[Axis Error] SERVOPACK alarm: 0x%x", alarmCode);
	}
	else
	{
		for(int i=0; i<SERVO_ALARM_COUNT; i++)
		{
			if(alarmCode == gScomAlarmList[i].alarm_number)
			{
				strAlarm.sprintf("SERVOPACK alarm: 0x%x (%s)", alarmCode, gScomAlarmList[i].alarm_name.c_str());
				OHTMainForm->memoMain->Lines->Add(strAlarm);
				ADD_ERR_LOG("[Axis Error] SERVOPACK alarm: 0x%x (%s)", alarmCode, gScomAlarmList[i].alarm_name.c_str());
			}
		}
	}
	*/
}

bool ServoMonitoring::SetFn009(BYTE axis)
{
	DWORD	dwErrCode = NO_ERR;
	long readData;
	AnsiString strTemp;


	//1단계
	if(axis <= 2 && m_pHWCommon!= NULL){
		dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, false); // driving axis인 경우 break;
		if(dwErrCode != NO_ERR) return false;
	}

	//2단계
	m_pHWCommon->SetAMPEnable(axis-1, false);
	//OHTMainForm->memoMain->Lines->Add("Servo Off....");
	Sleep(100);

	//3단계
	dwErrCode = ReadMonitorData(axis, SERVO_STATUS_MONITOR, &readData);
	//DisplayServoMonitorResult(SERVO_STATUS_MONITOR, readData);

	//4단계
	//Step 1. 2000H 작동 모드 설정  - 영점 모드 설정
	dwErrCode = SigmaComWrite(axis, FUNC_CODE, REGISTER_WRITE, Operation_Mode_Settig, Analog_Command_Automatic_Null_Adjustment_Mode, 600);
	Sleep(100);

	//Step 2.2001H 설정 - 실행

	//OHTMainForm->memoMain->Lines->Add("FN009....");

	//5단계
	if(dwErrCode == NO_ERR)
	{
		dwErrCode = SigmaComWrite(axis, FUNC_CODE, REGISTER_WRITE, Operation_Settig, Excute, 600);
	}

	//6단계
	m_pHWCommon->SetAMPEnable(axis-1, true);
	//OHTMainForm->memoMain->Lines->Add("Servo On....");
	Sleep(100);

	//7단계
	if(axis <= 2 && m_pHWCommon!= NULL){
		dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, true); // driving axis인 경우 break;
	}


	if(dwErrCode != NO_ERR) {
		strTemp.sprintf("%d축 FN009 진행 중 Error 발생, Error code: %d", axis, dwErrCode);
		OHTMainForm->memoMain->Lines->Add(strTemp);
		return false;
	}
	else {
		strTemp.sprintf("%d축 FN009 완료", axis);
		OHTMainForm->memoMain->Lines->Add(strTemp);
		return true;
	}

}

bool  ServoMonitoring::Fn009ing()
{
	bool bRetrun = false;

	if((m_nStepFn009 > SERVOPACK_FN009_STEP_INIT )&& (m_nStepFn009 < SERVOPACK_FN009_STEP_ERROR))
		bRetrun =true;

	 return bRetrun;
}

int ServoMonitoring::GetFn009Step()
{
     return m_nStepFn009;
}
void ServoMonitoring::SetFn009Step(int nStep)
{
	m_nStepFn009  = nStep;
}
bool ServoMonitoring::IsFn009Flag()
{
    return  m_Fn009FrontFlag && m_Fn009RearFlag;
}

/**
@brief   시리얼로 ServoPack Data를 읽어서 내용 분석을 하는 함수
@author  puting
@date    2018.03.12
@note    미리 보낸 Data를 가져온다. 단 가져오는 시간이 200ms이상이 되어야 데이터가 보장되기 때문에 그 이후에 데이터를 가져오게 된다.
@return  정상인 경우 0, 에러 발생한 경우 Error Code 리턴
*/
DWORD ServoMonitoring::SMCH_Update()
{

	DWORD	dwErrCode = NO_ERR;
	long readData;
	AnsiString strTemp;

	switch (m_nStepFn009)
	{
		 case SERVOPACK_FN009_STEP_INIT:
		 break;

		 case SERVOPACK_FN009_STEP_1:
		 //1단계
		if(m_pHWCommon!= NULL)
		{
			dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, false); // driving axis인 경우 break;
		}
		 //2단계
		 if(dwErrCode == NO_ERR)
		 {
			 m_pHWCommon->SetAMPEnable(0, false);
//			 OHTMainForm->memoMain->Lines->Add("SERVOPACK_FN009_STEP_1....");
			 Sleep(100);
			 m_nStepFn009 = SERVOPACK_FN009_STEP_2;
		 }else
		 {
			 m_nStepFn009 =	SERVOPACK_FN009_STEP_ERROR;
         }

		 break;

		 case SERVOPACK_FN009_STEP_2:
		 //3단계
//		 OHTMainForm->memoMain->Lines->Add("SERVOPACK_FN009_STEP_2....");
		 ReadMonitorData(1, SERVO_STATUS_MONITOR, &readData);
		//DisplayServoMonitorResult(SERVO_STATUS_MONITOR, readData);
		 dwErrCode = SigmaComWrite(1, FUNC_CODE, REGISTER_WRITE, Operation_Mode_Settig, Analog_Command_Automatic_Null_Adjustment_Mode, 600);
		 Sleep(100);
		//4단계
		//Step 1. 2000H 작동 모드 설정  - 영점 모드 설정
		if(dwErrCode == NO_ERR)
		{
			 m_nStepFn009 = SERVOPACK_FN009_STEP_3;
		}else
		{
            m_nStepFn009 =	SERVOPACK_FN009_STEP_ERROR;
        }
		 break;

		 case SERVOPACK_FN009_STEP_3:
//		 OHTMainForm->memoMain->Lines->Add("SERVOPACK_FN009_STEP_3....");
		 dwErrCode = SigmaComWrite(1, FUNC_CODE, REGISTER_WRITE, Operation_Settig, Excute, 600);
		 if(dwErrCode == NO_ERR)
		 {
			 m_pHWCommon->SetAMPEnable(0, true);
			//OHTMainForm->memoMain->Lines->Add("Servo On....");
			 Sleep(100);
			 m_nStepFn009 = SERVOPACK_FN009_STEP_4;
		 }else
		{
            m_nStepFn009 =	SERVOPACK_FN009_STEP_ERROR;
        }
		 break;

		 case SERVOPACK_FN009_STEP_4:
			//7단계
//			OHTMainForm->memoMain->Lines->Add("SERVOPACK_FN009_STEP_4....");
		if(m_pHWCommon!= NULL)
		{
			dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, true); // driving axis인 경우 break;
		}
		 if(dwErrCode == NO_ERR)
		 {
				 m_nStepFn009 = SERVOPACK_FN009_STEP_5;
		 }else
		 {
           m_nStepFn009 =	SERVOPACK_FN009_STEP_ERROR;
         }

		 break;

		 case SERVOPACK_FN009_STEP_5:
		 strTemp.sprintf("%d축 FN009 완료", 1);
		 OHTMainForm->memoMain->Lines->Add(strTemp);
		 m_Fn009FrontFlag = true;
		 m_nStepFn009 = SERVOPACK_FN009_STEP_6;
		 break;

		 case SERVOPACK_FN009_STEP_6:
		 //1단계
//		 OHTMainForm->memoMain->Lines->Add("SERVOPACK_FN009_STEP_6....");
		if(m_pHWCommon!= NULL)
		{
			dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, false); // driving axis인 경우 break;
		}
		 //2단계
		 if(dwErrCode == NO_ERR)
		 {
			 m_pHWCommon->SetAMPEnable(1, false);
			 //OHTMainForm->memoMain->Lines->Add("Servo Off....");
			 Sleep(100);
			 m_nStepFn009 = SERVOPACK_FN009_STEP_7;
		 }else
		 {
			 m_nStepFn009 =	SERVOPACK_FN009_STEP_ERROR;
         }

		 break;

		 case SERVOPACK_FN009_STEP_7:
		 //3단계
		ReadMonitorData(2, SERVO_STATUS_MONITOR, &readData);
		//DisplayServoMonitorResult(SERVO_STATUS_MONITOR, readData);
		dwErrCode = SigmaComWrite(2, FUNC_CODE, REGISTER_WRITE, Operation_Mode_Settig, Analog_Command_Automatic_Null_Adjustment_Mode, 600);
			 Sleep(100);
		//4단계
		//Step 1. 2000H 작동 모드 설정  - 영점 모드 설정
		if(dwErrCode == NO_ERR)
		{
			 m_nStepFn009 = SERVOPACK_FN009_STEP_8;
		}else
		{
            m_nStepFn009 =	SERVOPACK_FN009_STEP_ERROR;
        }
		 break;

		 case SERVOPACK_FN009_STEP_8:

		 dwErrCode = SigmaComWrite(2, FUNC_CODE, REGISTER_WRITE, Operation_Settig, Excute, 600);
		 if(dwErrCode == NO_ERR)
		 {
//			 m_pHWCommon->SetAMPEnable(1, true);
			//OHTMainForm->memoMain->Lines->Add("Servo On....");
			 Sleep(100);
			 m_nStepFn009 = SERVOPACK_FN009_STEP_9;
		 }else
		{
            m_nStepFn009 =	SERVOPACK_FN009_STEP_ERROR;
        }
		 break;

		 case SERVOPACK_FN009_STEP_9:
		 	//7단계
		if(m_pHWCommon!= NULL)
		{
			dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, true); // driving axis인 경우 break;
		}
		 if(dwErrCode == NO_ERR)
		 {
				 m_nStepFn009 = SERVOPACK_FN009_STEP_10;
		 }else
		 {
           m_nStepFn009 =	SERVOPACK_FN009_STEP_ERROR;
         }

		 break;

		 case SERVOPACK_FN009_STEP_10:
		 strTemp.sprintf("%d축 FN009 완료", 2);
		 OHTMainForm->memoMain->Lines->Add(strTemp);
		 m_Fn009RearFlag = true;
		 m_nStepFn009 = SERVOPACK_FN009_STEP_FINAL;
		 break;




		 case SERVOPACK_FN009_STEP_ERROR:
		 strTemp.sprintf("FN009 진행 중 Error 발생, Error code: %d", dwErrCode);
		 OHTMainForm->memoMain->Lines->Add(strTemp);
		 break;

		 case SERVOPACK_FN009_STEP_FINAL:
		 break;
	}

	return dwErrCode;
/*
	static	DWORD	dwServoTrigerTime	= timeGetTime();     //함수 진입 시간.
	static  int		nErrorCount	= 0;
//	static	bool	bHIDOnTriger	= false;
	bool	bHIDOnTriger	= false;
	DWORD	dwStartTime = timeGetTime();

	UINT  nError = NO_ERROR;

	char chReadBuff[255]={0,};
	char chSendBuff[255]={0,};

	int TempMoveinfo = nMoveInfo;
	int TempnPathInfo = nPathInfo;

	int nRtnByteNo = 0;
//	HID_LOG("UpdateData:%d %d", nMoveInfo, nPathInfo, TempMoveinfo, TempnPathInfo);
	//1. Open Check puting
	// HID가 열리지 않은 상태이므로 에러를 바로 리턴해야함.
	if(IsCommOpen() == false)
		return ERR_HID_OPEN;

	//2. 데이터 읽기.
	//if( bHIDOnTriger==true)) //가능은 하나 데이터 보장을 못함.
	if(dwStartTime - dwServoTrigerTime > 200)
	{

		nRtnByteNo = Read(chReadBuff, chETX);

	   //	chhidBuff=chReadBuff;

		if(nRtnByteNo == 0)
		{
			nErrorCount++;
		}
		else
		{
			trim(chReadBuff);
			//1. 받은 데이터 분석/ 구조체 적용
			nError = AnalysisData(chReadBuff);

			if(nError != NO_ERROR)
				return nError;

			//*********이경우에만 데이터를 실제로 받음*************
			//데이터를 받을 경우, 누적하지 않기위해 초기화
			nErrorCount =0;
			//2. 데이터 보내기.
			GetSendData(chSendBuff, nMoveInfo, nPathInfo);
		  //	memmove(chSendBuff,chWriteBuff, SEND_PK_LENGTH70+2);
			Write(chSendBuff);
			dwServoTrigerTime	= timeGetTime();
			bHIDOnTriger = true;
			//**************나머지 부분은 retry이거나 실패*********

		}
		// 5번이상  받은 데이터가 없을경우, 다시 읽기 시도.
		if(nErrorCount > RETRY_HID_MAX_COUNT)
		{
				bHIDOnTriger= false;
				nErrorCount =0;
				return ERR_HID_READTIMEOUT;
		}
	}
	//초기 시작때 데이터 보내기 위해. 혹은 데이터를 연속해서 못받을 경우 다시 시도 하기 위해

	if( bHIDOnTriger == false)
	{
		//2. 데이터 보내기.
		GetSendData(chSendBuff, nMoveInfo, nPathInfo);
	   //	memmove(chSendBuff,chWriteBuff, SEND_PK_LENGTH70+2);
		nError = Write(chSendBuff);
		dwServoTrigerTime	= timeGetTime();

		return RERTY_HID_SEND_DATA;
	}

	return nError;
	*/
}

void ServoMonitoring::SetInnerLimitTorque_Front()
{
	int FRONT_SERVOPACK = OHTMainForm->Edit_frontName->Text.ToInt();

	WORD wParaData = wParaData = OHTMainForm->Edit_front_torque->Text.ToInt();
	DWORD dwErrCode = NO_ERR;
	String memoStr;

	dwErrCode = SigmaComWrite((BYTE)FRONT_SERVOPACK, FUNC_CODE, REGISTER_WRITE, ForwardTorqe, wParaData);
	memoStr.sprintf(L"Front Forward Torque ErrCode : %d", dwErrCode);
	ADD_MEMO_S(memoStr);
	dwErrCode = SigmaComWrite((BYTE)FRONT_SERVOPACK, FUNC_CODE, REGISTER_WRITE, BackwardTorqe, wParaData);
	memoStr.sprintf(L"Front Backward Torque ErrCode : %d", dwErrCode);
	ADD_MEMO_S(memoStr);
}
void ServoMonitoring::SetInnerLimitTorque_Rear()
{
	int REAR_SERVOPACK = OHTMainForm->Edit_rearName->Text.ToInt();

	WORD wParaData = wParaData = OHTMainForm->Edit_rear_torque->Text.ToInt();
	DWORD dwErrCode = NO_ERR;
    String memoStr;

	dwErrCode = SigmaComWrite((BYTE)REAR_SERVOPACK, FUNC_CODE, REGISTER_WRITE, ForwardTorqe, wParaData);
	memoStr.sprintf(L"Rear Forward Torque ErrCode : %d", dwErrCode);
	ADD_MEMO_S(memoStr);
	dwErrCode = SigmaComWrite((BYTE)REAR_SERVOPACK, FUNC_CODE, REGISTER_WRITE, BackwardTorqe, wParaData);
	memoStr.sprintf(L"Rear Backward Torque ErrCode : %d", dwErrCode);
	ADD_MEMO_S(memoStr);
}

#pragma package(smart_init)
