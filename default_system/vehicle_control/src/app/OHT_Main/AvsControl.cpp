
#pragma hdrstop

#include <vcl.h>

#include "AvsControl.h"
#include "OHTMain.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef int (__cdecl* _getLastError)(BOOL bClearError);
typedef int (__cdecl* _openLink)(void);
typedef int (__cdecl* _initAVS)(void);
typedef int (__cdecl* _ping)(void);
typedef int (__cdecl* _getID)(char* id);
typedef int (__cdecl* _getSerialNumber)(char* sn);
typedef int (__cdecl* _getCalDate)(int* month, int* day, int* year);
typedef int (__cdecl* _startVibData)(void);
typedef int (__cdecl* _stopVibData)(void);
typedef int (__cdecl* _getSample)(int, float*, float*, float*, int*, int*);
typedef int (__cdecl* _closeAVS)(void);

_getLastError		getLastError;
_openLink			openLink;
_initAVS			initAVS;
_ping				ping;
_getID				getID;
_getSerialNumber	getSerialNumber;
_getCalDate			getCalDate;
_startVibData		startVibData;
_stopVibData		stopVibData;
_getSample			getSample;
_closeAVS			closeAVS;

#ifdef  __cplusplus
}
#endif

// String strTmpFileName = L"AVS.dll";
AvsControl::AvsControl()
{
	m_defualtparam = OHTMainForm->GetDefaultParam();

	memset(fX,0.0,sizeof(fX));
	memset(fY,0.0,sizeof(fY));
	memset(fZ,0.0,sizeof(fZ));

	bDllLoadFail = false;
	String strOHTPath = "", strTmpFileName = "";
	m_pUDPCommunication = OHTMainForm->GetUDPCom();
	data_size = NUM_OF_VIB_SAMPLE;
	strOHTPath = ExtractFileDir(Application->ExeName);
	strTmpFileName = AnsiString(strOHTPath + STR_DATAFILE_PATH + AVSDLL_FILENAME);
	m_nAliveCounter = 0;
	m_nPreAliveCounter = 0;
	m_dwCheckCounter = 0;
	m_bIsStarted = false;
	bUseAVS =  m_defualtparam->bUseAVS;

	if(bUseAVS)
	{
    	hAVSDLL = LoadLibrary(strTmpFileName.c_str());
		if(hAVSDLL == NULL)
		{
		  Sleep(1000);
		  hAVSDLL = LoadLibrary(strTmpFileName.c_str());
		  if(hAVSDLL == NULL)
		  {
			m_defualtparam->bUseAVS = false;
			bDllLoadFail = true;
			OHTMainForm->memoMain->Lines->Add("AVS dll load fail..");
			return;
		  }
		}
    }


	batt = 0;
	temp = 0;

	memset(&mVibrationData,0x00, sizeof(AVS_VIBRATION_DATA_INFO));


	getLastError	= (_getLastError)GetProcAddress(hAVSDLL,"getLastError");
	openLink		= (_openLink)GetProcAddress(hAVSDLL,"openLink");
	initAVS			= (_initAVS)GetProcAddress(hAVSDLL,"initAVS");
	ping			= (_ping)GetProcAddress(hAVSDLL,"ping");
	getID			= (_getID)GetProcAddress(hAVSDLL,"getID");
	getSerialNumber	= (_getSerialNumber)GetProcAddress(hAVSDLL,"getSerialNumber");
	getCalDate		= (_getCalDate)GetProcAddress(hAVSDLL,"getCalDate");
	startVibData	= (_startVibData)GetProcAddress(hAVSDLL,"startVibData");
	stopVibData		= (_stopVibData)GetProcAddress(hAVSDLL,"stopVibData");
	getSample		= (_getSample)GetProcAddress(hAVSDLL,"getSample");
	closeAVS		= (_closeAVS)GetProcAddress(hAVSDLL,"closeAVS");
	//OPEN_AVS_DLL//(strTmpFileName.c_str())


}

AvsControl::~AvsControl()
{
	EndReceiveVibData();
	CLOSE_AVS_DLL
}

