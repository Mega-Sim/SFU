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

#define HID_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\HIDRawData.log", __VA_ARGS__)
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
	GetVer = false;
	BMS_Ver = false;     //기본은 통합 CP
	Ver_Cnt = 0;

    RegulatorCheck = false;
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

	unsigned char chReadBuff[255]={0,};
//	char chReadBuff[255]={0,};
	char chSendBuff[255]={0,};

	int TempMoveinfo = nMoveInfo;
	int TempnPathInfo = nPathInfo;

	int nRtnByteNo = 0;

	//1. Open Check puting
	// HID가 열리지 않은 상태이므로 에러를 바로 리턴해야함.
	if(m_HIDCommPort == NULL)
		return ERR_HID_OPEN;

	HID_LOG("dwHIDTrigerTime = %d", OHTUTILITY::PreventOverflowGetTimeMs(dwHIDTrigerTime));    //HIDTrigerTime 체크

	//2. 데이터 읽기.
	//if( bHIDOnTriger==true)) //가능은 하나 데이터 보장을 못함.
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	if(OHTUTILITY::PreventOverflowGetTimeMs(dwHIDTrigerTime) > 200)
	{
		nRtnByteNo = Read(chReadBuff, chETX);
		HID_LOG(">>nErrorCount=%d, Recv Data Len = %d, chReadBuff = %s", nErrorCount, strlen(chReadBuff), chReadBuff);     //Recv 로그 확인용&이 로직을 타는지 이중확인
		HID_LOG("Checksum=%02x, Last=%02x", chReadBuff[strlen(chReadBuff)-2], chReadBuff[strlen(chReadBuff)-1]);     //데이터 이상하게 짤리는거 확인용
//		HID_LOG("Vcell Data = %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",
//				chReadBuff[117],chReadBuff[118],chReadBuff[119],chReadBuff[120],chReadBuff[121],chReadBuff[122],chReadBuff[123],
//				chReadBuff[124],chReadBuff[125],chReadBuff[126],chReadBuff[127],chReadBuff[128],chReadBuff[129],chReadBuff[130],chReadBuff[131],
//				chReadBuff[132],chReadBuff[133],chReadBuff[134],chReadBuff[135],chReadBuff[136],chReadBuff[137],chReadBuff[138],chReadBuff[139],
//				chReadBuff[140],chReadBuff[141],chReadBuff[142],chReadBuff[143]);

		if(nRtnByteNo == 0)
		{
			nErrorCount++;
		}
		else
		{
			trim(chReadBuff);

			if(BMS_Ver == true)
			{
				ltrim(chReadBuff);
				HID_LOG("AFTER TRIM LEN = %d",strlen(chReadBuff));
			}
			else
			{
				trim(chReadBuff);
				HID_LOG("AFTER TRIM LEN = %d",strlen(chReadBuff));
//			for(int number = 116; number<strlen(chReadBuff); number++)
//			{
//				HID_LOG("%02x,", chReadBuff[number]);
//			}
				if(nBeforeLen-2 > strlen(chReadBuff)) chReadBuff[strlen(chReadBuff)] = recvChecksum;      //끝단에 trim으로 지워진 checksum 추가
				HID_LOG("AFTER Last Change LEN = %d",strlen(chReadBuff));
			}

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
		}
		// 5번이상  받은 데이터가 없을경우, 다시 읽기 시도.
		if(nErrorCount > RETRY_HID_MAX_COUNT)
		{
				bHIDOnTriger= false;
				nErrorCount =0;
				return ERR_HID_READTIMEOUT;
		}
	}
