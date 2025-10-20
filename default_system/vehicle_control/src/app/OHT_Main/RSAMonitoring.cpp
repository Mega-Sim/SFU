//---------------------------------------------------------------------------


#pragma hdrstop

#include "RSAMonitoring.h"

#include "OHTMain.h"

#define ADD_ERR_LOG(...)    WRITE_LOG_UNIT(pLogRSAMonitoring, __VA_ARGS__)


//---------------------------------------------------------------------------
/**
@brief   RSAMonitoring 생성자
@author  soming
@date    2020.04.20
*/
RSAMonitoring::RSAMonitoring(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;

	 // RSA는 이거 설정 어떻게 되는지?   Buadrate 1015  ,  Port No 9
      m_cCOMPort = new Comm_Port(COM_SERVOSETUP,19200, RDATABIT_8, R_PARITYBIT_NONE, R_STOPBIT_2);
\

	pLogRSAMonitoring = getLogUnit("D:\\log\\AbnormalError.log",MAX_BYTE_SIZE_OF_MAIN_LOG);

	if(!IsRSACommOpen())
	{
		ADD_ERR_LOG("RSATEST false");
	}
	else
	{
		 ADD_ERR_LOG("RSATEST true");
	}
	m_RSAIsSetTorqueFront   = 0x00;
	m_RSAIsSetTorqueFront_2 = 0x00;
	m_RSAIsSetTorqueRear    = 0x00;
	m_RSAIsSetTorqueRear_2  = 0x00;

	m_RSAIsSetTorqueHoist = 0x00;
	m_RSAIsSetTorqueHoist_2 = 0x00;
	m_RSAIsSetTorqueShift = 0x00;
	m_RSAIsSetTorqueShift_2 = 0x00;


	SetAVCOStep(RSA_AVCO_STEP_INIT);
	m_AVCOFrontFlag = false;
	m_AVCORearFlag =false;

}

/**
@brief   RSAMonitoring 소멸자
@author  soming
@date    2020.04.20
*/
RSAMonitoring::~RSAMonitoring()
{
	if(m_cCOMPort!=NULL)
	{
		delete m_cCOMPort;
		m_cCOMPort = NULL;
	}

}
bool RSAMonitoring::IsRSACommOpen()
{
	bool bCheck = false;
	if(m_cCOMPort!=NULL)
	{
		if(m_cCOMPort->m_hCOM != INVALID_HANDLE_VALUE)
			bCheck = true;
	}
	return bCheck;
}

DWORD RSAMonitoring::ReadErrorCode(WORD wAddress, DWORD* wMonitorData)
{
	static DWORD dwError = 0;
	static DWORD dwReadData = 0;
	static WORD  wMonitoringData = 1; // 사양서 내용 : Alarm Code 확인 기능은 1 로 세팅
	ADD_ERR_LOG("ReadErrorCode Start");

	dwError = RSACom(wAddress, INPUT_REG_READ, servo_alarm_code, DATASIZE, wMonitoringData, &dwReadData);

	if(dwError != SCOM_RT_SUCCESS) return dwError;
	else
	{
		*wMonitorData = dwReadData;
	}

	return SCOM_RT_SUCCESS;
}

DWORD RSAMonitoring::RSACom(WORD wAddress, WORD wFuncCode, WORD wDataAddress, int DataSize, WORD wData, DWORD* dwReadData)
{
	int Packetsize = DataSize+6;
	unsigned char szRSASendBuff[8] = {0};
	unsigned char szRSAReadBuff[8] = {0};
	DWORD dwError = 0, wCRC = 0;
	int nSedByteNo = 0;
	int nRtnByteNo = 0;
	DWORD dwMonitoringData = 0;

	ADD_ERR_LOG("RSACom Start");

	if(m_cCOMPort == NULL)
	return RCOM_RT_COM_OPEN_FAIL;

	 szRSASendBuff[0] =  wAddress ; // ID 축번호
	 szRSASendBuff[1] =  wFuncCode; //FunctionCode
	 szRSASendBuff[2] =  wDataAddress >> 8; //Address 사용하고자하는 기능 >>8
	 szRSASendBuff[3] =  wDataAddress ; //Address

	  for(int i =0;i<DataSize;i++)
	 {
		  szRSASendBuff[i+4] =  wData >>(8*(DataSize-i-1));
	 }

	 wCRC = CRC16(szRSASendBuff, (Packetsize-2));

	 szRSASendBuff[Packetsize-2] = wCRC;
	 szRSASendBuff[Packetsize-1] = wCRC >> 8;

	 nSedByteNo =  RSAWrite(szRSASendBuff, Packetsize );

	 if(nSedByteNo != 0 )
	 {
		return RCOM_RT_WRITE_FAIL;
	 }

	 Sleep(200); //Servomonitoring 처리와 동일

	 nRtnByteNo = RSARead(szRSAReadBuff,Packetsize);

	 if(nRtnByteNo == 0)
	 {
		return RCOM_RT_RECEIVE_INVALID_DATA;
	 }

	 dwMonitoringData = szRSAReadBuff[3] >> 8 | szRSAReadBuff[4] ;

	 if(dwReadData != NULL)
		*dwReadData = dwMonitoringData;

	 return RCOM_RT_SUCCESS;
}

