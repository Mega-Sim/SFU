/**
@file    HID.cpp
$Rev:: 679           $:  Revision of last commit
$Author:: ltw070     $:  Author of last commit
$Date:: 2013-03-11 1#$:  Date of last commit
@version OHT 7.0
@brief   HID Class CPP File
*/

#include "HID_VII_ZR.h"

#include "OHTMain.h"

using namespace std;

//#define HID_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\HIDRawData.log", __VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)

/**
@brief   HID 생성자
@author   puting
@date
@param   pHWCommon : HWCommon Class의 포인터
@note    Input, Ouput을 사용하기 위해 HWSet의 Pointer를 가져옴
*/
HID_VII_ZR::HID_VII_ZR(int ComPortNum): Virtual_HID()
{
	int nError =0;

	//1.크리티컬 섹션 생성
	InitializeCriticalSection(&m_CS);

	//2. 초기화
	memset(&m_HIDData, 0x00, sizeof(m_HIDData));
	m_HIDCommPort=NULL;


	//3. 시리얼 Open
	nError = Open(ComPortNum);
	if(nError != NO_ERR)
	{
		 Close();
	 }
	 else
	 {
		// Open 성공 로그 기록
	   //	MainForm->OHTStatus->Caption = "Open성공";
	   //	 MainForm->Memo1->Lines->Add("OPEN 성공");
	   //	 m_nHIDComFailCount=0;
	 }

	 m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	 m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
	 pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
}
/**
@brief   HID 소멸자
@author  zzang9un
@date    2012.10.13
*/
HID_VII_ZR::~HID_VII_ZR()
{
	//소멸 및 닫기
	DeleteCriticalSection(&m_CS);
	//delete m_HIDInfo;
	Close();
}
/**
@brief   시리얼로 HID Data를 읽어서 내용 분석을 하는 함수
@author  puting
@date    2012.11.07
@note    미리 보낸 Data를 가져온다. 단 가져오는 시간이 200ms이상이 되어야 데이터가 보장되기 때문에 그 이후에 데이터를 가져오게 된다.
@return  정상인 경우 0, 에러 발생한 경우 Error Code 리턴
*/
int HID_VII_ZR::UpdateData(int nMoveInfo, int nPathInfo)
{
	static	DWORD	dwHIDTrigerTime	= timeGetTime();     //함수 진입 시간.
	static  int		nErrorCount	= 0;
//	static	bool	bHIDOnTriger	= false;
	bool	bHIDOnTriger	= false;


	UINT  nError = NO_ERROR;

//	char *chhidBuff="";
//	char *chWriteBuff="";
	char chReadBuff[255]={0,};
	char chSendBuff[255]={0,};

	int TempMoveinfo = nMoveInfo;
	int TempnPathInfo = nPathInfo;

	int nRtnByteNo = 0;
//	HID_LOG("UpdateData:%d %d", nMoveInfo, nPathInfo, TempMoveinfo, TempnPathInfo);
	//1. Open Check puting
	// HID가 열리지 않은 상태이므로 에러를 바로 리턴해야함.
	if(m_HIDCommPort == NULL)
		return ERR_HID_OPEN;

	//2. 데이터 읽기.
	//if( bHIDOnTriger==true)) //가능은 하나 데이터 보장을 못함.
	if(OHTUTILITY::PreventOverflowGetTimeMs(dwHIDTrigerTime) > 200)
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
			dwHIDTrigerTime	= timeGetTime();
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
		dwHIDTrigerTime	= timeGetTime();

		return RERTY_HID_SEND_DATA;
	}

	return nError;
}