#else
	if(OHTUTILITY::PreventOverflowGetTimeMs(dwHIDTrigerTime) > 300)
	{
		nRtnByteNo = Read(chReadBuff, chETX);
		HID_LOG(">>nErrorCount=%d, Recv Data Len = %d, chReadBuff = %s", nErrorCount, strlen(chReadBuff), chReadBuff);     //Recv 로그 확인용&이 로직을 타는지 이중확인

		if(nRtnByteNo == 0)
		{
			nErrorCount++;
		}
		else
		{
			int nBeforeLen = 0;
			unsigned char recvChecksum = chReadBuff[strlen(chReadBuff)-2];
			nBeforeLen = strlen(chReadBuff);

			if(BMS_Ver == true)
			{
				ltrim(chReadBuff);
				HID_LOG("AFTER TRIM LEN = %d",strlen(chReadBuff));
			}
			else
			{
				trim(chReadBuff);
				HID_LOG("AFTER TRIM LEN = %d",strlen(chReadBuff));
//			for(int number = 116; number<strlen(chReadBuff); number++)
//			{
//				HID_LOG("%02x,", chReadBuff[number]);
//			}
				if(nBeforeLen-2 > strlen(chReadBuff)) chReadBuff[strlen(chReadBuff)] = recvChecksum;      //끝단에 trim으로 지워진 checksum 추가
				HID_LOG("AFTER Last Change LEN = %d",strlen(chReadBuff));
			}

			HID_LOG("chReadBuff = %s", chReadBuff);

			if(RegulatorCheck == true && GetVer == false)  //regulator 종류 받기
			{
				if((GetRcvDataCheckSum(chReadBuff, 8)!=true)&&chReadBuff[0]!='g')
				{
					HID_LOG("Error 발생chReadBuff[0] = %c", chReadBuff[0]);
					return RERTY_HID_SEND_DATA;
				}
				else
				{
					GetVer = true;       //regulator 종류 가져오기 완료
					HID_LOG("First Recv Data length = %d", strlen(chReadBuff));
					HID_LOG("Recv Data = %s", chReadBuff);
					if(chReadBuff[1] == 'e')
						BMS_Ver = true;

					HID_LOG("By Receive Data GetVer=%d, BMS_Ver=%d", GetVer, BMS_Ver);
					nErrorCount = 0;
				}
			}
			else
			{
				//1. 받은 데이터 분석/ 구조체 적용
				nError = AnalysisData(chReadBuff);

				if(nError != NO_ERROR)
					return nError;

				//*********이경우에만 데이터를 실제로 받음*************
				//데이터를 받을 경우, 누적하지 않기위해 초기화
				nErrorCount =0;
				//2. 데이터 보내기.
				HID_LOG("GetSend Check 1");
				GetSendData(chSendBuff, nMoveInfo, nPathInfo);
				HID_LOG("Send = %s", chSendBuff);         //끝나는 시간 확인겸 데이터 확인
				nError = Write(chSendBuff);
				dwHIDTrigerTime	= timeGetTime();
				bHIDOnTriger = true;
				//**************나머지 부분은 retry이거나 실패*********
			}
		}
		// 5번이상  받은 데이터가 없을경우, 다시 읽기 시도.
		if(nErrorCount > RETRY_HID_MAX_COUNT)
		{
				HID_LOG("!!Read Again!!");
				bHIDOnTriger= false;
				nErrorCount =0;
				return ERR_HID_READTIMEOUT;
		}
	}
#endif
	//초기 시작때 데이터 보내기 위해. 혹은 데이터를 연속해서 못받을 경우 다시 시도 하기 위해

	if(bHIDOnTriger == false)
	{
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
    	GetSendData(chSendBuff, nMoveInfo, nPathInfo);
		HID_LOG("Send = %s", chSendBuff);
#else
		if(RegulatorCheck == true && GetVer == false)
		{
			if(Ver_Cnt > 4)      //5번 이상 시도했는데 실패했으면 통합버전으로
			{
				HID_LOG("Set Regulator Version 통합CP");
				GetVer = true;
			}
			else
			{
				SendFirstData(chSendBuff);
				Ver_Cnt++;     //regulator 버전 가져오려 SendData 보낸 횟수
				HID_LOG("Regulator Version Cehck 횟수 : %d", Ver_Cnt);
			}
		}
		else
		{
			//2. 데이터 보내기.
			HID_LOG("GetSend Check ------ 2");
			GetSendData(chSendBuff, nMoveInfo, nPathInfo);
			HID_LOG("Send = %s", chSendBuff);             //끝나는 시간 확인겸 데이터 확인
		}
#endif
		nError = Write(chSendBuff);
		dwHIDTrigerTime	= timeGetTime();

		return RERTY_HID_SEND_DATA;
	}

	return nError;
}