DWORD RSAMonitoring::DisplayRSAErrorCode(DWORD readAlarm)
{
	AnsiString strTmp;

	for (int i = 0; i < RSA_ALARM_COUNT; i++)
	{
		if(readAlarm == gRSAErrorList[i].alarm_number)
		{
			strTmp.sprintf("RSA SERVOPACK alarm: 0x%x (%s)", readAlarm, gRSAErrorList[i].alarm_name.c_str());
			OHTMainForm->memoMain->Lines->Add(strTmp);
			ADD_ERR_LOG("[Axis Error] RSA SERVOPACK alarm: 0x%x (%s)", readAlarm, gRSAErrorList[i].alarm_name.c_str());
		}
	}
}

//0x03 test code
DWORD RSAMonitoring::RSAComReadTest(WORD wAddress, WORD wFuncCode, WORD wDataAddress, int DataSize, WORD wData)
{
	int Packetsize = DataSize +6;
	char szRSASeneBuff[8] = {0};
	char szRSAReadBuff[8] = {0};

	DWORD dwError = 0, wCRC = 0;
	int nSedByteNo = 0;
	int nRtnByteNo =0;

	szRSASeneBuff[0] = wAddress;
	szRSASeneBuff[1] = wFuncCode;
	szRSASeneBuff[2] = wDataAddress >> 8;
	szRSASeneBuff[3] = wDataAddress;

	for(int i =0;i<DataSize;i++)
	 {
		  szRSASeneBuff[i+4] =  wData >>(8*(DataSize-i-1));
	 }

	 wCRC = CRC16(szRSASeneBuff, (Packetsize-2));

	 szRSASeneBuff[Packetsize-2] = wCRC;
	 szRSASeneBuff[Packetsize-1] = wCRC >> 8;

	 nSedByteNo =  RSAWrite(szRSASeneBuff, Packetsize );

	 if(nSedByteNo != 0 )
	 {
		return RCOM_RT_WRITE_FAIL;
	 }

	 Sleep(500); //Servomonitoring 처리와 동일

	 nRtnByteNo = RSARead(szRSAReadBuff,Packetsize);

	 if(nRtnByteNo == 0)
	 {
		return RCOM_RT_RECEIVE_INVALID_DATA;
	 }

	 return RCOM_RT_SUCCESS;
}
 // 원하는 데이터를 쓰기 위한 Com Write