/**
@brief   HID로부터 Read한 Data를 분석하는 함수
@date    2012.11.07
@author  puting
@param   strRcvData : HID로부터 Read한 문자열
@param   nTmpError : 에러 유무 여부(?)
@return  정상인 경우 0, 에러 발생한 경우 Error Code 리턴
@warning StrToIntDef 함수는 Builder에서만 컴파일됨
*/
int HID_VII_ZR::AnalysisData(char* chRcvData)
{
	int nError = 0;
	char Substr[10] = {0,};
	bool bV85Protocol = false;
	//char* chLogData;
   //	static char* chPreData70 = "";

	  //Form1->Memo1->Lines->Add(chRcvData);
	//1. data 정합성 확인(길이)
//	int lens = strlen(chRcvData);
//	if(lens!=RCV_PK_LENGTH70)                           //HID Data 고정 길이  6.5일 경우
//   		return ERR_DEFFER_HID_RCV_DATA;      //받은 HID 데이터 길이가 다를 경우

	//크리티컬섹션 시작
	EnterCriticalSection(&m_CS);

	if(strlen(chRcvData) > RCV_PK_PROTOCOL_LENSTD)
		bV85Protocol = true;

	//2.s식별자 및 CheckSum 확인
//	HID_LOG("HID Data : %s",chRcvData);
	if(bV85Protocol == false)
	{
		if((GetRcvDataCheckSum(chRcvData, RCV_PK_LENGTH70-1)!=true)&&(chRcvData[0]!='s'))
		{
			memset(&m_HIDData, 0x00, sizeof(m_HIDData));
			nError = ERR_HID_READTIMEOUT;   //받은 데이터가 이상할 경우.
		}
		else
		{
//	   HID_LOG("RawData:%s", chRcvData);
        //디버깅 필요...
		//추가 수정 필요.
		m_HIDData.Edc 						= CharToInt(SubChar(chRcvData,Substr,1,3,sizeof(Substr)));
		m_HIDData.Ecap 						= CharToInt(SubChar(chRcvData,Substr,4,3,sizeof(Substr)));
		m_HIDData.Eout  					= CharToInt(SubChar(chRcvData,Substr,7,3,sizeof(Substr)));
		m_HIDData.Iout_sign 		   		= chRcvData[10];
		m_HIDData.Iout 						= CharToInt(SubChar(chRcvData,Substr,11,3,sizeof(Substr)));
		m_HIDData.Iac 						= CharToInt(SubChar(chRcvData,Substr,14,3,sizeof(Substr)));
		m_HIDData.ErrorCode  			    = CharToInt(SubChar(chRcvData,Substr,17,2,sizeof(Substr)));
		m_HIDData.ErrorCode 				= ErrorConvert(m_HIDData.ErrorCode);    //puting  2013.11.12
//		m_HIDData.WarningCode 				= CharToInt(SubChar(chRcvData,23,2));
		m_HIDData.PickupTemperature 		= CharToInt(SubChar(chRcvData,Substr,19,3,sizeof(Substr)));
		m_HIDData.IGBTTemperature			= CharToInt(SubChar(chRcvData,Substr,22,3,sizeof(Substr)));
		m_HIDData.ECap1Temperature			= CharToInt(SubChar(chRcvData,Substr,25,3,sizeof(Substr)));
		m_HIDData.ECap2Temperature 			= CharToInt(SubChar(chRcvData,Substr,28,3,sizeof(Substr)));
		m_HIDData.ECapRoundTemperature 		= CharToInt(SubChar(chRcvData,Substr,31,3,sizeof(Substr)));
		m_HIDData.Reserve1  				= CharToInt(SubChar(chRcvData,Substr,34,3,sizeof(Substr)));
		m_HIDData.Reserve2  				= CharToInt(SubChar(chRcvData,Substr,37,3,sizeof(Substr)));
		m_HIDData.Fimware  				 	= CharToInt(SubChar(chRcvData,Substr,40,4,sizeof(Substr)));

		}
	}else
	{
		//recv 크기도 다름.. RCV_PK_LENGTH70
		if((GetRcvDataCheckSum_V85(chRcvData, RCV_PK_LENGTH70-1)!=true)&&(chRcvData[0]!='s'))
		{
			memset(&m_HIDData, 0x00, sizeof(m_HIDData));
			nError = ERR_HID_READTIMEOUT;   //받은 데이터가 이상할 경우.
		}
		else
		{
	//	   HID_LOG("RawData:%s", chRcvData);
			//디버깅 필요...
			//추가 수정 필요.
			m_HIDData.Edc 						= CharToInt(SubChar(chRcvData,Substr,1,3,sizeof(Substr)));
			m_HIDData.Ecap 						= CharToInt(SubChar(chRcvData,Substr,4,3,sizeof(Substr)));
			m_HIDData.Eout  					= CharToInt(SubChar(chRcvData,Substr,7,3,sizeof(Substr)));
			m_HIDData.Iout_sign 		   		= chRcvData[10];
			m_HIDData.Iout 						= CharToInt(SubChar(chRcvData,Substr,11,3,sizeof(Substr)));
			m_HIDData.Iac 						= CharToInt(SubChar(chRcvData,Substr,14,3,sizeof(Substr)));
			m_HIDData.ErrorCode  			    = CharToInt(SubChar(chRcvData,Substr,17,2,sizeof(Substr)));
			m_HIDData.ErrorCode 				= ErrorConvert(m_HIDData.ErrorCode);    //puting  2013.11.12
			m_HIDData.PickupTemperature 		= CharToInt(SubChar(chRcvData,Substr,19,3,sizeof(Substr)));
			m_HIDData.IGBTTemperature			= CharToInt(SubChar(chRcvData,Substr,22,3,sizeof(Substr)));
			m_HIDData.ECap1Temperature			= CharToInt(SubChar(chRcvData,Substr,25,3,sizeof(Substr)));
			m_HIDData.ECap2Temperature 			= CharToInt(SubChar(chRcvData,Substr,28,3,sizeof(Substr)));
			m_HIDData.ECapRoundTemperature 		= CharToInt(SubChar(chRcvData,Substr,31,3,sizeof(Substr)));
			m_HIDData.Reserve1  				= CharToInt(SubChar(chRcvData,Substr,34,3,sizeof(Substr)));
			m_HIDData.Reserve2  				= CharToInt(SubChar(chRcvData,Substr,37,3,sizeof(Substr)));
			m_HIDData.Fimware  				 	= CharToInt(SubChar(chRcvData,Substr,40,4,sizeof(Substr)));


		}
    }





	//chPreData70 = m_chHIDCommData;

	LeaveCriticalSection(&m_CS);
	 //크리티컬센션 종료

	///@todo 로그 저장 방법 확인 후 수정 필요



	return nError;
}
/**
@brief   HIDData 구조체 반환하는 함수.
@date    2013.03.14
@author  puting
@param
@return  HIDDATA 구조체 리턴
*/
HID_DATA HID_VII_ZR::GetHIDData()
{
	HID_DATA Temp_HIDData;

	EnterCriticalSection(&m_CS);
	Temp_HIDData = m_HIDData;
	LeaveCriticalSection(&m_CS);

	return Temp_HIDData;
}