//처음 시작할 때 타서 Regulator 버전 가져오는 함수   - sseung
void HID_VII_ZR::SendFirstData(char* SendData)
{
	int Error;

	SendData[0] = chSTX;
	SendData[1] = 'v';
	SendData[2] = '0';
	SendData[3] = '0';   //헥사로는 0x30이라서 recv가 안옴
	SendData[4] = 'v';
	SendData[5] = chETX;

	HID_LOG("First Send = %s, Len=%d", SendData, strlen(SendData));
//	HID_LOG("First Send = %c,%c,%c,%c,%c", chSendBuff[0],chSendBuff[1],chSendBuff[2],chSendBuff[4],chSendBuff[5]);
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
	static int nSerialCommCheck = 0;
	int nRecvLen = 0;

	  //Form1->Memo1->Lines->Add(chRcvData);
	//1. data 정합성 확인(길이)
//	int lens = strlen(chRcvData);
//	if(lens!=RCV_PK_LENGTH70)                           //HID Data 고정 길이  6.5일 경우
//   		return ERR_DEFFER_HID_RCV_DATA;      //받은 HID 데이터 길이가 다를 경우

	//크리티컬섹션 시작
	EnterCriticalSection(&m_CS);
	nRecvLen = strlen(chRcvData);
	HID_LOG("Recv Data length = %d, nSerialCommCheck = %d", nRecvLen, nSerialCommCheck);   //recv data 길이로 v85 루트 타는지 확인

#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	if(nRecvLen > RCV_PK_PROTOCOL_LENSTD)
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
	}
	else
	{
		//recv 크기도 다름.. RCV_PK_LENGTH70
		if((GetRcvDataCheckSum_V85(chRcvData, RCV_PK_LENGTH70-1)!=true)&&(chRcvData[0]!='s'))
		{
			memset(&m_HIDData, 0x00, sizeof(m_HIDData));
			nError = ERR_HID_READTIMEOUT;   //받은 데이터가 이상할 경우.
		}
		else
		{
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
#else
	if(nRecvLen == (RCV_PK_LENGTH_CP+1) && (chRcvData[37]=='0' && chRcvData[38]=='8' && chRcvData[39]=='5'))  //길이가 동일하고 Data가 85 버전일때
		bV85Protocol = true;
	else if(nRecvLen != (RCV_PK_LENGTH70+1) && nRecvLen != (RCV_PK_LENGTH_ECO+1))  //2.s식별자 및 CheckSum 확인
	{
		HID_LOG("Data Length Diff");
		nSerialCommCheck++;
		if(nSerialCommCheck > 3)
		{
			nSerialCommCheck = 0;
			memset(&m_HIDData, 0x00, sizeof(m_HIDData));
			nError = ERR_HID_READTIMEOUT;   //받은 데이터가 이상할 경우.
		}

		LeaveCriticalSection(&m_CS);
		return nError;
	}

	if(RegulatorCheck)     //ECO나 통합 CP일 때
	{
		if(BMS_Ver && (nRecvLen!=(RCV_PK_LENGTH_BMS+1) || (GetRcvDataCheckSum_V85(chRcvData, RCV_PK_LENGTH_BMS-1)!=true) || (chRcvData[0]!='s')))
		{
			HID_LOG("ECO ver. ");
			nSerialCommCheck++;
		}
		else if(!BMS_Ver && bV85Protocol==true && (nRecvLen!=(RCV_PK_LENGTH_CP+1) || (GetRcvDataCheckSum_V85(chRcvData, RCV_PK_LENGTH_CP)!=true) || (chRcvData[0]!='s')))  //regulator ver.85이면 81ver checksum 사용
		{
			HID_LOG("85ver. ");
			nSerialCommCheck++;
		}
		else if(!BMS_Ver && bV85Protocol==false && (nRecvLen!=(RCV_PK_LENGTH70+1) || GetRcvDataCheckSum(chRcvData, RCV_PK_LENGTH70)!=true || chRcvData[0]!='s'))  //RCV_PK_LENGTH70 regulaotr 종류에 맞게 조건 필요
		{
			HID_LOG("통합 CP ver.");
			nSerialCommCheck++;
		}
		else
		{
			if(BMS_Ver)
			{
				m_HIDData.InputStatus				= chRcvData[1];
				m_HIDData.BMSStatus					= chRcvData[2];
				m_HIDData.BatteryStatus				= chRcvData[3];
				m_HIDData.ExCharge					= chRcvData[4];
				m_HIDData.ErrorCode					= CharToInt(SubChar(chRcvData,Substr,5,3,sizeof(Substr)));
				m_HIDData.ErrorCode					= ErrorConvert(m_HIDData.ErrorCode);    //puting  2013.11.12
				m_HIDData.Iac						= CharToInt(SubChar(chRcvData,Substr,8,3,sizeof(Substr)));
				m_HIDData.BMS_Edc					= CharToInt(SubChar(chRcvData,Substr,11,3,sizeof(Substr)));
				m_HIDData.BMS_Iout_Sign 			= chRcvData[14];
				m_HIDData.BMS_Iout					= CharToInt(SubChar(chRcvData,Substr,15,3,sizeof(Substr)));
				m_HIDData.BVoltage					= CharToInt(SubChar(chRcvData,Substr,18,4,sizeof(Substr)));

				m_HIDData.BIout_Sign 				= chRcvData[22];
				m_HIDData.BCurrent					= CharToInt(SubChar(chRcvData,Substr,23,5,sizeof(Substr)));
				m_HIDData.ConverterEfficiency		= CharToInt(SubChar(chRcvData,Substr,28,3,sizeof(Substr)));
				m_HIDData.PickupTemperature			= CharToInt(SubChar(chRcvData,Substr,31,3,sizeof(Substr)));
				m_HIDData.HeatsinkTemperature		= CharToInt(SubChar(chRcvData,Substr,34,2,sizeof(Substr)));
				m_HIDData.ECap1Temperature			= CharToInt(SubChar(chRcvData,Substr,36,2,sizeof(Substr)));
				m_HIDData.ECap2Temperature			= CharToInt(SubChar(chRcvData,Substr,38,2,sizeof(Substr)));
				m_HIDData.ECapRoundTemperature		= CharToInt(SubChar(chRcvData,Substr,40,2,sizeof(Substr)));
				m_HIDData.CTransTemperature			= CharToInt(SubChar(chRcvData,Substr,42,3,sizeof(Substr)));
				m_HIDData.CMosFETPriTemperature		= CharToInt(SubChar(chRcvData,Substr,45,3,sizeof(Substr)));

				m_HIDData.CMOsFETSecTemperature		= CharToInt(SubChar(chRcvData,Substr,48,3,sizeof(Substr)));
				m_HIDData.Voltage5					= CharToInt(SubChar(chRcvData,Substr,51,2,sizeof(Substr)));
				m_HIDData.Current5					= CharToInt(SubChar(chRcvData,Substr,53,2,sizeof(Substr)));
				m_HIDData.Voltage24					= CharToInt(SubChar(chRcvData,Substr,55,3,sizeof(Substr)));
				m_HIDData.Current24					= CharToInt(SubChar(chRcvData,Substr,58,3,sizeof(Substr)));
				m_HIDData.Voltage27					= CharToInt(SubChar(chRcvData,Substr,61,3,sizeof(Substr)));
				m_HIDData.Current27					= CharToInt(SubChar(chRcvData,Substr,64,2,sizeof(Substr)));
				m_HIDData.Regulator_ver				= CharToInt(SubChar(chRcvData,Substr,66,3,sizeof(Substr)));
				m_HIDData.Fimware					= CharToInt(SubChar(chRcvData,Substr,69,3,sizeof(Substr)));
				m_HIDData.Soc_per					= CharToInt(SubChar(chRcvData,Substr,72,3,sizeof(Substr)));

				m_HIDData.Soc  						= CharToInt(SubChar(chRcvData,Substr,75,5,sizeof(Substr)));
				m_HIDData.Soh  						= CharToInt(SubChar(chRcvData,Substr,80,3,sizeof(Substr)));
				m_HIDData.BMS_Voltage				= CharToInt(SubChar(chRcvData,Substr,83,4,sizeof(Substr)));
				m_HIDData.Ref						= CharToInt(SubChar(chRcvData,Substr,87,3,sizeof(Substr)));
				m_HIDData.Current_Sign				= chRcvData[90];
				m_HIDData.Current					= CharToInt(SubChar(chRcvData,Substr,91,5,sizeof(Substr)));
				m_HIDData.Max_Temp					= CharToInt(SubChar(chRcvData,Substr,98,3,sizeof(Substr)));
				m_HIDData.Min_Temp					= CharToInt(SubChar(chRcvData,Substr,99,5,sizeof(Substr)));
				m_HIDData.PCBTemperature			= CharToInt(SubChar(chRcvData,Substr,102,5,sizeof(Substr)));
				m_HIDData.StartDate					= CharToInt(SubChar(chRcvData,Substr,105,6,sizeof(Substr)));

				m_HIDData.TotalTime					= SubChar(chRcvData,Substr,111,8,sizeof(Substr)); // Hex값 사용
				m_HIDData.TotalIac					= CharToInt(SubChar(chRcvData,Substr,119,7,sizeof(Substr)));
				m_HIDData.TotalIout					= CharToInt(SubChar(chRcvData,Substr,126,7,sizeof(Substr)));

				for(int i=0; i<11; i++){
					m_HIDData.CellVoltage[i]					= CharToInt(SubChar(chRcvData,Substr,133+(i*4),4,sizeof(Substr)));
				}
			}
			else
			{
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
				m_HIDData.Regulator_ver  			= CharToInt(SubChar(chRcvData,Substr,37,3,sizeof(Substr)));
				m_HIDData.Fimware  				 	= CharToInt(SubChar(chRcvData,Substr,40,4,sizeof(Substr)));
				m_HIDData.Voltage12                 = CharToInt(SubChar(chRcvData,Substr,44,3,sizeof(Substr)));
				m_HIDData.Voltage5                  = CharToInt(SubChar(chRcvData,Substr,47,3,sizeof(Substr)));
				m_HIDData.Voltage12_Sign            = chRcvData[50];
				m_HIDData.Voltage12_Minus           = CharToInt(SubChar(chRcvData,Substr,51,3,sizeof(Substr)));
				m_HIDData.Voltage24                 = CharToInt(SubChar(chRcvData,Substr,54,3,sizeof(Substr)));
				m_HIDData.Voltage27                 = CharToInt(SubChar(chRcvData,Substr,57,3,sizeof(Substr)));
				m_HIDData.Current5                  = CharToInt(SubChar(chRcvData,Substr,60,3,sizeof(Substr)));
				m_HIDData.Current24                 = CharToInt(SubChar(chRcvData,Substr,63,3,sizeof(Substr)));
				m_HIDData.Current27                 = CharToInt(SubChar(chRcvData,Substr,66,3,sizeof(Substr)));
				m_HIDData.Current12                 = CharToInt(SubChar(chRcvData,Substr,69,3,sizeof(Substr)));
				m_HIDData.Current12_Minus           = CharToInt(SubChar(chRcvData,Substr,72,3,sizeof(Substr)));
				m_HIDData.SMPS_ErrorCode            = CharToInt(SubChar(chRcvData,Substr,75,2,sizeof(Substr)));
				m_HIDData.SMPS_Firmware             = CharToInt(SubChar(chRcvData,Substr,77,3,sizeof(Substr)));
				m_HIDData.Reserve2                  = CharToInt(SubChar(chRcvData,Substr,80,2,sizeof(Substr)));
				m_HIDData.Reserve3                  = CharToInt(SubChar(chRcvData,Substr,82,2,sizeof(Substr)));
			}
			nSerialCommCheck = 0;

			if(BMS_Ver)
			{
				// BMS 통합 파워 모듈 상태  정보 sg.lee 25.06.26
				switch (m_HIDData.BatteryStatus)
				{
					// 정상
					case 'A': // 충전 중
					case 'B': // 방전 중
					case 'C': // 셀 밸런싱 진행 중
					case 'D': // 배터리 SOC가 15% 이하로 떨어질 경우 충전 요청
					case 'E': //배터리 SOC가 10% 이하 시 알람 발생 => 10% 이하 200s 유지 시 과방전 방지 전원 OFF
						break;

					case 'F':
						HID_LOG("!Warning! Cell 간 전압 편차 0.5V 이상");
						break;

					case 'G':
						HID_LOG("!ERROR! CHARGE DISABLE");
						break;
					case 'H':
						HID_LOG("!ERROR! DISCHARGE DISABLE");
						break;

					case 'I':
						HID_LOG("!ERROR! OVER CURRENT");
						break;

					case 'J':
						HID_LOG("!ERROR! OVER TEMPERATURE");
						break;

					case 'K':
						HID_LOG("!ERROR! 충전 중 Cell 간 전압 편차 0.55V 이상");
						break;

					case 'L':
						HID_LOG("!ERROR! 방전 중 Cell 간 전압 편차 0.55V 이상");
						break;

					case 'M':
						HID_LOG("!ERROR! EMERGENCY!!!"); // Fault Error 전부
						break;
					
					default:
						break;
				}

				/*
				if(m_HIDData.Fault_Code == NO_ERR)
				{
					//배터리 데이터 중 Error 수치에 도달한 경우 에러 발생
					if(m_HIDData.CD == 'C')
					{
						if(m_HIDData.Max >= 3600)
						{
							HID_LOG("!ERROR! 셀 과전압 발생. 현재 셀 전압: %d", m_HIDData.Max);
							nError = ERR_CELL_OVERVOLTAGE;
						}

						if(m_HIDData.Bms >= 287)   //소수점단위 받아오지 않음 -> 소수점 버림
						{
							HID_LOG("!ERROR! Pack 충전 과전압 발생. 현재 최대 전압: %d", m_HIDData.Bms);
							nError = ERR_PACK_OVERVOLTAGE;
						}

						if(m_HIDData.Current_Sign == '+')
						{
							if(m_HIDData.Current >= 33)  //33 = 3.3, 데이터 저장시에 소수점 표시X
							{
								HID_LOG("!ERROR! 배터리 과전류  발생. 현재 배터리 전류: %d", m_HIDData.Current);
								nError = ERR_CHARGE_OVER;
							}
						}
					}
					else if(m_HIDData.CD == 'D')
					{
						if(m_HIDData.Bms <= 260)     //소수점단위 받아오지 않음 -> 소수점 버림
						{
							HID_LOG("!ERROR! Pack 방전 저전압 발생. 현재 최소 전압: %d", m_HIDData.Bms);
							nError = ERR_PACK_UNDERVOLTAGE;
						}

						if(m_HIDData.Current_Sign == '-')
						{
							if(m_HIDData.Current >= 121)  //지정된 자리수 넘어감 -> 자리수 추가
							{
								HID_LOG("!ERROR! 배터리 저전류  발생. 현재 배터리 전류: %d", m_HIDData.Current);
								nError = ERR_DISCHARGE_OVER;
							}
						}
					}

					if(m_HIDData.Min <= 3170)
					{
						HID_LOG("!ERROR! 셀 저전압 발생. 현재 셀 전압: %d", m_HIDData.Min);
						nError = ERR_CELL_UNDERVOLTAGE;
					}

					if(m_HIDData.Max_Temp_Sign == '+')
					{
						if(m_HIDData.Max_Temp >= 40)
						{
							HID_LOG("!ERROR! 셀 과열 발생. 현재 셀 온도: %d", m_HIDData.Max_Temp);
							nError = ERR_CELL_OVERTEMP;
						}
					}
				}
				else
				{
					HID_LOG("Battery Error Code: %d", m_HIDData.Fault_Code);
//					m_HIDData.ErrorCode = ERR_HID_WRITE;
				}
				*/
			}

			if(nError != NO_ERR)
			{
				m_HIDData.ErrorCode = nError;
				return 0; //에러만 발생시키고 레귤레이터 값은 정상 전달 필요
			}
		}
	}
	else if(bV85Protocol == false)
	{
		if((GetRcvDataCheckSum(chRcvData, RCV_PK_LENGTH70)!=true) || (chRcvData[0]!='s'))
		{
			nSerialCommCheck++;
		}
		else
		{
	//	   HID_LOG("RawData:%s", chRcvData);
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
			nSerialCommCheck = 0;
		}
	}
	else
	{
		HID_LOG("chRcvData[0] = %c", chRcvData[0]); //0번 배열에 뭐 들어가있는지
		//recv 크기도 다름.. RCV_PK_LENGTH70
		if(nRecvLen!=(RCV_PK_LENGTH_CP+1) || (GetRcvDataCheckSum_V85(chRcvData, RCV_PK_LENGTH_CP)!=true) || (chRcvData[0]!='s'))
		{
			nSerialCommCheck++;
		}
		else
		{
			m_HIDData.Edc 						= CharToInt(SubChar(chRcvData,Substr,1,3,sizeof(Substr)));;
			m_HIDData.Ecap 						= CharToInt(SubChar(chRcvData,Substr,4,3,sizeof(Substr)));
			m_HIDData.Eout  					= CharToInt(SubChar(chRcvData,Substr,7,3,sizeof(Substr)));
			m_HIDData.Iout_sign 		   		= chRcvData[10];
			m_HIDData.Iout 						= CharToInt(SubChar(chRcvData,Substr,11,3,sizeof(Substr)))
			m_HIDData.Iac 						= CharToInt(SubChar(chRcvData,Substr,14,3,sizeof(Substr)));
			m_HIDData.ErrorCode  			    = CharToInt(SubChar(chRcvData,Substr,17,2,sizeof(Substr)));
			m_HIDData.ErrorCode 				= ErrorConvert(m_HIDData.ErrorCode);    //puting  2013.11.12
			m_HIDData.PickupTemperature 		= CharToInt(SubChar(chRcvData,Substr,19,3,sizeof(Substr)));
			m_HIDData.IGBTTemperature			= CharToInt(SubChar(chRcvData,Substr,22,3,sizeof(Substr)));
			m_HIDData.ECap1Temperature			= CharToInt(SubChar(chRcvData,Substr,25,3,sizeof(Substr)));
			m_HIDData.ECap2Temperature 			= CharToInt(SubChar(chRcvData,Substr,28,3,sizeof(Substr)));
			m_HIDData.ECapRoundTemperature 		= CharToInt(SubChar(chRcvData,Substr,31,3,sizeof(Substr)));
			m_HIDData.Reserve1  				= CharToInt(SubChar(chRcvData,Substr,34,3,sizeof(Substr)));
			m_HIDData.Regulator_ver  			= CharToInt(SubChar(chRcvData,Substr,37,3,sizeof(Substr)));
			m_HIDData.Fimware  				 	= CharToInt(SubChar(chRcvData,Substr,40,4,sizeof(Substr)));
			nSerialCommCheck = 0;
		}
    }

	if(nSerialCommCheck > 3)
	{
		nSerialCommCheck = 0;
		memset(&m_HIDData, 0x00, sizeof(m_HIDData));
		nError = ERR_HID_READTIMEOUT;   //받은 데이터가 이상할 경우.
	}
	//Recv Data 출력문
	if(nSerialCommCheck == 0)
	{
		if(BMS_Ver)
		{
			HID_LOG("BMS Recv Data = %c,%c,%c,%c,%d,%d,%d,%c,%d,%d,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%c,%d,%d,%d,%d,%d,%s,%d,%d",
					m_HIDData.InputStatus, m_HIDData.BMSStatus, m_HIDData.BatteryStatus, m_HIDData.ExCharge, m_HIDData.ErrorCode,
					m_HIDData.Iac, m_HIDData.BMS_Edc, m_HIDData.BMS_Iout_Sign, m_HIDData.BMS_Iout, m_HIDData.BVoltage, m_HIDData.BIout_Sign, m_HIDData.BCurrent,
					m_HIDData.ConverterEfficiency, m_HIDData.PickupTemperature, m_HIDData.HeatsinkTemperature, m_HIDData.ECap1Temperature, m_HIDData.ECap2Temperature, m_HIDData.ECapRoundTemperature,
					m_HIDData.CTransTemperature, m_HIDData.CMosFETPriTemperature, m_HIDData.CMOsFETSecTemperature,
					m_HIDData.Voltage5, m_HIDData.Current5, m_HIDData.Voltage24, m_HIDData.Current24, m_HIDData.Voltage27, m_HIDData.Current27,
					m_HIDData.Regulator_ver, m_HIDData.Fimware, m_HIDData.Soc_per, m_HIDData.Soc, m_HIDData.Soh, m_HIDData.BMS_Voltage, m_HIDData.Ref, m_HIDData.Current_Sign, m_HIDData.Current,
					m_HIDData.Max_Temp, m_HIDData.Min_Temp, m_HIDData.PCBTemperature, m_HIDData.StartDate, m_HIDData.TotalTime, m_HIDData.TotalIac, m_HIDData.TotalIout);

			HID_LOG("BMS Vcell Data = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				m_HIDData.CellVoltage[0], m_HIDData.CellVoltage[1], m_HIDData.CellVoltage[2], m_HIDData.CellVoltage[3], m_HIDData.CellVoltage[4],
				m_HIDData.CellVoltage[5], m_HIDData.CellVoltage[6], m_HIDData.CellVoltage[7], m_HIDData.CellVoltage[8], m_HIDData.CellVoltage[9],
				m_HIDData.CellVoltage[10]);
		}
		else if(m_HIDData.Regulator_ver == 85)    //regulator ver85일 때
		{
			HID_LOG("ver.85 CP Recv Data = %d,%d,%d,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				m_HIDData.Edc,m_HIDData.Ecap,m_HIDData.Eout,m_HIDData.Iout_sign,m_HIDData.Iout,m_HIDData.Iac,m_HIDData.ErrorCode,m_HIDData.PickupTemperature,
				m_HIDData.IGBTTemperature,m_HIDData.ECap1Temperature,m_HIDData.ECap2Temperature,m_HIDData.ECapRoundTemperature,m_HIDData.Regulator_ver,m_HIDData.Reserve1,
				m_HIDData.Fimware,m_HIDData.Voltage12,m_HIDData.Voltage5,m_HIDData.Voltage12_Sign,m_HIDData.Voltage12_Minus,m_HIDData.Voltage24,m_HIDData.Voltage27,
				m_HIDData.Current5,m_HIDData.Current24,m_HIDData.Current27,m_HIDData.Current12,m_HIDData.Current12_Minus,m_HIDData.SMPS_ErrorCode,
				m_HIDData.SMPS_Firmware,m_HIDData.Reserve2,m_HIDData.Reserve3);
		}
		else
		{
			HID_LOG("ver.81 CP Recv Data = %d,%d,%d,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				m_HIDData.Edc,m_HIDData.Ecap,m_HIDData.Eout,m_HIDData.Iout_sign,m_HIDData.Iout,m_HIDData.Iac,m_HIDData.ErrorCode,m_HIDData.PickupTemperature,
				m_HIDData.IGBTTemperature,m_HIDData.ECap1Temperature,m_HIDData.ECap2Temperature,m_HIDData.ECapRoundTemperature,m_HIDData.Regulator_ver,m_HIDData.Reserve1,
				m_HIDData.Fimware);
		}
	}

	//2023.01.03 sseung 버전관리 시스템 Regultor Version 저장
	if(OHTMainForm->Version_Info.Regulator_ver == "" && !(chRcvData[40]=='0' && chRcvData[41]=='0' && chRcvData[42]=='0' && chRcvData[43]=='0'))
	{
		OHTMainForm->Version_Info.Regulator_ver.sprintf("%c.%c.%c.%c", chRcvData[40], chRcvData[41], chRcvData[42], chRcvData[43]);
        OHTMainForm->Version_Info.Regulator_Date.sprintf(OHTMainForm->strVersion_Date.c_str());
		HID_LOG("RegulatorFW_ver = %s", OHTMainForm->Version_Info.Regulator_ver);
	}
#endif
	LeaveCriticalSection(&m_CS);    //크리티컬섹션 종료

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

	//2023.12.08 sseung ECO 충방전, 에러 여부 확인 값 입력
	bChargeEnable = m_pExecuteInfo->bChargeEnable;
	bError = m_pExecuteInfo->bErrorStatus;


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

	if(BMS_Ver)         //통합 모듈일 때는 급전,비급전 상태 표시를 위해 인자 추가
	{
		SendData[18] = 'R';

		if((bError == 0 && bChargeEnable == false && m_HIDData.Soc > 20) || m_HIDData.Soc > 80)    //에러 없이 방전 Node이고 배터리 잔량이 20% 초과일때 또는 배터리 잔량이 80% 이상일 때 방전명령
		{
			SendData[18] = 'S';
		}
		
		SendData[19] = 'O'; // 외부 충전 기능 추가 필요 sg.lee 25.06.26

		wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70);

		SendData[20] = wCRCData;
		SendData[21] = chETX;

		HID_LOG("HID bChargeEnable=%d, bError=%d", bChargeEnable, bError);
		if(bChargeEnable == false && (m_HIDData.Soc < 20 || bError != 0))  //배터리 20%이하 또는 에러발생시 무조건 충전 확인용
			HID_LOG("비급전 구간 충전중 Battery:%d", m_HIDData.Soc);

		HID_LOG("모니터링 데이터: %d/%d/%d/%c/%d/%d/%d/%d/%d/%d", m_HIDData.Soc,(100-m_HIDData.Soc), m_pExecuteInfo->ExecutePositionInfo.CurrNode,
				m_HIDData.Current_Sign, m_HIDData.Current, m_HIDData.Max, m_HIDData.Min, m_HIDData.Max_Temp, m_HIDData.Min_Temp, m_HIDData.Soh);
	}
	else
	{
		wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70 -1);

		SendData[18] = wCRCData;
		SendData[19] = chETX;
	}

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

	if(BMS_Ver)         //통합 모듈일 때는 급전,비급전 상태 표시를 위해 인자 추가
	{
		SendData[18] = 'R';

		wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70);

		SendData[19] = wCRCData;
		SendData[20] = chETX;
	}
	else
	{
		wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70 -1);

		SendData[18] = wCRCData;
		SendData[19] = chETX;
	}
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

	for(i = 0; i < Len; i++)    //checksumData를 1~17에서 1~18까지로 확장
	{
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
		wBuff = SendData[i] & 0xFF;
#else
		wBuff = SendData[i] & 0xFF;      //210609 연산 수정
#endif
		wCalcValue += wBuff;
	}