DWORD RSAMonitoring::RSAComWrite(WORD wAddress, WORD wFuncCode, WORD wDataAddress, int DataSize, WORD wData, int sleep_ms)
{
	int Packetsize =DataSize+6;
	char szRSASendBuff[8] = {0};
	char szRSAReadBuff[8] = {0};
	DWORD dwError = 0, wCRC = 0;
	int nSedByteNo = 0;
	int nRtnByteNo = 0;

	ADD_ERR_LOG("RSAComWrite Start");

	if(m_cCOMPort == NULL)
	return RCOM_RT_COM_OPEN_FAIL;

	 szRSASendBuff[0] =  wAddress ; // ID 축번호
	 szRSASendBuff[1] =   wFuncCode; //FunctionCode
	 szRSASendBuff[2] =   wDataAddress >> 8; //Address 사용하고자하는 기능 >>8
	 szRSASendBuff[3] =  wDataAddress ; //Address

	 for(int i =0;i<DataSize;i++)
	 {
		  szRSASendBuff[i+4] =  wData >>(8*(DataSize-i-1));
	 }

	 wCRC = CRC16(szRSASendBuff, (Packetsize-2));

	 szRSASendBuff[Packetsize-2] = wCRC;
	 szRSASendBuff[Packetsize-1] = wCRC >> 8;

	 nSedByteNo =  RSAWrite(szRSASendBuff, Packetsize );

	 if(nSedByteNo != 0 )
	 {

		return RCOM_RT_WRITE_FAIL;
	 }

	 Sleep(sleep_ms); //Servomonitoring 처리와 동일

	 nRtnByteNo = RSARead(szRSAReadBuff,Packetsize);

	 if(nRtnByteNo == 0)
	 {
		return RCOM_RT_RECEIVE_INVALID_DATA;
	 }

	//트크 값 비정상 세팅인 경우 Error 리턴
	if(szRSAReadBuff[4] != szRSASendBuff[4])
	{
		ADD_ERR_LOG("RSAComWrite Error, szRSAReadBuff[4] : %x, szRSASendBuff[4] : %x", szRSAReadBuff[4], szRSASendBuff[4]);
		return  RCOM_RT_RECEIVE_INVALID_DATA;
	}
	if(szRSAReadBuff[5] != szRSASendBuff[5])
	{
		ADD_ERR_LOG("RSAComWrite Error, szRSAReadBuff[5] : %x, szRSASendBuff[5] : %x", szRSAReadBuff[5], szRSASendBuff[5]);
		return  RCOM_RT_RECEIVE_INVALID_DATA;
	}

	return RCOM_RT_SUCCESS;

}int RSAMonitoring::RSARead(char* chReadData, int len){	int i, index = 0;   char chBuff;


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

			break;
		}
		*(chReadData + index ) = chBuff;
		 index++;
		 if(index == len) break;
   }

	return index;}int RSAMonitoring::RSAWrite(char* chSndData, int len){	 PurgeComm(m_cCOMPort->m_hCOM, PURGE_TXCLEAR|PURGE_RXCLEAR);	 if(len == (int)(m_cCOMPort->WriteCommBlock(chSndData,len)))	  return NO_ERROR;	 else	 {		 ADD_ERR_LOG("len 불일치");	 }}WORD RSAMonitoring::CRC16(const byte *nData, WORD wLength){  	 static const WORD wCRCTable[] = {	 0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
	 0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
	 0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
	 0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
	 0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
	 0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
	 0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
	 0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
	 0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
	 0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
	 0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
	 0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
	 0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
	 0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
	 0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
	 0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
	 0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
	 0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
	 0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
	 0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
	 0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
	 0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
	 0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
	 0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
	 0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
	 0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
	 0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
	 0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
	 0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
	 0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
	 0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
	 0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };

	 BYTE nTemp;
	 WORD wCRCWord = 0xFFFF;

	while (wLength--)
	{
		nTemp = *nData++ ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord ^= wCRCTable[nTemp];
	}
	return wCRCWord;}bool RSAMonitoring::SetInnerLimitTorque_300_Trans()
{
	DWORD dwErrCode;
	WORD HOIST_SERVOPACK = 0x03;
	WORD SHIFT_SERVOPACK = 0x04;
	bool bcheck = true;
	WORD wParaData = 300;

	// Hoist Positive Torque Limit
	dwErrCode = RSAComWrite((BYTE)HOIST_SERVOPACK, SINGLE_REG_WRITE, PositiveTorque,sizeof(wParaData), wParaData, 200);
	Sleep(200);

	if(dwErrCode != RCOM_RT_SUCCESS)
	{
		bcheck = false;
		m_RSAIsSetTorqueHoist = 0x00;
		ADD_ERR_LOG("Hoist PositiveTorque error %d", dwErrCode);
		return bcheck;
	}
	else
		m_RSAIsSetTorqueHoist = 0x02;

	// Hoist Negative Torque Limit
	dwErrCode = RSAComWrite((BYTE)HOIST_SERVOPACK, SINGLE_REG_WRITE, NegetiveTorque,sizeof(wParaData), wParaData, 200);
	Sleep(200);
	if(dwErrCode != RCOM_RT_SUCCESS)
	{
		bcheck = false;
		m_RSAIsSetTorqueHoist_2 = 0x00;
		ADD_ERR_LOG("Hoist NegativeTorque error %d", dwErrCode);
		return bcheck;
	}
	else
		m_RSAIsSetTorqueHoist_2 = 0x02;

	// Shift Positive Torque Limit
	dwErrCode = RSAComWrite((BYTE)SHIFT_SERVOPACK, SINGLE_REG_WRITE, PositiveTorque,sizeof(wParaData), wParaData, 200);
	Sleep(200);
	if(dwErrCode != RCOM_RT_SUCCESS)
	{
		bcheck = false;
		m_RSAIsSetTorqueShift = 0x00;
		ADD_ERR_LOG("Shift PositiveTorque error %d", dwErrCode);
		return bcheck;
	}
	else
		m_RSAIsSetTorqueShift = 0x02;

	// Shift Negative Torque Limit
	dwErrCode = RSAComWrite((BYTE)SHIFT_SERVOPACK, SINGLE_REG_WRITE, NegetiveTorque,sizeof(wParaData), wParaData, 200);
	Sleep(200);
	if(dwErrCode != RCOM_RT_SUCCESS)
	{
		bcheck = false;
		m_RSAIsSetTorqueShift_2 = 0x00;
		ADD_ERR_LOG("Shift PositiveTorque error %d", dwErrCode);
		return bcheck;
	}
	else
		m_RSAIsSetTorqueShift_2 = 0x02;

	return  bcheck;
}

