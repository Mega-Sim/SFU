
#pragma hdrstop

#include <vcl.h>

#include "AvsControl.h"
#include "OHTMain.h"

//#define BSH_TEST

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
	String strOHTPath = "", strTmpFileName = "";
	m_pUDPCommunication = OHTMainForm->GetUDPCom();
	data_size = NUM_OF_VIB_SAMPLE;
	strOHTPath = ExtractFileDir(Application->ExeName);
	strTmpFileName = AnsiString(strOHTPath + STR_DATAFILE_PATH + AVSDLL_FILENAME);

	hAVSDLL = LoadLibrary(strTmpFileName.c_str());
	if(hAVSDLL == NULL)
	{
	  Sleep(1000);
	  hAVSDLL = LoadLibrary(strTmpFileName.c_str());
	  if(hAVSDLL == NULL){
		OHTMainForm->memoMain->Lines->Add("AVS dll load fail..");
		return;
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
}

int AvsControl::LinkSensor(void)
{
	int nError = AVS_SUCESS;
#ifdef BSH_TEST
	//nError = openLink();
	return AVS_SUCESS;
#else
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
#endif
}
#include "mmsystem.h"
void AvsControl::StartReceiveVibData()
{
	int nError = AVS_SUCESS;
#ifdef BSH_TEST
	OHTMainForm->tmAvsControl->Interval = NUM_OF_VIB_SAMPLE;
	OHTMainForm->tmAvsControl->Enabled = true;
#else
	nError = startVibData();
	
	if(nError != AVS_SUCESS)
	{
		printError(getLastError(TRUE));
		closeAVS();
		//CLOSE_AVS_DLL
		return;
	}
	Sleep(NUM_OF_VIB_SAMPLE);
//	OHTMainForm->m_AvsControlThread->beginThread();
#endif
}

void AvsControl::StopReceiveVibData()
{
	int nError = AVS_SUCESS;
#ifdef BSH_TEST
//	OHTMainForm->tmAvsControl->Enabled = false;
#else
//	OHTMainForm->m_AvsControlThread->qiutRealThreadProc();
	nError = stopVibData();
	if(nError != AVS_SUCESS)
	{
		printError(getLastError(TRUE));
		closeAVS();
		//CLOSE_AVS_DLL
		return;
	}
#endif
}


int AvsControl::EndReceiveVibData()
{
	int nError = AVS_SUCESS;
#ifdef BSH_TEST
//	OHTMainForm->tmAvsControl->Enabled = false;
#else
//	OHTMainForm->m_AvsControlThread->qiutRealThreadProc();
	nError = closeAVS();
#endif

	return nError;
}

void AvsControl::ReceiveVibDataTimer()
{
	int nError = AVS_SUCESS;
    AnsiString strMachineID = "TP001";
	//////////////////////////////////////// 임시

	static int nDisconnectCnt = 0;
	double SpeedFrontM;
	double SpeedTrans;

	static int index = 0;
	static int index_vel = 0;
	for(int i=0;i<data_size; i++){
		mVibrationData.fX[i]=g_temp_vib_table[index].x;
		mVibrationData.fY[i]=g_temp_vib_table[index].y;
		mVibrationData.fZ[i]=g_temp_vib_table[index].z;
		index++;
		if(index >= 4096) index = 0;
	}
	SpeedFrontM = (int)g_temp_vel_table[index_vel];

	mVibrationData.CurNodeOffset = g_temp_offset_table[index_vel];
	if(g_temp_steer_left_table[index_vel] == 1) {
		mVibrationData.SteerFrontL = mVibrationData.SteerRearL = 1;  // left
	}
	else if(g_temp_steer_right_table[index_vel] == 1) {
		mVibrationData.SteerFrontR = mVibrationData.SteerRearR = 1;  // right
	}
	else{
		mVibrationData.SteerFrontR = mVibrationData.SteerRearR = 0; // move
		mVibrationData.SteerFrontL = mVibrationData.SteerRearL = 0;
	}

	mVibrationData.CurNodeID =  g_temp_tag_table[index_vel++];

	if(index_vel >= 1024) index_vel = 0;

	mVibrationData.VelFrontMotor = SpeedFrontM;

	static int cnt = 0;
	mVibrationData.VelTransMotor = cnt;

	int dwTime = timeGetTime();
	String strTest =IntToStr(cnt);
	OHTMainForm->memoMain->Lines->Add(IntToStr(cnt) + "/" + IntToStr(dwTime));
	cnt++;

	if(nError==AVS_SUCESS){
		nDisconnectCnt = 0;
		// transfer to TP
		// 진동 외 어떤 데이터를 보낼것인가??

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

	}
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

	//Offset값 조정
	mVibrationData.CurNodeOffset -= SpeedFrontM*0.2;

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

	if(nAvsResp==AVS_SUCESS){
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
		if(nDisconnectCnt > 10) // 10번 이상 data를 못 받아 올경우 끊김으로 간주
		{
			printError(getLastError(TRUE));
			closeAVS();
			CLOSE_AVS_DLL

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

#pragma package(smart_init)