int AvsControl::LinkSensor(void)
{
	int nError = AVS_SUCESS;
	nError = openLink();
	if(nError != AVS_SUCESS)
	{
		printError(getLastError(TRUE));
	}

	nError = initAVS();
	if(nError != AVS_SUCESS)
	{
		printError(getLastError(TRUE));
		return 0;
	}

	return AVS_SUCESS;
}


#include "mmsystem.h"
void AvsControl::StartReceiveVibData()
{
	int nError = AVS_SUCESS;

	nError = startVibData();
	
	if(nError != AVS_SUCESS)
	{
		printError(getLastError(TRUE));
		closeAVS();
		//CLOSE_AVS_DLL
		return;
	}

	Sleep(NUM_OF_VIB_SAMPLE);

	m_bIsStarted = true;
	m_nAliveCounter = 0;
	m_nPreAliveCounter = 0;
	m_dwCheckCounter = timeGetTime();
}

void AvsControl::StopReceiveVibData()
{
	int nError = AVS_SUCESS;

	m_bIsStarted = false;

	nError = stopVibData();

	Sleep(500);
	if(nError != AVS_SUCESS)
	{
		printError(getLastError(TRUE));
		closeAVS();
		//CLOSE_AVS_DLL
		return;
	}
}


int AvsControl::EndReceiveVibData()
{

	int nError = AVS_SUCESS;

	if(	m_bIsStarted == true)
		StopReceiveVibData();

	nError = closeAVS();

	return nError;
}