bool RSAMonitoring::SetInnerLimitTorque_300()
{
	DWORD dwErrCode;
	WORD FRONT_SERVOPACK = 0x01;
	WORD REAR_SERVOPACK = 0x02;
	bool bcheck = true;
	WORD	wParaData = 300;

	ADD_ERR_LOG("[RSA]SetInnerLimitTorque_300 start test");

	if(m_RSAIsSetTorqueFront != 0x02)
	{
		dwErrCode = RSAComWrite((BYTE)FRONT_SERVOPACK, SINGLE_REG_WRITE, PositiveTorque,sizeof(wParaData), wParaData, 200);
		Sleep(200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Front PositiveTorque error %d", dwErrCode);
			m_RSAIsSetTorqueFront = 0x00;
			bcheck = false;

			return bcheck;
		}
		if(bcheck==true)
			m_RSAIsSetTorqueFront = 0x02;
	}
	if(m_RSAIsSetTorqueFront_2 != 0x02)
	{
		dwErrCode = RSAComWrite((BYTE)FRONT_SERVOPACK, SINGLE_REG_WRITE, NegetiveTorque,sizeof(wParaData), wParaData, 200);
		Sleep(200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Front NegativeTorque error %d", dwErrCode);
			m_RSAIsSetTorqueFront_2 = 0x00;
			bcheck = false;

			return bcheck;

		}
		if(bcheck==true)
			m_RSAIsSetTorqueFront_2 = 0x02;
	}
	if(m_RSAIsSetTorqueRear != 0x02)
	{
		dwErrCode = RSAComWrite((BYTE)REAR_SERVOPACK, SINGLE_REG_WRITE, PositiveTorque,sizeof(wParaData), wParaData, 200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Rear PositiveTorque error %d", dwErrCode);
			m_RSAIsSetTorqueRear = 0x00;
			bcheck = false;

			return bcheck;
		}
		if(bcheck==true)
			m_RSAIsSetTorqueRear = 0x02;

	}
	if(m_RSAIsSetTorqueRear_2 != 0x02)
	{
		dwErrCode = RSAComWrite((BYTE)REAR_SERVOPACK, SINGLE_REG_WRITE, NegetiveTorque,sizeof(wParaData), wParaData, 200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Rear NegativeTorque error %d", dwErrCode);
			m_RSAIsSetTorqueRear_2 = 0x00;
			bcheck = false;

			return bcheck;
		}
		if(bcheck==true)
			m_RSAIsSetTorqueRear_2 = 0x02;

	}

	ADD_ERR_LOG("[RSA]SetInnerLimitTorque_300 finish\n");

	return bcheck;

}