char HID_VII_ZR::GetSendData_Type(int nType)
{
	char nChar ='D';

	switch(nType)
	{
		 case 0:
			nChar = 'D'; //대기
		 break;
		 case 1:
			nChar = 'A'; //가속
		 break;

		 case 2:
			nChar = 'C';  //감속
		 break;
		 case 3:
			nChar = 'B'; //등록
		 break;
	}

	return nChar;
}

char HID_VII_ZR::GetSendData_Path(int nType)
{
	char nChar ='N';

	switch(nType)
	{
		 case 0:
			nChar = 'N';   //불가
		 break;

		 case 1:
			nChar = 'E';    //직선
		 break;

		 case 2:
			nChar = 'F';    //곡선
		 break;

		 case 3:
			nChar = 'G';     //경사로
		 break;
	}

	return nChar;
}

/**
@brief   HID 2차측에 보낼 Data를 변환하는 함수
@date    2013.03.14
@author  puting
@param
@return  Send Data를 반환(16자리)
*/
void HID_VII_ZR::GetSendData(char *SendData,int nMoveInfo, int nPathInfo)
{
//	char SendData[20] ={0,};
	BYTE wCRCData;
	struct date today;
	struct time currenttime;
    int tmep;
	getdate(&today);
	gettime(&currenttime);

	SendData[0] = chSTX;

	SendData[1] = 'S';

	//OHTSection Data
	SendData[2] = GetSendData_Type(nMoveInfo); //직선

	//OHTTraveling Data
	SendData[3] =GetSendData_Path(nPathInfo); //주행

	//OHT Time Stamp Year Data
	SendData[4] = (today.da_year /1000)+ 0x30;  //2013
	SendData[5] = ((today.da_year /100)%10)+ 0x30;  //2013
	SendData[6] = ((today.da_year /10)%10)+ 0x30;  //2013
	SendData[7] = (today.da_year % 10)+ 0x30 ;

	//OHT Time Stamp Month data
	tmep = today.da_mon;
	SendData[8] = (tmep / 10) + 0x30;
	SendData[9] = (tmep % 10) +0x30;

	//OHT Time Stamp Date Data
	tmep = today.da_day;
	SendData[10] = (tmep / 10) +0x30;
	SendData[11] = (tmep % 10) +0x30;

	//OHT Time Stamp Hour Data
	tmep = currenttime.ti_hour;
	SendData[12] = (tmep / 10) +0x30;
	SendData[13] = (tmep % 10) +0x30;

	//OHT Time Stamp Minute Data
	tmep = currenttime.ti_min;
	SendData[14] = (tmep / 10) +0x30;
	SendData[15] = (tmep % 10) +0x30;

	//OHT Time Stamp Second Data
	tmep = currenttime.ti_sec;
	SendData[16] = (tmep / 10) +0x30;
	SendData[17] = (tmep % 10) +0x30;

	wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70 -1);

	SendData[18] = wCRCData;

	SendData[19] = chETX;

	//V80