//	HID_LOG("wCalcValue=%06x, wCalcValue& 0x00FF=%06x, wCalcValue& 0xFF=%06x", wCalcValue, wCalcValue & 0x00FF, wCalcValue& 0xFF);
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	return wCalcValue & 0x00FF;
#else
	return wCalcValue & 0xFF;
#endif
}

/**
@brief   HID 2차측에서 받은 Data의 CheckSum이 정상인지 확인하는
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
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
		wBuff = RcvData[i] & 0x00FF;
#else
		wBuff = RcvData[i] & 0xFF;
#endif
		wCalcValue += wBuff;
	}
//	if(RcvData[Len] == (char)(wCalcValue & 0x00FF))        //if(RcvData[RCV_PK_LENGTH70-1] == (char)(wCalcValue & 0x00FF))
	HID_LOG("Len=%d, RcvData[0]=%c, RcvData[Len]=%c, wCalcValue=%04x", Len, RcvData[0], RcvData[Len], (wCalcValue & 0xFF));
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	if(RcvData[RCV_PK_LENGTH70-1] == (char)(wCalcValue & 0x00FF))
		return true;
#else
	if(RcvData[Len] == (char)(wCalcValue & 0xFF))
		return true;
#endif
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
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	{
		wBuff = RcvData[i] & 0x00FF;
		wCalcValue += wBuff;
	}
	sprintf(Tmep, "%x", (wCalcValue & 0x00F0)>>4);
	Check1 = Tmep[0];
	sprintf(Tmep, "%x", (wCalcValue & 0x000F));
	Check2 = Tmep[0];

	HID_LOG("Len=%d, RcvData[0]=%c, RcvData[Len]=%c", Len, RcvData[0], RcvData[Len]);
	if((RcvData[76] == Check1) && (RcvData[77] == Check2))
		return true;
#else
	{
		wBuff = RcvData[i] & 0xFF;
		wCalcValue += wBuff;
	}

	sprintf(Tmep, "%x", (wCalcValue & 0xF0)>>4);
	Check1 = Tmep[0];
	sprintf(Tmep, "%x", (wCalcValue & 0x0F));
	Check2 = Tmep[0];

	HID_LOG("Len=%d, RcvData[0]=%c, RcvData[Len]=%c, wCalcValue=%c", Len, RcvData[0], RcvData[Len], (char)(wCalcValue & 0xFF));  //wCalcValue=%04x
	if(RcvData[Len] == (char)(wCalcValue & 0xFF))
		return true;
#endif
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
		nErrorCode = ERR_OUTPUT_OVERCURRENT;
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
		nErrorCode = ERR_IGBT_PEAKTEMP;
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
		nErrorCode = ERR_OUTPUT_PEAKPOWER;
		break;
	case 20:
		nErrorCode = ERR_OUTPUT_OVERPOWER;
		break;
	case 22:
		nErrorCode = ERR_RES_CAP_PEAKTEMP1;
		break;
	case 23:
		nErrorCode = ERR_RES_CAP_PEAKTEMP2;
		break;
	case 24:
		nErrorCode = ERR_F_CAP_PEAKTEMP;
		break;
	case 25:
		nErrorCode = ERR_F_L_PEAKTEMP;
		break;
	case 26:
		nErrorCode = ERR_TB_PEAKTEMP;
		break;
	case 30:
		nErrorCode = ERR_ECO_OV;
		break;
	case 37:
		nErrorCode = ERR_HEATSINK_PEAKTEMP;
		break;
	case 51:
		nErrorCode = ERR_EDC_UNDERVOLTAGE;
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
	case 69:
	case 70:
	case 71:
	case 72:
	case 73:
	case 74:
		nErrorCode = ERR_ECAP_OVERTEMP;
		break;
	case 60:
		nErrorCode = ERR_PEAKCURRENT_WARN;
		break;
	case 62:
		nErrorCode = ERR_SEG_GEN_SIGNAL;
		break;
	case 63:
	case 90:
		nErrorCode = ERR_MULTI_SMPS_FAULT;
		break;
	case 64:
	case 65:
	case 75:
	case 76:
	case 77:
	case 78:
	case 79:
	case 80:
		nErrorCode = ERR_REGUL_OPEN;
		break;
	case 84:
		nErrorCode = ERR_CAP_BANK_DISCONNECT;
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
	DWORD		dwStartTime = timeGetTime();
	DWORD		dwEndTime;
	OHTMainForm->memoMain->Lines->Add("file open");
	int index = 0;

	while(1){

		if(m_HIDCommPort->ReadCommBlock((char*)&chBuff,1) == 0) {
			Sleep(10);

			// 읽어지지 않았다면 현재 시간을 먼저 보고
			dwEndTime = timeGetTime();
			//AOHC-288
			if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 3000) // 읽어지지 않은 시간이 100ms 이상이면 타임아웃   //20191104 KDS
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

//CP 통합 Regulator 통신일 때 - sseung
void HID_VII_ZR::SetRegulatorCheck(bool bUse)
{
	HID_LOG("RegulatorCheck=%d",bUse);
	RegulatorCheck = bUse;

	if(!RegulatorCheck)
		GetVer = true;     //regulator 버전 확인이 필요 없으니 true로 변환

	HID_LOG("RegulatorCheck=%d, GetVer=%d, BMS_Ver=%d",RegulatorCheck, GetVer, BMS_Ver);
}

/////////////////////////////////////////////////////////////////////////////////////
//추후에 추가적으로 필요한거 순수 가상 클래스 및 함수화를 통해 HID6.5/7.0을 따로 분류