bool RSAMonitoring::SetInnerLimitTorque_90()
{
	DWORD dwErrCode;
	WORD FRONT_SERVOPACK = 0x01;
	WORD REAR_SERVOPACK = 0x02;
	bool bcheck = true;
	WORD	wParaData = 90;

	ADD_ERR_LOG("[RSA]SetInnerLimitTorque_90 start test");

	if(m_RSAIsSetTorqueFront != 0x01)
	{
		dwErrCode = RSAComWrite(FRONT_SERVOPACK, SINGLE_REG_WRITE, PositiveTorque,sizeof(wParaData), wParaData, 200);
		Sleep(200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Front PositiveTorque error %d", dwErrCode);
			m_RSAIsSetTorqueFront = 0x00;
			bcheck = false;

			return bcheck;
		}
		if(bcheck==true)
			m_RSAIsSetTorqueFront = 0x01;
	}
	if(m_RSAIsSetTorqueFront_2 != 0x01)
	{
		dwErrCode = RSAComWrite(FRONT_SERVOPACK, SINGLE_REG_WRITE, NegetiveTorque,sizeof(wParaData), wParaData, 200);
		Sleep(200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Front NegativeTorque error %d", dwErrCode);
			m_RSAIsSetTorqueFront_2 = 0x00;
			bcheck = false;

			return bcheck;
		}
		if(bcheck==true)
			m_RSAIsSetTorqueFront_2 = 0x01;
	}
	if(m_RSAIsSetTorqueRear != 0x01)
	{
		dwErrCode = RSAComWrite(REAR_SERVOPACK, SINGLE_REG_WRITE, PositiveTorque,sizeof(wParaData), wParaData, 200);
		Sleep(200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Rear PositiveTorque error %d", dwErrCode);
			m_RSAIsSetTorqueRear = 0x00;
			bcheck = false;

			return bcheck;
		}

		if(bcheck==true)
			m_RSAIsSetTorqueRear = 0x01;
	}
	if(m_RSAIsSetTorqueRear_2 != 0x01)
	{
		dwErrCode = RSAComWrite(REAR_SERVOPACK, SINGLE_REG_WRITE, NegetiveTorque,sizeof(wParaData), wParaData, 200);
		Sleep(200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Rear NegativeTorque error %d", dwErrCode);
			m_RSAIsSetTorqueRear_2 = 0x00;
			bcheck = false;
			return bcheck;
		}

		if(bcheck==true)
			m_RSAIsSetTorqueRear_2 = 0x01;
	}

	ADD_ERR_LOG("[RSA]SetInnerLimitTorque_90 finish\n");

	return bcheck;


}

bool RSAMonitoring::SetInnerLimitTorque_0()
{
	DWORD dwErrCode;
	WORD FRONT_SERVOPACK = 0x01;
	WORD REAR_SERVOPACK = 0x02;
	bool bcheck = true;
	WORD	wParaData = 0;

	ADD_ERR_LOG("[RSA]SetInnerLimitTorque_0 start test");

	if(m_RSAIsSetTorqueFront != 0x01)
	{
		dwErrCode = RSAComWrite(FRONT_SERVOPACK, SINGLE_REG_WRITE, PositiveTorque,sizeof(wParaData), wParaData, 200);
		Sleep(200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Front PositiveTorque error %d", dwErrCode);
			m_RSAIsSetTorqueFront = 0x00;
			bcheck = false;

			return bcheck;
		}
		if(bcheck==true)
			m_RSAIsSetTorqueFront = 0x01;
	}
	if(m_RSAIsSetTorqueFront_2 != 0x01)
	{
		dwErrCode = RSAComWrite(FRONT_SERVOPACK, SINGLE_REG_WRITE, NegetiveTorque,sizeof(wParaData), wParaData, 200);
		Sleep(200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Front NegativeTorque error %d", dwErrCode);
			m_RSAIsSetTorqueFront_2 = 0x00;
			bcheck = false;

			return bcheck;
		}
		if(bcheck==true)
			m_RSAIsSetTorqueFront_2 = 0x01;
	}
	if(m_RSAIsSetTorqueRear != 0x01)
	{
		dwErrCode = RSAComWrite(REAR_SERVOPACK, SINGLE_REG_WRITE, PositiveTorque,sizeof(wParaData), wParaData, 200);
		Sleep(200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Rear PositiveTorque error %d", dwErrCode);
			m_RSAIsSetTorqueRear = 0x00;
			bcheck = false;

			return bcheck;
		}

		if(bcheck==true)
			m_RSAIsSetTorqueRear = 0x01;
	}
	if(m_RSAIsSetTorqueRear_2 != 0x01)
	{
		dwErrCode = RSAComWrite(REAR_SERVOPACK, SINGLE_REG_WRITE, NegetiveTorque,sizeof(wParaData), wParaData, 200);
		Sleep(200);

		if(dwErrCode != RCOM_RT_SUCCESS)
		{
			ADD_ERR_LOG("Rear NegativeTorque error %d", dwErrCode);
			m_RSAIsSetTorqueRear_2 = 0x00;
			bcheck = false;
			return bcheck;
		}

		if(bcheck==true)
			m_RSAIsSetTorqueRear_2 = 0x01;
	}

	ADD_ERR_LOG("[RSA]SetInnerLimitTorque_0 finish\n");

	return bcheck;


}