//	wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70 -1);
//
//	SendData[18] = wCRCData;



//   HID_LOG("RawSendData:%s", SendData);
//	return SendData;

}

void HID_VII_ZR::GetSendData(char *SendData,int nMoveInfo, int nPathInfo, bool errorDataFlag)
{
//	char SendData[20] ={0,};
	BYTE wCRCData;
	struct date today;
	struct time currenttime;
    int tmep;
	getdate(&today);
	gettime(&currenttime);

	SendData[0] = chSTX;

	if(errorDataFlag) SendData[1] = 'E';
	else SendData[1] = 'S';

	//OHTSection Data
	SendData[2] = GetSendData_Type(nMoveInfo); //직선

	//OHTTraveling Data
	SendData[3] =GetSendData_Path(nPathInfo); //주행

	//OHT Time Stamp Year Data
	SendData[4] = (today.da_year /1000)+ 0x30;  //2013
	SendData[5] = ((today.da_year /100)%10)+ 0x30;  //2013
	SendData[6] = ((today.da_year /10)%10)+ 0x30;  //2013
	SendData[7] = (today.da_year % 10)+ 0x30 ;

	//OHT Time Stamp Month data
	tmep = today.da_mon;
	SendData[8] = (tmep / 10) + 0x30;
	SendData[9] = (tmep % 10) +0x30;

	//OHT Time Stamp Date Data
	tmep = today.da_day;
	SendData[10] = (tmep / 10) +0x30;
	SendData[11] = (tmep % 10) +0x30;

	//OHT Time Stamp Hour Data
	tmep = currenttime.ti_hour;
	SendData[12] = (tmep / 10) +0x30;
	SendData[13] = (tmep % 10) +0x30;

	//OHT Time Stamp Minute Data
	tmep = currenttime.ti_min;
	SendData[14] = (tmep / 10) +0x30;
	SendData[15] = (tmep % 10) +0x30;

	//OHT Time Stamp Second Data
	tmep = currenttime.ti_sec;
	SendData[16] = (tmep / 10) +0x30;
	SendData[17] = (tmep % 10) +0x30;


	wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70 -1);
	SendData[18] = wCRCData;
	SendData[19] = chETX;

}
/**
@brief   HID 2차측에 보낼 Data의 CheckSum을 만드는 함수
@date    2013.03.14
@author  puting
@param
@return  BYTE 리턴
*/
BYTE HID_VII_ZR::CheckSUMData(char* SendData, int Len)
{
	WORD    wCalcValue = 0, wBuff = 0;
	int	i = 0;

	for(i = 0; i < Len; i++)
	{
		wBuff = SendData[i] & 0x00FF;

		wCalcValue += wBuff;

	}

	return wCalcValue & 0x00FF;
}
/**
@brief   HID 2차측에 받은 Data의 CheckSum이 정상인지 확인하는
@date    2013.03.14
@author  puting
@param
@return  정상일 경우 true, 비정상일 경우 false 리턴
*/
bool  HID_VII_ZR::GetRcvDataCheckSum(char* RcvData, int Len)
{
 	WORD    wCalcValue = 0, wBuff = 0;
	int	i = 0;

	for(i = 0; i < Len; i++)
	{
		wBuff = RcvData[i] & 0x00FF;

		wCalcValue += wBuff;

	}
	if(RcvData[RCV_PK_LENGTH70-1] == (char)(wCalcValue & 0x00FF))
		return true;
	else
		return false;
}