int AvsControl::ReceiveVibData()
{
	int nError = NO_ERR;
	int nAvsResp = AVS_SUCESS;
	static int nDisconnectCnt = 0;
	double SpeedFrontM;
	double SpeedTrans;
	static int nPreNodeId = 0;
	static int nPreNodeOffset = 0;
	static bool bMinusOffsetFlag = false;
	int nCurNodeId = OHTMainForm->GetExecuteInfo()->ExecutePositionInfo.CurrNode;
	NODE_CHECK_DATA TmpNodeCheckData;

	OHTMainForm->GetHWCommon()->GetNodeCountInfo(TmpNodeCheckData);

	nAvsResp = getSample(data_size,mVibrationData.fX,mVibrationData.fY,mVibrationData.fZ,&temp, &batt);

	SpeedFrontM = OHTMainForm->GetHWSet()->m_pDrivingAxis->GetCurrentVelocity() * 1000.0;
	SpeedTrans = OHTMainForm->GetHWSet()->m_pHoistAxis->GetCurrentVelocity() * 1000.0;
	mVibrationData.CurNodeID =	nCurNodeId;
	mVibrationData.CurNodeOffset = OHTMainForm->GetExecuteInfo()->ExecutePositionInfo.CurrNodeOffset;
	mVibrationData.SteerFrontL = OHTMainForm->GetHWSet()->m_pSteer->IsLeftPos(FRONT);
	mVibrationData.SteerFrontR = OHTMainForm->GetHWSet()->m_pSteer->IsRightPos(FRONT);
	mVibrationData.SteerRearL = OHTMainForm->GetHWSet()->m_pSteer->IsLeftPos(REAR);
	mVibrationData.SteerRearR = OHTMainForm->GetHWSet()->m_pSteer->IsRightPos(REAR);

	mVibrationData.VelFrontMotor = SpeedFrontM;
	mVibrationData.VelTransMotor = SpeedTrans;

	mVibrationData.temperature = temp;
	mVibrationData.battery = batt;


	//UBG Input, Output,  UBG 센서는 B접이지만 -> Input의 경우 Masked Output만 반전처리
	mVibrationData.UBGInputLong	  = OHTMainForm->GetHWSet()->m_pOHTDetect->IsDetect(0);
	mVibrationData.UBGInputMiddle = OHTMainForm->GetHWSet()->m_pOHTDetect->IsDetect(1);
	mVibrationData.UBGInputShort  = OHTMainForm->GetHWSet()->m_pOHTDetect->IsDetect(2);
	mVibrationData.UBGInputFail   = OHTMainForm->GetHWSet()->m_pOHTDetect->IsDetect(3);

	mVibrationData.UBGOutputSel1   = !OHTMainForm->GetHWSet()->m_pOHTDetect->IsRegion(0);
	mVibrationData.UBGOutputSel2   = !OHTMainForm->GetHWSet()->m_pOHTDetect->IsRegion(1);
	mVibrationData.UBGOutputSel3   = !OHTMainForm->GetHWSet()->m_pOHTDetect->IsRegion(2);
	mVibrationData.UBGOutputSel4   = !OHTMainForm->GetHWSet()->m_pOHTDetect->IsRegion(3);

	//OBS Input, Output   OBS 센서는 B접이지만 -> Input의 경우 Masked Output만 반전처리
	mVibrationData.OBSInputLong   = OHTMainForm->GetHWSet()->m_pObstacle->IsDetect(0);
	mVibrationData.OBSInputMiddle = OHTMainForm->GetHWSet()->m_pObstacle->IsDetect(1);
	mVibrationData.OBSInputShort  = OHTMainForm->GetHWSet()->m_pObstacle->IsDetect(2);
	mVibrationData.OBSInputFail   = OHTMainForm->GetHWSet()->m_pObstacle->IsDetect(3);

	mVibrationData.OBSOutputSel1   = !OHTMainForm->GetHWSet()->m_pObstacle->IsRegion(0);
	mVibrationData.OBSOutputSel2   = !OHTMainForm->GetHWSet()->m_pObstacle->IsRegion(1);
	mVibrationData.OBSOutputSel3   = !OHTMainForm->GetHWSet()->m_pObstacle->IsRegion(2);
	mVibrationData.OBSOutputSel4   = !OHTMainForm->GetHWSet()->m_pObstacle->IsRegion(3);

	//SpeedMode 갱신
	mVibrationData.SpeedMode = OHTMainForm->GetHWSet()->m_pDrivingAxis->GetSpeedMode();


	//Offset값 조정
	mVibrationData.CurNodeOffset -= SpeedFrontM*0.2;

//	//Trans 정보
//	mVibrationData.HandPushSen =  OHTMainForm->GetHWCommon()->m_Input.HAND_Push;
//	mVibrationData.InputHandOpen = OHTMainForm->GetHWSet()->m_pHand->IsOpen();
//	mVibrationData.InputHandClose = OHTMainForm->GetHWSet()->m_pHand->IsClose();
//	mVibrationData.InputHandGripLeft = OHTMainForm->GetHWSet()->m_pHand->IsLeftGripOn();
//	mVibrationData.InputHandGripRight = OHTMainForm->GetHWSet()->m_pHand->IsRightGripOn();
//	mVibrationData.InputHandInposition =  OHTMainForm->GetHWSet()->m_pHand->IsInposition();
//	mVibrationData.HoistHomeSen = OHTMainForm->GetHWSet()->m_pHoistAxis->IsHoistHome();
//
//	//Trans Step
//	mVibrationData.TranStepDetail = OHTMainForm->GetTransControl()->GetTransStep();
//	mVibrationData.TransHoistPos =  OHTMainForm->GetHWSet()->m_pHoistAxis->GetCurrentPosition();
//	mVibrationData.TransTypeDetail = OHTMainForm->GetTransControl()->GetTransType();

	if((mVibrationData.CurNodeOffset < 0)
	&&(nPreNodeId != nCurNodeId)
	&&(nPreNodeId != 0)
	&&(TmpNodeCheckData.NodeCount != 0)
	&&(bMinusOffsetFlag == false))
	{
		bMinusOffsetFlag = true;

		int nCurNodeId = OHTMainForm->GetExecuteInfo()->ExecutePositionInfo.CurrNode;
		int nPathIndex = OHTMainForm->GetMap()->GetPathIndex(nPreNodeId, nCurNodeId);

		nPreNodeOffset = OHTMainForm->GetMap()->m_PathArray[nPathIndex].Distance;
	}
	else if(mVibrationData.CurNodeOffset >= 0)
	{
		bMinusOffsetFlag = false;

		nPreNodeId = OHTMainForm->GetExecuteInfo()->ExecutePositionInfo.CurrNode;
	}

	if(bMinusOffsetFlag == true)
	{
    	mVibrationData.CurNodeID = nPreNodeId;
		mVibrationData.CurNodeOffset += nPreNodeOffset;
	}

	if(nAvsResp == AVS_SUCESS){
		nDisconnectCnt = 0;
		// transfer to TP
		// 진동 외 어떤 데이터를 보낼것인가??
		AnsiString strMachineID = "TP001";
		int iPacketSize = sizeof(PKT_OHT2TP_VIBRATION_INFO);
		int iReturn;

		PKT_OHT2TP_VIBRATION_INFO	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);
		//memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		memcpy(&SEND_PACKET.BODY,&mVibrationData, sizeof(AVS_VIBRATION_DATA_INFO));

		//SEND_PACKET.BODY.Reserved2 = Load_AutoTeaching_DrvingData(SEND_PACKET.BODY.Reserved1,SEND_PACKET.BODY.PortType);
		SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
		memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());
		SEND_PACKET.TAG.Value = TagID::CMD_VIBDATA_REQ_TP_RESPONSE;

		iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
		return nError;
	}
	else if(nAvsResp==AVS_WAIT_DATA)
	{
		// sample 개수 만큼 받을 준비가 안된 상태
		return nError;
	}
	else
	{
		nDisconnectCnt++;
		if(nDisconnectCnt > 20) // 20번 이상 data를 못 받아 올경우 끊김으로 간주
		{
			printError(getLastError(TRUE));
			closeAVS();

			int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
			PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);

			SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
			SEND_PACKET.TAG.Value = TagID::CMD_TP_VIB_DATA_CLOSE;
			m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);

			// TP로 끊김 메세지 전송
			return 0;
		}
		return nError;
	}
}