bool RSAMonitoring::IsSetInnerLimitTorque_90()
{
	if((m_RSAIsSetTorqueFront==0x01) &&(m_RSAIsSetTorqueRear==0x01) && (m_RSAIsSetTorqueFront_2==0x01) &&(m_RSAIsSetTorqueRear_2==0x01))
		return true;
	else
		return false;
}
bool RSAMonitoring::IsSetInnerLimitTorque_300()
{
	if((m_RSAIsSetTorqueFront==0x02) &&(m_RSAIsSetTorqueRear==0x02) && (m_RSAIsSetTorqueFront_2==0x02) &&(m_RSAIsSetTorqueRear_2==0x02))
		return true;
	else
		return false;
}

bool RSAMonitoring::IsSetInnerLimitTorque_0()
{
	if((m_RSAIsSetTorqueFront==0x03) &&(m_RSAIsSetTorqueRear==0x03) && (m_RSAIsSetTorqueFront_2==0x03) &&(m_RSAIsSetTorqueRear_2==0x03))
		return true;
	else
		return false;
}

//OHT Main에서 Servo 버튼 누를떄 사용
bool RSAMonitoring::SetVelocityCommandOffset(Byte axis)
{
	DWORD	dwErrCode = NO_ERR;
	long readData;
	AnsiString strTemp;
	WORD wParaData = 1; // 운전 명령 데이터는 오직 1

	ADD_ERR_LOG("SetVelocityCommandOffset_TP");
	//1단계
	if(axis <= 2 && m_pHWCommon!= NULL)
	{
		ADD_ERR_LOG("SetVelocityCommandOffset_break release");
		dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, false); // driving axis인 경우 break;
		if(dwErrCode != NO_ERR) return false;
	}

	//2단계
	if(dwErrCode == NO_ERR)
	{
		ADD_ERR_LOG("SetVelocityCommandOffset axis 1 amp disable");
		m_pHWCommon->SetAMPEnable(axis-1, false);
		Sleep(100);
		if(dwErrCode != NO_ERR) return false;
	}
	//3단계
	if(dwErrCode == NO_ERR)
	{
		OHTMainForm->memoMain->Lines->Add("FN009....");
		dwErrCode = RSAComWrite(axis, SINGLE_REG_WRITE, Analog_Velocity_Command_Offset, DATASIZE, wParaData);
		ADD_ERR_LOG("SetVelocityCommandOffset axis 1 Analog_Velocity_Command_Offset");
		Sleep(100);
		if(dwErrCode != NO_ERR) return false;
	}


	//4단계
	if(dwErrCode == NO_ERR)
	{
		ADD_ERR_LOG("SetVelocityCommandOffset axis 1 amp enable");
		m_pHWCommon->SetAMPEnable(axis-1, true);
		Sleep(100);
		if(dwErrCode != NO_ERR) return false;
	}


	//5단계
	if(axis <= 2 && m_pHWCommon!= NULL)
	{
		dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, true); // driving axis인 경우 break;
		ADD_ERR_LOG("SetVelocityCommandOffset axis 1 break release true");
		if(dwErrCode != NO_ERR) return false;
	}

	//6단계 Error Chcek
	if(dwErrCode != NO_ERR)
	{
		ADD_ERR_LOG("SetVelocityCommandOffset axis 1 != NO_ERR");
		strTemp.sprintf("%d축 RSA Offset Auto Tune 진행 중 Error 발생, Error code: %d", axis, dwErrCode);
		OHTMainForm->memoMain->Lines->Add(strTemp);
		return false;
	}
	else
	{
		ADD_ERR_LOG("SetVelocityCommandOffset axis 1 == NO_ERR");
		strTemp.sprintf("%d축 RSA Offset Auto Tune 완료", axis);
		OHTMainForm->memoMain->Lines->Add(strTemp);
		return true;
	}

}

bool  RSAMonitoring::AVCOing()
{
	bool bRetrun = false;

	if((m_nStepAVCO > RSA_AVCO_STEP_INIT )&& (m_nStepAVCO < RSA_AVCO_STEP_ERROR))
		bRetrun =true;

	 return bRetrun;
}

int RSAMonitoring::GetAVCOStep()
{
	 return m_nStepAVCO;
}
void RSAMonitoring::SetAVCOStep(int nStep)
{
	m_nStepAVCO  = nStep;
}
bool RSAMonitoring::IsAVCOFlag()
{
	return  m_AVCOFrontFlag && m_AVCORearFlag;
}