/**
@brief   HID 2차측에 보낼 Data의 CheckSum을 만드는 함수
@date    2013.03.14
@author  puting
@param
@return  BYTE 리턴
*/
/*
char HID_VII_ZR::CheckSUMData_V85(char* SendData, int Len, int Num)
{
	WORD    wCalcValue = 0, wBuff = 0;
	char Tmep[10] = {0,};

	int	i = 0;

	for(i = 0; i < Len; i++)
	{
		wBuff = SendData[i] & 0x00FF;

		wCalcValue += wBuff;

	}

	if(Num ==1)
		sprintf(Tmep, "%x", (wCalcValue & 0x00F0)>>4);
	else
		sprintf(Tmep, "%x", (wCalcValue & 0x000F));

	return Tmep[0];
}
*/
bool  HID_VII_ZR::GetRcvDataCheckSum_V85(char* RcvData, int Len)
{
	WORD    wCalcValue = 0, wBuff = 0;
	char Tmep[10] = {0,};
	bool breturn = false;

	char Check1, Check2;
	int	i = 0;

	for(i = 0; i < Len; i++)
	{
		wBuff = RcvData[i] & 0x00FF;

		wCalcValue += wBuff;

	}
	sprintf(Tmep, "%x", (wCalcValue & 0x00F0)>>4);
	Check1 = Tmep[0];
	sprintf(Tmep, "%x", (wCalcValue & 0x000F));
	Check2 = Tmep[0];

	if((RcvData[76] == Check1) &&
	  (RcvData[77] == Check2))
		return true;
	else
		return false;
}

/**
@brief   HID 2차측에 받은 Data 중 Error Code를 메인에 맞게 변환
@date    2013.11.12
@author  puting
@param
@return  에러코드
*/
int HID_VII_ZR::ErrorConvert(int nerrorcode)
{
	int nErrorCode = 0;
	switch(nerrorcode)
	{
	case 1:
		nErrorCode = ERR_EDC_PEAKVOLTAGE;
		break;
	case 2:
		nErrorCode = ERR_EDC_OVERVOLTAGE;
		break;
	case 3:
		nErrorCode = ERR_EOUT_PEAKVOLTAGE;
		break;
	case 4:
		nErrorCode = ERR_EOUT_OVERVOLTAGE;
		break;
	case 5:
		nErrorCode = ERR_ECAP_PEAKVOLTAGE;
		break;
	case 6:
		nErrorCode = ERR_ECAP_OVERVOLTAGE;
		break;
	case 7:
		nErrorCode = ERR_IAC_PEAKCURRENT;
		break;
	case 8:
		nErrorCode = ERR_IAC_OVERCURRENT;
		break;
	case 9:
		nErrorCode = ERR_OUTPUT_PEAKCURRENT;
		break;
	case 10:
		nErrorCode  =   ERR_OUTPUT_OVERCURRENT;
		break;
	case 11:
		nErrorCode = ERR_OUTPUT_FUSE_OPEN;
		break;
	case 12:
		nErrorCode = ERR_ECAP_FUSE_OPEN;
		break;
	case 13:
		nErrorCode = ERR_PICKUP_PEAKTEMP;
		break;
	case 14:
		nErrorCode  = ERR_IGBT_PEAKTEMP;
		break;
	case 15:
		nErrorCode = ERR_ECAP_PEAKTEMP;
		break;
	case 16:
		nErrorCode = ERR_GATE_DRIVER_FAULT;
		break;
	case 17:
		nErrorCode = ERR_HEATSINK_OVERTEMP;
		break;
	case 18:
		nErrorCode = ERR_PROTECT_BD_CABLE_OPEN;
		break;
	case 19:
		nErrorCode  = ERR_OUTPUT_PEAKPOWER;
		break;
	case 20:
		nErrorCode = ERR_OUTPUT_OVERPOWER;
		break;
	case 51:
		nErrorCode =  ERR_EDC_UNDERVOLTAGE;
		break;
	case 52:
		nErrorCode = ERR_EDC_LOWVOLTAGE;
		break;
	case 53:
		nErrorCode = ERR_EOUT_UNDERVOLTAGE;
		break;
	case 54:
		nErrorCode = ERR_EOUT_LOWVOLTAGE;
		break;
	case 55:
		nErrorCode = ERR_ECAP_UNDERVOLTAGE;
		break;
	case 56:
		nErrorCode = ERR_ECAP_LOWERVOLTAGE;
		break;
	case 57:
		nErrorCode = ERR_PICKUP_OVERTEMP;
		break;
	case 58:
		nErrorCode = ERR_IGBT_OVERTEMP;
		break;
	case 59:
		nErrorCode = ERR_ECAP_OVERTEMP;
		break;
//	case 60:
//		nErrorCode
//		break;
//	case 61:
//		nErrorCode
//		break;
	case 62:
		nErrorCode = ERR_SEG_GEN_SIGNAL;
		break;
	case 63:
		nErrorCode = ERR_MULTI_SMPS_FAULT;
		break;
	default:
		nErrorCode = 0;
		break;
	}

	if(nErrorCode != NO_ERR)
	{
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					nErrorCode,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}

	return nErrorCode;

}