void AvsControl::printError(int err)
{
	switch(err){
		case ERROR_BAD_ID_STRING:
			OHTMainForm->memoMain->Lines->Add("ERROR: bad id string");
			break;
		case ERROR_DATA_NOT_READY:
			OHTMainForm->memoMain->Lines->Add("ERROR: new vibration data not ready");
			break;
		case ERROR_DATA_OVERFLOW:
			OHTMainForm->memoMain->Lines->Add("ERROR: vibration data overflow");
			break;
		case ERROR_DATA_DROP:
			OHTMainForm->memoMain->Lines->Add("ERROR: detected vibration data drop");
			break;
		case ERROR_BT_LINK_MODULE_FAILED:
			OHTMainForm->memoMain->Lines->Add("ERROR: link module failed to open");
			break;
		case ERROR_INCOMPATIBLE_DEVICE:
			OHTMainForm->memoMain->Lines->Add("ERROR: incompatible AVS device");
			break;
		case ERROR_COMMUNICATION_TIMEOUT:
			OHTMainForm->memoMain->Lines->Add("ERROR: communication timeout");
			break;
		case ERROR_AVS_NOT_READY:
			OHTMainForm->memoMain->Lines->Add("ERROR: AVS not ready or initialized");
			break;

	}
}

//TP로부터 연결상태 확인을 위한 패킷 수신
void AvsControl::ReceiveAlivePacket()
{
	m_nAliveCounter++;

	//Overflow 방지
	if(m_nAliveCounter >= 1000)
		m_nAliveCounter = 0;
}

//TP와 연결되었는지 확인하는 함수
void AvsControl::CheckTPConnected()
{
	if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwCheckCounter) > 5000)
	{
		if(m_nAliveCounter == m_nPreAliveCounter)
		{
			if(EndReceiveVibData())
			{
				int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
				PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
				memset(&SEND_PACKET, 0, iPacketSize);
				SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
				SEND_PACKET.TAG.Value = TagID::CMD_TP_VIB_DATA_CLOSE;
				m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
			}
		}
		else
		{
			m_nPreAliveCounter = m_nAliveCounter;
		}

		m_dwCheckCounter = timeGetTime();
	}
}

#pragma package(smart_init)