//Auto 전환 시 사용
//AVCO Auto Velocity Command Offset
DWORD RSAMonitoring::AutoVelocityCommandOffset()
{

	DWORD	dwErrCode = NO_ERR;
	long readData;
	AnsiString strTemp;
	WORD wParaData = 1; // 운전 명령 데이터는 오직 1

	switch (m_nStepAVCO)
	{
		 case RSA_AVCO_STEP_INIT:
		 break;

		 //Brake Relaese 시작
		 case RSA_AVCO_STEP_1:
		 if(m_pHWCommon!= NULL)
		 {
			ADD_ERR_LOG("AutoVelocityCommandOffset_Brake Release false");
			dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, false);
			Sleep(100);
		 }
		 if(dwErrCode == NO_ERR)
		 {
			 m_nStepAVCO = RSA_AVCO_STEP_2;
		 }
		 else
		 {
			m_nStepAVCO =  RSA_AVCO_STEP_ERROR;
		 }
		 break;

		 // Front amp disable
		 case RSA_AVCO_STEP_2:
		 ADD_ERR_LOG("AutoVelocityCommandOffset_Front amp disable");
		 dwErrCode = m_pHWCommon->SetAMPEnable(0, false);
		 Sleep(100);
		 if(dwErrCode == NO_ERR)
		 {
			 m_nStepAVCO = RSA_AVCO_STEP_3;
		 }
		 else
		 {
			m_nStepAVCO =	RSA_AVCO_STEP_ERROR;
		 }
		 break;

		 //AutoVelocityCommandOffset_Front
		 case RSA_AVCO_STEP_3:
		 ADD_ERR_LOG("AutoVelocityCommandOffset_Front analog velocity command");
		 dwErrCode = RSAComWrite(1, SINGLE_REG_WRITE, Analog_Velocity_Command_Offset, DATASIZE, wParaData, 600);
		 if(dwErrCode == NO_ERR)
		 {
			 m_nStepAVCO = RSA_AVCO_STEP_4;
		 }
		 else
		 {
			m_nStepAVCO =	RSA_AVCO_STEP_ERROR;
		 }
		 break;
		 //Front amp enable
		 case RSA_AVCO_STEP_4:
		 ADD_ERR_LOG("AutoVelocityCommandOffset_Front amp enable");
		 dwErrCode = m_pHWCommon->SetAMPEnable(0, true);
		 Sleep(100);
		 if(dwErrCode == NO_ERR)
		 {
			 m_nStepAVCO = RSA_AVCO_STEP_5;
		 }
		 else
		 {
			m_nStepAVCO =	RSA_AVCO_STEP_ERROR;
		 }
		 break;
		 //Brake Release true
		 case RSA_AVCO_STEP_5:
		 if(m_pHWCommon!= NULL)
		 {
			ADD_ERR_LOG("AutoVelocityCommandOffset_Brake Release true");
			dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, true); // driving axis인 경우 break;
		 }
		 if(dwErrCode == NO_ERR)
		 {
			 m_nStepAVCO = RSA_AVCO_STEP_6;
		 }
		 else
		 {
		   m_nStepAVCO = RSA_AVCO_STEP_ERROR;
		 }

		 break;

		 //Front 축 완료
		 case RSA_AVCO_STEP_6:
		 ADD_ERR_LOG("AutoVelocityCommandOffset_Front Complete");
		 strTemp.sprintf("%d축 AVCO 완료", 1);
		 OHTMainForm->memoMain->Lines->Add(strTemp);
		 m_AVCOFrontFlag = true;
		 m_nStepAVCO = RSA_AVCO_STEP_7;
		 break;

		 //Rear 축 시작 - Brake release false
		 case RSA_AVCO_STEP_7:
		 if(m_pHWCommon!= NULL)
		 {
			ADD_ERR_LOG("AutoVelocityCommandOffset_Brake Release false");
			dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, false); // driving axis인 경우 break;
		 }
		 if(dwErrCode == NO_ERR)
		 {
			 m_nStepAVCO = RSA_AVCO_STEP_8;
		 }
		 else
		 {
			 m_nStepAVCO = RSA_AVCO_STEP_ERROR;
		 }
		 //Rear amp disable
		 case RSA_AVCO_STEP_8:
		 ADD_ERR_LOG("AutoVelocityCommandOffset_Rear amp disable");
		 dwErrCode = m_pHWCommon->SetAMPEnable(1, false);
		 Sleep(100);

		 if(dwErrCode == NO_ERR)
		 {
			 m_nStepAVCO = RSA_AVCO_STEP_9;
		 }
		 else
		 {
			 m_nStepAVCO = RSA_AVCO_STEP_ERROR;
		 }
		 break;
		 //AutoVelocityCommandOffset_Rear
		 case RSA_AVCO_STEP_9:
		 ADD_ERR_LOG("AutoVelocityCommandOffset_Rear analog velocity command");
		 dwErrCode = RSAComWrite(2, SINGLE_REG_WRITE, Analog_Velocity_Command_Offset, 2, wParaData, 600);
		 if(dwErrCode == NO_ERR)
		 {
			// rear amp enable 안하는게 맞는지 확인
			//	m_pHWCommon->SetAMPEnable(1, true);
			//OHTMainForm->memoMain->Lines->Add("Servo On....");
			//	Sleep(100);
			m_nStepAVCO = RSA_AVCO_STEP_10;
		 }
		 else
		 {
			m_nStepAVCO =	RSA_AVCO_STEP_ERROR;
		 }
		 break;
		 //Brake Release true
		 case RSA_AVCO_STEP_10:
		 if(m_pHWCommon!= NULL)
		 {
			ADD_ERR_LOG("AutoVelocityCommandOffset_Brake Release true");
			dwErrCode = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, true); // driving axis인 경우 break;
		 }
		 if(dwErrCode == NO_ERR)
		 {
			m_nStepAVCO = RSA_AVCO_STEP_11;
		 }
		 else
		 {
			m_nStepAVCO =	RSA_AVCO_STEP_ERROR;
		 }

		 break;
		 //Rear 축 완료
		 case RSA_AVCO_STEP_11:
		 ADD_ERR_LOG("AutoVelocityCommandOffset_Rear Complete");
		 strTemp.sprintf("%d축 AVCO 완료", 2);
		 OHTMainForm->memoMain->Lines->Add(strTemp);
		 m_AVCORearFlag = true;
		 m_nStepAVCO = RSA_AVCO_STEP_FINAL;
		 break;

		 //Error 발생
		 case RSA_AVCO_STEP_ERROR:
		 ADD_ERR_LOG("AutoVelocityCommandOffset_Error");
		 strTemp.sprintf("AVCO 진행 중 Error 발생, Error code: %d", dwErrCode);
		 OHTMainForm->memoMain->Lines->Add(strTemp);
		 break;
		 //완료
		 case RSA_AVCO_STEP_FINAL:
		 ADD_ERR_LOG("AutoVelocityCommandOffset_Front, Rear Complete");
		 break;
	}

	return dwErrCode;
}