// 레귤레이터 오류시 TP를 통해 로그 저장을 지시하면, 이 함수가 호출되어 로그를 저장한다
// 로그 저장이 약 5분간 걸리며, 이 동안 다른일을 하면 안된다.
#define HID_ERROR_NO_ERR				0				// 동작중 에러없이 완료
#define HID_ERROR_SERIAL_CAN_T_OPEN 	ERR_HID_OPEN	// 시리얼포트를 열지 못했음
#define HID_ERROR_SERIAL_WRITE			ERR_HID_WRITE
#define HID_ERROR_SERIAL_TIMEOUT		333


int HID_VII_ZR::SaveErrorDataLog(void)
{
	char chSendBuff[255]={0,};
	volatile char chReadBuff[1024]={0,};
	char chDataEndBuff[8]={0,};
	int nError=HID_ERROR_NO_ERR;
	bool bErrorRequest = 1;
	ofstream mfsb;
	
	String strFileName = "D:\\log\\HIDLogDump"+Now().FormatString("yyyymmddhhnn")+".dat" ;
	mfsb.open(strFileName.c_str(),ios::out|ios::binary);

	if(mfsb.is_open()){
		//PurgeComm(m_HIDCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
		GetSendData(chSendBuff, 0, 0, bErrorRequest);
				  //	memmove(chSendBuff,chWriteBuff, SEND_PK_LENGTH70+2);
		nError = Write(chSendBuff);

		if(nError!=NO_ERR){
			OHTMainForm->memoMain->Lines->Add("Write fail...");
			return nError;
		}
	}

	Sleep(50);

	volatile char chBuff;
	int nData;
	DWORD		dwStartTime = timeGetTime();
	DWORD		dwEndTime;
	OHTMainForm->memoMain->Lines->Add("file open");
	int index = 0;

	while(1){

		if(m_HIDCommPort->ReadCommBlock((char*)&chBuff,1) == 0) {
			Sleep(10);

			// 읽어지지 않았다면 현재 시간을 먼저 보고
			dwEndTime = timeGetTime();

			if ((dwEndTime - dwStartTime) > 3000) // 읽어지지 않은 시간이 100ms 이상이면 타임아웃
			{
				OHTMainForm->memoMain->Lines->Add("Time Out");
				nError = HID_ERROR_SERIAL_TIMEOUT;
				break;
			}
			else
				continue;
		}

		// 데이터가 읽어졌다면 새로운 시간을 저장
		dwStartTime = timeGetTime();

		chReadBuff[index++] = chBuff;



		memcpy(&chDataEndBuff[1], &chDataEndBuff[0], sizeof(chDataEndBuff)-1);
		chDataEndBuff[0] = chBuff;
		if(chDataEndBuff[7] == 'D' &&
		   chDataEndBuff[6] == 'A' &&
		   chDataEndBuff[5] == 'T' &&
		   chDataEndBuff[4] == 'A' &&
		   chDataEndBuff[3] == 'E' &&
		   chDataEndBuff[2] == 'N' &&
		   chDataEndBuff[1] == 'D' &&
		   chDataEndBuff[0] == chETX){
		   OHTMainForm->memoMain->Lines->Add("Write Success");
			mfsb.write((char*)chReadBuff, index);
			break;
		}
		if(index == 1024)
		{
			mfsb.write((char*)chReadBuff, 1024);
			index = 0;
		}
	}
	mfsb.close();
	// OHTMainForm->tmHID->Enabled = false;
	OHTMainForm->memoMain->Lines->Add("file close");

	return nError;
}
/////////////////////////////////////////////////////////////////////////////////////
//추후에 추가적으로 필요한거 순수 가상 클래스 및 함수화를 통해 HID6.5/7.0을 따로 분류