void RSAMonitoring::SetInnerLimitTorque_Front()
{
	WORD FRONT_SERVOPACK = 0x01;
	WORD wParaData = wParaData = OHTMainForm->Edit_front_torque->Text.ToInt();
	DWORD dwErrCode = NO_ERR;
	String memoStr;

	dwErrCode = RSAComWrite(FRONT_SERVOPACK, SINGLE_REG_WRITE, PositiveTorque,sizeof(wParaData), wParaData, 200);
	memoStr.sprintf(L"Front Forward Torque ErrCode : %d", dwErrCode);
	ADD_MEMO_S(memoStr);
	dwErrCode = RSAComWrite(FRONT_SERVOPACK, SINGLE_REG_WRITE, NegetiveTorque,sizeof(wParaData), wParaData, 200);
	memoStr.sprintf(L"Front Backward Torque ErrCode : %d", dwErrCode);
	ADD_MEMO_S(memoStr);
}
void RSAMonitoring::SetInnerLimitTorque_Rear()
{
	WORD REAR_SERVOPACK = 0x02;
	WORD wParaData = wParaData = OHTMainForm->Edit_front_torque->Text.ToInt();
	DWORD dwErrCode = NO_ERR;
	String memoStr;

	dwErrCode = RSAComWrite(REAR_SERVOPACK, SINGLE_REG_WRITE, PositiveTorque,sizeof(wParaData), wParaData, 200);
	memoStr.sprintf(L"Rear Forward Torque ErrCode : %d", dwErrCode);
	ADD_MEMO_S(memoStr);
	dwErrCode = RSAComWrite(REAR_SERVOPACK, SINGLE_REG_WRITE, NegetiveTorque,sizeof(wParaData), wParaData, 200);
	memoStr.sprintf(L"Rear Backward Torque ErrCode : %d", dwErrCode);
	ADD_MEMO_S(memoStr);

}

#pragma package(smart_init)
