// ---------------------------------------------------------------------------
/**
 @file    MainControlTask.cpp
 @version OHT 7.0
 @brief   OHT에서 MainControl 역할을 담당하는 CPP File
 */

#pragma hdrstop

#include "Def_OHT.h"
#include "TaskControl.h"
#include "Pingunit.h"

#include "OHTMain.h" //test
#include "STD_Station.h"
#include "ProcessTerminator.h"

#include "STD_MapMakerFile.h"
// ---------------------------------------------------------------------------
// by zzang9un 2014.01.14 : Map Maker 최대, 최소 속도
#define MAX_SPEED_MAPMAKE           200
#define MIN_SPEED_MAPMAKE           100

#define OHT_EXE_FOLDER 0
#define OHT_LOG_FOLDER 1

#define CHECK_DISTANCE 2000
#define LIMIT_DISTANCE 1000
#define CHECK_STATUS_POS 9
#define MAX_SPEED 5000 //p-pjt 최대속도  AOHC-107

#define MAX_PASSREQ 10

#define TESTCODE  0

#define FILE_TMP_SIZE 50

// ---------------------------------------------------------------------------
// 로그 작성 함수
#define ADD_LOG_TASKCONTROL(...)    WRITE_LOG_UNIT(pLogTaskControl, __VA_ARGS__)
#define ADD_LOG_MAPMAKE(...)        WRITE_LOG_UNIT(pLogMapMake, __VA_ARGS__)

//#define ADD_ERROR_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\Error.log", __VA_ARGS__)
#define ADD_ERROR_LOG(...)       WRITE_LOG_UNIT(pLogError, __VA_ARGS__)
//#define Ping_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\Ping.log", __VA_ARGS__)
#define Ping_LOG(...)       WRITE_LOG_UNIT(pLogPing, __VA_ARGS__)
#define ADD_IOTCOMM_LOG(...)         WRITE_LOG_UNIT(pLogIotComm, __VA_ARGS__)
#define ADD_MD_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\MonitoringDetail.log", __VA_ARGS__) //AOHC-253 Monitoring 항목 추가 2
//#define ADD_HANDTP_LOG(...)         WRITE_LOG_ON_THIS_FILE("D:\\log\\HandTP.log", __VA_ARGS__)
#define ADD_HANDTP_LOG(...)       WRITE_LOG_UNIT(pLogHandTP, __VA_ARGS__)
#define ADD_MCC_LOG(...)      WRITE_LOG_UNIT(pLogMCC, __VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)
#define SEND_WARN(...)				writeFDCUnit_WARN(pFDCUnit, __VA_ARGS__)
#define Change_FDC_Auto_Mode()			changeFDCAutoMode(pFDCUnit)
#define Change_FDC_Manual_Mode(...)			changeFDCManualMode(pFDCUnit, __VA_ARGS__)
// ---------------------------------------------------------------------------

unsigned char g_CMDType;

#define MAX(a,b)    ((a>b)?a:b)
#define MIN(a,b)    ((a<b)?a:b)

CRITICAL_SECTION g_csIOT; //AOHC-203 IoT정보 데이터 취득 및 활용
/**
 @class   TaskControl 생성자
 @date    2013.05.24
 @author  zzang9un
 @brief   TaskControl Class
 */
TaskControl::TaskControl() {
	// by zzang9un 2013.07.23 : 로그 생성
	pLogTaskControl = getLogUnit("D:\\log\\TaskControl.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogMapMake = getLogUnit("D:\\log\\MapMake.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_pLogOCSComm = getLogUnit("D:\\log\\OCSComm.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_pLogTPComm = getLogUnit("D:\\log\\TPComm.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogIotComm = getLogUnit("D:\\log\\IOTComm.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogError = getLogUnit("D:\\log\\Error.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogPing = getLogUnit("D:\\log\\Ping.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogHandTP = getLogUnit("D:\\log\\HandTP.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogMCC = getLogUnit("D:\\log\\MCC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	m_pUDPCommunication = OHTMainForm->GetUDPCom();
	m_pUDPDiag =   OHTMainForm->GetUDPDiag();
	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
	m_pPassPermit = OHTMainForm->GetPassPermit();
	m_pCmdManager = OHTMainForm->GetCmdManager();
	m_pPathSearch = OHTMainForm->GetPathSearch();

	m_defualtparam = OHTMainForm->GetDefaultParam();
    m_bMultiTPConnected = false;


	// 2013.07.09 doori.shin 구조체 받음
	m_pMapMake_Status = OHTMainForm->GetMapMakeStatus();
	m_pMapMake_Process = OHTMainForm->GetMapMakeProcess();

	m_DiagManager = DiagManager::getInstance();

	m_pDrivingControl = OHTMainForm->GetDrivingControl(); 
	m_pTransControl   = OHTMainForm->GetTransControl();

	m_pHWCommon = OHTMainForm->GetHWCommon();
	m_pHWSet = OHTMainForm->GetHWSet();
	
	OHTError_Current = NO_ERR;

	ErrorCode_Main = NO_ERR;

	m_bflagARPClear = false;  //AOHC-234 C3850 Switch Fail-Over 시 통신 끊김 개선

	m_bLoadedMap = false;
	m_bLoadedStationData = false;
	m_bLoadedTeachingData = false;

	m_bLoadedCID_STD = false;
	m_bLoadedCollision_STD = false;
	m_bLoadedFlowControl_STD = false;
	m_bLoadedMap_STD = false;
	m_bLoadedStation_STD = false;
	m_bLoadedTeaching_STD = false;
	m_bLoadedPathData_STD = false;

	m_pRemocon = NULL;
	bOpenRemocon = false;

	memset(&m_FIRMWARE_UPDATE_INFO, 0x00, sizeof(FIRMWARE_INFO));
	memset(m_nInputData, 0x00, sizeof(m_nInputData));
	memset(&m_StatusOHT2TP, 0x00, sizeof(MSG_STATUS_OHT2TP));
	memset(&m_PreStatusOHT2OCS, 0x00, sizeof(PROTOCOL_STATUS_OHT));

	m_IOMapIni = NULL;
	m_dwRotateJogMoveStartTime = timeGetTime();
	dwTurnOffJobOrderTime = timeGetTime();
	m_nPortType_HandTPLongrun = 0;
	FirmwareUpdateFlag = false;
	m_bAutoChaning = 0;

	m_pPassPermit->SetPassPermitTime(timeGetTime());

	TaskState_Cur = MAIN_TASK_INITSOCKET;
	TaskState_Pre = MAIN_TASK_INITSOCKET;


	TaskControlfalg = true;
	nCheckErrorOHTType = NO_ERR;

	m_NetWorkDll = new NetworkSinalLib();

	// by zzang9un 2013.07.22 : 소켓 에러로 인해 추가함

	// 2013.08.16 doori.shin IOMap 경로
	strIOMapPath = String(ExtractFileDir(Application->ExeName) +
		STR_DATAFILE_PATH + STR_FILENAME_IOMAP);

	// 2013.08.16 doori.shin Initialize IO Map Member Variable
	m_IOMapField = new IO_MAP_FIELD;
	memset(m_IOMapField, 0, sizeof(IO_MAP_FIELD));

	// 2014.07.08 doori.shin TP Type
	m_nTPType = CONNECTED_TP_NONE;

	gm_Ping = new TOHT_PING;

	// 2016.12.09 shkim. 펌웨어 업데이트 진행시 응답을 어느 쪽으로 보낼지 설정
	m_iFWU_Direction = FWU_TO_OCS;

    // 2016.12.15 sh17.jo Commfail Check 변수 추가
    m_bCheckRcvData = false;
	m_bSetCheckCommfail = false;
    memset(m_nComfailCount, 0, sizeof(m_nComfailCount));
	m_nComOKCount = 0;
	m_nCountCheckPathSearch =0;

    m_nLongrunTypeHandyTP = HANDTP_LONGRUN_NONE;
	m_bRotateJogCmdExist = false;

	m_FileMapMakeflag = false;
	m_FileTeachingflag = false;

	// 2017-01-14, Labeler, Auto Change Check 용 Flag 변수 초기화
	bAutoChange = false;
	
	bMainRestart = false;	// Firmware Update Main Restart
	bMainCopy = false;

	memset(&m_pCurrCIDUSE, 0x00, sizeof(FLAG_CID_NOT_USE));
	memset(&m_pListCIDUSE, 0x00, sizeof(FLAG_CID_NOT_USE));

	m_bDataTransper_InProgress = false;

	m_bCheckAutoToManual = false;

	memset(&m_OCSStaionInfo, 0x00, sizeof(NODE_INFO_STD_STATION));

//	m_dwJobChangeCheckTime = (DWORD)timeGetTime();
//	m_bJobChangeCheckFlag = false;

	m_pServoControl = m_pHWSet->m_pServoMonitoring;

   if(m_defualtparam->IsIOTHub ==true)
   {
   		// IoT Hub Manger
		m_IotManager = new IotManager;

		m_IOTHubServerSock = new ServerSocket();
		m_IOTHubSocket = NULL;
		m_ConnectedIOTHub = false;
		ServerSocketOpen();
        bSlope = true;
   }
   m_bCheckPassOpenListCount = false;

   m_nOHTDetect_Inpunt_Prev = 0;
   m_StepAutoChangeReset.bAllDone = true;
	//AOHC-203 IoT정보 데이터 취득 및 활용
   InitializeCriticalSection(&g_csIOT);
   memset(&m_IoTData, 0, sizeof(m_IoTData));
   memset(&IotHeader, 0, sizeof(IotHeader));
   DefSignalCount = NULL;
   m_dwUBGTrigerTime = 0;
   m_LogTime = 0;
   m_dMinAccel = 0.0;  //AOHC-107 OHT->OCS Nack 개선 마진 거리 최적화
   m_mccparam.bHandSequenceFlag = false;
   m_mccparam.bTransMessageFlag = false;
   m_mccparam.nEqType = 0;
   m_mccparam.dwHandOpenOutOnTime = 0;
   m_mccparam.dwHandOpenOutOffTime = 0;
   m_mccparam.dwHandCloseOutOnTime = 0;
   m_mccparam.dwHandCloseOutOffTime = 0;
   m_mccparam.dwHandInpoInOnTime = 0;
   m_mccparam.dwHandInpoInOffTime = 0;
   m_mccparam.dwHandOpenInOnTime = 0;
   m_mccparam.dwHandOpenInOffTime = 0;
   m_mccparam.dwHandCloseInOnTime = 0;
   m_mccparam.dwHandCloseInOffTime = 0;
   m_mccparam.dwHandLeftGripInOnTime = 0;
   m_mccparam.dwHandLeftGripInOffTime = 0;
   m_mccparam.dwHandRightGripInOnTime = 0;
   m_mccparam.dwHandRightGripInOffTime = 0;
   m_mccparam.dwHandPushInOnTime = 0;
   m_mccparam.dwHandPushInOffTime = 0;
   m_mccparam.nHandPushCount = 0;

   m_mccparam.nJobO_NAK_Count = 0;
   m_mccparam.nJobC_NAK_Count = 0;
   m_mccparam.nJobD_NAK_Count = 0;
   m_mccparam.nOCS_Comm = 0;

   m_mccparam.bpreHandCloseOut = m_pHWSet->m_pHand->IsCloseOutputOn();
   m_mccparam.bpreHandOpenOut = m_pHWSet->m_pHand->IsOpenOutputOn();
   m_mccparam.bpreHandCloseIn = m_pHWSet->m_pHand->IsClose();
   m_mccparam.bpreHandOpenIn	= m_pHWSet->m_pHand->IsOpen();
   m_mccparam.bpreHandInposition = m_pHWSet->m_pHand->IsInposition();
   m_mccparam.bpreHandLeftGrip = m_pHWSet->m_pHand->IsLeftGripOn();
   m_mccparam.bpreHandRightGrip = m_pHWSet->m_pHand->IsRightGripOn();
   m_mccparam.bpreHandPush = m_pHWSet->m_pHand->IsPushOn();
   m_mccparam.bTransLoaded = false;

   m_MoniUBGPat=0;
   m_bUBGReadCheck = false;

	bCheck  = false;
	nCheck = 0;
	dwUBGCheckTime = 0;
	bprebumpper = false;
	nUBGSensorDetecting = 0;
	bUBGOneHourMonitoring =false;
	nCycleCnt= 0;

	bIncreased_UD =false;
	bIncreased_UF =false;
	bIncreased_UC =false;
}
// ---------------------------------------------------------------------------
/**
 @brief   에러 발생시 바이러스 체크
 @author  ks4423
 @date    2014.04.22
 */
void TaskControl::CheckVirus() {
	// Virus 체크
	int nChkV = OHTUTILITY::Check_VirusDirectory();
	AnsiString strOHTPath = ExtractFileDir(Application->ExeName);

	if ((nChkV >> OHT_EXE_FOLDER) & 0x1) {
		ADD_LOG_TASKCONTROL("-****---> [WARNNING !!!] Virus Detect :%s%s",
			strOHTPath.c_str(), STR_VIRUS_PATH);
	}

	if ((nChkV >> OHT_LOG_FOLDER) & 0x1) {
		ADD_LOG_TASKCONTROL("-****---> [WARNNING !!!] Virus Detect :%s%s",
			STR_LOG_PATH, STR_VIRUS_PATH);
	}
}


/**
 @brief   MainControlTask 메인 함수. 주기적으로 실행됨
 @author  puting
 @date    2015.11.17
 */
void TaskControl::ExcuteStandardTaskUpdate()
{
	char szLogMsg[1024];
    ZeroMemory(szLogMsg, sizeof(szLogMsg));
	
	static bool bcheckvirusError = true;
	static bool bErrorCIDOccupFlag = false;
	String strLog = NULL; // 로그를 저장하기 위한 String 변수
	int Error = NO_ERR; // 내부 에러 체크를 위한 변수
	int iRet = NO_ERR; // 내부 에러 체크를 위한 변수
	AnsiString TemStr;
	AnsiString strFullPath, strTmp;
	FILE *fi;
	char cTmp[FILE_TMP_SIZE];
	// by zzang9un 2013.08.19 : Station에 Mark한 경우는 Offset을 Data에서 가져와서 Auto로 전환함
	STATION_DATA StationData; // Station Data를 가져올 구조체 포인터
	memset(&StationData, 0x00, sizeof(StationData));

	PATH_STEERING_INFO pTempSteering =  PATH_STEERING_NONE;

	static DWORD dCheckAutoToManualTime = timeGetTime();
	static DWORD dCheckAutoToErrorTime = timeGetTime();
	unsigned char SteerFront;

	static bool bcheckFN009 = false;

	SYSTEMTIME CurTime;
	GetLocalTime(&CurTime);
	if (TaskControlfalg == false)
		return;


	// UBG INI 대차패턴 값 읽지 못했으면 실행시점부터 알리기 위해서 무조건 에러를 띄움
	// SHCHO
	//if(Error == NO_ERR)
	//{
	//	if(OHTMainForm->m_bUBGINILoaded == false)
	//		Error = ERR_UBG_NO_DATA;
	//}



	switch (TaskState_Cur) {
		// -------------------------------------------
		// 1 Step : MAIN_TASK_INITSOCKET
	case MAIN_TASK_INITSOCKET:
		// by zzang9un 2012.12.03 : 기본 Class 생성은 이미 했으므로 나머지 초기화 기능 수행

		// Step 1. 통신 Socket 초기화
		// InitSocket();
		ChangeCmdIDAndCycle(0,0,0,true);
		InitClearCycle();
		InitExcuteOverRun();
		ChangeMainTaskState(MAIN_TASK_DATALOAD);

		break;

		// -------------------------------------------
		// 1 Step : MAIN_TASK_DATALOAD
	case MAIN_TASK_DATALOAD:
		// ?? 기존 Map data만 로딩 실패시 처리 나머지 파일들은 파일이 없어도 pass
		// ?? MAIN_TASK_DATALOAD error 발생시 처리 부분 검토 필요.
		// Step 1. STD Map set Load
		m_pOHTMainStatus->DataUseFlag.fMapsetLoad = true;
		m_pOHTMainStatus->DataUseFlag.fUBGLoad = 0xff;
		iRet = LoadCIDData_STD();
		if (iRet) 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadCIDData_STD File Load Fail...");
			OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
			m_pOHTMainStatus->DataUseFlag.fMapsetLoad = false;
		}
		else 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadCIDData_STD File Load OK...");
		}

		iRet = LoadCollisionData_STD();
		if (iRet) 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadCollisionData_STD File Load Fail...");
			OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
			m_pOHTMainStatus->DataUseFlag.fMapsetLoad = false;
		}
		else 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadCollisionData_STD File Load OK...");
		}

		iRet = LoadFlowControlData_STD();
		if (iRet) 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadFlowControlData_STD File Load Fail...");
			OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
			m_pOHTMainStatus->DataUseFlag.fMapsetLoad = false;
		}
		else 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadFlowControlData_STD File Load OK...");
		}

		iRet = LoadMapData_STD();
		if (iRet) 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadMapData_STD File Load Fail...");
			OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
			m_pOHTMainStatus->DataUseFlag.fMapsetLoad = false;
		}
		else 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadMapData_STD File Load OK...");
		}

		iRet = LoadStationData_STD();
		if (iRet) 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadStationData_STD File Load Fail...");
			OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
			m_pOHTMainStatus->DataUseFlag.fMapsetLoad = false;
		}
		else 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadStationData_STD File Load OK...");
		}

		iRet = LoadTeachingData_STD();
		if (iRet) 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadTeachingData_STD File Load Fail...");
			OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
			m_pOHTMainStatus->DataUseFlag.fMapsetLoad = false;
		}
		else 
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadTeachingData_STD File Load OK...");
		}

		iRet = LoadPathData_STD();
		if (iRet)
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadPathData_STD File Load Fail...");
			OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
			m_pOHTMainStatus->DataUseFlag.fMapsetLoad = false;
		}
		else
		{
			ADD_LOG_TASKCONTROL("[INIT] LoadPathData_STD File Load OK...");
		}

		// Step 2. mapping PathMap, NodeMap, TeachingMap, StationMap from STD Map Set
		// 7.0버전의  LoadMapData, LoadTeachingData, LoadStationData에서 사용하는 자료구조 생성
		iRet = MakeMapData();
		if (iRet)
		{
			ADD_LOG_TASKCONTROL("[INIT] make PathMap, NodeMap Fail...");
			OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
			m_pOHTMainStatus->DataUseFlag.fMapsetLoad = false;
		}
		else
		{
			ADD_LOG_TASKCONTROL("[INIT] make PathMapd, NodeMap OK...");

			iRet = MakeStationData();
			if (iRet)
			{
				ADD_LOG_TASKCONTROL("[INIT] make Station Fail...");
				OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
				m_pOHTMainStatus->DataUseFlag.fMapsetLoad = false;
			}
			else
			{
				ADD_LOG_TASKCONTROL("[INIT] make Station OK...");
			}

			iRet = MakeTeachingData();
			if (iRet)
			{
				ADD_LOG_TASKCONTROL("[INIT] make Teaching Fail...");
				OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
				m_pOHTMainStatus->DataUseFlag.fMapsetLoad = false;
			}
			else
			{
				ADD_LOG_TASKCONTROL("[INIT] make Teaching OK...");
			}
		}



		ADD_LOG_TASKCONTROL("[INIT] Load Mapset result(1:pass) : %d",m_pOHTMainStatus->DataUseFlag.fMapsetLoad);

		// Step 3. 기존 Map, Station, Teaching 생성 후 Free STD Map data(CID , Flowcontrol 는 Free 안시킴)
		// STD m_NodeArray 삭제(CID, FlowControl는 삭제 안함)
		// 일단 전체 삭제 안하는 걸로….
		// 주행학습, 티칭등으로 VHL에서 수정되어지는 값들 확인 필요


		// Step 4. Load IOMap Field Ini file
		// Check file exists
		if (!FileExists(strIOMapPath)) {
			// There is no file
			ADD_LOG_TASKCONTROL("[INIT] IOMap.ini File doesn't exists...");
		}
		else {
			// Load IO Map Field
#if(OHT_NAME == OHT_NAME_STD_V80)
			iRet = LoadIOMapField(L"FOUP", L"Ver 8.0");
#elif(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
			iRet = LoadIOMapField("RETICLE", "Ver 8.0");
#elif(OHT_NAME == OHT_NAME_STD_V81S)
			iRet = LoadIOMapField("FOUP", "Ver 8.1S");
#elif(OHT_NAME == OHT_NAME_STD_V81)
			iRet = LoadIOMapField("FOUP", "Ver 8.1");
#elif(OHT_NAME == OHT_NAME_STD_V85)
            iRet = LoadIOMapField("EUV", "Ver 8.5");
#else
			iRet = LoadIOMapField("FOUP", "Ver 7.0");
#endif
			// If Success to Load IO Map ini file, Timer is terminated.
			// else if Error is detected, timer will excute until load file successfully

			if (iRet) {
				ADD_LOG_TASKCONTROL("[INIT] Success to Load IOMap.ini file...");
				m_IOMapField->fIsLoaded = true;
			}
			else
				ADD_LOG_TASKCONTROL("[INIT] Fail to Load IOMap.ini file");


		}



		m_strMonitor = "";

		// GetAsVer
		strFullPath = AS_VER_FILE_PATH;

		if(FileExists(strFullPath.c_str()) == true) 	// 파일이 존재하면
		{
			if( (fi = fopen( strFullPath.c_str(), "r" )) == NULL )
			{
				strTmp = "As=Unknown ";
			}
			else
			{
				memset(&cTmp, '\0', FILE_TMP_SIZE);
				fscanf_s(fi,"%s", &cTmp, 40);
				strTmp.sprintf("As=%s ", &cTmp);
				fclose(fi);
			}
		}
		else
		{	// File Not Found
				strTmp = "As=Unknown ";
		}
		m_strMonitor = m_strMonitor + strTmp;

		// GetFmVer
		strFullPath = FM_VER_FILE_PATH;

		if(FileExists(strFullPath.c_str()) == true) 	// 파일이 존재하면
		{
			if( (fi = fopen( strFullPath.c_str(), "r" )) == NULL )
			{
				strTmp = "Fm=Unknown ";
			}
			else
			{
				memset(&cTmp, '\0', FILE_TMP_SIZE);
				fscanf_s(fi,"%s", &cTmp, 40);
				strTmp.sprintf("Fm=%s ", &cTmp);
				fclose(fi);
			}
		}
		else
		{	// File Not Found
				strTmp = "Fm=Unknown ";
		}
		m_strMonitor = m_strMonitor + strTmp;

		//== 통신개시
		//==진단서버

		TemStr.printf(m_defualtparam->VHLName, "%s");
		m_pUDPCommunication->MyInfoUpdate(MACHINETYPE_VHL,TemStr,ADDRESS_GET_DEFAULT_PORT+m_defualtparam->nPortNumAdd,CONTROL_DEFAULT_PORT+m_defualtparam->nPortNumAdd);
		m_pUDPDiag->SetDiagInfo(m_defualtparam->strDiagIpAddr,CONTROL_DIAG_DEFAULT_PORT,TemStr,MACHINETYPE_VHL);
		TemStr.printf(m_defualtparam->BroadCasMask, "%s");
		m_pUDPCommunication->SetBroadCastMask(TemStr);
		m_pUDPCommunication->Address_Broadcast();   //주소획득 연결 실패시 재시도 해야됨.
		m_pOHTMainStatus->CheckCommfailtime = timeGetTime();

//		if(m_defualtparam->OHTType == OHTTYPE_NONE)
//		{
////			Error = ERR_LAYOUT_OHT_LIMIT;
//			nCheckErrorOHTType = ERR_LAYOUT_OHT_LIMIT;
//		}
//		else
//		{
//			//OHT Type에따라 분류될 파라미터는 별도로 정의
//
//
//
//        }



		ChangeMainTaskState(MAIN_TASK_SOCKETENABLE);

		break;

		// -------------------------------------------
		// 1 Step : MAIN_TASK_SOCKETENABLE
	case MAIN_TASK_SOCKETENABLE:
		// Step 1. Socket Active
		Sleep(100);

#ifdef USE_LOG_MEMO
		ADD_MEMO_T("Socket Active....");
#endif /* USE_LOG_MEMO */

		// ji7412.lee 2013.05.27 : Remocon Open
		ComOpenForRemocon();

		//2016.05.16 doori.shin
		m_DiagManager->SetDiagStatCheckTimer();	//최초 실행시 상태정보 업데이트 되었는지 체크하는 타이머
		SendReqeustDiagStatData();			//Diag에게 OHT상태정보 요청

		m_DiagManager->CheckOHTStatus();	//OHT 진단 Status 정보 업데이트
		SendStatusDiagData(m_DiagManager->GetDiagOHTstatus());	//진단으로 Status 정보 전송

		//del txt files
		//system("touch D:\\log\\test.txt");
		system("del D:\\log\\*.txt");
		system("del /q C:\\User\\OHT\\DataFiles\\Down\\*.*");
	
		ChangeMainTaskState(MAIN_TASK_READY);
		break;

		// -------------------------------------------
		// 1 Step : MAIN_TASK_READY
	case MAIN_TASK_READY:
#ifdef USE_LOG_MEMO
		ADD_MEMO_T("Task Ready....");
#endif /* USE_LOG_MEMO */

		m_pOHTMainStatus->fMainReady = true;

		//퍼즈상태 해제
		m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_RESUME_OCS);

		ChangeMainTaskState(MAIN_TASK_CHECKCMD);
		break;

		// -------------------------------------------
		// 1 Step : MAIN_TASK_CHECKCMD
	case MAIN_TASK_CHECKCMD:
		// Step 1. 에러 체크 함수 호출
		// -------------------------------------------
		// 1 Step : MAIN_TASK_CHECKCMD
  //	case MAIN_TASK_CHECKCMD:

		if(nCheckErrorOHTType == ERR_LAYOUT_OHT_LIMIT)
            Error = nCheckErrorOHTType;

        if (((m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist)||
        (m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status==STS_UNLOADING) ||  //AOHC-233 MapUpdate 시점 개선위해 추가
        (m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status==STS_LOADING)) &&  //AOHC-233 MapUpdate 시점 개선위해 추
		   (m_pOHTMainStatus->DataUseFlag.fReadySwapping==true) &&
		   (m_pOHTMainStatus->DataUseFlag.fSwapping == false))
		   	m_pOHTMainStatus->DataUseFlag.fSwapping = true;

		if ((m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist) &&
		   (m_pOHTMainStatus->DataUseFlag.fReadySwappingParam==true) &&
		   (m_pOHTMainStatus->DataUseFlag.fSwappingParam == false))
			m_pOHTMainStatus->DataUseFlag.fSwappingParam = true;

		// CmdList 체크
		PKT_TASKCMDDATA * pCurCmd; // Command를 위한 포인터 변수
		// -------------------------------------------
		// [BEGIN] by zzang9un 2013.08.16 : Quick Command 처리
		switch (m_pOHTMainStatus->QuickCmd)
		{
		case QUICK_CMD_PAUSE_OCS:
			SendQuickCmd2Execute(EXECUTE_QUICK_CMD_PAUSE_OCS);
			m_pOHTMainStatus->QuickCmd = QUICK_CMD_NONE;
			ADD_LOG_TASKCONTROL("[MAIN_TASK_CHECKCMD][SendQuickCmd2Execute:%d]",
				EXECUTE_QUICK_CMD_PAUSE_OCS);
			break;

		case QUICK_CMD_RESUME_OCS:
		// 2016-12-29, Add by Labeler
		// OCS, TP, CID Pause가 모두 해제 되어야지만 Resume 명령 발생시키기 위해 주석 처리
			SendQuickCmd2Execute(EXECUTE_QUICK_CMD_RESUME_OCS);
			m_pOHTMainStatus->QuickCmd = QUICK_CMD_NONE;
			ADD_LOG_TASKCONTROL("[MAIN_TASK_CHECKCMD][SendQuickCmd2Execute:%d]",
				EXECUTE_QUICK_CMD_RESUME_OCS);
			break;

		case QUICK_CMD_PAUSE_TP:
			SendQuickCmd2Execute(EXECUTE_QUICK_CMD_PAUSE_TP);
			m_pOHTMainStatus->QuickCmd = QUICK_CMD_NONE;
			ADD_LOG_TASKCONTROL("[MAIN_TASK_CHECKCMD][SendQuickCmd2Execute:%d]",
				EXECUTE_QUICK_CMD_PAUSE_TP);
			break;

		case QUICK_CMD_TURN_OFF:      //기동 Off
			SendQuickCmd2Execute(EXECUTE_QUICK_CMD_TURN_OFF);
			m_pOHTMainStatus->QuickCmd = QUICK_CMD_NONE;
			ADD_LOG_TASKCONTROL("[MAIN_TASK_CHECKCMD][SendQuickCmd2Execute:%d]",
				EXECUTE_QUICK_CMD_TURN_OFF);
			if((m_pExecuteInfo->ExecutePositionInfo.Marked == true) && (m_pExecuteInfo->ExecutePositionInfo.CurrStation !=0))
			{
				//초기위치 변경, VHL Map Data로 변경.
				if(m_OCSStaionInfo.StationID != m_pExecuteInfo->ExecutePositionInfo.CurrStation)
				{
					STATION_DATA STStationData;
					GetStationData(&STStationData,m_pExecuteInfo->ExecutePositionInfo.CurrStation);
					m_OCSStaionInfo.StationID = STStationData.StationID;
					m_OCSStaionInfo.StationType = STStationData.StationType;
					m_OCSStaionInfo.ParentNodeID = STStationData.ParentNodeID;
					m_OCSStaionInfo.NextNodeID = STStationData.NextNodeID;
					m_OCSStaionInfo.Offset = STStationData.StationOffset;
					m_OCSStaionInfo.Safety_Level = STStationData.Safety_Level;
                }
			}
			break;

		case QUICK_CMD_TURN_ON:      //기동 Off
			SendQuickCmd2Execute(EXECUTE_QUICK_CMD_TURN_ON);
			m_pOHTMainStatus->QuickCmd = QUICK_CMD_NONE;
			ADD_LOG_TASKCONTROL("[MAIN_TASK_CHECKCMD][SendQuickCmd2Execute:%d]",
				EXECUTE_QUICK_CMD_TURN_ON);
			break;

		case QUICK_CMD_RESUME_TP:
// 2016-12-29, Add by Labeler
// OCS, TP, CID Pause가 모두 해제 되어야지만 Resume 명령 발생시키기 위해 주석 처리
			SendQuickCmd2Execute(EXECUTE_QUICK_CMD_RESUME_TP);
			m_pOHTMainStatus->QuickCmd = QUICK_CMD_NONE;
			ADD_LOG_TASKCONTROL("[MAIN_TASK_CHECKCMD][SendQuickCmd2Execute:%d]",
				EXECUTE_QUICK_CMD_RESUME_TP);
			break;

		case QUICK_CMD_STOP: // E-Stop
		   //	ChangeMode(MODE_MANUAL);

			//20160414 Test 해볼것.
			m_pCmdManager->InitTaskList();
	//		InitClearCycle();

			SendQuickCmd2Execute(EXECUTE_QUICK_CMD_STOP);
			ADD_LOG_TASKCONTROL("[MAIN_TASK_CHECKCMD][SendQuickCmd2Execute:%d]",
				EXECUTE_QUICK_CMD_STOP);

//			if (m_pExecuteInfo->bJogInterlockIgnore == true) {
//				EnterCriticalSection(&OHTMainForm->m_CS);
//				m_pExecuteInfo->bJogInterlockIgnore = false;
//				LeaveCriticalSection(&OHTMainForm->m_CS);
//			}

			m_pOHTMainStatus->QuickCmd = QUICK_CMD_NONE; // 처리한 E-Stop은 복귀하도록 변경

            bAutoChange = false;

			break;

		case QUICK_CMD_IDRESET: // ID Reset 명령
			if (CheckStatusForReset())
			{
                if(GetExecuteStatus() == STS_AUTO_COMPLETE)
                {
                    bAutoChange = true;
                }
                else
                {
                    bAutoChange = false;
                }

//                m_pOHTMainStatus->bCheckCycleRunning = false;
				InitClearCycle();

				ErrorCode_Main = NO_ERR;
				SendQuickCmd2Execute(EXECUTE_QUICK_CMD_RESET);
				ADD_LOG_TASKCONTROL(
					"[MAIN_TASK_CHECKCMD][SendQuickCmd2Execute:%d]",
					EXECUTE_QUICK_CMD_RESET);

				m_pOHTMainStatus->QuickCmd = QUICK_CMD_NONE;
				// 처리한 Quick Command 복귀하도록 변경

                // 2016-12-26, Add by Labeler
                // Handy TP Longrun 동작 중일 경우, Lognrun 동작 Clear
                if(GetHandyTPLongRun() != HANDTP_LONGRUN_NONE)
                {
                    SetHandyTPLongRun(HANDTP_LONGRUN_NONE);
                }

				if (m_pMapMake_Status->bReady) {
					// by zzang9un 2013.08.16 : ID Reset인 경우 Data Flag도 처리해야 함
					m_pMapMake_Status->bReady = false;
					m_pMapMake_Status->bStart = false;
					ADD_LOG_MAPMAKE("[Exception][QUICK_CMD]IDRESET");

					Send_MapMakeResult("[Exception][QUICK_CMD]IDRESET");
				}
			}
			break;

		default:
			break;

		}
		// [END] by zzang9un 2013.08.16 : Quick Command 처리
		// -------------------------------------------
		if((m_pCmdManager->GetTaskCount() !=0) && (CheckStatusForTask()==true))
		{
			// CmdList가 있는 경우
			pCurCmd = (PKT_TASKCMDDATA*)m_pCmdManager->ReferFirstTask();

			// 오토 전환 명령이 왔거나, 대차 패턴 확인 중이거나, FN009 작업중이면 auto 작업 중이라고 판단
			bool bDoAuto = (pCurCmd->DetailTagSort == TAGTYPE_AUTOMODE) || m_StepDetectAreaCheck.Step != 0 || m_pServoControl->Fn009ing();

			m_bAutoChaning = bDoAuto ? 1 : 0;

			// 특정 명령 처리 : E-Stop 외 Quick Command 처리
			if (pCurCmd->CategoryDiv == CATEGORY_PRIORITY)
			{
				// by zzang9un 2012.12.10 : Auto, Manual 모드 전환 명령의 경우 우선 처리
				// by zzang9un 2013.04.09 : switch로 변경
				switch (pCurCmd->DetailTagSort)
				{
				case TAGTYPE_MANUALMODE:
					if (CheckStatusForCommon())
					{
						pCurCmd = (PKT_TASKCMDDATA*)m_pCmdManager->PopFirstTask();

						// Mode를 변경할 Status인 경우에만 모드 변경
						ChangeMode(MODE_MANUAL);

						ADD_LOG_TASKCONTROL("[Mode Change][A->M]");

						m_pCmdManager->InitTaskList();
						InitExcuteOverRun();
						InitClearCycle();

					
						if (m_pMapMake_Status->bReady)
						{
							// by zzang9un 2014.01.15 : Map Make Flag 초기화
							m_pMapMake_Status->bReady = false;
							m_pMapMake_Status->bStart = false;
							ADD_LOG_MAPMAKE("[Exception][QUICK_CMD]TPCMD_OHT_MANUALMODE");

							Send_MapMakeResult("[Exception][QUICK_CMD]TPCMD_OHT_MANUALMODE");
						}
						Change_FDC_Manual_Mode(0);

						delete pCurCmd;
					}
					else
					{
						if(m_bCheckAutoToManual == false)
						{
							m_bCheckAutoToManual = true;
							dCheckAutoToManualTime = timeGetTime();
						}

						if(m_bCheckAutoToManual == true)
						{

							if(OHTUTILITY::PreventOverflowGetTimeMs(dCheckAutoToManualTime) > 10000)
							{

								//메뉴얼 명령 삭제
								ADD_LOG_TASKCONTROL("[Mode Change Delete!!][Timeout Manual Mode Change]");
								delete (PKT_TASKCMDDATA*)m_pCmdManager->PopFirstTask();

								m_bCheckAutoToManual = false;
								dCheckAutoToManualTime = timeGetTime();
                            }
                        }
                    }
					break;

				case TAGTYPE_AUTOMODE:
				
				
					// Manual --> Auto 변경시
					// 대차감지 센서 세팅값 확인하기  - m_StepDetectAreaCheck 스텝
					// 스텝 대기
					if(OHTMainForm->GetStatusCheckThread()->GetDistanceSensorType() != DISTANCE_SENSOR_DISABLE)
					{
						OHTMainForm->GetStatusCheckThread()->SetDistanceSensorType(DISTANCE_SENSOR_DISABLE);
                    }

#if(OHT_NAME != OHT_NAME_STD_V81S)
					if(m_StepDetectAreaCheck.Step != 0)
						break;

					if(!m_StepDetectAreaCheck.bAllDone)   // Go to step run
					{
						m_StepDetectAreaCheck.Step = 100;

						break;
					}
					else	 // Step Finished
					{
						//m_StepDetectAreaCheck.bAllDone = false;  FN009 끝나고 Alldone을 false로 만드는게 유리하다
						bool bChkResult = m_StepDetectAreaCheck.nResult == 0;	// Get result from step
						if(bChkResult)
						{
							Error = NO_ERR;
                            OHTMainForm->GetOHTStatus()->DataUseFlag.fUBGLoad = true;

						}
						else
						{
							m_StepDetectAreaCheck.bAllDone = false;
                            OHTMainForm->GetOHTStatus()->DataUseFlag.fUBGLoad = 0xFF;
							Error = ERR_UBG_NO_DATA;
							sprintf(szLogMsg, "UBG Detect Area Info Check Fail. m_StepDetectAreaCheck FailCode=%d, FailStep=%d", m_StepDetectAreaCheck.nResult, m_StepDetectAreaCheck.ErrorStep);
							RapidLog(RLOG_UBG_CHK, szLogMsg);
							break;
						}
					}
#else
					OHTMainForm->GetOHTStatus()->DataUseFlag.fUBGLoad = true;
#endif

					// Trans.ini M->A true로 바로 변환   다솜 프로
					m_pTransControl->ChangeUse(USE_SET_EQPIO, true);
					m_pTransControl->ChangeUse(USE_SET_GRIPPERSEN, true);
					m_pTransControl->ChangeUse(USE_SET_OSCILLATIONSEN, true);
					m_pTransControl->ChangeUse(USE_SET_HANDPUSHSENCHK, true);
					m_pTransControl->ChangeUse(USE_SET_LOOKDOWNSENSOR, true);
					m_pTransControl->ChangeUse(USE_SET_SHUTTERCONTROL, true);
					m_pTransControl->ChangeUse(USE_SET_HOISTHOMESENSOR, true);
					m_pTransControl->ChangeUse(USE_SET_HANDCONTROL, true);
                    m_pTransControl->ChangeUse(USE_SET_STBSEN, true);
					m_pTransControl->ChangeUse(USE_SET_UTBSEN, true);
					m_pTransControl->ChangeUse(USE_SET_UTBFOUPCHECK, true);
					m_pTransControl->ChangeUse(USE_SET_PIOIGNOREERROR, true);
//				   	m_pTransControl->ChangeUse(USE_SET_OUTTRIGGER, false);
//				  	m_pTransControl->ChangeUse(USE_SET_OSCILLATION_PAUSE, true);


					sprintf(szLogMsg, "UBG Detect Area Check Finished OK!! Error=%d", Error);
					RapidLog(RLOG_UBG_CHK, szLogMsg);

					
					
					pCurCmd = (PKT_TASKCMDDATA*)m_pCmdManager->PopFirstTask();

					// by zzang9un 2013.08.19 : Mark된 상태 확인
					if (m_pExecuteInfo->ExecutePositionInfo.Marked)
					{
						switch (m_pExecuteInfo->ExecutePositionInfo.StationType)
						{
						case MARK_TYPE_NODE: // 주행 Node에 마크된 경우
							if (Error == NO_ERR && CheckStatusForAuto())
							{
                                //Node일 경우 분기정보 초기화
								m_pExecuteInfo->nCheckDivInfo =PATH_DIVERGENCE_NONE;

								if(m_pServoControl->IsFn009Flag()==false)
								{
									if(!GetNodeTypeSlop())
									{

										if((m_pServoControl->GetFn009Step() == SERVOPACK_FN009_STEP_INIT) || (m_pServoControl->GetFn009Step() == SERVOPACK_FN009_STEP_ERROR))
                                         {
											m_pServoControl->SetFn009Step(SERVOPACK_FN009_STEP_1);
										 }
										 break;
										//										if(m_pHWSet->m_pServoMonitoring->SetFn009(1))
//										{
//											if(m_pHWSet->m_pServoMonitoring->SetFn009(2))
//											{
//												bcheckFN009 = true;   //팝업 띄우기.
//
//											}
//										}

									}
								}

								Error = OHTMainForm->GetReadyToOperation();

								if (Error == NO_ERR) 
								{

									if(IsBeforeAutoCheckPassPermit(false))
									{

										memset(&m_OCSStaionInfo, 0x00, sizeof(NODE_INFO_STD_STATION));

										m_pOHTMainStatus->WaitCycle = CYCLE::CycleNotExist;
										m_pOHTMainStatus->WaitTargetStationNID = 0;

										ChangeMode(MODE_AUTO);
										if(m_pExecuteInfo->bJogInterlockIgnore == true)
										{
											EnterCriticalSection(&OHTMainForm->m_CS);
											m_pExecuteInfo->bJogInterlockIgnore = false;
											LeaveCriticalSection(&OHTMainForm->m_CS);
										}
									// by zzang9un 2014.04.14 : Auto 명령 수행후 Command List 초기화
										m_pCmdManager->InitTaskList();
//										m_pCmdManager->InitPassPermitList();

										InitClearCycle();
										InitExcuteOverRun();
								   //	m_pCmdManager->InitCmdList();

									ADD_LOG_TASKCONTROL(
										"[Mode Change][M->A], NodeID=%06d",
										m_pExecuteInfo->ExecutePositionInfo.
										CurrNode);
										Change_FDC_Auto_Mode();
									}


								}
							}
							break;

						case MARK_TYPE_TAG_STATION: // 이적재 Tag에 마크된 경우
						case MARK_TYPE_TAG_STATION_BACK: // 이적재 Tag에 마크된 경우
						case MARK_TYPE_QR_STATION_LEFT:
						case MARK_TYPE_QR_STATION_RIGHT:
							Error = GetStationData(&StationData, m_pExecuteInfo->ExecutePositionInfo.CurrStation);
							if (Error)
							{
								ADD_LOG_TASKCONTROL(
									"[Mode Change][M->A][Error=%d] Get Station Data",
									Error);
							}
							else
							{
								// Station Data를 Execute로 전달함

								EnterCriticalSection(&OHTMainForm->m_CS);
								m_pExecuteInfo->nCheckDivInfo = m_pPathSearch->GetStationDivInfo(OHTMainForm->GetMap(),StationData.ParentNodeID,StationData.NextNodeID);
								LeaveCriticalSection(&OHTMainForm->m_CS);


								Error = MakeSetStationOffset(&StationData);

								if (Error == NO_ERR)
								{

									pTempSteering = m_pPathSearch->GetSteeringInfo(OHTMainForm->GetMap(),StationData.ParentNodeID,StationData.NextNodeID);

									if(pTempSteering ==PATH_STEERING_LEFT)
									{
										if(!m_pHWSet->m_pSteer->IsLeftPos(BOTH))
										{
											Error =  ERR_STR_F_LEFT_POS_AND_R_LEFT_POS;
											ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
														ERR_STR_F_LEFT_POS_AND_R_LEFT_POS,
														m_pOHTMainStatus->StatusCommon.CurNodeID,
														m_pExecuteInfo->ExecutePositionInfo.NextNode,
														m_pOHTMainStatus->StatusCommon.CurNodeOffset,
														m_pOHTMainStatus->StatusCommon.ExistFoup,
														m_pOHTMainStatus->StatusCommon.StopStationID,
														m_pHWSet->m_pSteer->IsLeftPos(FRONT),
														m_pHWSet->m_pSteer->IsRightPos(FRONT),
														m_pHWSet->m_pSteer->IsLeftPos(REAR),
														m_pHWSet->m_pSteer->IsRightPos(REAR));
										}

									}
									else if(pTempSteering ==PATH_STEERING_RIGHT)
									{
										if(!m_pHWSet->m_pSteer->IsRightPos(BOTH))
										{
											Error = ERR_STR_F_RIGHT_POS_AND_R_RIGHT_POS;
											ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
														ERR_STR_F_RIGHT_POS_AND_R_RIGHT_POS,
														m_pOHTMainStatus->StatusCommon.CurNodeID,
														m_pExecuteInfo->ExecutePositionInfo.NextNode,
														m_pOHTMainStatus->StatusCommon.CurNodeOffset,
														m_pOHTMainStatus->StatusCommon.ExistFoup,
														m_pOHTMainStatus->StatusCommon.StopStationID,
														m_pHWSet->m_pSteer->IsLeftPos(FRONT),
														m_pHWSet->m_pSteer->IsRightPos(FRONT),
														m_pHWSet->m_pSteer->IsLeftPos(REAR),
														m_pHWSet->m_pSteer->IsRightPos(REAR));
										}

                                    }

								}
								int CIDResetNode_arr = INDEX_NONE;
								int CIDResetNode = 0;
								int CIDResetNodeType = 0;

								CIDResetNode_arr = OHTMainForm->GetMap()->FindNodeIndexFromCIDReset(StationData.NextNodeID);
								if (CIDResetNode_arr != INDEX_NONE)
								{
								  CIDResetNode = OHTMainForm->GetCID_STD()->m_NodeArray[CIDResetNode_arr].ResetNode;
								  CIDResetNodeType = OHTMainForm->GetCID_STD()->m_NodeArray[CIDResetNode_arr].ResetNodeType;
								}
								
								ADD_LOG_TASKCONTROL("[CID] StationData.NextNodeID : %d, CIDResetNode : %d", StationData.NextNodeID, CIDResetNode);

								if(CIDResetNode == StationData.NextNodeID)//StationData.NextNodeID 가 Resetnode 이면, 점유
								{
									ADD_LOG_TASKCONTROL("[CID] OUT_CID_Occupancy before AutoMode");
									//1 조향 방향 찾기
									SteerFront = pTempSteering;

									//2 CID 모드 선택(CID모드 및 매뉴얼 설정)
									//pHWSet->m_pCID->ElevetorOperatingModeSet(ON);//CID Mode
									m_pHWCommon->SetBit(OUT_CID_Operation_Mode, ON);
									Sleep(20);
									//ADD_LOG_TASKCONTROL("[CID] CID_Mode_ON");
									if (m_pHWCommon->m_Output.CID_Abnormal == OFF)
									{
										m_pHWCommon->SetBit(OUT_CID_Abnormal, ON);
										//ADD_LOG_TASKCONTROL("[CID] Abnormal_ON");
									}
									Sleep(20);

									//3 Select ON
									if(SteerFront == HW_STEER_DIRECTION_RIGHT)
									{	
										//pHWSet->m_pCID->CID_Out_Right_Select(ON);
										m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, ON);
										ADD_LOG_TASKCONTROL("[CID] OUT_CID_Right_Select_ON");
									}
									else if(SteerFront == HW_STEER_DIRECTION_LEFT)
									{
										//pHWSet->m_pCID->CID_Out_Left_Select(ON);
										m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, ON);
										ADD_LOG_TASKCONTROL("[CID] OUT_CID_Left_Select_ON");
									}
									else
									{}
									Sleep(200);//100ms 기다려도 안되는 경우 존재 
									
									//4 Link 확인 후 점유
									if(m_pHWCommon->m_Input.CID_Link_Compt_IN == ON)
									{
										m_pHWCommon->SetBit(OUT_CID_Occupancy, ON);
										ADD_LOG_TASKCONTROL("[CID] OUT_CID_Occupancy");
										//ADD_LOG_TASKCONTROL("[CID] sleep(200)");
										//GetReadyToOperation 함수에서 CID init 관련 처리 필요!!
									}
									else
									{
										// 링크 확인 후 점유가 안되면 Automode 전환 안되도록
										break;
									}
								}

								// 에러 없는 경우 모드 전환
								if ((Error == NO_ERR) && (CheckStatusForAuto()))
								{

									if(m_pServoControl->IsFn009Flag()==false)
									{
										if(!GetNodeTypeSlop())
										{
											 if((m_pServoControl->GetFn009Step() == SERVOPACK_FN009_STEP_INIT) || (m_pServoControl->GetFn009Step() == SERVOPACK_FN009_STEP_ERROR))
											 {
												m_pServoControl->SetFn009Step(SERVOPACK_FN009_STEP_1);
											 }
											 break;
											/*
											if(m_pHWSet->m_pServoMonitoring->SetFn009(1))
											{
												if(m_pHWSet->m_pServoMonitoring->SetFn009(2))
												{
													bcheckFN009 = true;

												}
											}
											*/
										}
									}

									Error = OHTMainForm->GetReadyToOperation();
									if (Error == NO_ERR)
									{
										if(IsBeforeAutoCheckPassPermit(true,StationData.ParentNodeID, StationData.NextNodeID))
										{

											m_OCSStaionInfo.StationID = StationData.StationID;
											m_OCSStaionInfo.StationType = StationData.StationType;
											m_OCSStaionInfo.ParentNodeID = StationData.ParentNodeID;
											m_OCSStaionInfo.NextNodeID = StationData.NextNodeID;
											m_OCSStaionInfo.Offset = StationData.StationOffset;
											m_OCSStaionInfo.Safety_Level = StationData.Safety_Level;

											// Mode를 변경할 Status인 경우에만 모드 변경
											m_pOHTMainStatus->WaitCycle = CYCLE::CycleNotExist;
											m_pOHTMainStatus->WaitTargetStationNID = 0;



											ChangeMode(MODE_AUTO);
										    if(m_pExecuteInfo->bJogInterlockIgnore == true)
											{
												EnterCriticalSection(&OHTMainForm->m_CS);
												m_pExecuteInfo->bJogInterlockIgnore = false;
												LeaveCriticalSection(&OHTMainForm->m_CS);
											}
											// by zzang9un 2014.04.14 : Auto 명령 수행후 Command List 초기화
										   //	m_pCmdManager->InitCmdList();
											m_pCmdManager->InitTaskList();
	//										m_pCmdManager->InitPassPermitList();


											InitClearCycle();
											InitExcuteOverRun();

											ADD_LOG_TASKCONTROL(
											"[Mode Change][M->A], StationID=%06d, Type=%d, Offset=%d, P-Node=%06d, N-Node=%06d",
											StationData.StationID,
											StationData.StationType,
											StationData.StationOffset,
											StationData.ParentNodeID,
											StationData.NextNodeID);

											Change_FDC_Auto_Mode();
										}
									  }
									}
								}

							break;
						}
					}

					delete pCurCmd;
					break;

				case TAGTYPE_RESET:
					pCurCmd = (PKT_TASKCMDDATA*)m_pCmdManager->PopFirstTask();
					bcheckvirusError = true;

					if (CheckStatusForReset() && (GetCurrentMode() == MODE_MANUAL) && (m_pServoControl->Fn009ing()==false) )
					{

                        // 2017-01-15, Add by Labeler
                        // OCS, TP, CID Pause 걸린 상태에서 Error 발생 할 경우, Reset 시 Pause 해제 처리 추가
                        if((m_pExecuteInfo->bTpPause != false) || (m_pExecuteInfo->bOcsPause != false) || (m_pHWSet->m_pCID->GetCIDPause() != false))
                        {
                            m_pExecuteInfo->bTpPause = false;
                            m_pExecuteInfo->bOcsPause = false;
                            m_pHWSet->m_pCID->SetCIDPause(false);
                        }

						SendQuickCmd2Execute(EXECUTE_QUICK_CMD_RESET);
						ADD_LOG_TASKCONTROL(
							"[TPCMD_OHT_RESET][SendQuickCmd2Execute:%d]",
							EXECUTE_QUICK_CMD_RESET);

						// by zzang9un 2013.03.04 : Reset 명령의 경우 Main의 에러를 클리어 한다.
						// 임시로 이곳에 구현함... 근본적인 해결 필요함.
						ErrorCode_Main = NO_ERR;

//						if (m_pExecuteInfo->bJogInterlockIgnore == true)
//						{
//							EnterCriticalSection(&OHTMainForm->m_CS);
//							m_pExecuteInfo->bJogInterlockIgnore = false;
//							LeaveCriticalSection(&OHTMainForm->m_CS);
//						}

						// by zzang9un 2013.10.03 : 최종 목적지 변수를 초기화
						m_pOHTMainStatus->FinalNodeOfPathSearch = 0;
						m_pOHTMainStatus->FinalEndNodeOfPathSearch = 0;
						m_pOHTMainStatus->FinalStationOfPathSearch = 0;
//						m_pOHTMainStatus->FinalSeqNumOfPP = 0;
						//m_pCmdManager->InitPreOCSCmdBuffer();

                        // 2016-12-26, Add by Labeler
                        // Handy TP Longrun 동작 중일 경우, Lognrun 동작 Clear
                        if(GetHandyTPLongRun() != HANDTP_LONGRUN_NONE)
                        {
                            SetHandyTPLongRun(HANDTP_LONGRUN_NONE);
                        }

						InitClearCycle();
                        bAutoChange = false;
						OHTMainForm->m_MapUpdateResult = NO_ERR;
						// by zzang9un 2014.04.14 : Reset 명령 수행후 Command List 초기화
						//m_pCmdManager->InitCmdList();
					}
					delete pCurCmd;
					break;
					
				case TAGTYPE_AUTOCHANGE:				
					if(!m_StepAutoChangeReset.bAllDone)
					{
						break;
					}					
					pCurCmd = (PKT_TASKCMDDATA*)m_pCmdManager->PopFirstTask();
					bcheckvirusError = true;					
					delete pCurCmd;
					Change2AutoMode();
										Change_FDC_Auto_Mode();
					break;

				default:
					break;
				}
			}
			else
			{
				try{
					if(m_pServoControl->Fn009ing()==false)
						Error = SuperviseTaskList2CycleList();
				}catch(...)
				{
					m_pCmdManager->InitTaskList();
					InitClearCycle();
					InitExcuteOverRun();
					ChangeCmdIDAndCycle(0,0,0,true);

					ADD_LOG_TASKCONTROL("SuperviseTaskList2CycleList Exception!!!!!!");
				}

			}
		}
		 //puting 추가된 함수.
		 /// 진척보고(X), 통과허가는 Cycle 수행중일때만 보고
		//진척보고는 바뀌면 무조건 보고

		try{
		   if(Error== NO_ERR)	Error = SuperviseCycleList2ExcuteInfo();
		}
		catch(...)
		   ADD_LOG_TASKCONTROL("SuperviseCycleList2ExcuteInfo Exception!!!!!!");

		  if(Error== NO_ERR)
		  {
			 if((GetCurrentMode() == MODE_AUTO) &&(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist))
			 {
				if(GetExecuteStatus() == STS_UNLOADING  ||   GetExecuteStatus() ==  STS_LOADING )
				{
				   Error = ERR_CMD_CANCEL;    //작업취소 관련 에러 처리. 오토에서 싸이클이 없는데 이적재 작업중일때 에러 처리. E460
                	ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
							ERR_CMD_CANCEL,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID);
                }
             }
		  }

		m_pServoControl->SMCH_Update();

		if((m_pServoControl->GetFn009Step() == SERVOPACK_FN009_STEP_FINAL)
		|| (m_pServoControl->GetFn009Step() == SERVOPACK_FN009_STEP_INIT)
		|| (m_pServoControl->GetFn009Step() == SERVOPACK_FN009_STEP_ERROR))
		{
			if(m_pServoControl->GetFn009Step() == SERVOPACK_FN009_STEP_FINAL)
			{
				if((GetExecuteStatus() == STS_AUTO_COMPLETE) && (bAutoChange != true))
				{
					if((CheckError() == NO_ERR) && (GetCurrentMode() == MODE_MANUAL))
					{
						m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_IDRESET);
						ADD_LOG_TASKCONTROL("Reset for Auto Change");
					}
				}else
				{
					PKT_TASKCMDDATA pTempCmdPktData;
					pTempCmdPktData.DetailTagSort = TAGTYPE_AUTOMODE;
					pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;

					m_pCmdManager->MakeTaskList_Standard(&pTempCmdPktData);
				}
			}
			else if(m_pServoControl->GetFn009Step() == SERVOPACK_FN009_STEP_INIT)
			{
              // 2017-01-14, Add by Labeler, Auto Change 전환 위해 Reset Logic 추가
				if((GetExecuteStatus() == STS_AUTO_COMPLETE) && (bAutoChange != true))
				{
					if((CheckError() == NO_ERR) && (GetCurrentMode() == MODE_MANUAL))
					{
						m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_IDRESET);
						ADD_LOG_TASKCONTROL("Reset for Auto Change");
					}
				}

				// 2017-01-14, Add by Labeler, Auto Change 전환 위해 Auto 전환 Logic 추가
				if((bAutoChange == true) && (GetExecuteStatus() == STS_INIT))
				{
					PKT_OCS2OHT SEND_PACKET;
					PKT_ACK_OHT2OCS pAckPkt;
					PKT_OHT2TP_RESPONSE AckPktPC;
					int QuickCmd =0;
					int iPacketSize = sizeof(PKT_OCS2OHT);
					memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

					SEND_PACKET.TAG.Value = TagID::CMD_AUTOMODE_TP_OHT;
					bAutoChange = false;

					CheckCMDData2TaskList(&SEND_PACKET, &pAckPkt, &AckPktPC, QuickCmd);
					ADD_LOG_TASKCONTROL("Auto for Auto Change");
				}
            }

			m_pServoControl->SetFn009Step(SERVOPACK_FN009_STEP_INIT);
		}

/*
        // 2017-01-14, Add by Labeler, Auto Change 전환 위해 Reset Logic 추가
        if((GetExecuteStatus() == STS_AUTO_COMPLETE) && (bAutoChange != true))
        {
            if((CheckError() == NO_ERR) && (GetCurrentMode() == MODE_MANUAL))
            {
                m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_IDRESET);
                ADD_LOG_TASKCONTROL("Reset for Auto Change");
            }
        }

        // 2017-01-14, Add by Labeler, Auto Change 전환 위해 Auto 전환 Logic 추가
        if((bAutoChange == true) && (GetExecuteStatus() == STS_INIT))
        {
            PKT_OCS2OHT SEND_PACKET;
            PKT_ACK_OHT2OCS pAckPkt;
            PKT_OHT2TP_RESPONSE AckPktPC;
            int QuickCmd;
            int iPacketSize = sizeof(PKT_OCS2OHT);
            memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

            SEND_PACKET.TAG.Value = TagID::CMD_AUTOMODE_TP_OHT;
            bAutoChange = false;

            CheckCMDData2TaskList(&SEND_PACKET, &pAckPkt, &AckPktPC, QuickCmd);
            ADD_LOG_TASKCONTROL("Auto for Auto Change");
		}
*/
		// Path search 에러 발생
		if(m_nCountCheckPathSearch > 5)
		{
			ADD_LOG_TASKCONTROL("nCountCheckPathSearch Over!!");
			m_nCountCheckPathSearch = 0;
         	Error = ERR_PATHSEARCH_FAIL;
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
        }

		// Restart CMD Main->AS for AutoPatch
		if(bMainRestart == true
			&& (m_FIRMWARE_UPDATE_INFO.iMainUpdate == true || m_FIRMWARE_UPDATE_INFO.iAmcUpdate== true || m_FIRMWARE_UPDATE_INFO.iParamUpdate == true))
		{
			EXECUTE_INFO *tmp_ExecuteInfo = OHTMainForm->GetExecuteInfo();
			ADD_LOG_TASKCONTROL("FirmwareUpdate Main Change!! %X, %d, %d, %d", m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode, tmp_ExecuteInfo->ExecutePositionInfo.StationType, tmp_ExecuteInfo->ExecutePositionInfo.Marked, OHTMainForm->GetCycleList()->getCount());
			//Driving Node check
			if( tmp_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_NODE
				&& tmp_ExecuteInfo->ExecutePositionInfo.Marked == true
				&& m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO
				&& OHTMainForm->GetCycleList()->getCount() == 0
			  )
			{
				static int FirmwareUpdateCount = 0; // 완료 보고 후 Manual 전환하려고 추가

				if (FirmwareUpdateCount >= 100)
				{
					FirmwareUpdate_ModeChange();
					ADD_LOG_TASKCONTROL("FirmwareUpdate_ModeChange()!!");
					FirmwareUpdateCount = 0;
				}
				else
				{
					if(FirmwareUpdateFlag == true)
					{
						FirmwareUpdateCount = 97;
						ADD_LOG_TASKCONTROL("FirmwareUpdateCount = 97");
						FirmwareUpdateFlag = false;
					}
					else
					{
						FirmwareUpdateCount++;
					}
				}
			}
			else if( tmp_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_NODE
					&& tmp_ExecuteInfo->ExecutePositionInfo.Marked == true
					&& m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL
					&& OHTMainForm->GetCycleList()->getCount() == 0
				   )
			{
				FirmwareUpdate_RESTART(m_FIRMWARE_UPDATE_INFO);
				ADD_LOG_TASKCONTROL("FirmwareUpdate_RESTART!!");
				bMainRestart = false;
				Sleep(5000);
			}
			else
			{
				// AutoMode
				// AutoPatch 미진행
				if(tmp_ExecuteInfo->ExecutePositionInfo.StationType =! MARK_TYPE_NODE)
				{
					bMainRestart = false;
					ADD_LOG_TASKCONTROL("FirmwareUpdate_Fail(MARK_TYPE_TAG_STATION)");
					system("del /q C:\\User\\OHT\\DataFiles\\Down\\*.*");
				}
				else
				{

                }
			}
			// ...will be shutdown
		}

		// Map Make 시작 Flag 설정
		if (m_pMapMake_Status->bReady)
		{
			if (m_pExecuteInfo->OnAddingMapInfo.Info.FlagOnMapMaking ||
				(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status ==	STS_GOING))
			{
				m_pMapMake_Status->bStart = true; // start flag 설정
			}

			// Map Make 정보 처리
			if (m_pMapMake_Status->bStart) {

				if(Error== NO_ERR)	Error = HandleMapMake();

				// Map Make 종료 flag 설정
				// by zzang9un 2014.01.14 : 기존 종료 판단 조건
				// if(!m_pExecuteInfo->OnAddingMapInfo.Info.FlagOnMapMaking
				// && (m_pExecuteInfo->ListOfMapInfo.getCount() == 0)
				// && (m_pOHTMainStatus->MSCmdID.Info.Status == STS_ARRIVED))
				if ((m_pMapMake_Status->EndNodeID == m_pOHTMainStatus->StatusCommon.CurNodeID) &&
					(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status == STS_ARRIVED) &&
					(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist) &&
					(m_pExecuteInfo->ListOfMapInfo.getCount() == 0))
					{
					// Map make가 종료됨을 뜻함
					m_pMapMake_Status->bReady = false;
					m_pMapMake_Status->bStart = false;
					m_pMapMake_Status->bComplete = true;

					ADD_LOG_MAPMAKE("[Complete][Driving][%06d~%06d]", m_pMapMake_Status->StartNodeID, m_pMapMake_Status->EndNodeID);

					AnsiString strMapLog;
					strMapLog.sprintf("[Complete][Driving][%06d~%06d]", m_pMapMake_Status->StartNodeID, m_pMapMake_Status->EndNodeID);
					Send_MapMakeResult(strMapLog);
				}
				else if((m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status == STS_ERROR) &&
						(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist) &&
						(m_pExecuteInfo->ListOfMapInfo.getCount() == 0))
						{
							// Error에 의해 이전값 갱신을 위한 부분
							m_pMapMake_Status->bReady = false;
							m_pMapMake_Status->bStart = false;
							m_pMapMake_Status->bComplete = true;

							ADD_LOG_MAPMAKE("[Complete][Error][%06d][E:%d]",m_pOHTMainStatus->StatusCommon.CurNodeID,OHTError_Current);

							AnsiString strMapLog;
							strMapLog.sprintf("[Complete][Error][%06d][E:%d]", m_pOHTMainStatus->StatusCommon.CurNodeID,OHTError_Current);
							Send_MapMakeResult(strMapLog);
						}
			}
		}

		// 에러가 있는 경우 에러 처리
		if (Error)
		{
			ErrorCode_Main = Error;
		}
		else if(OHTMainForm->m_MapUpdateResult == ERR_MAPDATA_LOAD_FAIL)
		{
			ErrorCode_Main = ERR_MAPDATA_LOAD_FAIL;
		}
		else if(OHTMainForm->m_ParamUpdateResult == ERR_PARADATA_LOAD_FAIL)
		{
			ErrorCode_Main = ERR_PARADATA_LOAD_FAIL;
        }
		//ADD_LOG_TASKCONTROL("[TEST 0430]%d, %d", Error, OHTMainForm->m_MapUpdateResult);

		// Main Task 상태 변경
	   //	ChangeMainTaskState(MAIN_TASK_CHECKERROR);

		// ji7412.lee 2013.05.27 : Remocon Data를 읽어와서 ParsingTPCmd 전송
//		GetReadRemoconData();
//        ADD_LOG_TASKCONTROL("before CheckError %d",Error);
		Error = CheckError();

		if ((Error) ||
		   ((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning ==OFF)))
		{
			// by ks4423 : 에러 발생시 바이러스 감염 여부 확인
			if (bcheckvirusError) // 에러 발생시 최초 1회 실행을 위해
			{
				bcheckvirusError = false;
				CheckVirus();
			}

			m_FileMapMakeflag = false;
			m_FileTeachingflag = false;




			if (OHTError_Current != Error)
			{
				// by zzang9un 2013.08.26 : AutoRecovery 상태일 때는 Error 처리를 하지 않고 Errorcode만 반영한다.
				if(Error !=0)
				{
					if ((GetExecuteStatus() == STS_AUTORECOVERY) ||
						(GetExecuteStatus() == STS_AUTORECOVERY_BLANKTRANS) ||
						(GetExecuteStatus() == STS_AUTORECOVERY_DOUBLETRANS) ||
						(GetExecuteStatus() == STS_AUTORECOVERY_OVERRUN))

					{
						// Status와 Command ID를 변경
						ChangeStatus(GetExecuteStatus());


						// 지시ID로 변경해 줄것 .
					 //	ChangeCmdID(GetExecutePreCmdID(), GetExecuteCurCmdID());
						ADD_LOG_TASKCONTROL(
							"[MAIN_TASK_CHECKCMD][Change Status:%02X, Error:%d]",
							m_pOHTMainStatus->StatusCommon.MSCmdID.Info.
							Status, Error);
					}
					else
					{
						ADD_LOG_TASKCONTROL(
							"[MAIN_TASK_CHECKCMD][SendQuickCmd2Execute:%d][Status:%02X] Error:%d",
							EXECUTE_QUICK_CMD_STOP, GetExecuteStatus(), Error);
						m_pCmdManager->SendQuickCmd2Main(EXECUTE_QUICK_CMD_STOP);

						// 에러 발생 시 모드, 상태 변경
						if(m_OCSStaionInfo.StationID !=0)
							memset(&m_OCSStaionInfo, 0x00, sizeof(NODE_INFO_STD_STATION));

						ChangeMode(MODE_MANUAL);
						ChangeStatus(STS_ERROR);
						ADD_LOG_TASKCONTROL("[ChangeMode:%02X]Status:%02X, Error:%d"
							, MODE_MANUAL, STS_ERROR, Error);
						bErrorCIDOccupFlag = true;

						m_pOHTMainStatus->WaitCycle = CYCLE::CycleNotExist;
						m_pOHTMainStatus->WaitTargetStationNID = 0;

						dCheckAutoToErrorTime = timeGetTime();

						Change_FDC_Manual_Mode(Error);
					}
				}

				// 이전 에러 값에 현재 에러 저장
				OHTError_Current = Error;

				m_pOHTMainStatus->InnerErrCode  = OHTError_Current;

				if((OHTError_Current == ERR_AXIS1_F_AMP_FAULT) ||(OHTError_Current == ERR_AXIS1_R_AMP_FAULT) ||
				(OHTError_Current == ERR_AXIS2_AMP_FAULT) ||(OHTError_Current == ERR_AXIS3_AMP_FAULT) ||
				(OHTError_Current == ERR_AXIS1_F_ERROR_LIMIT) ||(OHTError_Current == ERR_AXIS1_R_ERROR_LIMIT) ||
				(OHTError_Current == ERR_AXIS1_F_ABORT_EVENT) ||(OHTError_Current == ERR_AXIS1_R_ABORT_EVENT) ||
				(OHTError_Current == ERR_AXIS2_ABORT_EVENT) ||(OHTError_Current == ERR_AXIS3_ABORT_EVENT))
				{
					GetServoAlarmCode(OHTError_Current);
				}

			}

			ChangeCmdID(GetExecutePreCmdID(), GetExecuteCurCmdID());



			// by zzang9un 2013.10.02 : 에러 발생 시 FinalNodeOfPathSearch 변수를 초기화함
				m_pOHTMainStatus->FinalNodeOfPathSearch = 0;
				m_pOHTMainStatus->FinalEndNodeOfPathSearch = 0;
				m_pOHTMainStatus->FinalStationOfPathSearch = 0;
//				m_pOHTMainStatus->FinalSeqNumOfPP = 0;
	   //		m_pCmdManager->InitPreOCSCmdBuffer();

			// by zzang9un 2014.04.14 : 에러 난 경우 Command List 초기화
			if ((((m_pExecuteInfo->ExecuteMode != EXECUTE_MODE_MANUAL) ||
				(!m_pExecuteInfo->bJogInterlockIgnore)) ||
				((m_pOHTMainStatus->CurCycle != CYCLE::CycleNotExist) && (m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning ==OFF)) ||
				((m_pOHTMainStatus->CurCycle != CYCLE::CycleNotExist) && (GetExecuteStatus() == STS_AUTORECOVERY_OVERRUN) && (m_pPassPermit->GetPassPermitCount() < 2))) &&
				  (m_pPassPermit->GetRollBackFinish() == true))
			{
				ChangeStatus(GetExecuteStatus());
				ADD_LOG_TASKCONTROL("[Clear][Error:%d]", OHTError_Current);
				InitClearCycle();
				m_pCmdManager->InitTaskList();
			  //	InitExcuteOverRun();
				ChangeCmdIDAndCycle(0,0,0,true);
//				m_pCmdManager->InitCmdList();
			}

			ChangeStatus(GetExecuteStatus());


		}
		else
		{
			OHTError_Current = NO_ERR;

			// 에러 없는 경우 Status는 ExcuteInfo로 변경

			ChangeStatus(GetExecuteStatus());
			ChangeCmdID(GetExecutePreCmdID(), GetExecuteCurCmdID());
			bErrorCIDOccupFlag = false;
		}

		m_pOHTMainStatus->InnerErrCode  = OHTError_Current;


		if((GetExecuteStatus() == STS_ERROR)  && (GetCurrentMode() == MODE_MANUAL))
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(dCheckAutoToErrorTime) > 2000)
			{
				//메뉴얼 에러에 이동할 경우 정지명령 추가. 속도 0.07m/s이상일 경우
				if(fabs(m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningSpeed) > 70.0)
				{
					m_pCmdManager->SendQuickCmd2Main(EXECUTE_QUICK_CMD_STOP);
				}
				dCheckAutoToErrorTime = timeGetTime();
			}
		}

		//Error 발생시 CID 동작 추가
		if((OHTError_Current == ERR_AXIS1_F_AMP_FAULT)
		 ||(OHTError_Current == ERR_AXIS1_R_AMP_FAULT)
		 ||(OHTError_Current == ERR_AXIS2_AMP_FAULT)
		 ||(OHTError_Current == ERR_AXIS3_AMP_FAULT)
		 ||(OHTError_Current == ERR_AXIS1_F_ABORT_EVENT)
		 ||(OHTError_Current == ERR_AXIS1_R_ABORT_EVENT)
		 ||(OHTError_Current == ERR_AXIS2_ABORT_EVENT)
		 ||(OHTError_Current == ERR_AXIS3_ABORT_EVENT)
//		 ||(OHTError_Current == ERR_TP_ESTOP)//test 용도
//		 ||(OHTError_Current == NO_ERR)//test 용도
		 )
		{
			
			//1 조향 방향 찾기
			//SteerFront = pHWSet->m_pSteer->GetFrontPos();
			if(m_pHWCommon->m_Input.STEER_F_Left==ON)
			{
			   SteerFront = HW_STEER_DIRECTION_LEFT;
			}
			else if(m_pHWCommon->m_Input.STEER_F_Right==ON)
			{
			   SteerFront = HW_STEER_DIRECTION_RIGHT;
			}
			else
			{}

			//2 CID 모드 선택(CID모드 및 매뉴얼 설정)
			//pHWSet->m_pCID->ElevetorOperatingModeSet(ON);//CID Mode
			m_pHWCommon->SetBit(OUT_CID_Operation_Mode, ON);
			//ADD_LOG_TASKCONTROL("[CID] CID_Mode_ON");
			if (m_pHWCommon->m_Output.CID_Abnormal == OFF)
			{
				m_pHWCommon->SetBit(OUT_CID_Abnormal, ON);
				//ADD_LOG_TASKCONTROL("[CID] Abnormal_ON");
			}

			//3 Select ON
			if(SteerFront == HW_STEER_DIRECTION_RIGHT)
			{	
				//pHWSet->m_pCID->CID_Out_Right_Select(ON);
				m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, ON);
//				ADD_LOG_TASKCONTROL("[CID] 1 OUT_CID_Right_Select_ON");
			}
			else if(SteerFront == HW_STEER_DIRECTION_LEFT)
			{
				//pHWSet->m_pCID->CID_Out_Left_Select(ON);
				m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, ON);
//				ADD_LOG_TASKCONTROL("[CID] 1 OUT_CID_Left_Select_ON");
			}
			else
			{}
			
			//4 Link 확인 후 점유
			if(m_pHWCommon->m_Input.CID_Link_Compt_IN == ON)
			{
				m_pHWCommon->SetBit(OUT_CID_Occupancy, ON);
								ADD_LOG_TASKCONTROL("[CID] 1 OUT_CID_Occupancy");
			}
		}
		else if(OHTError_Current != NO_ERR)
		{
			unsigned int CIDStopNode1_arr = OHTMainForm->GetMap()->FindNodeIndexFromCIDStop_1(m_pExecuteInfo->ExecutePositionInfo.CurrNode);
			unsigned int CIDStopNode2_arr = OHTMainForm->GetMap()->FindNodeIndexFromCIDStop_2(m_pExecuteInfo->ExecutePositionInfo.CurrNode);
//			ADD_LOG_TASKCONTROL("[CID] 2node: %d, flag: %d, offset: %d", m_pExecuteInfo->ExecutePositionInfo.CurrNode, bErrorCIDOccupFlag, m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset);
			if(m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset >= 30 && bErrorCIDOccupFlag == true &&
				(CIDStopNode1_arr != INDEX_NONE || CIDStopNode2_arr != INDEX_NONE))
			{
				ADD_LOG_TASKCONTROL("[CID] 3node: %d, flag: %d, offset: %d", m_pExecuteInfo->ExecutePositionInfo.CurrNode, bErrorCIDOccupFlag, m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset);
				//1 v�� ���� ã��
				//SteerFront = pHWSet->m_pSteer->GetFrontPos();
				if(m_pHWCommon->m_Input.STEER_F_Left==ON)
				{
				   SteerFront = HW_STEER_DIRECTION_LEFT;
				}
				else if(m_pHWCommon->m_Input.STEER_F_Right==ON)
				{
				   SteerFront = HW_STEER_DIRECTION_RIGHT;
				}
				else
				{}

				//2 CID ��� ����(CID��� �� �Ŵ��� ��d)
				//pHWSet->m_pCID->ElevetorOperatingModeSet(ON);//CID Mode
				m_pHWCommon->SetBit(OUT_CID_Operation_Mode, ON);
				//ADD_LOG_TASKCONTROL("[CID] CID_Mode_ON");
				if (m_pHWCommon->m_Output.CID_Abnormal == OFF)
				{
					m_pHWCommon->SetBit(OUT_CID_Abnormal, ON);
					//ADD_LOG_TASKCONTROL("[CID] Abnormal_ON");
				}

				//3 Select ON
				if(SteerFront == HW_STEER_DIRECTION_RIGHT)
				{	
					//pHWSet->m_pCID->CID_Out_Right_Select(ON);
					m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, ON);
//					ADD_LOG_TASKCONTROL("[CID] 2 OUT_CID_Right_Select_ON");
				}
				else if(SteerFront == HW_STEER_DIRECTION_LEFT)
				{
					//pHWSet->m_pCID->CID_Out_Left_Select(ON);
					m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, ON);
//					ADD_LOG_TASKCONTROL("[CID] 2 OUT_CID_Left_Select_ON");
				}
				else
				{}
				
				//4 Link Ȯ�� �� a/
				if(m_pHWCommon->m_Input.CID_Link_Compt_IN == ON)
				{
					m_pHWCommon->SetBit(OUT_CID_Occupancy, ON);
					ADD_LOG_TASKCONTROL("[CID] 2 OUT_CID_Occupancy");
					bErrorCIDOccupFlag = false;
				}
			}
		}

		break;

		// -------------------------------------------
		// 1 Step : MAIN_TASK_SHUTDOWN
	case MAIN_TASK_SHUTDOWN:
		break;

		// -------------------------------------------
		// 1 Step : MAIN_TASK_ERROR
	case MAIN_TASK_ERROR:
		// #ifdef USE_LOG_MEMO
		// ADD_MEMO_T("메인 프로그램 에러 발생....\r\n");
		// #endif /* USE_LOG_MEMO */
		break;

	default:
		break;
	}

	//TP에서 Auto 변환중인지 확인하기 위해
	m_bAutoChaning = m_StepDetectAreaCheck.Step != 0 || m_pServoControl->Fn009ing();

	DWORD dwCurrTime = timeGetTime();
	if((m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status == STS_LOADING) || (m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status == STS_UNLOADING))
	{
		if(m_mccparam.bHandSequenceFlag == true)
		{
			//Hand Close Output On/Off Time
			if((m_mccparam.bpreHandCloseOut == 0) && (m_pHWSet->m_pHand->IsCloseOutputOn() == 1))
			{
				m_mccparam.dwHandCloseOutOnTime = dwCurrTime;
			}
			if((m_mccparam.bpreHandCloseOut == 1) && (m_pHWSet->m_pHand->IsCloseOutputOn() == 0))
			{
				m_mccparam.dwHandCloseOutOffTime = dwCurrTime;
			}
			//Hand Open Output On/Off Time
			if((m_mccparam.bpreHandOpenOut == 0) && (m_pHWSet->m_pHand->IsOpenOutputOn() == 1))
			{
				m_mccparam.dwHandOpenOutOnTime = dwCurrTime;
			}
			if((m_mccparam.bpreHandOpenOut == 1) && (m_pHWSet->m_pHand->IsOpenOutputOn() == 0))
			{
				m_mccparam.dwHandOpenOutOffTime = dwCurrTime;
			}
			//Hand Close Input On/Off Time
			if((m_mccparam.bpreHandCloseIn == 0) && (m_pHWSet->m_pHand->IsClose() == 1))
			{
				m_mccparam.dwHandCloseInOnTime = dwCurrTime;
			}
			if((m_mccparam.bpreHandCloseIn == 1) && (m_pHWSet->m_pHand->IsClose() == 0))
			{
				m_mccparam.dwHandCloseInOffTime = dwCurrTime;
			}
			//Hand Open Input On/Off Time
			if((m_mccparam.bpreHandOpenIn == 0) && (m_pHWSet->m_pHand->IsOpen() == 1))
			{
				m_mccparam.dwHandOpenInOnTime = dwCurrTime;
			}
			if((m_mccparam.bpreHandOpenIn == 1) && (m_pHWSet->m_pHand->IsOpen() == 0))
			{
				m_mccparam.dwHandOpenInOffTime = dwCurrTime;
			}
			//Hand Inposition Input On/Off Time
			if((m_mccparam.bpreHandInposition == 0) && (m_pHWSet->m_pHand->IsInposition() == 1))
			{
				m_mccparam.dwHandInpoInOnTime = dwCurrTime;
			}
			if((m_mccparam.bpreHandInposition == 1) && (m_pHWSet->m_pHand->IsInposition() == 0))
			{
				m_mccparam.dwHandInpoInOffTime = dwCurrTime;
			}
			//Hand Left Gripper Input On/Off Time
			if((m_mccparam.bpreHandLeftGrip == 0) && (m_pHWSet->m_pHand->IsLeftGripOn() == 1))
			{
				m_mccparam.dwHandLeftGripInOnTime = dwCurrTime;
			}
			if((m_mccparam.bpreHandLeftGrip == 1) && (m_pHWSet->m_pHand->IsLeftGripOn() == 0))
			{
				m_mccparam.dwHandLeftGripInOffTime = dwCurrTime;
			}
			//Hand Right Gripper Input On/Off Time
			if((m_mccparam.bpreHandRightGrip == 0) && (m_pHWSet->m_pHand->IsRightGripOn() == 1))
			{
				m_mccparam.dwHandRightGripInOnTime = dwCurrTime;
			}
			if((m_mccparam.bpreHandRightGrip == 1) && (m_pHWSet->m_pHand->IsRightGripOn() == 0))
			{
				m_mccparam.dwHandRightGripInOffTime = dwCurrTime;
			}
			//Hand Push Input On/Off Time
			if((m_mccparam.bpreHandPush == 0) && (m_pHWSet->m_pHand->IsPushOn() == 1) && (m_mccparam.dwHandPushInOnTime == 0))
			{
				m_mccparam.dwHandPushInOnTime = dwCurrTime;
			}
			if((m_mccparam.bpreHandPush == 0) && (m_pHWSet->m_pHand->IsPushOn() == 1))
			{
				m_mccparam.nHandPushCount++;
			}
			if((m_mccparam.bpreHandPush == 1) && (m_pHWSet->m_pHand->IsPushOn() == 0))
			{
				m_mccparam.dwHandPushInOffTime = dwCurrTime;
			}
		}

		m_mccparam.bpreHandCloseOut = m_pHWSet->m_pHand->IsCloseOutputOn();
		m_mccparam.bpreHandOpenOut = m_pHWSet->m_pHand->IsOpenOutputOn();
		m_mccparam.bpreHandCloseIn = m_pHWSet->m_pHand->IsClose();
		m_mccparam.bpreHandOpenIn	= m_pHWSet->m_pHand->IsOpen();
		m_mccparam.bpreHandInposition = m_pHWSet->m_pHand->IsInposition();
		m_mccparam.bpreHandLeftGrip = m_pHWSet->m_pHand->IsLeftGripOn();
		m_mccparam.bpreHandRightGrip = m_pHWSet->m_pHand->IsRightGripOn();
		m_mccparam.bpreHandPush = m_pHWSet->m_pHand->IsPushOn();

		m_mccparam.bHandSequenceFlag = true;
		if((m_mccparam.bTransMessageFlag == false) && (m_mccparam.bpreHandPush == 1))
		{
			m_mccparam.bTransMessageFlag = true;
			if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status == STS_LOADING)
			{
				m_mccparam.bTransLoaded = true;
				ADD_MCC_LOG(",%s,TRANS,HAND SEQUENCE OPEN,Start,FOUP,%06d,d,%06d,%d",
							m_defualtparam->VHLName,
							m_pOHTMainStatus->StatusCommon.CurStationID,
							m_mccparam.nEqType,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset);
			}
			else
			{
				m_mccparam.bTransLoaded = false;
				ADD_MCC_LOG(",%s,TRANS,HAND SEQUENCE CLOSE,Start,FOUP,%06d,d,%06d,%d",
							m_defualtparam->VHLName,
							m_pOHTMainStatus->StatusCommon.CurStationID,
							m_mccparam.nEqType,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset);
			}
		}
	}
	else
	{
		if(m_mccparam.bTransMessageFlag == true)
		{
			if(m_mccparam.bTransLoaded == true)
			{
				ADD_MCC_LOG(",%s,TRANS,HAND SEQUENCE OPEN,End,FOUP,%06d,%d,%06d,%d,Push On Time,%d,Hand Open Out On Time,%d,Hand Inpo Off Time,%d,Hand Close In Off Time,%d,Left Grip In Off Time,%d,Right Grip In Off Time,%d,Hand Inpo On Time,%d,Hand Open In On Time,%d,Hand Open Out Off Time,%d,Push Off Time,%d,Hand Push Count,%d",
						m_defualtparam->VHLName,
						m_pOHTMainStatus->StatusCommon.CurStationID,
						m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dwHandPushInOnTime,
						m_mccparam.dwHandOpenOutOnTime,
						m_mccparam.dwHandInpoInOffTime,
						m_mccparam.dwHandCloseInOffTime,
						m_mccparam.dwHandLeftGripInOffTime,
						m_mccparam.dwHandRightGripInOffTime,
						m_mccparam.dwHandInpoInOnTime,
						m_mccparam.dwHandOpenInOnTime,
						m_mccparam.dwHandOpenOutOffTime,
						m_mccparam.dwHandPushInOffTime,
						m_mccparam.nHandPushCount);
			}
			else
			{
				ADD_MCC_LOG(",%s,TRANS,HAND SEQUENCE CLOSE,End,FOUP,%06d,%d,%06d,%d,Push On Time,%d,Hand Close Out On Time,%d,Hand Inpo Off Time,%d,Hand Open In Off Time,%d,Left Grip In On Time,%d,Right Grip In On Time,%d,Hand Inpo On Time,%d,Hand Close In On Time,%d,Hand Close Out Off Time,%d,Push Off Time,%d,Hand Push Count,%d",
						m_defualtparam->VHLName,
						m_pOHTMainStatus->StatusCommon.CurStationID,
						m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dwHandPushInOnTime,
						m_mccparam.dwHandCloseOutOnTime,
						m_mccparam.dwHandInpoInOffTime,
						m_mccparam.dwHandOpenInOffTime,
						m_mccparam.dwHandLeftGripInOnTime,
						m_mccparam.dwHandRightGripInOnTime,
						m_mccparam.dwHandInpoInOnTime,
						m_mccparam.dwHandCloseInOnTime,
						m_mccparam.dwHandCloseOutOffTime,
						m_mccparam.dwHandPushInOffTime,
						m_mccparam.nHandPushCount);
			}
			m_mccparam.dwHandOpenOutOnTime = 0;
			m_mccparam.dwHandOpenOutOffTime = 0;
			m_mccparam.dwHandCloseOutOnTime = 0;
			m_mccparam.dwHandCloseOutOffTime = 0;
			m_mccparam.dwHandInpoInOnTime = 0;
			m_mccparam.dwHandInpoInOffTime = 0;
			m_mccparam.dwHandOpenInOnTime = 0;
			m_mccparam.dwHandOpenInOffTime = 0;
			m_mccparam.dwHandCloseInOnTime = 0;
			m_mccparam.dwHandCloseInOffTime = 0;
			m_mccparam.dwHandLeftGripInOnTime = 0;
			m_mccparam.dwHandLeftGripInOffTime = 0;
			m_mccparam.dwHandRightGripInOnTime = 0;
			m_mccparam.dwHandRightGripInOffTime = 0;
			m_mccparam.dwHandPushInOnTime = 0;
			m_mccparam.dwHandPushInOffTime = 0;
			m_mccparam.nHandPushCount = 0;
		}

		m_mccparam.bHandSequenceFlag = false;
		m_mccparam.bTransMessageFlag = false;
	}
}

//********************************************************************
// SHCHO 2018.07.19
// 시퀀스 스탭 관련 실행 함수
//
void TaskControl::ExecuteSteps()
{

	char szLogMsg[1024];
	ZeroMemory(szLogMsg, sizeof(szLogMsg));

  // shcho 2018.04.24 ********************************************************************
	// 스텝 : m_StepDetectAreaCheck
	// OHT 대차 패턴 Check Step (switch~case)
	// UBG에 시리얼 통신으로 데이터 받은후 INI에서 읽은 값과 대조. 같은지 확인한다.
	// ### Step Result 정의 ###
	// nResult == 0  [OK]
	// nResult == -1 [Fail]
	// nResult == -2 [Comm Fail]
	// nResult == -3 [Different Data. 불일치]

	// Step 100 : UGB에 Send -> Recv 데이터 확인
	// Step 300 : Step 마무리 작업. CloseCom, TAGTYPE_AUTOMODE 세팅 등

	// 처음에 한번은 무조건 OHTDetectAreaInfo_UBG_RECV.ini 파일 남기기 위해서 한번 스탭 태움
	static bool bOnce = true;
	if(bOnce)
	{
		bOnce = false;

		m_StepDetectAreaCheck.Step = 100;
		m_StepDetectAreaCheck.nRetryCnt = 0;

		RapidLog(RLOG_UBG_CHK, "== OHT Program On UBG Check Start ==");

	}
	switch(m_StepDetectAreaCheck.Step)
    {
		case 0:

			//if(m_StepDetectAreaCheck.StepComment != "")	// init step comment
			//	m_StepDetectAreaCheck.StepComment = "";

			break;

		// Step 100 : UGB에 Send -> Recv 데이터 확인
        case 100:
		{
			m_StepDetectAreaCheck.StepComment = "Send [$FR11] to UBG to Get Pattern Info.";


			m_StepDetectAreaCheck.nResult = 0;		// 결과 초기화
			m_StepDetectAreaCheck.ErrorStep = 0;
			m_StepDetectAreaCheck.bAllDone = false;

			ZeroMemory(OHTMainForm->m_szUBGInfo_Comm, sizeof(OHTMainForm->m_szUBGInfo_Comm));

			char SendBuf[40] = {'$', 'F', 'R', '1', '1', 0x0D, '\0'};

			if(m_pHWSet == NULL)	// Getting HW Set Obj. fail case
			{
				sprintf(szLogMsg, "step 100 : GetHWSet() is NULL");
				RapidLog(RLOG_UBG_CHK, szLogMsg);
				m_StepDetectAreaCheck.nResult = -1;	//fail
				m_StepDetectAreaCheck.Step = 300;
				break;
			}

			// FR11 데이터 전달. 패턴 1~4번 얻기 위함
			int nTmpRet = 0;
			nTmpRet = m_pHWSet->m_pOHTDetect->Send_UBGData(SendBuf,6);


			if(nTmpRet != 0)	// send [$FR11]
			{
                sprintf(szLogMsg, "step 100 : Send error. Err= %d", nTmpRet);
				RapidLog(RLOG_UBG_CHK, szLogMsg);
                m_StepDetectAreaCheck.nResult = -1;	//fail
				m_StepDetectAreaCheck.Step = 300;
				break;
            }

			m_StepDetectAreaCheck.StepDelay.OnDelay(false);	// timer init
            m_StepDetectAreaCheck.Step++;
            break;
        }
        case 101:
		{
			m_StepDetectAreaCheck.StepComment = "Wait for UBG response of [$FR11]";

			// Non block waiting for a while... Comm 통신 recv 받는 대기시간
			if(!m_StepDetectAreaCheck.StepDelay.OnDelay(true, 300))
				break;


			// FR11 요청 값 받음. 1~4번 패턴 데이터
			char RcvBuf[1024] = {0,};
			ZeroMemory(RcvBuf, sizeof(RcvBuf));
			int nReadBytes = m_pHWSet->m_pOHTDetect->Recieve_UBGData(RcvBuf);	//recv

			if(nReadBytes < 201)	//201 Bytes 받아야 함
			{
				RapidLog(RLOG_UBG_CHK, "step 101: Short Bytes");
				m_StepDetectAreaCheck.nResult = -2;	//comm fail
				m_StepDetectAreaCheck.Step = 300;
				break;
			}

			// 비교하지 않고 데이터만 일단 저장하고 있는다
			OHTMainForm->AddUBGPattern(1, RcvBuf);

            m_StepDetectAreaCheck.Step++;
            break;
		}
		case 102:
		{
			m_StepDetectAreaCheck.StepComment = "Send [$FR12] to UBG to Get Pattern Info.";

			char SendBuf[] = {'$', 'F', 'R', '1', '2', 0x0D, '\0',};


			// FR12 데이터 전달. 패턴 5~8번 얻기 위함
			m_pHWSet->m_pOHTDetect->Send_UBGData(SendBuf,6);

			m_StepDetectAreaCheck.StepDelay.OnDelay(false);	// timer init
			m_StepDetectAreaCheck.Step++;
			break;
		}
        case 103:
		{
			m_StepDetectAreaCheck.StepComment = "Wait for UBG response of [$FR12]";

			// Non block waiting for a while... Comm 통신 recv 받는 대기시간
			if(!m_StepDetectAreaCheck.StepDelay.OnDelay(true, 300))
				break;


			// FR12 요청 값 받음. 5~8번 패턴 데이터
			char RcvBuf[1024] = {0,};
			ZeroMemory(RcvBuf, sizeof(RcvBuf));
			int nReadBytes = m_pHWSet->m_pOHTDetect->Recieve_UBGData(RcvBuf);	//recv


			if(nReadBytes < 201)	//201 Bytes 받아야 함
			{
				RapidLog(RLOG_UBG_CHK, "step 103 : Short Bytes");
				m_StepDetectAreaCheck.nResult = -2;	//comm fail
				m_StepDetectAreaCheck.Step = 300;
				break;
			}

			// 비교하지 않고 데이터만 일단 저장하고 있는다
			OHTMainForm->AddUBGPattern(2, RcvBuf);

            m_StepDetectAreaCheck.StepDelay.OnDelay(false);	// timer init
			m_StepDetectAreaCheck.Step++;
			break;
		}
		case 104:
		{
			m_StepDetectAreaCheck.StepComment = "Send [$FR13] to UBG to Get Pattern Info.";

			char SendBuf[] = {'$', 'F', 'R', '1', '3', 0x0D, '\0',};

			// FR12 데이터 전달. 패턴 9~12번 얻기 위함
			m_pHWSet->m_pOHTDetect->Send_UBGData(SendBuf,6);

			m_StepDetectAreaCheck.StepDelay.OnDelay(false);	// timer init
			m_StepDetectAreaCheck.Step++;
			break;
		}
		case 105:
		{
			m_StepDetectAreaCheck.StepComment = "Wait for UBG response of [$FR13]";

			// Non block waiting for a while... Comm 통신 recv 받는 대기시간
			if(!m_StepDetectAreaCheck.StepDelay.OnDelay(true, 300))
				break;


			// FR13 요청 값 받음. 9~12번 패턴 데이터
			char RcvBuf[1024] = {0,};
			ZeroMemory(RcvBuf, sizeof(RcvBuf));
			int nReadBytes = m_pHWSet->m_pOHTDetect->Recieve_UBGData(RcvBuf);	//recv

			if(nReadBytes < 201)	//201 Bytes 받아야 함
			{
				RapidLog(RLOG_UBG_CHK, "step 105 : Short Bytes");
				m_StepDetectAreaCheck.nResult = -2;	//comm fail
				m_StepDetectAreaCheck.Step = 300;
				break;
			}

			// 비교하지 않고 데이터만 일단 저장하고 있는다
			OHTMainForm->AddUBGPattern(3, RcvBuf);

			m_StepDetectAreaCheck.StepDelay.OnDelay(false);	// timer init
			m_StepDetectAreaCheck.Step++;
			break;
		}
		 case 106:
		{
			m_StepDetectAreaCheck.StepComment = "Send [$FR14] to UBG to Get Pattern Info.";

			char SendBuf[] = {'$', 'F', 'R', '1', '4', 0x0D, '\0',};

			// FR12 데이터 전달. 패턴 13~16번 얻기 위함
			m_pHWSet->m_pOHTDetect->Send_UBGData(SendBuf,6);

			m_StepDetectAreaCheck.StepDelay.OnDelay(false);	// timer init
			m_StepDetectAreaCheck.Step++;
			break;
		}
		case 107:
		{
			m_StepDetectAreaCheck.StepComment = "Wait for UBG response of [$FR14]";

			// Non block waiting for a while... Comm 통신 recv 받는 대기시간
			if(!m_StepDetectAreaCheck.StepDelay.OnDelay(true, 300))
				break;

			// FR14 요청 값 받음. 13~16번 패턴 데이터
			char RcvBuf[1024] = {0,};
			ZeroMemory(RcvBuf, sizeof(RcvBuf));
			int nReadBytes = m_pHWSet->m_pOHTDetect->Recieve_UBGData(RcvBuf);	//recv

			if(nReadBytes < 201)	//201 Bytes 받아야 함
			{
				RapidLog(RLOG_UBG_CHK, "step 107 : Short Bytes");
				m_StepDetectAreaCheck.nResult = -2;	//comm fail
				m_StepDetectAreaCheck.Step = 300;
				break;
			}

			// 비교하지 않고 데이터만 일단 저장하고 있는다
			OHTMainForm->AddUBGPattern(4, RcvBuf);

			m_StepDetectAreaCheck.StepDelay.OnDelay(false);	// timer init
			m_StepDetectAreaCheck.Step++;
			break;
		}
		case 108:
		{
			// 비교하는 스탭 ******

			// 비교할 패턴 번호가 없을 경우 에러
			int nSize = OHTMainForm->m_nArrCheckPattern.size();
			if(nSize == 0)
			{
				RapidLog(RLOG_UBG_CHK, "step 108 : No Pattern to compare");
				m_StepDetectAreaCheck.nResult = -1;
				m_StepDetectAreaCheck.Step = 300;
				break;

			}

			int nCheckNull= 0;
			bool bCheckNull =false;
			// 비교하기. 만약 다른게 있다면 다른 패턴 번호를 기억해놓는다.
			std::vector<int> nArrDiffIdx;
			for(int i=0; i<nSize ; i++)
			{
				int nPtnIdx = OHTMainForm->m_nArrCheckPattern[i];
				bool bSame = OHTMainForm->DoCompUBGPattern(nPtnIdx);
				if(bSame == false)
				{
					nArrDiffIdx.push_back(nPtnIdx);

				}

				if((i < 384) && (nPtnIdx ==0x03))
				{
					nCheckNull++;
				}

				if(nCheckNull >180)
				{
					bCheckNull = true;
				}


			}
			// 다른 패턴이 한개라도 있으면 에러처리
			int nDiffCnt = nArrDiffIdx.size();
			if(nDiffCnt > 0)
			{
				// 다른 패턴들 번호 일일히 로깅해줌
				std::string strPatternNo = "[CompDiff] ";
				for(int i=0; i<nDiffCnt ; i++)
				{
					char szTmp[10];
					sprintf(szTmp, "%d%s", nArrDiffIdx[i]+1, (i == nDiffCnt-1 ? " " : ","));
					strPatternNo += szTmp;
				}

				RapidLog(RLOG_UBG_CHK, (char*)strPatternNo.c_str());
				m_StepDetectAreaCheck.nResult = -3;  // difference
				m_StepDetectAreaCheck.Step = 300;
				break;

			}else if(bCheckNull == true)
			{
				RapidLog(RLOG_UBG_CHK, "UBG Data Null Check");
				m_StepDetectAreaCheck.nResult = -3;  // difference
				m_StepDetectAreaCheck.Step = 300;
				break;
            }

			if(nDiffCnt == 0)
			{
				RapidLog(RLOG_UBG_CHK, "UBG Compare Success!");
            }

			m_StepDetectAreaCheck.nResult = 0;	// SUCCESS

			m_StepDetectAreaCheck.Step = 300;
			break;
		}
		// Step 300 : Step 마무리 작업. CloseCom, TAGTYPE_AUTOMODE 세팅 등
		case 300:
		{
			m_StepDetectAreaCheck.StepComment = "m_StepDetectAreaCheck step finished.";
//			HWSet *pHWSet = OHTMainForm->GetHWSet();
//			if(pHWSet)
//				pHWSet->m_pOHTDetect->CloseCom();


			// 가끔 시리얼 잘 못읽는 경우 있기 떄문에 다시시도 3번
			if(m_StepDetectAreaCheck.nRetryCnt < 3 && m_StepDetectAreaCheck.nResult < 0)
			{
				m_StepDetectAreaCheck.nRetryCnt++;
				m_StepDetectAreaCheck.Step = 100;

				sprintf(szLogMsg, "[Retry:%d] Result=%d", m_StepDetectAreaCheck.nRetryCnt, m_StepDetectAreaCheck.nResult);

				RapidLog(RLOG_UBG_CHK, szLogMsg);
				break;
			}
			m_StepDetectAreaCheck.nRetryCnt = 0;

			OHTMainForm->WriteUBGRecv_INI(); 	//UBG에서 받은 데이터로 INI 저장. 패턴 바꼈을 경우 INI 세팅을 빠르게 하기 위함

			// 처음에 프로그램 켤때 한번 실행되는거는 Auto 모드 전환 목적이 아니기 때문에
			// 전환하지 않고 끝낸다
			static bool bOnceStart = true;
			if(bOnceStart)
			{
				bOnceStart = false;

#if(OHT_NAME != OHT_NAME_STD_V81S)
				if(m_StepDetectAreaCheck.nResult < 0)
					ErrorCode_Main = ERR_UBG_NO_DATA; // error 전달
#endif

				RapidLog(RLOG_UBG_CHK, "=== OHT Program On UBG Check Finished ===");
				m_StepDetectAreaCheck.bAllDone = false;//true;
				m_StepDetectAreaCheck.Step = 0;
				break;
			}

			// Auto Mode 전환 명령 다시 줌.
			PKT_TASKCMDDATA 	pTempCmdPktData;
			pTempCmdPktData.DetailTagSort = TAGTYPE_AUTOMODE;
			pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
			m_pCmdManager->MakeTaskList_Standard(&pTempCmdPktData);

			m_StepDetectAreaCheck.bAllDone = true;
			m_StepDetectAreaCheck.Step = 0;
			break;
		}

	}
}
// ---------------------------------------------------------------------------
/**
 @class   TaskControl 소멸자
 @date    2013.05.24
 @author  zzang9un
 @brief   TaskControl Class
 */
TaskControl::~TaskControl()
{
	if(m_defualtparam->IsIOTHub ==true)
	{
		ServerSocketClose();
		delete m_IotManager;
    }

	delete gm_Ping;
	delete m_NetWorkDll;
	delete m_IOMapField;


}
// ---------------------------------------------------------------------------
 /**
 @brief   Main Task State를 변경하는 함수

 기존 State는 Pre변수에 저장하고, Cur State를 변경한다.
 @author  zzang9un
 @date    2012.11.16
 @param   MainState : 변경할 State
 */
void TaskControl::ChangeMainTaskState(MAIN_TASK_STATE MainState) {
	// 현재 State를 저장한 후
	TaskState_Pre = TaskState_Cur;

	// 현재 State를 변경한다.
	TaskState_Cur = MainState;
}
// ---------------------------------------------------------------------------
/**
 @brief   Map File에서 Map Data를 Load하는 함수
 @author  zzang9un
 @date    2012.11.13
 @return  Map File 성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */
int TaskControl::LoadMapData()
{
	int Error = NO_ERR;

	if (m_bLoadedMap) {
		// Map이 이미 Load된 경우 바로 성공한 것으로 return
		return NO_ERR;
	}
	else {
		String strOHTPath, strMapFileName;

		// Map 파일 이름 설정
		strOHTPath = ExtractFileDir(Application->ExeName);
		strMapFileName = String(strOHTPath + STR_DATAFILE_PATH) +
			STR_FILENAME_MAP;

		// 2013.08.07 doori.shin Set Flag
		Error = OHTMainForm->GetMapDataManager()->LoadMapFile
			(strMapFileName.c_str(), OHTMainForm->GetMap());

		if (Error != NO_ERR)
			m_bLoadedMap = false;
		else {
			// Map 파일 Load 플래그 true로 셋팅
			m_bLoadedMap = true;
			m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.nMapVer =
				OHTMainForm->GetMapDataManager()->GetMapVersion();
		}
	}

	return Error;
}
// ---------------------------------------------------------------------------
///**
// @brief   Station File에서 Data를 Load하는 함수
// @author  doori.shin
// @date    2013.07.09
// @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
// @note    성공인 경우 Load 플래그를 true로 셋팅한다.
// */
//int TaskControl::LoadStationData() {
//	int Error = NO_ERR;
//
//	String strOHTPath = "", strPath = "";
//
//	strOHTPath = ExtractFileDir(Application->ExeName);
//	strPath = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_STATION;
//
//	Error = OHTMainForm->GetStationDataManager()->LoadDataFromFile(strPath);
//
//	if (Error)
//		m_bLoadedStationData = false;
//	else {
//		// Station파일  Load 플래그 true로 셋팅
//		m_bLoadedStationData = true;
//		m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.nStationVer =
//			OHTMainForm->GetStationDataManager()->m_FileHeader.Revision;
//	}
//
//	return Error;
//}


// ---------------------------------------------------------------------------
/**
 @brief   7.0버전의  LoadMapData 사용하는 자료구조 생성 함수
          MAP_MAPDATA.dat, MAP_FLOWCONTROL.dat, MAP_COLLISTION.dat
 @author  KJD
 @date    2016.04.20
 @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */
int TaskControl::MakeMapData() {
	int Error = NO_ERR;
	UINT uiDataTmp = OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.DataCount;
	UINT uiCollisionTmp = OHTMainForm->GetCollisionDataManager_STD()->m_CollisionFileHeader.DataCount;

	bool  bAdjustMap = ((OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.DataType & bit_0)==bit_0)? true:false;
	if (m_bLoadedMap) {
		// Map이 이미 Load된 경우 바로 성공한 것으로 return
		return NO_ERR;
	}
	else {		
		ADD_LOG_TASKCONTROL("[MakeMapData] uiDataTmp: %d, uiCollisionTmp: %d", uiDataTmp, uiCollisionTmp);

		if( (uiDataTmp <= NODE_COUNT_MAX) && (uiDataTmp>0) && (uiDataTmp == uiCollisionTmp))
		{

			Error = OHTMainForm->GetMapDataManager()->LoadDataFromSTDMap(
					OHTMainForm->GetMap(),
					OHTMainForm->GetMap_STD()->m_NodeArray,
					OHTMainForm->GetCollision_STD()->m_NodeArray,
					OHTMainForm->GetFlowControl_STD()->m_NodeArray,
					OHTMainForm->GetCID_STD()->m_NodeArray,
					m_pMapMake_Process,
					OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.DataCount,
					OHTMainForm->GetFlowControlDataManager_STD()->m_FlowControlFileHeader.DataCount,
					OHTMainForm->GetCIDDataManager_STD()->m_CIDFileHeader.DataCount,bAdjustMap);
		}
		else
		{
			//Data 없음.
			Error = 1;
		}
    }

	if (Error)
	{
		m_bLoadedMap = false;
    }
	else
	{
		// Station파일  Load 플래그 true로 셋팅
		m_bLoadedMap = true;
	}
	return Error;
}
// ---------------------------------------------------------------------------
/**
 @brief   7.0버전의 LoadStationData에서 사용하는 자료구조 생성 함수
          MAP_STATION.dat
 @author  KJD
 @date    2016.04.20
 @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */

int TaskControl::MakeStationData() {
	int Error = NO_ERR;
	UINT uiDataTmp = OHTMainForm->GetStationDataManager_STD()->m_StationFileHeader.DataCount;

	if( (uiDataTmp <= NODE_COUNT_MAX) && (uiDataTmp>0) )
	{
		Error = OHTMainForm->GetStationDataManager()->LoadDataFromSTDMap(
				OHTMainForm->GetMap(), OHTMainForm->GetStation_STD()->m_NodeArray, uiDataTmp);
	}
	else
	{
		//Data 없음.
        Error = 1;
	}

	if (Error)
	{
		m_bLoadedStationData = false;
    }
	else
	{
		// Station파일  Load 플래그 true로 셋팅
		m_bLoadedStationData = true;
	}

	return Error;
}
// ---------------------------------------------------------------------------
/**
 @brief   7.0버전의  LoadTeachingData에서 사용하는 자료구조 생성 함수
          MAP_TEACHING.dat, MAP_LOOKDOWN.dat
 @author  KJD
 @date    2016.04.20
 @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */

int TaskControl::MakeTeachingData() {
	int Error = NO_ERR;
	UINT uiDataTmp_Teaching = OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.DataCount;
	
	if( (uiDataTmp_Teaching <= NODE_COUNT_MAX) && (uiDataTmp_Teaching>0) )
	{
		Error = OHTMainForm->GetTeachingDataManager()->LoadDataFromSTDMap(
				OHTMainForm->GetTeaching_STD()->m_NodeArray, 
				uiDataTmp_Teaching);
	}
	else
	{
		//Data 없음.
        Error = 1;
	}

	if (Error)
	{
		m_bLoadedTeachingData = false;
    }
	else
	{
		// Teaching파일  Load 플래그 true로 셋팅
		m_bLoadedTeachingData = true;
	}

	return Error;
}

// ---------------------------------------------------------------------------
/**
 @brief   CID.dat File에서 Data를 Load하는 함수
 @author  KJD
 @date    2016.04.20
 @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */
int TaskControl::LoadCIDData_STD() 
{
	int Error = NO_ERR;

	if (m_bLoadedCID_STD) {
		// Map이 이미 Load된 경우 바로 성공한 것으로 return
		return NO_ERR;
	}
	else
	{
		String strOHTPath = "", strFileName = "";

		strOHTPath = ExtractFileDir(Application->ExeName);
		strFileName = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_CID;

		Error = OHTMainForm->GetCIDDataManager_STD()->LoadMapFile(strFileName.c_str(),OHTMainForm->GetCID_STD());

		// by zzang9un 2013.07.16 : Teaching File이 없는 경우에는 Teaching Data Map만 초기화 한다.
		if (Error) 
		{
			m_bLoadedCID_STD = false;
		}
		else
		{
			m_bLoadedCID_STD = true;
			// Todo : 파일 버전 입력
//			m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.nCIDVer =
//				OHTMainForm->GetCIDDataManager_STD()->m_FileHeader.Revision;
		}
	}

	return Error;
 }

// ---------------------------------------------------------------------------
/**
 @brief   COLLISION.dat File에서 Data를 Load하는 함수
 @author  KJD
 @date    2016.04.20
 @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */
int TaskControl::LoadCollisionData_STD() 
{
	int Error = NO_ERR;

	if (m_bLoadedCollision_STD) {
		// Map이 이미 Load된 경우 바로 성공한 것으로 return
		return NO_ERR;
	}
	else
	{
		String strOHTPath = "", strFileName = "";

		strOHTPath = ExtractFileDir(Application->ExeName);
		strFileName = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_COLLISION;

		Error = OHTMainForm->GetCollisionDataManager_STD()->LoadMapFile(strFileName.c_str(),OHTMainForm->GetCollision_STD());

		// by zzang9un 2013.07.16 : Teaching File이 없는 경우에는 Teaching Data Map만 초기화 한다.
		if (Error) 
		{
			m_bLoadedCollision_STD = false;
		}
		else 
		{
			m_bLoadedCollision_STD = true;
			// Todo : 파일 버전 입력
//			m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.nCollisionVer =
//				OHTMainForm->GetCIDDataManager_STD()->m_FileHeader.Revision;
		}
	}

	return Error;
 }

// ---------------------------------------------------------------------------
/**
 @brief   FLOWCONTROL.dat File에서 Data를 Load하는 함수
 @author  KJD
 @date    2016.04.20
 @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */
int TaskControl::LoadFlowControlData_STD() 
{
	int Error = NO_ERR;

	if (m_bLoadedFlowControl_STD) {
		// Map이 이미 Load된 경우 바로 성공한 것으로 return
		return NO_ERR;
	}
	else
	{
		String strOHTPath = "", strFileName = "";

		strOHTPath = ExtractFileDir(Application->ExeName);
		strFileName = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_FLOWCONTROL;

		Error = OHTMainForm->GetFlowControlDataManager_STD()->LoadMapFile(strFileName.c_str(),OHTMainForm->GetFlowControl_STD());

		// by zzang9un 2013.07.16 : Teaching File이 없는 경우에는 Teaching Data Map만 초기화 한다.
		if (Error) 
		{
			m_bLoadedFlowControl_STD = false;
		}
		else 
		{
			m_bLoadedFlowControl_STD = true;
			// Todo : 파일 버전 입력
//			m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.nFlowControlVer =
//				OHTMainForm->GetCIDDataManager_STD()->m_FileHeader.Revision;
		}
	}

	return Error;
 }


// ---------------------------------------------------------------------------
/**
 @brief   MAPDATA.dat File에서 Data를 Load하는 함수
 @author  KJD
 @date    2016.04.20
 @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */
int TaskControl::LoadMapData_STD() 
{
	int Error = NO_ERR;

	if (m_bLoadedMap_STD) {
		// Map이 이미 Load된 경우 바로 성공한 것으로 return
		return NO_ERR;
	}
	else
	{
		String strOHTPath = "", strFileName = "";

		strOHTPath = ExtractFileDir(Application->ExeName);
		strFileName = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_MAP;

		Error = OHTMainForm->GetMapDataManager_STD()->LoadMapFile(strFileName.c_str(),OHTMainForm->GetMap_STD());

		if (Error) 
		{
			m_bLoadedMap_STD = false;
		}
		else 
		{
			m_bLoadedMap_STD = true;
			// Todo : 파일 버전 입력
//			m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.nMapVer =
//				OHTMainForm->GetCIDDataManager_STD()->m_FileHeader.Revision;
		}
	}

	return Error;
 }

// ---------------------------------------------------------------------------
/**
 @brief   STATION.dat File에서 Data를 Load하는 함수
 @author  KJD
 @date    2016.04.20
 @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */
int TaskControl::LoadStationData_STD() 
{
	int Error = NO_ERR;

	if (m_bLoadedStation_STD) {
		// Map이 이미 Load된 경우 바로 성공한 것으로 return
		return NO_ERR;
	}
	else
	{
		String strOHTPath = "", strFileName = "";

		strOHTPath = ExtractFileDir(Application->ExeName);
		strFileName = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_STATION;

		Error = OHTMainForm->GetStationDataManager_STD()->LoadMapFile(strFileName.c_str(),OHTMainForm->GetStation_STD());

		if (Error) 
		{
			m_bLoadedStation_STD = false;
		}
		else 
		{
			m_bLoadedStation_STD = true;
			// Todo : 파일 버전 입력
//			m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.nStationVer =
//				OHTMainForm->GetCIDDataManager_STD()->m_FileHeader.Revision;
		}
	}

	return Error;
 }

// ---------------------------------------------------------------------------
/**
 @brief   TEACHING.dat File에서 Data를 Load하는 함수
 @author  KJD
 @date    2016.04.20
 @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */
int TaskControl::LoadTeachingData_STD() 
{
	int Error = NO_ERR;

	if (m_bLoadedTeaching_STD) {
		// Map이 이미 Load된 경우 바로 성공한 것으로 return
		return NO_ERR;
	}
	else
	{
		String strOHTPath = "", strFileName = "";

		strOHTPath = ExtractFileDir(Application->ExeName);
		strFileName = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_TEACHING;

		Error = OHTMainForm->GetTeachingDataManager_STD()->LoadMapFile(strFileName.c_str(),OHTMainForm->GetTeaching_STD());

		if (Error) 
		{
			m_bLoadedTeaching_STD = false;
		}
		else 
		{
			m_bLoadedTeaching_STD = true;
			// Todo : 파일 버전 입력
//			m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.nTeachingVer =
//				OHTMainForm->GetCIDDataManager_STD()->m_FileHeader.Revision;
		}
	}

	return Error;
 }
 // ---------------------------------------------------------------------------
/**
 @brief   PATHDATA.dat File에서 Data를 Load하는 함수
 @author  KJD
 @date    2016.05.08
 @return  성공인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 @note    성공인 경우 Load 플래그를 true로 셋팅한다.
 */
int TaskControl::LoadPathData_STD()
{
	int Error = NO_ERR;

	if (m_bLoadedPathData_STD) {
		// Map이 이미 Load된 경우 바로 성공한 것으로 return
		return NO_ERR;
	}
	else
	{
		String strOHTPath = "", strFileName = "";

		strOHTPath = ExtractFileDir(Application->ExeName);
		strFileName = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_PATHDATA;

		Error = OHTMainForm->GetPathDataManager_STD()->LoadMapFile(strFileName.c_str(),OHTMainForm->GetPathData_STD());

		if (Error)
		{
			m_bLoadedPathData_STD = false;
		}
		else
		{
			m_bLoadedPathData_STD = true;
			// Todo : 파일 버전 입력
//			m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.nTeachingVer =
//				OHTMainForm->GetCIDDataManager_STD()->m_FileHeader.Revision;
		}
	}

	return Error;
 }

// ---------------------------------------------------------------------------
/**
 @brief   Teaching Data를 업데이트하는 함수
 @author  doori.shin
 @date    2013.07.16
 @param   uCmdCommon : TP로 부터 받은 데이터
 @return  정상인 경우 0을 리턴, 에러 발생 시 에러 코드 리턴
 */
//int TaskControl::UpdateTeachingData(uTP_CMD *pTPCmd)
int TaskControl::UpdateTeachingData(PKT_OHT2TP_TEACHING_INFO *pTPPacket, bool bOnlyLookDownChange)
{
	int Error = NO_ERR;

	uTP_CMD TPCmdData;
	uTP_CMD* pTPCmd;

	memset(&TPCmdData, 0x00, sizeof(uTP_CMD));
	memcpy(&(TPCmdData.TeachingSave), &(pTPPacket->BODY), sizeof(PROTOCOL_BODY_TEACHING_SAVE));
	pTPCmd = &TPCmdData;

	// 현재 각 축의 거리값과 데이터를 받아온다.
	//UINT CurStationID = m_StatusOHT2TP.Data.CurStationID;
	UINT CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;
	BYTE PortType = (BYTE)pTPCmd->TeachingSave.PortType;

	// Update할 Teaching Data를 Find한다.
	TEACHING_DATA *teachingData = OHTMainForm->GetTeachingDataManager()->Find
		(CurStationID, PortType);

    if(bOnlyLookDownChange == true)
    {
        if(teachingData == NULL)
        {
            Error = ERR_DATA_TEACHING_NOEXIST;
            ADD_LOG_TASKCONTROL("[ERR_DATA_TEACHING_NOEXIST] CurStationID : %d, PortType : %d", CurStationID, PortType);
        }
        else
        {
            // 이미존재하는 경우는 값을 업데이트한다.
            teachingData->StationID = CurStationID;
            teachingData->PortType = PortType;

            // by zzang9un 2013.08.30 : Teaching Offset 적용
            // 기존 LookDonw 외 기존 Teaching Value 사용
            teachingData->LookDownLevel = pTPCmd->TeachingSave.EtcData.Data.LookDownLevel;

            // by zzang9un 2013.07.23 : 로그 생성
            ADD_LOG_TASKCONTROL(
                "[Data][LookDown][Update] StationID:%06d, PortType:%02X, LookDownLevel :%d",
                teachingData->StationID, teachingData->PortType, teachingData->LookDownLevel);

            Error = OHTMainForm->GetTeachingDataManager()->TeachingUpdate(teachingData);

            if (Error) {
                // 에러처리
                ADD_LOG_TASKCONTROL(
                    "[Data][LookDown][Update]Fail SID:%d, PT:%d",
                    teachingData->StationID, teachingData->PortType)
            }
        }
    }
    else
    {
        if (teachingData == NULL) {
            // 찾지 못한경우, 즉 새롭게 추가해야하는경우
            teachingData = new TEACHING_DATA;
            memset(teachingData, 0, sizeof(TEACHING_DATA));

            teachingData->StationID = CurStationID;
            teachingData->PortType = PortType;

            // by zzang9un 2013.08.30 : Teaching Offset 적용
			//teachingData->Hoist = m_StatusOHT2TP.Hoist.Position +	(int)m_pExecuteInfo->TransDifference.dHoist;
            //teachingData->Shift = m_StatusOHT2TP.Shift.Position + 	(int)m_pExecuteInfo->TransDifference.dShift;
            //teachingData->Rotate = m_StatusOHT2TP.Rotate.Position;
            //OHT main에서 가진 Hoist, Shift, Rotate값에 Offset반영하여 저장
            teachingData->Hoist = m_pCmdManager->m_MainStatus->StatusTP.Hoist.Position + (int)m_pExecuteInfo->TransDifference.dHoist;
			//soming
			if(PortType == PORT_TYPE_STB_L)
			{
				  teachingData->Shift = m_pCmdManager->m_MainStatus->StatusTP.Shift.Position + (int)m_pExecuteInfo->TransDifference.dShift + teachingData->STBL_Shift;
				  ADD_LOG_TASKCONTROL("저장된 Shift 값: %d, %d",teachingData->Shift,teachingData->STBL_Shift);
			}
			else
			{
			teachingData->Shift = m_pCmdManager->m_MainStatus->StatusTP.Shift.Position + (int)m_pExecuteInfo->TransDifference.dShift;
			}
		//	teachingData->Shift = m_pCmdManager->m_MainStatus->StatusTP.Shift.Position + (int)m_pExecuteInfo->TransDifference.dShift;
            teachingData->Rotate = m_pCmdManager->m_MainStatus->StatusTP.Rotate.Position;

			teachingData->PIODirection = pTPCmd->TeachingSave.EtcData.Data.PIODirection;
			teachingData->PIOTimeLevel = pTPCmd->TeachingSave.EtcData.Data.PIOTimeLevel;
			teachingData->LookDownLevel = pTPCmd->TeachingSave.EtcData.Data.LookDownLevel;
			teachingData->TransOption.Data = (BYTE)pTPCmd->TeachingSave.EtcData.Data.TransOption;
			teachingData->PIOID = pTPCmd->TeachingSave.PIOID;
			teachingData->PIOType = pTPCmd->TeachingSave.PIOType-1;// 1 : COMBO BOX OFFSET
            teachingData->PIOChannel = pTPCmd->TeachingSave.PIOChannel;
			teachingData->IsDefaultMap.LOW_DefaultMapInfo = 1;

			// by zzang9un 2013.07.23 : 로그 생성
			ADD_LOG_TASKCONTROL(
				"[Data][Teaching][Insert] StationID:%06d, PortType:%02X, Hoist:%d(ofs:%d), Shift:%d(ofs:%d), Rotate:%d",
				teachingData->StationID, teachingData->PortType,
                teachingData->Hoist, (int)m_pExecuteInfo->TransDifference.dHoist,
                teachingData->Shift, (int)m_pExecuteInfo->TransDifference.dShift,
                teachingData->Rotate);
            ADD_LOG_TASKCONTROL(
                "[Data][Teaching][Insert] PIODirection:%02X, PIOTimeLevel:%d, LookDownLevel:%d, TransOption:%02X",
                teachingData->PIODirection, teachingData->PIOTimeLevel,
                teachingData->LookDownLevel, teachingData->TransOption.Data);

            Error = OHTMainForm->GetTeachingDataManager()->Insert(teachingData);

            if (Error) {
                // 에러처리
                ADD_LOG_TASKCONTROL(
                    "[Data][Teaching][Insert] Fail to Insert TeachingData");
            }
        }
        else {
			// 이미존재하는 경우는 값을 업데이트한다.
            teachingData->StationID = CurStationID;
            teachingData->PortType = PortType;


			teachingData->Hoist = m_pCmdManager->m_MainStatus->StatusTP.Hoist.Position +	(int)m_pExecuteInfo->TransDifference.dHoist;
			 //soming
			 if(PortType == PORT_TYPE_STB_L)
			{
				  teachingData->Shift = m_pCmdManager->m_MainStatus->StatusTP.Shift.Position + (int)m_pExecuteInfo->TransDifference.dShift + teachingData->STBL_Shift;
					ADD_LOG_TASKCONTROL("저장된 Shift 값: %d, %d",teachingData->Shift,teachingData->STBL_Shift);
			}
			else
			{
				teachingData->Shift = m_pCmdManager->m_MainStatus->StatusTP.Shift.Position + (int)m_pExecuteInfo->TransDifference.dShift;
			}
	//		teachingData->Shift = m_pCmdManager->m_MainStatus->StatusTP.Shift.Position +	(int)m_pExecuteInfo->TransDifference.dShift;
            teachingData->Rotate = m_pCmdManager->m_MainStatus->StatusTP.Rotate.Position;

			teachingData->PIODirection = pTPCmd->TeachingSave.EtcData.Data.PIODirection;
			teachingData->PIOTimeLevel = pTPCmd->TeachingSave.EtcData.Data.PIOTimeLevel;
			teachingData->LookDownLevel = pTPCmd->TeachingSave.EtcData.Data.LookDownLevel;
			teachingData->TransOption.Data = (BYTE)pTPCmd->TeachingSave.EtcData.Data.TransOption;
			teachingData->PIOID = pTPCmd->TeachingSave.PIOID;
			teachingData->PIOType = pTPCmd->TeachingSave.PIOType-1;// 1 : COMBO BOX OFFSET
			teachingData->PIOChannel = pTPCmd->TeachingSave.PIOChannel;
			teachingData->IsDefaultMap.LOW_DefaultMapInfo = 1;

            // by zzang9un 2013.07.23 : 로그 생성
			ADD_LOG_TASKCONTROL(
				"[Data][Teaching][Update] StationID:%06d, PortType:%02X, Hoist:%d(ofs:%d), Shift:%d(ofs:%d), Rotate:%d",
				teachingData->StationID, teachingData->PortType,
                teachingData->Hoist, (int)m_pExecuteInfo->TransDifference.dHoist,
                teachingData->Shift, (int)m_pExecuteInfo->TransDifference.dShift,
                teachingData->Rotate);
            ADD_LOG_TASKCONTROL(
				"[Data][Teaching][Update] PIODirection:%02X, PIOTimeLevel:%d, LookDownLevel:%d, TransOption:%02X",
                teachingData->PIODirection, teachingData->PIOTimeLevel,
                teachingData->LookDownLevel, teachingData->TransOption.Data);

            Error = OHTMainForm->GetTeachingDataManager()->TeachingUpdate(teachingData);

            if (Error) {
				// 에러처리
                ADD_LOG_TASKCONTROL(
                    "[Data][Teaching][Update]Fail SID:%d, PT:%d, H:%d, S:%d, R:%d",
                    teachingData->StationID, teachingData->PortType,
                    teachingData->Hoist, teachingData->Shift, teachingData->Rotate);
            }
        }
    }

	return Error;
}

/**
 @brief   OHT의 Cycle을 변경하는 함수
 @author  puting
 @date    2015.11.27
 @param   status : 변경할 Status
 @note    공유변수인 OHTMainStatus의 Status를 변경한다.
 */
void TaskControl::ChangeCmdIDAndCycle(BYTE CurCycle, unsigned short CurCmdID, UINT StationNID,bool bWaitCycleClear)
{
	CS_OHTMainStatus->Acquire();
	try {
		m_pOHTMainStatus->CurCycle = CurCycle;
		m_pOHTMainStatus->CurCmdID = CurCmdID;
		m_pOHTMainStatus->TargetStationNID = StationNID;


		if((CurCycle !=0) ||
		   (bWaitCycleClear ==true))
		{
			//현재싸이클이 새로운 싸이클로 갱신 될때 초기화 처리할것.
			m_pOHTMainStatus->WaitCycle = CYCLE::CycleNotExist;
			m_pOHTMainStatus->WaitTargetStationNID = 0;
        }

	}
	__finally {
		CS_OHTMainStatus->Release();
	}
}
// ---------------------------------------------------------------------------

/**
 @brief   OHT의 Status를 변경하는 함수
 @author  zzang9un
 @date    2013.03.21
 @param   status : 변경할 Status
 @note    공유변수인 OHTMainStatus의 Status를 변경한다.
 */
void TaskControl::ChangeStatus(BYTE status) {
	BYTE CurStatus = m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status;

	if ((CurStatus == STS_GOING) && (status != STS_GOING) ||
		(status == STS_ERROR) || (status == STS_TURN_OFF) || (status == STS_AUTORECOVERY_OVERRUN)) {
		CS_OHTMainStatus->Acquire();
		try {
			m_pOHTMainStatus->IsOnGoCmd = false;
		}
		__finally {
			CS_OHTMainStatus->Release();
		}
	}

	// by zzang9un 2013.03.21 : Mode 변수를 union 구조체의 status를 사용
	// by zzang9un 2013.04.10 : Critical Section 추가
	CS_OHTMainStatus->Acquire();
	try {
		m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status = status;
	}
	__finally {
		CS_OHTMainStatus->Release();
	}
}
// ---------------------------------------------------------------------------

/**
 @brief   OHT Mode를 변경하는 함수
 @author  zzang9un
 @date    2012.11.16
 @param   mode : 변경할 모드
 */
void TaskControl::ChangeMode(BYTE mode)
{
	// by zzang9un 2013.03.21 : Mode 변수를 union 구조체의 mode를 사용

	m_StepDetectAreaCheck.bAllDone = false;  // shcho 2018.05.16 : UBG 체크 스탭을 여러번 돌지 않기 위해서 마지막에 Alldone = false 한번
	
	if ( m_pOHTMainStatus->DataUseFlag.fMapsetLoad == false)
	{
		m_pExecuteInfo->ExecuteMode = EXECUTE_MODE_MANUAL;
		ADD_ERROR_LOG("[MAP Error] Mapset Load Fail");
		return;
	}
	if ( m_pOHTMainStatus->DataUseFlag.fUBGLoad == false || m_pOHTMainStatus->DataUseFlag.fUBGLoad == 0xff)
	{
		m_pExecuteInfo->ExecuteMode = EXECUTE_MODE_MANUAL;
		ADD_ERROR_LOG("[MAP Error] UBG DATA Invalid %d, %d ",m_pOHTMainStatus->DataUseFlag.fUBGLoad, m_pOHTMainStatus->DataUseFlag.fUBGLoad);
		return;
	}
	
	CS_OHTMainStatus->Acquire();
	try {
		m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode = mode;
		if (mode == MODE_MANUAL)
		{
			m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning = OFF;
			m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp = OFF;
		}

	}
	__finally {
		CS_OHTMainStatus->Release();
	}

	if (mode == MODE_AUTO)
	{
		m_pExecuteInfo->ExecuteMode = EXECUTE_MODE_AUTO;
	}
	else
	{
		m_bCheckAutoToManual = false;
		m_pExecuteInfo->ExecuteMode = EXECUTE_MODE_MANUAL;
	}



}
// ---------------------------------------------------------------------------

/**
 @brief   OHT CmdID를 변경하는 함수
 @author  zzang9un
 @date    2013.04.10
 @param   PreCmdID : 이전 수행한 Command ID
 @param   CurCmdID : 현재 수행 Command ID
 */
void TaskControl::ChangeCmdID(BYTE PreCmdID, BYTE CurCmdID) {
	// by zzang9un 2013.04.10 : Critical Section 추가
	CS_OHTMainStatus->Acquire();
	try {
		m_pOHTMainStatus->StatusCommon.MSCmdID.Info.PreCmdID = PreCmdID;
		m_pOHTMainStatus->StatusCommon.MSCmdID.Info.CurCmdID = CurCmdID;
	}
	__finally {
		CS_OHTMainStatus->Release();
	}
}
// ---------------------------------------------------------------------------

/**
 @brief   OHT의 Mode를 가져오는 함수
 @author  zzang9un
 @date    2013.07.10
 @return  현재 OHT의 Mode를 리턴한다.
 */
BYTE TaskControl::GetCurrentMode() {
	return m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
}

// ---------------------------------------------------------------------------

/**
 @brief   OHT의 Status를 가져오는 함수
 @author  zzang9un
 @date    2013.03.21
 @return  현재 OHT의 Status를 리턴한다.
 */
BYTE TaskControl::GetExecuteStatus() {
	return m_pExecuteInfo->StatusCmdId.Info.Status;
}
// ---------------------------------------------------------------------------

/**
 @brief   Execute의 CurCmdID를 가져오는 함수
 @author  zzang9un
 @date    2013.03.21
 @return  현재 Execute의 CurCmdID를 리턴한다.
 */
BYTE TaskControl::GetExecuteCurCmdID() {
	return m_pExecuteInfo->StatusCmdId.Info.CurrCmd;
}
// ---------------------------------------------------------------------------

/**
 @brief   Execute의 PreCmdID를 가져오는 함수
 @author  zzang9un
 @date    2013.03.21
 @return  현재 Execute의 PreCmdID를 리턴한다.
 */
BYTE TaskControl::GetExecutePreCmdID() {
	return m_pExecuteInfo->StatusCmdId.Info.PreCmd;
}
// ---------------------------------------------------------------------------

/**
 @brief   Driving이 가능한 상태인지를 체크하는 함수
 @author  zzang9un
 @date    2012.11.21
 @return  Driving이 가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 */
bool TaskControl::CheckStatusForDriving()
{
	BYTE chkStatus = GetExecuteStatus();
	bool RtnVal = false;

	if(m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing == true)
		return false;

	switch (chkStatus) {
	case STS_GOING: // Going
	case STS_INIT: // Initialized
	case STS_ARRIVED: // Arrived
	case STS_UNLOADED: // Unloaded
	case STS_LOADED: // Loaded
	case STS_COMPLETE: // Manual 수행 완료
	case STS_AUTOPOSITION: // Auto Position
	case STS_TURN_OFF: // 기동 off
	case STS_AUTO_TAUGHT:
		RtnVal = true;
		break;

	case STS_LOADING: // Loading
	case STS_UNLOADING: // Unloading
	case STS_RUNNING: // Manual 수행 중
	case STS_ERROR: // Error
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
	case STS_AUTO_COMPLETE:
	case STS_AUTO_TRANS_HOMING:
		RtnVal = false;
		break;
	}

	return RtnVal;

}
// ---------------------------------------------------------------------------

/**
 @brief   Trans가 가능한 상태인지를 체크하는 함수
 @author  zzang9un
 @date    2012.11.21
 @return  Trans가 가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 */
bool TaskControl::CheckStatusForTrans() {
	BYTE chkStatus = GetExecuteStatus();
	BYTE chkProgress = m_pOHTMainStatus->StandardOHTPreProgress;

	UINT CycleListCount = m_pCmdManager->GetCycleCount();
	UINT PassOpenListCount = m_pPassPermit->GetPassOpenCount();

	bool RtnVal = false;

	//puting 추가 수정 2015.11.19
	if ((m_pExecuteInfo->ListOfCmd.getCount() != LISTCOUNT_EMPTY) ||
//		(m_pPathSearch->GetPathCount() !=LISTCOUNT_EMPTY) ||
		(m_pPassPermit->GetPPIndex() != m_pPassPermit->GetPPOIndex()) ||
		(CycleListCount != (m_pPassPermit->GetPPOIndex()+1)) ||
		(PassOpenListCount !=LISTCOUNT_EMPTY)   ||
		(chkProgress == PROGRESS::DrivingBegin) ||
		(chkProgress ==PROGRESS::TransBegin))
		{
			if(CycleListCount ==(m_pPassPermit->GetPPOIndex()+1))
			{
				//ADD_LOG_TASKCONTROL("CheckStatusForTrans %d %d %d %d",CycleListCount,PassOpenListCount,chkProgress,m_pExecuteInfo->ListOfCmd.getCount());
			}

			return false;
		}


	if(m_pExecuteInfo->bPopListOfCmd ==true)
		return false;

	switch (chkStatus)
	{
		case STS_INIT: // Initialized
		case STS_ARRIVED: // Arrived
		case STS_UNLOADED: // Unloaded
		case STS_LOADED: // Loaded
		case STS_COMPLETE: // Manual 수행 완료
		case STS_AUTO_TAUGHT:
			RtnVal = true;
			break;

		case STS_GOING: // Going
		case STS_UNLOADING: // Unloading
		case STS_LOADING: // Loading
		case STS_AUTOPOSITION: // Auto Position
		case STS_TURN_OFF: // 기도우오프
		case STS_RUNNING: // Manual 수행 중
		case STS_ERROR: // Error
		case STS_AUTO_TEACHING:
		case STS_AUTO_TEACHING_NULLTYPE:
		case STS_AUTO_COMPLETE:
		case STS_AUTO_TRANS_HOMING:
			RtnVal = false;
			break;
	}

	return RtnVal;
}

// ---------------------------------------------------------------------------

/**
 @brief   Common 명령 수행이 가능한 상태인지를 체크하는 함수
 @author  zzang9un
 @date    2012.11.21
 @return  가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 @note    에러인 경우에도 manual 동작이 수행될 수 있도록 변경
 */
bool TaskControl::CheckStatusForCommon() {
	BYTE chkStatus = GetExecuteStatus();
	bool RtnVal = false;

	if ((m_pExecuteInfo->ListOfCmd.getCount() != LISTCOUNT_EMPTY) ||
		(m_pCmdManager->GetCycleCount() != LISTCOUNT_EMPTY) ||
		(m_pOHTMainStatus->bCycleing == true))
		return false;

	switch (chkStatus) {
	case STS_INIT: // Initialized
	case STS_ARRIVED: // Arrived
	case STS_UNLOADED: // Unloaded
	case STS_LOADED: // Loaded
	case STS_COMPLETE: // Manual 수행 완료
	case STS_ERROR: // Error
	case STS_AUTORECOVERY: // 오토리커버리
	case STS_AUTORECOVERY_BLANKTRANS: // 공추라
	case STS_AUTORECOVERY_DOUBLETRANS: // 이중반입
	case STS_AUTORECOVERY_OVERRUN: // 오브런
	case STS_AUTO_TAUGHT:
		RtnVal = true;
		break;

	case STS_GOING: // Going
	case STS_UNLOADING: // Unloading
	case STS_LOADING: // Loading
	case STS_AUTOPOSITION: // Auto Position
	case STS_TURN_OFF: // 기도우오프
	case STS_RUNNING: // Manual 수행 중
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
	case STS_AUTO_COMPLETE:
	case STS_AUTO_TRANS_HOMING:
		RtnVal = false;
		break;
	}

	return RtnVal;
}
// ---------------------------------------------------------------------------

/**
 @brief   Reset 명령 수행이 가능한 상태인지를 체크하는 함수
 @author  임태웅
 @date    2013.04.14
 @return  가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 @note    동장 중인 경우는 Reset 실행하지 않음
 */
bool TaskControl::CheckStatusForReset() {
	BYTE chkStatus = GetExecuteStatus();
	bool RtnVal = false;

	if(m_pOHTMainStatus->bCheckCycleRunning == true)
		return false;
	
	switch (chkStatus) {
	case STS_INIT: // Initialized
	case STS_ARRIVED: // Arrived
	case STS_UNLOADED: // Unloaded
	case STS_LOADED: // Loaded
	case STS_COMPLETE: // Manual 수행 완료
	case STS_ERROR: // Error
	case STS_RUNNING: // Manual 수행 중
	case STS_AUTORECOVERY: // AUTORECOVERY
	case STS_AUTORECOVERY_BLANKTRANS:
	case STS_AUTORECOVERY_DOUBLETRANS:
	case STS_AUTOPOSITION: // Auto Position
	case STS_TURN_OFF: // 기도우오프
	case STS_AUTORECOVERY_OVERRUN: // Auto Position
	case STS_AUTO_TAUGHT:
    case STS_AUTO_COMPLETE:
		RtnVal = true;
		break;

	case STS_GOING: // Going
	case STS_UNLOADING: // Unloading
	case STS_LOADING: // Loading
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
	case STS_AUTO_TRANS_HOMING:
		RtnVal = false;
		break;
	}

	return RtnVal;
}
// ---------------------------------------------------------------------------

/**
 @brief   Manual 명령 수행이 가능한 상태인지를 체크하는 함수
 @author  임태웅
 @date    2013.03.04
 @return  가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 @note    에러인 경우에도 manual 동작이 수행될 수 있도록 변경
 */
bool TaskControl::CheckStatusForManual()
{
	BYTE chkStatus = GetExecuteStatus();
	bool RtnVal = false;

	//puting 추가 수정 2015.11.19
	if ((m_pExecuteInfo->ListOfCmd.getCount() != LISTCOUNT_EMPTY) ||
//		(m_pPathSearch->GetPathCount() !=LISTCOUNT_EMPTY) ||
		(m_pPassPermit->GetPPIndex() != m_pPassPermit->GetPPOIndex()))
		return false;

	switch (chkStatus) {
	case STS_INIT: // Initialized
	case STS_ARRIVED: // Arrived
	case STS_UNLOADED: // Unloaded
	case STS_LOADED: // Loaded
	case STS_COMPLETE: // Manual 수행 완료
	case STS_ERROR: // Error
	case STS_RUNNING: // Manual 수행 중
	case STS_AUTO_TAUGHT:
		RtnVal = true;
		break;

	case STS_GOING: // Going
	case STS_UNLOADING: // Unloading
	case STS_LOADING: // Loading
	case STS_AUTOPOSITION: // Auto Position
	case STS_TURN_OFF: // 기도우오프
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
	case STS_AUTO_COMPLETE:
	case STS_AUTO_TRANS_HOMING:
		RtnVal = false;
		break;
	}

	return RtnVal;
}
// ---------------------------------------------------------------------------
//AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
bool TaskControl::CheckAutoChangeFullSemi()
{
	bool bReturn = true; //false 세미오토, true full auto

	if(m_pHWSet->m_pSteer->IsLeftPos(BOTH) == false && m_pHWSet->m_pSteer->IsRightPos(BOTH) == false)
	{
		bReturn = false;
	}
	else if(m_pHWSet->m_pDrivingTag->IsOn() == false ||
		   m_pHWSet->m_pTransStopTag->IsOnFrontSensor() == false  ||
		   m_pHWSet->m_pTransStopTag->IsOnRearSensor() == false)
   {
		bReturn = false;
   }
   //QR Code Reading Left, Right  넣기
   //else if
   //
   ADD_LOG_TASKCONTROL("CheckAutoChangeFullSemi bReturn %d",bReturn);

	return bReturn;
}

int TaskControl::CheckStatusForAutoChange()
{

	int nRtnVal = NO_ERR;
	// Shift Hoist 모두 Home 위치 x ->

	if(m_pHWSet->m_pHoistAxis->IsHoistHome() == false || m_pHWSet->m_pShiftAxis->IsShiftHome() == false)
	{
		if(m_pHWSet->m_pHand->IsLeftGripOn() == true || m_pHWSet->m_pHand->IsRightGripOn() == true  || m_pHWSet->m_pHand->IsClose() == true )//gripper 하나라도 ON이면 Nack
		{  //hoist, slide 같이 확인
		// Hoist, Slide 모두 Home에 없어도 autochange 가능하도록 구현 하기 위해 Hoist, Slide 원점 확인 제외
			nRtnVal = 0x07;
		}
	}

	else if(m_pHWSet->m_pBumper->IsBumperPress() == ON)
	{
		nRtnVal = 0x04;
	}
	else if(m_pHWCommon->IsSerialOpen() == false || m_pHWCommon->IsAMCDllOpen() == false )
	{
		nRtnVal = 0x05;

	}else if(OHTMainForm->m_MapUpdateResult != NO_ERR)
	{
		nRtnVal = 0x06;
	}
	else if(GetCurrentMode() == MODE_AUTO)
	{
		nRtnVal = 0x02;
	}
	else if((CheckError() != ERR_AXIS1_F_AMP_FAULT && CheckError() != ERR_AXIS1_R_AMP_FAULT) && CheckError() != NO_ERR)
	{
		nRtnVal = 0x0A;
	}
	else if(m_pHWSet->m_pHand->IsPushOn() == true)
	{
		nRtnVal = 0x08;
	}
	else if(m_StepAutoChangeReset.bAllDone == false)
	{
		nRtnVal = 0x01;
	}
	else if(GetExecuteStatus() != STS_ERROR && GetExecuteStatus() != STS_INIT)
	{
		nRtnVal = 0x03;
	}

	else if(m_pHWSet->m_pHand->IsGoSigOn() == false)    // Go signal 로 Hand Power OFF 확인  대체
	{
		nRtnVal = 0x09;
	}
	 //hoist Home 위치가 아니면 nack
	 return nRtnVal;
}

/**
 @brief   Auto 전환이 가능한 상태인지를 체크하는 함수
 @author  장영환
 @date    2013.08.18
 @return  가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 */
bool TaskControl::CheckStatusForAuto() {
	BYTE chkStatus = GetExecuteStatus();
	bool RtnVal = false;

	if (m_pExecuteInfo->ListOfCmd.getCount() != LISTCOUNT_EMPTY)
		return false;

	if(GetCurrentMode() == MODE_AUTO)
	 	return false;

	switch (chkStatus) {
	case STS_INIT: // Initialized
		RtnVal = true;
		break;

	case STS_ARRIVED: // Arrived
	case STS_UNLOADED: // Unloaded
	case STS_LOADED: // Loaded
	case STS_COMPLETE: // Manual 수행 완료
	case STS_ERROR: // Error
	case STS_RUNNING: // Manual 수행 중
	case STS_GOING: // Going
	case STS_UNLOADING: // Unloading
	case STS_LOADING: // Loading
	case STS_AUTOPOSITION: // Auto Position
	case STS_TURN_OFF: // 기도우오프
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
	case STS_AUTO_TAUGHT:
	case STS_AUTO_COMPLETE:
	case STS_AUTO_TRANS_HOMING:
		RtnVal = false;
		break;
	}

	return RtnVal;
}

/**
 @brief   Task 수행 가능한 상태인지를 체크하는 함수
 @author  박태욱
 @date    2013.08.18
 @return  가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 */
bool TaskControl::CheckStatusForTask()
{
	BYTE chkStatus = GetExecuteStatus();
	bool RtnVal = false;

	switch (chkStatus)
	{
	case STS_INIT: // Initialized
	case STS_ARRIVED: // Arrived
	case STS_UNLOADED: // Unloaded
	case STS_LOADED: // Loaded
	case STS_COMPLETE: // Manual 수행 완료
	case STS_ERROR: // Error
	case STS_RUNNING: // Manual 수행 중
	case STS_GOING: // Going
	case STS_UNLOADING: // Unloading
	case STS_LOADING: // Loading
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
	case STS_AUTO_TAUGHT:
	case STS_AUTOPOSITION: // Auto Position
	case STS_TURN_OFF: // 기도우오프
	case STS_AUTO_TRANS_HOMING:
 		RtnVal = true;
		break;

	case STS_AUTORECOVERY: // Auto Position
	case STS_AUTORECOVERY_OVERRUN: // Auto Position
	case STS_AUTORECOVERY_BLANKTRANS: // Auto Position
	case STS_AUTORECOVERY_DOUBLETRANS: // Auto Position
	case STS_AUTO_COMPLETE:
		RtnVal = false;
		break;
	}

	return RtnVal;
}
// ---------------------------------------------------------------------------

/**
 @brief   Execute Thread로 Quick Command를 보내는 함수
 @author  zzang9un
 @date    2013.02.06
 @param   QuickCmd : 전송할 Quick Command
 */
void TaskControl::SendQuickCmd2Execute(EXECUTE_QUICK_CMD QuickCmd)
{
	CS_CmdList->Acquire();
	switch (QuickCmd)
	{
	case EXECUTE_QUICK_CMD_PAUSE_OCS:
		m_pExecuteInfo->QuickCmd = QuickCmd;
		ADD_LOG_TASKCONTROL("[QuickCmd] Send EXECUTE_QUICK_CMD_PAUSE_OCS");
		break;

	case EXECUTE_QUICK_CMD_RESUME_OCS:
		m_pExecuteInfo->QuickCmd = QuickCmd;
		ADD_LOG_TASKCONTROL("[QuickCmd] Send EXECUTE_QUICK_CMD_RESUME_OCS");
		break;

	case EXECUTE_QUICK_CMD_PAUSE_TP:
		m_pExecuteInfo->QuickCmd = QuickCmd;
		ADD_LOG_TASKCONTROL("[QuickCmd] Send EXECUTE_QUICK_CMD_PAUSE_TP");
		break;

	case EXECUTE_QUICK_CMD_RESUME_TP:
		m_pExecuteInfo->QuickCmd = QuickCmd;
		ADD_LOG_TASKCONTROL("[QuickCmd] Send EXECUTE_QUICK_CMD_RESUME_TP");
		break;

	case EXECUTE_QUICK_CMD_TURN_OFF:
		m_pExecuteInfo->QuickCmd = QuickCmd;
		m_pExecuteInfo->QuickCmdEStopCommander = ESTOP_BY_TURM_OFF;
		m_pExecuteInfo->QuickCmdEStopType = TRUNOFF_STOP;
		ADD_LOG_TASKCONTROL("[QuickCmd] Send EXECUTE_QUICK_CMD_TURN_OFF");
		break;

	case EXECUTE_QUICK_CMD_TURN_ON:
		m_pExecuteInfo->QuickCmd = QuickCmd;
		m_pExecuteInfo->QuickCmdEStopCommander = BY_TURM_ON;
		m_pExecuteInfo->QuickCmdEStopType = TRUNON;
		ADD_LOG_TASKCONTROL("[QuickCmd] Send EXECUTE_QUICK_CMD_TURN_ON");
		break;


	case EXECUTE_QUICK_CMD_STOP:
		// by zzang9un 2013.04.22 : Status에 관계없이 E-Stop을 바로 전달함
		// by zzang9un 2013.04.22 : 임태웅 선임과 협의 후 수정
		m_pExecuteInfo->QuickCmd = QuickCmd;

		m_pExecuteInfo->QuickCmdEStopCommander = m_pCmdManager->m_MainStatus->QuickCmdEStopCommander;

		if (m_pExecuteInfo->QuickCmdEStopCommander == ESTOP_BY_OCS)
		{
			m_pExecuteInfo->QuickCmdEStopType = m_pCmdManager->m_MainStatus->QuickCmdEStopType;
			ADD_LOG_TASKCONTROL("[QuickCmd] Send OCS EXECUTE_QUICK_CMD_STOP");
		}
		else {
			m_pExecuteInfo->QuickCmdEStopType = 0;
			ADD_LOG_TASKCONTROL("[QuickCmd] Send TP EXECUTE_QUICK_CMD_STOP");
		}

		break;

	case EXECUTE_QUICK_CMD_RESET:
		// reset 할 때 상태를 Manual로 만들어 준다.
		if (m_pExecuteInfo->ListOfCmd.getCount() == 0) {
			m_pExecuteInfo->ExecuteUnit = EXECUTE_UNIT_MANUAL;
		}

		m_pExecuteInfo->QuickCmd = QuickCmd;
		ADD_LOG_TASKCONTROL("[QuickCmd] Send EXECUTE_QUICK_CMD_RESET");
		break;

	default:
		m_pExecuteInfo->QuickCmd = QuickCmd;
		break;
	}
	CS_CmdList->Release();
}
// ---------------------------------------------------------------------------
/**
 @brief   MainControlTask, StatusCheckThread, ExecuteThread의 에러를 체크한다.
 @author  zzang9un
 @date    2013.03.04
 @return  에러 코드 리턴
 @note    에러코드의 우선순위는 Main -> StatusCheck -> Execute 순으로 체크한다.
 */
int TaskControl::CheckError() {
	int Error = NO_ERR;

	// Step 1. MainControlTask 에러 체크
	if (ErrorCode_Main)
		Error = ErrorCode_Main;

	// Step 2. StatusCheckThread 중 에러가 있는 경우
	if ((Error == NO_ERR) && (m_pOHTMainStatus->ErrCode_StatusChk))
		Error = m_pOHTMainStatus->ErrCode_StatusChk;

	// Step 3. ExecuteThread 중 에러가 있는 경우
	if ((Error == NO_ERR) && (m_pExecuteInfo->ErrCode_ExecuteInfo))
		Error = m_pExecuteInfo->ErrCode_ExecuteInfo;

	
	return Error;
}
// ---------------------------------------------------------------------------
/**
 @brief   Station Data를 make하는 함수
 @date    2013.05.24
 @author  zzang9un
 @param   pStationData : Make한 Station Data가 저장될 구조체 포인터
 @return  정상인 경우 0, 그렇지 않은 경우 에러 코드 리턴
 */
int TaskControl::MakeSetStationOffset(STATION_DATA *pStationData) {

	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO;

	int Error = NO_ERR;

	 if(CheckStatusForDriving())
	{
		m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = true;
		CS_CmdList->Acquire();
		ADD_LOG_TASKCONTROL("Criticle CmdList1");

		pEXECUTE_DRIVING_INFO = new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->PathIndexNum = 0;
		pEXECUTE_DRIVING_INFO->AddInfo.CmdType = EXECUTE_DRIVING_CMD_SET_STATION_OFFSET;
		pEXECUTE_DRIVING_INFO->AddInfo.StationType = pStationData->StationType;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID = pStationData->ParentNodeID;
		pEXECUTE_DRIVING_INFO->AddInfo.StationID = pStationData->StationID;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID = pStationData->NextNodeID;
		pEXECUTE_DRIVING_INFO->AddInfo.DistToStation = pStationData->StationOffset;

		m_pExecuteInfo->ExecuteUnit = EXECUTE_UNIT_DRIVING;
		m_pExecuteInfo->ListOfCmd.insertBack((void*)pEXECUTE_DRIVING_INFO);

		CS_CmdList->Release()
		;
		m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = false;

		Sleep(50); // by zzang9un 2013.08.26 : ExecuteInfo가 처리하는 시간

		ADD_LOG_TASKCONTROL(
			"[MakeSetStationOffset] M->A, CurrStation=%06d, StationID=%06d, Type=%d, Offset=%d, P-Node=%06d, N-Node=%06d",
			m_pExecuteInfo->ExecutePositionInfo.CurrStation,
			pStationData->StationID, pStationData->StationType,
			pStationData->StationOffset, pStationData->ParentNodeID,
			pStationData->NextNodeID);

		if ((m_pExecuteInfo->ExecutePositionInfo.CurrNode !=  pStationData->ParentNodeID))
		{
//			(m_pExecuteInfo->ExecutePositionInfo.CurrStation !=
//			pStationData->StationID) ||
//			(m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset !=
//			pStationData->StationOffset)) {
			// 제대로 반영이 되지 않는 경우 에러 처리
			Error = ERR_MARK_COMMAND;
			ADD_LOG_TASKCONTROL(
				"[Auto Fail] NodeID:%06d, StationID:%06d, StationOffset:%d",
				m_pExecuteInfo->ExecutePositionInfo.CurrNode,
				m_pExecuteInfo->ExecutePositionInfo.CurrStation,
				m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset);
			ADD_FDC_LOG("ERR/%d/2/6/%06d/%06d/%d/%d/%06d/%06x/%d",
						ERR_MARK_COMMAND,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_pExecuteInfo->ExecutePositionInfo.CurrStation);
		}
	}

	return Error;
}
void TaskControl::StandardAddress_Broadcast()
{
	try{
		if(m_pUDPCommunication->OcsConnected() !=2)
			m_pUDPCommunication->Address_Broadcast_NotClear();   //주소획득 연결 실패시 재시도 해야됨.


	}catch(...)
		ADD_LOG_TASKCONTROL("StandardAddress_Broadcast Exception!!!");
}
// ---------------------------------------------------------------------------
/**
 @brief   표준화  OHT Read Data Check 함수.
 @date    2015.11.10
 @author  puting
 */
void TaskControl::StandardOCSReadDataUpdate()
{
	int nError = 0;
	int nCheckRcvData = RCV_ST;  		///< 버퍼 Data 확인하는 부분  //0 기준 : Data 있음
	int nCheckAckData = CMD_ACKNONE; 	///< ACK Data 확인하는 부분   //1: ACK 있음
	PKT_OCS2OHT 			RcvPkt; 	///< 수신한 내용이 저장되는 구조체
	char strLog[1024] = {0,};

	// 2016.12.15 sh17.jo Commfail Check 변수 추가
	static DWORD dwRcvDataTime =  timeGetTime();
	static DWORD dwArpClearTime =  0;
	DWORD dwCurrentDataTime =  timeGetTime();
	static bool bSet = false;
	static bool bArpCleared = false;
	static DStep stepCommFail;
	// Step 1. Communication 처리 부분
	// 통신개시 확인하는 부분

	try{
		 m_pUDPCommunication->BroadCast_ThreadCall();

		 nCheckRcvData = m_pUDPCommunication->UniCast_ThreadCall(RcvPkt);

		// ReadData 값 확인하는 부분


		// Step 2. ReadData 확인하는 부분
		if (nCheckRcvData < RCV_ST)
		{
			// Commfail NG 플래그 설정.     ?? 김승현 책임과 어떻게 처리될지 이야기 필요.
			// NG 일 경우, 상태값 전송 안함.??
			if(nCheckRcvData !=UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND)
			{
			  //에러처리 사양서에서 없는 부분
				return;
			}
            // 16.12.15 Commfail Check 로직 추가 sh17.jo
			// 수신한 데이터가 없을 경우 Commfail Check 로직 동작
            // OCS의 경우 최대 10초 주기로 OHT로 상태 정보를 송신함
			else
			{
				// OCS로 부터 데이터를 수신 받은적인 있는 경우
				if(m_bCheckRcvData)
				{
					// OCS로 부터 Data가 nMaxOCSRcvTime 이상 수신되지 않는 경우
					DWORD dwFailElapse = dwCurrentDataTime - dwRcvDataTime;
					if(dwFailElapse > m_defualtparam->nMaxOCSRcvTime)
					{
						if(m_bSetCheckCommfail == false)
						{
							m_mccparam.nOCS_Comm++;
						}
						// Commfail Check 로직 수행
						m_bSetCheckCommfail = true;
                        m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_OT); //AOHC-253 Monitoring Log 항목추가 2
						// OCS 수신 데이터 확인 미수행
						m_bCheckRcvData = false;
					}
				}
			}
		}
		else if (nCheckRcvData > RCV_ST)
		{

			// ACK_DATA 구조체 초기화
		  //	ADD_LOG_TASKCONTROL(" StandardOCSReadDataUpdate Start");
			//Sleep(100);
			//ADD_LOG_TASKCONTROL("StandardOCSReadDataUpdate_ TagID  %d ",RcvPkt.TAG.Value);
			// Step 3. 읽은 Data Parsing하는 부분(무조건 기본 포멧으로 받음)
			 //Commfail Check! puting 20160527
			 m_pOHTMainStatus->CheckCommfailtime = timeGetTime();
			 nCheckAckData = StandardParsingOCSCmd(&RcvPkt);

			// 16.12.15 OCS로 부터 데이터를 수신 받은 경우 최종 수신 시간 업데이트 sh17.jo
            if(bSet)
            {
				m_DiagManager->IncreasePreviosDiagMax(PREVIOS_DIAG_COMM, abs((int)(dwCurrentDataTime - dwRcvDataTime)));
            }
            else
            {
				bSet = true;
            }

			dwRcvDataTime = dwCurrentDataTime;
			dwArpClearTime = dwCurrentDataTime;
			m_bCheckRcvData = true;
			bArpCleared = false;
			stepCommFail.Step = 0;
			
			// 2017.01.02 added.
			if(m_bSetCheckCommfail)
			{
				m_bSetCheckCommfail = false;
			
				// Commfail 관련 변수 초기화
				memset(m_nComfailCount, 0, sizeof(m_nComfailCount));
				m_nComOKCount=0;
			}
		}
	}catch(...)
		ADD_LOG_TASKCONTROL(" StandardOCSReadDataUpdate Exception!!!!!");
}

// ---------------------------------------------------------------------------
/**
 @brief   표준화  OHT Read Data Check 함수.
 @date    2015.11.10
 @author  puting
 */
void TaskControl::StandardPCReadDataUpdate()
{
	int nError = 0;
	int nCheckRcvData = RCV_ST;  		///< 버퍼 Data 확인하는 부분  //0 기준 : Data 있음
	int nCheckAckData = CMD_ACKNONE; 	///< ACK Data 확인하는 부분   //1: ACK 있음

	PKT_OCS2OHT 			RcvPkt; 	///< 수신한 내용이 저장되는 구조체

	char strLog[1024] = {0,};
//	int nCount = 0;
//
//	bool nFinishFlag = false;


	// Step 1. Communication 처리 부분
	// 통신개시 확인하는 부분



//	m_pUDPCommunication->BroadCast_ThreadCall();

	
	char szTpClientIP[30];
	ZeroMemory(szTpClientIP, sizeof(szTpClientIP));

	// ReadData 값 확인하는 부분
	nCheckRcvData = m_pUDPCommunication->OHT_TP_ThreadCall(RcvPkt, szTpClientIP);




	// Step 2. ReadData 확인하는 부분
   if (nCheckRcvData <= RCV_ST)
	{
		// Commfail NG 플래그 설정.     ?? 김승현 책임과 어떻게 처리될지 이야기 필요.
		// NG 일 경우, 상태값 전송 안함.??
		if(nCheckRcvData !=UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND)
		{
		  //에러처리
			return;
		}

		//무시


	}
	else if (nCheckRcvData > RCV_ST)
	{
		// TP 관리 객체에 현재 접속된 TP 주소 등록
		m_TPAddrMan.Add(szTpClientIP);

		// ACK_DATA 구조체 초기화
//		memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));
		// ADD_LOG_TASKCONTROL("StandardPCReadDataUpdate_ TagID  %d ",RcvPkt.TAG.Value);
		// Step 3. 읽은 Data Parsing하는 부분(무조건 기본 포멧으로 받음)
		 nCheckAckData = StandardParsingPCCmd(&RcvPkt);

	}
}

// ---------------------------------------------------------------------------
/**
 @brief   표준화  AST Read Data Check 함수.
 @date    2015.11.10
 @author  puting
 */
void TaskControl::StandardASTReadDataUpdate()
{

	int nCheckRcvData = RCV_ST;  		///< 버퍼 Data 확인하는 부분  //0 기준 : Data 있음
//	int nCheckAckData = CMD_ACKNONE; 	///< ACK Data 확인하는 부분   //1: ACK 있음

	PKT_OHT2OCS 			RcvPkt; 	///< 수신한 내용이 저장되는 구조체
//	PKT_OHT2OHT 			RcvPkt; 	///< 수신한 내용이 저장되는 구조체


	// ReadData 값 확인하는 부분
	nCheckRcvData = m_pUDPDiag->OHT_AS_ThreadCall(RcvPkt);

	// Step 2. ReadData 확인하는 부분
   if (nCheckRcvData < RCV_ST)
	{
		// Commfail NG 플래그 설정.     ?? 김승현 책임과 어떻게 처리될지 이야기 필요.
		// NG 일 경우, 상태값 전송 안함.??
		if(nCheckRcvData !=UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND)
		{
		  //에러처리
			return;
		}

		//무시


	}
	else if (nCheckRcvData > RCV_ST)
	{

		// ACK_DATA 구조체 초기화
//		memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));
		ADD_LOG_TASKCONTROL("StandardASTReadDataUpdate_ TagID  %d ",RcvPkt.TAG.Value);
		// Step 3. 읽은 Data Parsing하는 부분(무조건 기본 포멧으로 받음)
		 StandardParsingASTCmd(&RcvPkt);

	}
}
// ---------------------------------------------------------------------------
/**
 @brief   표준화  AST Read Data Check 함수.
 @date    2015.11.10
 @author  puting
 */
void TaskControl::StandardMonitoringReadDataUpdate()
{

try{
	int nCheckRcvData = RCV_ST;  		///< 버퍼 Data 확인하는 부분  //0 기준 : Data 있음

	PKT_OHT2DIAG 			RcvPkt; 	///< 수신한 내용이 저장되는 구조체

	// ReadData 값 확인하는 부분
//	m_pUDPDiag->DIAG_BroadCast_ThreadCall();

	nCheckRcvData = m_pUDPDiag->DIAG_UniCast_ThreadCall(RcvPkt);

	// Step 2. ReadData 확인하는 부분
   if (nCheckRcvData < RCV_ST)
	{
		// Commfail NG 플래그 설정.     ?? 김승현 책임과 어떻게 처리될지 이야기 필요.
		// NG 일 경우, 상태값 전송 안함.??
		if(nCheckRcvData !=UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND)
		{
		  //에러처리
			return;
		}

		//무시

	}
	else if (nCheckRcvData > RCV_ST)
	{

		// ACK_DATA 구조체 초기화
//		memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));
		ADD_LOG_TASKCONTROL("StandardMonitoringReadDataUpdate");
		// Step 3. 읽은 Data Parsing하는 부분(무조건 기본 포멧으로 받음)
		StandardParsingMonitoringCmd(&RcvPkt);


	}

	// Send Data Check
	UpdateAbnormalDiagData();
	UpdateStatisticsDiagData();
	UpdateStatusDiagData();
	UpdateCIDStatusDiagData();
	}catch(...)
     	ADD_LOG_TASKCONTROL("StandardMonitoringReadDataUpdate Exception!!!!");

}

// ---------------------------------------------------------------------------
/**
 @brief   OCS에게 ACK할 데이터를 버퍼에 저장하는 함수
 @author  puting
 @date    2015.11.10
 @param   SndACKData : ACK할 정보 구조체
 @param   SndBuf : 전송할 버퍼의 포인터
 @return  정상인 경우 전송할 bytes 수, 에러 발생한 경우 에러 코드 리턴
 */
void TaskControl::MakeResponseStandardOHT2OCS(PKT_ACK_OHT2OCS* pTempAckData)
{
	//Response List에 저장될 구조체 (동적 메모리 할당)
	PKT_ACK_OHT2OCS* AckData = new PKT_ACK_OHT2OCS;

	memset(AckData, 0x00, sizeof(PKT_OHT2OCS));

	// Step 1. Header 정보  저장

	///기종과 머신ID는 어떤걸로 할지 김승현 책임님과 이야기하기. OHT 정보인지? 아니면 받은 그대로 인지
	memmove(&AckData->AckPkt, &pTempAckData->AckPkt, sizeof(PKT_OHT2OCS));

	AckData->Length =  pTempAckData->Length;
	/*
	memmove(&AckData->AckPkt.HEADER.MachineID, "OHT001",sizeof(AckData->AckPkt.HEADER.MachineID));
	AckData->AckPkt.HEADER.MachineType = MACHINETYPE_VHL; // OHT로 변경

	// Step 3. OHT status Data 저장
	memmove(&AckData->AckPkt.STATUS, &m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus, sizeof(PROTOCOL_STATUS_OHT));
	*/


	//ADD_LOG_TASKCONTROL("Staion TagID:%c,%c,%c, Length:%d",m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.StopNowAddr[0],m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.StopNowAddr[1],m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.StopNowAddr[2]);

	ADD_LOG_TASKCONTROL("MakeResponse TagID:%d, Length:%d",AckData->AckPkt.TAG.Value, AckData->Length, pTempAckData->AckPkt);

	// Step 4. Response List에 저장
	if(m_pCmdManager->GetOCSResponseCount() < 5)
		m_pCmdManager->AddOCSResponse(AckData);
	else
	{
		ADD_LOG_TASKCONTROL("MakeResponse Max Count");
		delete AckData;
    }


}
void TaskControl::MakeResponseStandardOHT2TP(PKT_OHT2TP_RESPONSE* pTempAckData)
{
	//Response List에 저장될 구조체 (동적 메모리 할당)
	PKT_OHT2TP_RESPONSE AckData;

	memmove(&AckData, pTempAckData, sizeof(PKT_OHT2TP_RESPONSE));

	// Step 1. Header 정보  저장

	// Step 3. OHT status Data 저장
	memmove(&AckData.HEADER.MachineID, &m_defualtparam->VHLName, sizeof(AckData.HEADER.MachineID));

	AckData.HEADER.MachineType = MACHINETYPE_VHL; // OHT로 변경
 //	AckData->TAG.Value =

	memmove(&AckData.OHT_STATUS, &m_pOHTMainStatus->StatusTP, sizeof(MSG_STATUS_OHT2TP));

	//2018.10.12 Axis Error 발생 당시 Bumper_press 감지 여부 확인 #Jrespect.im
	if((OHTError_Current != NO_ERR) && (m_pHWSet->m_pBumper->IsBumperPress() == true))
	{
	   AckData.OHT_STATUS.Data.ErrorCode = ERR_BUMPER_PRESS;
//	   ADD_LOG_TASKCONTROL("Make TP ErrorCode Bumper Press, Check BumperSensor");
	}
	else
	{
		AckData.OHT_STATUS.Data.ErrorCode = OHTError_Current;
    }
	// by zzang9un 2013.07.22 : Data Load 여부를 추가한다.
	AckData.OHT_STATUS.DataLoaded.Info.Map = m_bLoadedMap;
	AckData.OHT_STATUS.DataLoaded.Info.Station = m_bLoadedStationData;
	AckData.OHT_STATUS.DataLoaded.Info.Teaching = m_bLoadedTeachingData;
	AckData.OHT_STATUS.DataLoaded.Info.MultiConnTP =  m_bMultiTPConnected;  // TP 다중접속 되었는지 Flag
	AckData.OHT_STATUS.DataLoaded.Info.AutoChanging =  m_bAutoChaning;      // 현재 Auto 변환중인지 Flag
	m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&AckData, sizeof(PKT_OHT2TP_RESPONSE));
}

void TaskControl::MakeResponseStandardOHT2DIAG(PKT_ACK_OHT2DIAG* pTempAckData)
{
	//Response List에 저장될 구조체 (동적 메모리 할당)
	PKT_ACK_OHT2DIAG AckData;

	memmove(&AckData, pTempAckData, sizeof(PKT_ACK_OHT2DIAG));

  	// Step 1. Header 정보  저장
	memmove(&AckData.AckPkt.HEADER.MachineID, &m_defualtparam->VHLName,sizeof(AckData.AckPkt.HEADER.MachineID));
	AckData.AckPkt.HEADER.MachineType = MACHINETYPE_DIAG_NORMAL_OHT; // OHT로 변경


	//빅엔드리안 변환이후에 전송할것.
	ConvertBigLitteDIAG(&AckData);

	m_pUDPDiag->UnicastSend((char*)&AckData.AckPkt, pTempAckData->Length);


}
// ---------------------------------------------------------------------------
/**
 @brief   Map Make 정보를 내부 Data에 적용하는 함수
 @author  zzang9un
 @date    2013.07.23
 @param   MapInfo : 적용할 Map Make 정보
 @return  정상인 경우 0, 에러 발생한 경우 에러 코드 리턴
 */
int TaskControl::UpdateMapMakeInfo(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo) {
	int Error = NO_ERR;

	// Step 1. 업데이트할 정보(주행 map & Station)에 따라 Update 함수 호출
	switch (MapInfo->StationType) {
	case TAG_TYPE_NONE: // 주행 Tag
		if (!m_pOHTMainStatus->DataUseFlag.fSwapping) {
			m_pOHTMainStatus->DataUseFlag.fMap = true;
			Error = UpdateMapData(MapInfo);
			m_pOHTMainStatus->DataUseFlag.fMap = false;
		}
		break;

	case TAG_TYPE_STOP: // Station Tag
	case TAG_TYPE_QR_LEFT:
	case TAG_TYPE_QR_RIGHT:
		if (!m_pOHTMainStatus->DataUseFlag.fSwapping) {
			m_pOHTMainStatus->DataUseFlag.fStation = true;
			Error = UpdateStationData(MapInfo);
			m_pOHTMainStatus->DataUseFlag.fStation = false;
		}
		break;
	}

	return Error;
}
// ---------------------------------------------------------------------------


/**
 @brief   OHTError를 얻기 위한 함수
 @author  zzang9un
 @date    2013.05.23
 */
int TaskControl::GetOHTError() {
	return OHTError_Current;
}
// ---------------------------------------------------------------------------

/**
 @brief   OHTInfo 설정 하기 위한  함수
 @author  zzang9un
 @date    2013.05.23
 */
void TaskControl::SetOHTInfo(uTP_CMD m_pOHTInfo) {
	m_OHTInfo = m_pOHTInfo;
}
// ---------------------------------------------------------------------------

/**
 @brief   Map Make한 정보를 주행 Map에 업데이트 하는 함수
 @author  zzang9un
 @date    2013.07.23
 @param   MapInfo : 업데이트할 Map Make Info
 @return  정상인 경우 0, 에러 발생 시 에러 코드 리턴
 */
int TaskControl::UpdateMapData(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo) {

	AnsiString strMapLog;
	
	int Error = NO_ERR;

	// Step 1. 업데이트 할 Path ID를 가져온다.
	int PathIndex = 0;
	int NodeIndex = 0;
	int Pre_X = 0;
	int Pre_Y = 0;

	// NextIndex 정보로 찾을수 있는 방법 찾을것... 김정대 선임한테 문의
	PathIndex = OHTMainForm->GetMap()->GetPathIndex(MapInfo->NodeOffsetInfo[0].StartNodeID,	MapInfo->NodeOffsetInfo[0].EndNodeID);

	if (PathIndex == INDEX_NONE)
	{
		// by zzang9un 2013.07.24 : Map은 에러를 처리해서 진행이 안되도록 한다.
		Error = ERR_MAPMAKE_NOTFOUNDNODEID;
		ADD_LOG_MAPMAKE("[확인필요][Error][ERR_MAPMAKE_NOTFOUNDNODEID](%06d~%06d)"
			, MapInfo->NodeOffsetInfo[0].StartNodeID,
			MapInfo->NodeOffsetInfo[0].EndNodeID);


		strMapLog.sprintf("[확인필요][Error][ERR_MAPMAKE_NOTFOUNDNODEID](%06d~%06d)"
							, MapInfo->NodeOffsetInfo[0].StartNodeID
							, MapInfo->NodeOffsetInfo[0].EndNodeID);
		Send_MapMakeResult(strMapLog);

		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_MAPMAKE_NOTFOUNDNODEID,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);

	}
	else
	{

		if((OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.DataType & bit_0)==bit_0)
		{
			// Step 2. 해당 PID의 Distance 정보를 업데이트 한다.
			//기존 Map Data Update
			int PreDist = OHTMainForm->GetMap()->m_PathArray[PathIndex].Distance;
			OHTMainForm->GetMap()->m_PathArray[PathIndex].Distance = MapInfo->NodeOffsetInfo[0].Dist;
			NodeIndex = OHTMainForm->GetMap()->FindNodeIndex(MapInfo->NodeOffsetInfo[0].StartNodeID);

			MAMMAKEPROCESS_LINK_INFO update;

			 //표준화 Map Data Update
			if(OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].LeftNodeID == MapInfo->NodeOffsetInfo[0].EndNodeID)
			{

				OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].LeftNodeDistance = MapInfo->NodeOffsetInfo[0].Dist;

				if(MapInfo->StationType == TAG_TYPE_MODIFY)
				{
                 	ADD_LOG_MAPMAKE("[Modify][Driving]LeftDistance:%d",OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].LeftNodeDistance);
                }
				else
				{
					ADD_LOG_MAPMAKE("[Update][Driving]LeftDistance:%d",OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].LeftNodeDistance);
					OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].IsDefaultMap.LOW_DefaultMapInfo = 1;

					update.LinkNID = OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].NodeID;
					update.IsCheck = 0x00;
					m_pMapMake_Process->Update(&update);
                }



			}
			else if(OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].RightNodeID == MapInfo->NodeOffsetInfo[0].EndNodeID)
			{

				OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].RightNodeDistance = MapInfo->NodeOffsetInfo[0].Dist;


				if(MapInfo->StationType == TAG_TYPE_MODIFY)
				{
					ADD_LOG_MAPMAKE("[Modify][Driving]RightDistance:%d",OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].RightNodeDistance);
				}
				else
				{
					ADD_LOG_MAPMAKE("[Update][Driving]RightDistance:%d",OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].RightNodeDistance);
					OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].IsDefaultMap.HIGH_DefaultMapInfo = 1;

					update.LinkNID = 1000000 + (OHTMainForm->GetMap_STD()->m_NodeArray[NodeIndex].NodeID);
					update.IsCheck = 0x00;
					m_pMapMake_Process->Update(&update);
                }


			}

				ADD_LOG_MAPMAKE("[Update][Driving](%06d~%06d), Dist:%d →%d",
					MapInfo->NodeOffsetInfo[0].StartNodeID,
					MapInfo->NodeOffsetInfo[0].EndNodeID, PreDist,
					OHTMainForm->GetMap()->m_PathArray[PathIndex].Distance);

				strMapLog.sprintf("[Update][Driving](%06d~%06d), Dist:%d →%d",
					MapInfo->NodeOffsetInfo[0].StartNodeID,
					MapInfo->NodeOffsetInfo[0].EndNodeID, PreDist,
					OHTMainForm->GetMap()->m_PathArray[PathIndex].Distance);

				Send_MapMakeResult(strMapLog);
			// Map이 수정됨을 알림
			m_pMapMake_Status->bMapChanged = true;
		}
		else
		{
			ADD_LOG_MAPMAKE("[Update][Error][Not AdjustMap](%06d~%06d)"
			, MapInfo->NodeOffsetInfo[0].StartNodeID,
			MapInfo->NodeOffsetInfo[0].EndNodeID);


			strMapLog.sprintf("[Update][Error][Not AdjustMap](%06d~%06d)"
							, MapInfo->NodeOffsetInfo[0].StartNodeID
							, MapInfo->NodeOffsetInfo[0].EndNodeID);
			Send_MapMakeResult(strMapLog);
		}
	}

	return Error;
}
// ---------------------------------------------------------------------------

/**
 @brief   Map Make한 정보를 Station Data에 업데이트 하는 함수
 @author  zzang9un
 @date    2013.07.23
 @param   MapInfo : 업데이트할 Map Make Info
 @return  정상인 경우 0, 에러 발생 시 에러 코드 리턴
 */
int TaskControl::UpdateStationData(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo) {

	AnsiString strMapLog;
	
	int Error = NO_ERR;
	bool bIsNextNodeZero = false;

	// Step 1. 에러 처리
	if (!OHTMainForm->GetStationDataManager()->m_bDataLoaded) {
		// Station Data가 없는 경우 에러 처리 : 에러 처리하지 않고 주석만 남김
		ADD_LOG_MAPMAKE("[확인필요][DataLoad] No Station Data File");

		Send_MapMakeResult("[확인필요][DataLoad] No Station Data File");
		
		return Error;
	}

	if (MapInfo->StationID == 0) {
		// by zzang9un 2013.07.24 : Station 데이터가 이상한 경우 에러 처리하지 않고 넘어가도록 한다.
		ADD_LOG_MAPMAKE("[확인필요][DataLoad] StationID == 0");

		Send_MapMakeResult("[확인필요][DataLoad] StationID == 0");

		return Error;
	}

	// Step 2. 해당 Station 구조체를 얻어온다.
	STATION_DATA *pSTD = NULL;
	UINT StationID = MapInfo->StationID;

	pSTD = OHTMainForm->GetStationDataManager()->Find(StationID);

	if (pSTD == NULL) {
		// by zzang9un 2013.07.24 : Station 데이터가 이상한 경우 에러 처리하지 않고 넘어가도록 한다.
		ADD_LOG_MAPMAKE(
			"[확인필요][DataLoad] Can't Find Station:%6d, Type:%x, Offset:%d ParentNID:%d NextNID:%d",
			MapInfo->StationID, MapInfo->StationType, MapInfo->NodeOffsetInfo[0].Dist,
			MapInfo->NodeOffsetInfo[0].StartNodeID,MapInfo->NodeOffsetInfo[0].EndNodeID);

		strMapLog.sprintf(
			"[확인필요][DataLoad] Can't Find Station:%6d, Type:%x, Offset:%d ParentNID:%d NextNID:%d",
			MapInfo->StationID, MapInfo->StationType, MapInfo->NodeOffsetInfo[0].Dist,
			MapInfo->NodeOffsetInfo[0].StartNodeID,MapInfo->NodeOffsetInfo[0].EndNodeID);
		Send_MapMakeResult(strMapLog);
		
		return Error;
	}
	else
	{
		bool isExist = false;


		//Map Maker 부모노드에 관련된 변경내용 정리
		int PathIndex = 0;

		if(MapInfo->NodeOffsetInfo[0].EndNodeID ==0)
		{
			//마지막에 정지한경우는 Left 분기측 정보 입력함.
			if(pSTD->ParentNodeID == MapInfo->NodeOffsetInfo[0].StartNodeID)
			{
				MapInfo->NodeOffsetInfo[0].EndNodeID = pSTD->NextNodeID;
            }
			else
			{

				MapInfo->NodeOffsetInfo[0].EndNodeID = m_pPathSearch->GetNextNode(OHTMainForm->GetMap(),MapInfo->NodeOffsetInfo[0].StartNodeID, 0x01);
				ADD_LOG_MAPMAKE("[확인필요][Update]StID:%6d, NewMapData:%06d~%06d, OriginData:%06d~%06d  Offset:%d→%d, IsDefault %d",
				StationID, MapInfo->NodeOffsetInfo[0].StartNodeID, MapInfo->NodeOffsetInfo[0].EndNodeID,
				pSTD->ParentNodeID,	pSTD->NextNodeID,
				pSTD->StationOffset, MapInfo->NodeOffsetInfo[0].Dist,
				pSTD->IsDefaultMap.LOW_DefaultMapInfo);

				strMapLog.sprintf("[확인필요][Update]StID:%6d, NewMapData:%06d~%06d, OriginData:%06d~%06d  Offset:%d→%d, IsDefault %d",
				StationID, MapInfo->NodeOffsetInfo[0].StartNodeID, MapInfo->NodeOffsetInfo[0].EndNodeID,
				pSTD->ParentNodeID,	pSTD->NextNodeID,
				pSTD->StationOffset, MapInfo->NodeOffsetInfo[0].Dist,
				pSTD->IsDefaultMap.LOW_DefaultMapInfo);

				Send_MapMakeResult(strMapLog);
			}
			bIsNextNodeZero = true;
		}

		PathIndex = OHTMainForm->GetMap()->GetPathIndex(MapInfo->NodeOffsetInfo[0].StartNodeID,	MapInfo->NodeOffsetInfo[0].EndNodeID);

		if (PathIndex == INDEX_NONE)
		{
			// by zzang9un 2013.07.24 : Map은 에러를 처리해서 진행이 안되도록 한다.
			Error = ERR_MAPMAKE_NOTFOUNDNODEID;
			ADD_LOG_MAPMAKE("[확인필요][Error][ERR_MAPMAKE_NOTFOUNDNODEID](%06d~%06d)"
				, MapInfo->NodeOffsetInfo[0].StartNodeID,
				MapInfo->NodeOffsetInfo[0].EndNodeID);


			strMapLog.sprintf("[확인필요][Error][ERR_MAPMAKE_NOTFOUNDNODEID](%06d~%06d)"
								, MapInfo->NodeOffsetInfo[0].StartNodeID
								, MapInfo->NodeOffsetInfo[0].EndNodeID);
			Send_MapMakeResult(strMapLog);

			ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
						ERR_MAPMAKE_NOTFOUNDNODEID,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);

		}
		else
		{
			// 찾은 Station Data와 일치하는 MapInfo를 선택한다.
			// 시작 Node와 끝 Node가 일치하고, Station Type이 일치하는 경우만 업데이트 한다.
			if (((pSTD->ParentNodeID == MapInfo->NodeOffsetInfo[0].StartNodeID) && (pSTD->NextNodeID == MapInfo->NodeOffsetInfo[0].EndNodeID))
				||((pSTD->ParentNodeID == MapInfo->NodeOffsetInfo[0].StartNodeID) && (bIsNextNodeZero ==true))
				||(pSTD->ParentNodeID == MapInfo->NodeOffsetInfo[0].EndNodeID)
				||(pSTD->NextNodeID == MapInfo->NodeOffsetInfo[0].StartNodeID))
				{

					int preDist = pSTD->StationOffset;
					int preParentNode = pSTD->ParentNodeID;
					int preStationType = pSTD->StationType;
					int preNextNodeID = pSTD->NextNodeID;
                   
					// Station Manager의 Update 함수를 사용하기 위한 작업
					STATION_DATA UpdateSTD;

					memset(&UpdateSTD, 0x00, sizeof(STATION_DATA));

					UpdateSTD.StationID = MapInfo->StationID;
					UpdateSTD.StationType = MapInfo->StationType;
					UpdateSTD.ParentNodeID = MapInfo->NodeOffsetInfo[0].StartNodeID;
					UpdateSTD.NextNodeID = MapInfo->NodeOffsetInfo[0].EndNodeID;
					UpdateSTD.StationOffset = MapInfo->NodeOffsetInfo[0].Dist;
					UpdateSTD.Safety_Level = pSTD->Safety_Level;
					UpdateSTD.IsDefaultMap.LOW_DefaultMapInfo = 1;



					// Station Data Update
					Error = OHTMainForm->GetStationDataManager()->Update(&UpdateSTD);

					if (Error)
					{
						// by zzang9un 2013.07.24 : 업데이트 실패 시 에러 처리 하지 않고 로그만 생성
						ADD_LOG_MAPMAKE("[확인필요][Error]UpdateFailErrorcode:%d,Update Fail Station:%6d"
							, Error, UpdateSTD.StationID);

						strMapLog.sprintf("[확인필요][Error]UpdateFailErrorcode:%d,Update Fail Station:%6d"
											, Error, UpdateSTD.StationID);
						Send_MapMakeResult(strMapLog);

						Error = NO_ERR;
					}
					else
					{
						ADD_LOG_MAPMAKE("[Update][Station] Station:%6d, Type:%x→%x, Offset:%d →%d ParentNID:%d →%d NextNID:%d →%d",
						UpdateSTD.StationID, preStationType, UpdateSTD.StationType, preDist,
						UpdateSTD.StationOffset,preParentNode,MapInfo->NodeOffsetInfo[0].StartNodeID, preNextNodeID,MapInfo->NodeOffsetInfo[0].EndNodeID);

						strMapLog.sprintf("[Update][Station] Station:%6d, Type:%x→%x, Offset:%d →%d ParentNID:%d →%d NextNID:%d →%d",
						UpdateSTD.StationID, preStationType, UpdateSTD.StationType, preDist,
						UpdateSTD.StationOffset,preParentNode,MapInfo->NodeOffsetInfo[0].StartNodeID, preNextNodeID,MapInfo->NodeOffsetInfo[0].EndNodeID);
						Send_MapMakeResult(strMapLog);
					}

					isExist = true;

				}
		}





		if (!isExist) {
			// by zzang9un 2013.09.13 : 소속이 달라서 업데이트를 못하는 경우 로그를 생성한다.
			ADD_LOG_MAPMAKE(
				"[확인필요][Error] Not Match ParentNID, StID:%6d, NewMapData:%06d~%06d, OriginData:%06d~%06d  Offset:%d→%d, IsDefault %d",
				StationID, MapInfo->NodeOffsetInfo[0].StartNodeID, MapInfo->NodeOffsetInfo[0].EndNodeID,
				pSTD->ParentNodeID,	pSTD->NextNodeID,
				pSTD->StationOffset, MapInfo->NodeOffsetInfo[0].Dist,
				pSTD->IsDefaultMap.LOW_DefaultMapInfo);

			strMapLog.sprintf(
				"[확인필요][Error] Not Match ParentNID, StID:%6d, NewMapData:%06d~%06d, OriginData:%06d~%06d  Offset:%d→%d, IsDefault %d",
				StationID, MapInfo->NodeOffsetInfo[0].StartNodeID, MapInfo->NodeOffsetInfo[0].EndNodeID,
				pSTD->ParentNodeID,	pSTD->NextNodeID,
				pSTD->StationOffset, MapInfo->NodeOffsetInfo[0].Dist,
				pSTD->IsDefaultMap.LOW_DefaultMapInfo);

			Send_MapMakeResult(strMapLog);

		}
		else
		{
			//정상적으로 업데이트 된경우 map Maker File측 정보 갱신
			 if(OHTMainForm->GetMapMakerFileManager_STD()->IsMapMakeFileLoading())
			 {
				OHTMainForm->GetMapMakerFileManager_STD()->SucessesFindUpdate('S',MapInfo->StationID);
			 }
		}



	}

	return Error;
}
// ---------------------------------------------------------------------------

/**
 @brief   Map Make 정보를 처리하는 함수
 @author  zzang9un
 @date    2013.07.24
 @return  정상인 경우 0, 에러 발생 시 에러 코드 리턴
 */
int TaskControl::HandleMapMake() {
	int Error = NO_ERR;

	AnsiString strMapLog;

	// by zzang9un 2013.07.22
	// Map Make 정보 갱신을 위해 Map Make 여부를 체크한다(두개 flag 사용)
	if (m_pExecuteInfo->OnAddingMapInfo.Info.FlagOnMapMaking ||
		m_pExecuteInfo->OnAddingMapInfo.Info.FlagExecuteThreadUsing ||
		(m_pExecuteInfo->ListOfMapInfo.getCount() > 0)) {
		// Map Make 수행 중인 경우

		// Map Make Data를 List에서 꺼내온다.
		// Data가 Backup 중이 아닐 경우에만 수행한다.
		if (!m_pMapMake_Status->bStartBackup_Map ||
			!m_pMapMake_Status->bStartBackup_Station)
			{

			EXECUTE_RESPONSE_MAPMAKING_INFO *pData = NULL;
			pData = (EXECUTE_RESPONSE_MAPMAKING_INFO*)m_pExecuteInfo->ListOfMapInfo.popFront();


			if (pData) {
				// by zzang9un 2013.09.09 : 로그 생성을 주행, Station에 따라 구분
				if ((pData->StationID == 0) &&
					(pData->StationType == MARK_TYPE_NODE)) {
					ADD_LOG_MAPMAKE(
						"[PopList][Driving]Start:%06d, End:%06d, Dist:%d, NID:%06d, PointX:%d, PointY:%d",
						pData->NodeOffsetInfo[0].StartNodeID,
						pData->NodeOffsetInfo[0].EndNodeID,
						pData->NodeOffsetInfo[0].Dist, pData->NodeDataInfo.NID,
						pData->NodeDataInfo.Point_X,
						pData->NodeDataInfo.Point_Y);

				}
				else {
					ADD_LOG_MAPMAKE(
						"[PopList][Station]StID:%06d, SType:%X, ParentNID:%06d, NextNID:%06d, Dist:%d",
						pData->StationID, pData->StationType,
						pData->NodeOffsetInfo[0].StartNodeID,
						pData->NodeOffsetInfo[0].EndNodeID,
						pData->NodeOffsetInfo[0].Dist);

				}

				// TP에 Map Make 정보를 전송한다.
//				Error = SendMapMakeInfoOHT2TP(pData);

				// Map Make 정보를 업데이트 한다.
				Error = UpdateMapMakeInfo(pData);

               delete pData;
			}
		}
	}

	return Error;
}
// ---------------------------------------------------------------------------

/**
 @brief   특정 Station ID의 Station Data를 얻어오는 함수
 @author  zzang9un
 @date    2013.08.19
 @param   StationID : Station ID
 @param   pStationData : Find한 STATION_DATA 구조체가 저장될 구조체 포인터
 @return  정상인 경우 0, 에러 발생 시 에러 코드 리턴
 */
int TaskControl::GetStationData(STATION_DATA *pStationData, UINT StationID) {
	int Error = NO_ERR;

	STATION_DATA *pSData;

	pSData = OHTMainForm->GetStationDataManager()->Find(StationID);

	if (pSData == NULL) {
		Error = ERR_DATA_STATION_NOEXIST;
		ADD_LOG_TASKCONTROL(
			"[GetStationData] ERR_DATA_STATION_NOEXIST, StationID:%06d",
			StationID);
	}
	else {
		memcpy(pStationData, pSData, sizeof(STATION_DATA));
	}

	return Error;
}


// ---------------------------------------------------------------------------

/**
 @brief   HandTP의 Com통신을 Open하는 함수
 @author  ji7412.lee
 @date    2013.05.27
 */
int TaskControl::ComOpenForRemocon() {
	int nError = NO_ERR;
	String strLog = NULL;
	m_pRemocon = NULL;

	m_pRemocon = OHTMainForm->GetHWSetRemocon();
	if (m_pRemocon != NULL) {
		nError = m_pRemocon->OpenCom();
		if (nError == NO_ERR) {
			bOpenRemocon = true;
			ADD_LOG_TASKCONTROL("[ComOpenForRemocon]Remocon Open");
		}
		else {
			bOpenRemocon = false;
			ADD_LOG_TASKCONTROL("[ComOpenForRemocon]Remocon NotOpen");
		}
	}
	else
		nError = -1;

	return nError;
}
// ---------------------------------------------------------------------------

/**
 @brief   HandTP의 Data를 Read하는 함수
 @author  ji7412.lee
 @date    2013.05.27
 @note    HandTP 테스트 목적을 위한 function : 추후 Parsing,TLV 형태로 변경할것
 */
int TaskControl::GetReadRemoconData() {
	int Error = NO_ERR;
	String strLog = NULL;
	unsigned char szReadBuff[1024];
	char cAckStatus[30] = {0, };

    static int nOHTType = 0;
    static DEFAULT_PARAM_SET *m_defualtparam = NULL;

    if(m_defualtparam == NULL)
    {
        char cTemp[6] = {0,};
        m_defualtparam = OHTMainForm->GetDefaultParam();
#if ((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S))
        memmove(cTemp,&m_defualtparam->VHLName[3],3);//OHT001
        nOHTType = atoi(cTemp);
#elif  ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
        memmove(cTemp,&m_defualtparam->VHLName[4],2); // ROHT01
        nOHTType = atoi(cTemp);
#endif
	}

	memset(szReadBuff, 0x00, sizeof(szReadBuff));
	memset(m_nInputData, 0x00, sizeof(m_nInputData));
	uInput.Data = 0x0000000000000000;

	if (bOpenRemocon) {
		try {
			if (m_pRemocon != NULL)
			{
				Error = m_pRemocon->GetID(szReadBuff);
				if(szReadBuff[0] !=0 )
				{
				ADD_LOG_TASKCONTROL(
					"[Remocon]:%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x/%x result : %d",
					szReadBuff[0], szReadBuff[1], szReadBuff[2], szReadBuff[3],
					szReadBuff[4], szReadBuff[5], szReadBuff[6], szReadBuff[7],
					szReadBuff[8], szReadBuff[9], szReadBuff[10],szReadBuff[11],
					szReadBuff[12],szReadBuff[13],szReadBuff[14],szReadBuff[15],Error);
				}
			}
			if (Error == NO_ERR) {
				Analysis_IOData();
				Error = MakeCmdList_HandTP(szReadBuff, true,	uInput);
			}
			else if (Error == ERR_HANDTP_REMOTE_TYPE) {
				// data 미수신
				//sprintf(cAckStatus, "GETID(E:%d)", Error);
			}
			else if (Error == ERR_HANDTP_READ_LENGTH)
			{
				ADD_LOG_TASKCONTROL("[Remocon]:Remocon ReadError[%d]", Error);
				//sprintf(cAckStatus, "GETID(E:%d)", Error);
			}
            else if (Error == ERR_HANDTP_NO_DATA)
            {
                if(GetRotateJogCmd() == true)
                {
					if(fabs(OHTUTILITY::PreventOverflowGetTimeMs(GetRotateJogMoveStartTime())) >= 500)
					{
						szReadBuff[0] = REMOTE_TYPE_RF;
						szReadBuff[1] = CYCLE_CMD;
						szReadBuff[5] = (CYC_JOG_ROTATE_STOP >> 8) & 0xFF;
                        szReadBuff[6] = CYC_JOG_ROTATE_STOP & 0xFF;

#if ((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S))
                        szReadBuff[3] = (nOHTType >> 8) & 0xFF;
                        szReadBuff[4] = nOHTType & 0xFF;
                        szReadBuff[2] = OHT_FOUP;
#elif  ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
                        szReadBuff[3] = (nOHTType >> 8) & 0xFF;
                        szReadBuff[4] = nOHTType & 0xFF;
						szReadBuff[2] = OHT_RETICLE;
#endif
                        SetRotateJogCmd(false);
                        Error = MakeCmdList_HandTP(szReadBuff, true,	uInput);
                    }
                }

                // 2016-12-25, Add by Labeler
                // Longrun 동작을 위해 Logic 추가
                if((GetHandyTPLongRun() != HANDTP_LONGRUN_NONE) && (m_pCmdManager->Is_Executable() == NO_ERR))
				{
#if ((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S))
                    szReadBuff[3] = (nOHTType >> 8) & 0xFF;
					szReadBuff[4] = nOHTType & 0xFF;
                    szReadBuff[2] = OHT_FOUP;
#elif  ((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
                    szReadBuff[3] = (nOHTType >> 8) & 0xFF;
                    szReadBuff[4] = nOHTType & 0xFF;
                    szReadBuff[2] = OHT_RETICLE;
#endif
                    MakeLongrunCmdForHandyTP(szReadBuff, uInput);
                }
            }
		}
		catch (Exception &e) {
			ADD_LOG_TASKCONTROL("[Remocon]:Remocon Exception[%s]", e.ToString());
		}
	}
	return Error;
}
// ---------------------------------------------------------------------------

/**
 @brief   IO Map 파일을 로딩하는 함수
 @author  doori.shin
 @date    2013.08.20
 @param   OHTType : OHT Type(FOUP, MAC, ...)
 @param   OHTVer : OHT Version(v7.0, ...)
 */
int TaskControl::LoadIOMapField(String OHTType, String OHTVer) {
	// 2013.08.14 IO Map 파일 읽기
	AnsiString strInSection = NULL; // INI 파일 Input Section 값
	AnsiString strOutSection = NULL; // INI 파일 Output Section 값

	int nInput = 0; // Input 개수
	int nOutput = 0; // Output 개수

    m_strIOVersion = "00";
	m_IOMapField->fIsLoaded = false;

	// Step 1. INI 파일 Load
	m_IOMapIni = new TIniFile(strIOMapPath);

	// Step 2. Ini파일 Section 값 설정
	strInSection = OHTType + L"_" + OHTVer + L"_" + L"INPUT";
	strOutSection = OHTType + L"_" + OHTVer + L"_" + L"OUTPUT";

	// by zzang9un 2013.03.11 : 에러 유무 추가 확인
	// Step 2.5 Ini파일 Section 유무 확인

	if (m_IOMapIni->SectionExists(strInSection) == false) {
		// Input Section이 없는 경우 경고창
		ADD_LOG_TASKCONTROL("[LoadIOMapField] [%s] Section이 없습니다.",
			strInSection.c_str());
		return 0;
	}
	else if (m_IOMapIni->SectionExists(strOutSection) == false) {
		// Output Section이 없는 경우 경고창
		ADD_LOG_TASKCONTROL("[LoadIOMapField] [%s] Section이 없습니다.",
			strOutSection.c_str());
		return 0;
	}

	// Step 3. IO 개수 확인 후 Grid에 반영
	nInput = m_IOMapIni->ReadInteger(strInSection, "Num", -1);
	nOutput = m_IOMapIni->ReadInteger(strOutSection, "Num", -1);

	ADD_LOG_TASKCONTROL("[LoadIOMapField] nInput : %d, nOutput : %d", nInput,
		nOutput);

	if (nInput != nOutput) {
		// Input과 Output 개수가 틀린 경우
		ADD_LOG_TASKCONTROL("[LoadIOMapField] Input과 Ouput의 개수가 다릅니다.");
		return 0;
	}

	m_strIOVersion = m_IOMapIni->ReadString("Version", "IniVersion", "00");


	Sleep(1000);

	// 데이터 갯수 삽입 및 배열 초기화
	m_IOMapField->InputNum = nInput;
	m_IOMapField->OutputNum = nOutput;

	m_IOMapField->InputField = new AnsiString[nInput];
	m_IOMapField->OutputField = new AnsiString[nOutput];

	String strKey = NULL;

	// Read Input
	for (int i = 0; i < nInput; i++) {
		strKey.sprintf(L"INPUT%02d", (i + 1));

		String strTmp;
		strTmp = m_IOMapIni->ReadString(strInSection, strKey, "None");
		m_IOMapField->InputField[i] = m_IOMapIni->ReadString(strInSection,
			strKey, "None");
	}

	// Read Output
	for (int i = 0; i < nOutput; i++) {
		strKey.sprintf(L"OUTPUT%02d", (i + 1));

		String strTmp;
		strTmp = m_IOMapIni->ReadString(strInSection, strKey, "None");
		m_IOMapField->OutputField[i] = m_IOMapIni->ReadString(strOutSection,
			strKey, "None");
	}

	return 1;
}

// ---------------------------------------------------------------------------
/**
 @brief   Remocon AckData를 만들기 위한 함수
 @author  ji7412.lee
 @date    2013.08.20
 @param   OHTType : OHT Type(FOUP, MAC, ...)
 @param   OHTVer : OHT Version(v7.0, ...)
 */
int TaskControl::MakeAckDataRemocon(char* szReadBuff, int ExecuteError) {
	int Error = 0;
	int nCmdData = 0;
	int nPosition = 0;
	char cAckStatus[40] = {0, };
	char cErrorCode[6] = {0, };
	char cCurNodeID[7] = {0, };
	char cStationID[7] = {0, };
	uHTagData uhtagData;
	int nRecvBufLength = sizeof(szReadBuff);

	if (nRecvBufLength < HANDTP_DATA_MIN_LENGTH)
		return HANDTP_ERR_RECV_DATA_ERROR;

	memset(&uhtagData, 0x00, sizeof(uhtagData));
	uhtagData.Htag_Data.SenderReceiver = 0;
	uhtagData.Htag_Data.ProtoCmdType = 0;
	uhtagData.Htag_Data.CmdData1 = szReadBuff[2];
	uhtagData.Htag_Data.CmdData2 = szReadBuff[3];
	nCmdData = uhtagData.nWholeTagData;

	if (nCmdData == TPCMD_OHT_HANDTP_STATUS) {
		if (m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO)
			cAckStatus[nPosition] = 'A';
		else if (m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode ==
			MODE_MANUAL)
			cAckStatus[nPosition] = 'M';
		else
			cAckStatus[nPosition] = '-';

		nPosition++;
		cAckStatus[nPosition] = '/';
		nPosition++;

		switch (m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status) {
		case STS_INIT:
			cAckStatus[nPosition] = 'I';
			break;
		case STS_GOING:
			cAckStatus[nPosition] = 'G';
			break;
		case STS_ARRIVED:
			cAckStatus[nPosition] = 'A';
			break;
		case STS_UNLOADING:
			cAckStatus[nPosition] = 'U';
			break;
		case STS_UNLOADED:
			cAckStatus[nPosition] = 'N';
			break;
		case STS_LOADING:
			cAckStatus[nPosition] = 'L';
			break;
		case STS_LOADED:
			cAckStatus[nPosition] = 'O';
			break;
		case STS_AUTOPOSITION:
			cAckStatus[nPosition] = 'P';
			break;
		case STS_RUNNING:
			cAckStatus[nPosition] = 'R';
			break;
		case STS_COMPLETE:
			cAckStatus[nPosition] = 'C';
			break;
		case STS_ERROR:
			cAckStatus[nPosition] = 'E';
			break;
		case STS_AUTO_TEACHING:
		case STS_AUTO_TEACHING_NULLTYPE:
			cAckStatus[nPosition] = 'T';
			break;
		case STS_AUTO_TAUGHT:
			cAckStatus[nPosition] = 'H';
			break;
		case STS_AUTO_TRANS_HOMING:
			cAckStatus[nPosition] = 'O';
			break;
		default:
			cAckStatus[nPosition] = 'F';
			break;
		}
		nPosition++;
		cAckStatus[nPosition] = '/';
		nPosition++;

		sprintf(cErrorCode, "E%04d", GetOHTError());
		memcpy(cAckStatus + nPosition, cErrorCode, sizeof(cErrorCode));

		nPosition += sizeof(cErrorCode);

		cAckStatus[nPosition] = '/';
		nPosition++;
		if (uInput.Info.STEER_F_Left == ON && uInput.Info.STEER_R_Left == ON)
			cAckStatus[nPosition] = 'L';
		else if (uInput.Info.STEER_F_Right == ON &&
			uInput.Info.STEER_R_Right == ON)
			cAckStatus[nPosition] = 'R';
		else
			cAckStatus[nPosition] = 'N';
		nPosition++;

		cAckStatus[nPosition] = '*';
		nPosition++;

		sprintf(cCurNodeID, "%6x", m_pOHTMainStatus->StatusCommon.CurNodeID);
		memcpy(cAckStatus + nPosition, cCurNodeID, sizeof(cCurNodeID));
		nPosition += sizeof(cCurNodeID);
		cAckStatus[nPosition] = '/';
		nPosition++;
		sprintf(cStationID, "%6x", m_pOHTMainStatus->StatusCommon.CurStationID);
		memcpy(cAckStatus + nPosition, cStationID, sizeof(cStationID));
	}
	else if (ExecuteError == NO_ERR) {
		sprintf(cAckStatus, "ACK OK(%d)", sizeof(szReadBuff));
	}
	else {

/*
		// temporary Define
		if (ExecuteError == HANDTP_OHT_MODE_AUTO)
			sprintf(cAckStatus, "NG:MODE AUTO");
		if (ExecuteError == HANDTP_OHT_LOADING)
			sprintf(cAckStatus, "NG:LOADING");
		if (ExecuteError == HANDTP_OHT_UNLOADING)
			sprintf(cAckStatus, "NG:UNLOADING");
		if (ExecuteError == HANDTP_OHT_GOING)
			sprintf(cAckStatus, "NG:GOING");
		if (ExecuteError == HANDTP_OHT_DRV_F_RUNNING)
			sprintf(cAckStatus, "NG:DRV_F_RUNNING");
		if (ExecuteError == HANDTP_OHT_DRV_R_RUNNING)
			sprintf(cAckStatus, "NG:DRV_R_RUNNING");
		if (ExecuteError == HANDTP_OHT_HOIST_RUNNING)
			sprintf(cAckStatus, "NG:HOIST_RUNNING");
		if (ExecuteError == HANDTP_OHT_SHIFT_RUNNING)
			sprintf(cAckStatus, "NG:SHIFT_RUNNING");
		if (ExecuteError == HANDTP_OHT_HOIST_LIMIT)
			sprintf(cAckStatus, "NG:HOIST_LIMIT");
		if (ExecuteError == HANDTP_OHT_SHIFT_LIMIT)
			sprintf(cAckStatus, "NG:SHIFT_LIMIT");
		if (ExecuteError == HANDTP_OHT_RAIL_NOT_HOME)
			sprintf(cAckStatus, "NG:RAIL_NOT_HOME");
		if (ExecuteError == HANDTP_OHT_DRV_RUNNING)
			sprintf(cAckStatus, "NG:DRV_RUNNING");
		if (ExecuteError == HANDTP_OHT_SHUTTER_OPEN)
			sprintf(cAckStatus, "NG:SHUTTER_OPEN");
		if (ExecuteError == HANDTP_OHT_OSCILL_OFF)
			sprintf(cAckStatus, "NG:OSCILL_OFF");
		if (ExecuteError == HANDTP_OHT_HAND_LIMIT_SENSOR)
			sprintf(cAckStatus, "NG:HAN_LIMIT_SEN");
		if (ExecuteError == HANDTP_OHT_SHUTTER_CLOSE)
			sprintf(cAckStatus, "NG:SHUTTER_CLOSE");
		if (ExecuteError == HANDTP_OHT_HAND_GRIP_ON)
			sprintf(cAckStatus, "NG:HAND_GRIP_ON");
		if (ExecuteError == HANDTP_OHT_HOIST_NOT_ORIGIN)
			sprintf(cAckStatus, "NG:H_NOT_ORIGIN");
		if (ExecuteError == HANDTP_OHT_SHIFT_NOT_ORIGIN)
			sprintf(cAckStatus, "NG:S_NOT_ORIGIN");
		if (ExecuteError == HANDTP_OHT_FOUP_DETECT)
			sprintf(cAckStatus, "NG:FOUP_DETECT");
		if (ExecuteError == HANDTP_OHT_MOTOR_ERROR)
			sprintf(cAckStatus, "NG:MOTOR_ERROR");
		if (ExecuteError == HANDTP_OHT_NOT_INIT)
			sprintf(cAckStatus, "NG:NOT_INIT");
		if (ExecuteError == HANDTP_OHT_NO_MARK_INFO)
			sprintf(cAckStatus, "NG:NO_MARK_INFO");
		if (ExecuteError == HANDTP_OHT_BEFORE_AUTO)
			sprintf(cAckStatus, "NG:BEFORE_AUTO");
		if (ExecuteError == HANDTP_OHT_NO_NODE_ID)
			sprintf(cAckStatus, "NG:NO_NODEID");
		if (ExecuteError == HANDTP_RECV_DATA_ERROR)
			sprintf(cAckStatus, "NG:RECV_DATA_ERR");
*/
	}

	ADD_LOG_TASKCONTROL("[MakeAckDataRemocon] ACKData[%s] Error[%d] ",
		cAckStatus, Error);
//	if (m_pRemocon != NULL)
//		Error = m_pRemocon->SendTriger(cAckStatus, strlen(cAckStatus));

	return 0;
}

/**
 @brief   OHT로부터 받은 IO Data를 Analysis하는 함수
 @author  puting
 @date    2012.12.14
 */
void TaskControl::Analysis_IOData() {
	UINT nInput[3] = {0, }, nOutput[3] = {0, };

	for (int i = 0; i < 3; i++) {
		// by zzang9un 2013.03.19 : IO Data 가져오기
		nInput[i] = m_pOHTMainStatus->StatusTP.Input[i];
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 32; j++) {
			m_nInputData[i][j] = ((nInput[i] >> j) & 0x01) ? 0x01 : 0x00;
		}
	}
	// by zzang9un 2012.11.26 : Iunput의 경우 bit filed 구조체에 반영함
	UINT64 InputData = nInput[1];
	InputData = (InputData << 32) | nInput[0];
	uInput.Data = InputData;
}

bool TaskControl::ConvertBigLitte(PKT_ACK_OHT2OCS *Temp)
{
	unsigned short Tagid = Temp->AckPkt.TAG.Value;
     //OHT ->OCS에 전달할 경우
	switch(Tagid)
	{

		case TagID::CMD_STATION_INFO_REQUST_OHT2OCS:   //추가
			PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111 Tmep111;
			memmove(&Tmep111,&Temp->AckPkt, sizeof(PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111));
			Conv_PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111(Tmep111);
			memmove(&Temp->AckPkt,&Tmep111, sizeof(PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111));
		break;

		case TagID::JOB_ORDER_RESPONSE:
			PKT_JOB_ORDER_RESPONSE_ID_122 Tmep122;
			memmove(&Tmep122,&Temp->AckPkt, sizeof(PKT_JOB_ORDER_RESPONSE_ID_122));
			Conv_PKT_JOB_ORDER_RESPONSE_ID_122(Tmep122);
			memmove(&Temp->AckPkt,&Tmep122, sizeof(PKT_JOB_ORDER_RESPONSE_ID_122));
		break;
		case TagID::CLW_TO_MCP_STATUS:
			PKT_STATUS_REPORT_ID_03 Tmep3;
			memmove(&Tmep3,&Temp->AckPkt, sizeof(PKT_STATUS_REPORT_ID_03));
			Conv_PKT_STATUS_REPORT_ID_03(Tmep3);
			memmove(&Temp->AckPkt,&Tmep3, sizeof(PKT_STATUS_REPORT_ID_03));
		break;
		case TagID::CMD_TAKEOUTREQ_OHT_OCS:
			PKT_TAKEOUT_REPORT_ID_17 Tmep17;
			memmove(&Tmep17,&Temp->AckPkt, sizeof(PKT_TAKEOUT_REPORT_ID_17));
			Conv_PKT_TAKEOUT_REPORT_ID_17(Tmep17);
			memmove(&Temp->AckPkt,&Tmep17, sizeof(PKT_TAKEOUT_REPORT_ID_17));
		break;

		case TagID::JOB_PROGRESS_REPORT:
			PKT_JOB_PROGRESS_REPORT_ID_103 Tmep103;
			memmove(&Tmep103,&Temp->AckPkt, sizeof(PKT_JOB_PROGRESS_REPORT_ID_103));
			Conv_PKT_JOB_PROGRESS_REPORT_ID_103(Tmep103);
			memmove(&Temp->AckPkt,&Tmep103, sizeof(PKT_JOB_PROGRESS_REPORT_ID_103));
		break;
		case TagID::CMD_PASSPERMITOPEN_OHT2OCS:
			PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243 Tmep243;
			memmove(&Tmep243,&Temp->AckPkt, sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243));
			Conv_PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243(Tmep243);
			memmove(&Temp->AckPkt,&Tmep243, sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243));
		break;
		case TagID::CMD_PASSPERMIT_OHT2OCS:
			PKT_PASSPERMIT_OHT2OCS_ID_241 Tmep241;
			memmove(&Tmep241,&Temp->AckPkt, sizeof(PKT_PASSPERMIT_OHT2OCS_ID_241));
			Conv_PKT_PASSPERMIT_OHT2OCS_ID_241(Tmep241);
			memmove(&Temp->AckPkt,&Tmep241, sizeof(PKT_PASSPERMIT_OHT2OCS_ID_241));
		break;
		case TagID::RESPONSE_PASSPERMITREQ_OHT2OCS:
			PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246 Tmep246;
			memmove(&Tmep246,&Temp->AckPkt, sizeof(PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246));
			Conv_PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246(Tmep246);
			memmove(&Temp->AckPkt,&Tmep246, sizeof(PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246));
		break;
		case TagID::REQ_DATATRANSFER_OHT2OCS:
			PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS TmepDataReq;
			memmove(&TmepDataReq,&Temp->AckPkt, sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS));
			Conv_PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS(TmepDataReq);
			memmove(&Temp->AckPkt,&TmepDataReq, sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS));
		break;
		case TagID::CMD_INSERTREQ_OHT_OCS:
			PKT_OHT_INPUT_REQUEST TmepInsert;
			memmove(&TmepInsert,&Temp->AckPkt, sizeof(PKT_OHT_INPUT_REQUEST));
			Conv_PKT_OHT_INPUT_REQUEST(TmepInsert);
			memmove(&Temp->AckPkt,&TmepInsert, sizeof(PKT_OHT_INPUT_REQUEST));
		break;

		case TagID::CMD_DEF_REQ_OHT_OCS:
			PKT_DEF_REQ_ID_19  Tmep19;
			memmove(&Tmep19,&Temp->AckPkt, sizeof(PKT_DEF_REQ_ID_19));
			Conv_PKT_DEF_REQ_ID_19(Tmep19);
			memmove(&Temp->AckPkt,&Tmep19, sizeof(PKT_DEF_REQ_ID_19));
			break;

		case TagID::RESPONSE_VERSIONREQ_OHT_OCS:
			PKT_VERSION_RESPONSE TmepVer;
			memmove(&TmepVer,&Temp->AckPkt, sizeof(PKT_VERSION_RESPONSE));
			Conv_PKT_VERSION_RESPONSE(TmepVer);
			memmove(&Temp->AckPkt,&TmepVer, sizeof(PKT_VERSION_RESPONSE));
		break;
		case TagID::RESPONSE_DATATRANSFER_OHT2OCS:
			PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE TmepDataResponse;
			memmove(&TmepDataResponse,&Temp->AckPkt, sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE));
			Conv_PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE(TmepDataResponse);
			memmove(&Temp->AckPkt,&TmepDataResponse, sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE));
		break;
		case TagID::REPORT_DATATRANSFERPROGRESS_OHT2OCS:
			PKT_DATA_TRANSPER_ORDER_PROGRESS TmepData;
			memmove(&TmepData,&Temp->AckPkt, sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS));
			Conv_PKT_DATA_TRANSPER_ORDER_PROGRESS(TmepData);
			memmove(&Temp->AckPkt,&TmepData, sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS));
		break;
		case TagID::CMD_PASSPERMITFORCEOPEN_OHT2OCS:
			PKT_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247 Tmep247;
			memmove(&Tmep247,&Temp->AckPkt, sizeof(PKT_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247));
			Conv_PKT_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247(Tmep247);
			memmove(&Temp->AckPkt,&Tmep247, sizeof(PKT_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247));
		break;
		case TagID::CID_ERROR_REPORT_OHT2OCS:
			PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249 Tmep249;
			memmove(&Tmep249,&Temp->AckPkt, sizeof(PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249));
			Conv_PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249(Tmep249);
			memmove(&Temp->AckPkt,&Tmep249, sizeof(PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249));
		break;
		case TagID::RESPONSE_FLOORMOVE_OHT2OCS:
			PKT_FLOOR_MOVE_ORDER_RESPONSE TmepFloor;
			memmove(&TmepFloor,&Temp->AckPkt, sizeof(PKT_FLOOR_MOVE_ORDER_RESPONSE));
			Conv_PKT_FLOOR_MOVE_ORDER_RESPONSE(TmepFloor);
			memmove(&Temp->AckPkt,&TmepFloor, sizeof(PKT_FLOOR_MOVE_ORDER_RESPONSE));
		break;
		case TagID::RESPONSE_SIMPLE_VERSION_OHT_OCS:
			PKT_SIMPLE_VERSION_RESPONSE TmepVersion;
			memmove(&TmepVersion,&Temp->AckPkt, sizeof(PKT_SIMPLE_VERSION_RESPONSE));
			Conv_PKT_SIMPLE_VERSION_RESPONSE(TmepVersion);
			memmove(&Temp->AckPkt,&TmepVersion, sizeof(PKT_SIMPLE_VERSION_RESPONSE));
		break;
		case TagID::RESPONSE_AUTOCHANGE_OCS_OHT:     //AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
			PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252 TmepID252;
			memmove(&TmepID252,&Temp->AckPkt, sizeof(PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252));
			Conv_PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252(TmepID252);
			memmove(&Temp->AckPkt,&TmepID252, sizeof(PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252));
		break;

	}
}

bool TaskControl::ConvertBigLitteDIAG(PKT_ACK_OHT2DIAG *Temp)
{
	unsigned short Tagid = Temp->AckPkt.HEADER.CommandID;
     //OHT ->OCS에 전달할 경우
	switch(Tagid)
	{
		case DiagCmdID::CLW_TO_AGENT_CONNECT_CHK_RESPONSE:
			DIAG_PKT_CONNECT_CHK_RESPONSE Tmep01;
			memmove(&Tmep01,&Temp->AckPkt, sizeof(DIAG_PKT_CONNECT_CHK_RESPONSE));
			Conv_DIAG_PKT_CONNECT_CHK_RESPONSE(Tmep01);
			memmove(&Temp->AckPkt,&Tmep01, sizeof(DIAG_PKT_CONNECT_CHK_RESPONSE));
		break;
		case DiagCmdID::DIAG_INFO_DATA_RESPONSE:
			DIAG_PKT_INFO_DATA_RESPONSE Tmep02;
			memmove(&Tmep02,&Temp->AckPkt, sizeof(DIAG_PKT_INFO_DATA_RESPONSE));
			Conv_DIAG_PKT_INFO_DATA_RESPONSE(Tmep02);
			memmove(&Temp->AckPkt,&Tmep02, sizeof(DIAG_PKT_INFO_DATA_RESPONSE));
		break;
		case DiagCmdID::DIAG_DATA_RESET_RESPONSE:
			DIAG_PKT_DATA_RESET_RESPONSE Tmep03;
			memmove(&Tmep03,&Temp->AckPkt, sizeof(DIAG_PKT_DATA_RESET_RESPONSE));
			Conv_DIAG_PKT_DATA_RESET_RESPONSE(Tmep03);
			memmove(&Temp->AckPkt,&Tmep03, sizeof(DIAG_PKT_DATA_RESET_RESPONSE));
		break;
		case DiagCmdID::DIAG_INFO_LEVEL_CHANGE_RESPONSE:
			DIAG_PKT_LEVEL_CHANGE_RESPONSE Tmep04;
			memmove(&Tmep04,&Temp->AckPkt, sizeof(DIAG_PKT_LEVEL_CHANGE_RESPONSE));
			Conv_DIAG_PKT_LEVEL_CHANGE_RESPONSE(Tmep04);
			memmove(&Temp->AckPkt,&Tmep04, sizeof(DIAG_PKT_LEVEL_CHANGE_RESPONSE));
		break;
		case DiagCmdID::DIAG_INFO_STATUS_REPORT:
			DIAG_PKT_INFO_STATUS_REPORT Tmep05;
			memmove(&Tmep05,&Temp->AckPkt, sizeof(DIAG_PKT_INFO_STATUS_REPORT));
			Conv_DIAG_PKT_INFO_STATUS_REPORT(Tmep05);
			memmove(&Temp->AckPkt,&Tmep05, sizeof(DIAG_PKT_INFO_STATUS_REPORT));
		break;
		case DiagCmdID::DIAG_INFO_ERROR_REPORT:
			DIAG_PKT_ERROR_REPORT Tmep06;
			memmove(&Tmep06,&Temp->AckPkt, sizeof(DIAG_PKT_ERROR_REPORT));
			Conv_DIAG_PKT_ERROR_REPORT(Tmep06);
			memmove(&Temp->AckPkt,&Tmep06, sizeof(DIAG_PKT_ERROR_REPORT));
		break;
		case DiagCmdID::DIAG_INFO_STATISTICS_REQUEST:
			DIAG_PKT_DATA_INFO_REQUEST_ Tmep07;
			memmove(&Tmep07,&Temp->AckPkt, sizeof(DIAG_PKT_DATA_INFO_REQUEST_));
			Conv_DIAG_PKT_DATA_INFO_REQUEST(Tmep07);
			memmove(&Temp->AckPkt,&Tmep07, sizeof(DIAG_PKT_DATA_INFO_REQUEST_));
		break;
		case DiagCmdID::DIAG_INFO_CIDMONITORING_REPORT:
			DIAG_PKT_CID_REPORT Tmep08;
			memmove(&Tmep08,&Temp->AckPkt, sizeof(DIAG_PKT_CID_REPORT));
			Conv_DIAG_PKT_CID_REPORT(Tmep08);
			memmove(&Temp->AckPkt,&Tmep08, sizeof(DIAG_PKT_CID_REPORT));
		break;
		case DiagCmdID::CLEAN_CLW_TO_AGENT_LINK_INFO:
			DIAG_PKT_CLEAN_LINK_INFO Tmep09;
			memmove(&Tmep09,&Temp->AckPkt, sizeof(DIAG_PKT_CLEAN_LINK_INFO));
			Conv_DIAG_PKT_CLEAN_LINK_INFO(Tmep09);
			memmove(&Temp->AckPkt,&Tmep09, sizeof(DIAG_PKT_CLEAN_LINK_INFO));
		break;


	}

	//m_pUDPCommunication->UnicastSend((char*)&TempSendData.AckPkt,TempSendData.Length);
}

/**
 @brief   응답에 대해서 나올수 OCS에게 전달하는 부분  Check 할것.
 @author  puting
 @date    2015.10.01
 @param   pCmdData : 처리할 Commmand
 @return  정상인 경우 0을 리턴, 에러 발생 시 에러 코드 리턴
 */
void TaskControl::SuperviseSendCmdDataOHT2OCS()
{

	static DWORD dwCheckStatusTime = timeGetTime();
	DWORD dwStartTime = timeGetTime();
	char TempNull[3] = {0,};

	if(m_pOHTMainStatus->bOCSReportFlag ==false)
	{
		if (m_pCmdManager->GetOCSResponseCount() != 0)
		{

			// 기종이 OCS경우

	//		for(int i=0; i<m_pCmdManager->GetOCSResponseCount();i++)
	//		{

			// 현재 상태값을 임시 이전 상태값에 저장.
			//헌팅성으로 보고안함.(이동 ,내림,올림일 경우만)
			if((GetCurrentMode() == MODE_AUTO)
			&& ((m_pOHTMainStatus->StandardOHTStatusReport.RunningCycle == CYCLE::CraneUp) || (m_pOHTMainStatus->StandardOHTStatusReport.RunningCycle == CYCLE::CraneDown) || (m_pOHTMainStatus->StandardOHTStatusReport.RunningCycle == CYCLE::MovingToNode)||(m_pOHTMainStatus->StandardOHTStatusReport.RunningCycle == CYCLE::MovingToStation))
			&& (memcmp(&m_pOHTMainStatus->StandardOHTStatusReport.DestinationAddr, &TempNull, 3)==0))
			{
				ADD_LOG_TASKCONTROL("OHT2OCSStatus:Cycle%d,DestinationAddr %d/%d/%d",
				m_pOHTMainStatus->StandardOHTStatusReport.RunningCycle,
				m_pOHTMainStatus->StandardOHTStatusReport.DestinationAddr[0],
				m_pOHTMainStatus->StandardOHTStatusReport.DestinationAddr[1],
				m_pOHTMainStatus->StandardOHTStatusReport.DestinationAddr[2]);

				return;
			}

			PKT_ACK_OHT2OCS* SendData;
			SendData = (PKT_ACK_OHT2OCS*)m_pCmdManager->PopFirstOCSResponse();

		   //	ADD_LOG_TASKCONTROL("SuperviseSendCmdDataOHT2OCS %d",SendData->Length);

			if(((GetExecuteStatus() ==STS_AUTORECOVERY)||
			(GetExecuteStatus() ==STS_AUTORECOVERY_BLANKTRANS)||
	//		((GetExecuteStatus() ==STS_AUTORECOVERY_OVERRUN)&& (m_pPassPermit->GetPassPermitCount() < 1))||
			(GetExecuteStatus() ==STS_AUTORECOVERY_DOUBLETRANS)) &&
			(m_pOHTMainStatus->CurCycle != CYCLE::CycleNotExist))
			{
					InitClearCycle();
					ChangeCmdIDAndCycle(0,0,0,true);
			}

			memmove(SendData->AckPkt.HEADER.MachineID, &m_defualtparam->VHLName,sizeof(SendData->AckPkt.HEADER.MachineID));
			SendData->AckPkt.HEADER.MachineType = MACHINETYPE_VHL; // OHT로 변경
			memmove(&SendData->AckPkt.STATUS,&m_pOHTMainStatus->StandardOHTStatusReport, sizeof(PROTOCOL_STATUS_OHT));



	//		if(AckPkt.AckPkt.TAG.Value = TagID::CMD_DEF_REQ_OHT_OCS;
			if(SendData->AckPkt.TAG.Value == TagID::CMD_DEF_REQ_OHT_OCS)
				SendData->AckPkt.STATUS.RunningCycle = 0x00;

			ChangePreStandardOHTStatus(&SendData->AckPkt.STATUS);
			ConvertBigLitte(SendData);
			m_pUDPCommunication->UnicastSend((char*)&SendData->AckPkt,SendData->Length);

			delete SendData;
	//		}
			dwCheckStatusTime = timeGetTime();




		}
		else if (CheckStandardOHTStatus() ||
				(OHTUTILITY::PreventOverflowGetTimeMs(dwCheckStatusTime) > 10000)) // 상태 변화가 유(거리는 2000mm), 전파강도는 제외
		{
			if((GetCurrentMode() == MODE_AUTO)
			&& ((m_pOHTMainStatus->StandardOHTStatusReport.RunningCycle == CYCLE::CraneUp) || (m_pOHTMainStatus->StandardOHTStatusReport.RunningCycle == CYCLE::CraneDown) || (m_pOHTMainStatus->StandardOHTStatusReport.RunningCycle == CYCLE::MovingToNode)||(m_pOHTMainStatus->StandardOHTStatusReport.RunningCycle == CYCLE::MovingToStation))
			&& (memcmp(&m_pOHTMainStatus->StandardOHTStatusReport.DestinationAddr, &TempNull, 3)==0))
			{
				ADD_LOG_TASKCONTROL("OHT2OCSStatus:Cycle%d,DestinationAddr %d/%d/%d",
				m_pOHTMainStatus->StandardOHTStatusReport.RunningCycle,
				m_pOHTMainStatus->StandardOHTStatusReport.DestinationAddr[0],
				m_pOHTMainStatus->StandardOHTStatusReport.DestinationAddr[1],
				m_pOHTMainStatus->StandardOHTStatusReport.DestinationAddr[2]);

				return;
			}
			// Step 1. Header 정보  저장
			PKT_ACK_OHT2OCS SendData;

			if(((GetExecuteStatus() ==STS_AUTORECOVERY)||
			(GetExecuteStatus() ==STS_AUTORECOVERY_BLANKTRANS)||
	//		((GetExecuteStatus() ==STS_AUTORECOVERY_OVERRUN) && (m_pPassPermit->GetPassPermitCount() < 1))||
			(GetExecuteStatus() ==STS_AUTORECOVERY_DOUBLETRANS)) &&
			(m_pOHTMainStatus->CurCycle != CYCLE::CycleNotExist))
			{
					InitClearCycle();
					ChangeCmdIDAndCycle(0,0,0,true);
			}

			memmove(SendData.AckPkt.HEADER.MachineID, &m_defualtparam->VHLName,sizeof(SendData.AckPkt.HEADER.MachineID));
			SendData.AckPkt.HEADER.MachineType = MACHINETYPE_VHL; // OHT로 변경
			SendData.AckPkt.TAG.Value = TagID::CLW_TO_MCP_STATUS;
			memmove(&SendData.AckPkt.STATUS,&m_pOHTMainStatus->StandardOHTStatusReport, sizeof(PROTOCOL_STATUS_OHT));

	//		ADD_LOG_TASKCONTROL("OHT2OCSStatus:Cycle%d:LoadHP:%dPossible:%d isLoad:%d %d/%d/%d/%d/%d/%d/%d",
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningCycle,
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.LoadUnloadHP,
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.ReturnPossible,
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1,
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CorrectPositionStop,
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.Running,
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.BzWait,
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.CrashAvoidWait,
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.SlowDownStop,
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.Traffic_Sapche,
	//		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningStatus.Traffic_Jungche);

			SendData.Length = sizeof(PKT_STATUS_REPORT_ID_03);

			ChangePreStandardOHTStatus(&SendData.AckPkt.STATUS);
			ConvertBigLitte(&SendData);
			m_pUDPCommunication->UnicastSend((char*)&SendData.AckPkt,SendData.Length);




			dwCheckStatusTime = timeGetTime();

		}
	}else
	{
        ADD_LOG_TASKCONTROL("OCSReportFlag True");
    }


}
// ---------------------------------------------------------------------------

/**
 @brief   이전 상태값 갱신용으로 사용(상태값 비교할때 사용하기 위하여.)
 @author  puting
 @date    2015.10.01
 @param   pCmdData : 처리할 Commmand
 @return  정상인 경우 0을 리턴, 에러 발생 시 에러 코드 리턴
 */
bool TaskControl::ChangePreStandardOHTStatus(PROTOCOL_STATUS_OHT* pPreStatusOHT2OCS)
{
	///< 이전상태값 갱신
	memmove(&m_PreStatusOHT2OCS, pPreStatusOHT2OCS, sizeof(PROTOCOL_STATUS_OHT));

}

/**
 @brief   OCS Cmd를 파싱하는 함수
 Parsing을 완료한 후 바로 Command List를 make한다.
 @author  puting
 @date    2015.10.16
 @param   RcvPkt : 수신한 버퍼 포인터
 @return  ACKResult 리턴
 */
int TaskControl::StandardParsingPCCmd(PKT_OCS2OHT* RcvPkt)
{
	int ACKResult = CMD_ACKNONE;
	////0 :Ack(X)/TaskList(X) 1 : CMD_ACK 2: CMD_LISTUP 3: CMD_ACK_LISTUP
	bool bRrnACk = false;
	bool bQuickstatus = false;
	int  QuickCmd = 0;

	PKT_ACK_OHT2OCS RtnACKPkt;
	memset(&RtnACKPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));

	PKT_OHT2TP_RESPONSE RtnACKPktPC;
	memset(&RtnACKPktPC, 0x00, sizeof(PKT_OHT2TP_RESPONSE));



	ACKResult = CheckCMDData2TaskList(RcvPkt, &RtnACKPkt, &RtnACKPktPC, QuickCmd);


	if (ACKResult == CMD_ACK)
	{
		// TP 다중 접속 되었을 때, 나중에 접속된 호기들은
		// OHT_STATUS.DataLoaded.Info.Reserved1 값을 1로 정하고,
		// 자신이 나중에 중복접속된걸 알게한다.
		m_bMultiTPConnected = m_TPAddrMan.GetCount() > 1 && m_TPAddrMan.bIsFirstTP() == false;
		MakeResponseStandardOHT2TP(&RtnACKPktPC);
	}

	return bRrnACk;
}

/**
 @brief   OCS Cmd를 파싱하는 함수
 Parsing을 완료한 후 바로 Command List를 make한다.
 @author  puting
 @date    2015.10.16
 @param   RcvPkt : 수신한 버퍼 포인터
 @return  ACKResult 리턴
 */
int TaskControl::StandardParsingASTCmd(PKT_OHT2OCS* RcvPkt)
{
	int ACKResult = CMD_ACKNONE;
	////0 :Ack(X)/TaskList(X) 1 : CMD_ACK 2: CMD_LISTUP 3: CMD_ACK_LISTUP
	bool bRrnACk = false;
	bool bQuickstatus = false;
	int  QuickCmd = 0;

	PKT_ACK_OHT2OCS RtnACKPkt;
	memset(&RtnACKPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));


	ACKResult = CheckASTCMDData(RcvPkt, &RtnACKPkt);

	if (ACKResult == CMD_ACK)
	{
		MakeResponseStandardOHT2OCS(&RtnACKPkt);
	}

	return bRrnACk;
}
/**
 @brief   Monitoring Cmd를 파싱하는 함수
 Parsing을 완료한 후 바로 Command List를 make한다.
 @author  puting
 @date    2015.10.16
 @param   RcvPkt : 수신한 버퍼 포인터
 @return  ACKResult 리턴
 */
int TaskControl::StandardParsingMonitoringCmd(PKT_OHT2DIAG* RcvPkt)
{
	int ACKResult = CMD_ACKNONE;
	////0 :Ack(X)/TaskList(X) 1 : CMD_ACK 2: CMD_LISTUP 3: CMD_ACK_LISTUP
	bool bRrnACk = false;
	bool bQuickstatus = false;
	int  QuickCmd = 0;

	PKT_ACK_OHT2DIAG RtnACKPkt;
	memset(&RtnACKPkt, 0x00, sizeof(PKT_ACK_OHT2DIAG));

	ACKResult = CheckDIAGCMDData(RcvPkt, &RtnACKPkt);

	if (ACKResult == CMD_ACK)
	{
	   	MakeResponseStandardOHT2DIAG(&RtnACKPkt);
	}

	return bRrnACk;
}

/**
 @brief   OCS Cmd를 파싱하는 함수
 Parsing을 완료한 후 바로 Command List를 make한다.
 @author  puting
 @date    2015.10.16
 @param   RcvPkt : 수신한 버퍼 포인터
 @return  ACKResult 리턴
 */
int TaskControl::StandardParsingOCSCmd(PKT_OCS2OHT* RcvPkt)
{
	int ACKResult = CMD_ACKNONE;
	////0 :Ack(X)/TaskList(X) 1 : CMD_ACK 2: CMD_LISTUP 3: CMD_ACK_LISTUP
	bool bRrnACk = false;
	bool bQuickstatus = false;
	int  QuickCmd = 0;

	static DWORD dwTurnOffTime = timeGetTime();     //함수 진입 시간.


	PKT_ACK_OHT2OCS RtnACKPkt;
	memset(&RtnACKPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));

	PKT_OHT2TP_RESPONSE RtnACKPktPC;
	memset(&RtnACKPktPC, 0x00, sizeof(PKT_OHT2TP_RESPONSE));

	// step1. Status(Quick) 명령 확인
	 // 기동 On off 처리 .
	 if(GetCurrentMode() == MODE_MANUAL)
	 {
		// 메뉴얼 상태에서는 기동 On->Off Off에서 On  상태를 확인할 필요없음
		// 수동에서는 상위에서 기동 On 명령을 줘도 기동 Off 상태를 보고함. 2015 11 17 확인 완료
		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning = OFF;     //보고용
		m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp = OFF; ///내부

	 }
	 else if(GetCurrentMode() == MODE_AUTO)
	 {
		// MoveStart : 상위로부터의 기동 On off 명령
		// NowRunning : OHT 내부적으로 사용하는 기동 On/off 상태
		// OHT_StartUp : 통과허가에 사용되는 기동 On/Off 상태

		// 일시정지 요청 상태가 아니며, 이전상태값과 차이가 발생할 경우 기동명령에 대해 처리함.
		// 보고용이 아닌, 내부상태를 보고 판단.
		if((m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning != RcvPkt->STATUS.StatusOrder.MoveStart)// ||
		   && (GetExecuteStatus()!= STS_LOADING)&&(GetExecuteStatus()!= STS_UNLOADING))  //이적재 중에는 처리안함.
		{


		   ADD_LOG_TASKCONTROL("[OCS_StatusCheck] StartUp(OCS:%d,OHT:%d,%d) %d, %d",
		   RcvPkt->STATUS.StatusOrder.MoveStart,                                                 //OCS 받은 명령
		   m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp,                          //내부 상태용
		   m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning,       //OCS 보고용
		   RcvPkt->STATUS.StopOrder.DrivingEmergen,
		   RcvPkt->STATUS.StopOrder.TransEmergen);


		   	//이적재 동작중 기동 off시에 대해 처리방안 검토 필요.
            //기동처리를 하지않아야될 상황 넣기.



			/// 해당 구간에 넣기.

            ////////////////////////////////////////////////////////////
			//기동 Off -> On 명령일 경우의 Case,  정지명령이 없어야됨.
			////////////////////////////////////////////////////////////
			if((RcvPkt->STATUS.StatusOrder.MoveStart ==ON)
			&&(OHTUTILITY::PreventOverflowGetTimeMs(dwTurnOffTime) >200))
			{
				/*
				On On : Pass
				On Off : Pass
				off On : Check
				Off off : Check
				*/
				//내부 상태에 따라 달라짐.
				if(m_pOHTMainStatus->bCheckCycleRunning == false)
				{
					if((m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp == OFF) ||
					(m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning ==OFF))
					{
						m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp = ON;
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning = ON;

						ADD_LOG_TASKCONTROL("[OCSOrder] OHT_StartUp Off -> On");

						//초기화 처리명령
						InitClearCycle();
						ChangeCmdIDAndCycle(0,0,0,true);

						m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_TURN_ON);
					}
				}


			}

			//기동 On -> Off

			else if((RcvPkt->STATUS.StatusOrder.MoveStart ==OFF)
			&&(OHTUTILITY::PreventOverflowGetTimeMs(dwTurnOffJobOrderTime) > 2500))
			{
				dwTurnOffTime = timeGetTime(); //기동 Off 이후 기동 On 처리시간전 대기

			   if(!(m_pPathSearch->CheckTurnOffOfPathInfo(OHTMainForm->GetMap(),m_pExecuteInfo->ExecutePositionInfo.CurrNode)))
			   {

					if(((m_pOHTMainStatus->bCheckCycleRunning == false)  && (m_pOHTMainStatus->bCycleing == false)) ||
						((GetExecuteStatus()!= STS_GOING) && (GetExecuteStatus()!= STS_TURN_OFF)))
					{
						m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp = OFF;
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning  = OFF;

						InitExcuteOverRun();
						m_pCmdManager->InitTaskList();

						m_pPassPermit->SetRollBackFinish(true);

						bQuickstatus =true;
						QuickCmd =	TAG_CMDDATA_CTRL_TURN_OFF; //별도 기동 Off Stop 만들어 줄 것.

					}
					else
					{
						if((m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp == ON)
						&& ((m_pExecuteInfo->ExecuteDrivingStep > DRIVING_CTRL_STEP_GO_PREPARE) || (m_pExecuteInfo->ExecuteDrivingStep < DRIVING_CTRL_STEP_GO_PASSBOX)))
							{
								m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp = OFF;

								if(m_pOHTMainStatus->bCheckCycleRunning == true)
								{
									bQuickstatus =true;
									QuickCmd =	TAG_CMDDATA_CTRL_TURN_OFF; //별도 기동 Off Stop 만들어 줄 것.
								}
								else
								{
									InitExcuteOverRun();
									m_pCmdManager->InitTaskList();
									m_pPassPermit->SetRollBackFinish(true);
									m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning  = OFF;
								}
							}
					}

				}
				ADD_LOG_TASKCONTROL("[OCSOrder] OHT_StartUp On -> Off %d",m_pOHTMainStatus->bCheckCycleRunning);
			}
		}
	 }

	 //기동 상태와 별개로 정지상태만 확인할 것.

	 if((RcvPkt->STATUS.StopOrder.DrivingEmergen == ON) ||
	   (RcvPkt->STATUS.StopOrder.TransEmergen == ON))
	   {
			ADD_LOG_TASKCONTROL("[OCSOrder] Emergen Stop");
			bQuickstatus =true;
			QuickCmd =  TAG_CMDDATA_CTRL_ESTOP;
	   }

	 //나머지 상태 저장
	 m_pOHTMainStatus->StandardStatusOHT.StandardOrderStatus.LayoutChangeRunOrder = RcvPkt->STATUS.StatusOrder.LayoutChangeRunOrder;
	 m_pOHTMainStatus->StandardStatusOHT.StandardOrderStatus.CycleUnable = RcvPkt->STATUS.StopOrder.CycleUnable;

	 //Data 전송 확인 부분은 구현관려해서 추가 확인 필요 puting 20151030
	 m_pOHTMainStatus->StandardStatusOHT.StandardOrderStatus.DataTransfer = RcvPkt->STATUS.DataTransOrder.CheckDataTransfer;
	 m_pOHTMainStatus->StandardStatusOHT.StandardOrderStatus.CraneDownAdmission  = RcvPkt->STATUS.StatusOrder.CraneDownAdmission;

     //이재 작업
	 m_pOHTMainStatus->StandardStatusOHT.StandardOrderStatus.CraneUpAdmission= RcvPkt->STATUS.StatusOrder.CraneUpAdmission;
	 m_pOHTMainStatus->StandardStatusOHT.StandardOrderStatus.CraneUpAdmission= RcvPkt->STATUS.StatusOrder.CraneUpAdmission;

	 if(RcvPkt->STATUS.MaintananceOrder.Insert_OK ==ON)
	 {
	 	ADD_LOG_TASKCONTROL("[OCSOrder] Insert_OK");
		m_pOHTMainStatus->StandardStatusOHT.VHL_Insert = SUCCESS;  //성공
     }
	 else if (RcvPkt->STATUS.MaintananceOrder.Insert_NG ==ON)
		m_pOHTMainStatus->StandardStatusOHT.VHL_Insert = FAIL;  //실패
	 else if((RcvPkt->STATUS.MaintananceOrder.Insert_OK == OFF) && (RcvPkt->STATUS.MaintananceOrder.Insert_NG == OFF))
		m_pOHTMainStatus->StandardStatusOHT.VHL_Insert = INIT;  //실패

	 if(RcvPkt->STATUS.MaintananceOrder.FloorTransfer_OK ==ON)
		m_pOHTMainStatus->StandardStatusOHT.VHL_FloorTransfer = SUCCESS;
	 else if (RcvPkt->STATUS.MaintananceOrder.FloorTransfer_NG ==ON)
		m_pOHTMainStatus->StandardStatusOHT.VHL_FloorTransfer = FAIL;
	 else if((RcvPkt->STATUS.MaintananceOrder.FloorTransfer_OK == OFF) && (RcvPkt->STATUS.MaintananceOrder.FloorTransfer_NG == OFF))
		m_pOHTMainStatus->StandardStatusOHT.VHL_FloorTransfer = INIT;

	 if(RcvPkt->STATUS.MaintananceOrder.Board_OK ==ON)
		m_pOHTMainStatus->StandardStatusOHT.VHL_Board = SUCCESS;
	 else if (RcvPkt->STATUS.MaintananceOrder.Board_NG ==ON)
		m_pOHTMainStatus->StandardStatusOHT.VHL_Board = FAIL;
	 else if((RcvPkt->STATUS.MaintananceOrder.Board_OK == OFF) && (RcvPkt->STATUS.MaintananceOrder.Board_NG == OFF))
		m_pOHTMainStatus->StandardStatusOHT.VHL_Board = INIT;





	  //로그 찍기 왜냐하면 상위에서 NG OK를 둘다 On할 경우 처리를 위해서.

	 // 1) 기동/정지/수동 자동에 의해 퀵 커맨드 생성
	 if(bQuickstatus == true)
	 {
		 switch(QuickCmd)
		 {
			 case TAG_CMDDATA_CTRL_TURN_OFF:
			 m_pOHTMainStatus->QuickCmdEStopCommander = ESTOP_BY_TURM_OFF;
			 m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_TURN_OFF);
			 //ADD_LOG("[QuickCmd] Send QUICK_CMD_PAUSE");
			 ADD_LOG_TASKCONTROL("[QuickCmd] Send QUICK_CMD_TURN OFF");
				break;

			 case TAG_CMDDATA_CTRL_ESTOP:
			 m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_STOP);

			 m_pOHTMainStatus->QuickCmdEStopCommander = ESTOP_BY_OCS;
			 m_pOHTMainStatus->QuickCmdEStopType = RcvPkt->STATUS.StopReason;

			 ADD_LOG_TASKCONTROL("[QuickCmd] StopReason %c", RcvPkt->STATUS.StopReason);

			 //ADD_LOG("[QuickCmd] Send OCS QUICK_CMD_STOP");
				break;

			 default:
				break;
		 }
	}


	//////////////////////////SW 분과측에서 해당 내용에 대해 답변이 오면 진행 ////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// Step2. OCS의 명령이 접수가능/불가 판단 및 Task(Cmd) List에 저장 가능한지 판단.

	ACKResult = CheckCMDData2TaskList(RcvPkt, &RtnACKPkt, &RtnACKPktPC, QuickCmd);

	//////////////////////////////////////////////////
	// Step 3. Ack가 필요 한지 확인하는 부분
	// OHT가 접수 가능한 상태이면 List에 저장//////////
	if (ACKResult == CMD_ACK)
	{
		MakeResponseStandardOHT2OCS(&RtnACKPkt);
	}

	return bRrnACk;
}


int TaskControl::CheckASTCMDData(PKT_OHT2OCS* pRcvData, PKT_ACK_OHT2OCS* pAckPkt)
{
	// CMD 검사 및 응답.
	// cmd List에 추가 순서에 맞게 적용
	int ACKResult = CMD_ACKNONE;
	bool bCheck = false;


	UINT nTagID = 0;
	// step1. Tag의 카테고리/TEXTID 분류

   //	nTagCategory = pRcvData->TAG.Value / 1000;
	nTagID = pRcvData->TAG.Value % 1000;


	switch (nTagID)
	{

	case TEXTID::DATATRANSFERORDER_RESPONSE:   	//데이터 전송지시 응답
	{
		 AnsiString strTmp;

		 PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS TempDataResponse;
		 memmove(&TempDataResponse, pRcvData->Value, sizeof(PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS));
		 
		 // -------------------- 전송하는 패킷 검증을 위해 모두 찍어보자 ----------------------
		strTmp.sprintf("[데이터 전송 지시 응답] 전송구분(0 - FTP->OHT, 1 - OHT -> FTP) : %d 지시구분 : %d  접수불가여부(정상:0, 불가:1) :%d  접수불가구분 : 0x%02x   전송지시ID : %d",
								TempDataResponse.Request.TestMode, TempDataResponse.Request.CMDType,
								TempDataResponse.Ack.Accept_Error,  TempDataResponse.Ack.Accept_Error_Class,
								TempDataResponse.Ack.TransOrderID);

		ADD_LOG_TASKCONTROL(strTmp.c_str());
		FirmwareUpdateMessageToTp(strTmp);
		 // ------------------------------------------------------------------------------------

		 if(TempDataResponse.Ack.Accept_Error == OFF)
		 {
			m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID =  TempDataResponse.Ack.TransOrderID;
			m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataType = TempDataResponse.Request.CMDType;


		 }
		 else
		 	m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail = ON;


		 ADD_LOG_TASKCONTROL("[DATATrans] Response %x, %x %x",
		 m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID,
		 m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataType,
		 m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail);


		if(m_iFWU_Direction == FWU_TO_OCS)
		{
			memmove(&pAckPkt->AckPkt, pRcvData, sizeof(PKT_OHT2OCS));
			pAckPkt->Length = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);
			ACKResult = CMD_ACK;	// response to OCS

			ADD_LOG_TASKCONTROL("[DATATrans] 데이터 전송지시 응답 (OHT -> OCS)");
		}
		else
		{
			int iReturn;
			int iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);
			
			PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE SEND_PACKET;

			memcpy(&SEND_PACKET, pRcvData, sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE));
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
			SEND_PACKET.TAG.Value = TagID::RESPONSE_DATATRANSFER_OHT_TP;
			ACKResult = CMD_ACK;

			iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
			ADD_LOG_TASKCONTROL("[DATATrans] 데이터 전송지시 응답 (OHT -> TP)");
		}

    	FirmwareUpdateFlag = false;
	}
	break;

	case TEXTID::DATATRANSFERORDER_PROGRESS:   	//데이터 진척보고
	{
		AnsiString strTmp;

		PROTOCOL_BODY_DATA_TRANSPER_PROGRESS_OHT2OCS  TempDataProgress;
		memmove(&TempDataProgress, pRcvData->Value, sizeof(PROTOCOL_BODY_DATA_TRANSPER_PROGRESS_OHT2OCS));

		// -------------------- 전송하는 패킷 검증을 위해 모두 찍어보자 ----------------------
		strTmp.sprintf("[데이터 전송 진척 보고] 전송구분(0 - FTP->OHT, 1 - OHT -> FTP) : %d  지시구분 : %d  전송실패 : %d  전송성공 : %d  전송상태구분(0정상, 1:통신중, 2:체크중 3:보존중) : 0x%02x  전송지시ID : %d  전송실패구분코드 : %d",
							TempDataProgress.Request.TestMode,  		TempDataProgress.Request.CMDType,  		TempDataProgress.Result.Trans_Fail,
							TempDataProgress.Result.Trans_OK,
							TempDataProgress.Result.Not_Accept_Class, 	TempDataProgress.Result.TransOrderID,  	TempDataProgress.FailClass);
		ADD_LOG_TASKCONTROL(strTmp.c_str());
		FirmwareUpdateMessageToTp(strTmp);

		if (TempDataProgress.Result.Trans_OK == true)
		{
			FirmwareUpdateFlag = true;
		}
		else
		{
			FirmwareUpdateFlag = false;
		}

		// ------------------------------------------------------------------------------------

		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID =  TempDataProgress.Result.TransOrderID;
		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataType = TempDataProgress.Request.TestMode;//전송구분

		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataStatus =TempDataProgress.Result.Not_Accept_Class;
		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataSuccess =TempDataProgress.Result.Trans_OK;
		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail =TempDataProgress.Result.Trans_Fail;


		ADD_LOG_TASKCONTROL("[DATATrans] PROGRESS ID%x,DType%x DStatus%x Success%x, Fail%x",
		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID,
		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataType,
		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataStatus,
		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataSuccess,
		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail);

		memmove(&pAckPkt->AckPkt, pRcvData, sizeof(PKT_OHT2OCS));
		pAckPkt->Length = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);

		// -------------------------------------------------------------------
		AnsiString strTpMsg;

		// Firmware Update Added. shkim. 2016.11.29  234
		// if ((TempDataProgress.Request.CMDType&0x40) == FTP_CMDTYPE_AXIS_PARA)	// Order Type -> Firmware Update
		if ((TempDataProgress.Request.CMDType & FTP_CMDTYPE_APPLICATION) == FTP_CMDTYPE_APPLICATION)
		{
			strTpMsg.sprintf("[Firmware Update] CmdType=FTP_CMDTYPE_APPLICATION, TempDataProgress.Result.Trans_OK(0x01 success)=%d, TestMode=%d",
								TempDataProgress.Result.Trans_OK, TempDataProgress.Request.TestMode);
			FirmwareUpdateMessageToTp(strTpMsg);

			// Run update process
			// if(TempDataProgress.Result.Trans_OK == 0x01)	// Success
			if(TempDataProgress.Result.Not_Accept_Class == 0x03)	// 파일보존 처리 시작지시 (FM -> OHT)
			{
				if(TempDataProgress.Request.TestMode == FTP_TESTMODE_FTP2OHT)	    // FTP -> OHT
				{
					// Firmware Update Trigger
					AnsiString strFileName =  FIRMWARE_UPDATE_PATH;
					FIRMWARE_INFO FIRMWARE_UPDATE_INFO;
					//bool bNeedMainRestart = false;	// AMC와 OHT.exe의 업데이트 시, Main을 재 시작해야 하므로, 이를 저장하는 변수임
					int iResult = 0;
					
					strFileName = strFileName + "UPDATE.INI";
					LoadUpdateIniFile(strFileName, &FIRMWARE_UPDATE_INFO);

					// ---------------------------------------------
					//		Check Process Go
					// ---------------------------------------------

					// 리턴값은 0이면 정상. 1또는 그 이상이면 해당 파일이 없는 개수
					int iFileChk = FirmwareUpdate_FileExistCheck(FIRMWARE_UPDATE_INFO);
					strTpMsg.sprintf("[Firmware Update] FileChk Result = %d", iFileChk);
					FirmwareUpdateMessageToTp(strTpMsg);

					if(iFileChk == 0)	// all file exist
					{
						FirmwareUpdateMessageToTp("[Firmware Update] 필요한 파일이 모두 있으므로 업데이트를 진행합니다.");
						ADD_LOG_TASKCONTROL("[Firmware Update] 필요한 파일이 모두 있으므로 업데이트를 진행합니다.");
						// ---------------------------------------------
						//		Update Process Go
						// ---------------------------------------------

						// Step 0. File Backup
						FirmwareUpdate_FileBackup(FIRMWARE_UPDATE_INFO);
						
						// Step 1. Assistance, File Manager Update
						if((FIRMWARE_UPDATE_INFO.iAsUpdate == TRUE) || (FIRMWARE_UPDATE_INFO.iFmUpdate == TRUE))
						{
							ADD_LOG_TASKCONTROL("[Firmware Update] Assistance Update...");
							FirmwareUpdateMessageToTp("[Firmware Update] Assistance Update...");
							iResult = FirmwareUpdate_AS_FM(FIRMWARE_UPDATE_INFO);

						}

						// Step 2. Parameter Update
						if(FIRMWARE_UPDATE_INFO.iParamUpdate == TRUE)
						{
							ADD_LOG_TASKCONTROL("[Firmware Update] Param Update...");
							FirmwareUpdateMessageToTp("[Firmware Update] Param Update...");
							iResult = FirmwareUpdate_PARAM(FIRMWARE_UPDATE_INFO);
							bMainRestart = true;

							// 2) 파라미터 값을 apply한다
						}


						// Step 3. Wireless Update
						if(
							(FIRMWARE_UPDATE_INFO.iWirelessConfig == TRUE) ||
							(FIRMWARE_UPDATE_INFO.iWirelessMacfil == TRUE) ||
							(FIRMWARE_UPDATE_INFO.iWirelessFirmware == TRUE)
						  )
						{
							ADD_LOG_TASKCONTROL("[Firmware Update] Wireless Update...");
							FirmwareUpdateMessageToTp("[Firmware Update] Wireless Update...");
							//iResult = FirmwareUpdate_WIRELESS(FIRMWARE_UPDATE_INFO);
						}


						// Step 4. AMC Update
						// AMC file checked, main Restart
						if(FIRMWARE_UPDATE_INFO.iAmcUpdate == TRUE)
						{

							//FirmwareUpdate_ModeChange();	// if Auto, change to manual

                            // 2) update execute
							ADD_LOG_TASKCONTROL("[Firmware Update] amc Update...");
							FirmwareUpdateMessageToTp("[Firmware Update] amc Update...");
							//iResult = FirmwareUpdate_AMC(FIRMWARE_UPDATE_INFO);
						}

						// Step 5. OHT.exe Update & Restart Process
						// OHT.exe file checked, main Restart
						if((FIRMWARE_UPDATE_INFO.iMainUpdate == TRUE) || (FIRMWARE_UPDATE_INFO.iAmcUpdate == TRUE) || (FIRMWARE_UPDATE_INFO.iParamUpdate == TRUE))
						{

							//FirmwareUpdate_ModeChange();	// if Auto, change to manual

							// update Main

							if (FIRMWARE_UPDATE_INFO.iMainUpdate == TRUE)
							{
								ADD_LOG_TASKCONTROL("[Firmware Update] Update Main.exe ..... ");
								FirmwareUpdateMessageToTp("[Firmware Update] Update Main.exe ..... ");

								bMainCopy = true;
							}

							FirmwareUpdateMessageToTp("[Firmware Update] Restart Main.exe ...");
							memset(&m_FIRMWARE_UPDATE_INFO, 0, sizeof(FIRMWARE_INFO));
							memcpy(&m_FIRMWARE_UPDATE_INFO, &FIRMWARE_UPDATE_INFO, sizeof(FIRMWARE_INFO));

							bMainRestart = true;
							// iResult = FirmwareUpdate_RESTART(FIRMWARE_UPDATE_INFO);
						}
						else
						{
							//SendDataProgressReport(1, 0);
						}
					}
					else
					{
						FirmwareUpdateMessageToTp("[Firmware Update] 필요한 파일이 빠져 있습니다!  업데이트 불가능!!");
						ADD_LOG_TASKCONTROL("[Firmware Update] 필요한 파일이 빠져 있습니다!  업데이트 불가능!!");
					}
				}
				else
				{
					// OHT -> FTP
				}
			}
			// 마지막 완료 패킷을 OCS로 전송
			else if((TempDataProgress.Result.Not_Accept_Class == 0x00) && (TempDataProgress.FailClass != 999))
			{
				ADD_LOG_TASKCONTROL("[Firmware Update] Complete Packet send (0x00)");
			}
			// 통신모듈을 리셋
			else if((TempDataProgress.Result.Not_Accept_Class == 0x00) && (TempDataProgress.FailClass == 999))
			{
				ADD_LOG_TASKCONTROL("[Firmware Update] Reset..1");
				FirmwareUpdateMessageToTp("[Firmware Update] Reset..");

				// if((TempDataProgress.Result.Trans_OK == 0x01) || (TempDataProgress.Result.Trans_Fail == 0x01))
				{
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID = 0x00;
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataStatus = 0x00;
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataType = 0x00;
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataSuccess = 0x00;
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail = 0x00;

				m_bDataTransper_InProgress = false;
				}

				if((bMainRestart == true) && (GetCurrentMode() == MODE_MANUAL))
				{
					FIRMWARE_INFO FIRMWARE_UPDATE_INFO;
					FIRMWARE_UPDATE_INFO.iMainUpdate = bMainCopy;
					bMainCopy = false;
					//FirmwareUpdate_RESTART(FIRMWARE_UPDATE_INFO);
				}
			}
			// ---------------------------------------------------
			
		}
		// test add...
		else
		{
			if((TempDataProgress.Result.Not_Accept_Class == 0x00) && (TempDataProgress.FailClass == 999))			
			{
				ADD_LOG_TASKCONTROL("[Map Update] Reset..2");
				FirmwareUpdateMessageToTp("[Firmware Update] Reset..");
				
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID = 0x00;
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataStatus = 0x00;
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataType = 0x00;
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataSuccess = 0x00;
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail = 0x00;

				m_bDataTransper_InProgress = false;
			}

		}

		// ---------------------------------------------------------------------

		if(TempDataProgress.FailClass == 999)	// not send
		{
			ACKResult = CMD_ACKNONE;
		}
		else if(m_iFWU_Direction == FWU_TO_OCS)
		{
			ACKResult = CMD_ACK;

			ADD_LOG_TASKCONTROL("[Firmware Update] 데이터 진척보고 (OHT -> OCS)");
		}
		else
		{
			int iReturn;
			int iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);
			
			PKT_DATA_TRANSPER_ORDER_PROGRESS SEND_PACKET;

			memcpy(&SEND_PACKET, pRcvData, sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS));
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
			SEND_PACKET.TAG.Value = TagID::REPORT_DATATRANSFERPROGRESS_OHT_TP; // RESPONSE_DATATRANSFER_OHT_TP;
			// ACKResult = CMD_ACKNONE;  // response -> TP, not OCS

			iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
			ADD_LOG_TASKCONTROL("[Firmware Update] 데이터 진척보고 (OHT -> TP)");

			ACKResult = CMD_ACK;
		}
	}
	break;

	case TEXTID::FIRMWARE_UPDATE_RESULT_WIRELESS_AS2OHT:
	{
		AnsiString strTmp;

		PROTOCOL_BODY_FTP_PUT_GET_RESULT_AS2OHT  TempDataProgress;
		memmove(&TempDataProgress, pRcvData->Value, sizeof(PROTOCOL_BODY_FTP_PUT_GET_RESULT_AS2OHT));
		strTmp.sprintf("무선모듈 업데이트 또는 데이터읽기 결과 (AS => OHT) Result Code : %d  Result Text : %s",
						TempDataProgress.iResult, &TempDataProgress.ResultText);

		// Report to TP
		FirmwareUpdateMessageToTp(strTmp);		
	}
	break;

	
	case TEXTID::REQ_DATATRANSFER:   	//데이터 요구
	{
		AnsiString strTmp;

		strTmp.sprintf("[데이터 전송요구 Value Status] CmdID : 0x%02x  DataStatus : 0x%02x DataType : 0x%02x  DataSuccess : 0x%02x  DataFail : 0x%02x",
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID,
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataStatus,
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataType,
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataSuccess,
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail);

		ADD_LOG_TASKCONTROL(strTmp.c_str());
		FirmwareUpdateMessageToTp(strTmp);

		PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS  TempDataReq;
		memmove(&TempDataReq, pRcvData->Value, sizeof(PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS));

		memmove(&pAckPkt->AckPkt, pRcvData, sizeof(PKT_OHT2OCS));
		pAckPkt->Length = sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS);		

		ACKResult = CMD_ACK;
	}
	break;

	case TEXTID::FIRMWARE_UPDATE_CLOSE_ORDER_AS2OHT:
	{
		// ShowMessage("OHT Closing....");
		// exit(0);
		OHTMainForm->ForceClose();
	}
	break;

	case TEXTID::FIRMWARE_UPDATE_AUTOMODE_ORDER_AS2OHT:
	{
		// ShowMessage("OHT (Manual -> Auto) change");

		ADD_LOG_TASKCONTROL("OHT (Manual -> Auto) change");
		Change2AutoMode();
		Change_FDC_Auto_Mode();
	}
	break;


	}

	return ACKResult;
}


//진단서버용 puting
int TaskControl::CheckDIAGCMDData(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt)
{
	// CMD 검사 및 응답.
	// cmd List에 추가 순서에 맞게 적용
	int ACKResult = CMD_ACKNONE;
	bool bCheck = false;


	UINT nTagID = 0;
	// step1. Tag의 카테고리/TEXTID 분류

	nTagID = pRcvData->HEADER.CommandID;


	switch (nTagID)
	{

	case DiagCmdID::AGENT_TO_CLW_CONNECT_CHK_REQUEST:   	// 연결 확인 요청 (진단 Agent -> EQ)
		bCheck = CheckCmdDiagConnect(pRcvData,pAckPkt);
		if(bCheck ==true) ACKResult = CMD_ACK;
		break;

	case DiagCmdID::DIAG_INFO_DATA_REQUEST:   			// 통계정보보고 요청 (진단 Agent -> 일반 OHT)
		bCheck = CheckCmdDiagInfoReq(pRcvData,pAckPkt);
		if(bCheck ==true) ACKResult = CMD_ACK;
		break;

	case DiagCmdID::DIAG_DATA_RESET_REQUEST:   				// 통계정보 초기화 요청 (진단 Agent -> 일반 OHT)
		bCheck = CheckCmdDiagInitReq(pRcvData,pAckPkt);
		if(bCheck ==true) ACKResult = CMD_ACK;
		break;
/*
	case DiagCmdID::DIAG_INFO_LEVEL_CHANGE_REQUEST:  	// 통계정보 기준 변경 요청 (진단 Agent -> 일반 OHT)
		DIAG_PKT_LEVEL_CHANGE_RESPONSE  TemplevelChange;
		TempReset.HEADER.CommandID = DiagCmdID::DIAG_INFO_LEVEL_CHANGE_RESPONSE;	// 통계정보 기준 변경 응답 (일반 OHT -> 진단 Agent)
		//---------- 상위로부터의 명령---
		DIAG_PKT_BODY_LEVEL_CHANGE_REQUEST TemplevelReq;
		memmove(&TemplevelReq, &pRcvData->Value, sizeof(DIAG_PKT_BODY_LEVEL_CHANGE_REQUEST));


		//--------------------------------------
		//----------OHT 내부 처리해야될 부분----


		//-------------------------------------
		memmove(&pAckPkt->AckPkt, &TemplevelChange, sizeof(DIAG_PKT_LEVEL_CHANGE_RESPONSE));
		pAckPkt->Length = sizeof(DIAG_PKT_LEVEL_CHANGE_RESPONSE);
		ACKResult = CMD_ACK;
		break;
*/
	case DiagCmdID::DIAG_INFO_STATISTICS_RESPONSE:   		// 통계정보 응답 (진단 Agent -> 일반 OHT)
		//응답없음
		bCheck = CheckCmdDiagInfoResponse(pRcvData,pAckPkt);
		if(bCheck ==true) ACKResult = CMD_ACK;
		//--------------------------------------
		break;

	}

	return ACKResult;
}


int TaskControl::CheckCMDData2TaskList(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, PKT_OHT2TP_RESPONSE* AckPktPC,  int QuickCmd)
{
	// CMD 검사 및 응답.
	// cmd List에 추가 순서에 맞게 적용
	int ACKResult = CMD_ACKNONE;
	bool bCheck = false;
	bool bCheckInsertTaskList = false;
	///<Task(CMD) LIst에 들어가는지 확인 하는 부분

	//PKT_TASKCMDDATA *pTempCmdPktData = new PKT_TASKCMDDATA;
	PKT_TASKCMDDATA pTempCmdPktData;
	memset(&pTempCmdPktData, 0x00, sizeof(PKT_TASKCMDDATA));

	UINT nTagCategory, nTagID = 0;
	// step1. Tag의 카테고리/TEXTID 분류

	nTagCategory = pRcvData->TAG.Value / 1000;
	nTagID = pRcvData->TAG.Value % 1000;

	static unsigned int iManualTransCmdCnt = 0;	// 매뉴얼로 업, 다운하는 커맨드의 명령번호  //김승현K


	switch (nTagID)
	{

	case TEXTID::MANUALMODE:   	//PC or Hand TP에서만 명령옴
		// 1. Tag 세부 분류 및 명령 처리 등급
		pTempCmdPktData.DetailTagSort = TAGTYPE_MANUALMODE;
		pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
		// 2. Task List에 추가 유무
		bCheckInsertTaskList = true;
		break;
	case TEXTID::AUTOMODE:     //PC or Hand TP에서만 명령옴
		if (GetCurrentMode() == MODE_MANUAL)
		 {
			// 1. Tag 세부 분류 및 명령 처리 등급
			pTempCmdPktData.DetailTagSort = TAGTYPE_AUTOMODE;
			pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
			// 2. Task List에 추가 유무
			bCheckInsertTaskList = true;
		}
		break;

	case TEXTID::SOFTRESET:    //PC or Hand TP에서만 명령옴
		if (GetCurrentMode() == MODE_MANUAL)
		 {
			// 1. Tag 세부 분류 및 명령 처리 등급
			pTempCmdPktData.DetailTagSort = TAGTYPE_RESET;
			pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
			// 2. Task List에 추가 유무
			bCheckInsertTaskList = true;
		}
		break;
	case TEXTID::E_STOP_PC:    //PC or Hand TP에서만 명령옴
			// 1. Tag 세부 분류 및 명령 처리 등급
		 m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_STOP);
		 m_pOHTMainStatus->QuickCmdEStopCommander = ESTOP_BY_TP;
			// 2. Task List에 추가 유무
//		 bCheckInsertTaskList = false;

		break;


	case TEXTID::PASSPERMIT_RESPONSE: // 통과허가응답
		// 1. Tag 세부 분류 및 명령 처리 등급
		pTempCmdPktData.DetailTagSort = TAGTYPE_PASSPERMIT_RESPONSE;
		pTempCmdPktData.CategoryDiv = CATEGORY_PRIMARY;

		// 2. Task List에 추가 유무
		if(m_pOHTMainStatus->CurCycle != CYCLE::CycleNotExist)
			bCheck = CheckCmdPPResponse(pRcvData,QuickCmd);

		if (bCheck)
		{
			bCheckInsertTaskList = true;
		}
		//bCheckInsertTaskList = true;
		break;
	case TEXTID::PASSPERMIT_OPEN_RESPONSE: // ////<통과허가개방응답

        DATA_PASSOPEN   *pTempDataPOCmd;  // data pass type 판별 변수 추가
        pTempDataPOCmd  = (DATA_PASSOPEN *)m_pPassPermit->ReferFirstPassOpen();

    	if (pTempDataPOCmd->bPassOpenType == FORCE_TYPE)
		{

            ADD_LOG_TASKCONTROL("통과개방응답[Manual] by hjh");

			//  Task List에 추가 하지 않음 by hjh
			bCheck = CheckCmdForcePPOResponse(pRcvData,QuickCmd);

            bCheckInsertTaskList = false;

		}
		else
        {
            // 1. Tag 세부 분류 및 명령 처리 등급
	    	pTempCmdPktData.DetailTagSort = TAGTYPE_PASSPERMIT_OPEN_RESPONSE;
	    	pTempCmdPktData.CategoryDiv = CATEGORY_PRIMARY;

		    // 2. Task List에 추가 유무
	    	if(m_pOHTMainStatus->CurCycle != CYCLE::CycleNotExist)
		    	bCheck = CheckCmdPPOResponse(pRcvData,QuickCmd);

		    if (bCheck)
		    {
		    	bCheckInsertTaskList = true;
	    	}

        }

		break;
	case TEXTID::PASSPERMIT_REQ: // 통과허가 문의
		pTempCmdPktData.DetailTagSort = TAGTYPE_PASSPERMIT_REQ;
		pTempCmdPktData.CategoryDiv = CATEGORY_PRIMARY;

		// 2. Task List에 추가 유무
		bCheck = CheckCmdPPREQResponse(pRcvData,pAckPkt,QuickCmd);



//		bCheckInsertTaskList = false;
		ACKResult = CMD_ACK;
		break;

	case TEXTID::FORCE_PASSPERMIT_OPEN: // ////<강제통과허가개방(from TP) -> 일반개방으로 바뀜
		if ((GetCurrentMode() == MODE_MANUAL)  &&  (m_pExecuteInfo->ExecutePositionInfo.Marked))
		{
			// 1. Tag 세부 분류 및 명령 처리 등급
			//pTempCmdPktData.DetailTagSort = TAGTYPE_FORCE_PASSPERMIT_OPEN_RESPONSE;
//			pTempCmdPktData.CategoryDiv = CATEGORY_PRIMARY;

			// 2. Task List에 추가 유무
           	bCheck = CheckCmdPassOpen_fromTP_and_AutoChange(FROMTP_TYPE,pRcvData,QuickCmd);

//			bCheckInsertTaskList = false;

		}
		break;



	case TEXTID::MOVE_RESTART: // 주행재개
		pTempCmdPktData.DetailTagSort = TAGTYPE_PASSPERMIT_REQ;
		pTempCmdPktData.CategoryDiv = CATEGORY_PRIMARY;

		// 2. Task List에 추가 유무
		bCheck = CheckCmdMoveRestart(pRcvData,QuickCmd);

//		bCheckInsertTaskList = false;

		break;



	case TEXTID::JOBORDER: // ///<작업지시

		if((nTagCategory > TAGCATEGORY::TAG_CUTLINE_OCSCMD_OUT) && (nTagCategory < TAGCATEGORY::TAG_CUTLINE_OCSCMD_IN)) // OCS의 명령
			pTempCmdPktData.CategoryDiv =CATEGORY_SECONDARY;
		else if((nTagCategory > TAGCATEGORY::TAG_CUTLINE_PCCMD_OUT) && (nTagCategory < TAGCATEGORY::TAG_CUTLINE_PCCMD_IN)) // PC or Hand TP의 명령
			pTempCmdPktData.CategoryDiv =CATEGORY_TERTIARY;


		 bCheck = CheckCmdJobOrder(pRcvData, pAckPkt,&pTempCmdPktData.DetailTagSort, QuickCmd, pTempCmdPktData.CategoryDiv);


		if (bCheck)
		{
			if(CheckStatusForTask()==false)
			{
				 m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_IDRESET);
			}

			bCheckInsertTaskList = true;
		}

		ACKResult = CMD_ACK;
		break;


	// ===================================================================
	case TEXTID::TP_MOVE_BY_MAP: // TP에서 OHT로 이동명령 (목적지만 지정하고 MAP정보 사용)
	{
		/*
			구현에 대한 방향 (shkim)
			TP이동명령은 위의 작업지시를 이용한 이동명령과 결국 동일 이므로,   필요한 정보를 패킷에 채운 후,
			Tag를 작업지시로 바꾸고 그 다음 단계로 진행하도록 한다면 동일하게 동작한다고 본다

		*/
		ADD_LOG_TASKCONTROL("[TP_MOVE_BY_MAP] START");

		int StartNodeID = 0;
		int EndNodeID = 0;
		int PortType = 0;
		int iTemp = 0;
		int iDirection, iCount;
		bool bIsStation;
		int iPathList[500];
		int nResult = NO_ERR;
		int nError = NO_ERR;

		BYTE bMapMaker = 0x00;
		BYTE bMapMakeSteerChk = 0x00;

		// (1) 들어온 데이터를 파싱 준비
		int iInputPacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
		PKT_TP2OHT_MOVE_BY_MAP	ORIGINAL_PACKET;
		memcpy(&ORIGINAL_PACKET, pRcvData, iInputPacketSize);

		// path get
		StartNodeID = ORIGINAL_PACKET.BODY.StartNodeID;
		EndNodeID	= ORIGINAL_PACKET.BODY.StopNodeID;
		bIsStation  = ORIGINAL_PACKET.BODY.IsGoStation;	// TRUE(1) : Station, FALSE(0) : Normal Node
		PortType    = ORIGINAL_PACKET.BODY.PortType;

		//bMapMaker  = ORIGINAL_PACKET.BODY.PortType / 100;
		bMapMaker  = ORIGINAL_PACKET.BODY.IsMapMaker;  //0x00 일반 모드, 0x01 MapMaker 모드 , 0x02 : MapMaker 검증 모드
		bMapMakeSteerChk  = ORIGINAL_PACKET.BODY.IsSteerCheck;

		// Map Maker 조향 확인 모드
		if((bMapMaker == 0x01) && (bMapMakeSteerChk == 0x01))
		{
			Sleep(100);
			if(m_pHWSet->m_pSteer->IsLeftPos(BOTH) == true)
			{
				nError = m_pHWSet->m_pSteer->Right(false);
			}
			else if(m_pHWSet->m_pSteer->IsRightPos(BOTH) == true)
			{
				nError = m_pHWSet->m_pSteer->Left(false);
			}
			else
			{
				nError = ERR_STEER_NOTSAMESTATUS;
			}
		}

		if(nError == NO_ERR)
		{
			if(bMapMaker == 0x01)     //주행노드에서만 가능하게 인터락 추가 필요.
				nResult = ManualMovingMapMake(StartNodeID, EndNodeID,bMapMaker);
//				bResult = ManualMoving(StartNodeID, EndNodeID, bIsStation, PortType, (int)CYCLE::TP_RunningLearning);
			else if(bMapMaker == 0x02)
				nResult = ManualMovingMapMake(StartNodeID, EndNodeID,bMapMaker);
			else if (bIsStation == TRUE)
				nResult = ManualMoving(StartNodeID, EndNodeID, bIsStation, PortType, (int)CYCLE::MovingToStation);
			else
				nResult = ManualMoving(StartNodeID, EndNodeID, bIsStation, PortType, (int)CYCLE::MovingToNode);

			ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] Last Result : %d", nResult);
		}else
		   nResult = 0x05; //조향 관련된 에러.


		if(ORIGINAL_PACKET.HEADER.MachineType == MACHINETYPE_TP)  //핸드 TP는 제외
        {
        	// ACK 가 필요한지 생각. 그리고 ACK 정보를 어디서 넣어주나 확인해봐야 한다
            int iReturn;
            int iPacketSize = sizeof(PKT_OHT2TP_MOVE_BY_MAP_RESULT);
            PKT_OHT2TP_MOVE_BY_MAP_RESULT SEND_PACKET;

            memset(&SEND_PACKET, 0, iPacketSize);
            SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
            SEND_PACKET.TAG.Value = TagID::CMD_TP_MOVE_RESULT;
            SEND_PACKET.BODY.iResult = nResult;

            iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
        }


		if(nError != NO_ERR)
			ErrorCode_Main = nError;

		//if (bResult == true)
		//	ACKResult = CMD_ACK;
	}
	break;

	case TEXTID::TP_MOVE_LOAD: // TP에서 OHT로 이동명령 (목적지만 지정하고 MAP정보 사용)
	{
		/*
			구현에 대한 방향 (shkim)
			TP이동명령은 위의 작업지시를 이용한 이동명령과 결국 동일 이므로,   필요한 정보를 패킷에 채운 후,
			Tag를 작업지시로 바꾸고 그 다음 단계로 진행하도록 한다면 동일하게 동작한다고 본다

		*/
		ADD_LOG_TASKCONTROL("[TP_MOVE_LOAD] START");

		int StartNodeID = 0;
		int EndNodeID = 0;
        int PortType = 0;
		int iTemp = 0;
		int iDirection, iCount;
        bool bIsStation;
		int iPathList[500];
		int bResult;

		// (1) 들어온 데이터를 파싱 준비
		int iInputPacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
		PKT_TP2OHT_MOVE_BY_MAP	ORIGINAL_PACKET;
		memcpy(&ORIGINAL_PACKET, pRcvData, iInputPacketSize);

		// path get
		StartNodeID = ORIGINAL_PACKET.BODY.StartNodeID;
		EndNodeID	= ORIGINAL_PACKET.BODY.StopNodeID;
		bIsStation  = ORIGINAL_PACKET.BODY.IsGoStation;	// TRUE(1) : Station, FALSE(0) : Normal Node
        PortType    = ORIGINAL_PACKET.BODY.PortType;

		if (bIsStation == TRUE)
			bResult = ManualMoving(StartNodeID, EndNodeID, bIsStation, PortType, (int)CYCLE::CraneDown);
		else
			bResult = ManualMoving(StartNodeID, EndNodeID, bIsStation, PortType, (int)CYCLE::MovingToNode);

		ADD_LOG_TASKCONTROL("[TP_MOVE_LOAD] Last Result : %d", bResult);

        if(ORIGINAL_PACKET.HEADER.MachineType != MACHINETYPE_HANDYTP)
        {
        	// ACK 가 필요한지 생각. 그리고 ACK 정보를 어디서 넣어주나 확인해봐야 한다
            int iReturn;
            int iPacketSize = sizeof(PKT_OHT2TP_MOVE_BY_MAP_RESULT);
            PKT_OHT2TP_MOVE_BY_MAP_RESULT SEND_PACKET;

            memset(&SEND_PACKET, 0, iPacketSize);
            SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
            SEND_PACKET.TAG.Value = TagID::CMD_TP_MOVE_RESULT;
            SEND_PACKET.BODY.iResult = bResult;

            iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
        }

		//if (bResult == true)
		//	ACKResult = CMD_ACK;
	}
	break;


	case TEXTID::TP_MOVE_UNLOAD: // TP에서 OHT로 이동명령 (목적지만 지정하고 MAP정보 사용)
	{
		/*
			구현에 대한 방향 (shkim)
			TP이동명령은 위의 작업지시를 이용한 이동명령과 결국 동일 이므로,   필요한 정보를 패킷에 채운 후,
			Tag를 작업지시로 바꾸고 그 다음 단계로 진행하도록 한다면 동일하게 동작한다고 본다

		*/
		ADD_LOG_TASKCONTROL("[TP_MOVE_UNLOAD] START");

		int StartNodeID = 0;
		int EndNodeID = 0;
        int PortType = 0;
		int iTemp = 0;
		int iDirection, iCount;
        bool bIsStation;
		int iPathList[500];
		int bResult;

		// (1) 들어온 데이터를 파싱 준비
		int iInputPacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
		PKT_TP2OHT_MOVE_BY_MAP	ORIGINAL_PACKET;
		memcpy(&ORIGINAL_PACKET, pRcvData, iInputPacketSize);

		// path get
		StartNodeID = ORIGINAL_PACKET.BODY.StartNodeID;
		EndNodeID	= ORIGINAL_PACKET.BODY.StopNodeID;
		bIsStation  = ORIGINAL_PACKET.BODY.IsGoStation;	// TRUE(1) : Station, FALSE(0) : Normal Node
        PortType    = ORIGINAL_PACKET.BODY.PortType;

		if (bIsStation == TRUE)
			bResult = ManualMoving(StartNodeID, EndNodeID, bIsStation, PortType, (int)CYCLE::CraneUp);
		else
			bResult = ManualMoving(StartNodeID, EndNodeID, bIsStation, PortType, (int)CYCLE::MovingToNode);

		ADD_LOG_TASKCONTROL("[TP_MOVE_UNLOAD] Last Result : %d", bResult);

        if(ORIGINAL_PACKET.HEADER.MachineType != MACHINETYPE_HANDYTP)
        {
        	// ACK 가 필요한지 생각. 그리고 ACK 정보를 어디서 넣어주나 확인해봐야 한다
            int iReturn;
            int iPacketSize = sizeof(PKT_OHT2TP_MOVE_BY_MAP_RESULT);
            PKT_OHT2TP_MOVE_BY_MAP_RESULT SEND_PACKET;

            memset(&SEND_PACKET, 0, iPacketSize);
            SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
            SEND_PACKET.TAG.Value = TagID::CMD_TP_MOVE_RESULT;
            SEND_PACKET.BODY.iResult = bResult;

            iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
        }

		//if (bResult == true)
		//	ACKResult = CMD_ACK;
	}
	break;

    // 2017-01-02, Add by Labeler
    // Hoist, Shift, Rotate, Hand, Shutter Home 이동 명령 추가 구현
	case TEXTID::TP_MOVE_ALL_HOME:
	{
		PARSING_TRANS_INFO tmpCmd;
		memset(&tmpCmd, 0, sizeof(PARSING_TRANS_INFO));		// 패킷 메모리 클리어

		ADD_LOG_TASKCONTROL("[TP_MOVE_ALL_HOME] START");

    	tmpCmd.CmdID = iManualTransCmdCnt++;	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
		tmpCmd.CmdCycle = CYCLE::TP_CraneHome;

		if(CheckStatusForManual())
			MakeTransCycleList(&tmpCmd);
	}
	break;

	case TEXTID::TP_MOVE_HOIST_HOME:
	{
		PARSING_TRANS_INFO tmpCmd;
		memset(&tmpCmd, 0, sizeof(PARSING_TRANS_INFO));

		ADD_LOG_TASKCONTROL("[TP_MOVE_HOIST_HOME] START");

		tmpCmd.CmdID = iManualTransCmdCnt++;
		tmpCmd.CmdCycle = CYCLE::TP_SmalladdHoistHome;

		if(CheckStatusForManual())
			MakeTransCycleList(&tmpCmd);
	}
	break;

	case TEXTID::TP_AUTO_HOISTHOME:
	{
		PARSING_TRANS_INFO tmpCmd;
		memset(&tmpCmd, 0, sizeof(PARSING_TRANS_INFO));		// 패킷 메모리 클리어

		ADD_LOG_TASKCONTROL("[TP_AUTO_HOISTHOME] START");

    	tmpCmd.CmdID = iManualTransCmdCnt++;	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
		tmpCmd.CmdCycle = CYCLE::TP_AutoHoistHome;

		if(CheckStatusForManual())
			MakeTransCycleList(&tmpCmd);
	}
	break;



	case TEXTID::TP_DEBUG_COMMAND:
	{

		// param apply test
		m_pHWSet->m_pLeftEQPIO->Save_HybridPIO_Data();
		
#if 0
		int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		int iReturn;
		AnsiString strTmp;

		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		if(SEND_PACKET.BODY.iVar2 == 6)
		{
			FirmwareUpdate_ModeChange();
		}
		else if(SEND_PACKET.BODY.iVar2 == 7)
		{
			FIRMWARE_INFO FIRMWARE_UPDATE_INFO;			
			FIRMWARE_UPDATE_INFO.iMainUpdate = FALSE;			
			FirmwareUpdate_RESTART(FIRMWARE_UPDATE_INFO);

			FirmwareUpdateMessageToTp("[Firmware Update] Test for restart function");
		}
		else if(SEND_PACKET.BODY.iVar2 == 8)
		{
			Change2AutoMode();
			Change_FDC_Auto_Mode();
		}
		else
		{
			ParamApply(SEND_PACKET.BODY.iVar2);
		}

		/*
		if(SEND_PACKET.BODY.iVar1 == TP_DEBUG_START)
		{
			Send_AutoTeachResult(TP_DEBUG_PROGRESS);
		}
		else if(SEND_PACKET.BODY.iVar1 == TP_DEBUG_STOP)
		{
			Send_AutoTeachResult(AUTO_TEACH_ENDED);
		}
		*/
#endif

	}
	break;
	
    // 2017-01-14, Add by Labeler
    // 자동 Auto Mode 전환 기능 구현
	case TEXTID::TP_AUTOMODE_CHANGE:
	{

		Change2AutoMode();
		Change_FDC_Auto_Mode();

	}
    break;
	
	case TEXTID::AUTOCHANGE_REQ:
	{

		// 2. Task List에 추가 유무
		//테스트 이후에 TP에서 주는 버튼을 OCS에서 주는 명령으로 변경할것.
		pTempCmdPktData.DetailTagSort = TAGTYPE_AUTOCHANGE;
		pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;

		bCheck = CheckCmdAutoChangeResponse(pRcvData,pAckPkt,QuickCmd);

		if(bCheck == true)
		{
			bCheckInsertTaskList = true;
			m_StepAutoChangeReset.bAllDone = false;
			m_StepAutoChangeReset.nRetryCnt = 0;
		}

		ACKResult = CMD_ACK;
	}
	break;

	case TEXTID::TP_OHT_TAKEOUT: // TP에서 OHT를 MTL로 Take Out 명령 발생 시 호출
	{
        int nError = NO_ERR;
		int iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
		int iReturn;
        STATION_DATA * pStationData = NULL;
		AnsiString strTmp;

		PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

        if(m_pExecuteInfo->ExecutePositionInfo.Marked == true)
	    {
            SEND_PACKET.BODY.CurNodeID = m_pExecuteInfo->ExecutePositionInfo.CurrNode;
            SEND_PACKET.BODY.CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;

            pStationData = OHTMainForm->GetStationDataManager()->Find(SEND_PACKET.BODY.CurStationID);

            if(pStationData == NULL)
            {
                ADD_LOG_TASKCONTROL("[ERR_DATA_STATION_NOEXIST] Station Id: %d",SEND_PACKET.BODY.CurStationID );
                break;
            }
        }

		ADD_LOG_TASKCONTROL("[TAKEOUT] OHT_TAKE_OUT");

		ADD_LOG_TASKCONTROL("테스트 인수 CurNodeID : %d CurStationID : %d",
			SEND_PACKET.BODY.CurNodeID,
			SEND_PACKET.BODY.CurStationID);

		PARSING_TRANS_INFO tmpCmd;

		tmpCmd.CmdID = iManualTransCmdCnt++;	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
		tmpCmd.CmdCycle = CYCLE::TakeOut;

		memcpy(&tmpCmd.TransCmd, (TRANS_CMD *)&SEND_PACKET.BODY, sizeof(TRANS_CMD));

		if(CheckStatusForManual())
			nError = MakeTransCycleList(&tmpCmd);

        // 2017-01-20, Add by Labeler
        // Manual TakeOut 명령 발생 시, 현재 Cycle 상태 변경 Logic 추가
        if(nError == NO_ERR)
        {
            ChangeCmdIDAndCycle(tmpCmd.CmdCycle, tmpCmd.CmdID, SEND_PACKET.BODY.CurStationID);
        }

	}
    break;

	case TEXTID::FLOORORDER: // ///<층간이동지시


		if((nTagCategory > TAGCATEGORY::TAG_CUTLINE_OCSCMD_OUT) && (nTagCategory < TAGCATEGORY::TAG_CUTLINE_OCSCMD_IN)) // OCS의 명령
			pTempCmdPktData.CategoryDiv =CATEGORY_SECONDARY;
		else if((nTagCategory > TAGCATEGORY::TAG_CUTLINE_PCCMD_OUT) && (nTagCategory < TAGCATEGORY::TAG_CUTLINE_PCCMD_IN)) // PC or Hand TP의 명령
			pTempCmdPktData.CategoryDiv =CATEGORY_TERTIARY;

		pTempCmdPktData.DetailTagSort = TAGTYPE_FLOORMOVEMENT;

		// ACKResult = CheckOCSCmd(DecodeSize, m_GoEtcCmd.GoEtcCmd.CmdID, pRecvBuf, &Header);

		// Return할 구조체 값 대입
		// RtnACKData = GetOCSAckData(Header.uTag, m_GoEtcCmd.GoEtcCmd.CmdID, ACKResult);

		if (bCheck)
		{
			bCheckInsertTaskList = true;
		}
		ACKResult = CMD_ACK;
		break;
		// ===================================================================//

    // 2016-12-27, Add by Labeler
    // 데이터 전송 요구, 추가 구현, 데이터 전송 요구의 경우, Note TP or Hand TP에서만 발생
    case TEXTID::REQ_DATATRANSFER:
	{
		// ---------------------------------
		AnsiString strTmp;

		strTmp.sprintf("[데이터 전송요구 TP에서 수신] CmdID : 0x%02x  DataStatus : 0x%02x DataType : 0x%02x  DataSuccess : 0x%02x  DataFail : 0x%02x m_bDataTransper_InProgress : %d",
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID,
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataStatus,
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataType,
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataSuccess,
						m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail,
						m_bDataTransper_InProgress);

		ADD_LOG_TASKCONTROL(strTmp.c_str());
		FirmwareUpdateMessageToTp(strTmp);

		// ---------------------------------
		if(m_bDataTransper_InProgress == false)
		{
	        PKT_ACK_OHT2OCS  TempPkt;

			TempPkt.AckPkt.TAG.Value = TagID::REQ_DATATRANSFER_OHT2OCS;
			memmove(&TempPkt.AckPkt.Value, &pRcvData->Value, sizeof(PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS));
			TempPkt.Length = sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS);		

			/* 값이 맞게 들어오는지 꼭 확인하자 */
			PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS  TEST_PKT;
			memcpy(&TEST_PKT, &pRcvData->Value, TempPkt.Length);
			/* -------------------------------- */

			ADD_LOG_TASKCONTROL("[데이터 전송요구 Send] TP -> OHT -> OCS");
			FirmwareUpdateMessageToTp("[데이터 전송요구 Send] TP -> OHT -> OCS");

	        MakeResponseStandardOHT2OCS(&TempPkt);

			AckPktPC->TAG.Value = TagID::REQ_DATATRANSFER_OHT2OCS;
			ACKResult = CMD_ACK;
		}
		else
		{
				ADD_LOG_TASKCONTROL("[데이터 전송요구 Cancel]");
				FirmwareUpdateMessageToTp("[데이터 전송요구 Cancel]");
				ACKResult = CMD_ACKNONE;
		}
    }
    break;

	case TEXTID::DATATRANSFERORDER: // 데이터 전송지시
	{
		if((nTagCategory > TAGCATEGORY::TAG_CUTLINE_OCSCMD_OUT) && (nTagCategory < TAGCATEGORY::TAG_CUTLINE_OCSCMD_IN)) // OCS의 명령
			pTempCmdPktData.CategoryDiv =CATEGORY_SECONDARY;
		else if((nTagCategory > TAGCATEGORY::TAG_CUTLINE_PCCMD_OUT) && (nTagCategory < TAGCATEGORY::TAG_CUTLINE_PCCMD_IN)) // PC or Hand TP의 명령
			pTempCmdPktData.CategoryDiv =CATEGORY_TERTIARY;

		pTempCmdPktData.DetailTagSort = TAGTYPE_DATATRANSFER;

		chkFirmwareUpdateDirectory();	// 업데이트 동작에 필요한 디렉토리가 없으면 만든다 (순환 생성가능)
		
		// 여기서 처리 가능하면 바로 응답
		// 바로 처리되는 경우는 현재 데이터 전송 지시가 실행중인 경우로서, 보낸 쪽으로 리턴하고, 
		// 응답 방향은 바꾸지 않는다
		bCheck = CheckCmdDataTransfer(pRcvData, pAckPkt, QuickCmd);
		ADD_LOG_TASKCONTROL("[데이터전송지시] 현상태 확인결과 : %d (1=실행중(NAK), 0=실행가능(수락)", bCheck);

		if(bCheck)
		{
			if(nTagCategory == (TagID::CMD_DATATRANSFER_OCS2OHT / 1000))
			{
				m_iFWU_Direction = FWU_TO_OCS;	// 2017.03.09 added
				ACKResult = CMD_ACK;
			}
			else
			{
				m_iFWU_Direction = FWU_TO_TP;	// 2017.03.09 added
				
				// Response Return (NAK) to TP
				PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE SEND_PACKET;
				int iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);

				memmove(&SEND_PACKET, &pAckPkt->AckPkt, iPacketSize);
				SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
				SEND_PACKET.TAG.Value = TagID::RESPONSE_DATATRANSFER_OHT_TP;
				int iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
			}
		}
		else
		{	// Response Direction Change
			if(nTagCategory == (TagID::CMD_DATATRANSFER_OCS2OHT / 1000))
			{
				m_iFWU_Direction = FWU_TO_OCS;
			}
			else
			{
				m_iFWU_Direction = FWU_TO_TP;
			}
		}
		
		// Return할 구조체 값 대입
		// RtnACKData = GetOCSAckData(Header.uTag, m_GoEtcCmd.GoEtcCmd.CmdID, ACKResult);
	}
	break;

    case TEXTID::VERSION_REQ_DETAIL:
    {
        int iReturn;
			
        // Set Direction
        if(nTagCategory == (TagID::CMD_DATATRANSFER_OCS2OHT / 1000))
        {
            m_iFWU_Direction = FWU_TO_OCS;
        }
        else
        {
            m_iFWU_Direction = FWU_TO_TP;
        }

        // 정보를 취득한다 (OHT Main에서 취득 가능한 것을 먼저, 모두)


        // 이 아래부분은 원래는 트리거만 해 놓고 AS에서 응답이 오면 보낼 코드이다
        // 방향은 위 변수를 참고한다
        PKT_VERSION_RESPONSE SEND_PACKET;
        int iPacketSize = sizeof(PKT_VERSION_RESPONSE);
        memset(&SEND_PACKET, 0, iPacketSize);
			
        getInfoDetailVersion(SEND_PACKET, m_iFWU_Direction);
			
        // Response
        if(nTagCategory == (TagID::CMD_VERSIONREQ_OCS_OHT / 1000))
		{
			Conv_PKT_VERSION_RESPONSE(SEND_PACKET);
			m_pUDPCommunication->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			/*
			memmove(pAckPkt->AckPkt.Value, &SEND_PACKET.BODY , sizeof(PROTOCOL_BODY_OHT_VERSION_OHT2OCS));
			ACKResult = CMD_ACK;
			*/
        }
        else
        {
            iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
        }

        //FIRMWARE_INFO FIRMWARE_UPDATE_INFO;
        //FIRMWARE_UPDATE_INFO.iAmcUpdate = true;
        //FirmwareUpdate_AMC(FIRMWARE_UPDATE_INFO);
    }
    break;

	case TEXTID::VERSION_REQ: // ///<version 정보 요구 응답
		pTempCmdPktData.CategoryDiv = CATEGORY_SECONDARY;
		pTempCmdPktData.DetailTagSort = TAGTYPE_INFOREQ;
		// 여기서 처리 가능하면 바로 응답
		bCheck = CheckCmdVersion(pRcvData, pAckPkt, QuickCmd);

//		bCheckInsertTaskList = false;
		ACKResult = CMD_ACK;
		break;

	case TEXTID::INSERTREQ_RESPONSE: // 투입요구 응답
		pTempCmdPktData.CategoryDiv = CATEGORY_SECONDARY;
		pTempCmdPktData.DetailTagSort = TAGTYPE_INFOREQ;
		// 여기서 처리 가능하면 바로 응답

//		bCheckInsertTaskList =false;

		bCheck = CheckCmdInsertResponse(pRcvData,QuickCmd);

		if((GetCurrentMode() == MODE_MANUAL) ||
			 (GetOHTError() != NO_ERR) ||
			((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF)) ||
			((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning ==OFF)))
			{

			   	 //수행불가
			}
		else
		{
			if(bCheck)
			{
				if (m_pExecuteInfo->FlagMTLInfo.nTakeInOrder != SUCCESS)
				{
					EnterCriticalSection(&OHTMainForm->m_CS);
					m_pExecuteInfo->FlagMTLInfo.nTakeInOrder = SUCCESS;
					LeaveCriticalSection(&OHTMainForm->m_CS);
					MakeOHTInCmd(SUCCESS);
			 	}
			}
			else
			{
				EnterCriticalSection(&OHTMainForm->m_CS);
                m_pExecuteInfo->FlagMTLInfo.nTakeInOrder = FAIL;
				LeaveCriticalSection(&OHTMainForm->m_CS);
				ADD_LOG_TASKCONTROL("Insert Fail...");
			}

        }

		break;
	case TEXTID::OCSSTATUS: // 상태보고
		pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
		break;
	case TEXTID::TPSTATUS:
		pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
		AckPktPC->TAG.Value = TagID::CMD_STATUSREQ_TP_RESPONSE;
		ACKResult = CMD_ACK;
		break;

	case TEXTID::TP_VIB_DATA:
		pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
		AckPktPC->TAG.Value = TagID::CMD_VIBDATA_REQ_TP_RESPONSE;
		ACKResult = CMD_ACK;

		if(CheckStatusForManual())
		{
			/*
			int iPacketSize = sizeof(PKT_OHT2TP_VIBRATION_INFO);
			int iReturn;

			PKT_OHT2TP_VIBRATION_INFO	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);
			memcpy(&SEND_PACKET, pRcvData, iPacketSize);

			pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
			ACKResult = CMD_ACKNONE;

			SEND_PACKET.BODY.Reserved2 = Load_AutoTeaching_DrvingData(SEND_PACKET.BODY.Reserved1,SEND_PACKET.BODY.PortType);
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
			SEND_PACKET.TAG.Value = TagID::CMD_VIBDATA_REQ_TP_RESPONSE;

			iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
			ADD_LOG_TASKCONTROL("[TP] TP_AUTOTEACHING_LOAD_DRIVING");
            */
		}


		break;

	case TEXTID::TP_INIT:
		pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
		AckPktPC->TAG.Value = TagID::CMD_INITREQ_TP_RESPONSE;
		memmove(&AckPktPC->OHT_BODY, &m_OHTInfo, sizeof(uTP_CMD));
		ACKResult = CMD_ACK;
		break;

	// TP에서 IO 리스트를 요청한 경우
	case TEXTID::IOMAP_REQ_TP2OHT:
	
		pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
		ACKResult = CMD_ACKNONE;
		SendIoStatus_2_OHT();

		ADD_LOG_TASKCONTROL("[TP] IOMAP_REQ_TP2OHT");
		
		break;

	case TEXTID::IOMAP_USE_ONOFF_TP2OHT:
	{
		if(CheckStatusForManual())
		{
			PKT_OHT2TP_IO_SET CmdPkt;
			int iPacketSize = sizeof(PKT_OHT2TP_IO_SET);
			memcpy(&CmdPkt, pRcvData, iPacketSize);

			int iIndex = CmdPkt.BODY.iIoNum;
			int iValue = CmdPkt.BODY.iOnOff;

			OHTMainForm->GetManualControl()->SetOnOff(iIndex, iValue);

			ADD_LOG_TASKCONTROL("[TP] IOMAP_USE_ONOFF_TP2OHT(MAN = 1) - Index : %d  SetVal : %d", iIndex, iValue);
		}
		else
		{
			ADD_LOG_TASKCONTROL("[TP] IOMAP_USE_ONOFF_TP2OHT(MAN = 0)");
		}
	}
	break;

	case TEXTID::CID_M_COMMAND_SEND:
	{
		OHTMainForm->GetManualControl()->SendCidMonitor();
		ADD_LOG_TASKCONTROL("[TP] CID_M_COMMAND_SEND");
		Sleep(200);
		ReadMCommandResult();
	}
	break;

	case TEXTID::CID_M_COMMAND_RESULT_READ:
	{
		Send_CID_INFO_2_OHT();
		ADD_LOG_TASKCONTROL("[TP] CID_M_COMMAND_RESULT_READ");
	}
	break;



	case TEXTID::IOMAP_SET_RESET_TP2OHT:
	{
		if(CheckStatusForManual())
		{
			PKT_OHT2TP_IO_SET CmdPkt;
			int iPacketSize = sizeof(PKT_OHT2TP_IO_SET);
			memcpy(&CmdPkt, pRcvData, iPacketSize);

			int iIndex = CmdPkt.BODY.iIoNum;
			int iValue = CmdPkt.BODY.iOnOff;

			OHTMainForm->GetManualControl()->SetOutBit(iIndex, iValue);

			ADD_LOG_TASKCONTROL("[TP] IOMAP_SET_RESET_TP2OHT(MAN = 1) - Index : %d  SetVal : %d", iIndex, iValue);
		}		
		else
		{
			ADD_LOG_TASKCONTROL("[TP] IOMAP_SET_RESET_TP2OHT(MAN = 0)");
		}
    }
	break;

	case TEXTID::TEACHING_INFO_SAVE:  // pRcvData
	{
		int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
		int iReturn;
		unsigned int Teaching_DataType = OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.DataType;
		AnsiString strTmp;

		PKT_OHT2TP_TEACHING_INFO	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		ADD_LOG_TASKCONTROL("[TP] TEACHING_INFO_SAVE : %d", Teaching_DataType);

		//Teaching 및 LookDown 데이터가 조정용인 경우 티칭데이터 저장
		if( (Teaching_DataType & bit_0) == bit_0)
		{
			//Teaching and LookDown 데이터가 조정용
			//티칭 데이터 저장
			UpdateTeachingData(&SEND_PACKET, false);
		}
		else
		{
			//티칭 데이터 저장 안함.
			//TP 명령 무시.
			Teaching_DataType = 0;
		}


		/*	PortType (SEND_PACKET.BODY.PortType)
		EQ			= 1
		EQ_X		= 2
		Stocker 	= 3
		Loader		= 4
		STB_Left	= 5
		STB_Right	= 6
		UTB
		*/

		/*	PIODirection (SEND_PACKET.BODY.EtcData.Info.PIODirection)
		Left	= 1
		Right	= 2
		*/
		#if 0
		strTmp.sprintf("테스트 인수 %d %d %d %d %d",
			SEND_PACKET.BODY.PortType,
			SEND_PACKET.BODY.EtcData.Info.PIODirection,
			SEND_PACKET.BODY.EtcData.Info.PIOTimeLevel,
			SEND_PACKET.BODY.EtcData.Info.LookDownLevel,
			SEND_PACKET.BODY.EtcData.Info.RFReader);

		ShowMessage(strTmp);
		#endif
	}
	break;


	case TEXTID::TEACHING_INFO_REQ:		// TP에서 OHT의 티칭 데이터를 요청한다
	{
		int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
		int iReturn;

		PKT_OHT2TP_TEACHING_INFO	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
		ACKResult = CMD_ACKNONE;

		//SendTeachingInfo_2_OHT();
		SendTeachingInfo_2_OHT(SEND_PACKET.BODY.PortType);

		ADD_LOG_TASKCONTROL("[TP] TEACHING_INFO_LOAD");
	}
	break;

    case TEXTID::DRIVINGMAP_INFO_SAVE:  // pRcvData
	{
		int iPacketSize = sizeof(PKT_OHT2TP_DRIVINGMAP_INFO);
		int iReturn;
		unsigned int Map_DataType = OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.DataType;
		AnsiString strTmp;

		PKT_OHT2TP_DRIVINGMAP_INFO	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		ADD_LOG_TASKCONTROL("[TP] DrivingMap_INFO_SAVE : %d", Map_DataType);

		EXECUTE_RESPONSE_MAPMAKING_INFO MapInfo;

		MapInfo.StationType = TAG_TYPE_MODIFY;
		MapInfo.NodeOffsetInfo[0].StartNodeID = SEND_PACKET.BODY.nStartNodeID;
		MapInfo.NodeOffsetInfo[0].EndNodeID = SEND_PACKET.BODY.nEndNodeID;
		MapInfo.NodeOffsetInfo[0].Dist =  SEND_PACKET.BODY.nMapMakeDist;

		//Teaching 및 LookDown 데이터가 조정용인 경우 티칭데이터 저장
		if( (Map_DataType & bit_0) == bit_0)
		{
			//Teaching and LookDown 데이터가 조정용
			//티칭 데이터 저장
			UpdateMapData(&MapInfo);
		}
		else
		{
			//티칭 데이터 저장 안함.
			//TP 명령 무시.
			Map_DataType = 0;
		}

	}
	break;

	//오아시스 내용 저장 puting
	 case TEXTID::OACIS_INFO_SAVE:
	{
		int iPacketSize = sizeof(PKT_OHT2TP_OACIS_INFO);
		int iReturn;

//		AnsiString strTmp;
		char szTmpLog[1024] ={0,};
		PKT_OHT2TP_OACIS_INFO	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);


	   switch(SEND_PACKET.BODY.Type)
	   {
			case CATEGORY_STEER:
			sprintf(szTmpLog, "/%s/%s/",
			OACISType(SEND_PACKET.BODY.TypeNum),
			OACISResult(SEND_PACKET.BODY.nData1));
			OACISLog(OACIS_LOG_STEER, szTmpLog);
			break;

			case CATEGORY_VHLDECT:
			sprintf(szTmpLog, "/%s/%s/Center:%dStartPos:%d-EndP:%d",
			OACISType(SEND_PACKET.BODY.TypeNum),
			OACISResult(SEND_PACKET.BODY.nData1),
			SEND_PACKET.BODY.nData2,
			SEND_PACKET.BODY.nData3,
			SEND_PACKET.BODY.nData4);
			OACISLog(OACIS_LOG_VHLDETECT, szTmpLog);
			break;

			case CATEGORY_OBSDECT:
			sprintf(szTmpLog, "/%s/%s/Center:%dStartPos:%d-EndP:%d",
			OACISType(SEND_PACKET.BODY.TypeNum),
			OACISResult(SEND_PACKET.BODY.nData1),
			SEND_PACKET.BODY.nData2,
			SEND_PACKET.BODY.nData3,
			SEND_PACKET.BODY.nData4);
			OACISLog(OACIS_LOG_OBSDETECT, szTmpLog);
			break;

			case CATEGORY_SHIFTHOME:
			sprintf(szTmpLog, "/%s/%s/Left:%d,Right:%d(0.1mm단위)",
			OACISType(SEND_PACKET.BODY.TypeNum),
			OACISResult(SEND_PACKET.BODY.nData1),
			SEND_PACKET.BODY.nData2,
			SEND_PACKET.BODY.nData3);
			OACISLog(OACIS_LOG_SHIFTHOME, szTmpLog);
			break;

			case CATEGORY_HOISTHOME:
			sprintf(szTmpLog, "/%s/%s/Down:%d(0.1mm단위)",
			OACISType(SEND_PACKET.BODY.TypeNum),
			OACISResult(SEND_PACKET.BODY.nData1),
			SEND_PACKET.BODY.nData2);
			OACISLog(OACIS_LOG_HOISTHOME, szTmpLog);
			break;

			case CATEGORY_SMPSVOLTAGE:
			sprintf(szTmpLog, "/%s/%s/%dV-%dV-%dV-%dV(0.1V단위)",
			OACISType(SEND_PACKET.BODY.TypeNum),
			OACISResult(SEND_PACKET.BODY.nData1),
			m_pHWSet->GetVolatageMonitor(0)&0xFFFF,
			(m_pHWSet->GetVolatageMonitor(0)>>16)&0xFFFF,
			m_pHWSet->GetVolatageMonitor(1)&0xFFFF,
			(m_pHWSet->GetVolatageMonitor(1)>>16)&0xFFFF);
			OACISLog(OACIS_LOG_SMPSVOLTAGE, szTmpLog);
			break;

	   }

	}
	break;

	case TEXTID::DRIVINGMAP_INFO_REQ:		// TP에서 OHT의 티칭 데이터를 요청한다
	{
		int iPacketSize = sizeof(PKT_OHT2TP_DRIVINGMAP_INFO);
		int iReturn;

		PKT_OHT2TP_DRIVINGMAP_INFO	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
		ACKResult = CMD_ACKNONE;

		//SendTeachingInfo_2_OHT();
		SendDrivingMapInfo_2_OHT(SEND_PACKET.BODY.nStartNodeID,SEND_PACKET.BODY.nEndNodeID);

		ADD_LOG_TASKCONTROL("[TP] DrinvMap_INFO_LOAD");
	}
	break;
    case TEXTID::TEACHING_LOOKDOWN_LEVEL_SET:   // Handy TP에서 LookDown Level 설정 시 사용
    {
		int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
		int iReturn;
		unsigned int Teaching_DataType = OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.DataType;
		AnsiString strTmp;

		PKT_OHT2TP_TEACHING_INFO	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		ADD_LOG_TASKCONTROL("[TP] TEACHING_INFO_SAVE");

		//Teaching 및 LookDown 데이터가 조정용인 경우 티칭데이터 저장
		if( (Teaching_DataType & bit_0) == bit_0)
		{
			//Teaching and LookDown 데이터가 조정용
			//티칭 데이터 저장
			UpdateTeachingData(&SEND_PACKET, true);
		}
		else
		{
			//티칭 데이터 저장 안함.
			//TP 명령 무시.
			Teaching_DataType = 0;
		}
    }
	break;




	case TEXTID::TRANS_LOAD: 	// 재하 LOAD (장비로 내려놓음)
	{
		int iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
		int iReturn;
		AnsiString strTmp;

		if((m_pExecuteInfo->ExecutePositionInfo.Marked == false && bHoistJig == false))
		{
			bCheckInsertTaskList = false;
			break;
		}

		PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		ADD_LOG_TASKCONTROL("[UPDOWN] TRANS_LOAD");

		ADD_LOG_TASKCONTROL("테스트 인수 CurNodeID : %d CurStationID : %d PortType : %d PIODirection : %d HoistSpeedLevel : %d",
			SEND_PACKET.BODY.CurNodeID,
			SEND_PACKET.BODY.CurStationID,
			SEND_PACKET.BODY.PortType,
			SEND_PACKET.BODY.PIODirection,
			SEND_PACKET.BODY.HoistSpeedLevel);

		PARSING_TRANS_INFO tmpCmd;

		tmpCmd.CmdID = iManualTransCmdCnt++;	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
		tmpCmd.CmdCycle = CYCLE::CraneDown;
		if(SEND_PACKET.BODY.TransOption.Info.AutoRecovery == 1)	// this is bit field
			tmpCmd.AutoRecovery = 1;	// this is 1 byte
		else
			tmpCmd.AutoRecovery = 0;
		tmpCmd.TrasType = 0; 				// Not Used!!
		// Not Used!! tmpCmd.StationData_PortID[]

		memcpy(&tmpCmd.TransCmd, (TRANS_CMD *)&SEND_PACKET.BODY, sizeof(TRANS_CMD));

		if(CheckStatusForManual())
			MakeTransCycleList(&tmpCmd);
	}
	break;

	case TEXTID::TRANS_UNLOAD: 	// 재하 UNLOAD (장비에서 꺼내 OHT로 이동)
	{
		int iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
		int iReturn;
		AnsiString strTmp;

		PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어

		ADD_LOG_TASKCONTROL("[UPDOWN] TRANS_UNLOAD");

		if((m_pExecuteInfo->ExecutePositionInfo.Marked == false && bHoistJig == false))
		{
			bCheckInsertTaskList = false;
			break;
		}


		PARSING_TRANS_INFO tmpCmd;

		tmpCmd.CmdID = iManualTransCmdCnt++;	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
		tmpCmd.CmdCycle = CYCLE::CraneUp;
		if(SEND_PACKET.BODY.TransOption.Info.AutoRecovery == 1) // this is bit field
			tmpCmd.AutoRecovery = 1;	// this is 1 byte
		else
			tmpCmd.AutoRecovery = 0;
		tmpCmd.TrasType = 0;				// Not Used!!
		// Not Used!! tmpCmd.StationData_PortID[]

		memcpy(&tmpCmd.TransCmd, (TRANS_CMD *)&SEND_PACKET.BODY, sizeof(TRANS_CMD));

		if(CheckStatusForManual())
			MakeTransCycleList(&tmpCmd);

	}
	break;

	case TEXTID::TRANS_MOVE_TEACHING_POINT: 	// TEACHING POINT로 이동하도록 지시
	{
		int iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
		int iReturn;
		AnsiString strTmp;

		PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		ADD_LOG_TASKCONTROL("[TP] TRANS_MOVE_TEACHING_POINT");

		ADD_LOG_TASKCONTROL("테스트 인수 CurNodeID : %d CurStationID : %d PortType : %d PIODirection : %d HoistSpeedLevel : %d",
			SEND_PACKET.BODY.CurNodeID,
			SEND_PACKET.BODY.CurStationID,
			SEND_PACKET.BODY.PortType,
			SEND_PACKET.BODY.PIODirection,
			SEND_PACKET.BODY.HoistSpeedLevel);

        PARSING_TRANS_INFO tmpCmd;

        tmpCmd.CmdID = iManualTransCmdCnt++;	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
        tmpCmd.CmdCycle = CYCLE::TP_CheckPortTeaching;
        if(SEND_PACKET.BODY.TransOption.Info.AutoRecovery == 1)	// this is bit field
            tmpCmd.AutoRecovery = 1;	// this is 1 byte
        else
            tmpCmd.AutoRecovery = 0;
        tmpCmd.TrasType = 0; 				// Not Used!!
        // Not Used!! tmpCmd.StationData_PortID[]

		memcpy(&tmpCmd.TransCmd, (TRANS_CMD *)&SEND_PACKET.BODY, sizeof(TRANS_CMD));

		if(CheckStatusForManual())
			MakeTransCycleList(&tmpCmd);
	}
	break;

	case TEXTID::STATIONINFOREQ_TP2OHT: 	// Station 정보 요구
	{
		if(CheckStatusForManual())
		{
			PKT_ACK_OHT2OCS  TempPkt;
			TempPkt.AckPkt.TAG.Value = TagID::CMD_STATION_INFO_REQUST_OHT2OCS;
		    PROTOCOL_BODY_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111 Temp;
			memmove(&Temp, &pRcvData->Value, sizeof(PROTOCOL_BODY_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111));
			memmove(&TempPkt.AckPkt.Value, &pRcvData->Value, sizeof(PROTOCOL_BODY_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111));
			TempPkt.Length = sizeof(PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111);


			ADD_LOG_TASKCONTROL("==Station 정보 요구== %d %d %d",Temp.StopStationID[0],Temp.StopStationID[1],Temp.StopStationID[2]);

			MakeResponseStandardOHT2OCS(&TempPkt);
		}
	}
	break;

	case TEXTID::STATION_INFO_RESPONSE: 	// Station 정보 응답
	{
		if(CheckStatusForManual())
		{
			ADD_LOG_TASKCONTROL("==Station 정보 요구 응답 ==");
			pTempCmdPktData.CategoryDiv =CATEGORY_TERTIARY;
			pTempCmdPktData.DetailTagSort = TAGTYPE_STATIONMOVEINFO;

			bCheck = CheckStaionInfoReqResponse(pRcvData, QuickCmd);
			if(bCheck) 	bCheckInsertTaskList = true;
		}
	}
	break;

	case TEXTID::TP_SOUND_ONOFF:
	{
		int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		int iReturn;
		AnsiString strTmp;

		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		if(SEND_PACKET.BODY.iVar1 == 0x01)	// Sound On
		{
			OHTMainForm->GetManualControl()->SoundOn(SEND_PACKET.BODY.iVar2);
		}
		else if(SEND_PACKET.BODY.iVar1 == 0x02)	// Sound Off
		{
			OHTMainForm->GetManualControl()->SoundOff();
		}
	}
	break;

	case TEXTID::TP_POWER_ONOFF:
	{
		int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		int iReturn;
		AnsiString strTmp;

		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		if(CheckStatusForManual())
		{	
			ADD_LOG_TASKCONTROL("TEXTID::TP_POWER_ONOFF - now Manual mode");
			ADD_LOG_TASKCONTROL("Power Info - Axis : %d, OnOff : %d", SEND_PACKET.BODY.iVar1, SEND_PACKET.BODY.iVar2);
			OHTMainForm->GetManualControl()->PowerOnOff(SEND_PACKET.BODY.iVar1, SEND_PACKET.BODY.iVar2);
		}
		else
		{
			ADD_LOG_TASKCONTROL("TEXTID::TP_POWER_ONOFF - now auto mode");
		}
	}
	break;

	case TEXTID::TP_AMC_UPDATE:
	{
		int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		int iReturn;
		AnsiString strTmp;

		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		if(CheckStatusForManual())
		{	
			ADD_LOG_TASKCONTROL("TEXTID::TP_AMC_UPDATE - now Manual mode");
			UpdateAMCDownloadFile(AMC_BINARY_PATH);
		}	
		else
		{
			ADD_LOG_TASKCONTROL("TEXTID::TP_AMC_UPDATE - now auto mode. can not amc update");
		}
	}
	break;

	case TEXTID::TP_OCS_PAUSE:
	{
		int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		int iReturn;
		AnsiString strTmp;

		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_PAUSE_OCS);
	}
	break;

	case TEXTID::TP_OCS_RESUME:
	{
		int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		int iReturn;
		AnsiString strTmp;

		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_RESUME_OCS);
	}
	break;

	case TEXTID::TP_TP_PAUSE:
	{
		int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		int iReturn;
		AnsiString strTmp;

		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_PAUSE_TP);
	}
	break;

	case TEXTID::TP_TP_RESUME:
	{
		int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		int iReturn;
		AnsiString strTmp;

		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_RESUME_TP);
	}
	break;



	case TEXTID::TP_MAPMAKER_FILE:     //파일참조하여 이동( 일반이동은 싸이클 이동명령으로 줄것.)
		bCheck = CheckMapMakerFile(1);
		ADD_LOG_TASKCONTROL("[TP_MAPMAKER_FILE] %d",bCheck);
		if(bCheck)
		{
			//현재 진행중이 아닐 경우만, 현재 싸이클 중이 아니고
			//주행 Node Mark 상태아니면 처리안함.
			if((m_pExecuteInfo->ExecutePositionInfo.Marked) &&
			   (m_pExecuteInfo->ExecutePositionInfo.CurrStation == 0))
			   {
					if((m_FileMapMakeflag ==false) && (m_FileTeachingflag ==false) &&
					CheckStatusForCycle() &&
					(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist))
					{
						//map file 초기화 확인

						OHTMainForm->GetMapMakerFileManager_STD()->InitFindUpdate();
						m_FileMapMakeflag = true;
						ADD_LOG_TASKCONTROL("[TP_MAPMAKER_FILE] Complet");
					}

				}

		}
	break;

	case TEXTID::TP_AUTOTEACHING_FILE: //파일참조하여 티칭
		bCheck = CheckMapMakerFile(2);
		ADD_LOG_TASKCONTROL("[TP_AUTOTEACHING_FILE] %d",bCheck);
		if(bCheck)
		{

			if(m_pExecuteInfo->ExecutePositionInfo.Marked)  //없어져야될수도 있음 puting
			{
				//현재 진행중이 아닐 경우만, 현재 싸이클 중이 아니고     puting
				if((m_FileMapMakeflag ==false) && (m_FileTeachingflag ==false) &&
				   CheckStatusForCycle() &&
				   (m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist))
				{
					//STB, FRT를 앞을 기준으로 왼쪽 445.5mm  오른쪽 456mm   puting

					m_FileTeachingflag = true;
					ADD_LOG_TASKCONTROL("[TP_AUTOTEACHING_FILE] Complet");
				}
				else
				{

				}
			}
			//있음
		}
	break;

	case TEXTID::TP_AUTOTEACHING:   //현재위치에서 티칭
			if((m_FileMapMakeflag ==false) && (m_FileTeachingflag ==false) &&
			(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist) &&
			CheckStatusForCycle())
			{

				int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
				int iReturn;

				PKT_OHT2TP_TEACHING_INFO	SEND_PACKET;
				memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
				memcpy(&SEND_PACKET, pRcvData, iPacketSize);

				//사용시 정보 입력하여 살릴것.
				if((m_pExecuteInfo->ExecutePositionInfo.CurrStation !=0) && (m_pExecuteInfo->ExecutePositionInfo.Marked ==true))
				{

					MakeAutoTeachingInfo(m_pExecuteInfo->ExecutePositionInfo.CurrStation, false,SEND_PACKET.BODY.PortType);
				}
			}
	break;

	case TEXTID::TP_AUTOTEACHING_NULL_TYPE:   //현재위치에서 티칭
			if((m_FileMapMakeflag ==false) && (m_FileTeachingflag ==false) &&
			(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist) &&
			CheckStatusForCycle())
			{

				int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
				int iReturn;

				PKT_OHT2TP_TEACHING_INFO	SEND_PACKET;
				memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
				memcpy(&SEND_PACKET, pRcvData, iPacketSize);

				//사용시 정보 입력하여 살릴것.
				if((m_pExecuteInfo->ExecutePositionInfo.CurrStation !=0) && (m_pExecuteInfo->ExecutePositionInfo.Marked ==true))
				{

					MakeAutoTeachingInfo(m_pExecuteInfo->ExecutePositionInfo.CurrStation, false,SEND_PACKET.BODY.PortType,true);
				}
			}
	break;


	case TEXTID::TP_AUTOTEACHING_READ_DRIVING:   //현재위치에서 티칭
		if(CheckStatusForManual())
		{
			//사용시 정보 입력하여 살릴것.

			int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
			int iReturn;

			PKT_OHT2TP_TEACHING_INFO	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
			memcpy(&SEND_PACKET, pRcvData, iPacketSize);

			pTempCmdPktData.CategoryDiv = CATEGORY_PRIORITY;
			ACKResult = CMD_ACKNONE;

			//응답
			SEND_PACKET.BODY.Reserved2 = Load_AutoTeaching_DrvingData(SEND_PACKET.BODY.Reserved1,SEND_PACKET.BODY.PortType);
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
			SEND_PACKET.TAG.Value = TagID::CMD_TP_AUTOTEACHING_LOAD_DRIVING;

			//SendTeachingInfo_2_OHT();
		   //	SendTeachingInfo_2_OHT(SEND_PACKET.BODY.PortType);
            iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
			ADD_LOG_TASKCONTROL("[TP] TP_AUTOTEACHING_LOAD_DRIVING");


		}
	break;

/*
	#define HID_RESULT_RESPONSE	1	// 응답을 수신
	#define HID_RESULT_COMPLETE 2	// 동작 완료
*/
	case TEXTID::TP_HID_LOG_SAVE_ORDER:
		{
			ADD_LOG_TASKCONTROL("[TP] TP_HID_LOG_SAVE_ORDER - Received");
				
			int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
			int iReturn, iResult;
			AnsiString strTmp;

			// Step 1. Order Parsing
			PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
			memcpy(&SEND_PACKET, pRcvData, iPacketSize);

			// Step 2. Response
			memset(&SEND_PACKET, 0, iPacketSize);
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
			SEND_PACKET.TAG.Value = TagID::CMD_TP_HID_LOG_SAVE_RESULT;
			SEND_PACKET.BODY.iVar1 = 1;		// HID_RESULT_RESPONSE
			iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);

			ADD_LOG_TASKCONTROL("[TP] TP_HID_LOG_SAVE_ORDER - Response Send");
			
			// Step 3. Order Run
			iResult = OHTMainForm->GetStatusCheckThread()->SaveHID_Log();

			// Step 4. Result Return
			memset(&SEND_PACKET, 0, iPacketSize);
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
			SEND_PACKET.TAG.Value = TagID::CMD_TP_HID_LOG_SAVE_RESULT;
			SEND_PACKET.BODY.iVar1 = 2;		// 동작 완료
			SEND_PACKET.BODY.iVar2 = iResult;
			iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);

			ADD_LOG_TASKCONTROL("[TP] TP_HID_LOG_SAVE_ORDER - Result. iVar2 : %d", SEND_PACKET.BODY.iVar2);
		}
	break;

	case TEXTID::TP_HOIST_TEACH_MOVE_TO_PULSE:
	{
		ADD_LOG_TASKCONTROL("[TP] TP_HOIST_TEACH_MOVE_TO_PULSE - Received");
			
		int iPacketSize = sizeof(PKT_TP2OHT_HOIST_TEACH_COMMAND);
		int iReturn, iResult;
		AnsiString strTmp;
		
		// Step 1. Order Parsing
		PKT_TP2OHT_HOIST_TEACH_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		// Step 2. Command Run
		OHTMainForm->GetManualControl()->MoveByPulse(SEND_PACKET.BODY.PositionValue[0].dPulse, SEND_PACKET.BODY.PositionValue[1].dPulse);

	}
	break;

	case TEXTID::TP_HOIST_TEACH_LOAD_VALUE:
	{
		ADD_LOG_TASKCONTROL("[TP] TP_HOIST_TEACH_LOAD_VALUE - Received");
			
		int iPacketSize = sizeof(PKT_TP2OHT_HOIST_TEACH_COMMAND);
		int iReturn, iResult;
		AnsiString strTmp;
		
		// Step 1. Order Parsing
		PKT_TP2OHT_HOIST_TEACH_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
		memcpy(&SEND_PACKET, pRcvData, iPacketSize);

		// Step 2. Command Run
		double dblPalse, dblUser;

		OHTMainForm->DeviceDifference_Read(dblPalse);
        SEND_PACKET.BODY.dHomeOffsetOfRailToJigOnUserUnit = dblPalse;

#ifdef OHT_HIGH_RAIL_HOIST_PRAM
		for (int i = 0; i < 10; i++)
		{
			 OHTMainForm->HoistParam_Read(i, dblPalse, dblUser);
			 SEND_PACKET.BODY.PositionValue[i].dPulse 		= dblPalse;
			 SEND_PACKET.BODY.PositionValue[i].dUserUnit 	= dblUser;
		}
#else
		for (int i = 0; i < 7; i++)
		{
			 OHTMainForm->HoistParam_Read(i, dblPalse, dblUser);
			 SEND_PACKET.BODY.PositionValue[i].dPulse 		= dblPalse;
			 SEND_PACKET.BODY.PositionValue[i].dUserUnit 	= dblUser;
		}
#endif

		// Step 3. Result Send
		SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
		SEND_PACKET.TAG.Value = TagID::CMD_TP_HOIST_TEACH_LOAD_RESULT;

		iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
		ADD_LOG_TASKCONTROL("[TP] CMD_TP_HOIST_TEACH_LOAD_RESULT");
	}
	break;

	case TEXTID::TP_HOIST_TEACH_SAVE_VALUE:
	{
			ADD_LOG_TASKCONTROL("[TP] TP_HOIST_TEACH_SAVE_VALUE - Received");
				
			int iPacketSize = sizeof(PKT_TP2OHT_HOIST_TEACH_COMMAND);
			int iReturn, iResult;
			AnsiString strTmp;
			
			// Step 1. Order Parsing
			PKT_TP2OHT_HOIST_TEACH_COMMAND	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
			memcpy(&SEND_PACKET, pRcvData, iPacketSize);
	
			// Step 2. Command Run
			OHTMainForm->HoistParamSaveParameter(SEND_PACKET);
	}
	break;

	case TEXTID::TP_VIB_DATA_LINK:
	{
		int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		int iReturn;
		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);
		/*
		if(OHTMainForm->GetAvsControl()->LinkSensor())
		{
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
			SEND_PACKET.TAG.Value = TagID::CMD_TP_VIB_DATA_LINK;
			iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
			ADD_LOG_TASKCONTROL("[TP] AVS Linke Success");
		}
		else
		{
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
			SEND_PACKET.TAG.Value = TagID::CMD_TP_VIB_DATA_CLOSE;
			iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
			ADD_LOG_TASKCONTROL("[TP] AVS Linke Fail");
		}
		*/
	}
	break;

	case TEXTID::TP_VIB_DATA_REQ_START:
	{
//		OHTMainForm->GetAvsControl()->StartReceiveVibData();
	}
	break;

	case TEXTID::TP_VIB_DATA_REQ_STOP:
	{
//		OHTMainForm->GetAvsControl()->StopReceiveVibData();
	}
	break;

	case TEXTID::TP_VIB_DATA_CLOSE:
	{
//		OHTMainForm->GetAvsControl()->EndReceiveVibData();

		int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		int iReturn;
		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);
	   /*
		if(OHTMainForm->GetAvsControl()->EndReceiveVibData())
		{
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
			SEND_PACKET.TAG.Value = TagID::CMD_TP_VIB_DATA_CLOSE;
			iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
		}
		*/
	}
	break;

	case TEXTID::TP_COMMON:
	{
		// ----- 터널링으로 도착한 구 TLV 패킷을 꺼낸다 ----
		/*
		CMD_BIG_HEADER Header;  // Header를 parsing한 구조체
		uTP_CMD     m_TPCmd;
		//char pTLV[LEN_TLV_BUFFER]= {0,};

		DecodeSize = DecodeTPCmd((char*)&pRcvData->Value, &Header, &m_TPCmd);

		if(DecodeSize > 0) 	// UDP이므로 항상 0보다 크다
		{
			switch(Header.uTag.Info.CmdData)
			{
				case TPCMD_TEACHING_SAVE:
					ShowMessage("TPCMD_TEACHING_SAVE");
				break;

				case TPCMD_TEACHING_REQ:
					ShowMessage("TPCMD_TEACHING_REQ");
				break;
			}
		}

		shkim. test중단
		*/
		// ------------------------------------------------
		if (GetCurrentMode() == MODE_MANUAL)
		 {
			pTempCmdPktData.DetailTagSort = TAGTYPE_MANUAL_COMMON;
			pTempCmdPktData.CategoryDiv = CATEGORY_TERTIARY;
			bCheckInsertTaskList = true;
		 }
	}
		break;

	default:
		break;

	}



	// Step.2 CMD ID List 추가 및 접수 가능 유무 판단 처리.
	/*
	 - 자동 동작/기동 ON 시에는 Manual 명령 or PC(HandTP)의 명령을 수행할 수 없고, OCS의 명령만 수행 가능하다.
	 - 자동 동작/기동 OFF시에는 OCS로 부터의 명령 및 Manual&PC(HandTP) 명령 모두 무시된다.
	 - 해당 명령은 자동 수동 상관없이 모두 호출가능하며, 여기서 분류되어 처리가능한 명령은 처리해 준다.
	 */

	 switch (pTempCmdPktData.CategoryDiv)
	 {
	 //우선 처리 명령(Auto, Manual, Reset 등 _ Monitoring 정보 포함) : 자동/수동, 기동 On/off, 에러 상관없는 명령들
	 case CATEGORY_PRIORITY:
	 case CATEGORY_PRIMARY:
		break;
	 //둘쨰 명령 :  Error가 없이, 기동 On, 자동 일떄만 처리되는 명령들.  : OCS 명령들
	 case CATEGORY_SECONDARY:
		 if((GetCurrentMode() == MODE_MANUAL) ||
			 (GetExecuteStatus() == STS_ERROR) ||
			((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF)) ||
			((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning ==OFF)))
			{

			   	if (bCheckInsertTaskList == true)  bCheckInsertTaskList =false;
			}
		break;

	 //셋째 명령 : Error가 없이 수동 상태에서, 기동 off에만 처리되는 명령들. : 메뉴얼 동작
	 case CATEGORY_TERTIARY:
		 if((GetCurrentMode() == MODE_AUTO) ||
			 (GetExecuteStatus() == STS_ERROR))
			{
				if (bCheckInsertTaskList == true)  bCheckInsertTaskList =false;
			}

		 break;
	 case CATEGORY_ETC:
	 default:
		 break;
	 }


//	ADD_LOG_TASKCONTROL("Catagory:%d,TextID:%d, bCheckInsertTaskList %d QuickCmd %d", nTagCategory, nTagID, bCheckInsertTaskList, QuickCmd);


	// Step.3 Task List에 접수 가능 할때 접수하는 부분 처리 .
	if (bCheckInsertTaskList == true)
	{

		memmove(&(pTempCmdPktData.PKTDATA), pRcvData, sizeof(PKT_OCS2OHT));
		// TASK CMD LIst에 추가될 경우에 여기 추가
		// 순서에 맞게 저장할 것.
		m_pCmdManager->MakeTaskList_Standard(&pTempCmdPktData);
	}
//	else {
//		delete pTempCmdPktData;
//
//	}
	//////////////////////////////////////////////////
	// Step 3. Task(Cmd) List에 추가가 필요할 경우 추가.
	// OHT가 처리 가능한 상태이면 List에 저장//////////

	return ACKResult;

}

void TaskControl::Send_AutoTeachResult(int iResult)
{
	int iReturn;
	int iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
	PKT_TP2OHT_SIMPLE_COMMAND SEND_PACKET;

	memset(&SEND_PACKET, 0, iPacketSize);	  
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	SEND_PACKET.TAG.Value = TagID::CMD_TP_DEBUG_RESULT;	
	SEND_PACKET.BODY.iVar1 = iResult;
	
	iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
}

void TaskControl::Send_CID_INFO_2_OHT()
{
	int iReturn;
	int iPacketSize = sizeof(PKT_OHT2TP_CID_INFO);
	int iReceivedByte = 0;
	PKT_OHT2TP_CID_INFO SEND_PACKET;
	


	// ready
	memset(&SEND_PACKET, 0, iPacketSize);
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	SEND_PACKET.TAG.Value = TagID::CMD_CID_M_COMMAND_RESULT_RETURN;

	// send direct
	memcpy(&SEND_PACKET.BODY, &m_pDrivingControl->TEST_PKT_Monitor.BODY, sizeof(CID_Monitoring_BODY));
	iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
	ADD_LOG_TASKCONTROL("[CID INFO] OHT -> TP Info Send direct!");	

}

void TaskControl::ReadMCommandResult()
{
	int iReceivedByte = 0;
	char TempBuffer[10240];
	
	iReceivedByte = m_pHWSet->m_pCID->ReadData((char *)&TempBuffer, (char)OPC_SERIAL_INIT_ETX);
	
	if(iReceivedByte > 1)
	{
		if(TempBuffer[1] == 'M')
		{
			if(iReceivedByte == sizeof(CID_Monitoring_PKT))
			{
				memcpy(&m_pDrivingControl->TEST_PKT_Monitor, &TempBuffer, iReceivedByte);
				ADD_LOG_TASKCONTROL("[CID M Cmd Read - TaskControl] Read Ok.");

			}
			else
			{
				ADD_LOG_TASKCONTROL("[CID M Cmd Read - TaskControl] Packet Size mismatch");
			}	
		}
		else
		{
			ADD_LOG_TASKCONTROL("[CID M Cmd Read - TaskControl] Result Is not M");
		}
	}
	else
	{
		ADD_LOG_TASKCONTROL("[CID M Cmd Read - TaskControl] Data Not Found");
	}

	ADD_LOG_TASKCONTROL("[CID M Cmd Result - TaskControl] CID R : %c%c%c  CID O : %c%c%c",
		m_pDrivingControl->TEST_PKT_Monitor.BODY.CIDR_FirstLinkLevel[0],
		m_pDrivingControl->TEST_PKT_Monitor.BODY.CIDR_FirstLinkLevel[1],
		m_pDrivingControl->TEST_PKT_Monitor.BODY.CIDR_FirstLinkLevel[2],

		m_pDrivingControl->TEST_PKT_Monitor.BODY.CIDO_FirstLinkLevel[0],
		m_pDrivingControl->TEST_PKT_Monitor.BODY.CIDO_FirstLinkLevel[1],
		m_pDrivingControl->TEST_PKT_Monitor.BODY.CIDO_FirstLinkLevel[2]);

}
void TaskControl::SendIoStatus_2_OHT()
{
	 int iReturn;
	 int iPacketSize = sizeof(PKT_OHT2TP_IOLIST);
	 int Offset = 0;
	 BYTE strLen = 0;
	 PKT_OHT2TP_IOLIST SEND_PACKET;
	 
	 memset(&SEND_PACKET, 0, iPacketSize);		 
	 SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	 SEND_PACKET.TAG.Value = TagID::CMD_IOMAP_RESPONSE_OHT2TP;
	 
	 SEND_PACKET.BODY.iOhtInputNum	= m_IOMapField->InputNum;
	 SEND_PACKET.BODY.iOhtOutputNum = m_IOMapField->OutputNum;

	 for(int i=0; i<m_IOMapField->InputNum; i++)
	 {
		 //문자열 길이를 먼저 넣음
		 strLen = strlen(m_IOMapField->InputField[i].c_str())+1;
		 memcpy((char*)&SEND_PACKET.BODY.cIONameList + Offset, &strLen, sizeof(BYTE));
		 Offset += sizeof(BYTE);

		 //맨뒤에 널문자 추가
		 m_IOMapField->InputField[i] += "\0";

		 //문자열을 넣음
		 memcpy((char*)&SEND_PACKET.BODY.cIONameList + Offset, m_IOMapField->InputField[i].c_str(), strLen);
		 Offset += strLen;
	 }

	 for(int i=0; i<m_IOMapField->OutputNum; i++)
	 {
		 //문자열 길이를 넣음
		 strLen = strlen(m_IOMapField->OutputField[i].c_str())+1;
		 memcpy((char*)&SEND_PACKET.BODY.cIONameList + Offset, &strLen, sizeof(BYTE));
		 Offset += sizeof(BYTE);

		 //맨뒤에 널문자 추가
		 m_IOMapField->OutputField[i] += "\0";

		 //문자열 넣음
		 memcpy((char*)&SEND_PACKET.BODY.cIONameList + Offset, m_IOMapField->OutputField[i].c_str(), strLen);
		 Offset += strLen;
	 }

	 iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
 }
// void TaskControl::SendTeachingInfo_2_OHT()
void TaskControl::SendTeachingInfo_2_OHT(BYTE portType)
 {
	int iReturn;
	int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
	PKT_OHT2TP_TEACHING_INFO SEND_PACKET;

	memset(&SEND_PACKET, 0, iPacketSize); 	  
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	SEND_PACKET.TAG.Value = TagID::CMD_TEACHING_INFO_LOAD;	  

	// 이 부분에서 실제 들어갈 내용을 써 주면 된다

	UINT stationID = 0;
    //stationID = m_StatusOHT2TP.Data.CurStationID; //stationID 받아오는 부분   m_StatusOHT2TP 사용 안함.
    stationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;//stationID 받아오는 부분
//	stationID = 10007;//테스트 용도
	
//    portType = (BYTE)uSndTPCmd->TeachingSave.PortType;

	TEACHING_DATA *teachingData = OHTMainForm->GetTeachingDataManager()->Find(stationID, portType);

	// by zzang9un 2013.07.19 : 찾지 못한 경우에는 Porttype에 0을 전송
    if(teachingData == NULL)
    {
        SEND_PACKET.BODY.PortType = 0;
    }
	// 이미존재하는 경우는 값을 보낼 프로토콜에 축의 값을 넣는다.
    else
    {
        SEND_PACKET.BODY.PortType = teachingData->PortType;
//		SEND_PACKET.BODY.CmdID = 0;
        SEND_PACKET.BODY.Hoist = teachingData->Hoist;
        SEND_PACKET.BODY.Shift = teachingData->Shift;
        SEND_PACKET.BODY.Rotate = teachingData->Rotate;
		SEND_PACKET.BODY.PIOType = teachingData->PIOType+1;	// 1 : COMBOBOX_OFFSET
		SEND_PACKET.BODY.PIOID = teachingData->PIOID;
		SEND_PACKET.BODY.PIOChannel = teachingData->PIOChannel;
        SEND_PACKET.BODY.EtcData.Info.PIODirection = teachingData->PIODirection;
        SEND_PACKET.BODY.EtcData.Info.PIOTimeLevel = teachingData->PIOTimeLevel;
        SEND_PACKET.BODY.EtcData.Info.LookDownLevel = teachingData->LookDownLevel;
        SEND_PACKET.BODY.EtcData.Info.HandDetectEQ = teachingData->TransOption.Info.HandDetectEQ;
		SEND_PACKET.BODY.EtcData.Info.AutoRecovery = teachingData->TransOption.Info.AutoRecovery;

	}

	// ----------------------------------------------------
	
	iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
 }

 // void TaskControl::SendTeachingInfo_2_OHT()
void TaskControl::SendDrivingMapInfo_2_OHT(int StartNID, int EndNID)
 {
	int iReturn;
	int iPacketSize = sizeof(PKT_OHT2TP_DRIVINGMAP_INFO);
	PKT_OHT2TP_DRIVINGMAP_INFO SEND_PACKET;

	memset(&SEND_PACKET, 0, iPacketSize);
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	SEND_PACKET.TAG.Value = TagID::CMD_DRIVINGMAP_INFO_LOAD;


	int PathIndex = 0;

	// NextIndex 정보로 찾을수 있는 방법 찾을것... 김정대 선임한테 문의
	PathIndex = OHTMainForm->GetMap()->GetPathIndex(StartNID, EndNID);

	if (PathIndex == INDEX_NONE) {
		// by zzang9un 2013.07.24 : Map은 에러를 처리해서 진행이 안되도록 한다.
		ADD_LOG_MAPMAKE("[확인필요][Error][ERR_MAPMAKE_NOTFOUNDNODEID](%06d~%06d)"
					, StartNID, EndNID);

         // by zzang9un 2013.07.19 : 찾지 못한 경우에는 0을 전송
	   SEND_PACKET.BODY.nStartNodeID =0;
	   SEND_PACKET.BODY.nEndNodeID = 0;
	   SEND_PACKET.BODY.nMapMakeDist = 0;

	}
	else
	{

		// Step 2. 해당 PID의 Distance 정보를 업데이트 한다.
		//기존 Map Data Update
		SEND_PACKET.BODY.nStartNodeID =StartNID;
		SEND_PACKET.BODY.nEndNodeID = EndNID;
		SEND_PACKET.BODY.nMapMakeDist = OHTMainForm->GetMap()->m_PathArray[PathIndex].Distance;
	}

	// ----------------------------------------------------

	iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
 }

bool TaskControl::CheckJobChange(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121 *Cmd121,BYTE &chNackToAckType)
{
		bool breturn = false;
		int nTempError = NO_ERR;
		bool bCheck = false;
		bool bCheckStop = false;

		// 시작위치(현재 위치가) Target 위치일 경우 인지 먼저확인      X
		// 아닐 경우                                       X
		//명령을 받은 분기와 비교해서 변경가능한지 확인.
		// 현재 경로에서의 분기 정보 찾기.
		//만약에 가능한 경우 해당 분기까지 정보들을 삭제함.
		//Cycle List 삭제(수정) 및 pathList 정보들 삭제
		//시작 위치는 성공한 분기위치부터 시작위치
		//그리고 나머지는 Task 관련해서 처리할때 새로 만들어줌.
		PARSING_GO_INFO TempGOInfo;
		memset(&TempGOInfo, 0x00, sizeof(PARSING_GO_INFO));
		UINT ChangeSqNum =0;
		int ChangeNodeID =0;
		int ChangeStationID =0;
		double ChangeNodeDistance =0.0;
		double dLastDistance =0.0;      //남은 거리
		double dCheckDistance =0.0;     //정지가능한 이동 거리.
		double dCheckDistance_margin = 0.0;  //Nack 개선 추가
		PKT_CYCLECMDDATA* pTempCmdData;

		int nTempExtraCount = 0;    //경로 변경위치 찾을때까지 이동한 값.
        unsigned int  			TempPathData_ExtraID[MAX_PATH_SIZE];

		pTempCmdData = (PKT_CYCLECMDDATA*)m_pCmdManager->ReferCycle(m_pPassPermit->GetPPIndex());


		m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag = true;
//		m_pOHTMainStatus->JobChangeFlag.StartStationID = m_pOHTMainStatus->FinalStationOfPathSearch;


		if (Cmd121->PathData_Def_Direction.Driving_Flag)
		{
//			TempGOInfo.CmdID = tmpID121.CmdID;
//			TempGOInfo.CmdCycle = tmpID121.CmdCycle;
			TempGOInfo.StationData_ParentNodeID = Cmd121->StationData_ParentNodeID;
			TempGOInfo.StationData_NextNodeID = Cmd121->StationData_NextNodeID;
			TempGOInfo.StationData_RunDist = Cmd121->StationData_Offset;
			TempGOInfo.PathData_Def_Direction = Cmd121->PathData_Def_Direction.Path_Direction;
			TempGOInfo.PathData_ExtraNum = Cmd121->PathData_ExtraNum;


			for (int i = 0; i < 3; i++)
			{
				// Node 비트연산   puting
				int Temp1 = Cmd121->StopNodeID[i];
				int Temp2 = Cmd121->StopStationID[i];
//					int Tmep3 = Cmd121->StationData_NextNodeID[i];
                int Tmep4 = Cmd121->PathData_StartNodeID[i];
				int Tmep5 = Cmd121->PathData_EndNodeID[i];
//				int Temp6 = Cmd121->StationData_NextNodeID[i];


				if (HIGH_BYTE == 0) {
					TempGOInfo.StopNodeID += Temp1 << ((2 - i) * 8);
					TempGOInfo.StopStationID +=	Temp2 << ((2 - i) * 8);
//						TempGOInfo.NextNodeID += Tmep3<< ((2 - i) * 8);
					TempGOInfo.PathData_StartNodeID += Tmep4 << ((2 - i) * 8);
					TempGOInfo.PathData_EndNodeID += Tmep5 << ((2 - i) * 8);
//					TempGOInfo.StationDataStart_NextNodeID += Temp6 << ((2 - i) * 8);

				}
				else
				{
					TempGOInfo.StopNodeID += Temp1 << (i * 8);
					TempGOInfo.StopStationID +=	Temp2 << (i * 8);
//						TempGOInfo.NextNodeID += Tmep3<< (i * 8);
					TempGOInfo.PathData_StartNodeID += Tmep4 << (i * 8);
					TempGOInfo.PathData_EndNodeID += Tmep5 << (i * 8);
//					TempGOInfo.StationDataStart_NextNodeID += Temp6 << (i * 8);

				}

			}

			for (int j = 0; j < TempGOInfo.PathData_ExtraNum; j++)
			{
				for (int i = 0; i < 3; i++)
				{

					// Extra Node 비트연산   puting
					int Temp = Cmd121->PathData_ExtraID[i + (j * 3)];

					if (HIGH_BYTE == 0)
						TempGOInfo.PathData_ExtraID[j] += Temp << ((2 - i) * 8);
					else
						TempGOInfo.PathData_ExtraID[j] += Temp << (i * 8);

				}
			}


		}

		ADD_LOG_TASKCONTROL("nStartID %d:%d %d %d %d",TempGOInfo.PathData_StartNodeID, TempGOInfo.PathData_EndNodeID ,m_pOHTMainStatus->FinalEndNodeOfPathSearch, m_pOHTMainStatus->FinalNodeOfPathSearch,GetExecuteStatus());


//		if(TempGOInfo.PathData_EndNodeID == TempGOInfo.PathData_StartNodeID)  nTempError = -1;


		if(nTempError ==NO_ERR)
		{

			if((TempGOInfo.PathData_StartNodeID == m_pOHTMainStatus->FinalNodeOfPathSearch) &&
			   ((GetExecuteStatus() == STS_ARRIVED) || (GetExecuteStatus() == STS_INIT)))
			{
				ADD_LOG_TASKCONTROL("CheckSearchChangePoint %d %d %d ",ChangeSqNum,ChangeNodeID,nTempExtraCount);
//				ChangeNodeID = m_pOHTMainStatus->FinalEndNodeOfPathSearch;
				m_pOHTMainStatus->JobChangeFlag.StartStationID =  m_pExecuteInfo->ExecutePositionInfo.CurrStation;
				ChangeNodeID = m_pExecuteInfo->ExecutePositionInfo.CurrNode;

				if(m_pOHTMainStatus->FinalEndNodeOfPathSearch != m_pExecuteInfo->ExecutePositionInfo.CurrNode)
					return breturn;

				if(m_pOHTMainStatus->JobChangeFlag.StartStationID ==0)
				{
					bCheck = m_pPathSearch->CheckSearchChangePoint(ChangeNodeID, ChangeSqNum, ChangeNodeDistance);
				}

				if(!bCheck)
					nTempError = -1;


					bCheckStop = true;
					ADD_LOG_TASKCONTROL("CheckSearchChangePoint_1 %d %d %d ",ChangeSqNum,ChangeNodeID,nTempExtraCount);

			}
			else
			{
				ADD_LOG_TASKCONTROL("CheckSearchDifferntPoint %d %d %d ",ChangeSqNum,ChangeNodeID,nTempExtraCount);
				nTempError = m_pPathSearch->CheckSearchDifferntPoint(OHTMainForm->GetMap(), &TempGOInfo,ChangeSqNum, ChangeNodeID, ChangeNodeDistance, nTempExtraCount,chNackToAckType,m_pOHTMainStatus->TargetStationNID);
				ADD_LOG_TASKCONTROL("CheckSearchDifferntPoint_1 %d %d %d ",ChangeSqNum,ChangeNodeID,nTempExtraCount);

				//인자로 더 들고오자

				/*
				if((ChangeNodeID ==m_pExecuteInfo->ExecutePositionInfo.CurrNode) ||
			        (ChangeNodeID ==m_pExecuteInfo->ExecutePositionInfo.CurrPreCheckNode))
                	return breturn;
				*/
			   //	dCheckDistance = 1500.0;    //기존 거리 마진
				//AOHC-107 OHT->OCS Nack 개선 마진 거리 최적화
				 dCheckDistance_margin = 1500.0 * (((double)m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningSpeed) / MAX_SPEED );
				// ADD_LOG_TASKCONTROL("dCheckDistance_margin : %f Speed : %f",dCheckDistance_margin,m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningSpeed);
				 if(dCheckDistance_margin > 400.0)
				 {
					  dCheckDistance =  dCheckDistance_margin;
				 }
				 else
				 {

					  dCheckDistance = 400.0;
				 }
				 ADD_LOG_TASKCONTROL("dCheckDistance %f",dCheckDistance );

				 //현재속도 확인
				 //작업받은 경로 조향 N인지 아닌지 판단해서 조향 쭉 N이면 감속도 3.0 / 조향 있으면 감속도 2.0
				 if(m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningSpeed > 3000.0 && m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningSpeed <= 4000.0)
				 {
					//조향 없으면 감속도 2.3
					if(m_pPathSearch->CheckSteering4Deccel(&TempGOInfo,ChangeNodeID))
					{
						m_dMinAccel = 2300.0;
						 ADD_LOG_TASKCONTROL("1. m_dMinAccel %f",m_dMinAccel );
					}
					//조향 있으면 감속도 2.0
					else
					{
						m_dMinAccel = 2000.0;
						ADD_LOG_TASKCONTROL("2. m_dMinAccel %f",m_dMinAccel );
					}
				 }
				 else if(m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningSpeed>4000.0)
				 {
					// 감속도 2.0
					m_dMinAccel = 2000.0;
					ADD_LOG_TASKCONTROL("3. m_dMinAccel %f",m_dMinAccel );

				 }
				 else // 현재속도 3.0m/s 보다 작은 경우
				 {
					//조향 없으면 감속도 3.0
					if(m_pPathSearch->CheckSteering4Deccel(&TempGOInfo,ChangeNodeID))
					{
						m_dMinAccel = 3000.0;
						ADD_LOG_TASKCONTROL("4. m_dMinAccel %f",m_dMinAccel );
					}
					//조향 있으면 감속도 2.0
					else
					{
						m_dMinAccel = 2000.0;
						ADD_LOG_TASKCONTROL("5. m_dMinAccel %f",m_dMinAccel );
					}
				 }

			}
		}

		ADD_LOG_TASKCONTROL("[작업변경] Step1 Error:%d Check %d",nTempError,bCheck);

		Sleep(1);   //컨텍스트 스위칭

		//해당 구간 삭제후 테스트시 확인해볼것.....puting 반드시
		/*
		 if(pTempCmdData->CycleType ==CYCLETYPE_DRIVING)
		 {
			if(pTempCmdData->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum > ChangeSqNum+1)
				nTempError = -1;
		 }
		 //*/
		if(nTempError ==NO_ERR)
		{
			//가능유무 Check

			//현재위치에서  남은 분기까지 남은 거리.
			dLastDistance = ((double)ChangeNodeDistance - ((double)m_pPassPermit->GetAbsolutePositon() + (double)m_pOHTMainStatus->dOHTTotalRunDis));

			// AOHC-107
			//Check되야하는 거리.
			dCheckDistance += m_pPathSearch->GetSpeedDownDist(m_dMinAccel,m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningSpeed) +
							 m_pPathSearch->GetMarginOfSpeedDist((double)m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningSpeed,CHANGEDISTMARIN);

			ADD_LOG_TASKCONTROL("[작업변경] Step2 LastD:%f/%f/%f/%f/%f/%f",ChangeNodeDistance,(double)m_pPassPermit->GetAbsolutePositon(), (double)m_pOHTMainStatus->dOHTTotalRunDis,dLastDistance,dCheckDistance,(double)m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningSpeed);

			m_pOHTMainStatus->JobChangeFlag.dChangDistance = ChangeNodeDistance;

			if((dLastDistance - dCheckDistance) >= 0.0)
			{
					//1) Driving쪽에 명령이 인가되었을시 명령 삭제.
					int nCycleListCount = m_pCmdManager->GetCycleCount();
 					int nCheckIndex = 0;
					PKT_CYCLECMDDATA * pCurPPCmd; // Command를 위한 포인터 변수
					PKT_CYCLECMDDATA * pTempCurPPCmd; // Command를 위한 포인터 변수

//					PKT_CYCLECMDDATA * pTempPPCmd = (PKT_CYCLECMDDATA*)m_pCmdManager->ReferCycle(m_pPassPermit->GetPPIndex());
                    PKT_ACK_OHT2OCS AckPkt;
					memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));


					for(int Index =  0; Index < nCycleListCount; Index++)
					{
						pCurPPCmd = (PKT_CYCLECMDDATA*)m_pCmdManager->PopBackCycle();


						if(pCurPPCmd->CycleType == CYCLETYPE_DRIVING)
						{
							//1) 변경할 싸이클 위치 탐색.
							if(pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum == ChangeSqNum)
							{
								ADD_LOG_TASKCONTROL("[작업변경] 명령 필요시 삭제 %d",Index);

								// 2-1)Driving쪽에 명령 인가시 삭제
								if((pCurPPCmd->EXE_CycleDriving_Info.D_PPStep >= CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE) &&
								   (bCheckStop == false) &&
									(pCurPPCmd->IsExcuteInfo == true))
								 {
								 	bool bCheckPoint =false;
									bCheckPoint = PathChangeReq2Driving(ChangeNodeID,pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum, true);
									if(bCheckPoint == false)
										m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_STOP); //실패할 경우 E-STOP으로 정지할 것. (실패할 확률 제로로 만들기.. )

									m_pOHTMainStatus->FinalNodeOfPathSearch = ChangeNodeID;
									m_pOHTMainStatus->FinalEndNodeOfPathSearch = ChangeNodeID;
									m_pOHTMainStatus->FinalStationOfPathSearch = 0;

									ADD_LOG_TASKCONTROL("[작업변경] 명령 필요시 주행명령 삭제");
								 }

								// 2-2)Cycle List 삭제 or 수정
								// 마지막 명령 수정 , 통과허가 상태에 따른 개방 확인.
								if(pCurPPCmd->EXE_CycleDriving_Info.StartPoint ==  ChangeNodeID)
								{
										//만약 통과허가 받은 경우, 받은거 개방(반납)할 것.
										nCheckIndex = (nCycleListCount - Index-1) -1;  //새로 생성함.
										if(nCheckIndex<0)
											nCheckIndex =0;

										if((pCurPPCmd->EXE_CycleDriving_Info.D_PPStep ==CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE) &&
										   ((pCurPPCmd->EXE_CycleDriving_Info.D_PPOStep ==NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE) || (pCurPPCmd->EXE_CycleDriving_Info.D_PPOStep ==NEXTCYCLESTEP_DRIVING_MEANINGLESS)))
										{
												//1. 통과개방 요청 등록(작업 변경)
												DATA_PASSOPEN * pDataPassOpen;
												pDataPassOpen = new DATA_PASSOPEN;
												pDataPassOpen->PassOpenIndex = 0;
												pDataPassOpen->bPassOpenType = JOBCHAGE_RETURN_TYPE;
												memmove(&pDataPassOpen->PassOpneInfo, &pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData,sizeof(PASSPERMIT_COMMON));
												m_pPassPermit->AddPassOpen(pDataPassOpen);
										}
										else if((pCurPPCmd->EXE_CycleDriving_Info.D_PPStep == CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE) &&
										(pCurPPCmd->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint == true)&&
										((double)pCurPPCmd->EXE_CycleDriving_Info.StopDistance > 0.0) &&
										(((double)pCurPPCmd->EXE_CycleDriving_Info.StopDistance - (double)m_pPassPermit->GetAbsolutePositon()) <= (double)m_pOHTMainStatus->dOHTTotalRunDis))
										{
											AckPkt.AckPkt.TAG.Value = TagID::CMD_PASSPERMITOPEN_OHT2OCS;
											memmove(&AckPkt.AckPkt.Value, &pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON));
											AckPkt.Length = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);
											MakeResponseStandardOHT2OCS(&AckPkt);
										   ADD_LOG_TASKCONTROL("RollBack_Return_3");
										}

										delete  pCurPPCmd;

										ADD_LOG_TASKCONTROL("[작업변경] Current Delete");
								}
								else
								{
									if(pCurPPCmd->EXE_CycleDriving_Info.FinalPoint !=  ChangeNodeID)
									{
										NODE_INFO	*pNODE_INFO	= NULL;	// NextNode 정보
										int nNodeIndex = OHTMainForm->GetMap()->FindNodeIndex(ChangeNodeID);	// Node Index 획득
										pNODE_INFO = &(OHTMainForm->GetMap()->m_NodeArray[nNodeIndex]);		// Node 정보

										pCurPPCmd->EXE_CycleDriving_Info.EndPoint = ChangeNodeID;
										pCurPPCmd->EXE_CycleDriving_Info.FinalPoint = ChangeNodeID;
										pCurPPCmd->EXE_CycleDriving_Info.EndNodeType =  pNODE_INFO->TagType;
										pCurPPCmd->EXE_CycleDriving_Info.EndDistance = ChangeNodeDistance;
									}

									//통과허가 반납할 필요 없음.
									m_pCmdManager->AddCycle(pCurPPCmd);
									nCheckIndex = (nCycleListCount - Index-1);
                                }

								// 2-3) Cycle list의 PP와 PPO index가 클경우 최초위치 이동..
								if(m_pPassPermit->GetPPIndex() > nCheckIndex)
									m_pPassPermit->SetPPIndex(nCheckIndex);

								if(m_pPassPermit->GetPPOIndex() > nCheckIndex)    //작은 경우는 없음.
									m_pPassPermit->SetPPOIndex(nCheckIndex);

								break;
							}
								if((pCurPPCmd->EXE_CycleDriving_Info.D_PPStep == CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE) &&
								(pCurPPCmd->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint == true)&&
								((double)pCurPPCmd->EXE_CycleDriving_Info.StopDistance > 0.0) &&
								(((double)pCurPPCmd->EXE_CycleDriving_Info.StopDistance - (double)m_pPassPermit->GetAbsolutePositon()) <= (double)m_pOHTMainStatus->dOHTTotalRunDis))
								{
									AckPkt.AckPkt.TAG.Value = TagID::CMD_PASSPERMITOPEN_OHT2OCS;
									memmove(&AckPkt.AckPkt.Value, &pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON));
									AckPkt.Length = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);
									MakeResponseStandardOHT2OCS(&AckPkt);
								   ADD_LOG_TASKCONTROL("RollBack_Return_2");
								}
						}

						// 2-4) 통과 개방 등록.
						if((pCurPPCmd->EXE_CycleDriving_Info.D_PPStep ==CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE) &&
						   ((pCurPPCmd->EXE_CycleDriving_Info.D_PPOStep ==NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE) || (pCurPPCmd->EXE_CycleDriving_Info.D_PPOStep ==NEXTCYCLESTEP_DRIVING_MEANINGLESS)))
						{

								ADD_LOG_TASKCONTROL("[작업변경] 통과 개방 등록");
								//1. 통과개방 요청 등록(작업 변경)
								DATA_PASSOPEN * pDataPassOpen;
								pDataPassOpen = new DATA_PASSOPEN;
								pDataPassOpen->PassOpenIndex = 0;
								pDataPassOpen->bPassOpenType = JOBCHAGE_RETURN_TYPE;
								memmove(&pDataPassOpen->PassOpneInfo, &pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData,sizeof(PASSPERMIT_COMMON));
								m_pPassPermit->AddPassOpen(pDataPassOpen);
						}


                        ADD_LOG_TASKCONTROL("[작업변경] 진행중 %d:%d", pCurPPCmd->EXE_CycleDriving_Info.StartPoint, pCurPPCmd->EXE_CycleDriving_Info.FinalPoint);

						delete  pCurPPCmd;

					}

				 //2) 주행명령 삭제 --마지막 위치가 아닌 경우만 수행 할 것.
				 ADD_LOG_TASKCONTROL("CancelPathInfo ChangeNodeID:ChangeSqNum %d:%d: %d", ChangeNodeID,ChangeSqNum, nTempExtraCount);
				 CancelPathInfo(ChangeNodeID, ChangeSqNum);


				 //3) 성공인 경우, 시작위치를 변경위치로 변경하고, 경로외 값도 변경할 것.
				 if(nTempExtraCount !=0)
				 {
					int  TempExtraNum = TempGOInfo.PathData_ExtraNum - nTempExtraCount;
					int TempIndex =0;
					for(int i = nTempExtraCount; i< TempGOInfo.PathData_ExtraNum; i++)
					{
						TempPathData_ExtraID[TempIndex] = TempGOInfo.PathData_ExtraID[i];
						TempIndex++;
                       //추가 수정 필요.
					}

					TempGOInfo.PathData_ExtraNum  = TempExtraNum;
					memmove(TempGOInfo.PathData_ExtraID, TempPathData_ExtraID, (MAX_PATH_SIZE*sizeof(unsigned int)));

					for (int j = 0; j < TempGOInfo.PathData_ExtraNum; j++)
					{
						for (int i = 0; i < 3; i++)
						{

							// Extra Node 비트연산   puting
							int Temp = TempGOInfo.PathData_ExtraID[j];

							if (HIGH_BYTE == 0)
								Cmd121->PathData_ExtraID[i + (j * 3)] = (Temp >> ((2 - i) * 8))&0xFF;
							else
								Cmd121->PathData_ExtraID[i + (j * 3)] = (Temp >> (i * 8))&0xFF;
						}
					}
				 }


				m_pExecuteInfo->nFinishOfNode = 0;
				m_pExecuteInfo->nFinishOfStation = 0;

				TempGOInfo.PathData_StartNodeID = ChangeNodeID;

				//현재가 최종 위치임.
				if((ChangeNodeID == TempGOInfo.PathData_EndNodeID) &&
				   (TempGOInfo.StopStationID ==0))
				{
                	Cmd121->PathData_Def_Direction.Driving_Flag = 0;
				}

				for (int i = 0; i < 3; i++)
				{

					// Extra Node 비트연산   puting
					int Temp1 = TempGOInfo.PathData_StartNodeID;

					if (HIGH_BYTE == 0)
						Cmd121->PathData_StartNodeID[i] = (Temp1 >> ((2 - i) * 8))&0xFF;
					else
						Cmd121->PathData_StartNodeID[i] = (Temp1 >> (i * 8))&0xFF;
				}

				//작업변경 완료
				m_pExecuteInfo->bChangeResult = true;

				memset(&m_pCurrCIDUSE, 0x00, sizeof(FLAG_CID_NOT_USE));
				m_pExecuteInfo->nNextNodeIDInfo = 0;

				breturn = true;
			}
			else   //Nack인경우에 가능한 경우 분류해서 처리해줄것  대기싸이클 처리하는구간 명령에 대해 처리 가능한 유무 확인 후 진행할것.
				  //aCK 처리가능한 케이스는 우선적으로 4가지중 2가지만 처리, 그리고 추후에 다른 케이스에 대해서도 처리 유무 확인 후진행할것.
				  //단순 거리에 의해 진행가능한 케이스는
				  /*
				  [ 작업변경 Nack에서 처리가 가능한 내용]
                  A->B
				 1) 거리가 짧게 변경될 경우 : X
				 2) A에서 B로 다른경로로 변경된 경우 : X
				 3) 거리가 멀리 변경된 경우 : O
				 4) 동일한 위치로 변경된 경우 : O
				  분류는 에러코드 응답으로 분류할것.
				 */
			{

				if(chNackToAckType ==JobChageNackType::CycleChage_Type2)
				{
					//가능
					//처리해줘야될 부분
					//기존 작업변경 위치와 마지막 위치가 동일할 경우 Driving flag는 0으로 처리해줄것. ---1)
					//현재가 최종 위치임.
					if((ChangeNodeID == TempGOInfo.PathData_EndNodeID) &&
					   (TempGOInfo.StopStationID ==m_pOHTMainStatus->TargetStationNID))
					{
						Cmd121->PathData_Def_Direction.Driving_Flag = 0;
					}


					TempGOInfo.PathData_StartNodeID = ChangeNodeID;

					for (int i = 0; i < 3; i++)
					{

						// Extra Node 비트연산   puting
						int Temp1 = TempGOInfo.PathData_StartNodeID;

						if (HIGH_BYTE == 0)
							Cmd121->PathData_StartNodeID[i] = (Temp1 >> ((2 - i) * 8))&0xFF;
						else
							Cmd121->PathData_StartNodeID[i] = (Temp1 >> (i * 8))&0xFF;
					}


					//기존 위치까지 이동할때 시작위치가 변경될 경우 위치는 기존 최종위치로 변경해줄것 ---2)
					 //3) 성공인 경우, 시작위치를 변경위치로 변경하고, 경로외 값도 변경할 것.
					 if(nTempExtraCount !=0)
					 {
						int  TempExtraNum = TempGOInfo.PathData_ExtraNum - nTempExtraCount;
						int TempIndex =0;
						for(int i = nTempExtraCount; i< TempGOInfo.PathData_ExtraNum; i++)
						{
							TempPathData_ExtraID[TempIndex] = TempGOInfo.PathData_ExtraID[i];
							TempIndex++;
						   //추가 수정 필요.
						}

						TempGOInfo.PathData_ExtraNum  = TempExtraNum;
						memmove(TempGOInfo.PathData_ExtraID, TempPathData_ExtraID, (MAX_PATH_SIZE*sizeof(unsigned int)));

						for (int j = 0; j < TempGOInfo.PathData_ExtraNum; j++)
						{
							for (int i = 0; i < 3; i++)
							{

								// Extra Node 비트연산   puting
								int Temp = TempGOInfo.PathData_ExtraID[j];

								if (HIGH_BYTE == 0)
									Cmd121->PathData_ExtraID[i + (j * 3)] = (Temp >> ((2 - i) * 8))&0xFF;
								else
									Cmd121->PathData_ExtraID[i + (j * 3)] = (Temp >> (i * 8))&0xFF;
							}
						}
					 }


					nTempError = -1;
					ADD_LOG_TASKCONTROL("[작업변경 Nack ->Ack 처리]");
				}
				else    //기존대로간다.
				{
					nTempError = -1;
					ADD_LOG_TASKCONTROL("[작업변경 불가]");
				}

            }

		}

        return breturn;
}

 ///< 작업지시 명령에 대한 응답 체크
bool TaskControl::CheckCmdJobOrder(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, TAGTYPE* TagType,  int QuickCmd, STANDARDCATEGORY Catagory)
{

	BYTE chNackToAckType = JobChageNackType::CycleNotUse;

	PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121  	 CmdTempID121;      //Cmd
	PROTOCOL_BODY_JOB_RESPONSE_CLW_TO_MCP_ID_122 AckTempID122;    //ACk


	memset(&AckTempID122, 0x00,sizeof(PROTOCOL_BODY_JOB_RESPONSE_CLW_TO_MCP_ID_122));

	memmove(&CmdTempID121, pRcvData->Value, sizeof(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121));

	AckTempID122.CmdID = CmdTempID121.CmdID;
	AckTempID122.CmdCycle = CmdTempID121.CmdCycle;

	AckTempID122.Riv.Riv_Value = CmdTempID121.Riv.Riv_Value;
	AckTempID122.Riv.Hotlot = CmdTempID121.Riv.Hotlot;

	AckTempID122.ResponseDiv.CMDType = CmdTempID121.CmdDiv.CMDType;
	AckTempID122.ResponseDiv.TestMode = CmdTempID121.CmdDiv.TestMode;

	memmove(&AckTempID122.StopNodeID, CmdTempID121.StopNodeID, 3);
	memmove(&AckTempID122.StopStationID, CmdTempID121.StopStationID, 3);
	

	ADD_LOG_TASKCONTROL("CmdID %d, Cycle %d, CmdType %d StopNode %d %d %d",
	CmdTempID121.CmdID,
	CmdTempID121.CmdCycle,
	CmdTempID121.CmdDiv.CMDType,
	CmdTempID121.PathData_Def_Direction.Path_Direction,
	CmdTempID121.PathData_StartNodeID[2],
	CmdTempID121.PathData_EndNodeID[2]);


	int TempError = NO_ERR;
	STATION_DATA StationData; // Station Data를 가져올 구조체 포인터
	memset(&StationData, 0x00, sizeof(StationData));
	PATH_DIVERGENCE_INFO nTmpDivInfo = PATH_DIVERGENCE_NONE;
	int TempCheckNodeID = 0;
	bool bExtraPath = false;

	int TempStartNID = 0;
	int TempEndNID = 0;
	int TempTargetStationID = 0;
	int TempTargetNID = 0;
	int nCheckDeviceOfPathInfo = 0;

	int CurrentOHTDiv = 0;
	bool bISNBrench = false;

	if(m_pExecuteInfo->nCheckDivInfo == PATH_DIVERGENCE_LEFT)
		CurrentOHTDiv =0x01;
	else if(m_pExecuteInfo->nCheckDivInfo == PATH_DIVERGENCE_RIGHT)
		CurrentOHTDiv =0x02;
	else
		CurrentOHTDiv =0x03; //직진

	bISNBrench = m_pPathSearch->GetIsNBrench(OHTMainForm->GetMap(),m_pExecuteInfo->ExecutePositionInfo.CurrNode,m_pPathSearch->GetNextNode(OHTMainForm->GetMap(), m_pExecuteInfo->ExecutePositionInfo.CurrNode, CurrentOHTDiv));

	if(m_pExecuteInfo->ExecutePositionInfo.CurrStation!=0)
	{
		try
		{
			GetStationData(&StationData, m_pExecuteInfo->ExecutePositionInfo.CurrStation);
			nTmpDivInfo = m_pPathSearch->GetStationDivInfo(OHTMainForm->GetMap(),StationData.ParentNodeID,StationData.NextNodeID);
		}catch(...)
		{
            ADD_LOG_TASKCONTROL("JobOrder:Station Fail");
        }
	}

	///< puting 상태명령있는지 확인
	if((QuickCmd == TAG_CMDDATA_CTRL_TURN_OFF) ||
		((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF)) ||
		((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning ==OFF)))
	{
		AckTempID122.ResponseAck.VHL_Error =OFF;     //기동 OFF일 경우
		AckTempID122.ResponseAck.Accept_Error =ON;
		AckTempID122.ResponseAck.PresionCode =0x01;

	}
	else if(GetExecuteStatus() == STS_TURN_OFF)
	{
		if(GetCurrentMode() == MODE_AUTO)
			m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_TURN_ON);

		AckTempID122.ResponseAck.VHL_Error =OFF;     //기동 OFF일 경우
		AckTempID122.ResponseAck.Accept_Error =ON;
		AckTempID122.ResponseAck.PresionCode =0x01;
	}
	else if((QuickCmd == TAG_CMDDATA_CTRL_ESTOP) ||   //정지 명령일 경우
			(GetExecuteStatus() == STS_ERROR))
	{
		AckTempID122.ResponseAck.VHL_Error =OFF;
		AckTempID122.ResponseAck.Accept_Error =ON;
		AckTempID122.ResponseAck.PresionCode =0x03;
	}
	else if((Catagory == CATEGORY_SECONDARY) &&  //메뉴얼 모드일 경우
			 (GetCurrentMode() ==MODE_MANUAL))
	{
		AckTempID122.ResponseAck.VHL_Error =OFF;
		AckTempID122.ResponseAck.Accept_Error =ON;
		AckTempID122.ResponseAck.PresionCode =0x02;
	}
	else if(m_pExecuteInfo->FlagMTLInfo.bTakeInCycle ==true)  //투입중일때 작업지시 수행불가.
	{
		AckTempID122.ResponseAck.VHL_Error =OFF;
		AckTempID122.ResponseAck.Accept_Error =ON;
		AckTempID122.ResponseAck.PresionCode =0x06;
	}
	else if(m_pOHTMainStatus->DataUseFlag.fSwapping ==true)  //맵 갱신중
	{
		AckTempID122.ResponseAck.VHL_Error =OFF;
		AckTempID122.ResponseAck.Accept_Error =ON;
		AckTempID122.ResponseAck.PresionCode =0x0A;
	}
//	else if((CmdTempID121.PathData_Def_Direction.Driving_Flag == ON)  //분기정보가 맞지않게 명령이 옴
//		&& (m_pExecuteInfo->nCheckDivInfo !=PATH_DIVERGENCE_NONE)
//		&& (((m_pExecuteInfo->nCheckDivInfo ==PATH_DIVERGENCE_LEFT) && (CmdTempID121.PathData_Def_Direction.Path_Direction == DefaultDirection::DIR_RIGHT)) || ((m_pExecuteInfo->nCheckDivInfo ==PATH_DIVERGENCE_RIGHT) && (CmdTempID121.PathData_Def_Direction.Path_Direction == DefaultDirection::DIR_LEFT)))
//		)
//	{
//		nCountCheckPathSearch++;
//		AckTempID122.ResponseAck.VHL_Error =OFF;
//		AckTempID122.ResponseAck.Accept_Error =ON;
//		AckTempID122.ResponseAck.PresionCode =0x2E;
//	}
	else
	{
		AckTempID122.ResponseAck.VHL_Error =OFF;
		AckTempID122.ResponseAck.Accept_Error =OFF;
		AckTempID122.ResponseAck.PresionCode =0x00;
	}

	//----PathSearch NACK처리 부분
	for (int i = 0; i < 3; i++)
	{
		// Node 비트연산   puting
		int Tmep4 = CmdTempID121.PathData_StartNodeID[i];
		int Tmep5 = CmdTempID121.PathData_EndNodeID[i];
		int Temp6 =  CmdTempID121.StopNodeID[i];
		int Temp7 =  CmdTempID121.StopStationID[i];


		if (HIGH_BYTE == 0) {
			TempStartNID += Tmep4 << ((2 - i) * 8);
			TempEndNID += Tmep5 << ((2 - i) * 8);
			TempTargetNID   += Temp6 << ((2 - i) * 8);
			TempTargetStationID   += Temp7 << ((2 - i) * 8);
		}
		else
		{
			TempStartNID += Tmep4 << (i * 8);
			TempEndNID += Tmep5 << (i * 8);
			TempTargetNID += Temp6 << (i * 8);
			TempTargetStationID += Temp7 << (i * 8);
		}

	}
	//Start End NID Check
	if(m_pPathSearch->CheckNodeAndLinkIndex(OHTMainForm->GetMap(),TempStartNID,TempEndNID)
	&& (CmdTempID121.CmdCycle != CYCLE::TP_PortTeaching)
	&& (CmdTempID121.CmdCycle != CYCLE::TP_PortTeaching_NullType))
	{

		AckTempID122.ResponseAck.VHL_Error =ON;
		AckTempID122.ResponseAck.Accept_Error =ON;
		AckTempID122.ResponseAck.PresionCode =0x05;
		m_nCountCheckPathSearch++;

		ADD_LOG_TASKCONTROL("JobOrder[CheckNodeAndLinkIndex]%d,%d",TempStartNID,TempEndNID);

	}
	else if((CmdTempID121.StationData_StationID !=0)
	&& (CmdTempID121.CmdCycle != CYCLE::TP_PortTeaching)
	&& (CmdTempID121.CmdCycle != CYCLE::TP_PortTeaching_NullType))
	{
		//Link 와 Station Offsest 값 확인
		if(m_pPathSearch->CheckEndStationOffset(OHTMainForm->GetMap(),CmdTempID121.StationData_ParentNodeID,CmdTempID121.StationData_NextNodeID,CmdTempID121.StationData_Offset))
		{
			AckTempID122.ResponseAck.VHL_Error =ON;
			AckTempID122.ResponseAck.Accept_Error =ON;
			AckTempID122.ResponseAck.PresionCode =0x04;
			m_nCountCheckPathSearch++;

			ADD_LOG_TASKCONTROL("JobOrder[CheckEndStationOffset]%d,%d %d %d",CmdTempID121.StationData_StationID,CmdTempID121.StationData_ParentNodeID,CmdTempID121.StationData_NextNodeID,CmdTempID121.StationData_Offset);
		}
	}
	//----PathSearch NACK처리 부분

	ADD_LOG_TASKCONTROL("Accept_Error %d,VHL_Error %d %d",AckTempID122.ResponseAck.Accept_Error, AckTempID122.ResponseAck.VHL_Error,AckTempID122.ResponseAck.PresionCode);

	///< 세부 에러 내용 확인    -----puting 20151118
	if(AckTempID122.ResponseAck.Accept_Error ==OFF)
	{

		m_nCountCheckPathSearch = 0;

		switch(CmdTempID121.CmdDiv.CMDType)
		{
		// 1. 작업지시 예외처리.
		case ORDERDIV::JobOrder:
			if((m_pOHTMainStatus->CurCycle != CYCLE::CycleNotExist) || (m_pOHTMainStatus->IsOnGoCmd == true))
			{
				AckTempID122.ResponseAck.VHL_Error =ON;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x15;
			}
			else if(m_pCmdManager->GetTaskCount() !=LISTCOUNT_EMPTY)  //작업지시 1개만 수행
			{
				AckTempID122.ResponseAck.VHL_Error =ON;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x16;

				if(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist)
				{
					m_pCmdManager->InitTaskList();
					ADD_LOG_TASKCONTROL("[작업지시 가능상태로 전환]");
				}
			}
			else if(AckTempID122.CmdID == m_pOHTMainStatus->CurCmdID)
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x12;
			}
			else if(((m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1 ==true) && (CmdTempID121.CmdCycle == CYCLE::CraneUp)) ||
					((m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1 ==false) && (CmdTempID121.CmdCycle == CYCLE::CraneDown)))
			{
						AckTempID122.ResponseAck.VHL_Error =OFF;
						AckTempID122.ResponseAck.Accept_Error =ON;
						AckTempID122.ResponseAck.PresionCode =0x13;
			}
			else if(m_pExecuteInfo->FlagMTLInfo.bTakeoutCycle == true)
			{
                    ADD_LOG_TASKCONTROL("[작업지시 불가 투입중]");
             		AckTempID122.ResponseAck.VHL_Error =OFF;
					AckTempID122.ResponseAck.Accept_Error =ON;
					AckTempID122.ResponseAck.PresionCode =0x06;
			}
			else if((nTmpDivInfo!= PATH_DIVERGENCE_NONE) &&
					(CmdTempID121.PathData_Def_Direction.Driving_Flag == true))
			{
				if(CmdTempID121.PathData_ExtraNum!=0)
				{
                  	for (int i = 0; i < 3; i++) {

						// Extra Node 비트연산   puting
						int Temp = CmdTempID121.PathData_ExtraID[i];

						if (HIGH_BYTE == 0)
							TempCheckNodeID += Temp << ((2 - i) * 8);
						else
							TempCheckNodeID += Temp << (i * 8);

					}
                }

				//기본외 분기위치가 아닐 경우
				if(TempCheckNodeID != m_pExecuteInfo->ExecutePositionInfo.CurrNode)
				{
					if((nTmpDivInfo== PATH_DIVERGENCE_LEFT) && (CmdTempID121.PathData_Def_Direction.Path_Direction != DefaultDirection::DIR_LEFT))
					{
						ADD_LOG_TASKCONTROL("[작업지시불가:분기정보다름],%d,%d,%d",TempCheckNodeID,nTmpDivInfo,CmdTempID121.PathData_Def_Direction.Path_Direction);
						AckTempID122.ResponseAck.VHL_Error =OFF;
						AckTempID122.ResponseAck.Accept_Error =ON;
						AckTempID122.ResponseAck.PresionCode =0x11;
						m_nCountCheckPathSearch++;
					}
					else if((nTmpDivInfo== PATH_DIVERGENCE_RIGHT) && (CmdTempID121.PathData_Def_Direction.Path_Direction != DefaultDirection::DIR_RIGHT))
					{
						ADD_LOG_TASKCONTROL("[작업지시불가:분기정보다름2],%d,%d,%d",TempCheckNodeID,nTmpDivInfo,CmdTempID121.PathData_Def_Direction.Path_Direction);
						AckTempID122.ResponseAck.VHL_Error =OFF;
						AckTempID122.ResponseAck.Accept_Error =ON;
						AckTempID122.ResponseAck.PresionCode =0x11;
						m_nCountCheckPathSearch++;
					}
				}
				//기본외 분기위치인 경우
				else
				{
					if((nTmpDivInfo== PATH_DIVERGENCE_LEFT) && (CmdTempID121.PathData_Def_Direction.Path_Direction == DefaultDirection::DIR_LEFT))
					{
						ADD_LOG_TASKCONTROL("[작업지시불가:분기정보다름3],%d,%d,%d",TempCheckNodeID,nTmpDivInfo,CmdTempID121.PathData_Def_Direction.Path_Direction);
						AckTempID122.ResponseAck.VHL_Error =OFF;
						AckTempID122.ResponseAck.Accept_Error =ON;
						AckTempID122.ResponseAck.PresionCode =0x11;
						m_nCountCheckPathSearch++;
					}
					else if((nTmpDivInfo== PATH_DIVERGENCE_RIGHT) && (CmdTempID121.PathData_Def_Direction.Path_Direction == DefaultDirection::DIR_RIGHT))
					{
						ADD_LOG_TASKCONTROL("[작업지시불가:분기정보다름4],%d,%d,%d",TempCheckNodeID,nTmpDivInfo,CmdTempID121.PathData_Def_Direction.Path_Direction);
						AckTempID122.ResponseAck.VHL_Error =OFF;
						AckTempID122.ResponseAck.Accept_Error =ON;
						AckTempID122.ResponseAck.PresionCode =0x11;
						m_nCountCheckPathSearch++;
                    }
                }
			}

           if(AckTempID122.ResponseAck.Accept_Error ==OFF)
		   {
				m_pOHTMainStatus->CurCmdID = AckTempID122.CmdID;
				dwTurnOffJobOrderTime = timeGetTime();
		   }
		//에러 확인 및 처리

			ADD_LOG_TASKCONTROL("[작업지시]Accept_Error %d,VHL_Error %d %d",AckTempID122.ResponseAck.Accept_Error, AckTempID122.ResponseAck.VHL_Error,AckTempID122.ResponseAck.PresionCode);
            *TagType = TAGTYPE_JOBORDER;
			break;

		//2. 작업변경 예외처리.
		case ORDERDIV::JobChange:
			//2-1) 작업변경 가능 상태 확인
			 nCheckDeviceOfPathInfo = m_pPathSearch->CheckDeviceOfPathInfo(OHTMainForm->GetMap(),m_pExecuteInfo->ExecutePositionInfo.CurrNode);

			if((GetExecuteStatus() ==STS_UNLOADING) ||
			   (GetExecuteStatus() ==STS_LOADING) ||
			   (GetExecuteStatus() ==STS_AUTORECOVERY_OVERRUN) ||
			   (GetExecuteStatus() ==STS_AUTORECOVERY) ||
			   (GetExecuteStatus() ==STS_AUTORECOVERY_BLANKTRANS) ||
			   (GetExecuteStatus() ==STS_AUTORECOVERY_DOUBLETRANS) ||
			   (GetExecuteStatus() ==STS_AUTO_TEACHING) ||
			   (GetExecuteStatus() ==STS_AUTO_TEACHING_NULLTYPE) ||
			   (GetExecuteStatus() ==STS_AUTO_TRANS_HOMING) ||
			   (m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute == true))
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x07;
			}
			else if(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist) //현재 사이클 중이 아님.
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x05;
			}
			else if((m_pOHTMainStatus->CurCycle == CYCLE::TP_RunningLearning) || (m_pOHTMainStatus->CurCycle == CYCLE::TP_TestRunningLearning)) //주행 학습중일때 안됨.
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x04;
			}
			else if(AckTempID122.CmdID != m_pOHTMainStatus->CurCmdID)   //작업변경 요청한 CMD ID와다름.
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x08;
			}
			else if(nCheckDeviceOfPathInfo !=0)   //리프터 경사로 MTL 등 장비와 연결된 구간은 무시
			{
				if(nCheckDeviceOfPathInfo ==1)   //경사로
				{
                    AckTempID122.ResponseAck.VHL_Error =OFF;
					AckTempID122.ResponseAck.Accept_Error =ON;
					AckTempID122.ResponseAck.PresionCode =0x09;
				}
				else if (nCheckDeviceOfPathInfo ==2)   //LIfter
				{
					AckTempID122.ResponseAck.VHL_Error =OFF;
					AckTempID122.ResponseAck.Accept_Error =ON;
					AckTempID122.ResponseAck.PresionCode =0x26;
				}
				else  //MTL
				{
					AckTempID122.ResponseAck.VHL_Error =OFF;
					AckTempID122.ResponseAck.Accept_Error =ON;
					AckTempID122.ResponseAck.PresionCode =0x27;
                }

			}
			else if(m_pOHTMainStatus->StatusTP.IsCIDPause == true)
			{
               	AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x10;
			}
            else if(m_pExecuteInfo->FlagCmdExcute.bCmdDrivingCMDExcute == true) //Falg 처리
			//else if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwJobChangeCheckTime) < 300)
			{
               	AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x0e;
			}
			else if(m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag == true)
			{
               	AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x0b;
			}
			/*
			else if(((m_pHWCommon->m_Output.CID_Left_Select_ON == ON) || (m_pHWCommon->m_Output.CID_Right_Select_ON == ON)) && (m_pHWCommon->m_Input.CID_Link_Compt_IN == OFF))
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x0c;
				//ADD_LOG_TASKCONTROL("[작업변경_Test]");
			}
			*/
			else
			{

            	//2-2) 작업변경 가능 유무 확인 .

				//통과허가 요청 중단.


				// 시작위치(현재 위치가) Target 위치일 경우 인지 먼저확인
				// 아닐 경우
				//명령을 받은 분기와 비교해서 변경가능한지 확인.
				// 현재 경로에서의 분기 정보 찾기.
				//만약에 가능한 경우 해당 분기까지 정보들을 삭제함.
				//Cycle List 삭제(수정) 및 pathList 정보들 삭제
				//시작 위치는 성공한 분기위치부터 시작위치
				//그리고 나머지는 Task 관련해서 처리할때 새로 만들어줌.



					// 현재 경로에서의 분기 정보 찾기.
					if(!(CheckJobChange(&CmdTempID121,chNackToAckType)))
					{
						//1. 시작위치가 경로상에 없는 경우.
						AckTempID122.ResponseAck.VHL_Error =OFF;
						AckTempID122.ResponseAck.Accept_Error =ON;
						AckTempID122.ResponseAck.PresionCode =0x0D;

					}else
					{
						memmove(pRcvData->Value, &CmdTempID121, sizeof(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121));
					   //	m_dwJobChangeCheckTime = (DWORD)timeGetTime();
					   //	m_bJobChangeCheckFlag = true; //일반적인 작업변경에 의한 경우.
						dwTurnOffJobOrderTime = timeGetTime();
					}


			}
			ADD_LOG_TASKCONTROL("[작업변경] %d %d", chNackToAckType,AckTempID122.ResponseAck.PresionCode);
			*TagType = TAGTYPE_JOBORDER_JOBCHAGE;

			if(AckTempID122.ResponseAck.Accept_Error ==ON)
			{
				//에러코드 13번일때에 대해서 처리해줄것.
				if(AckTempID122.ResponseAck.PresionCode ==0x0D)
				{
					m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag = false;
					m_pOHTMainStatus->JobChangeFlag.StartStationID = 0;
					m_pOHTMainStatus->JobChangeFlag.dChangDistance = 0.0;

				   if(chNackToAckType == JobChageNackType::CycleChage_Type2)
				   {
						if(m_pCmdManager->GetCycleCount() != LISTCOUNT_EMPTY)
						{
							//대기싸이클이 있으면 삭제 처리해줄것.
							  //마지막 작업 취소
                       		if((m_pOHTMainStatus->CurCycle ==  CYCLE::CraneUp)||   //올림 싸이클
							 (m_pOHTMainStatus->CurCycle ==  CYCLE::CraneDown))  //내림 싸이클
							{
								if(m_pOHTMainStatus->WaitCycle != CYCLE::CycleNotExist)
								{
								 //기존꺼 삭제처리
									 ADD_LOG_TASKCONTROL("[작업변경]_기존 ChageNack 삭제");
									 m_pCmdManager->MakeTaskList_Standard_Del_JobOrder();
								}

								JobOrderDataCancel();

								//내림 올림일때는 동일한 위치인 경우는 이적재 이동싸이클밖에 없음.
								if((CmdTempID121.CmdCycle ==  CYCLE::MovingToStation) &&
								(m_pOHTMainStatus->TargetStationNID  == TempTargetStationID))
								{
									m_pOHTMainStatus->WaitCycle = CYCLE::CycleNotExist;
									m_pOHTMainStatus->WaitTargetStationNID = 0;

								   *TagType = TAGTYPE_JOBORDER_TARGETINIT;
								}
								else
								{
									 if((CmdTempID121.CmdCycle !=  CYCLE::MovingToStation) &&
									 (m_pOHTMainStatus->TargetStationNID  == TempTargetStationID))
									 {
										CmdTempID121.PathData_Def_Direction.Driving_Flag = 0;
									 }

									m_pOHTMainStatus->WaitCycle = CmdTempID121.CmdCycle;
									if(TempTargetStationID !=0)
										m_pOHTMainStatus->WaitTargetStationNID = TempTargetStationID;
									else
										m_pOHTMainStatus->WaitTargetStationNID = TempTargetNID;

									*TagType = TAGTYPE_JOBORDER;
                                }

//								m_pOHTMainStatus->CurCycle = CYCLE::MovingToStation;
								ADD_LOG_TASKCONTROL("[작업변경]_JobChageNackType");
								memmove(pRcvData->Value, &CmdTempID121, sizeof(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121));

								AckTempID122.ResponseAck.VHL_Error =OFF;
								AckTempID122.ResponseAck.Accept_Error =OFF;
								AckTempID122.ResponseAck.PresionCode =0x00;

							}
							else if((m_pOHTMainStatus->CurCycle ==  CYCLE::MovingToNode)||   //이동  싸이클
									(m_pOHTMainStatus->CurCycle ==  CYCLE::MovingToStation))  //이동  싸이클
							{
								if(m_pOHTMainStatus->WaitCycle != CYCLE::CycleNotExist)
								{
								 //기존꺼 삭제처리
									 ADD_LOG_TASKCONTROL("[작업변경]_기존 ChageNack 삭제");
									 m_pCmdManager->MakeTaskList_Standard_Del_JobOrder();
								}

								 if((CmdTempID121.CmdCycle ==  CYCLE::MovingToStation) //|| (m_pOHTMainStatus->CurCycle ==  CYCLE::MovingToNode))
								 && (m_pOHTMainStatus->TargetStationNID  == TempTargetStationID))
								{
									m_pOHTMainStatus->WaitCycle = CYCLE::CycleNotExist;
									m_pOHTMainStatus->WaitTargetStationNID = 0;

								   *TagType = TAGTYPE_JOBORDER_TARGETINIT;
								}
								else
								{
									//이동은
									if(m_pOHTMainStatus->TargetStationNID  == TempTargetStationID)
									{
										if((CmdTempID121.CmdCycle ==  CYCLE::CraneUp)||   //올림 싸이클
											(CmdTempID121.CmdCycle ==  CYCLE::CraneDown))  //내림 싸이클
										{
											CmdTempID121.PathData_Def_Direction.Driving_Flag = 0;
										}
									}


									m_pOHTMainStatus->WaitCycle = CmdTempID121.CmdCycle;
									if(TempTargetStationID !=0)
										m_pOHTMainStatus->WaitTargetStationNID = TempTargetStationID;
									else
										m_pOHTMainStatus->WaitTargetStationNID = TempTargetNID;

									*TagType = TAGTYPE_JOBORDER;
								}
								ADD_LOG_TASKCONTROL("[작업변경]_JobChageNackType_2");

								memmove(pRcvData->Value, &CmdTempID121, sizeof(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121));

								AckTempID122.ResponseAck.VHL_Error =OFF;
								AckTempID122.ResponseAck.Accept_Error =OFF;
								AckTempID122.ResponseAck.PresionCode =0x00;
							}
						}
				   }else //Nack지만 다른 작업변경에 의해 발생 Case 분류
				   {
						if(chNackToAckType == JobChageNackType::CycleChage_Type1)
							AckTempID122.ResponseAck.PresionCode =0x1F;
						else if(chNackToAckType == JobChageNackType::CycleChage_Type1_1)
							AckTempID122.ResponseAck.PresionCode =0x20;
				   }
				}


			}else
			{
				//작업변경 Ack된경우, 대기싸이클을 경우 삭제 처리.
				if(m_pOHTMainStatus->WaitCycle != CYCLE::CycleNotExist)
				{
				 	//기존꺼 삭제처리
					 ADD_LOG_TASKCONTROL("[작업변경]_기존 ChageNack 삭제");
					 m_pCmdManager->MakeTaskList_Standard_Del_JobOrder();

					 m_pOHTMainStatus->WaitCycle = CYCLE::CycleNotExist;
					 m_pOHTMainStatus->WaitTargetStationNID = 0;

				}
			}
			break;
		case ORDERDIV::TargetChange:
			AckTempID122.ResponseAck.VHL_Error =OFF;
			AckTempID122.ResponseAck.Accept_Error =ON;
			AckTempID122.ResponseAck.PresionCode =0x24;

			 ADD_LOG_TASKCONTROL("[타겟변경]");
			*TagType = TAGTYPE_JOBORDER_TARGETCHAGE;
			break;
		case ORDERDIV::PathChange:
			AckTempID122.ResponseAck.VHL_Error =OFF;
			AckTempID122.ResponseAck.Accept_Error =ON;
			AckTempID122.ResponseAck.PresionCode =0x24;

			 ADD_LOG_TASKCONTROL("[경로변경]");
			*TagType = TAGTYPE_JOBORDER_PATHCHAGE;
			break;
		case ORDERDIV::JobClear:
		case ORDERDIV::JobCancel:
			AckTempID122.ResponseAck.VHL_Error =OFF;
			AckTempID122.ResponseAck.Accept_Error =ON;
			AckTempID122.ResponseAck.PresionCode =0x24;

			 ADD_LOG_TASKCONTROL("[작업삭제]");
			*TagType = TAGTYPE_JOBORDER_JOBCLEAR;
			 break;
		case ORDERDIV::JobCancel_2:
			if((GetExecuteStatus() ==STS_UNLOADING) ||
			   (GetExecuteStatus() ==STS_LOADING) ||
			   (GetExecuteStatus() ==STS_AUTO_TEACHING) ||
			   (GetExecuteStatus() ==STS_AUTO_TEACHING_NULLTYPE) ||
			   (GetExecuteStatus() ==STS_AUTO_TRANS_HOMING) ||
			   (m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute == true))
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x07;

			}
			else if(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist) //현재 사이클 중이 아님.
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x05;
			}
			else if((m_pOHTMainStatus->CurCycle == CYCLE::TP_RunningLearning) ||(m_pOHTMainStatus->CurCycle == CYCLE::TP_TestRunningLearning))//주행 학습중일때 안됨.
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x04;
			}
			else if(AckTempID122.CmdID != m_pOHTMainStatus->CurCmdID)   //작업변경 요청한 CMD ID와다름.
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x08;
			}
			else if((m_pOHTMainStatus->CurCycle !=  CYCLE::MovingToNode) &&
					(m_pOHTMainStatus->CurCycle !=  CYCLE::MovingToStation) &&
					(m_pOHTMainStatus->CurCycle !=  CYCLE::CraneUp) &&
					(m_pOHTMainStatus->CurCycle !=  CYCLE::CraneDown))
			{
				AckTempID122.ResponseAck.VHL_Error =OFF;
				AckTempID122.ResponseAck.Accept_Error =ON;
				AckTempID122.ResponseAck.PresionCode =0x25;  //작업취소 에러코드 추가.
			}
			else
			{
             	// Step 2)Cycle List 내용을 확인하여 Excute Thread로 전달해주는 부분
				if(m_pCmdManager->GetCycleCount() != LISTCOUNT_EMPTY)
				{
					ADD_LOG_TASKCONTROL("[작업취소] puting");
					//작업취소 Ack된경우, 대기싸이클을 경우 삭제 처리.
					if(m_pOHTMainStatus->WaitCycle != CYCLE::CycleNotExist)
					{
						//기존꺼 삭제처리
						 ADD_LOG_TASKCONTROL("[작업변경]_기존 ChageNack 삭제");
						 m_pCmdManager->MakeTaskList_Standard_Del_JobOrder();

						 m_pOHTMainStatus->WaitCycle = CYCLE::CycleNotExist;
						 m_pOHTMainStatus->WaitTargetStationNID = 0;

					}

					if((m_pOHTMainStatus->CurCycle ==  CYCLE::CraneUp)||   //올림 싸이클
					   (m_pOHTMainStatus->CurCycle ==  CYCLE::CraneDown))  //내림 싸이클
					{

						JobOrderDataCancel();
						m_pOHTMainStatus->CurCycle = CYCLE::MovingToStation;
					}
				}
            }

			 ADD_LOG_TASKCONTROL("[작업취소] %d %d %d",AckTempID122.ResponseAck.VHL_Error,AckTempID122.ResponseAck.Accept_Error,AckTempID122.ResponseAck.PresionCode);




			*TagType = TAGTYPE_JOBORDER_JOBCANCEL;
			break;

		case ORDERDIV::NotOrder:
		default:
		 //에러 처리
			AckTempID122.ResponseAck.VHL_Error =ON;
			AckTempID122.ResponseAck.Accept_Error =ON;
			AckTempID122.ResponseAck.PresionCode =2;

			break;

		}
	}

	memmove(pAckPkt->AckPkt.Value, &AckTempID122, sizeof(PROTOCOL_BODY_JOB_RESPONSE_CLW_TO_MCP_ID_122));
	if(Catagory == CATEGORY_SECONDARY)
		pAckPkt->AckPkt.TAG.Value = TagID::JOB_ORDER_RESPONSE;
	else
		pAckPkt->AckPkt.TAG.Value = TagID::JOB_ORDER_RESPONSE_TP;

	pAckPkt->Length = sizeof(PKT_JOB_ORDER_RESPONSE_ID_122);

	//AOHC-196 사전 로그 추가
   if(Catagory == CATEGORY_SECONDARY)
   {
		if(AckTempID122.ResponseAck.Accept_Error == ON)
		{
			if(CmdTempID121.CmdDiv.CMDType == ORDERDIV::JobOrder)  //작업 지시 Nack
			{
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_JOBO_NAK);
				m_mccparam.nJobO_NAK_Count++;
			}
			else if(CmdTempID121.CmdDiv.CMDType == ORDERDIV::JobChange)
			{
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_JOBC_NAK);    //작업 변경  Nack
				m_mccparam.nJobC_NAK_Count++;
			}
			else
			{
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_JOBD_NAK);   //작업 취소 및 기타 Nack
				m_mccparam.nJobD_NAK_Count++;
			}
		}
	}



  return (bool)!AckTempID122.ResponseAck.Accept_Error;

}
 ///< 통과허가에 대한 명령에 대한 응답 체크
bool TaskControl::CheckCmdPPResponse(PKT_OCS2OHT* pRcvData, int QuickCmd)
{
	bool bCheckReturn = true;

	int TmempStopNode =0;
	int nStopNode =0;
	int nResetNode =0;

	PROTOCOL_BODY_RESPONSE_PASSPERMIT_OCS2OHT_ID_242 CmdTempID242;
	memmove(&CmdTempID242, pRcvData->Value, sizeof(PROTOCOL_BODY_RESPONSE_PASSPERMIT_OCS2OHT_ID_242));

	for (int i = 0; i < 3; i++)
	{
		// Node 비트연산   puting
		int TmempStopNode = CmdTempID242.PassPermit_CM.StopNodeID[i];
		int TmempResetNode = CmdTempID242.PassPermit_CM.ResetNodeID_Main[i];

		if (HIGH_BYTE == 0) {
			nStopNode += TmempStopNode << ((2 - i) * 8);
			nResetNode +=TmempResetNode << ((2 - i) * 8);
		}
		else
		{
			nStopNode += TmempStopNode << (i * 8);
			nResetNode += TmempResetNode << (i * 8);
		}
	}

	m_pCurrCIDUSE.nStopNodeID = nStopNode;
	m_pCurrCIDUSE.nResetNodeID = nResetNode;
	m_pCurrCIDUSE.bCIDUse = 0x00;

//	ADD_LOG_TASKCONTROL("CheckCmdPPResponse %d %d",nStopNode,nResetNode);

	if((CmdTempID242.AckType == 0x00) || (CmdTempID242.ResultType == 0x02))
	{
		m_pPassPermit->SetPassPeriodTime(m_pPathSearch->GetPassPermitPriod());
		bCheckReturn = false;
		ADD_LOG_TASKCONTROL("통과허가응답 불가 : %d %d",CmdTempID242.AckType,CmdTempID242.ResultType);

	}else
	{
     	m_pPassPermit->SetPassPeriodTime(PASSTIME_NONE);
	}

	if(CmdTempID242.ResultType == 0x00)
	{
		m_pPassPermit->SetPassPeriodTime(PASSTIME_NONE);

	}
	else if(CmdTempID242.ResultType == 0x04)
	{

		m_pCurrCIDUSE.bCIDUse = 0x01;


		m_pPassPermit->SetPassPeriodTime(PASSTIME_NONE);

	}

	return bCheckReturn;


}
///< 통과허가개방에 대한 명령에 대한 응답 체크
bool TaskControl::CheckCmdPPOResponse(PKT_OCS2OHT* pRcvData, int QuickCmd)
{
	bool bCheckReturn = true;

	PROTOCOL_BODY_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244 CmdTempID244;
    memmove(&CmdTempID244, pRcvData->Value, sizeof(PROTOCOL_BODY_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244));

	//Check 할것들은??    Not thing
	return bCheckReturn;
}

///< 강제통과허가개방에 대한 명령에 대한 응답 체크 -> 일반개방으로 바뀜.
bool TaskControl::CheckCmdForcePPOResponse(PKT_OCS2OHT* pRcvData, int QuickCmd)
{

	bool bCheckReturn = true;
	DATA_PASS * pPasspermit; // Command를 위한 포인터 변수
    DATA_PASSOPEN * pPassOpen;
	//DATA_PASS * pTempPasspermit; // Command를 위한 포인터 변수
	PROTOCOL_BODY_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244 CmdTempID244;
	memmove(&CmdTempID244, pRcvData->Value, sizeof(PROTOCOL_BODY_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244));

	//Check 할것들은??    Not thing
	PASSPERMIT_COMMON TempPassPermit;
	int TempPassIndex = m_pPassPermit->GetPassPermitCount();
    int TempPassOpenIndex = m_pPassPermit->GetPassOpenCount();

//	if(CmdTempID244.AckType != 0x01)
//		return bCheckReturn;

	ADD_LOG_TASKCONTROL("[manual]통과허가요구 응답 : CheckCmdForcePPOResponse");

   if(TempPassIndex!= LISTCOUNT_EMPTY)
   {
		pPasspermit = (DATA_PASS *)m_pPassPermit->PopFirstPassPermit();
        TempPassPermit.SQNum = CmdTempID244.PassPermit_CM.SQNum;
		for(int i=0; i<3; i++)
		{
			TempPassPermit.StopNodeID[i] = pPasspermit->PassInfo.StopNodeID[i];
			TempPassPermit.ResetNodeID_Main[i] = pPasspermit->PassInfo.ResetNodeID_Main[i];
			TempPassPermit.ResetNodeID_Sub[i] = pPasspermit->PassInfo.ResetNodeID_Sub[i];

		}
        ADD_LOG_TASKCONTROL("[manual통과허가요구 응답 : CheckCmdForcePPOResponse_2 %d %d", TempPassPermit.StopNodeID[2],TempPassPermit.ResetNodeID_Main[2]);
        for(int i=0; i<3; i++)
		{
			TempPassPermit.StopNodeID[i] = CmdTempID244.PassPermit_CM.StopNodeID[i];
			TempPassPermit.ResetNodeID_Main[i] = CmdTempID244.PassPermit_CM.ResetNodeID_Main[i];
			TempPassPermit.ResetNodeID_Sub[i] = CmdTempID244.PassPermit_CM.ResetNodeID_Sub[i];
        }
		ADD_LOG_TASKCONTROL("[manual통과허가요구 응답 : CheckCmdForcePPOResponse_2 %d %d", TempPassPermit.StopNodeID[2],TempPassPermit.ResetNodeID_Main[2]);


		if(memcmp(&TempPassPermit, &(CmdTempID244.PassPermit_CM), sizeof(PASSPERMIT_COMMON))==0)
		{
			ADD_LOG_TASKCONTROL("[manual]통과허가응답 OK, Delete PassPermit");
        	delete pPasspermit;
		}
		else
		{
		   ADD_LOG_TASKCONTROL("[manual]통과허가응답 NG, Add PassPermit");
		   m_pPassPermit->AddPassPermit(pPasspermit);

        }

   }

   ADD_LOG_TASKCONTROL("[manual]통과허가개방 PassOpenListCount: %d",TempPassOpenIndex);
   if(TempPassOpenIndex!= LISTCOUNT_EMPTY)
   {
            for(int Index = 0; Index < TempPassOpenIndex; Index++)
			{
                ADD_LOG_TASKCONTROL("Index:%d",Index);
				pPassOpen  = (DATA_PASSOPEN *)m_pPassPermit->ReferPassOpen(Index);
                pPassOpen->PassOpneInfo.SQNum = CmdTempID244.PassPermit_CM.SQNum;

                if(memcmp(&pPassOpen->PassOpneInfo, &(CmdTempID244.PassPermit_CM), sizeof(PASSPERMIT_COMMON)) == 0)
                {
						ADD_LOG_TASKCONTROL("[manual]강제통과허가 개방 완료, deletePassOpen");
						Sleep(1);

                        m_pPassPermit->DeletePassOpen(Index);

                        if(pPassOpen->bPassOpenType == FORCE_TYPE)
                        {
                            m_bCheckPassOpenListCount = false;
                        }


						break;
                }


			}

   }

	//TP에 표시할 정보를 보내기.. puting 2016-05-24

	return bCheckReturn;
}



 ///< 투입 요구 에 대한 명령에 대한 응답 체크
bool TaskControl::CheckCmdInsertResponse(PKT_OCS2OHT* pRcvData, int QuickCmd)
{
	bool bCheckReturn = true;

	PROTOCOL_BODY_OHT_INPUT_REQUEST_RESPONSE CmdTempInsert;
	memmove(&CmdTempInsert, pRcvData->Value, sizeof(PROTOCOL_BODY_OHT_INPUT_REQUEST_RESPONSE));

	ADD_LOG_TASKCONTROL("Ack%x,VHL_Insert%x",CmdTempInsert.Ack,m_pOHTMainStatus->StandardStatusOHT.VHL_Insert);
	if((CmdTempInsert.Ack == 0x01) || (m_pOHTMainStatus->StandardStatusOHT.VHL_Insert != 0x02))
		bCheckReturn = false;

	return bCheckReturn;


}


 ///< 투입 요구 에 대한 명령에 대한 응답 체크
bool TaskControl::CheckCmdMoveRestart(PKT_OCS2OHT* pRcvData, int QuickCmd)
{
	bool bCheckReturn = true;
	int nCurNode = m_pExecuteInfo->ExecutePositionInfo.CurrNode;
	int nCurOffset = m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset;

	PROTOCOL_BODY_MOVE_RESTART_OCS2OHT_ID_250 CmdTempMoveRestart;
	memmove(&CmdTempMoveRestart, pRcvData->Value, sizeof(PROTOCOL_BODY_MOVE_RESTART_OCS2OHT_ID_250));

	ADD_LOG_TASKCONTROL("MoveRestart:%x",CmdTempMoveRestart.AcOrder);


	EnterCriticalSection(&OHTMainForm->m_CS);

	m_pExecuteInfo->FlagCIDInfo.cMoveResult = CmdTempMoveRestart.AcOrder;
	m_pExecuteInfo->FlagCIDInfo.bMoveRestart = true;

	m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_CID_RETYE); //AOHC-196 사전 로그 추가
	ADD_MD_LOG("CID_RETYE Cur N :%d, Offset : %d ",nCurNode,nCurOffset);

	LeaveCriticalSection(&OHTMainForm->m_CS);


	bCheckReturn = false;

	return bCheckReturn;


}
void TaskControl::CIDInfoOfPassPermit()
{
	int nCountPassPermit = m_pPassPermit->GetPassPermitCount();
	int nCountPassPermitOpen = m_pPassPermit->GetPassOpenCount();
	DATA_PASS 		* pCurPPCommon; // Command를 위한 포인터 변수
	DATA_PASSOPEN   *pPPOCommon;
	bool bSameCommon = false;

	if(nCountPassPermit != LISTCOUNT_EMPTY)
	{
		  //최초 통과허가 위치 확인(개방이 없는 싸이클리스트)
		  for(int Index =0; Index < nCountPassPermit; Index++)
		  {
				bSameCommon = false;
				pCurPPCommon = (DATA_PASS*)m_pPassPermit->ReferPassPermit(Index);

				if(nCountPassPermitOpen != LISTCOUNT_EMPTY)
				{
					for(int Index_2 =0; Index_2 < nCountPassPermitOpen; Index_2++)
					{
						//ADD_LOG_TASKCONTROL("[Check]CIDInfoOfPassPermit SQ %d",pCurPPCommon->PassInfo.SQNum);

						pPPOCommon = (DATA_PASSOPEN*)m_pPassPermit->ReferPassOpen(Index_2);
						if(memcmp(&(pPPOCommon->PassOpneInfo), &(pCurPPCommon->PassInfo), sizeof(PASSPERMIT_COMMON)) == 0)
						{
							bSameCommon = true;
							break;
						}

					}
					if(bSameCommon == false) break;
				}
				else
					break;

		  }
          //통과허가 값 반환
		  int nStopNode = 0, nResetNode = 0;
		  for (int i = 0; i < 3; i++)
		  {
				// Node 비트연산   puting
				int TmempStopNode = pCurPPCommon->PassInfo.StopNodeID[i];
				int TmempResetNode = pCurPPCommon->PassInfo.ResetNodeID_Main[i];

				if (HIGH_BYTE == 0) {
					nStopNode += TmempStopNode << ((2 - i) * 8);
					nResetNode +=TmempResetNode << ((2 - i) * 8);
				}
				else
				{
					nStopNode += TmempStopNode << (i * 8);
					nResetNode += TmempResetNode << (i * 8);
				}
		  }

		  m_pListCIDUSE.nStopNodeID = nStopNode;
		  m_pListCIDUSE.nResetNodeID = nResetNode;
		  m_pListCIDUSE.bCIDUse =  pCurPPCommon->bCIDType;

//		  ADD_LOG_TASKCONTROL("CIDInfoOfPassPermit %d %d ",nStopNode,nResetNode);



	}else
	{
		memset(&m_pListCIDUSE, 0x00, sizeof(FLAG_CID_NOT_USE));
    }
}

///< 통과문의에 대한 명령에 대한 응답 체크
bool TaskControl::CheckCmdPPREQResponse(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt,int QuickCmd)
{
	bool bCheckReturn = true;

	DATA_PASS * pCurPPCommon; // Command를 위한 포인터 변수
	int nCountPassPermit = m_pPassPermit->GetPassPermitCount();
	int TempCount = 0;

	PROTOCOL_BODY_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246 CmdTempID246;
	memset(&CmdTempID246, 0x00, sizeof(PROTOCOL_BODY_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246));

	ADD_LOG_TASKCONTROL("CountCylce Start");

	if(nCountPassPermit < MAX_PASSREQ)
		CmdTempID246.Finish_PassPermitNum  = nCountPassPermit;
	else
		CmdTempID246.Finish_PassPermitNum  = MAX_PASSREQ;

	ADD_LOG_TASKCONTROL("CountCylce:%d PassNum %d",nCountPassPermit,CmdTempID246.Finish_PassPermitNum);

	if(nCountPassPermit != LISTCOUNT_EMPTY)
	{
		try{
			for(int Index =0; Index < CmdTempID246.Finish_PassPermitNum; Index++)
			{
				pCurPPCommon = (DATA_PASS*)m_pPassPermit->ReferPassPermit(Index);

			   //	ADD_LOG_TASKCONTROL("[통과문의]GetPPIndex:%d Index %d Type %d",m_pPassPermit->GetPPIndex(),Index,pCurPPCmd->CycleType);
				memmove(&(CmdTempID246.PassPermit_CM[Index]), &(pCurPPCommon->PassInfo), sizeof(PASSPERMIT_COMMON));

//				memmove(CmdTempID246.PassPermit_CM[Index], pCurPPCommon, sizeof(PASSPERMIT_COMMON));

				ADD_LOG_TASKCONTROL("PassPermitData StopNode %d SQ %d",CmdTempID246.PassPermit_CM[Index].StopNodeID[2],CmdTempID246.PassPermit_CM[Index].SQNum);
			}
		}
		catch(...)
		{
			ADD_LOG_TASKCONTROL("CountCylce:%d",nCountPassPermit);
		}


	}

	memmove(pAckPkt->AckPkt.Value, &CmdTempID246, sizeof(PROTOCOL_BODY_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246));
	pAckPkt->AckPkt.TAG.Value = TagID::RESPONSE_PASSPERMITREQ_OHT2OCS;
	pAckPkt->Length = sizeof(PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246);

	return bCheckReturn;
}
//AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
///< 통과문의에 대한 명령에 대한 응답 체크
bool TaskControl::CheckCmdAutoChangeResponse(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt,int QuickCmd)
{

	PROTOCOL_BODY_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252 CmdTempID252;
	memset(&CmdTempID252, 0x00, sizeof(PROTOCOL_BODY_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252));


	//상세 코드 받기.
	CmdTempID252.AutoChangeACK.PresionCode = CheckStatusForAutoChange(); //AOHC-232 정전 복구 코드 비활성화 위해 0x01 추가 -> 추후 사용시 CheckStatusForAutoChange(); 활성 시키기


	if(CmdTempID252.AutoChangeACK.PresionCode !=0x00)
	{
		CmdTempID252.AutoChangeACK.Accept_Error = ON;
	}
	else
	{
	   CmdTempID252.AutoChangeACK.VHL_Auto = CheckAutoChangeFullSemi();

	   //세미 오토인 경우 Mark 동작없이 이적재 완료까지만 처리하도록 하기.
    }

	 ADD_LOG_TASKCONTROL("VHL Auto %d, Error  %d, 상세Code %d",CmdTempID252.AutoChangeACK.VHL_Auto,CmdTempID252.AutoChangeACK.Accept_Error,CmdTempID252.AutoChangeACK.PresionCode);
	memmove(pAckPkt->AckPkt.Value, &CmdTempID252, sizeof(PROTOCOL_BODY_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252));
	pAckPkt->AckPkt.TAG.Value = TagID::RESPONSE_AUTOCHANGE_OCS_OHT;
	pAckPkt->Length = sizeof(PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252);

	return !(bool)CmdTempID252.AutoChangeACK.Accept_Error;
}

   ///< 데이터 지시에 대한 응답 체크
bool TaskControl::CheckCmdDataTransfer(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, int QuickCmd)
{
	bool bCheckReturn = true;

	PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS AckTempIDData;
	PROTOCOL_BODY_DATA_TRANSPER_ORDER_OCS2OHT  CmdTempIDDataTrans;

	memset(&AckTempIDData, 0x00, sizeof(PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS));
	memmove(&CmdTempIDDataTrans, pRcvData->Value, sizeof(PROTOCOL_BODY_DATA_TRANSPER_ORDER_OCS2OHT));

	if((m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID ==0x00) &&
	   (m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataStatus == 0x00) &&
	   (m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataType == 0x00)&&
	   (m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataSuccess == 0x00) &&
	   (m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail == 0x00))
	  {
			bCheckReturn = false;
			ADD_LOG_TASKCONTROL("[데이터전송지시 ACK]OrderID %x %x Cmd_Type %x %d.%d.%d.%d User:%s,PW:%s,DSto:%s",
			CmdTempIDDataTrans.OrderID,CmdTempIDDataTrans.Request.TestMode,
			CmdTempIDDataTrans.Request.CMDType,
			CmdTempIDDataTrans.IpAddress[0],
			CmdTempIDDataTrans.IpAddress[1],
			CmdTempIDDataTrans.IpAddress[2],
			CmdTempIDDataTrans.IpAddress[3],
			CmdTempIDDataTrans.UserName,
			CmdTempIDDataTrans.PassWord,
			CmdTempIDDataTrans.DataSendTo);


			
			OHTMainForm->g_DataTransProgressStatus = DATATRANSFERPROGRESS_ACK_TRIGGER_ON;
			//하기 2개 이외의 데이터는 AST에서 전송받아 저장된 값 사용
			m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID = CmdTempIDDataTrans.OrderID;
			g_CMDType = CmdTempIDDataTrans.Request.CMDType;
			
			int iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT);

			// 동일 명령이지만 TP에서 온 경우 이렇게 바꿔서 보내준다. AS와 FM은 알 필요가 없다
			PKT_DATA_TRANSPER_ORDER_OCS2OHT SEND_PKT;
			memmove(&SEND_PKT, pRcvData, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));
			if(SEND_PKT.HEADER.MachineType == MACHINETYPE_TP)
			{
				SEND_PKT.HEADER.MachineType = MACHINETYPE_VHL;
				SEND_PKT.TAG.Value = TagID::CMD_DATATRANSFER_OCS2OHT;
			}

			m_pUDPDiag->UnicastSend_OHT_2_AS((char*)&SEND_PKT, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));
			//m_pUDPDiag->UnicastSend_OHT_2_AS((char*)pRcvData, iPacketSize);

			m_bDataTransper_InProgress = true;
			FirmwareUpdateMessageToTp("[데이터 전송 Start] InProgress = true");
			ADD_LOG_TASKCONTROL("[데이터 전송 Start] InProgress = true");
	  }
	  else
	  {
			AckTempIDData.Request.CMDType = CmdTempIDDataTrans.Request.CMDType;
			AckTempIDData.Ack.TransOrderID = CmdTempIDDataTrans.OrderID;
			AckTempIDData.Ack.Accept_Error  = 0x01;
			AckTempIDData.Ack.Accept_Error_Class = 0x07;

			ADD_LOG_TASKCONTROL("[데이터전송지시 NAK] 전송구분 : %d  요구구분 : %d  OrderID : %d  Accept_Error : 0x%02x  Accept_Error_Class : 0x%02x",
									AckTempIDData.Request.TestMode, AckTempIDData.Request.CMDType, 
									AckTempIDData.Ack.TransOrderID,
									AckTempIDData.Ack.Accept_Error, AckTempIDData.Ack.Accept_Error_Class);

			memmove(pAckPkt->AckPkt.Value, &AckTempIDData, sizeof(PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS));
			pAckPkt->AckPkt.TAG.Value = TagID::RESPONSE_DATATRANSFER_OHT2OCS;
			pAckPkt->Length = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);
	  }


	return bCheckReturn;

}
   ///< 버전 정보에 대한 응답 체크
bool TaskControl::CheckCmdVersion(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, int QuickCmd)
{
	bool bCheckReturn = true;
	AnsiString strMonitor, strAppVer, strParamVer, strMapVer;    //Monitor Parameter는 초기에 Loding 처리.
	AnsiString strFullPath, strTmp;
	FILE *fi;
	char cTmp[FILE_TMP_SIZE];

	PROTOCOL_BODY_VERSION_RESPONSE AckTempVersionsimple;    //ACk
	memset(&AckTempVersionsimple, 0x00,sizeof(PROTOCOL_BODY_VERSION_RESPONSE));

	// (1) get monitor -----------------------------------------

	strMonitor = "";

	strMonitor = m_strMonitor;

	memset(&AckTempVersionsimple.AppMonitorVersion, '\0', 32);	// ASCII_SPACE, 32);
	memcpy(&AckTempVersionsimple.AppMonitorVersion, strMonitor.c_str(), strMonitor.Length());

	ADD_LOG_TASKCONTROL("[버전정보 응답] Monitor : %s", strMonitor);
    // -----------------------------------------------------------------------------


	// (2) get main -----------------------------------------
	//  GetMainVer
	strAppVer = "";
	strTmp.sprintf("Main=%u.%u.%u.%u ",
		(BYTE)OHTMainForm->VerInfo.Major,
        (BYTE)OHTMainForm->VerInfo.Minor,
        (BYTE)OHTMainForm->VerInfo.Release,
        (BYTE)OHTMainForm->VerInfo.Build);
	strAppVer = strAppVer + strTmp;

	// GetAMCVer
	int nAMCVer = m_pHWSet->GetAMCVersion();

    int nAMCVerMajor = nAMCVer / 10000;
    int nAMCVerMinor = (nAMCVer - (nAMCVerMajor * 10000)) / 100;
    int nAMCVerRelease = (nAMCVer - (nAMCVerMajor * 10000)) % 100;

	strTmp.sprintf("Amc=%d.%d.%02d ", nAMCVerMajor, nAMCVerMinor, nAMCVerRelease);
	strAppVer = strAppVer + strTmp;

	memset(&AckTempVersionsimple.ApplicationVersion, '\0', 32);	//ASCII_SPACE, 32);
	memcpy(&AckTempVersionsimple.ApplicationVersion, strAppVer.c_str(), strAppVer.Length());

	ADD_LOG_TASKCONTROL("[버전정보 응답] strAppVer : %s", strAppVer);
	// -----------------------------------------------------------------------------

	// (3) get param -----------------------------------------
	// 각 파일별로 4바이트이므로, 파라미터는 총 8개까지만 버전 표시가능
	// 각 버전은 스트링으로 2글자까지만 허용 (용량부족함)

	strParamVer = "";

	// 펌웨어 업데이트 기능으로 지원하는 6개 파일
	// Driving.ini, EQ PIO.ini, Trans.ini, DiagParam.ini, PathSearch.ini, IOMap.ini
	strTmp.sprintf("DR%s", ReqIniVersion(PARMA_DRIVING));
	strParamVer = strParamVer + strTmp;

	strTmp.sprintf("EQ%s", ReqIniVersion(PARMA_EQPIO));
	strParamVer = strParamVer + strTmp;

	strTmp.sprintf("TR%s", ReqIniVersion(PARMA_TRANS));
	strParamVer = strParamVer + strTmp;

	strTmp.sprintf("DP%s", ReqIniVersion(PARMA_DIAG));
	strParamVer = strParamVer + strTmp;

	strTmp.sprintf("PS%s", ReqIniVersion(PARMA_PATHSEARCH));
	strParamVer = strParamVer + strTmp;

	strTmp.sprintf("IO%s", ReqIniVersion(PARMA_IOMAP));
	strParamVer = strParamVer + strTmp;
	// ---------------------------------------------------

	memset(&AckTempVersionsimple.ParameterVersion, '\0', 32);	// ASCII_SPACE, 32);
	memcpy(&AckTempVersionsimple.ParameterVersion, strParamVer.c_str(), strParamVer.Length());

	ADD_LOG_TASKCONTROL("[버전정보 응답] strParamVer : %s", strParamVer);
	// -----------------------------------------------------------------------------

	// (4) get map -----------------------------------------
	UINT Major, Minor, Revision;
	bool bMapAdjust = false;
	bool bMapModify = false;
	
	int iMapVer = OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.Version;

	// ------------------------------
	// 4-1. get info
    unsigned int CID_DataType 			= OHTMainForm->GetCIDDataManager_STD()->m_CIDFileHeader.DataType;
    unsigned int Collision_DataType 	= OHTMainForm->GetCollisionDataManager_STD()->m_CollisionFileHeader.DataType;
    unsigned int FlowControl_DataType 	= OHTMainForm->GetFlowControlDataManager_STD()->m_FlowControlFileHeader.DataType;
    unsigned int Map_DataType 			= OHTMainForm->GetMapDataManager_STD()->m_MapFileHeader.DataType;
    unsigned int Station_DataType 		= OHTMainForm->GetStationDataManager_STD()->m_StationFileHeader.DataType;
    unsigned int Teaching_DataType 		= OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.DataType;
    unsigned int Path_DataType 			= OHTMainForm->GetPathDataManager_STD()->m_PathDataFileHeader.DataType;

	// 4-2. check adjust
	 if(
	 	  ((Map_DataType & bit_0) == bit_0)	 	  
	 	||((CID_DataType & bit_0) == bit_0)
		||((Collision_DataType & bit_0) == bit_0)
		||((FlowControl_DataType & bit_0) == bit_0)		
		||((Station_DataType & bit_0) == bit_0)
		||((Teaching_DataType & bit_0) == bit_0)
		||((Path_DataType & bit_0) == bit_0)
	 
	   )
	 	{
			bMapAdjust = true;
	 	}

	 // 4-2. check modify
	  if(
	  	   ((Map_DataType & bit_1) == bit_1)	  	  
	  	 ||((CID_DataType & bit_1) == bit_1)
		 ||((Collision_DataType & bit_1) == bit_1)
		 ||((FlowControl_DataType & bit_1) == bit_1)		 
		 ||((Station_DataType & bit_1) == bit_1)
		 ||((Teaching_DataType & bit_1) == bit_1)
		 ||((Path_DataType & bit_1) == bit_1)
		 
		)
		  {
		  	bMapModify= true;
		  }
	// ------------------------------

	Major = iMapVer & 0xFFFF0000;
	Major = Major >> 16;
	Minor = iMapVer & 0x0000FFFF;

	strMapVer.sprintf("%d.%d", Major, Minor);

	if(bMapAdjust == true)
		strMapVer = strMapVer + ".Adjust"; 

	if(bMapModify == true)
		strMapVer = strMapVer + ".Modify";
	
/*
	Major = iMapVer & 0xFF000000;
	Major = Major >> 24;

	Minor = iMapVer & 0xFFF000;
	Minor = Minor >> 12;

	Revision = iMapVer & 0xFFF;

	strMapVer.sprintf("%d.%d.%d", Major, Minor, Revision);
*/
	
	memset(&AckTempVersionsimple.MapVersion, '\0', 32);	// ASCII_SPACE, 32);
	memcpy(&AckTempVersionsimple.MapVersion, strMapVer.c_str(), strMapVer.Length());

	ADD_LOG_TASKCONTROL("[버전정보 응답] strMapVer : %s", strMapVer);
	// -----------------------------------------------------------------------------
	
//	m_OHTMainStatus->StatusTP.AMCVer.Data = m_pHWSet->GetAMCVersion();


#if 0
	AckTempVersionsimple.AppMonitorVersion[0] = OHTMainForm->GetOHTProgramVersion().Info.Build;
	AckTempVersionsimple.AppMonitorVersion[1] = OHTMainForm->GetOHTProgramVersion().Info.Release;
	AckTempVersionsimple.AppMonitorVersion[2] = OHTMainForm->GetOHTProgramVersion().Info.Minor;
	AckTempVersionsimple.AppMonitorVersion[3] = OHTMainForm->GetOHTProgramVersion().Info.Major;
	AckTempVersionsimple.AppMonitorVersion[4] = 0x00;
	AckTempVersionsimple.AppMonitorVersion[5] = 0x00;

	AckTempVersionsimple.ApplicationVersion[0] = m_pOHTMainStatus->StatusTP.AMCVer.Info.Build;
	AckTempVersionsimple.ApplicationVersion[1] = m_pOHTMainStatus->StatusTP.AMCVer.Info.Release;
	AckTempVersionsimple.ApplicationVersion[2] = m_pOHTMainStatus->StatusTP.AMCVer.Info.Minor;
	AckTempVersionsimple.ApplicationVersion[3] = m_pOHTMainStatus->StatusTP.AMCVer.Info.Major;
	AckTempVersionsimple.ApplicationVersion[4] = 0x00;

	AckTempVersionsimple.ParameterVersion[0] = (m_pOHTMainStatus->m_cpMonitorData.HIDData.Fimware >> 24) & 0xFF;  	//HID 레귤레이터 버전
	AckTempVersionsimple.ParameterVersion[1] = (m_pOHTMainStatus->m_cpMonitorData.HIDData.Fimware >> 16) & 0xFF;  	//HID 레귤레이터 버전
	AckTempVersionsimple.ParameterVersion[2] = (m_pOHTMainStatus->m_cpMonitorData.HIDData.Fimware >> 8) & 0xFF;  	//HID 레귤레이터 버전
	AckTempVersionsimple.ParameterVersion[3] = (m_pOHTMainStatus->m_cpMonitorData.HIDData.Fimware) & 0xFF;  		//HID 레귤레이터 버전
	AckTempVersionsimple.ParameterVersion[4] = 0x00;

	AckTempVersionsimple.MapVersion[0] = 0x88;
	AckTempVersionsimple.MapVersion[1] = 0x88;
	AckTempVersionsimple.MapVersion[2] = 0x88;
	AckTempVersionsimple.MapVersion[3] = 0x88;
	AckTempVersionsimple.MapVersion[4] = 0x88;
#endif

	memmove(pAckPkt->AckPkt.Value, &AckTempVersionsimple, sizeof(PROTOCOL_BODY_VERSION_RESPONSE));
	pAckPkt->AckPkt.TAG.Value = TagID::RESPONSE_SIMPLE_VERSION_OHT_OCS;
	pAckPkt->Length = sizeof(PKT_SIMPLE_VERSION_RESPONSE);


	return bCheckReturn;

}

///< Station 정보 요구에 대한 응답.====================================
bool TaskControl::CheckStaionInfoReqResponse(PKT_OCS2OHT* pRcvData, int QuickCmd)
{
	bool bCheckReturn = true;

	PROTOCOL_BODY_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112 tmpID112;
	memmove(&tmpID112, (PROTOCOL_BODY_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112*)pRcvData->Value,	sizeof(PROTOCOL_BODY_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112));

	if((tmpID112.ResponseType == 0x01) ||(tmpID112.PathType ==0x01))
	{
		ADD_LOG_TASKCONTROL("Sation Info Req :Don't Exsist %d %d", tmpID112.ResponseType,tmpID112.PathType);
		bCheckReturn = false;
	}

	ADD_LOG_TASKCONTROL("[StaionInfo]MarkType:%d,ParentNodeID:%d,NextNodeID:%d RunDist:%d",
		tmpID112.StationData_MarkType,tmpID112.StationData_ParentNodeID,tmpID112.StationData_NextNodeID,tmpID112.StationData_Offset);

	//Check 할것들은??    Not thing
	return bCheckReturn;
}

//통계연결 유무
bool TaskControl::CheckCmdDiagConnect(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt)   	 ///< 통계정보 초기화 요청
{
	bool bCheckReturn = true;

	DIAG_PKT_BODY_CONNECT_CHK_RESPONSE TempConnect;    //ACk


	memset(&TempConnect, 0x00,sizeof(DIAG_PKT_BODY_CONNECT_CHK_RESPONSE));

	TempConnect.result = DIAG_ACK_OK;


	memmove(pAckPkt->AckPkt.Value, &TempConnect, sizeof(DIAG_PKT_BODY_CONNECT_CHK_RESPONSE));
	pAckPkt->AckPkt.HEADER.CommandID = DiagCmdID::CLW_TO_AGENT_CONNECT_CHK_RESPONSE;
	pAckPkt->Length = sizeof(DIAG_PKT_CONNECT_CHK_RESPONSE);

	return bCheckReturn;
}

bool TaskControl::CheckCmdDiagInfoReq(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt)   ///< 통계정보 보고 요청 응답
{
 	bool bCheckReturn = true;

	DIAG_PKT_BODY_INFO_DATA_RESPONSE AckTempDiagResponse;    //ACk
	DIAG_PKT_BODY_INFO_DATA_REQUEST TempRcv;


	memset(&AckTempDiagResponse, 0x00,sizeof(DIAG_PKT_BODY_INFO_DATA_RESPONSE));
	memmove(&TempRcv, &pRcvData->Value,sizeof(DIAG_PKT_BODY_INFO_DATA_REQUEST));
 	AckTempDiagResponse.Parameter =TempRcv.Parameter;
	AckTempDiagResponse.Value = m_DiagManager->GetStatDiagData(TempRcv.Parameter);


	memmove(pAckPkt->AckPkt.Value, &AckTempDiagResponse, sizeof(DIAG_PKT_INFO_DATA_RESPONSE));
	pAckPkt->AckPkt.HEADER.CommandID = DiagCmdID::DIAG_INFO_DATA_RESPONSE;
	pAckPkt->Length = sizeof(DIAG_PKT_INFO_DATA_RESPONSE);

	return bCheckReturn;
}

//통계정보 초기화
bool TaskControl::CheckCmdDiagInitReq(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt)   	 ///< 통계정보 초기화 요청
{
	bool bCheckReturn = true;

	DIAG_PKT_BODY_DATA_RESET_RESPONSE AckTempDiagInits;    //ACk
	DIAG_PKT_BODY_DATA_RESET TempRcv;

	memset(&AckTempDiagInits, 0x00,sizeof(DIAG_PKT_BODY_DATA_RESET_RESPONSE));
	memmove(&TempRcv, &pRcvData->Value,sizeof(DIAG_PKT_BODY_DATA_RESET));
	AckTempDiagInits.Parameter =TempRcv.Parameter;
	AckTempDiagInits.SuccessCode =  m_DiagManager->ClearStatDiagData(TempRcv.Parameter);


	memmove(pAckPkt->AckPkt.Value, &AckTempDiagInits, sizeof(DIAG_PKT_BODY_DATA_RESET_RESPONSE));
	pAckPkt->AckPkt.HEADER.CommandID = DiagCmdID::DIAG_DATA_RESET_RESPONSE;
	pAckPkt->Length = sizeof(DIAG_PKT_DATA_RESET_RESPONSE);

	return bCheckReturn;
}


bool TaskControl::CheckCmdDiagInfoResponse(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt)  	 ///< 통계정보 응답
{
	bool bCheckReturn = true;

	bCheckReturn = false;

	DIAG_PKT_BODY_DATA_INFO_RESPONSE TempRcv;
	memmove(&TempRcv, &pRcvData->Value,sizeof(DIAG_PKT_BODY_DATA_INFO_RESPONSE));

	m_DiagManager->UpdateStatDiagAllData(TempRcv);

	return bCheckReturn;
}


// ---------------------------------------------------------------------------
/**
 @brief   Task(Cmd)List에서 Cyclelist로 작업취소에 의해 마지막 이적재 부분만 삭제하는 함수
 @author  puting
 @date    2018.03.10
 @return
 */
void TaskControl::JobOrderDataCancel()
{
 	PKT_CYCLECMDDATA * pTempCurCmd = NULL; // Command를 위한 포인터 변수  puting 작업취소에 의해 생긴부분
	BYTE chkStatus;

		//싸이클 리스트 인덱스 정리
	//이적재 중이 아닐때, 주행중이나 정지상태. 대기상태
	pTempCurCmd = (PKT_CYCLECMDDATA*) m_pCmdManager->ReferCycle(m_pCmdManager->GetCycleCount()-1);
	chkStatus = GetExecuteStatus();
	if((chkStatus == STS_GOING) || (chkStatus == STS_INIT) || (chkStatus == STS_ARRIVED)) // Going
	{
		ADD_LOG_TASKCONTROL("Judge TAGTYPE_JOBORDER_JOBCANCEL_1:%d %d %d",chkStatus,pTempCurCmd->CycleType,pTempCurCmd->IsExcuteInfo);
		// 마지막 이적재 싸이클 삭제처리.
		if(pTempCurCmd->CycleType == CYCLETYPE_TRANS)
		{
			if(pTempCurCmd->IsExcuteInfo == false)
			{
				if(m_pPassPermit->GetPPOIndex()==(m_pCmdManager->GetCycleCount()-1))
				{
					m_pPassPermit->SetDecreasePPOIndex();
				}

				if(m_pPassPermit->GetPPIndex()==(m_pCmdManager->GetCycleCount()-1))
				{
					m_pPassPermit->SetDecreasePPIndex();
				}
			}
		   //여기서 메모리에 들은거 삭제처리..
			m_pCmdManager->DeleteBackCycle();

		}
	}

}


// ---------------------------------------------------------------------------
/**
 @brief   Task(Cmd)List에서 Cyclelist로 넘겨주기 위해 감시하는 부분
 @author  puting
 @date    2015.10.28
 @param   pCmdData : 처리할 Commmand
 @return  정상인 경우 0을 리턴, 에러 발생 시 에러 코드 리턴
 */
int TaskControl::SuperviseTaskList2CycleList()
{

	/*
	 -> OHT 에러 상태가 아니며, TaskControl에서 명령을 이전 명령에 대해 수행처리 완료되거나,
	 아무런 수행 명령 수행 중이 아닐때
	 //정지 지시일 경우, Pop하지 않음
	 // 기동에 대한 처리는 ??

	 < CycleList의 모든 작업사항이 완료되었을 때, 다음 Task List의 명령이 처리 될 수있다.>
	 - 주행이나 이적재 명령, MTL, 층간이동 명령이 쌓여있으면, Manual 동작 불가
	 - Trans or MTL,층간이동 명령은 2개 이상 쌓일 수 없다.
	 - 주행 명령은 여러 개 쌓일 수 있다.
	 - Trans or MTL, 층간이동 명령은 항상 마지막이다.
	 - 매뉴얼 명령일떄 다른 명령은 올 수 없다.
	 - 매뉴얼 명령은 수동 모드에서만 가능하다.
	 */

	int Error = NO_ERR;
	int nCheckCountTaskList = m_pCmdManager->GetTaskCount();
	int nCheckCountCycleList = m_pCmdManager->GetCycleCount();


	//Task List -> Cycle LIst 감시하는 부분 puting 2015.10.28
	if (nCheckCountTaskList != LISTCOUNT_EMPTY)
	{


		PKT_TASKCMDDATA * pCurCmd; // Command를 위한 포인터 변수

		pCurCmd = (PKT_TASKCMDDATA*)m_pCmdManager->ReferFirstTask();

		switch (pCurCmd->DetailTagSort)
		{

			case TAGTYPE_PASSPERMIT_RESPONSE: // 통과허가응답
			case TAGTYPE_PASSPERMIT_OPEN_RESPONSE: // ////<통과허가개방응답
			case TAGTYPE_FORCE_PASSPERMIT_OPEN_RESPONSE: // ////<강제통과허가개방응답
				 if(nCheckCountCycleList != LISTCOUNT_EMPTY)
				  {
					try{
						Error = CheckCycleList(pCurCmd);
					}catch(...)
						ADD_LOG_TASKCONTROL("CheckCycleList Exception!!");
						///< 강제 통과허가 개방 응답요청한 Cycle값을 Scan해서 값 변경해준다.

						// 실제 변경할거 구현.
				   }
				 	delete m_pCmdManager->PopFirstTask();
//				}
				break;
			case TAGTYPE_JOBORDER_JOBCHAGE:
			case TAGTYPE_JOBORDER_TARGETCHAGE:
			case TAGTYPE_JOBORDER_PATHCHAGE:

				// 1) 에러 상태 확인
				if((GetExecuteStatus() != STS_ERROR) && (m_pOHTMainStatus->CurCycle != CYCLE::CycleNotExist))
				{

					   if(CheckStatusForCycleClear())
					   {
							ADD_LOG_TASKCONTROL("Job Change");

							//목적지 정보 초기화(해제)
							m_pHWSet->m_pDrivingAxis->SetTargetPosRelease();

							Error = ChangeCycleList(pCurCmd);
							m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag = false;
							m_pOHTMainStatus->JobChangeFlag.StartStationID = 0;
							m_pOHTMainStatus->JobChangeFlag.dChangDistance = 0.0;

							delete m_pCmdManager->PopFirstTask();
					   }
						///< 강제 통과허가 개방 응답요청한 Cycle값을 Scan해서 값 변경해준다.

						// 실제 변경할거 구현.
 				}
				else
                	delete m_pCmdManager->PopFirstTask();
				break;

			case TAGTYPE_JOBORDER_JOBCLEAR:
					if((GetExecuteStatus() != STS_ERROR) && (nCheckCountCycleList != LISTCOUNT_EMPTY))
				{
						 if(CheckStatusForCycleClear())
						{
							InitClearCycle();
							delete m_pCmdManager->PopFirstTask();
                        }
						// 실제 변경할거 구현.					
				}
				else
					 delete m_pCmdManager->PopFirstTask();

				break;
			case TAGTYPE_JOBORDER_TARGETINIT: //싸이클은 이동싸이클로 변경하고 마지막에 이적재나 다른작업이 있는 경우는 모두 삭제처리.      puting
					//이동싸이클은 유지, 내림싸이클/올림싸이클,빼냄싸이클은 추후에 처리. (현재 주회주행이 있음)
					m_pOHTMainStatus->CurCycle = CYCLE::MovingToStation;

                    delete m_pCmdManager->PopFirstTask();
				break;
			case TAGTYPE_JOBORDER_JOBCANCEL: //싸이클은 이동싸이클로 변경하고 마지막에 이적재나 다른작업이 있는 경우는 모두 삭제처리.      puting
					//이동싸이클은 유지, 내림싸이클/올림싸이클,빼냄싸이클은 추후에 처리. (현재 주회주행이 있음)
//					if(m_pOHTMainStatus->CurCycle ==  CYCLE::TakeOut)   //주회주행으로 대체 가능


                    delete m_pCmdManager->PopFirstTask();
				break;

			case TAGTYPE_JOBORDER: // ///<작업지시
			case TAGTYPE_STATIONMOVEINFO: // Statiojn 이동 응답
				// Step 1) 주행 측 이동 명령 분리 및 저장

				if(GetExecuteStatus() != STS_ERROR)
				{
					 if(CheckStatusForCycle() &&
					   (nCheckCountCycleList == LISTCOUNT_EMPTY))
					 {
						//InitPassPermit();
						InitClearCycle();

						//목적지 정보 초기화(해제)
						m_pHWSet->m_pDrivingAxis->SetTargetPosRelease();

						Error = MakeCycleList(pCurCmd); // cycle의 종류에 분류
						delete m_pCmdManager->PopFirstTask();
					 }

				}
				else
					delete m_pCmdManager->PopFirstTask();


				// Step 1) 주행 측 이동 명령 분리 및 저장
              break;
			case TAGTYPE_FLOORMOVEMENT: // ///<층간이동지시

				// pCurCmd.TagType = TAGTYPE_FLOORMOVEMENT;

				if(GetExecuteStatus() != STS_ERROR)
				{
					 if(CheckStatusForCycle() &&
					   (nCheckCountCycleList == LISTCOUNT_EMPTY))
					 {
						InitClearCycle();
					    Error = MakeCycleList(pCurCmd); // cycle의 종류에 분류
						delete m_pCmdManager->PopFirstTask();

					 }

				}else
					delete m_pCmdManager->PopFirstTask();
				break;
			case TAGTYPE_MANUAL_COMMON: // TP 명령
				// by zzang9un 2013.07.16 : TP 명령을 2차 분류함

				if(GetExecuteStatus() != STS_ERROR)
				{
				   	if(CheckManualForCycle())
					{
						Error = MakeCycleList(pCurCmd);
					}
				}

				delete m_pCmdManager->PopFirstTask();

				break;

			default:
				///@todo 에러 코드 정의 필요
				Error = ERR_STD_CMD_DATA_FAIL;
				break;
		}

			//생성된 사이클이 없으면 초기화


	}

	return Error;

}

// ---------------------------------------------------------------------------
/**
 @brief   Command를 하위 cycle list에 추가하는 make하도록 분류하는 함수
 @author  puting
 @date    2015.10.01
 @param   pCmdData : 처리할 Commmand
 @return  정상인 경우 0을 리턴, 에러 발생 시 에러 코드 리턴
 */
int TaskControl::MakeCycleList(PKT_TASKCMDDATA* pCmdData)
{
	int nError = NO_ERR;
	PARSING_GO_INFO TempGOInfo;
	PARSING_TRANS_INFO TempTransInfo;
	EXECUTE_MANUALCONTROL_INFO	   TempManaulInfo;
	CMD_BIG_HEADER Header;  // Header를 parsing한 구조체

	UINT TempStationNID=0;

	STATION_DATA OCSStationData;
	memset(&OCSStationData, 0x00, sizeof(STATION_DATA));

	STATION_DATA *pSData = NULL;

	memset(&TempGOInfo, 0x00, sizeof(PARSING_GO_INFO));
	memset(&TempTransInfo, 0x00, sizeof(PARSING_TRANS_INFO));
	memset(&TempManaulInfo, 0x00, sizeof(EXECUTE_MANUALCONTROL_INFO));
	// bool IsListPop = false;  // 1이면 팝필요. 0이면 대기 .
	// PKT_CYCLECMDDATA *pTmeCycleData = new PKT_CYCLECMDDATA;
	/*
	 //일반 작업 지시 및 층간이동만 접수
	 //PathSearch는 안하고 및 Trans를 리스트에 넣어줌.
	 //통과허가 구간은 분리해서 list로 관리
	 // 실제 D T M에 바로 전달될 수준으로 구조체로 관리
	 */
//    char b =0x03;



	switch (pCmdData->DetailTagSort)
	 {
		case TAGTYPE_JOBORDER:
//		case TAGTYPE_JOBORDER_JOBCHAGE:
//		case TAGTYPE_JOBORDER_TARGETCHAGE:
//		case TAGTYPE_JOBORDER_PATHCHAGE:     // 작업지시 변경으로 추가됨. puting 2016 03 07
			PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121 tmpID121;
			memmove(&tmpID121, (PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121*)pCmdData->PKTDATA.Value,	sizeof(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121));
			
			ADD_LOG_TASKCONTROL("MakeCycleList_JOB Order");
//			ADD_LOG_TASKCONTROL("%d, %d, %d, %d, %d, %d, %d, %d ",tmpID121.StationData_Safety_Level, 
//				tmpID121.StationData_Reserved[0], tmpID121.StationData_Reserved[1], tmpID121.StationData_Reserved[2], tmpID121.StationData_Reserved[3], tmpID121.StationData_Reserved[4], tmpID121.StationData_Reserved[5], tmpID121.StationData_Reserved[6]);

			ADD_LOG_TASKCONTROL("GoInfo:[Extra]%d",tmpID121.PathData_Def_Direction.Driving_Flag);

//			m_pOHTMainStatus->bAutoRecovery = false;

			// Step 1) 이동명령이 발생한 경우만 처리
			// 위치확인 사이클도 이동명령으로 취급???
			if (tmpID121.PathData_Def_Direction.Driving_Flag)
			{
				m_pOHTMainStatus->bRealGoCycleing = true;


				TempGOInfo.CmdID = tmpID121.CmdID;
				TempGOInfo.CmdCycle = tmpID121.CmdCycle;

				TempGOInfo.MarkType =  tmpID121.StationData_MarkType;

				TempGOInfo.StationData_ParentNodeID = tmpID121.StationData_ParentNodeID;
				TempGOInfo.StationData_NextNodeID = tmpID121.StationData_NextNodeID;
				TempGOInfo.StationData_RunDist = tmpID121.StationData_Offset;
				TempGOInfo.TeachingData_STBOffset = tmpID121.TeachingData_STBOffset;    //Auto Teaching시 사용할 것.

				OCSStationData.StationID =  tmpID121.StationData_StationID;
				OCSStationData.StationType =  tmpID121.StationData_MarkType;
				OCSStationData.ParentNodeID =  tmpID121.StationData_ParentNodeID;
				OCSStationData.NextNodeID =  tmpID121.StationData_NextNodeID;
				OCSStationData.StationOffset =  tmpID121.StationData_Offset;
				OCSStationData.Safety_Level =  tmpID121.StationData_Safety_Level;

				ADD_LOG_TASKCONTROL("[Station]MarkType:%d,ParentNodeID:%d,NextNodeID:%d RunDist:%d, Safety_Level:%d, TempGOInfo.TeachingData_STBOffset %d",
				tmpID121.StationData_MarkType,tmpID121.StationData_ParentNodeID,tmpID121.StationData_NextNodeID,tmpID121.StationData_Offset, tmpID121.StationData_Safety_Level,TempGOInfo.TeachingData_STBOffset);

				TempGOInfo.PathData_Def_Direction = tmpID121.PathData_Def_Direction.Path_Direction;
				TempGOInfo.PathData_ExtraNum = tmpID121.PathData_ExtraNum;
				//tmpID121.StationData_Safety_Level = 0x01;//for test
				TempGOInfo.bIsTargetStationoPassPermitOpen = tmpID121.StationData_Safety_Level;  //puting 2017 08 03

				for (int i = 0; i < 3; i++)
				{
					// Node 비트연산   puting
					int Temp1 = tmpID121.StopNodeID[i];
					int Temp2 = tmpID121.StopStationID[i];
//					int Tmep3 = tmpID121.NextNodeID[i];
					int Tmep4 = tmpID121.PathData_StartNodeID[i];
					int Tmep5 = tmpID121.PathData_EndNodeID[i];
//					int Temp6 = tmpID121.StationData_Start_NextNodeID[i];


					if (HIGH_BYTE == 0) {
						TempGOInfo.StopNodeID += Temp1 << ((2 - i) * 8);
						TempGOInfo.StopStationID +=	Temp2 << ((2 - i) * 8);
//						TempGOInfo.NextNodeID += Tmep3<< ((2 - i) * 8);
						TempGOInfo.PathData_StartNodeID += Tmep4 << ((2 - i) * 8);
						TempGOInfo.PathData_EndNodeID += Tmep5 << ((2 - i) * 8);
//						TempGOInfo.StationDataStart_NextNodeID += Temp6 << ((2 - i) * 8);

					}
					else
					{
						TempGOInfo.StopNodeID += Temp1 << (i * 8);
						TempGOInfo.StopStationID +=	Temp2 << (i * 8);
//						TempGOInfo.NextNodeID += Tmep3<< (i * 8);
						TempGOInfo.PathData_StartNodeID += Tmep4 << (i * 8);
						TempGOInfo.PathData_EndNodeID += Tmep5 << (i * 8);
//						TempGOInfo.StationDataStart_NextNodeID += Temp6 << (i * 8);

					}

				}



				for (int j = 0; j < TempGOInfo.PathData_ExtraNum; j++) {
					for (int i = 0; i < 3; i++) {

						// Extra Node 비트연산   puting
						int Temp = tmpID121.PathData_ExtraID[i + (j * 3)];

						if (HIGH_BYTE == 0)
							TempGOInfo.PathData_ExtraID[j] += Temp << ((2 - i) * 8);
						else
							TempGOInfo.PathData_ExtraID[j] += Temp << (i * 8);

					}
				}

				ADD_LOG_TASKCONTROL("GoInfo:[Extra]Num:%d,First:%d,DefaulDirection:%d",
				TempGOInfo.PathData_ExtraNum,TempGOInfo.PathData_ExtraID[0], TempGOInfo.PathData_Def_Direction);

				//최종 위치 저장
				m_pExecuteInfo->nFinishOfNode = TempGOInfo.StopNodeID;
				m_pExecuteInfo->nFinishOfStation = TempGOInfo.StopStationID;

				TempStationNID =  TempGOInfo.StopStationID;
				// Step 2) 이적재 명령이 있으면 넣기


				if (GetCurrentMode() != MODE_MANUAL)
				{
					if(OCSStationData.StationID != 0)
					{
						pSData = OHTMainForm->GetStationDataManager()->Find(OCSStationData.StationID);

						if (pSData == NULL)
						{
							OHTMainForm->GetStationDataManager()->MemoryInsert(&OCSStationData);
							ADD_LOG_TASKCONTROL("[NotMatch VHL<->OCS StationData], StationID:%06d", OCSStationData.StationID);
						}
					}

				}


				if(nError==NO_ERR)
					nError = MakeGoCycleList(&TempGOInfo);

				if((nError ==NO_ERR) && (GetCurrentMode() == MODE_AUTO))
				{
					if(TempGOInfo.StopStationID !=0)
					{
						m_OCSStaionInfo.StationID = TempGOInfo.StopStationID;
						m_OCSStaionInfo.StationType = TempGOInfo.MarkType;
						m_OCSStaionInfo.ParentNodeID = TempGOInfo.StationData_ParentNodeID;
						m_OCSStaionInfo.NextNodeID = TempGOInfo.StationData_NextNodeID;
						m_OCSStaionInfo.Offset = TempGOInfo.StationData_RunDist;
						m_OCSStaionInfo.Safety_Level = TempGOInfo.bIsTargetStationoPassPermitOpen;
					}
					else
					{
						memset(&m_OCSStaionInfo, 0x00, sizeof(NODE_INFO_STD_STATION));
					}
				}

				//오버런 발생시 Data 초기화
				InitExcuteOverRun();
			}
			else
			{
				if(m_pOHTMainStatus->StandardOHTProgress != PROGRESS::Arrived)
				{
					m_pOHTMainStatus->StandardOHTProgress = PROGRESS::Arrived;
					m_pOHTMainStatus->StandardOHTPreProgress = PROGRESS::Arrived;

				}
                SendProgressReq(PROGRESS::Arrived);
			}

			// Step 3)  이동명령 이후 마지막에 추가될 명령을 구분하여 저장 	puting 2015.11.03
			if(nError==NO_ERR)
			{
				TempTransInfo.CmdID = tmpID121.CmdID;
				TempTransInfo.CmdCycle = tmpID121.CmdCycle;

				TempTransInfo.TransCmd.CmdID = tmpID121.CmdID;


				TempTransInfo.TransCmd.BufferType = tmpID121.BufferType.Buffer_1;
				TempTransInfo.AutoRecovery = tmpID121.AutoRecovery;


				//이적재 관련 정보---puting
				//TempTransInfo.WorkCarryType = tmpID121.WorkCarryType;

				//TempTransInfo.TransCmd.PortType = tmpID121.TeachingData_FinalPortType;

				//TempTransInfo.TransCmd.TransOption   = tmpID121.TeachingData_Option;
			   //	TempTransInfo.TransCmd.PIOID = tmpID121.TeachingData_PIOInfo;


				for (int i = 0; i < 3; i++)
				{
					int Temp1 = tmpID121.StopNodeID[i];
					int Tmep6 = tmpID121.StopStationID[i];

					if (HIGH_BYTE == 0)
					{
						TempTransInfo.TransCmd.CurNodeID += Temp1 << ((2 - i) * 8);
						TempTransInfo.TransCmd.CurStationID += 	Tmep6 << ((2 - i) * 8);
					}
					else
					{
						TempTransInfo.TransCmd.CurNodeID += Temp1 << (i * 8);
						TempTransInfo.TransCmd.CurStationID += 	Tmep6 << (i * 8);
                    }

				}

				TempStationNID =  TempTransInfo.TransCmd.CurStationID;


				ADD_LOG_TASKCONTROL("[Trans]PortType:%d hoist:%d, Shift:%d, PIOID:%d, PIODirect:%d,PIOType:%d",
				tmpID121.TeachingData_PortType,tmpID121.TeachingData_Hoist,tmpID121.TeachingData_Shift,tmpID121.TeachingData_PIOID
				,tmpID121.TeachingData_PIODirect,tmpID121.TeachingData_PIOType);


				TempTransInfo.TransCmd.PortType =  tmpID121.TeachingData_PortType; //EQ, STB 등
				TempTransInfo.TransCmd.Tposition_Hoist = tmpID121.TeachingData_Hoist;
				TempTransInfo.TransCmd.Tposition_Rotate = tmpID121.TeachingData_Rotate;
				TempTransInfo.TransCmd.Tposition_Shift = tmpID121.TeachingData_Shift;
				TempTransInfo.TransCmd.PIOID  = tmpID121.TeachingData_PIOID;
				//TempTransInfo.TransCmd.PIOCS  = tmpID121.TeachingData_PIOID;


				if(tmpID121.TeachingData_PIODirect == ON)
					TempTransInfo.TransCmd.PIODirection =LEFT_SIDE;
				else
					TempTransInfo.TransCmd.PIODirection =RIGHT_SIDE;

				TempTransInfo.TransCmd.PIOChannel = tmpID121.TeachingData_PIOChannel;


				TempTransInfo.TransCmd.PIOTimeLevel = tmpID121.TeachingData_PIOTimeLevel;
				TempTransInfo.TransCmd.LookDownLevel = tmpID121.TeachingData_LookDownLevel;
				TempTransInfo.TransCmd.CarrierType = CARRIER_FOUP;

				TempTransInfo.TransCmd.PIOCS = 0;

				TempTransInfo.TransCmd.TransOption.Info.AutoRecovery = tmpID121.TeachingData_AutoRecovery;
				TempTransInfo.TransCmd.TransOption.Info.HandDetectEQ =tmpID121.TeachingData_HandDetectEQ;

				TempTransInfo.TransCmd.ShiftSpeedLevel = 100;
				TempTransInfo.TransCmd.HoistSpeedLevel = 100;


				nError = MakeTransCycleList(&TempTransInfo);

			}
			// 층간이동은 사양 결정 필요
			//현재 cycle 등록
			if(m_pCmdManager->GetCycleCount() ==0)
			{
				ADD_LOG_TASKCONTROL("CheckTaskSatus Init");
				InitClearCycle();
				InitExcuteOverRun();
				ChangeCmdIDAndCycle(0,0,0,true);
			}else
			{
				m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag = false;
				m_pOHTMainStatus->JobChangeFlag.StartStationID = 0;
				m_pOHTMainStatus->JobChangeFlag.dChangDistance = 0.0;
				m_pOHTMainStatus->bCheckCycleRunning = true;
				m_pOHTMainStatus->bCycleing = true;

				InitExcuteOverRun();

				ADD_LOG_TASKCONTROL("CheckTaskSatus %d %d",TempTransInfo.TransCmd.CurStationID,TempTransInfo.TransCmd.CurNodeID);
				if(TempTransInfo.TransCmd.CurStationID !=0)
					ChangeCmdIDAndCycle(tmpID121.CmdCycle,tmpID121.CmdID, TempTransInfo.TransCmd.CurStationID);
				else
					ChangeCmdIDAndCycle(tmpID121.CmdCycle,tmpID121.CmdID, TempTransInfo.TransCmd.CurNodeID);
            }

			break;
	case TAGTYPE_STATIONMOVEINFO: // station 이동 명령
	   //Station 이동명령

		PROTOCOL_BODY_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112 tmpID112;
		memmove(&tmpID112, (PROTOCOL_BODY_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112*)pCmdData->PKTDATA.Value,	sizeof(PROTOCOL_BODY_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112));
		//명령
         ADD_LOG_TASKCONTROL("Station Move Info");
          m_pOHTMainStatus->bRealGoCycleing = true;


		TempGOInfo.CmdID = 9999;
		TempGOInfo.CmdCycle = CYCLE::MovingToStation;

		TempGOInfo.MarkType =  tmpID112.StationData_MarkType;

		TempGOInfo.StationData_ParentNodeID = tmpID112.StationData_ParentNodeID;
		TempGOInfo.StationData_NextNodeID = tmpID112.StationData_NextNodeID;
		TempGOInfo.StationData_RunDist = tmpID112.StationData_Offset;
		TempGOInfo.TeachingData_STBOffset = tmpID112.TeachingData_STBOffset;

		ADD_LOG_TASKCONTROL("[Station]MarkType:%d,ParentNodeID:%d,NextNodeID:%d RunDist:%d",
		tmpID112.StationData_MarkType,tmpID112.StationData_ParentNodeID,tmpID112.StationData_NextNodeID,tmpID112.StationData_Offset);

		//TempGOInfo.FinalPortType = tmpID121.FinalPortType;
		TempGOInfo.PathData_Def_Direction = tmpID112.PathData_Def_Direction.Path_Direction;
		TempGOInfo.PathData_ExtraNum = tmpID112.PathData_ExtraNum;
		for (int i = 0; i < 3; i++)
		{
			// Node 비트연산   puting
			//int Temp1 = tmpID121.StopNodeID[i];
			int Temp2 = tmpID112.StopStationID[i];
//					int Tmep3 = tmpID121.NextNodeID[i];
			int Tmep4 = tmpID112.PathData_StartNodeID[i];
			int Tmep5 = tmpID112.PathData_EndNodeID[i];
//					int Temp6 = tmpID121.StationData_Start_NextNodeID[i];


			if (HIGH_BYTE == 0) {
			   //	TempGOInfo.StopNodeID += Temp1 << ((2 - i) * 8);
				TempGOInfo.StopStationID +=	Temp2 << ((2 - i) * 8);
//						TempGOInfo.NextNodeID += Tmep3<< ((2 - i) * 8);
				TempGOInfo.PathData_StartNodeID += Tmep4 << ((2 - i) * 8);
				TempGOInfo.PathData_EndNodeID += Tmep5 << ((2 - i) * 8);
//						TempGOInfo.StationDataStart_NextNodeID += Temp6 << ((2 - i) * 8);

			}
			else
			{
			  //	TempGOInfo.StopNodeID += Temp1 << (i * 8);
				TempGOInfo.StopStationID +=	Temp2 << (i * 8);
//						TempGOInfo.NextNodeID += Tmep3<< (i * 8);
				TempGOInfo.PathData_StartNodeID += Tmep4 << (i * 8);
				TempGOInfo.PathData_EndNodeID += Tmep5 << (i * 8);
//						TempGOInfo.StationDataStart_NextNodeID += Temp6 << (i * 8);

			}

		}

		TempGOInfo.StopNodeID =  TempGOInfo.PathData_EndNodeID;

		for (int j = 0; j < TempGOInfo.PathData_ExtraNum; j++) {
			for (int i = 0; i < 3; i++) {

				// Extra Node 비트연산   puting
				int Temp = tmpID112.PathData_ExtraID[i + (j * 3)];

				if (HIGH_BYTE == 0)
					TempGOInfo.PathData_ExtraID[j] += Temp << ((2 - i) * 8);
				else
					TempGOInfo.PathData_ExtraID[j] += Temp << (i * 8);

			}
		}

		ADD_LOG_TASKCONTROL("GoInfo:[Extra]Num:%d,First:%d,DefaulDirection:%d",
		TempGOInfo.PathData_ExtraNum,TempGOInfo.PathData_ExtraID[0], TempGOInfo.PathData_Def_Direction);

		TempStationNID =  TempGOInfo.StopStationID;
		// Step 2) 이적재 명령이 있으면 넣기
		nError = MakeGoCycleList(&TempGOInfo);
		if((nError ==NO_ERR) && (GetCurrentMode() == MODE_AUTO))
		{
			if(TempGOInfo.StopStationID !=0)
			{
				m_OCSStaionInfo.StationID = TempGOInfo.StopStationID;
				m_OCSStaionInfo.StationType = TempGOInfo.MarkType;
				m_OCSStaionInfo.ParentNodeID = TempGOInfo.StationData_ParentNodeID;
				m_OCSStaionInfo.NextNodeID = TempGOInfo.StationData_NextNodeID;
				m_OCSStaionInfo.Offset = TempGOInfo.StationData_RunDist;
				m_OCSStaionInfo.Safety_Level = TempGOInfo.bIsTargetStationoPassPermitOpen;
			}
			else
			{
             	memset(&m_OCSStaionInfo, 0x00, sizeof(NODE_INFO_STD_STATION));
			}
		}

        //명령
	   //현재 cycle 등록
		if(m_pCmdManager->GetCycleCount() ==0)
		{
			ADD_LOG_TASKCONTROL("CheckTaskSatus Init");
			InitClearCycle();
			InitExcuteOverRun();
			ChangeCmdIDAndCycle(0,0,0,true);
		}else
		{
			InitExcuteOverRun();
			m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag = false;
			m_pOHTMainStatus->JobChangeFlag.StartStationID = 0;
			m_pOHTMainStatus->JobChangeFlag.dChangDistance = 0.0;
			m_pOHTMainStatus->bCheckCycleRunning = true;
			m_pOHTMainStatus->bCycleing = true;
			ChangeCmdIDAndCycle(CYCLE::MovingToStation, TempGOInfo.CmdID, TempGOInfo.StopStationID);
		}


		break;
	case TAGTYPE_FLOORMOVEMENT: // 작업지시 층간이동
		/*
		 //사양 미협의 된 상태 ---------------------------------------SW 분과측에서 답변 받으면 진행
		 PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_115 tmpID115;
		 memmove(&tmpID115, pCmdData.PKTDATA.Value, sizeof(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_115));
		 */
		/*
		 pTmeCycleData->CycleType = CYCLETYPE_TRANS;
		 pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_FLOORMOVE;

		 m_pCmdManager->AddCycle(pTmeCycleData);


		 IsListPop = true;

		 */
		break;

	case TAGTYPE_MANUAL_COMMON:
		// pTmeCycleData->CycleType = CYCLETYPE_MANUAL;
		// pTmeCycleData->EXE_Maual_Info;
		// m_pCmdManager->AddCycle(pTmeCycleData);
		Header = GetBIGHeader(pCmdData->PKTDATA.Value);
		TempManaulInfo.uCmdTag.Data = Header.uTag.Data;
		memmove(&TempManaulInfo.uTPCmd, &pCmdData->PKTDATA.Value[LEN_HEADER], sizeof(uTP_CMD));

		MakeManualCycleList(&TempManaulInfo);


		break;

	default:
		break;

	}

	// if(!IsListPop)	delete pTmeCycleData;

	return nError;

}
/**
 @brief   주행 관련 Command를 처리하는 함수
 @author  puting
 @date    2015.11.03
 @param   pCmdData : 처리해야 할 Command
 @return  정상인 경우 0을 리턴, 그렇지 않은 경우 에러 코드 리턴
 @todo    Status 상태 체크, 이전 Cmd type 체크, Path Search 하는 파라미터 등 협의 사항 미정
 */

int TaskControl::ChangeGoCycleList(PARSING_GO_INFO *pCmdData, int StartNode, int StationNode)
{
	int Error = NO_ERR;

	bool bChecksameNode = false;
	bool bCheckUTurn = false;
	bool bIgnolOverRun = false;

//	bool bCheckBetweenStation = false;
	int nListCount = 0;


	UINT uTargetNode = pCmdData->PathData_EndNodeID;  //pCmdData->StopNodeID;
	UINT uTargetStation = pCmdData->StopStationID;
   //	UINT uNextNode = pCmdData->NextNodeID;

	UINT uStartNode =  pCmdData->PathData_StartNodeID; // Path Search의 시작 Node
	UINT uStartStion = 0;

//	UINT uStartStationDist = 0; // NextNode까지 가야하는 거리   ------puting 상위로 부터 받음?? 아니면 Map Data 사용 ??
//	UINT uTargetStationDist = pCmdData->StationData_EndRunDist;

   //	UINT uFinalDrivingNodeOfPathSearch = 0;
	/// 현재 VHL의 마지막 목적지 (정차시는 현재 위치)  ??
	UINT uExtraPathNum = 0;

	// 추가 정보를 위한 변수
	LList TmpeList;         ///<임시 리스트
	EXECUTE_DRIVING_ADD_INFO DrivingAdditionalInfo; // 주행 추가 정보
	EXECUTE_DRIVING_ADD_INFO TempSTDrivingAdditionalInfo; // 임시 주행 추가 정보
	EXECUTE_DRIVING_ADD_INFO TempENDDrivingAdditionalInfo; // 임시 주행 추가 정보
	memset(&TempSTDrivingAdditionalInfo, 0x00, sizeof(EXECUTE_DRIVING_ADD_INFO));
	memset(&TempENDDrivingAdditionalInfo, 0x00, sizeof(EXECUTE_DRIVING_ADD_INFO));
	memset(&DrivingAdditionalInfo, 0x00, sizeof(EXECUTE_DRIVING_ADD_INFO));
	// 추가 주행 정보 초기화
	EXECUTE_DRIVING_CMD_TYPE CmdType;

	// 선조향을 위한 변수
	// PATH_STEERING_INFO PreSteeringInfo;
	//EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO = NULL;
	// ExcuteThread에 전달하는 구조체
	STATION_DATA STStationData;
	STATION_DATA ENDStationData;
	memset(&STStationData, 0x00, sizeof(STATION_DATA));
	memset(&ENDStationData, 0x00, sizeof(STATION_DATA));

//	MARK_TYPE StartMarkType;
	MARK_TYPE FinalMarkType;

	PKT_CYCLECMDDATA *CheckCycleList = NULL;

//    bool    bCheckMarked = m_pExecuteInfo->ExecutePositionInfo.Marked ;
	BYTE    OHTDiv = 0x00; //분기정보
	unsigned int  nOffset; //주행노드로부터 이동된 위치값.
	int    NextNode = 0;

	UINT FinalSeqNum = 0;
	double TempDistance = m_pOHTMainStatus->JobChangeFlag.dChangDistance;

	//정지노드 통과허가 반납으로  인해 추가. 20170730
	PASSPERMIT_COMMON TempPassPermitCommon;
	memset(&TempPassPermitCommon, 0x00, sizeof(PASSPERMIT_COMMON));


	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	// 1 1. <<전처리 부분>> 상태 확인 및 초기화
	// Mapmaker 유무에 의해 변경 확인

	//Map에서 정보 획득.

	if(StartNode != 0)
	{
		uStartNode = StartNode;
	}

	if(StationNode !=0)
	 {
		uStartStion = StationNode;
	 }


	if (m_pMapMake_Status->bReady &&
	   (m_pMapMake_Status->EndNodeID != m_pOHTMainStatus->StatusCommon.CurNodeID))
	{
		// Map Make가 수행 도중이면 Cmd Type을 MapMake로 설정
		CmdType = EXECUTE_DRIVING_CMD_MAPMAKE;

	}
	else
	{
		// 그 외 주행 명령은 모두 Go로 설정
		CmdType = EXECUTE_DRIVING_CMD_GO;
	}

//	if((m_pExecuteInfo->StatusCmdId.Info.Status == STS_GOING) &&
//		(uStartStion !=0)) // 주행 명령이 있을 경우
//	{
//		ADD_LOG_TASKCONTROL("Cycle Fail...%d %d",uStartStion, m_pExecuteInfo->StatusCmdId.Info.Status);
//		return ERR_STD_CMD_CYCLE_FAIL;
//	}

	// 2.1 Start/Target Station에 대한 정보 얻기
	///<1 시작에 대한 정보
    DrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
	DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = false;
	DrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
	DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID = pCmdData->PathData_StartNodeID;
	DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =0.0 ;
	ADD_LOG_TASKCONTROL(" Start Station No");

    // 2.1 Start/Target Station에 대한 정보 얻기
	///<1 시작에 대한 정보
	if(uStartStion !=0) //현재 미사용
	{

		Error = GetStationData(&STStationData,uStartStion);

		if(Error == NO_ERR)
		{
		   //	StartMarkType =  STStationData.StationType;


			TempSTDrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
			TempSTDrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = true;
			TempSTDrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
			TempSTDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID =pCmdData->PathData_StartNodeID;
			TempSTDrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =STStationData.StationOffset ;
			TempSTDrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;
			TempSTDrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->PathData_EndNodeID;
			TempSTDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = STStationData.NextNodeID;
			TempSTDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = 0.0;

			DrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
			DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = true;
			DrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
			DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID =STStationData.NextNodeID;
			DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =STStationData.StationOffset ;
			ADD_LOG_TASKCONTROL(" Start Station Step_1");
		}else
			return ERR_STD_CMD_ST_PATH_FAIL;
	}
	else
	{
        DrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
		DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = false;
		DrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
		DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID = pCmdData->PathData_StartNodeID;
		DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =0.0 ;
			ADD_LOG_TASKCONTROL(" Start Station No");
	}
 	///< 도착에 대한 정보
	if (uTargetStation != 0)
	 //	(uNextNode != 0))
	{


		if(Error == NO_ERR)
		{

            if(GetCurrentMode() == MODE_AUTO)
			{

				FinalMarkType =  pCmdData->MarkType;

				TempENDDrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = false;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_EndNodeID;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID = pCmdData->PathData_EndNodeID;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =0.0 ;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->StationData_NextNodeID;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = pCmdData->StationData_NextNodeID;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = true;

				DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = true;
				DrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->PathData_EndNodeID;
				DrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = pCmdData->StationData_NextNodeID;

				TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = pCmdData->StationData_RunDist;
				DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = pCmdData->StationData_RunDist;
				DrivingAdditionalInfo.StationSafetyLevel = pCmdData->bIsTargetStationoPassPermitOpen;


              
			}
			else
			{
				Error = GetStationData(&ENDStationData,uTargetStation);

				if(Error == NO_ERR)
				{
					FinalMarkType =  ENDStationData.StationType;

					TempENDDrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = false;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_EndNodeID;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID = pCmdData->PathData_EndNodeID;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =0.0 ;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = ENDStationData.NextNodeID;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = ENDStationData.NextNodeID;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = true;

					DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = true;
					DrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->PathData_EndNodeID;
					DrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = ENDStationData.NextNodeID;

					TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = ENDStationData.StationOffset;
					DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = ENDStationData.StationOffset;

					pCmdData->bIsTargetStationoPassPermitOpen =ENDStationData.Safety_Level;
					DrivingAdditionalInfo.StationSafetyLevel = ENDStationData.Safety_Level;
				}
			}

			ADD_LOG_TASKCONTROL(" End Station Step1");
		}
		else
			return ERR_STD_CMD_ED_PATH_FAIL;

	}
	else
	{

		DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;
		DrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->PathData_EndNodeID;
		DrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = pCmdData->PathData_EndNodeID;
		DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = 0.0;
		ADD_LOG_TASKCONTROL(" End Station No");
	}

	//이부분 버그있음.  //동일한 노드로 정보보내면 한바퀴 돌아서 Pathseaerh하도록 변경 완료.
    // 한번만 패스서치해야되는 경우만 넣어줄것.
	if((pCmdData->PathData_StartNodeID == pCmdData->PathData_EndNodeID) ||
	  ((TempSTDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID == pCmdData->PathData_EndNodeID) && (DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd ==false)))
	{
		bChecksameNode = true;
	}


	if(pCmdData->PathData_StartNodeID == pCmdData->PathData_EndNodeID)
	{
		if((((int)DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist - (int)DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist) < 0))   //사용
			  {
				bCheckUTurn = false;
				bChecksameNode = false;

			 }
	}

	if(bChecksameNode ==true)
			DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID =pCmdData->PathData_StartNodeID;



	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1 2. << Cycle List 처리 부분>>

	// Step 1. Start Node(NextNode 포함)와 Target Node(Next Node 포함)가 동일하지 않다는 가정하에 탐색.
	// 탐색은 Next Node에서부터 Next Node 탐색함.

	// Pass Search로 리스트를 보내 Cycle List를 생성
	ADD_LOG_TASKCONTROL("MakeGoCycleList StopNodeID:%06d, StartNodeID:%06d, EndNodeID:%06d,StationID:%06d,CmdID:%d, Cycle%X,bChecksameNode%d,bCheckUTurn%d %d %d"
	,pCmdData->StopNodeID,pCmdData->PathData_StartNodeID, pCmdData->PathData_EndNodeID,pCmdData->StopStationID, pCmdData->CmdID,pCmdData->CmdCycle,bChecksameNode,bCheckUTurn,DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart,DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd );


	if ((Error == NO_ERR) && (bChecksameNode == false))
	{


//			ADD_LOG_TASKCONTROL("MakeGoCycleList StopNodeID:%06d, StartNodeID:%06d, EndNodeID:%06d,StationID:%06d,NextID:%06d,CmdID:%d, Cycle%X"
//			,pCmdData->StopNodeID,pCmdData->PathData_StartNodeID, pCmdData->PathData_EndNodeID,pCmdData->StopStationID,pCmdData->NextNodeID, pCmdData->CmdID,pCmdData->CmdCycle);

		DrivingAdditionalInfo.CmdType = CmdType; // 명령 타입
		DrivingAdditionalInfo.FinalNode = uTargetNode; // 현재 받은 타겟 노드: 상황에 따라 업데이트 됨





		//pCmdData->MarkType;
		if(CmdType == EXECUTE_DRIVING_CMD_MAPMAKE)
		{
			DrivingAdditionalInfo.SpeedLimitOfMapMake = m_pMapMake_Status->Speed_MapMake;
		}
		//이적재가 Target인 경우 정보 추가.
		if (uTargetStation != 0)
		{
				 if(bCheckUTurn == false)
				{
					DrivingAdditionalInfo.StationID = uTargetStation;
					DrivingAdditionalInfo.StationType =FinalMarkType;// pCmdData->MarkType; // 2. 이적재 노드 타입  --주행도 포함.
					DrivingAdditionalInfo.FinalNode = TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID;//ENDStationData.NextNodeID;//pCmdData->NextNodeID; // 이적재
					DrivingAdditionalInfo.DistToStation = DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist ; // 3. 이적재 노드까지 거리
					DrivingAdditionalInfo.STBOffset = pCmdData->TeachingData_STBOffset;

					 if(DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist < 25.0)
					 {
					 	pCmdData->bIsTargetStationoPassPermitOpen =false;
						DrivingAdditionalInfo.StationSafetyLevel = 0x00;

						bIgnolOverRun = true;
                     }
				}else
				{
					TempENDDrivingAdditionalInfo.StationID = uTargetStation;
					TempENDDrivingAdditionalInfo.StationType = FinalMarkType;//pCmdData->MarkType; // 2. 이적재 노드 타입  --주행도 포함.
					TempENDDrivingAdditionalInfo.FinalNode = TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID;//ENDStationData.NextNodeID;//pCmdData->NextNodeID; // 이적재
				//	 DrivingAdditionalInfo.FinalPortType = pCmdData->GoCmd.FinalPortType;
					TempENDDrivingAdditionalInfo.DistToStation = TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist ; // 3. 이적재 노드까지 거리
//						DrivingAdditionalInfo.TPosition_Rotate =  pCmdData->GoCmd.TPosition_Rotate;   ///< Rotate Teaching값(degree)
					TempENDDrivingAdditionalInfo.STBOffset = pCmdData->TeachingData_STBOffset;

					if(TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist < 25.0)
					{
						pCmdData->bIsTargetStationoPassPermitOpen =false;
						DrivingAdditionalInfo.StationSafetyLevel = 0x00;
						bIgnolOverRun = true;
                    }
				}
		}



		if (!m_pOHTMainStatus->DataUseFlag.fSwapping)
		{
			m_pOHTMainStatus->DataUseFlag.fSwapping = true;

			//이적재에서 출바할때 부분에 대한 처리.
			if(DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart == true)
			{
				if(m_pPassPermit->GetPassPermitCount() != LISTCOUNT_EMPTY)
				{
					DATA_PASS * pCurPPCommon; // Command를 위한 포인터 변수
					pCurPPCommon = (DATA_PASS*)m_pPassPermit->ReferFirstPassPermit();
					FinalSeqNum = pCurPPCommon->PassInfo.SQNum;

					memmove(&TempPassPermitCommon, pCurPPCommon, sizeof(PASSPERMIT_COMMON));

				}
				Error = m_pPathSearch->StandardSearchOnlyOnePassPermit(OHTMainForm->GetMap(),OHTMainForm->GetFlowControl_STD(), pCmdData, TempSTDrivingAdditionalInfo, &TmpeList, false, CheckCycleList,uExtraPathNum,TempDistance,FinalSeqNum,TempPassPermitCommon,m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1); // 명령이 담기는 구조체

				ADD_LOG_TASKCONTROL("[MakeCmd][Go_StationStart][Error:%d]StartNID:%06d, StopNID:%06d %d %d %f %d",
									Error, uStartNode, uTargetNode,
									TempSTDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID,
									FinalSeqNum,
									TempDistance,
									TempSTDrivingAdditionalInfo.ProcessingGoInfo.PathDiv);

//					if(DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID  ==0)
//						DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID  = TempEndID;

				DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = false;
					// 일반 주행할 떄에 대한 부분.(Target이 이적재일경우도 포함됨.

				if(Error == NO_ERR)
					Error = m_pPathSearch->StandardSearchPassPermit(OHTMainForm->GetMap(),OHTMainForm->GetFlowControl_STD(), pCmdData, DrivingAdditionalInfo, &TmpeList, uExtraPathNum,m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1,TempDistance,bIgnolOverRun); // 명령이 담기는 구조체

			}
			else
			{

				// 일반 주행할 떄에 대한 부분.(Target이 이적재일경우도 포함됨.
				if(Error == NO_ERR)
					Error = m_pPathSearch->StandardSearchPassPermit(OHTMainForm->GetMap(), OHTMainForm->GetFlowControl_STD(),pCmdData, DrivingAdditionalInfo, &TmpeList, uExtraPathNum,m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1,TempDistance,bIgnolOverRun); // 명령이 담기는 구조체

			}

			m_pOHTMainStatus->DataUseFlag.fSwapping = false;
			// by zzang9un 2013.11.11 : 로그 추가
			if(Error)
			{
				ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node][Error:%d]StartNID:%06d, StopNID:%06d %f",
									Error, uStartNode, uTargetNode,TempDistance);
			}
			else
			{
				// List 정리부분
				nListCount = TmpeList.getCount();

				if ((Error == NO_ERR) && (nListCount != 0))
				{

					for (int Index = 0; Index < nListCount; Index++)

					{      //  Test용    puting
							PKT_CYCLECMDDATA *TempCycleList;

							TempCycleList = (PKT_CYCLECMDDATA*) TmpeList.popFront();


							ADD_LOG_TASKCONTROL(" MakeGoCycleList:Cnt%d SQ:%d [Start]Type%x,ND:%d,Dis:%f[End]Nd:%d,Dis%f[Stop]%f[Reset]%f, %d %d %d %d",
							nListCount,
							TempCycleList->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum,
							TempCycleList->EXE_CycleDriving_Info.StartNodeType,
							TempCycleList->EXE_CycleDriving_Info.StartPoint,
							TempCycleList->EXE_CycleDriving_Info.StartDistance,
							TempCycleList->EXE_CycleDriving_Info.EndPoint,
							TempCycleList->EXE_CycleDriving_Info.EndDistance,
							TempCycleList->EXE_CycleDriving_Info.StopDistance,
							TempCycleList->EXE_CycleDriving_Info.ResetNodeDistance,
							TempCycleList->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint,
							TempCycleList->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint,
							TempCycleList->EXE_CycleDriving_Info.D_PPStep,
							TempCycleList->EXE_CycleDriving_Info.D_PPOStep);

							m_pCmdManager->AddCycle(TempCycleList);
						   //

						   //	m_pCmdManager->AddCycle((PKT_CYCLECMDDATA*) TmpeList.popFront());
					}

				}
				else
				{
					Error == ERR_PATH_FORMAT;
					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
								ERR_PATH_FORMAT,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pExecuteInfo->ExecutePositionInfo.NextNode,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_pOHTMainStatus->StatusCommon.ExistFoup,
								m_pOHTMainStatus->StatusCommon.StopStationID);
				}


				if(DrivingAdditionalInfo.CmdType == EXECUTE_DRIVING_CMD_MAPMAKE)
				{
					ADD_LOG_TASKCONTROL("[MakeCmd][MapMake][EndNID:%06d] StartNID:%06d, StopNID:%06d, Speed:%d",
									m_pMapMake_Status->EndNodeID, uStartNode, uTargetNode,
									DrivingAdditionalInfo.SpeedLimitOfMapMake);
				}
				else
				{
					ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node] Error: %d StartNID:%06d, StopNID:%06d",
									Error,uStartNode, uTargetNode);
				}
			}

		}
		else
		{
			Error = ERR_PATHSEARCH_FAIL;
			ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		}

			ADD_LOG_TASKCONTROL("MakeGoCycleList uStartNode:%06d, uTargetNode:%06d, bChecksameNode:%d, Error%d"
				,uStartNode,uTargetNode, bChecksameNode,Error);



	}
	else if ((Error == NO_ERR) && (bChecksameNode == true))
	{
		 //이적재에서 출발할 경우 -> 이적재로 이동

					//이적재가 Target인 경우 정보 추가.
				DrivingAdditionalInfo.CmdType = CmdType; // 명령 타입
				DrivingAdditionalInfo.FinalNode = uTargetNode; // 현재 받은 타겟 노드: 상황에 따라 업데이트 됨
			  //DrivingAdditionalInfo.StationType = pCmdData->Station; // 2. 이적재 노드 타입  --주행도 포함.

				//이적재가 Target인 경우 정보 추가.
				if (uTargetStation != 0)
				{
						DrivingAdditionalInfo.StationID = uTargetStation;
						DrivingAdditionalInfo.StationType = FinalMarkType;//pCmdData->MarkType; // 2. 이적재 노드 타입  --주행도 포함.
						DrivingAdditionalInfo.FinalNode = TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID;//ENDStationData.NextNodeID; // 이적재
					 // DrivingAdditionalInfo.FinalPortType = pCmdData->GoCmd.FinalPortType;
						DrivingAdditionalInfo.DistToStation = DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist ; // 3. 이적재 노드까지 거리
		//					DrivingAdditionalInfo.TPosition_Rotate =  pCmdData->GoCmd.TPosition_Rotate;   ///< Rotate Teaching값(degree)
						DrivingAdditionalInfo.STBOffset = pCmdData->TeachingData_STBOffset;
						
						//AOHC-195 오버런 개선 작업(유형2)
						if(DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist < 25.0)
						{
							pCmdData->bIsTargetStationoPassPermitOpen =false;
							DrivingAdditionalInfo.StationSafetyLevel = 0x00;

							bIgnolOverRun = true;
						}
				}


				//정지노드의 통과허가 처리로 인해 추가됨.
				if(DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart == true)
				{
					if(m_pPassPermit->GetPassPermitCount() != LISTCOUNT_EMPTY)
					{
//						//예외처리 추가.
//						if(m_pPassPermit->GetPassPermitCount() != 1)
//							Error = ERR_STD_ETC_PASSPERMIT_FAIL;

						DATA_PASS * pCurPPCommon; // Command를 위한 포인터 변수
						pCurPPCommon = (DATA_PASS*)m_pPassPermit->ReferBackPassPermit();//ReferFirstPassPermit();

						memmove(&TempPassPermitCommon, pCurPPCommon, sizeof(PASSPERMIT_COMMON));

					}
				}


				if (!m_pOHTMainStatus->DataUseFlag.fSwapping)
				{
					m_pOHTMainStatus->DataUseFlag.fSwapping = true;

					FinalSeqNum = m_pPathSearch->GetIncreaseSQNumber();
					ADD_LOG_TASKCONTROL("Error %d",Error);


					//해당 정보를 넣어주기. 마지막 위치값 넣어주기.
					Error = m_pPathSearch->StandardSearchOnlyOnePassPermit(OHTMainForm->GetMap(),
																			OHTMainForm->GetFlowControl_STD(),
																			pCmdData,
																			DrivingAdditionalInfo,
																			&TmpeList, false,
																			CheckCycleList,
																			uExtraPathNum,
																			TempDistance,
																			FinalSeqNum,
																			TempPassPermitCommon,
																			m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1,bIgnolOverRun); // 명령이 담기는 구조체 //AOHC-195 오버런 개선 작업(유형2)



					m_pOHTMainStatus->DataUseFlag.fSwapping = false;
					// by zzang9un 2013.11.11 : 로그 추가
					if(Error)
					{
						ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node][Error:%d]StartNID:%06d, StopNID:%06d",
											Error, uStartNode, uTargetNode);
					}
					else
					{
						// List 정리부분
						nListCount = TmpeList.getCount();

						ADD_LOG_TASKCONTROL("MakeGoCycleList nListCount:%d",nListCount);

						if ((Error == NO_ERR) && (nListCount == 1))
						{

							PKT_CYCLECMDDATA *TempCycleList;
							TempCycleList = (PKT_CYCLECMDDATA*) TmpeList.popFront();
							m_pCmdManager->AddCycle(TempCycleList);

						}
						else
						{
							Error == ERR_PATH_FORMAT;
							ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
										ERR_PATH_FORMAT,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID);
						}


							ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node] StartNID:%06d, StopNID:%06d",
											uStartNode, uTargetNode);

					}

				}
				else
					Error = ERR_STD_CMD_MD_PATH_FAIL;




	}



	return Error;

}

/**
 @brief   주행 관련 Command를 처리하는 함수
 @author  puting
 @date    2015.11.03
 @param   pCmdData : 처리해야 할 Command
 @return  정상인 경우 0을 리턴, 그렇지 않은 경우 에러 코드 리턴
 @todo    Status 상태 체크, 이전 Cmd type 체크, Path Search 하는 파라미터 등 협의 사항 미정
 */

int TaskControl::MakeGoCycleList(PARSING_GO_INFO *pCmdData, int StartNode, int StationNode, int noffset)
{
	int Error = NO_ERR;

	bool bChecksameNode = false;
	bool bCheckUTurn = false;   //제자리 돌기 기능
	bool bIgnolOverRun = false;

//	bool bCheckBetweenStation = false;
	int nListCount = 0;

	UINT uTargetNode = pCmdData->PathData_EndNodeID;  //pCmdData->StopNodeID;
	UINT uTargetStation = pCmdData->StopStationID;
//	UINT uNextNode = pCmdData->NextNodeID;

	UINT uStartNode =  m_pExecuteInfo->ExecutePositionInfo.CurrNode; // Path Search의 시작 Node
	UINT uStartStion = m_pExecuteInfo->ExecutePositionInfo.CurrStation;

//	UINT uStartStationDist = 0; // NextNode까지 가야하는 거리   ------ puting 상위로 부터 받음?? 아니면 Map Data 사용 ??
//	UINT uTargetStationDist = pCmdData->StationData_EndRunDist;

   //	UINT uFinalDrivingNodeOfPathSearch = 0;
	/// 현재 VHL의 마지막 목적지 (정차시는 현재 위치)  ??
	UINT uExtraPathNum = 0;

	// 추가 정보를 위한 변수
	LList TmpeList;         ///<임시 리스트
	EXECUTE_DRIVING_ADD_INFO DrivingAdditionalInfo; // 주행 추가 정보
	EXECUTE_DRIVING_ADD_INFO TempSTDrivingAdditionalInfo; // 임시 주행 추가 정보
	EXECUTE_DRIVING_ADD_INFO TempENDDrivingAdditionalInfo; // 임시 주행 추가 정보
	memset(&TempSTDrivingAdditionalInfo, 0x00, sizeof(EXECUTE_DRIVING_ADD_INFO));
	memset(&TempENDDrivingAdditionalInfo, 0x00, sizeof(EXECUTE_DRIVING_ADD_INFO));
	memset(&DrivingAdditionalInfo, 0x00, sizeof(EXECUTE_DRIVING_ADD_INFO));
	// 추가 주행 정보 초기화
	EXECUTE_DRIVING_CMD_TYPE CmdType;

	// 선조향을 위한 변수
	// PATH_STEERING_INFO PreSteeringInfo;
	//EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO = NULL;
	// ExcuteThread에 전달하는 구조체
	STATION_DATA STStationData;
	STATION_DATA ENDStationData;
	memset(&STStationData, 0x00, sizeof(STATION_DATA));
	memset(&ENDStationData, 0x00, sizeof(STATION_DATA));

	MARK_TYPE StartMarkType;
	MARK_TYPE FinalMarkType;

	PKT_CYCLECMDDATA *CheckCycleList = NULL;

    bool    bCheckMarked = m_pExecuteInfo->ExecutePositionInfo.Marked ;
	BYTE    OHTDiv = 0x00; //분기정보
	int  nOffset; //주행노드로부터 이동된 위치값.
	int    NextNode = 0;

	UINT FinalSeqNum = 0;
	double TempDistance =0.0;

	//정지노드 통과허가 반납으로  인해 추가. 20170730
	PASSPERMIT_COMMON TempPassPermitCommon;
	memset(&TempPassPermitCommon, 0x00, sizeof(PASSPERMIT_COMMON));


	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	// 1 1. <<전처리 부분>> 상태 확인 및 초기화
	// Mapmaker 유무에 의해 변경 확인

	//Map에서 정보 획득.

	if(StartNode != 0)
	{
		uStartNode = StartNode;
		uStartStion = StationNode;

		ADD_LOG_TASKCONTROL("Cycle..%d %d",uStartNode,uStartStion);
    }

	if((pCmdData->CmdCycle ==CYCLE::TP_RunningLearning) &&
		(pCmdData->PathData_EndNodeID !=m_pOHTMainStatus->StatusCommon.CurNodeID))
//	if (m_pMapMake_Status->bReady &&
//	   (m_pMapMake_Status->EndNodeID != m_pOHTMainStatus->StatusCommon.CurNodeID))
	{




		m_pMapMake_Status->bReady  = true;
		m_pMapMake_Status->bStart = false;
		m_pMapMake_Status->bComplete = false;

		m_pMapMake_Status->StartNodeID = m_pOHTMainStatus->StatusCommon.CurNodeID;
		m_pMapMake_Status->EndNodeID = pCmdData->PathData_EndNodeID;

		m_pMapMake_Status->Speed_MapMake = MIN_SPEED_MAPMAKE; // 200mm/s (0.2m/s)

		// by zzang9un 2014.02.25 : Data Backup을 위한 flag 설정
		m_pMapMake_Status->bStartBackup_Map = true;
		m_pMapMake_Status->bStartBackup_Station = true;

		if(m_pExecuteInfo->ListOfMapInfo.getCount() > 0)
		{

			for(int i=0; i< m_pExecuteInfo->ListOfMapInfo.getCount(); i++)
			{
				EXECUTE_RESPONSE_MAPMAKING_INFO *pData = NULL;
				pData = (EXECUTE_RESPONSE_MAPMAKING_INFO*)m_pExecuteInfo->ListOfMapInfo.popFront();

				if (pData)
				{
					// by zzang9un 2013.09.09 : 로그 생성을 주행, Station에 따라 구분
					if ((pData->StationID == 0) &&
						(pData->StationType == MARK_TYPE_NODE)) {
						ADD_LOG_MAPMAKE(
							"[Ready][Driving]Start:%06d, End:%06d, Dist:%d, NID:%06d, PointX:%d, PointY:%d",
							pData->NodeOffsetInfo[0].StartNodeID,
							pData->NodeOffsetInfo[0].EndNodeID,
							pData->NodeOffsetInfo[0].Dist, pData->NodeDataInfo.NID,
							pData->NodeDataInfo.Point_X,
							pData->NodeDataInfo.Point_Y);

					}
					else {
						ADD_LOG_MAPMAKE(
							"[Ready][Station]StID:%06d, SType:%X, ParentNID:%06d, NextNID:%06d, Dist:%d",
							pData->StationID, pData->StationType,
							pData->NodeOffsetInfo[0].StartNodeID,
							pData->NodeOffsetInfo[0].EndNodeID,
							pData->NodeOffsetInfo[0].Dist);

					}

				   delete pData;
				}
			}
		}
		ADD_LOG_MAPMAKE("[Start][Driving][%06d~%06d] Speed:%d",
                            m_pMapMake_Status->StartNodeID,
                            m_pMapMake_Status->EndNodeID,
                            m_pMapMake_Status->Speed_MapMake);

		AnsiString strMapLog;
		strMapLog.sprintf("[Start][Driving][%06d~%06d] Speed:%d",
                            m_pMapMake_Status->StartNodeID,
							m_pMapMake_Status->EndNodeID,
                            m_pMapMake_Status->Speed_MapMake);
		Send_MapMakeResult(strMapLog);
		

		// Map Make가 수행 도중이면 Cmd Type을 MapMake로 설정
		CmdType = EXECUTE_DRIVING_CMD_MAPMAKE;

		DrivingAdditionalInfo.CmdType =  EXECUTE_DRIVING_CMD_MAPMAKE;
		DrivingAdditionalInfo.MapMakeType.MapMake_DrivingTag =  ON;
		DrivingAdditionalInfo.MapMakeType.MapMake_StopTag =  ON;
		DrivingAdditionalInfo.MapMakeType.MapMake_QR_Left_Tag =  ON;
		DrivingAdditionalInfo.MapMakeType.MapMake_QR_Right_Tag =  ON;

		TempSTDrivingAdditionalInfo.CmdType =  EXECUTE_DRIVING_CMD_MAPMAKE;
		TempSTDrivingAdditionalInfo.MapMakeType.MapMake_DrivingTag =  ON;
		TempSTDrivingAdditionalInfo.MapMakeType.MapMake_StopTag =  ON;
		TempSTDrivingAdditionalInfo.MapMakeType.MapMake_QR_Left_Tag =  ON;
		TempSTDrivingAdditionalInfo.MapMakeType.MapMake_QR_Right_Tag =  ON;

		TempENDDrivingAdditionalInfo.CmdType =  EXECUTE_DRIVING_CMD_MAPMAKE;
		TempENDDrivingAdditionalInfo.MapMakeType.MapMake_DrivingTag =  ON;
		TempENDDrivingAdditionalInfo.MapMakeType.MapMake_StopTag =  ON;
		TempENDDrivingAdditionalInfo.MapMakeType.MapMake_QR_Left_Tag =  ON;
		TempENDDrivingAdditionalInfo.MapMakeType.MapMake_QR_Right_Tag =  ON;

		/*
		CmdType = EXECUTE_DRIVING_CMD_MAPMAKE_TRANS;
		CmdType = EXECUTE_DRIVING_CMD_MAPMAKE_QR_L;
        CmdType = EXECUTE_DRIVING_CMD_MAPMAKE_QR_R;
		*/


	}
	else
	{
		// 그 외 주행 명령은 모두 Go로 설정
		CmdType = EXECUTE_DRIVING_CMD_GO;
	}
// Start Node(NextNode 포함)와 Target Node(Next Node 포함)가 동일 한지 확인.
// Currstion 정보가 날라가는 시점
// 에러 발생 or  marked가 false에서 Reset 명령시 날라감.  예외처리를 어떻게 할지 확인 필요...
// 현재  CurrStation은 마크디드 상태인지 같이 Check

	if((m_pOHTMainStatus->IsOnGoCmd == true) || // OnGoCmd 거나
		(m_pExecuteInfo->StatusCmdId.Info.Status == STS_GOING) ||// 주행 중이거나
		(m_pExecuteInfo->ExecuteUnit == EXECUTE_UNIT_DRIVING && m_pExecuteInfo->ListOfCmd.getCount() > 0)) // 주행 명령이 있을 경우
	{
		ADD_LOG_TASKCONTROL("Cycle Fail...%d %d %d",m_pExecuteInfo->StatusCmdId.Info.Status,m_pOHTMainStatus->IsOnGoCmd,StationNode);
		if(StationNode ==0)
		{
			return ERR_STD_CMD_CYCLE_FAIL;
        }
	}
	//동일한 부모노드로 이동할 경우, 주행 노드->이적재, 이적재->이적재, 이적재 -> 주행 노드
   	 // 마크 상태 확인
	if((bCheckMarked==false) && (StationNode ==0))
	{
		//기동 Off 상태이므로 현재 위치 파악할 것.
		ADD_LOG_TASKCONTROL("Trun off No Marked %d", m_pExecuteInfo->nCheckDivInfo);

		if(m_pExecuteInfo->nCheckDivInfo == PATH_DIVERGENCE_LEFT)
			OHTDiv =0x01;
		else if(m_pExecuteInfo->nCheckDivInfo == PATH_DIVERGENCE_RIGHT)
			OHTDiv =0x02;
		else
			OHTDiv =0x03; //직진


		nOffset = m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset; //주행노드로부터 이동된 위치값.
		if(nOffset <0)
		   nOffset = 0; //Node Margin

		if(m_pExecuteInfo->FlagOverRunInfo.bCheck ==false)
		{
			//기본 분기 방향 정보 확인
			if((nOffset <20) &&(OHTDiv == 0x03))
			{
				if(pCmdData->PathData_Def_Direction	==DefaultDirection::DIR_LEFT)
					OHTDiv =0x01;
				else if(pCmdData->PathData_Def_Direction == DefaultDirection::DIR_RIGHT)
					OHTDiv =0x02;
			}   
						
			NextNode = m_pPathSearch->GetNextNode(OHTMainForm->GetMap(), pCmdData->PathData_StartNodeID, OHTDiv);

        }
		else
		{
			m_pExecuteInfo->FlagOverRunInfo.bCheck = false;

			if(m_pExecuteInfo->FlagOverRunInfo.OverrunType == 0x01) //주행
			{
				ADD_LOG_TASKCONTROL("Overrun_Driving");
				uStartNode = m_pExecuteInfo->FlagOverRunInfo.nFinalNode;
				NextNode = m_pPathSearch->GetNextNode(OHTMainForm->GetMap(), m_pExecuteInfo->FlagOverRunInfo.nFinalNode, OHTDiv);
				nOffset = m_pExecuteInfo->FlagOverRunInfo.dDrivingInitPosition;
				STStationData.ParentNodeID =uStartNode;
				STStationData.StationOffset =nOffset;
				STStationData.StationType = MARK_TYPE_NODE;
				MakeSetStationOffset(&STStationData);
				memset(&STStationData, 0x00, sizeof(STATION_DATA));
 				bCheckMarked = true; //false;


			}else if(m_pExecuteInfo->FlagOverRunInfo.OverrunType == 0x02) //이적재
			{
				Error = GetStationData(&STStationData,m_pExecuteInfo->FlagOverRunInfo.nFinalNode);
				ADD_LOG_TASKCONTROL("Overrun_Station %d %d",m_OCSStaionInfo.StationID,m_pExecuteInfo->FlagOverRunInfo.nFinalNode);
				if(Error == NO_ERR)
				{
					if(GetCurrentMode() == MODE_AUTO)
					{
						if(m_pExecuteInfo->FlagOverRunInfo.nFinalNode == m_OCSStaionInfo.StationID)
						{
							uStartNode = m_OCSStaionInfo.ParentNodeID;
							NextNode = m_OCSStaionInfo.NextNodeID;
							nOffset = m_OCSStaionInfo.Offset + m_pExecuteInfo->FlagOverRunInfo.dDrivingInitPosition;
						}
						else
						  return ERR_STD_CMD_PATH_FAIL;
					}
					else
					{
						uStartNode = STStationData.ParentNodeID;
						NextNode = STStationData.NextNodeID;
						nOffset = STStationData.StationOffset + m_pExecuteInfo->FlagOverRunInfo.dDrivingInitPosition;
					}
				}else
					return ERR_STD_CMD_PATH_FAIL;

			}
		}

//		m_pExecuteInfo->FlagOverRunInfo.bCheck = false;
		ADD_LOG_TASKCONTROL("No Marked Go _ %d %d",OHTDiv,m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset)

	}

	 if((uStartNode ==0) ||
	   (uStartNode != pCmdData->PathData_StartNodeID))
	   {
			ADD_LOG_TASKCONTROL(" ERR_PATHSEARCH_FAIL Fail %d %d",uStartNode,pCmdData->PathData_StartNodeID);
			return ERR_STD_CMD_PATH_FAIL;
	   }


	// 2.1 Start/Target Station에 대한 정보 얻기
	///<1 시작에 대한 정보
	if(((uStartStion !=0) && (bCheckMarked==true)) ||
	   ((pCmdData->CmdCycle ==CYCLE::TakeOut) && (StationNode !=0)))
	{
		if((GetCurrentMode() == MODE_AUTO) && (m_pExecuteInfo->FlagMTLInfo.bTakeInCycle == false))
		{
			if(uStartStion == m_OCSStaionInfo.StationID)
			{
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = true;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID =pCmdData->PathData_StartNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =m_OCSStaionInfo.Offset ;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->PathData_EndNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = m_OCSStaionInfo.NextNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = 0.0;

				DrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
				DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = true;
				DrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
				DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID =m_OCSStaionInfo.NextNodeID;
				DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =m_OCSStaionInfo.Offset ;
				ADD_LOG_TASKCONTROL(" Start Station Step_1_");
			}
			else
			{
				ADD_LOG_TASKCONTROL(" Start Station Step_1_1 %d %d",uStartStion,m_OCSStaionInfo.StationID);
			  	return ERR_STD_CMD_ST_PATH_FAIL;
			}
		}
		else
		{
           	Error = GetStationData(&STStationData,uStartStion);

			if(Error == NO_ERR)
			{
			   //	StartMarkType =  STStationData.StationType;

				TempSTDrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = true;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID =pCmdData->PathData_StartNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =STStationData.StationOffset ;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->PathData_EndNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = STStationData.NextNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = 0.0;

				DrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
				DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = true;
				DrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
				DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID =STStationData.NextNodeID;
				DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =STStationData.StationOffset ;
				ADD_LOG_TASKCONTROL(" Start Station Step_1");
			}else
				return ERR_STD_CMD_ST_PATH_FAIL;
        }

	}
	else if(bCheckMarked==false)
	{

			if(NextNode == -1)
			{
				ADD_LOG_TASKCONTROL("NoMark Search Error");
				return ERR_STD_CMD_ST_PATH_FAIL;
            }

			if(m_pExecuteInfo->FlagOverRunInfo.OverrunType == 0x01)
			{
                 DrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
				DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = false;
				DrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
				DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID = pCmdData->PathData_StartNodeID;
				DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =0.0 ;
				ADD_LOG_TASKCONTROL(" Start Station No");
			}
			else
			{
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = true;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID =pCmdData->PathData_StartNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist = nOffset;//STStationData.StationOffset ;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID =pCmdData->PathData_EndNodeID;//STStationData.NextNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = NextNode;//STStationData.NextNodeID;
				TempSTDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = 0.0;

				DrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
				DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = true;
				DrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
				DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID = NextNode;//STStationData.NextNodeID; //PathSerch할때 찾아서 넣어줄것 .puting 20160505
				DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =nOffset;//STStationData.StationOffset ;
				ADD_LOG_TASKCONTROL(" Start Station Step_2");
			}
	}
	else
	{
        DrivingAdditionalInfo.ProcessingGoInfo.PathDiv = OHTDiv;
		DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = false;
		DrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
		DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID = pCmdData->PathData_StartNodeID;
		DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =0.0 ;
		ADD_LOG_TASKCONTROL(" Start Station No");
	}
 	///< 도착에 대한 정보
	if (uTargetStation != 0)
	 //	(uNextNode != 0))
	{

		if(Error == NO_ERR)
		{
			if((GetCurrentMode() == MODE_AUTO)&&(StationNode ==0))
			{

				FinalMarkType =  pCmdData->MarkType;

				TempENDDrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = false;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_EndNodeID;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID = pCmdData->PathData_EndNodeID;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =0.0 ;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->StationData_NextNodeID;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = pCmdData->StationData_NextNodeID;
				TempENDDrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = true;

				DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = true;
				DrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->PathData_EndNodeID;
				DrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = pCmdData->StationData_NextNodeID;

				DrivingAdditionalInfo.StationSafetyLevel = pCmdData->bIsTargetStationoPassPermitOpen;

				//추후에 상위와 협의후 변경 필요.

				TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = pCmdData->StationData_RunDist;
				DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = pCmdData->StationData_RunDist;
				ADD_LOG_TASKCONTROL(" End Station Step1_");

			}
			else
			{
				//VHL Map 참조 필요(메뉴얼 MTL 빼냄 등)
				Error = GetStationData(&ENDStationData,uTargetStation);
				if(Error == NO_ERR)
				{
					FinalMarkType =  ENDStationData.StationType;

					TempENDDrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = false;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_EndNodeID;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID = pCmdData->PathData_EndNodeID;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =0.0 ;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = ENDStationData.NextNodeID;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = ENDStationData.NextNodeID;
					TempENDDrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = true;

					DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = true;
					DrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->PathData_EndNodeID;
					DrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = ENDStationData.NextNodeID;

					//추후에 상위와 협의후 변경 필요.

					TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = ENDStationData.StationOffset;
					DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = ENDStationData.StationOffset;

					//puting
					pCmdData->bIsTargetStationoPassPermitOpen =ENDStationData.Safety_Level;
					DrivingAdditionalInfo.StationSafetyLevel = ENDStationData.Safety_Level;

					ADD_LOG_TASKCONTROL("puting Safty%d offset %d ",ENDStationData.Safety_Level,ENDStationData.StationOffset);
				}
			}

			ADD_LOG_TASKCONTROL(" End Station Step1_1");
		}
		else
			return ERR_STD_CMD_ED_PATH_FAIL;


	}
	else
	{

		DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;
		DrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->PathData_EndNodeID;
		DrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = pCmdData->PathData_EndNodeID;
		DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = 0.0;
		ADD_LOG_TASKCONTROL(" End Station No");
	}

	//이부분 버그있음.  //동일한 노드로 정보보내면 한바퀴 돌아서 Pathseaerh하도록 변경 완료.
    // 한번만 패스서치해야되는 경우만 넣어줄것.

	if((pCmdData->PathData_StartNodeID == pCmdData->PathData_EndNodeID) ||
	  ((TempSTDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID == pCmdData->PathData_EndNodeID) && (DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd ==false)))
	{
		bChecksameNode = true;
	}


	if(pCmdData->PathData_StartNodeID == pCmdData->PathData_EndNodeID)
	{


		if((TempSTDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID == TempENDDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID) &&
		   (TempSTDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID != TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID) &&
		   (DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart == true) &&
		   (DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd == true))
		   {
				bCheckUTurn = false;
				bChecksameNode = false;       //추가된 부분 확인해 볼것
           }
		else if((((int)DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist - (int)DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist) < 200)  //30mm에서 50mm로 늘림.
           && (DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist == 0.0)
		   && (DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart ==true))
		{
			bCheckUTurn = false;
			bChecksameNode = false;

		}
		else if(((int)DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist - (int)DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist) < 50)  //30mm에서 50mm로 늘림.
//		   && (DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist != 0.0)
	   //	   && (DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart ==true))
		{
			bCheckUTurn = false;
			bChecksameNode = false;
		}
        else if((NextNode!=0) && (NextNode != DrivingAdditionalInfo.ProcessingGoInfo.EndNodeID))
		{
            bCheckUTurn = false;
			bChecksameNode = false;
        }

	}

	if(bChecksameNode ==true)
			DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID =pCmdData->PathData_StartNodeID;


	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1 2. << Cycle List 처리 부분>>

	// Step 1. Start Node(NextNode 포함)와 Target Node(Next Node 포함)가 동일하지 않다는 가정하에 탐색.
	// 탐색은 Next Node에서부터 Next Node 탐색함.

	// Pass Search로 리스트를 보내 Cycle List를 생성
	ADD_LOG_TASKCONTROL("MakeGoCycleList StopNodeID:%06d, StartNodeID:%06d, EndNodeID:%06d,StationID:%06d,CmdID:%d, Cycle%X,bChecksameNode%d,bCheckUTurn%d %d %d"
	,pCmdData->StopNodeID,pCmdData->PathData_StartNodeID, pCmdData->PathData_EndNodeID,pCmdData->StopStationID, pCmdData->CmdID,pCmdData->CmdCycle,bChecksameNode,bCheckUTurn,DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart,DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd );


	if(pCmdData->CmdCycle ==CYCLE::PositionSearching) //마크인 경우
	{

			CmdType = EXECUTE_DRIVING_CMD_MARK;   //마크로 바꾸기 .
//     		pTmeCycleData = new PKT_CYCLECMDDATA;
//			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));
//
//			pTmeCycleData->CmdID = pCmdData->CmdID;
//			pTmeCycleData->IsExcuteInfo = false;
//			pTmeCycleData->IsListF2B = false;
//
//			pTmeCycleData->CycleType = CYCLETYPE_DRIVING;
//			pTmeCycleData->EXE_CycleDriving_Info.AddInfo.CmdType = EXECUTE_DRIVING_CMD_MARK;
//			pTmeCycleData->EXE_CycleDriving_Info.AddInfo.StationID = pCmdData->TransCmd.CurStationID;
//			// pTmeCycleData->EXE_Driving_Info.AddInfo.StationType = tmpID121.StationData_MarkType;
//			// 기타 추가 내용 넣기, Excute 부분도 변경 할 것.
//
//			m_pCmdManager->AddCycle(pTmeCycleData);
			return ERR_STD_CMD_ED_PATH_FAIL;

	}
	else //멀리 이동하는 경우.
	{
		if ((Error == NO_ERR) && (bChecksameNode == false))
		{


//			ADD_LOG_TASKCONTROL("MakeGoCycleList StopNodeID:%06d, StartNodeID:%06d, EndNodeID:%06d,StationID:%06d,NextID:%06d,CmdID:%d, Cycle%X"
//			,pCmdData->StopNodeID,pCmdData->PathData_StartNodeID, pCmdData->PathData_EndNodeID,pCmdData->StopStationID,pCmdData->NextNodeID, pCmdData->CmdID,pCmdData->CmdCycle);

			DrivingAdditionalInfo.CmdType = CmdType; // 명령 타입
			DrivingAdditionalInfo.FinalNode = uTargetNode; // 현재 받은 타겟 노드: 상황에 따라 업데이트 됨

			//pCmdData->MarkType;
			if(CmdType == EXECUTE_DRIVING_CMD_MAPMAKE)
			{
				DrivingAdditionalInfo.SpeedLimitOfMapMake = m_pMapMake_Status->Speed_MapMake;
			}
			//이적재가 Target인 경우 정보 추가.
			if (uTargetStation != 0)
			{
					 if(bCheckUTurn == false)
					{
						DrivingAdditionalInfo.StationID = uTargetStation;
						DrivingAdditionalInfo.StationType =FinalMarkType;// pCmdData->MarkType; // 2. 이적재 노드 타입  --주행도 포함.
						DrivingAdditionalInfo.FinalNode = TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID;//ENDStationData.NextNodeID;//pCmdData->NextNodeID; // 이적재
						DrivingAdditionalInfo.DistToStation = DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist ; // 3. 이적재 노드까지 거리
						DrivingAdditionalInfo.STBOffset = pCmdData->TeachingData_STBOffset;

						if(DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist < 25.0)
						{
							pCmdData->bIsTargetStationoPassPermitOpen =false;
							DrivingAdditionalInfo.StationSafetyLevel = 0x00;

							bIgnolOverRun = true;
                        }
					}else
					{
						TempENDDrivingAdditionalInfo.StationID = uTargetStation;
						TempENDDrivingAdditionalInfo.StationType = FinalMarkType;//pCmdData->MarkType; // 2. 이적재 노드 타입  --주행도 포함.
						TempENDDrivingAdditionalInfo.FinalNode = TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID;//ENDStationData.NextNodeID;//pCmdData->NextNodeID; // 이적재
					//	 DrivingAdditionalInfo.FinalPortType = pCmdData->GoCmd.FinalPortType;
						TempENDDrivingAdditionalInfo.DistToStation = TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist ; // 3. 이적재 노드까지 거리
						TempENDDrivingAdditionalInfo.STBOffset = pCmdData->TeachingData_STBOffset;
//						DrivingAdditionalInfo.TPosition_Rotate =  pCmdData->GoCmd.TPosition_Rotate;   ///< Rotate Teaching값(degree)
						if(TempENDDrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist < 25.0)
						{
							pCmdData->bIsTargetStationoPassPermitOpen =false;
							DrivingAdditionalInfo.StationSafetyLevel = 0x00;
							bIgnolOverRun = true;
						}
					}
			}



			if (!m_pOHTMainStatus->DataUseFlag.fSwapping)
			{


				//이적재에서 출바할때 부분에 대한 처리.
				if(DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart == true)
				{


					if(m_pPassPermit->GetPassPermitCount() != LISTCOUNT_EMPTY)
					{
						//예외처리 추가.
						if(m_pPassPermit->GetPassPermitCount() != 1)
							Error = ERR_STD_ETC_PASSPERMIT_FAIL;
						else
						{
							DATA_PASS * pCurPPCommon; // Command를 위한 포인터 변수
							pCurPPCommon = (DATA_PASS*)m_pPassPermit->ReferBackPassPermit();//ReferFirstPassPermit();

							int nTempResetNodeMain = 0;
							int nTempStopNode = 0;
							for (int i = 0; i < 3; i++)
							{
								// Node 비트연산   puting
								int Temp1 = pCurPPCommon->PassInfo.ResetNodeID_Main[i];
								int Temp2 = pCurPPCommon->PassInfo.StopNodeID[i];
								if (HIGH_BYTE == 0) {
									nTempResetNodeMain += Temp1 << ((2 - i) * 8);
									nTempStopNode += Temp2 << ((2 - i) * 8);
								}
								else
								{
									nTempResetNodeMain += Temp1 << (i * 8);
									nTempStopNode += Temp2 << ((2 - i) * 8);
								}

							}

							if((TempSTDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID ==  nTempResetNodeMain)
							&& (TempSTDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID ==nTempStopNode))
							{
								FinalSeqNum = pCurPPCommon->PassInfo.SQNum;
								memmove(&TempPassPermitCommon, &pCurPPCommon->PassInfo, sizeof(PASSPERMIT_COMMON));
							}
							else
							{
								DATA_PASSOPEN * pDataPassOpen;
								pDataPassOpen = new DATA_PASSOPEN;
								pDataPassOpen->PassOpenIndex = 0;
								pDataPassOpen->bPassOpenType = JOBCHAGE_RETURN_TYPE;
								memmove(&pDataPassOpen->PassOpneInfo, &pCurPPCommon->PassInfo,sizeof(PASSPERMIT_COMMON));
								m_pPassPermit->AddPassOpen(pDataPassOpen);

								ADD_LOG_TASKCONTROL("경로 변경에 의한 통허반납 %d",pDataPassOpen->PassOpneInfo.SQNum);
							}
						}
					}

                   	if(Error == NO_ERR)
						Error = m_pPathSearch->StandardSearchOnlyOnePassPermit(OHTMainForm->GetMap(),OHTMainForm->GetFlowControl_STD(), pCmdData, TempSTDrivingAdditionalInfo, &TmpeList, false, CheckCycleList,uExtraPathNum,TempDistance,FinalSeqNum,TempPassPermitCommon,m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1); // 명령이 담기는 구조체

					ADD_LOG_TASKCONTROL("[MakeCmd][Go_StationStart][Error:%d]StartNID:%06d, StopNID:%06d %d %d %f %d",
										Error, uStartNode, uTargetNode,
										TempSTDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID,
										FinalSeqNum,
										TempDistance,
										TempSTDrivingAdditionalInfo.ProcessingGoInfo.PathDiv);


					DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = false;
						// 일반 주행할 떄에 대한 부분.(Target이 이적재일경우도 포함됨.

					if(Error == NO_ERR)
						Error = m_pPathSearch->StandardSearchPassPermit(OHTMainForm->GetMap(),OHTMainForm->GetFlowControl_STD(), pCmdData, DrivingAdditionalInfo, &TmpeList, uExtraPathNum,m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1,TempDistance,bIgnolOverRun); // 명령이 담기는 구조체

				}
				else
				{
					//오버런에 의해서 발생한 경우 주행위치에서는 기존값 삭제
                    if(m_pPassPermit->GetPassPermitCount() != LISTCOUNT_EMPTY)
					{
						//예외처리 추가.
						if(m_pPassPermit->GetPassPermitCount() != 1)
							Error = ERR_STD_ETC_PASSPERMIT_FAIL;
						else
							m_pPassPermit->InitPassPermitList();

						ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node][Clear Overrun!]");
					}
					// 일반 주행할 떄에 대한 부분.(Target이 이적재일경우도 포함됨.
					if(Error == NO_ERR)
						Error = m_pPathSearch->StandardSearchPassPermit(OHTMainForm->GetMap(),OHTMainForm->GetFlowControl_STD(), pCmdData, DrivingAdditionalInfo, &TmpeList, uExtraPathNum,m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1,0.0, bIgnolOverRun); // 명령이 담기는 구조체

				}

				// by zzang9un 2013.11.11 : 로그 추가
				if(Error)
				{
					ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node][Error:%d]StartNID:%06d, StopNID:%06d",
										Error, uStartNode, uTargetNode);
				}
				else
				{
					// List 정리부분
					nListCount = TmpeList.getCount();

					if ((Error == NO_ERR) && (nListCount != 0))
					{

						for (int Index = 0; Index < nListCount; Index++)

						{      //  Test용    puting
								PKT_CYCLECMDDATA *TempCycleList;

								TempCycleList = (PKT_CYCLECMDDATA*) TmpeList.popFront();
//								TempCycleList->EXE_CycleDriving_Info.

								ADD_LOG_TASKCONTROL(" MakeGoCycleList:Cnt%d SQ:%d [Start]Type%x,ND:%d,Dis:%f[End]Nd:%d,Dis%f[Stop]%f[Reset]%f, %d %d %d %d",
								nListCount,
                                TempCycleList->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum,
								TempCycleList->EXE_CycleDriving_Info.StartNodeType,
								TempCycleList->EXE_CycleDriving_Info.StartPoint,
								TempCycleList->EXE_CycleDriving_Info.StartDistance,
								TempCycleList->EXE_CycleDriving_Info.EndPoint,
								TempCycleList->EXE_CycleDriving_Info.EndDistance,
								TempCycleList->EXE_CycleDriving_Info.StopDistance,
								TempCycleList->EXE_CycleDriving_Info.ResetNodeDistance,
								TempCycleList->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint,
								TempCycleList->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint,
								TempCycleList->EXE_CycleDriving_Info.D_PPStep,
								TempCycleList->EXE_CycleDriving_Info.D_PPOStep);


								m_pCmdManager->AddCycle(TempCycleList);
							   //

							   //	m_pCmdManager->AddCycle((PKT_CYCLECMDDATA*) TmpeList.popFront());
						}

					}
					else
					{
						Error == ERR_PATH_FORMAT;
						ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
									ERR_PATH_FORMAT,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
					}


					if(DrivingAdditionalInfo.CmdType == EXECUTE_DRIVING_CMD_MAPMAKE)
                    {
                        ADD_LOG_TASKCONTROL("[MakeCmd][MapMake][EndNID:%06d] StartNID:%06d, StopNID:%06d, Speed:%d",
                                        m_pMapMake_Status->EndNodeID, uStartNode, uTargetNode,
                                        DrivingAdditionalInfo.SpeedLimitOfMapMake);
                    }
                    else
                    {
						ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node] Error: %d StartNID:%06d, StopNID:%06d",
										Error,uStartNode, uTargetNode);
                    }
				}

			}
			else
			{
				Error = ERR_PATHSEARCH_FAIL;
				ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

				ADD_LOG_TASKCONTROL("MakeGoCycleList uStartNode:%06d, uTargetNode:%06d, bChecksameNode:%d, Error%d"
					,uStartNode,uTargetNode, bChecksameNode,Error);



		}
		else if ((Error == NO_ERR) && (bChecksameNode == true))
		{
			 //이적재에서 출발할 경우 -> 이적재로 이동

						//이적재가 Target인 경우 정보 추가.
					DrivingAdditionalInfo.CmdType = CmdType; // 명령 타입
					DrivingAdditionalInfo.FinalNode = uTargetNode; // 현재 받은 타겟 노드: 상황에 따라 업데이트 됨
			      //DrivingAdditionalInfo.StationType = pCmdData->Station; // 2. 이적재 노드 타입  --주행도 포함.

					//이적재가 Target인 경우 정보 추가.
					if (uTargetStation != 0)
					{
							DrivingAdditionalInfo.StationID = uTargetStation;
							DrivingAdditionalInfo.StationType = FinalMarkType;//pCmdData->MarkType; // 2. 이적재 노드 타입  --주행도 포함.
							DrivingAdditionalInfo.FinalNode = TempENDDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID;//ENDStationData.NextNodeID; // 이적재
						 // DrivingAdditionalInfo.FinalPortType = pCmdData->GoCmd.FinalPortType;
							DrivingAdditionalInfo.DistToStation = DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist ; // 3. 이적재 노드까지 거리
			//					DrivingAdditionalInfo.TPosition_Rotate =  pCmdData->GoCmd.TPosition_Rotate;   ///< Rotate Teaching값(degree)
							DrivingAdditionalInfo.STBOffset = pCmdData->TeachingData_STBOffset;
					}

					//정지노드의 통과허가 처리로 인해 추가됨.
					if(DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart == true)
					{
						if(m_pPassPermit->GetPassPermitCount() != LISTCOUNT_EMPTY)
						{
							//예외처리 추가.
							if(m_pPassPermit->GetPassPermitCount() != 1)
								Error = ERR_STD_ETC_PASSPERMIT_FAIL;
							else
							{

								DATA_PASS * pCurPPCommon; // Command를 위한 포인터 변수
								pCurPPCommon = (DATA_PASS*)m_pPassPermit->ReferBackPassPermit();//ReferFirstPassPermit();

								int nTempResetNodeMain = 0;
								int nTempStopNode = 0;
								for (int i = 0; i < 3; i++)
								{
									// Node 비트연산   puting
									int Temp1 = pCurPPCommon->PassInfo.ResetNodeID_Main[i];
									int Temp2 = pCurPPCommon->PassInfo.StopNodeID[i];
									if (HIGH_BYTE == 0)
									{
										nTempResetNodeMain += Temp1 << ((2 - i) * 8);
										nTempStopNode += Temp2 << ((2 - i) * 8);
									}
									else
									{
										nTempResetNodeMain += Temp1 << (i * 8);
										nTempStopNode += Temp2 << ((2 - i) * 8);
									}

								}

								if((TempSTDrivingAdditionalInfo.ProcessingGoInfo.EndNodeID ==  nTempResetNodeMain)
								&& (TempSTDrivingAdditionalInfo.ProcessingGoInfo.StartNodeID ==nTempStopNode))
								{
									memmove(&TempPassPermitCommon, &pCurPPCommon->PassInfo, sizeof(PASSPERMIT_COMMON));
                                    FinalSeqNum = pCurPPCommon->PassInfo.SQNum;
								}
								else
								{
									DATA_PASSOPEN * pDataPassOpen;
									pDataPassOpen = new DATA_PASSOPEN;
									pDataPassOpen->PassOpenIndex = 0;
									pDataPassOpen->bPassOpenType = JOBCHAGE_RETURN_TYPE;
									memmove(&pDataPassOpen->PassOpneInfo, &pCurPPCommon->PassInfo,sizeof(PASSPERMIT_COMMON));
									m_pPassPermit->AddPassOpen(pDataPassOpen);

									ADD_LOG_TASKCONTROL("경로 변경에 의한 통허반납 %d",pDataPassOpen->PassOpneInfo.SQNum);
								}
							}
						}
					}

					if (!m_pOHTMainStatus->DataUseFlag.fSwapping)
					{


						ADD_LOG_TASKCONTROL("Error %d",Error);
						Error = m_pPathSearch->StandardSearchOnlyOnePassPermit(OHTMainForm->GetMap(), OHTMainForm->GetFlowControl_STD() ,pCmdData, DrivingAdditionalInfo, &TmpeList, false, CheckCycleList,uExtraPathNum,TempDistance,FinalSeqNum, TempPassPermitCommon,m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.Carriage.Load1); // 명령이 담기는 구조체



						// by zzang9un 2013.11.11 : 로그 추가
						if(Error)
						{
							ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node][Error:%d]StartNID:%06d, StopNID:%06d",
												Error, uStartNode, uTargetNode);
						}
						else
						{
							// List 정리부분
							nListCount = TmpeList.getCount();

							ADD_LOG_TASKCONTROL("MakeGoCycleList nListCount:%d",nListCount);

							if ((Error == NO_ERR) && (nListCount == 1))
							{

								PKT_CYCLECMDDATA *TempCycleList;
								TempCycleList = (PKT_CYCLECMDDATA*) TmpeList.popFront();
								m_pCmdManager->AddCycle(TempCycleList);

							}
							else
							{
								Error == ERR_PATH_FORMAT;
								ADD_FDC_LOG("ERR/%d/4/5/%06d/%06d/%d/%d/%06d",
											ERR_PATH_FORMAT,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pExecuteInfo->ExecutePositionInfo.NextNode,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pOHTMainStatus->StatusCommon.ExistFoup,
											m_pOHTMainStatus->StatusCommon.StopStationID);
							}


								ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node] StartNID:%06d, StopNID:%06d",
												uStartNode, uTargetNode);

						}

					}
					else
						Error = ERR_STD_CMD_MD_PATH_FAIL;




		}


	}


	return Error;

}

/**
 @brief   주행 관련 Command를 처리하는 함수(후진)
 @author  puting
 @date    2016.10.22
 @param   pCmdData : 처리해야 할 Command
 @return  정상인 경우 0을 리턴, 그렇지 않은 경우 에러 코드 리턴
 @todo    Status 상태 체크, 이전 Cmd type 체크, Path Search 하는 파라미터 등 협의 사항 미정
 */
int TaskControl::BackGoCycleList(PARSING_GO_INFO *pCmdData, int StartNode, int StationNode, int nDiv)
{
	int Error = NO_ERR;

	int nListCount = 0;

	//시작 위치에서 다겟 위치까지
	UINT uTargetNode =  pCmdData->PathData_EndNodeID;  //pCmdData->StopNodeID;
	UINT uTargetStation = pCmdData->StopStationID;

	UINT uStartNode =  StartNode;//m_pExecuteInfo->ExecutePositionInfo.CurrNode; // Path Search의 시작 Node
	UINT uStartStion = StationNode;//m_pExecuteInfo->ExecutePositionInfo.CurrStation;

   //	UINT uFinalDrivingNodeOfPathSearch = 0;
	/// 현재 VHL의 마지막 목적지 (정차시는 현재 위치)  ??

	// 추가 정보를 위한 변수
	LList TmpeList;         ///<임시 리스트
	EXECUTE_DRIVING_ADD_INFO DrivingAdditionalInfo; // 주행 추가 정보

	memset(&DrivingAdditionalInfo, 0x00, sizeof(EXECUTE_DRIVING_ADD_INFO));
	// 추가 주행 정보 초기화
	EXECUTE_DRIVING_CMD_TYPE CmdType;

	// 선조향을 위한 변수

	STATION_DATA STStationData;
	STATION_DATA ENDStationData;
	memset(&STStationData, 0x00, sizeof(STATION_DATA));
	memset(&ENDStationData, 0x00, sizeof(STATION_DATA));

	MARK_TYPE StartMarkType;
	MARK_TYPE FinalMarkType;

	PKT_CYCLECMDDATA *CheckCycleList = NULL;

    bool    bCheckMarked = m_pExecuteInfo->ExecutePositionInfo.Marked ;

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	// 1 1. <<전처리 부분>> 상태 확인 및 초기화
	// Mapmaker 유무에 의해 변경 확인

	//Map에서 정보 획득.

	if(StartNode != 0)
	{
		uStartNode = StartNode;
		uStartStion = StationNode;

		ADD_LOG_TASKCONTROL("빼냄 Cycle..%d %d %d",uStartNode,uStartNode,uStartStion);
	}


	// 그 외 주행 명령은 모두 Go로 설정
	CmdType = EXECUTE_DRIVING_CMD_GOBACK;

// Start Node(NextNode 포함)와 Target Node(Next Node 포함)가 동일 한지 확인.
// Currstion 정보가 날라가는 시점
// 에러 발생 or  marked가 false에서 Reset 명령시 날라감.  예외처리를 어떻게 할지 확인 필요...
// 현재  CurrStation은 마크디드 상태인지 같이 Check

	if((m_pOHTMainStatus->IsOnGoCmd == true) || // OnGoCmd 거나
		(m_pExecuteInfo->StatusCmdId.Info.Status == STS_GOING) ||// 주행 중이거나
		(m_pExecuteInfo->ExecuteUnit == EXECUTE_UNIT_DRIVING && m_pExecuteInfo->ListOfCmd.getCount() > 0)) // 주행 명령이 있을 경우
	{
		ADD_LOG_TASKCONTROL("Cycle Fail...%d %d %d",m_pExecuteInfo->ListOfCmd.getCount(),m_pOHTMainStatus->IsOnGoCmd, StationNode);
		if(StationNode ==0)
		{
			return ERR_STD_CMD_CYCLE_FAIL;
        }
	}
	//동일한 부모노드로 이동할 경우, 주행 노드->이적재, 이적재->이적재, 이적재 -> 주행 노드
   	 // 마크 상태 확인
//	if(bCheckMarked==false)
//	{
//		//후진은 마크 안되면 에러 처리.
//		ADD_LOG_TASKCONTROL("No Marked Go _ %d %d",nDiv,m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset);
//		return ERR_STD_CMD_CYCLE_FAIL;
//	}

	 if((uStartNode ==0) ||
	   (uStartNode != pCmdData->PathData_StartNodeID))
	   {
			ADD_LOG_TASKCONTROL(" ERR_PATHSEARCH_FAIL Fail %d %d",uStartNode,pCmdData->PathData_StartNodeID);
			return ERR_STD_CMD_PATH_FAIL;
	   }


	// 2.1 Start/Target Station에 대한 정보 얻기
	///<1 시작에 대한 정보
	if(uStartStion !=0)
	{

		Error = GetStationData(&STStationData,uStartStion);

		if(Error == NO_ERR)
		{
			DrivingAdditionalInfo.ProcessingGoInfo.PathDiv = nDiv;
			DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart = true;
			DrivingAdditionalInfo.ProcessingGoInfo.OriginStartNodeID = pCmdData->PathData_StartNodeID;
			DrivingAdditionalInfo.ProcessingGoInfo.StartNodeID = pCmdData->PathData_StartNodeID;
			DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDist =STStationData.StationOffset ;
			ADD_LOG_TASKCONTROL(" Start Station Step_1");
		}else
			return ERR_STD_CMD_ST_PATH_FAIL;
	}
	else
	{
    	return ERR_STD_CMD_ST_PATH_FAIL;
	}
 	///< 도착에 대한 정보
	if (uTargetStation != 0)
	 //	(uNextNode != 0))
	{
		Error = GetStationData(&ENDStationData,uTargetStation);
		if(Error == NO_ERR)
		{

			FinalMarkType =  ENDStationData.StationType;

			DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;
			DrivingAdditionalInfo.ProcessingGoInfo.OriginEndNodeID = pCmdData->PathData_EndNodeID;
			DrivingAdditionalInfo.ProcessingGoInfo.EndNodeID = pCmdData->PathData_EndNodeID;

			//추후에 상위와 협의후 변경 필요.
			if(ENDStationData.StationOffset != pCmdData->StationData_RunDist)
			{
				DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = ENDStationData.StationOffset;
			}
			else
			{
				DrivingAdditionalInfo.ProcessingGoInfo.StationData_EndRunDist = pCmdData->StationData_RunDist;
			}
			ADD_LOG_TASKCONTROL(" End Station Step1");
		}
		else
			return ERR_STD_CMD_ED_PATH_FAIL;

	}
	else
	{
		return ERR_STD_CMD_ED_PATH_FAIL;
	}

	if(pCmdData->PathData_StartNodeID == pCmdData->PathData_EndNodeID)
	{
		return ERR_STD_CMD_ED_PATH_FAIL;
	}

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1 2. << Cycle List 처리 부분>>

	// Step 1. Start Node(NextNode 포함)와 Target Node(Next Node 포함)가 동일하지 않다는 가정하에 탐색.
	// 탐색은 Next Node에서부터 Next Node 탐색함.

	// Pass Search로 리스트를 보내 Cycle List를 생성
	ADD_LOG_TASKCONTROL("MakeGoCycleList StopNodeID:%06d, StartNodeID:%06d, EndNodeID:%06d,StationID:%06d,CmdID:%d, Cycle%X, %d %d"
	,pCmdData->StopNodeID,pCmdData->PathData_StartNodeID, pCmdData->PathData_EndNodeID,pCmdData->StopStationID, pCmdData->CmdID,pCmdData->CmdCycle,DrivingAdditionalInfo.ProcessingGoInfo.IsStationStart,DrivingAdditionalInfo.ProcessingGoInfo.IsStationEnd );


		if (Error == NO_ERR)
		{
			DrivingAdditionalInfo.CmdType = CmdType; // 명령 타입

			//후진기능에 의해 변경
			DrivingAdditionalInfo.StationID = uStartStion;
			DrivingAdditionalInfo.StationType =MARK_TYPE_TAG_STATION_BACK;// pCmdData->MarkType; // 2. 이적재 노드 타입  --주행도 포함.
			DrivingAdditionalInfo.FinalNode = pCmdData->PathData_StartNodeID;//pCmdData->NextNodeID; // 이적재
			DrivingAdditionalInfo.DistToStation = DrivingAdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode; // 3. 이적재 노드까지 거리


			if (!m_pOHTMainStatus->DataUseFlag.fSwapping)
			{

				// 일반 주행할 떄에 대한 부분.(Target이 이적재일경우도 포함됨.
				if(Error == NO_ERR)
					Error = m_pPathSearch->StandardSearchPassBack(OHTMainForm->GetMap(), pCmdData, DrivingAdditionalInfo, &TmpeList); // 명령이 담기는 구조체

				// by zzang9un 2013.11.11 : 로그 추가
				if(Error)
				{
					ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node][Error:%d]StartNID:%06d, StopNID:%06d",
										Error, uStartNode, uTargetNode);
				}
				else
				{
					// List 정리부분
					nListCount = TmpeList.getCount();

					if ((Error == NO_ERR) && (nListCount != 0))
					{

						for (int Index = 0; Index < nListCount; Index++)

						{      //  Test용    puting
								PKT_CYCLECMDDATA *TempCycleList;

								TempCycleList = (PKT_CYCLECMDDATA*) TmpeList.popFront();
//								TempCycleList->EXE_CycleDriving_Info.

								ADD_LOG_TASKCONTROL(" BackGoCycleList:Cnt%d SQ:%d [Start]Type%x,ND:%d,Dis:%f[End]Nd:%d,Dis%f[Stop]%f[Reset]%f, %d %d %d %d",
								nListCount,
                                TempCycleList->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum,
								TempCycleList->EXE_CycleDriving_Info.StartNodeType,
								TempCycleList->EXE_CycleDriving_Info.StartPoint,
								TempCycleList->EXE_CycleDriving_Info.StartDistance,
								TempCycleList->EXE_CycleDriving_Info.EndPoint,
								TempCycleList->EXE_CycleDriving_Info.EndDistance,
								TempCycleList->EXE_CycleDriving_Info.StopDistance,
								TempCycleList->EXE_CycleDriving_Info.ResetNodeDistance,
								TempCycleList->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint,
								TempCycleList->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint,
								TempCycleList->EXE_CycleDriving_Info.D_PPStep,
								TempCycleList->EXE_CycleDriving_Info.D_PPOStep);


								m_pCmdManager->AddCycle(TempCycleList);

						}

					}
					else
					{
						Error == ERR_PATH_FORMAT;
						ADD_FDC_LOG("ERR/%d/5/5/%06d/%06d/%d/%d/%06d",
									ERR_PATH_FORMAT,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
					}

						ADD_LOG_TASKCONTROL("[MakeCmd][Go_Node] Error: %d StartNID:%06d, StopNID:%06d",
										Error,uStartNode, uTargetNode);

				}

			}
			else
			{
				Error = ERR_PATHSEARCH_FAIL;
				ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

				ADD_LOG_TASKCONTROL("MakeGoCycleList uStartNode:%06d, uTargetNode:%06d, Error%d"
					,uStartNode,uTargetNode,Error);



		}



	return Error;

}

int TaskControl::MakeTransCycleList(PARSING_TRANS_INFO *pCmdData)
{

	int nError= NO_ERR;
	PKT_CYCLECMDDATA *pTmeCycleData;
	int PreCycleConut  =0;
	int CurCycleCount  =0;
	PARSING_GO_INFO OutTempGOInfo;
	int TempDiv =0;


	NODE_INFO_STD_PATHDATA Pathdata;
	STATION_DATA StationData;
	STATION_DATA TempStationData;
	int TakeOutStartStation = 0;

	bool bBackMove = false;

	m_mccparam.nEqType = pCmdData->TransCmd.PortType;

	 /*
	 // Trans Type 구별
	 pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_MOVET;
	 pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_AUTOSET_HOME;
	 */

// 2017-01-21, Add by Labeler
// V7.5 Test bed에서 임시로 이적재 롱런 위해 추가


	ADD_LOG_TASKCONTROL("MakeTransCycleList Init Cycle:%d", pCmdData->CmdCycle);
	switch (pCmdData->CmdCycle)
	{
		case CYCLE::CraneUp: // 올림 cycle
			pTmeCycleData = new PKT_CYCLECMDDATA;
			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));

			pTmeCycleData->CmdID = pCmdData->CmdID;
			pTmeCycleData->IsExcuteInfo = false;
			pTmeCycleData->IsNoExcuteCycle =false;
			pTmeCycleData->IsListF2B = false;
			pTmeCycleData->Isreturn = false;

			pTmeCycleData->CycleType = CYCLETYPE_TRANS;
			pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_UNLOAD;


			memmove(&pTmeCycleData->EXE_Trans_Info.TransCmd, &pCmdData->TransCmd, sizeof(TRANS_CMD));

			if (GetCurrentMode() == MODE_MANUAL)
			{
				 nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, pCmdData->TransCmd.PortType, &pTmeCycleData->EXE_Trans_Info.TransCmd);

				// PORT_TYPE_EQ   OCS 미적용으로 인한 포트 구분 하드코딩
				// PORT_TYPE_STB_L      ///< STB Left
				// PORT_TYPE_STB_R     ///< STB Right
				 if(nError) nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, PORT_TYPE_STB_L, &pTmeCycleData->EXE_Trans_Info.TransCmd);
				 if(nError) nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, PORT_TYPE_STB_R, &pTmeCycleData->EXE_Trans_Info.TransCmd);

				 //EQ PIO 사용 안함 유무
				 if(pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.EQPIODisable_TP == true)  //fasle 0427
					 pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.EQPIODisable_TP = false;  //true 0427

			}

			// puting 2015.11.04 : Auto Recovery 변수 설정
			// puting 20160.05.05 : Auto Recovery 변수 추가
			ADD_LOG_TASKCONTROL("AutoRecovery------------:%d", pCmdData->AutoRecovery);
			 if(pCmdData->AutoRecovery ==0x01)
				pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery = true;
			 else if(pCmdData->AutoRecovery ==0x02)
				pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery =false;
			// else
			   // 	pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery = false;
			   //	pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery = &pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery;
				// pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery = true;
			 //else if 와 else가 같은 내용 else if 삭제 or 내용 변경 !!

//			pTmeCycleData->EXE_Trans_Info.TransCmd.PIOChannel = 0;  //티칭정보에서 삭제됨.
			pTmeCycleData->EXE_Trans_Info.TransCmd.PIOCS = 0;  //티칭정보에서 삭제됨.

			ADD_LOG_TASKCONTROL("PIO Channel %d H:%f S:%f",  pTmeCycleData->EXE_Trans_Info.TransCmd.PIOChannel, m_pExecuteInfo->TransDifference.dHoist,m_pExecuteInfo->TransDifference.dShift);

			pTmeCycleData->EXE_Trans_Info.TransCmd.Tposition_Hoist -= (int)m_pExecuteInfo->TransDifference.dHoist;
			pTmeCycleData->EXE_Trans_Info.TransCmd.Tposition_Shift -= (int)m_pExecuteInfo->TransDifference.dShift;


			if(m_defualtparam->PreEQPIO)
			{
				if(nError==NO_ERR)
				{

					m_pExecuteInfo->FlagPreTrans.bPrePIO =true;
					m_pExecuteInfo->FlagPreTrans.nTargetID= pCmdData->TransCmd.CurStationID;
					m_pExecuteInfo->FlagPreTrans.nPIOID= pCmdData->TransCmd.PIOID; //pCmdData->TransCmd.PIOID;
					m_pExecuteInfo->FlagPreTrans.nPIOChannel= pTmeCycleData->EXE_Trans_Info.TransCmd.PIOChannel;
					m_pExecuteInfo->FlagPreTrans.cPortType= pCmdData->TransCmd.PortType; //pCmdData->TransCmd.PortType;

				}
			}

			if(nError==NO_ERR)
			{
				m_pCmdManager->AddCycle(pTmeCycleData);
			}
			else
				delete pTmeCycleData;

			break;

		case CYCLE::CraneDown: // 내림 cycle
			pTmeCycleData = new PKT_CYCLECMDDATA;
			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));

			pTmeCycleData->CmdID = pCmdData->CmdID;
			pTmeCycleData->IsExcuteInfo = false;
			pTmeCycleData->IsNoExcuteCycle =false;
			pTmeCycleData->IsListF2B = false;
			pTmeCycleData->Isreturn = false;

			pTmeCycleData->CycleType = CYCLETYPE_TRANS;
			pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_LOAD;

			memmove(&pTmeCycleData->EXE_Trans_Info.TransCmd, &pCmdData->TransCmd, sizeof(TRANS_CMD));

			if (GetCurrentMode() == MODE_MANUAL)
			{
				 nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, pCmdData->TransCmd.PortType, &pTmeCycleData->EXE_Trans_Info.TransCmd);
                 				// PORT_TYPE_EQ   OCS 미적용으로 인한 포트 구분 하드코딩
				// PORT_TYPE_STB_L      ///< STB Left
				//e PORT_TYPE_STB_R     ///< STB Right
				 if(nError) nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, PORT_TYPE_STB_L, &pTmeCycleData->EXE_Trans_Info.TransCmd);
				 if(nError) nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, PORT_TYPE_STB_R, &pTmeCycleData->EXE_Trans_Info.TransCmd);


				 if(pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.EQPIODisable_TP == true)  //false
					 pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.EQPIODisable_TP = false; //true

			}

			// puting 2015.11.04 : Auto Recovery 변수 설정
			// puting 20160.05.05 : Auto Recovery 변수 추가
			ADD_LOG_TASKCONTROL("AutoRecovery------------:%d", pCmdData->AutoRecovery);

			 if(pCmdData->AutoRecovery ==0x01)
				pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery = true;
			 else if(pCmdData->AutoRecovery ==0x02)
				pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery =false;
			 //else
			  //	pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery = false;
			 //else if 와 else가 같은 내용 else if 삭제 or 내용 변경 !!


			pTmeCycleData->EXE_Trans_Info.TransCmd.PIOCS = 0;  //티칭정보에서 삭제됨.
			ADD_LOG_TASKCONTROL("PIO Channel %d H:%f S:%f",  pTmeCycleData->EXE_Trans_Info.TransCmd.PIOChannel, m_pExecuteInfo->TransDifference.dHoist,m_pExecuteInfo->TransDifference.dShift);

			pTmeCycleData->EXE_Trans_Info.TransCmd.Tposition_Hoist -= (int)m_pExecuteInfo->TransDifference.dHoist;
			pTmeCycleData->EXE_Trans_Info.TransCmd.Tposition_Shift -= (int)m_pExecuteInfo->TransDifference.dShift;


			if(m_defualtparam->PreEQPIO)
			{
				if(nError==NO_ERR)
				{

//					m_pExecuteInfo->FlagPreTrans.bPreShutter = true;

					m_pExecuteInfo->FlagPreTrans.bPrePIO = true;
					m_pExecuteInfo->FlagPreTrans.nTargetID= pCmdData->TransCmd.CurStationID;
					m_pExecuteInfo->FlagPreTrans.nPIOID= pCmdData->TransCmd.PIOID; //pCmdData->TransCmd.PIOID;  하드코딩.
					m_pExecuteInfo->FlagPreTrans.nPIOChannel= pTmeCycleData->EXE_Trans_Info.TransCmd.PIOChannel;
					m_pExecuteInfo->FlagPreTrans.cPortType= pCmdData->TransCmd.PortType; //pCmdData->TransCmd.PortType; 하드코딩.

				}
			}

			if(m_defualtparam->PreShutter)
			{
				if(nError==NO_ERR)
				{
					m_pExecuteInfo->FlagPreTrans.bPreShutter = true;
					m_pExecuteInfo->FlagPreTrans.nTargetID= pCmdData->TransCmd.CurStationID;
				}
			}

			if(nError==NO_ERR)
				m_pCmdManager->AddCycle(pTmeCycleData);
			else
				delete pTmeCycleData;

			break;

        // 2016-12-20, Add by Labeler
		case CYCLE::TP_CheckPortTeaching:			// 이재부 학습 중(티칭중)
			pTmeCycleData = new PKT_CYCLECMDDATA;
			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));

			pTmeCycleData->CmdID = pCmdData->CmdID;
			pTmeCycleData->IsExcuteInfo = false;
			pTmeCycleData->IsNoExcuteCycle =false;
			pTmeCycleData->IsListF2B = false;
			pTmeCycleData->Isreturn = false;

			pTmeCycleData->CycleType = CYCLETYPE_TRANS;
			pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_MOVET;

			memmove(&pTmeCycleData->EXE_Trans_Info.TransCmd, &pCmdData->TransCmd, sizeof(TRANS_CMD));

			nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, pCmdData->TransCmd.PortType, &pTmeCycleData->EXE_Trans_Info.TransCmd);
			if(nError) nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, PORT_TYPE_STB_L, &pTmeCycleData->EXE_Trans_Info.TransCmd);
			if(nError) nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, PORT_TYPE_STB_R, &pTmeCycleData->EXE_Trans_Info.TransCmd);

			// puting 2015.11.04 : Auto Recovery 변수 설정
			// puting 20160.05.05 : Auto Recovery 변수 추가
			ADD_LOG_TASKCONTROL("AutoRecovery------------:%d", pCmdData->AutoRecovery);
			if(pCmdData->AutoRecovery ==0x01)
				pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery = true;
			else if(pCmdData->AutoRecovery ==0x02)
				pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery =false;
		   //	else
			 //	pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.AutoRecovery = false; //AOHC-196 사전 로그 추가 시 삭제 필요한 코드로 판


			if(pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.EQPIODisable_TP == true)  //false
				 pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.EQPIODisable_TP = false; //true

		 	ADD_LOG_TASKCONTROL("H:%f S:%f", m_pExecuteInfo->TransDifference.dHoist,m_pExecuteInfo->TransDifference.dShift);

			pTmeCycleData->EXE_Trans_Info.TransCmd.Tposition_Hoist -= (int)m_pExecuteInfo->TransDifference.dHoist;
			pTmeCycleData->EXE_Trans_Info.TransCmd.Tposition_Shift -= (int)m_pExecuteInfo->TransDifference.dShift;

			if(nError==NO_ERR)
				m_pCmdManager->AddCycle(pTmeCycleData);
			else
				delete pTmeCycleData;
            break;


			// 층간이동은 사양 및 처리 방안에 대해 확인 필요 puting 20151103
		case CYCLE::MovingBetweenFloor: // 층간이동 cycle
 			/*
			pTmeCycleData->CycleType = CYCLETYPE_DRIVING;
			pTmeCycleData->EXE_CycleDriving_Info.AddInfo.CmdType = EXECUTE_DRIVING_CMD_FLOOR;

			if (GetCurrentMode() == MODE_MANUAL) {
				// GetManaulTeachingData(&pTmeCycleData.EXE_Trans_Info.TransCmd)

				// puting 2015.11.04 : Auto Recovery 변수 설정
				if (pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.
					AutoRecovery)
					pTmeCycleData->EXE_Trans_Info.TransCmd.TransOption.Info.
						AutoRecovery = false;

			}


			m_pCmdManager->AddCycle(pTmeCycleData);
			*/
			break;

		case CYCLE::TakeOut:  			// 뺴냄 사이클      ----김태균선임에게 전달 받은 이후 구현 필요.
			//주행 사이클 만들기.


				PreCycleConut = m_pCmdManager->GetCycleCount();
				memset(&OutTempGOInfo, 0x00, sizeof(PARSING_GO_INFO));
				memset(&StationData, 0x00, sizeof(STATION_DATA));
				memset(&TempStationData, 0x00, sizeof(STATION_DATA));

				if(pCmdData->TransCmd.CurStationID != 0)
				{
					OutTempGOInfo.CmdID = pCmdData->CmdID;
					OutTempGOInfo.CmdCycle = pCmdData->CmdCycle;
					OutTempGOInfo.MarkType =  MARK_TYPE_TAG_STATION;      //이적재로 바꿀경우 변경할것...puting 20161022

					nError = OHTMainForm->GetPathDataManager_STD()->GetPathData(OHTMainForm->GetPathData_STD(),&Pathdata, pCmdData->TransCmd.CurStationID);

					if((nError == NO_ERR) && (Pathdata.LayoutType == 0x01))
					{
						//1. 뺴냄 경로 확인
						TakeOutStartStation = pCmdData->TransCmd.CurStationID;
						GetStationData(&StationData, pCmdData->TransCmd.CurStationID);
//						TakeOutStartStation = StationData.ParentNodeID;
						ADD_LOG_TASKCONTROL("Make TakeOut In %d ",TakeOutStartStation);
					 if(Pathdata.LeftNodeIDIn == pCmdData->TransCmd.CurStationID)
						{
							GetStationData(&TempStationData, Pathdata.LeftNodeIDMiddle);

							OutTempGOInfo.StopNodeID =TempStationData.ParentNodeID;
							OutTempGOInfo.PathData_StartNodeID = StationData.ParentNodeID;//pCmdData->TransCmd.CurNodeID;// Pathdata.LeftNodeIDIn;
							OutTempGOInfo.PathData_EndNodeID = TempStationData.ParentNodeID;
							OutTempGOInfo.StopStationID = Pathdata.LeftNodeIDMiddle;

							bBackMove  = Pathdata.LeftDriveDirection;   //후진

							TempDiv = 0; //왼쪽분기
						}
					 else
						{
							GetStationData(&TempStationData, Pathdata.RightNodeIDMiddle);

							OutTempGOInfo.StopNodeID = TempStationData.ParentNodeID;
							OutTempGOInfo.PathData_StartNodeID = StationData.ParentNodeID; //pCmdData->TransCmd.CurNodeID; //Pathdata.RightNodeIDIn;
                           	OutTempGOInfo.PathData_EndNodeID = TempStationData.ParentNodeID;
							OutTempGOInfo.StopStationID = Pathdata.RightNodeIDMiddle;


							bBackMove  = Pathdata.RightDriveDirection;  //후진

							TempDiv = 1; //오른쪽 분기
						}

						OutTempGOInfo.PathData_Def_Direction = TempDiv; //왼쪽 0, 오른쪽 1

                        // 2. 뺴냄 명령 경로 생성.
						if(bBackMove ==false)
							nError = MakeGoCycleList(&OutTempGOInfo,OutTempGOInfo.PathData_StartNodeID, TakeOutStartStation);
						else
						{
							ADD_LOG_TASKCONTROL("Make Back TakeOut In");
							//후진일 경우 차가되는 정보(뺴냄케이스)

							TakeOutStartStation = OutTempGOInfo.StopStationID;

							OutTempGOInfo.StopNodeID = StationData.ParentNodeID;
							OutTempGOInfo.PathData_StartNodeID = TempStationData.ParentNodeID; //pCmdData->TransCmd.CurNodeID; //Pathdata.RightNodeIDIn;
                           	OutTempGOInfo.PathData_EndNodeID = StationData.ParentNodeID;
							OutTempGOInfo.StopStationID = pCmdData->TransCmd.CurStationID;

							OutTempGOInfo.MarkType = MARK_TYPE_TAG_STATION_BACK;


							nError = BackGoCycleList(&OutTempGOInfo,OutTempGOInfo.PathData_StartNodeID, TakeOutStartStation, TempDiv);

						}

						//빼냄/투입 정보 초기화
						EnterCriticalSection(&OHTMainForm->m_CS);
						m_pExecuteInfo->FlagMTLInfo.nTakeIn = INIT;
						m_pExecuteInfo->FlagMTLInfo.nTakeOut = INIT;
						m_pExecuteInfo->FlagMTLInfo.nRailNum = INIT;
						LeaveCriticalSection(&OHTMainForm->m_CS);

						CurCycleCount = m_pCmdManager->GetCycleCount();

						ADD_LOG_TASKCONTROL("MakeTransCycleList Init PreCycle Cur:%d %d %d %d %d %d Back:%d", PreCycleConut,CurCycleCount,
						OutTempGOInfo.StopNodeID,OutTempGOInfo.PathData_StartNodeID,OutTempGOInfo.PathData_EndNodeID,pCmdData->TransCmd.CurNodeID, bBackMove);

						for(int i= PreCycleConut; i< CurCycleCount; i++)
						{
							pTmeCycleData = (PKT_CYCLECMDDATA *)m_pCmdManager->ReferCycle(i);
							pTmeCycleData->IsNoExcuteCycle = true;
						}

					}
					else
					{
						nError = 9999;
						ADD_LOG_TASKCONTROL("TakeOut Fail Layout:%d Error%d", Pathdata.LayoutType,nError);
					}
				}else
				{
					nError = 9999;
					ADD_LOG_TASKCONTROL("TakeOut Fal: Not Station!!");
				}

				ADD_LOG_TASKCONTROL("TakeOut %d %d %d %d %d", pCmdData->TransCmd.CurStationID,pCmdData->TransCmd.CurNodeID,
					TakeOutStartStation,StationData.ParentNodeID,StationData.StationID);

                m_pExecuteInfo->FlagPreTrans.bPrePIO = false;
				m_pExecuteInfo->FlagPreTrans.bPreShutter = false;

				break;

		case CYCLE::TP_PortTeaching:
			pTmeCycleData = new PKT_CYCLECMDDATA;
			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));

			pTmeCycleData->CmdID = pCmdData->CmdID;
            pTmeCycleData->CycleType = CYCLETYPE_TRANS;

			pTmeCycleData->IsExcuteInfo = false;
			pTmeCycleData->IsNoExcuteCycle =false;
			pTmeCycleData->IsListF2B = false;
			pTmeCycleData->Isreturn = false;

			memmove(&pTmeCycleData->EXE_Trans_Info.TransCmd, &pCmdData->TransCmd, sizeof(TRANS_CMD));
            nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, pCmdData->TransCmd.PortType, &pTmeCycleData->EXE_Trans_Info.TransCmd);

			pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_AUTOTEACHING;


			if(nError==NO_ERR)
				m_pCmdManager->AddCycle(pTmeCycleData);
			else
				delete pTmeCycleData;

			break;

		case CYCLE::TP_PortTeaching_NullType:
			pTmeCycleData = new PKT_CYCLECMDDATA;
			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));

			pTmeCycleData->CmdID = pCmdData->CmdID;
            pTmeCycleData->CycleType = CYCLETYPE_TRANS;

			pTmeCycleData->IsExcuteInfo = false;
			pTmeCycleData->IsNoExcuteCycle =false;
			pTmeCycleData->IsListF2B = false;
			pTmeCycleData->Isreturn = false;

			memmove(&pTmeCycleData->EXE_Trans_Info.TransCmd, &pCmdData->TransCmd, sizeof(TRANS_CMD));
            nError = GetManaulTeachingData(pCmdData->TransCmd.CurStationID, pCmdData->TransCmd.PortType, &pTmeCycleData->EXE_Trans_Info.TransCmd);

			pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_AUTOTEACHING_NULLTYPE;


			if(nError==NO_ERR)
				m_pCmdManager->AddCycle(pTmeCycleData);
			else
				delete pTmeCycleData;

			break;

        // 2017-01-02, Add by Labeler
        // All Axis Home Cycle 추가 구현
        case CYCLE::TP_CraneHome:
			pTmeCycleData = new PKT_CYCLECMDDATA;
			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));

			pTmeCycleData->CmdID = pCmdData->CmdID;
			pTmeCycleData->IsExcuteInfo = false;
			pTmeCycleData->IsNoExcuteCycle =false;
			pTmeCycleData->IsListF2B = false;
			pTmeCycleData->Isreturn = false;

            pTmeCycleData->EXE_Trans_Info.TransCmd.CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;
			pTmeCycleData->CycleType = CYCLETYPE_TRANS;
			pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_MOVE_ALL_HOME;

			if(nError==NO_ERR)
				m_pCmdManager->AddCycle(pTmeCycleData);
			else
				delete pTmeCycleData;
			break;

	case CYCLE::TP_SmalladdHoistHome:
			ADD_LOG_TASKCONTROL("TP_SmalladdHoistHome start");
			pTmeCycleData = new PKT_CYCLECMDDATA;
			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));

			pTmeCycleData->CmdID = pCmdData->CmdID;
			pTmeCycleData->IsExcuteInfo = false;
			pTmeCycleData->IsNoExcuteCycle =false;
			pTmeCycleData->IsListF2B = false;
			pTmeCycleData->Isreturn = false;

			pTmeCycleData->EXE_Trans_Info.TransCmd.CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;
			pTmeCycleData->CycleType = CYCLETYPE_TRANS;
			pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_SMALLADD_HOIST_HOME;

			if(nError==NO_ERR)  {
				m_pCmdManager->AddCycle(pTmeCycleData);
				ADD_LOG_TASKCONTROL("TP_SmalladdHoistHome add cycle");
			}
			else
				delete pTmeCycleData;
			break;

		// 2018-01-02, Add by puting
        // Hoist Home 보정 Cycle 추가 구현
        case CYCLE::TP_AutoHoistHome:
			pTmeCycleData = new PKT_CYCLECMDDATA;
			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));

			pTmeCycleData->CmdID = pCmdData->CmdID;
			pTmeCycleData->IsExcuteInfo = false;
			pTmeCycleData->IsNoExcuteCycle =false;
			pTmeCycleData->IsListF2B = false;
			pTmeCycleData->Isreturn = false;

            pTmeCycleData->EXE_Trans_Info.TransCmd.CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;
			pTmeCycleData->CycleType = CYCLETYPE_TRANS;
			pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_AUTOSET_HOME;

			if(nError==NO_ERR)
				m_pCmdManager->AddCycle(pTmeCycleData);
			else
				delete pTmeCycleData;
            break;


		// 2017.01.11 added
		case CYCLE::TP_AutoMode:

			ADD_LOG_TASKCONTROL("[CYCLE::TP_AutoMode:] adding...");

			PKT_CYCLECMDDATA *pTmeCycleData;

			pTmeCycleData = new PKT_CYCLECMDDATA;
			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));

			pTmeCycleData->CmdID = pCmdData->CmdID;
			pTmeCycleData->CycleType = CYCLETYPE_TRANS;

			pTmeCycleData->IsExcuteInfo = false;
			pTmeCycleData->IsNoExcuteCycle =false;
			pTmeCycleData->IsListF2B = false;
			pTmeCycleData->Isreturn = false;

            pTmeCycleData->EXE_Trans_Info.TransCmd.CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;
			pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_AUTO_CHANGE;

			if(nError==NO_ERR)
			{
				ADD_LOG_TASKCONTROL("[TP_AutoMode] add ok");
				m_pCmdManager->AddCycle(pTmeCycleData);
			}
			else
			{
				ADD_LOG_TASKCONTROL("[TP_AutoMode] add fail");
				delete pTmeCycleData;
			}
		break;
			

		case CYCLE::CycleNotExist: 		// No Cycle
		case CYCLE::MovingToNode: 		// 이동 사이클
		case CYCLE::MovingToStation: 	// 이동 사이클
		case CYCLE::PositionSearching:  // 위치 확인 사이클(마크할 위치 정보 추가 필요) ----Driving쪽으로 뺼것..
		default:
//			 //Temp 싸이클
//			pTmeCycleData = new PKT_CYCLECMDDATA;
//			memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));
//
//			pTmeCycleData->CmdID = pCmdData->CmdID;
//			pTmeCycleData->IsExcuteInfo = false;
//			pTmeCycleData->IsListF2B = false;
//
//			pTmeCycleData->CycleType = CYCLETYPE_TRANS;
//			pTmeCycleData->EXE_Trans_Info.TransType = TRANSTYPE_LOAD;
//
//			if(nError==NO_ERR)
//				m_pCmdManager->AddCycle(pTmeCycleData);
//			else
//				delete pTmeCycleData;

			m_pExecuteInfo->FlagPreTrans.bPrePIO = false;
			m_pExecuteInfo->FlagPreTrans.bPreShutter = false;

			break;
			/*   //미사용 사이클
			 case CYCLE::Instrument: 		//게측사이클
			 case CYCLE::CycleNotExist:		//흡인구 이동 사이클
			 case CYCLE::AreaMoving: 		//동간 이동 사이클
			 case CYCLE::CycleNotExist:		//퇴피 이동 사이클
			 */

			/*
			 // ＝＝＝이하, 수동 모드시에 유효＝＝＝   상태값 표시에만사용함.
			 case CYCLE::TP_RoundRunning:		// 주회 주행중 사이클 중(미사용)
			 case CYCLE::TP_ManualOperation:		// 수동 조작 중
			 case CYCLE::TP_RunningLearning:			// 주행 학습 사이클 중
			 case CYCLE::TP_PortTeaching:			// 이재부 학습 중(티칭중)
			 case CYCLE::TP_TestPattern1:			// 테스트 사이클 중(패턴1)
			 break;
			 */

	}

	return nError;

}
	///< 조그, IO동작 등 순수 수동명령만.
int TaskControl::MakeManualCycleList(EXECUTE_MANUALCONTROL_INFO *pCmdData)
{
		int nError =NO_ERR;
		PKT_CYCLECMDDATA* pTmeCycleData = new PKT_CYCLECMDDATA;

		memset(pTmeCycleData, 0x00, sizeof(PKT_CYCLECMDDATA));

		pTmeCycleData->CmdID = 0;       //메뉴얼은 미사용
		pTmeCycleData->IsExcuteInfo = false;
		pTmeCycleData->IsNoExcuteCycle = false;
		pTmeCycleData->IsListF2B = false;
		pTmeCycleData->Isreturn = false;

		pTmeCycleData->CycleType = CYCLETYPE_MANUAL;

		pTmeCycleData->EXE_Maual_Info.uCmdTag.Data = pCmdData->uCmdTag.Data;
		memmove(&pTmeCycleData->EXE_Maual_Info.uTPCmd, &pCmdData->uTPCmd, sizeof(uTP_CMD));

		ADD_LOG_TASKCONTROL("MakeCycleList_ MANUAL_COMMON");


		if(nError==NO_ERR)
			m_pCmdManager->AddCycle(pTmeCycleData);
		else
			delete pTmeCycleData;

		return nError;


}

/**
 @brief   주행 관련 Command를 처리하는 함수
 @author  puting
 @date    2015.11.03
 @param   pCmdData : 처리해야 할 Command
 @return  정상인 경우 0을 리턴, 그렇지 않은 경우 에러 코드 리턴
 @todo    Status 상태 체크, 이전 Cmd type 체크, Path Search 하는 파라미터 등 협의 사항 미정
 */
int TaskControl::MakeGoExcuteInfo(PKT_CYCLECMDDATA *pCmdData, LList *pExeCmd)
{
	int Error = NO_ERR;
  //	UINT uStartNode = 0; // Path Search의 시작 Node
	UINT uFinalDrivingNodeOfPathSearch = 0; // Path Search의 마지막 Node
	int nListCount = 0;
	bool bFinalCheck = false;
	bool bStartCheck = false;


	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO = NULL;
	// ExcuteThread에 전달하는 구조체


		 //1 0. 초기 정보 설정
		 //2 1) 시작 Node정보 설정

		 /*
		 if((m_pOHTMainStatus->IsOnGoCmd == true) || // OnGoCmd 거나
		 (m_pExecuteInfo->StatusCmdId.Info.Status == STS_GOING) || // 주행 중이거나
		 (m_pExecuteInfo->ExecuteUnit == EXECUTE_UNIT_DRIVING && m_pExecuteInfo->ListOfCmd.getCount() > 0)) // 주행 명령이 있을 경우
		 {
			 uStartNode = m_pOHTMainStatus->FinalNodeOfPathSearch;
		 }
		 else
		 {
			 uStartNode = m_pExecuteInfo->ExecutePositionInfo.CurrNode;
		 }
		 */
		/*
		// Step 1. Driving이 가능한 상태인지를 체크
		 // Path Search로 리스트를 보내 Driving Command List를 생성
		 // SerchPath 함수 내부에서 자동으로 Add함
		*/
		 //puting 작업변경 관련해서 처리해줄것 20160521


		m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = true;
		CS_CmdList->Acquire();
        ADD_LOG_TASKCONTROL("Criticle CmdList2");
		nListCount = m_pPathSearch->GetPathCount();

		if(nListCount ==0)
		{
			Error = ERR_PATHSEARCH_NOT_COUNT;
			ADD_LOG_TASKCONTROL("GetPathCount %d", nListCount);
		}
		else
		{
			for(int Index=0; Index < nListCount; Index++)
			{

				pEXECUTE_DRIVING_INFO =(EXECUTE_DRIVING_INFO *) m_pPathSearch->ReferPath(Index);

				if(pEXECUTE_DRIVING_INFO->AddInfo.CmdType != EXECUTE_DRIVING_CMD_GOBACK)
				{
					if(bFinalCheck == true)
					{
						if((pCmdData->EXE_CycleDriving_Info.StartPoint == pEXECUTE_DRIVING_INFO->BasicInfo.StartNID) &&
						  (pCmdData->IsNoExcuteCycle == false) &&
						  (pEXECUTE_DRIVING_INFO->SQNum == pCmdData->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum))
						{
							bFinalCheck = false;
						}
						else
						{
							m_pExecuteInfo->nNextNodeIDInfo =  pEXECUTE_DRIVING_INFO->BasicInfo.EndNID;
							break;
						}


					}

					if(pCmdData->EXE_CycleDriving_Info.StartPoint == pEXECUTE_DRIVING_INFO->BasicInfo.StartNID)
						bStartCheck = true;
				}
				else
				{

					if(pCmdData->EXE_CycleDriving_Info.StartPoint == pEXECUTE_DRIVING_INFO->BasicInfo.EndNID)
						bStartCheck = true;
				}


				if((bStartCheck ==true) &&
				   (pEXECUTE_DRIVING_INFO->CmdID == pCmdData->CmdID) &&
				   (pEXECUTE_DRIVING_INFO->SQNum == pCmdData->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum))
				   {
						EXECUTE_DRIVING_INFO * TempDrivingInfo = new EXECUTE_DRIVING_INFO;

						TempDrivingInfo->PathIndexNum = pEXECUTE_DRIVING_INFO->PathIndexNum;
						TempDrivingInfo->CmdID = pEXECUTE_DRIVING_INFO->CmdID;
						TempDrivingInfo->SQNum = pEXECUTE_DRIVING_INFO->SQNum;
						TempDrivingInfo->RealPositionOfStop[0] =  pEXECUTE_DRIVING_INFO->RealPositionOfStop[0];
						TempDrivingInfo->RealPositionOfStop[1] =  pEXECUTE_DRIVING_INFO->RealPositionOfStop[1];

						memmove(&TempDrivingInfo->BasicInfo, &pEXECUTE_DRIVING_INFO->BasicInfo, sizeof(DRIVING_CMD_INFO_OF_PATH));
						memmove(&TempDrivingInfo->AddInfo, &pEXECUTE_DRIVING_INFO->AddInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));


//						ADD_LOG_TASKCONTROL("[MakeGoExcuteInfo]ExcuteGo StartID:EndID %d:%d  Add:%X -> %X %d", pEXECUTE_DRIVING_INFO->BasicInfo.StartNID, pEXECUTE_DRIVING_INFO->BasicInfo.EndNID, pEXECUTE_DRIVING_INFO, TempDrivingInfo, pEXECUTE_DRIVING_INFO->SQNum);

						 try
						 {
							pExeCmd->insertBack((void*)TempDrivingInfo);
						 }
						 catch(...)
							ADD_LOG_TASKCONTROL("Excute Exception!!!");

						  m_pOHTMainStatus->IsOnGoCmd = true; // Go 명령 인가 되면 true로 전환

						  m_pExecuteInfo->ExecuteUnit = EXECUTE_UNIT_DRIVING;


				   }


				   if((pEXECUTE_DRIVING_INFO->BasicInfo.EndNID == pCmdData->EXE_CycleDriving_Info.FinalPoint)&&
					  (pEXECUTE_DRIVING_INFO->SQNum == pCmdData->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum))
					  {
					  	m_pExecuteInfo->nNextNodeIDInfo = 0;
						bFinalCheck = true;
					  }


			}
		}

		CS_CmdList->Release();
		m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = false;
		// //1 4. ExecuteInfo의 Flag 활성화
		if (Error == NO_ERR)
		{
//				m_pPassPermit->SetSumCyclePositon((double)pCmdData->EXE_CycleDriving_Info.EndDistance);

				m_pOHTMainStatus->FinalNodeOfPathSearch = pCmdData->EXE_CycleDriving_Info.EndPoint; // 주행 중 Go 명령 추가 되면 Path Search 마지막 노드부터 시작
				m_pOHTMainStatus->FinalEndNodeOfPathSearch = pCmdData->EXE_CycleDriving_Info.FinalPoint;

				if(m_pOHTMainStatus->FinalEndNodeOfPathSearch  != m_pOHTMainStatus->FinalNodeOfPathSearch)
				{
                	m_pOHTMainStatus->FinalStationOfPathSearch = pEXECUTE_DRIVING_INFO->AddInfo.StationID;
				}
				else
					m_pOHTMainStatus->FinalStationOfPathSearch = 0;
//				m_pOHTMainStatus->FinalSeqNumOfPP = pCmdData->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum;

				ADD_LOG_TASKCONTROL("ExcuteGo %d FinalNodeOfPathSearch%d",
				pCmdData->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum,m_pOHTMainStatus->FinalNodeOfPathSearch);
			  //m_pOHTMainStatus->FinalMarkType = pCmdData.EXE_Driving_Info.AddInfo.StationType;
		}


		 //by puting 2015.11.11 Excute Thread에서 CmdID 사용용도 확인할것.
		 //m_pExecuteInfo->OnAddingCmd.Info.CmdID = pCmdData->GoCmd.CmdID;


		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// 1 1. 일반 주행 노드로의 명령인 경우

	//주행 명령 들어간 이후 일정 시간 이후에 작업변경 가능(약 300msec 이상)
//	m_dwJobChangeCheckTime = (DWORD)timeGetTime();

	return Error;
}
// ---------------------------------------------------------------------------

/**
 @brief   이적재 관련 Command를 처리하는 함수
 @author  puting
 @date    2015.11.11
 @param   pCmdData : 처리해야 할 Command
 @param   pExeCmd : Add할 Execute Command List
 @return  정상인 경우 0, 에러 발생한 경우 에러 코드 리
 */
int TaskControl::MakeTransExcuteInfo(PKT_CYCLECMDDATA *pCmdData, LList *pExeCmd)
{
	 int Error = NO_ERR;

	 EXECUTE_TRANS_INFO *tmpData = new EXECUTE_TRANS_INFO;

	 // by puting 2015.11.11 : 메모리 복사로 변경
	 memcpy(tmpData, &pCmdData->EXE_Trans_Info, sizeof(EXECUTE_TRANS_INFO));

	//Trans 관련된 내용을 List에 추가
	 m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = true;

     CS_CmdList->Acquire();
     ADD_LOG_TASKCONTROL("Criticle CmdList3");
	 if(pExeCmd->getCount() == 0)
	 {
		 pExeCmd->insertBack((void*)tmpData);
	 }
	 else
	 {
		delete tmpData;
	 }

	 m_pExecuteInfo->ExecuteUnit = EXECUTE_UNIT_TRANS;
	 CS_CmdList->Release();
   	m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = false;

	 ADD_LOG_TASKCONTROL("[MakeCmd][Trans] TransType:%x, Station ID:%X, PortType:%X, PIODirection:%d, Hoist:%d, Shift:%d, HoistLevel:%d, ShiftLevel:%d",
	 tmpData->TransType,
	 tmpData->TransCmd.CurStationID,
	 tmpData->TransCmd.PortType,
	 tmpData->TransCmd.PIODirection,
	 tmpData->TransCmd.Tposition_Hoist,
	 tmpData->TransCmd.Tposition_Shift,
	 tmpData->TransCmd.HoistSpeedLevel,
	 tmpData->TransCmd.ShiftSpeedLevel);
	 ADD_LOG_TASKCONTROL("[MakeCmd][Trans] PIOTimeLevel:%d, LookDownLevel:%d, AutoRecovery:%d, RFReader:%d, TransSound:%d, Oscillation:%d, HandDetectEQ:%d, EQPIODisable_TP:%d",
	 tmpData->TransCmd.PIOTimeLevel,
	 tmpData->TransCmd.LookDownLevel,
	 tmpData->TransCmd.TransOption.Info.AutoRecovery,
	 tmpData->TransCmd.TransOption.Info.RFReader,
	 tmpData->TransCmd.TransOption.Info.TransSound,
	 tmpData->TransCmd.TransOption.Info.Oscillation,
	 tmpData->TransCmd.TransOption.Info.HandDetectEQ,
	 tmpData->TransCmd.TransOption.Info.EQPIODisable_TP);



	return Error;

}
// ---------------------------------------------------------------------------

/**
 @brief   Common 관련 Manual를 처리하는 함수, *Teaching Update도 수행한다.
 @author  puting
 @date    2015.11.11
 @param   pCmdData : 처리해야 할 Command
 @param   pExeCmd : Add할 Execute Command List
 @return  정상인 경우 0, 에러 발생한 경우 에러 코드 리
 */
int TaskControl::MakeManualExcuteInfo(PKT_CYCLECMDDATA *pCmdData, LList *pExeCmd)
{
	int Error = NO_ERR;


	 EXECUTE_MANUALCONTROL_INFO *tmpData = new EXECUTE_MANUALCONTROL_INFO;


	 EnterCriticalSection(&OHTMainForm->m_CS);
	 m_pExecuteInfo->bJogInterlockIgnore = true;
	 LeaveCriticalSection(&OHTMainForm->m_CS);

	 memset(tmpData, 0x00, sizeof(EXECUTE_MANUALCONTROL_INFO));

	 memmove(tmpData, &pCmdData->EXE_Maual_Info, sizeof(EXECUTE_MANUALCONTROL_INFO));

	 m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = true;

     CS_CmdList->Acquire();
     ADD_LOG_TASKCONTROL("Criticle CmdList4");
	 try
	 {
		pExeCmd->insertBack((void*)tmpData);
	 }
	 __finally
	 {
//    	delete tmpData;
	 }

	 /*
			// by ehoto 2012.11.23 : 임태웅 구현 내용
		if (pExeCmd->getCount() == 0) {
			m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = true;
			pExeCmd->insertBack(tmpData);
		}
		else {
			delete tmpData;
		}
       */

	 m_pExecuteInfo->ExecuteUnit = EXECUTE_UNIT_MANUAL;
     CS_CmdList->Release();
	 m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = false;


	/*
	 // Common구조체는 Common과 Manual Type이 공유하므로 OR 체크
	 if(pCmdData->uCmdTag.Info.CmdType != TAG_CMDTYPE_TPCMD)
	 {
	 ///@todo 에러 코드 정의 필요
	 Error = ERR_TAG_CMDTYPE;
	 return Error;
	 }

	 EXECUTE_MANUALCONTROL_INFO *tmpData = new EXECUTE_MANUALCONTROL_INFO;

	 tmpData->uCmdTag = pCmdData->uCmdTag;
	 tmpData->uTPCmd.TPCmd.CmdID = pCmdData->uTP_CMD.TPCmd.CmdID;
	 tmpData->uTPCmd.TPCmd.Data1 = pCmdData->uTP_CMD.TPCmd.Data1;
	 tmpData->uTPCmd.TPCmd.Data2 = pCmdData->uTP_CMD.TPCmd.Data2;
	 tmpData->uTPCmd.TPCmd.Data3 = pCmdData->uTP_CMD.TPCmd.Data3;
	 tmpData->uTPCmd.TPCmd.Data4 = pCmdData->uTP_CMD.TPCmd.Data4;

	 // by ehoto 2012.11.23 : 임태웅 구현 내용
	 if(pExeCmd->getCount() == 0)
	 {
	 m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = true;
	 pExeCmd->insertBack(tmpData);
	 }
	 else
	 {
	 delete tmpData;
	 }

	 // by zzang9un 2013.03.20 : 현재 CmdID를 Execute로 넘겨줌
	 m_pExecuteInfo->OnAddingCmd.Info.CmdID = pCmdData->uTP_CMD.TPCmd.CmdID;

	 m_pExecuteInfo->ExecuteUnit = EXECUTE_UNIT_MANUAL;
	 m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = false;

	 }
	 */
	return Error;
}

// ---------------------------------------------------------------------------
bool TaskControl::CancelPathInfo(int ChangeTagID, int SQNum)
{
	bool bReturn = false;

	int nListCount = m_pPathSearch->GetPathCount();
	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO = NULL;

	if(nListCount !=0)
	{
		for(int Index=0; Index < nListCount; Index++)
		{
			pEXECUTE_DRIVING_INFO =(EXECUTE_DRIVING_INFO *) m_pPathSearch->PopBackPath();

			ADD_LOG_TASKCONTROL("[CancelPathInfo]ExcuteGo StartID:EndID %d:%d", pEXECUTE_DRIVING_INFO->BasicInfo.StartNID, pEXECUTE_DRIVING_INFO->BasicInfo.EndNID);

			if((ChangeTagID == pEXECUTE_DRIVING_INFO->BasicInfo.EndNID) &&
			   (SQNum==pEXECUTE_DRIVING_INFO->SQNum))
			   {
					m_pPathSearch->AddPath(pEXECUTE_DRIVING_INFO);
					bReturn = true;
               		break;
			   }


			if((ChangeTagID == pEXECUTE_DRIVING_INFO->BasicInfo.StartNID) &&
			   (SQNum==pEXECUTE_DRIVING_INFO->SQNum))
				bReturn = true;

				delete pEXECUTE_DRIVING_INFO;

			   if(bReturn == true) break;
		}
	}


	return bReturn;
}


// ---------------------------------------------------------------------------
/**
 @brief  작업변경에 대해 주행쪽에 요청하는 부분
 @author  puting
 @date    2016.06.12
 @param   Tag ID
 @return  정상인 경우 true , 실패할경우 false ( ESTOP 강제로 해줄것 초반에는.)
 */
bool TaskControl::PathChangeReq2Driving(int ChangeTagID, int SQNum, bool bJobChage)
{
	bool breturn = false;
	int nChecking = INIT;

	bool bfinishFlag = false;

	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO = NULL;

	//변경하고자 하는 tag ID를 넣고, Driiving쪽에 요청
	if((ChangeTagID == 0) || (m_pExecuteInfo->FlagPathChange.bPathChange == true))
		return breturn;

	m_pExecuteInfo->FlagPathChange.nChageNodeID =ChangeTagID;
	m_pExecuteInfo->FlagPathChange.bPathChange = true;
	m_pExecuteInfo->FlagPathChange.cResult = INIT;

	m_pExecuteInfo->bPopListOfCmd = false;

	Sleep(1); //컨택스트 스위칭

	nChecking = CheckPathChange2Driving();

	ADD_LOG_TASKCONTROL("PathChangeReq2Driving 결과 : %d TadID %d %d",nChecking, ChangeTagID, SQNum);


	if(nChecking == SUCCESS)
	{
	   breturn =true;
	}
	//경로 삭제 성공할 경우라면 남아있는게 있으면 삭제처리.
	//경로삭제 실패할 경우라면 ChangeTagID와 일치한 위치까지 삭제처리.
	m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = true;
	CS_CmdList->Acquire();
	ADD_LOG_TASKCONTROL("Criticle CmdList7");
	int nCount = m_pExecuteInfo->ListOfCmd.getCount();

	if(m_pExecuteInfo->ListOfCmd.getCount() != 0)
	{
		for(int i= 0; i<nCount; i++)
		{
			try
			{
				pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)m_pExecuteInfo->ListOfCmd.referBack();


				ADD_LOG_TASKCONTROL("PathChangeReq2Driving SQ %d, NID %d",pEXECUTE_DRIVING_INFO->SQNum, pEXECUTE_DRIVING_INFO->BasicInfo.StartNID);
//					if(pEXECUTE_DRIVING_INFO->SQNum != SQNum )
//						break;
//					else
//					{
				if(pEXECUTE_DRIVING_INFO->SQNum == SQNum)
				{
					if(pEXECUTE_DRIVING_INFO->BasicInfo.StartNID == ChangeTagID)
					{

						breturn = true;
						bfinishFlag = true;
					}

					if(pEXECUTE_DRIVING_INFO->BasicInfo.EndNID == ChangeTagID)
					{
					   breturn = true;
					   break;
					}
				}

				delete (EXECUTE_DRIVING_INFO*)m_pExecuteInfo->ListOfCmd.popBack();

				if(bfinishFlag == true) break;

//					}
			}
			catch(...)
			{
				ADD_LOG_TASKCONTROL("PathChangeReq2Driving List Exception");
			}
		}
	}
	CS_CmdList->Release();
	m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = false;
	m_pExecuteInfo->FlagPathChange.bPathChange =false;



	//최종 에러 유무
//	if(breturn ==false)
//    	m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_STOP); //실패할 경우 E-STOP으로 정지할 것. (실패할 확률 제로로 만들기.. )


	return breturn;


}

/**
 @brief  Driving쪽에 처리 결과에 대해 확인하는 부분
 @author  puting
 @date    2016.06.12
 @param   pCmdData : 처리할 Commmand
 @return  정상인 경우 0x00 체크 실패, 0x01 실패, 0x02 성공
 */
int TaskControl::CheckPathChange2Driving()
{
	int nResult = INIT;
	int ChecingCount = 0;

	if(GetExecuteStatus() == STS_GOING)
	{
		while(m_pExecuteInfo->FlagPathChange.bPathChange == true)
		{
			ChecingCount++;
			ADD_LOG_TASKCONTROL("CheckingPathChanged %d",ChecingCount);

			if(ChecingCount ==100)  //약 800msec ~ 500msec
				break;		       // return nResult;

			if(m_pExecuteInfo->ExecuteDrivingStep == DRIVING_CTRL_STEP_GO_PREPARE)
				Sleep(100); //컨택스트 스위칭
			else
				Sleep(1); //컨택스트 스위칭
		}

		nResult = m_pExecuteInfo->FlagPathChange.cResult;
	}
	else if((GetExecuteStatus() == STS_INIT) || (GetExecuteStatus() == STS_ARRIVED))
	{
		if(m_pExecuteInfo->FlagPathChange.nChageNodeID ==m_pExecuteInfo->ExecutePositionInfo.CurrNode)
		{
			nResult = SUCCESS;
		}

		m_pExecuteInfo->FlagPathChange.bPathChange =false;
		
	}
	else if((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF))
	{
		nResult = SUCCESS;
		m_pExecuteInfo->FlagPathChange.bPathChange =false;
		
	}


	return nResult;


}
/**
 @brief  cycle list에 추가하는 make하도록 분류하는 함수
 @author  puting
 @date    2015.10.15
 @param   pCmdData : 처리할 Commmand
 @return  정상인 경우 0을 리턴, 에러 발생 시 에러 코드 리턴
 */
int TaskControl::ChangeCycleList(PKT_TASKCMDDATA *pCmdData) {
	/*
	 1) 목적지 변경----미사용
	 2) 경로 변경----미사용
	 3) 작업 변경으로 구분
	 */


	// 추가 작업진행내용 확인 필요.
	// 2015 11 25
	//  추가 내용 :  OHT가 마지막 정지노드에 정지하였는지 확인 필요. 정지한 이후에 다른 명령 발생
	//  특정 플래그를 이용하여 통과허가 개방은 하되, 통과허가 요청은 하지 않는 플래그 설정 필요. 그리고 통과허가와 통과허가개방인덱스가
	//  동일할 경우, 명령 클리어

	// cycle의 종류에 분류
	//이후 모든 명령이 클리어 된 이후에 다른 명령 수행 전달.

	int nError = NO_ERR;
	PARSING_GO_INFO TempGOInfo;
	PARSING_TRANS_INFO TempTransInfo;

	UINT TempStationNID=0;


	STATION_DATA *pSData = NULL;

   	STATION_DATA OCSStationData;
	memset(&OCSStationData, 0x00, sizeof(STATION_DATA));

	memset(&TempGOInfo, 0x00, sizeof(PARSING_GO_INFO));
	memset(&TempTransInfo, 0x00, sizeof(PARSING_TRANS_INFO));

	switch (pCmdData->DetailTagSort)
	 {
		case TAGTYPE_JOBORDER_JOBCHAGE:
		case TAGTYPE_JOBORDER_TARGETCHAGE:
		case TAGTYPE_JOBORDER_PATHCHAGE:     // 작업지시 변경으로 추가됨. puting 2016 03 07
			PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121 tmpID121;
			memmove(&tmpID121, (PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121*)pCmdData->PKTDATA.Value,	sizeof(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121));

			ADD_LOG_TASKCONTROL("ChangeCycleList_JOB Order");


			ADD_LOG_TASKCONTROL("GoInfo:[Extra]%d",tmpID121.PathData_Def_Direction.Driving_Flag);

//			m_pOHTMainStatus->bAutoRecovery = false;

			// Step 1) 이동명령이 발생한 경우만 처리
			// 위치확인 사이클도 이동명령으로 취급???
			if (tmpID121.PathData_Def_Direction.Driving_Flag)
			{
				m_pOHTMainStatus->bRealGoCycleing = true;


				TempGOInfo.CmdID = tmpID121.CmdID;
				TempGOInfo.CmdCycle = tmpID121.CmdCycle;
				TempGOInfo.MarkType =  tmpID121.StationData_MarkType;


				TempGOInfo.StationData_ParentNodeID = tmpID121.StationData_ParentNodeID;
				TempGOInfo.StationData_NextNodeID = tmpID121.StationData_NextNodeID;
				TempGOInfo.StationData_RunDist = tmpID121.StationData_Offset;
				TempGOInfo.TeachingData_STBOffset = tmpID121.TeachingData_STBOffset;

				OCSStationData.StationID =  tmpID121.StationData_StationID;
				OCSStationData.StationType =  tmpID121.StationData_MarkType;
				OCSStationData.ParentNodeID =  tmpID121.StationData_ParentNodeID;
				OCSStationData.NextNodeID =  tmpID121.StationData_NextNodeID;
				OCSStationData.StationOffset =  tmpID121.StationData_Offset;
				OCSStationData.Safety_Level =  tmpID121.StationData_Safety_Level;

				TempGOInfo.PathData_Def_Direction = tmpID121.PathData_Def_Direction.Path_Direction;
				TempGOInfo.PathData_ExtraNum = tmpID121.PathData_ExtraNum;
				//tmpID121.StationData_Safety_Level = 0x01;//for test
				TempGOInfo.bIsTargetStationoPassPermitOpen = tmpID121.StationData_Safety_Level;  //puting 2017 08 03

				for (int i = 0; i < 3; i++)
				{
					// Node 비트연산   puting
					int Temp1 = tmpID121.StopNodeID[i];
					int Temp2 = tmpID121.StopStationID[i];
//					int Tmep3 = tmpID121.NextNodeID[i];
					int Tmep4 = tmpID121.PathData_StartNodeID[i];
					int Tmep5 = tmpID121.PathData_EndNodeID[i];
//					int Temp6 = tmpID121.StationData_Start_NextNodeID[i];


					if (HIGH_BYTE == 0) {
						TempGOInfo.StopNodeID += Temp1 << ((2 - i) * 8);
						TempGOInfo.StopStationID +=	Temp2 << ((2 - i) * 8);
//						TempGOInfo.NextNodeID += Tmep3<< ((2 - i) * 8);
						TempGOInfo.PathData_StartNodeID += Tmep4 << ((2 - i) * 8);
						TempGOInfo.PathData_EndNodeID += Tmep5 << ((2 - i) * 8);
//						TempGOInfo.StationDataStart_NextNodeID += Temp6 << ((2 - i) * 8);

					}
					else
					{
						TempGOInfo.StopNodeID += Temp1 << (i * 8);
						TempGOInfo.StopStationID +=	Temp2 << (i * 8);
//						TempGOInfo.NextNodeID += Tmep3<< (i * 8);
						TempGOInfo.PathData_StartNodeID += Tmep4 << (i * 8);
						TempGOInfo.PathData_EndNodeID += Tmep5 << (i * 8);
//						TempGOInfo.StationDataStart_NextNodeID += Temp6 << (i * 8);

					}

				}

				for (int j = 0; j < TempGOInfo.PathData_ExtraNum; j++) {
					for (int i = 0; i < 3; i++) {

						// Extra Node 비트연산   puting
						int Temp = tmpID121.PathData_ExtraID[i + (j * 3)];

						if (HIGH_BYTE == 0)
							TempGOInfo.PathData_ExtraID[j] += Temp << ((2 - i) * 8);
						else
							TempGOInfo.PathData_ExtraID[j] += Temp << (i * 8);

					}
				}

				ADD_LOG_TASKCONTROL("GoInfo:[Extra]Num:%d,First:%d,DefaulDirection:%d",
				TempGOInfo.PathData_ExtraNum,TempGOInfo.PathData_ExtraID[0], TempGOInfo.PathData_Def_Direction);


				//최종 위치 저장
				m_pExecuteInfo->nFinishOfNode = TempGOInfo.StopNodeID;
				m_pExecuteInfo->nFinishOfStation = TempGOInfo.StopStationID;

				TempStationNID =  TempGOInfo.StopStationID;

				if (GetCurrentMode() != MODE_MANUAL)
				{
					if(OCSStationData.StationID != 0)
					{
						pSData = OHTMainForm->GetStationDataManager()->Find(OCSStationData.StationID);

						if (pSData == NULL)
						{
							OHTMainForm->GetStationDataManager()->MemoryInsert(&OCSStationData);
							ADD_LOG_TASKCONTROL("[NotMatch VHL<->OCS StationData], StationID:%06d", OCSStationData.StationID);
						}
					}

				}

                //메모리에 갱신
				// Step 2) 이적재 명령이 있으면 넣기
				if(nError==NO_ERR)
					nError = ChangeGoCycleList(&TempGOInfo);

				if((nError ==NO_ERR) && (GetCurrentMode() == MODE_AUTO))
				{
					if(TempGOInfo.StopStationID !=0)
					{
						m_OCSStaionInfo.StationID = TempGOInfo.StopStationID;
						m_OCSStaionInfo.StationType = TempGOInfo.MarkType;
						m_OCSStaionInfo.ParentNodeID = TempGOInfo.StationData_ParentNodeID;
						m_OCSStaionInfo.NextNodeID = TempGOInfo.StationData_NextNodeID;
						m_OCSStaionInfo.Offset = TempGOInfo.StationData_RunDist;
						m_OCSStaionInfo.Safety_Level = TempGOInfo.bIsTargetStationoPassPermitOpen;
					}
					else
					{
						memset(&m_OCSStaionInfo, 0x00, sizeof(NODE_INFO_STD_STATION));
					}
				}
			}


			// Step 3)  이동명령 이후 마지막에 추가될 명령을 구분하여 저장 	puting 2015.11.03
			if(nError==NO_ERR)
			{
				TempTransInfo.CmdID = tmpID121.CmdID;
				TempTransInfo.CmdCycle = tmpID121.CmdCycle;

				TempTransInfo.TransCmd.CmdID = tmpID121.CmdID;


				TempTransInfo.TransCmd.BufferType = tmpID121.BufferType.Buffer_1;
			    TempTransInfo.AutoRecovery = tmpID121.AutoRecovery;

				for (int i = 0; i < 3; i++)
				{
					int Temp1 = tmpID121.StopNodeID[i];
					int Tmep6 = tmpID121.StopStationID[i];

					if (HIGH_BYTE == 0)
					{
						TempTransInfo.TransCmd.CurNodeID += Temp1 << ((2 - i) * 8);
						TempTransInfo.TransCmd.CurStationID += 	Tmep6 << ((2 - i) * 8);
					}
					else
					{
						TempTransInfo.TransCmd.CurNodeID += Temp1 << (i * 8);
						TempTransInfo.TransCmd.CurStationID += 	Tmep6 << (i * 8);
                    }

				}

				TempStationNID =  TempTransInfo.TransCmd.CurStationID;

				TempTransInfo.TransCmd.PortType =  tmpID121.TeachingData_PortType; //EQ, STB 등
				TempTransInfo.TransCmd.Tposition_Hoist = tmpID121.TeachingData_Hoist;
				TempTransInfo.TransCmd.Tposition_Rotate = tmpID121.TeachingData_Rotate;
				TempTransInfo.TransCmd.Tposition_Shift = tmpID121.TeachingData_Shift;
				TempTransInfo.TransCmd.PIOID  = tmpID121.TeachingData_PIOID;
				//TempTransInfo.TransCmd.PIOCS  = tmpID121.TeachingData_PIOID;


				if(tmpID121.TeachingData_PIODirect == ON)
					TempTransInfo.TransCmd.PIODirection =LEFT_SIDE;
				else
					TempTransInfo.TransCmd.PIODirection =RIGHT_SIDE;

				TempTransInfo.TransCmd.PIOChannel = tmpID121.TeachingData_PIOChannel;


				TempTransInfo.TransCmd.PIOTimeLevel = tmpID121.TeachingData_PIOTimeLevel;
				TempTransInfo.TransCmd.LookDownLevel = tmpID121.TeachingData_LookDownLevel;
				TempTransInfo.TransCmd.CarrierType = CARRIER_FOUP;

				TempTransInfo.TransCmd.PIOCS = 0;

				TempTransInfo.TransCmd.TransOption.Info.AutoRecovery = tmpID121.TeachingData_AutoRecovery;
				TempTransInfo.TransCmd.TransOption.Info.HandDetectEQ =tmpID121.TeachingData_HandDetectEQ;

				TempTransInfo.TransCmd.ShiftSpeedLevel = 100;
				TempTransInfo.TransCmd.HoistSpeedLevel = 100;

				nError = MakeTransCycleList(&TempTransInfo);

			}
			// 층간이동은 사양 결정 필요
			//현재 cycle 등록
			if(m_pCmdManager->GetCycleCount() ==0)
			{
				ADD_LOG_TASKCONTROL("CheckTaskSatus Init");
				InitClearCycle();
				InitExcuteOverRun();
				ChangeCmdIDAndCycle(0,0,0,true);
			}else
			{
				m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag = false;
				m_pOHTMainStatus->JobChangeFlag.StartStationID = 0;
				m_pOHTMainStatus->JobChangeFlag.dChangDistance = 0.0;

				InitExcuteOverRun();

				if(TempTransInfo.TransCmd.CurStationID !=0)
					ChangeCmdIDAndCycle(tmpID121.CmdCycle,tmpID121.CmdID, TempTransInfo.TransCmd.CurStationID);
				else
					ChangeCmdIDAndCycle(tmpID121.CmdCycle,tmpID121.CmdID, TempTransInfo.TransCmd.CurNodeID);
			}

			break;


	default:
		break;

	}


	return nError;


}
/**
 @brief  cycle list에 추가하는 make하도록 분류하는 함수
 @author  puting
 @date    2015.10.15
 @param   pCmdData : 처리할 Commmand
 @return  정상인 경우 0을 리턴, 에러 발생 시 에러 코드 리턴
 */

void TaskControl::InitClearCycle(bool bcheckManual)
{

    //상위쪽에 명령 클리어되는 시점은?
	// 현재 Cycle List의 명령들 삭제, 마지막 CycleList는 수행..
	// 다른처리부분도 여기서 처리할 것.

	m_pPassPermit->InitPassPermit();

	m_pPassPermit->SetPassPermitTime(timeGetTime());

	m_pOHTMainStatus->dOHTTotalRunDis = 0.0;

	m_pExecuteInfo->nFinishOfNode = 0;
	m_pExecuteInfo->nFinishOfStation = 0;

	m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = true;
	m_pExecuteInfo->FlagInitPositon.bInitFlag = false;
	m_pExecuteInfo->dCompensatedPosition = 0.0;
	m_pExecuteInfo->FlagInitPositon.dDrivingInitPosition= 0.0;
	m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = false;
	m_pExecuteInfo->FlagMTLInfo.bTakeoutCycle = false;
	m_pExecuteInfo->FlagRunningPositon = false;

	m_pExecuteInfo->FlagPathChange.bPathChange =false; //경로 변경 유무 확인 플래그
	m_pExecuteInfo->FlagPathChange.nChageNodeID =0; // 경로 변경 노드 위치
	m_pExecuteInfo->FlagPathChange.cResult= 0x00;		 ///< 경로 변경 결과. //0 초기화, 1 성공, 2 실패
	m_pExecuteInfo->bPopListOfCmd = false;


	m_pExecuteInfo->FlagPreTrans.bPreShutter =false;
	m_pExecuteInfo->FlagPreTrans.bPrePIO =false;
	m_pExecuteInfo->FlagPreTrans.nTargetID= 0;
	m_pExecuteInfo->FlagPreTrans.nPIOID= 0;
	m_pExecuteInfo->FlagPreTrans.nPIOChannel= 0;
	m_pExecuteInfo->FlagPreTrans.cPortType= 0;

	m_pExecuteInfo->nNextNodeIDInfo = 0; //경로정보 초기화

//	m_pExecuteInfo->bCIDIgnor = false;
	m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute = false;
	m_pExecuteInfo->FlagCmdExcute.bCmdDrivingCMDExcute = false;

	m_pOHTMainStatus->bCheckCycleRunning = false;

	m_pOHTMainStatus->bCheckRollbacking = false;
	m_pOHTMainStatus->bRealGoCycleing = false;
	m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag = false;
	m_pOHTMainStatus->JobChangeFlag.StartStationID = 0;
	m_pOHTMainStatus->JobChangeFlag.dChangDistance = 0.0;
	m_pOHTMainStatus->bCycleing = false;

	m_pPassPermit->InitPassOpenList();  //통과허가 관련내용 초기화.
	m_pCmdManager->InitCycleList();
	m_pPathSearch->InitPathList();

    m_bCheckPassOpenListCount = false;

	if(bcheckManual == false)
	{
		CS_CmdList->Acquire();
		ADD_LOG_TASKCONTROL("Criticle CmdList6");
		m_pExecuteInfo->ListOfCmd.clear();
    	CS_CmdList->Release();
		ADD_LOG_TASKCONTROL("List Clear");
	}


}
void TaskControl::InitExcuteOverRun()
{
	m_pExecuteInfo->FlagOverRunInfo.bCheck = false;
	m_pExecuteInfo->FlagOverRunInfo.OverrunType = 0x00;
	m_pExecuteInfo->FlagOverRunInfo.nFinalNode = 0;

	m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute = false;
	m_pExecuteInfo->FlagCmdExcute.bCmdDrivingCMDExcute = false;
}


/**
 @class   Checkcyclelist 통과허가개방 및 응답에 대한 함수
 @date    2015.10.05
 @author  puting
 @brief   TaskControl Class
 */
int TaskControl::CheckCycleList(PKT_TASKCMDDATA *pCmdData)
{

	int nError = NO_ERR;
	int TmepError = NO_ERR;
  //	bool bCheckFlag = false;
	PKT_CYCLECMDDATA* pTempCmdData;
	PKT_CYCLECMDDATA * pTempRollBackPPCmd; // Command를 위한 포인터 변수

	DATA_PASSOPEN   *pTempDataPOCmd;
	DATA_PASS 		* pPasspermit;

	///< Cycle List에 값과 비교할 임시 구조체
	PROTOCOL_BODY_RESPONSE_PASSPERMIT_OCS2OHT_ID_242 Temp242;
	PASSPERMIT_COMMON TempPassPermit;
	FORCE_PASSPERMIT_COMMON TempForcePassPermit;



	int nCheckCountCycleList = m_pCmdManager->GetCycleCount();
	int TempIndex = m_pPassPermit->GetPassOpenCount();
	int TempPassIndex = m_pPassPermit->GetPassPermitCount();

	// Step 1 통과허가 개방 or 통과허가 요청 등을 공통 부분에 저장.

	ADD_LOG_TASKCONTROL("CheckCycleList Start %d",nCheckCountCycleList);

//	memmove(&TempPassPermit,(PASSPERMIT_COMMON*)pCmdData->PKTDATA.Value, sizeof(PASSPERMIT_COMMON));

	// Step 2 Cycle List에 저장된 값 확인 후, 변경
	if (nCheckCountCycleList != LISTCOUNT_EMPTY)
	 {
			switch (pCmdData->DetailTagSort)
			{
				///< 1.통과허가응답인 경우
				case TAGTYPE_PASSPERMIT_RESPONSE:

					memmove(&Temp242,(PROTOCOL_BODY_RESPONSE_PASSPERMIT_OCS2OHT_ID_242*)pCmdData->PKTDATA.Value, sizeof(PROTOCOL_BODY_RESPONSE_PASSPERMIT_OCS2OHT_ID_242));


					pTempCmdData = (PKT_CYCLECMDDATA*)m_pCmdManager->ReferCycle(m_pPassPermit->GetPPIndex());


					ADD_LOG_TASKCONTROL("[Check]통과허가 요청  EXE_CycleDriving_Info SQ:%d, STOP %x Reset %x ResetSub %x  Index %d",
					Temp242.PassPermit_CM.SQNum,
					Temp242.PassPermit_CM.StopNodeID[0],
					Temp242.PassPermit_CM.ResetNodeID_Main[0],
					Temp242.PassPermit_CM.ResetNodeID_Sub[0],
					m_pPassPermit->GetPPIndex());


				    ///< 1.통과허가응답
					// memmove(TempPassPermit, pCmdData->PKTDATA.Value, sizeof(PASSPERMIT_COMMON));
					//Case1. Next Link에 대한 통과허가가 났을 경우. //  수정 필요
					if ((pTempCmdData->IsExcuteInfo ==false) &&
//						(m_pOHTMainStatus->bCheckRollbacking  ==false) &&
						(m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag == false) &&
						(pTempCmdData->EXE_CycleDriving_Info.D_PPStep ==  CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE) &&
						(memcmp(&pTempCmdData->EXE_CycleDriving_Info.CurLinkPassPermitData, &Temp242.PassPermit_CM, sizeof(PASSPERMIT_COMMON)) == 0))
						// Data가 동일 할 경우
					{
						if(m_pOHTMainStatus->bCheckRollbacking  ==false)
						{
							pTempCmdData->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;


						//통과허가 상태 저장
							pPasspermit = new DATA_PASS;

							memmove(&(pPasspermit->PassInfo),&(Temp242.PassPermit_CM), sizeof(PASSPERMIT_COMMON));

							if(Temp242.ResultType == 0x04)
								pPasspermit->bCIDType = 0x01;
							else
								pPasspermit->bCIDType = 0x00;

							if(m_pPassPermit->GetPassPermitCount() != LISTCOUNT_EMPTY)
							{
								DATA_PASS  * pTempDataPass;
								pTempDataPass = (DATA_PASS *)m_pPassPermit->ReferFirstPassPermit();

								if(memcmp(&Temp242.PassPermit_CM, &pTempDataPass->PassInfo, sizeof(PASSPERMIT_COMMON)) != 0)
								{
									ADD_LOG_TASKCONTROL("[Check]통과허가 요청 -CASE2 %d %d %d",pTempDataPass->PassInfo.SQNum,pTempDataPass->PassInfo.StopNodeID[0],pTempDataPass->PassInfo.ResetNodeID_Main[0]);
									m_pPassPermit->AddPassPermit(pPasspermit);
								}
								else
                                   	ADD_LOG_TASKCONTROL("[Check]통과허가 요청 -중복 응답에 대한 처리");
							}else
							{
								m_pPassPermit->AddPassPermit(pPasspermit);
								ADD_LOG_TASKCONTROL("[Check]통과허가 요청 -CASE1");
							}

							memset(&m_pCurrCIDUSE, 0x00, sizeof(FLAG_CID_NOT_USE));

						}
						else
						{
                            //예외 케이스
							if(m_pPassPermit->GetPassOpenCount() != LISTCOUNT_EMPTY)
							{
								DATA_PASSOPEN * pTempDataPassOpen;
								pTempDataPassOpen = (DATA_PASSOPEN *)m_pPassPermit->ReferFirstPassOpen();

								if(memcmp(&Temp242.PassPermit_CM, &pTempDataPassOpen->PassOpneInfo, sizeof(PASSPERMIT_COMMON)) != 0)
								{
									DATA_PASSOPEN * pDataPassOpen;
									pDataPassOpen = new DATA_PASSOPEN;
									pDataPassOpen->PassOpenIndex = 0;
									pDataPassOpen->bPassOpenType = JOBCHAGE_RETURN_TYPE;
									memmove(&pDataPassOpen->PassOpneInfo, &Temp242.PassPermit_CM,sizeof(PASSPERMIT_COMMON));
									m_pPassPermit->AddPassOpen(pDataPassOpen);
								}
							}else
							{
									DATA_PASSOPEN * pDataPassOpen;
									pDataPassOpen = new DATA_PASSOPEN;
									pDataPassOpen->PassOpenIndex = 0;
									pDataPassOpen->bPassOpenType = JOBCHAGE_RETURN_TYPE;
									memmove(&pDataPassOpen->PassOpneInfo, &Temp242.PassPermit_CM,sizeof(PASSPERMIT_COMMON));
									m_pPassPermit->AddPassOpen(pDataPassOpen);
							}
						}

//						m_pPassPermit->SetIncreasePPCount();



					//	bCheckFlag = true;
					}
					else
					{
						ADD_LOG_TASKCONTROL("[Pass]통과허가 요청   EXE_CycleDriving_Info SQ:%d",Temp242.PassPermit_CM.SQNum)
                    	memset(&m_pCurrCIDUSE, 0x00, sizeof(FLAG_CID_NOT_USE));
					}

					break;
                ///< 2.통과개방인 경우
				case TAGTYPE_PASSPERMIT_OPEN_RESPONSE:

					memmove(&TempPassPermit,(PASSPERMIT_COMMON*)pCmdData->PKTDATA.Value, sizeof(PASSPERMIT_COMMON));

					pTempRollBackPPCmd = (PKT_CYCLECMDDATA*)m_pCmdManager->ReferCycle(m_pPassPermit->GetPPIndex());
					///< 통과허가개방응답


					ADD_LOG_TASKCONTROL("[Check]개방요청  TempPassPermit SQ:%d, STOP %x Reset %x ResetSub %x",
					TempPassPermit.SQNum,
					TempPassPermit.StopNodeID,
					TempPassPermit.ResetNodeID_Main,
					TempPassPermit.ResetNodeID_Sub);     				

					if(TempIndex!= LISTCOUNT_EMPTY)
					{
						try{
							for(int Index = 0; Index < TempIndex; Index++)
							{
								pTempDataPOCmd  = (DATA_PASSOPEN *)m_pPassPermit->ReferPassOpen(Index);
	//								ADD_LOG_TASKCONTROL("Check]통과허가개방요청  Step0");


								if(pTempDataPOCmd->bPassOpenType ==NORMAL_TYPE)
								{
									//통과허가개방 일 경우,
									if(nCheckCountCycleList > pTempDataPOCmd->PassOpenIndex)
									{
										pTempCmdData = (PKT_CYCLECMDDATA*)m_pCmdManager->ReferCycle(pTempDataPOCmd->PassOpenIndex);
									}
									else
									{
										ADD_LOG_TASKCONTROL("Check]개방요청  Exception %d:%d",nCheckCountCycleList,pTempDataPOCmd->PassOpenIndex );
										return nError;
									}

									if ((pTempCmdData->IsListF2B == false) &&
	//									(m_pOHTMainStatus->bCheckRollbacking == false) &&
	//									(pTempCmdData->IsExcuteInfo == true)  &&
										(pTempCmdData->EXE_CycleDriving_Info.D_PPOStep == NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE) &&
										(pTempCmdData->EXE_CycleDriving_Info.D_PPStep >= CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE) &&
										(memcmp(&pTempCmdData->EXE_CycleDriving_Info.CurLinkPassPermitData, &TempPassPermit, sizeof(PASSPERMIT_COMMON)) == 0))
									// Data가 동일 할 경우.
									{
									 //	ADD_LOG_TASKCONTROL("Check]통과허가개방요청  Step1");
											if(pTempCmdData->IsExcuteInfo ==true)
											{

											// 통과개방 완료
												pTempCmdData->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_OPEN_RESPONSE;
												pTempCmdData->IsListF2B = true;

												m_pPassPermit->DeletePassOpen(Index);

												m_pPassPermit->SetPassPermitTime(timeGetTime());

												break;

											}

										}

									}
									else if(pTempDataPOCmd->bPassOpenType == JOBCHAGE_RETURN_TYPE)  //작업변경 및 반납시
									{
										//2. 통과개방을 통해 통과허가 등록된 부분 제거
										if(memcmp(&pTempDataPOCmd->PassOpneInfo, &TempPassPermit, sizeof(PASSPERMIT_COMMON)) == 0)
										{
											m_pPassPermit->DeletePassOpen(Index);

											if(TempPassIndex!= LISTCOUNT_EMPTY)
											{
												for (int i = 0; i < TempPassIndex; i++)
												{
													pPasspermit = (DATA_PASS *)m_pPassPermit->ReferPassPermit(i);

													if(memcmp(&(pPasspermit->PassInfo), &TempPassPermit, sizeof(PASSPERMIT_COMMON)) == 0)
													  {
															ADD_LOG_TASKCONTROL("Check]작업변경/예외_개방요청  Step");
															Sleep(1);
															m_pPassPermit->DeletePassPermit(i);
															break;
													  }
												}
											}
											break;
										}
									}

								   Sleep(1);
								}
							}catch(...)
							{
								ADD_LOG_TASKCONTROL("[exception]개방요청 Index:%d SQNum:%d", TempIndex,pTempCmdData->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum);
							}

					} 
					break;
			}
	}
	else
	{
			ADD_LOG_TASKCONTROL("[CheckCycleList]LISTCOUNT_EMPTY");
//		nError = 100;
	  //Error 처리? 아니면 강재통과허가 개방으로 처리?
	}

	
	
	return nError;
}


/**
 @class   투입 빼냄에 의해 내부 Cmd 생성하는 함수
 @date    2015.11.17
 @author  puting
 @brief   TaskControl Class
 */
void TaskControl::MakeOHTInCmd(BYTE Type)
{
		PKT_TASKCMDDATA pTempCmdPktData;
		PKT_OCS2OHT TempRcvData;
		PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121  	 CmdTempID121;      //Cmd

		memset(&pTempCmdPktData, 0x00, sizeof(PKT_TASKCMDDATA));
		memset(&CmdTempID121, 0x00, sizeof(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121));

		TempRcvData.TAG.Value = TagID::JOB_ORDER;
		pTempCmdPktData.CategoryDiv =CATEGORY_SECONDARY;
		pTempCmdPktData.DetailTagSort = TAGTYPE_JOBORDER;
		CmdTempID121.CmdID=0xFFFFF;
		CmdTempID121.CmdDiv.CMDType = ORDERDIV::JobOrder;

		STATION_DATA StationData;
		STATION_DATA StationDataED;

		NODE_INFO_STD_PATHDATA Pathdata;
		int TmepNode = 0;
		int TempEndNode= 0;
		int TempEndStation= 0;
		int nError = NO_ERR;
		BYTE TempDiv = 0x00;

		bool bBackMove = false;


		if(Type==SUCCESS) //투입
		{
				CmdTempID121.CmdCycle = CYCLE::CycleNotExist;  ///CYCLE::CycleNotExist;   //뺴냄 이동중 사이클_싸이클 없음 OCS와 협의 완료

				if(m_pExecuteInfo->ExecutePositionInfo.CurrStation != 0)
				{

					nError = OHTMainForm->GetPathDataManager_STD()->GetPathData(OHTMainForm->GetPathData_STD(),&Pathdata, m_pExecuteInfo->ExecutePositionInfo.CurrStation);
					if(nError == NO_ERR)
					{
						 if((Pathdata.LeftNodeIDIn == m_pExecuteInfo->ExecutePositionInfo.CurrStation)  ||
							(Pathdata.LeftNodeIDMiddle == m_pExecuteInfo->ExecutePositionInfo.CurrStation))
							{
								TempDiv = 0; //왼쪽분기
								bBackMove  = Pathdata.LeftDriveDirection;

								if(bBackMove)
									TempEndStation =  Pathdata.LeftNodeIDIn;
								else
									TempEndStation =  Pathdata.LeftNodeIDOut;
							}
						 else
						 {
								TempDiv = 1; //오른쪽 분기
								bBackMove  = Pathdata.RightDriveDirection;

								if(bBackMove)
									TempEndStation =  Pathdata.RightNodeIDIn;
								else
									TempEndStation =  Pathdata.RightNodeIDOut;
						 }
					}

					ADD_LOG_TASKCONTROL("[Insert]------투입Check  %d %d %d %d %d E:%d", Pathdata.LeftNodeIDIn, Pathdata.LeftNodeIDMiddle, Pathdata.LeftDriveDirection, TempEndStation, TempDiv,nError);
				}
				else
				{
						return;
				}

				if((nError == NO_ERR) && (Pathdata.LayoutType == 0x01))
				{

					GetStationData(&StationData, m_pExecuteInfo->ExecutePositionInfo.CurrStation);
					GetStationData(&StationDataED, TempEndStation);

					CmdTempID121.PathData_Def_Direction.Path_Direction =  TempDiv;
					CmdTempID121.PathData_Def_Direction.Driving_Flag = 0x01;
					CmdTempID121.StationData_MarkType = StationDataED.StationType;
					CmdTempID121.StationData_Offset = StationDataED.StationOffset;
					CmdTempID121.StationData_Safety_Level = StationDataED.Safety_Level;


					CmdTempID121.StationData_StationID = StationDataED.StationID;
					CmdTempID121.StationData_ParentNodeID = StationDataED.ParentNodeID;
					CmdTempID121.StationData_NextNodeID = StationDataED.NextNodeID;
					CmdTempID121.StationData_Offset = StationDataED.StationOffset;
					CmdTempID121.StationData_Safety_Level = StationDataED.Safety_Level;


					m_OCSStaionInfo.StationID = StationDataED.StationID;
					m_OCSStaionInfo.StationType = StationDataED.StationType;
					m_OCSStaionInfo.ParentNodeID = StationDataED.ParentNodeID;
					m_OCSStaionInfo.NextNodeID = StationDataED.NextNodeID;
					m_OCSStaionInfo.Offset = StationDataED.StationOffset;
					m_OCSStaionInfo.Safety_Level = StationDataED.Safety_Level;

					TmepNode =	StationData.ParentNodeID;
					TempEndNode = StationDataED.ParentNodeID;

					for(int Index =0; Index< 3; Index++)
					{
						CmdTempID121.PathData_StartNodeID[2-Index] = (TmepNode >> (Index * 8))&0xFF;
						CmdTempID121.PathData_EndNodeID[2-Index] = CmdTempID121.StopNodeID[2-Index] = (TempEndNode >> (Index * 8))&0xFF;
						CmdTempID121.StopStationID[2-Index] =  (TempEndStation >> (Index * 8))&0xFF;
					}
					ADD_LOG_TASKCONTROL("[Insert]------투입Check_Go %d %d %d", StationData.ParentNodeID,StationDataED.ParentNodeID, bBackMove);

					memmove(&(TempRcvData.Value), &CmdTempID121, sizeof(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121));
					memmove(&(pTempCmdPktData.PKTDATA), &TempRcvData, sizeof(PKT_OCS2OHT));
					m_pCmdManager->MakeTaskList_Standard(&pTempCmdPktData);

				}



		}



}

/**
 @class   투입 빼냄 보고 요청하는 함수
 @date    2015.11.17
 @author  puting
 @brief   TaskControl Class
 */
void TaskControl::JudgeOHTInOutReq()
{
	 //실제 코드 구현필요 부분
	 int TmepCheckTakeOut =INIT;
	 int TmepCheckTakeIn =INIT;
	 BYTE TmepCheckRailNum =INIT;

	 int  nListCount =  m_pCmdManager->GetCycleCount();
	 PKT_CYCLECMDDATA * pCurCmd; // Command를 위한 포인터 변수
	 PKT_CYCLECMDDATA * pTempCmd; // Command를 위한 포인터 변수


	 TmepCheckTakeOut = m_pExecuteInfo->FlagMTLInfo.nTakeOut;
	 TmepCheckTakeIn = m_pExecuteInfo->FlagMTLInfo.nTakeIn;
	 TmepCheckRailNum = (BYTE)m_pExecuteInfo->FlagMTLInfo.nRailNum;

	 if(TmepCheckTakeIn == REQ_INSERT)   //투입 OK
	 {

	   SendOHTInReq(TmepCheckRailNum);

//	   if(TmepCheckRailNum ==0x01)
//			m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.Rail_1_InputReq = ON;
//	   else if(TmepCheckRailNum ==0x02)
//			m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.Rail_2_InputReq = ON;


	   ADD_LOG_TASKCONTROL("[Insert]------투입 요구 OK");

	   EnterCriticalSection(&OHTMainForm->m_CS);
	   m_pExecuteInfo->FlagMTLInfo.nTakeIn = INIT;
	   m_pExecuteInfo->FlagMTLInfo.nRailNum = INIT;
	   LeaveCriticalSection(&OHTMainForm->m_CS);

	 }
//	 else if(TmepCheckTakeIn == FAIL)   //투입 NG
//	 {
//			ADD_LOG_TASKCONTROL("[Insert]------투입 NG");
//			EnterCriticalSection(&OHTMainForm->m_CS);
//			m_pExecuteInfo->FlagMTLInfo.nTakeIn = INIT;
//			m_pExecuteInfo->FlagMTLInfo.nRailNum = INIT;
//			LeaveCriticalSection(&OHTMainForm->m_CS);
//     }


	 if((nListCount == LISTCOUNT_EMPTY) || (m_pOHTMainStatus->CurCycle != CYCLE::TakeOut))
		return;


	 pCurCmd = (PKT_CYCLECMDDATA*) m_pCmdManager->ReferCycle(m_pPassPermit->GetPPIndex());

	 if(pCurCmd->IsNoExcuteCycle == true)
	 {
//        ADD_LOG_TASKCONTROL("[Out]------배출 Check");

		EnterCriticalSection(&OHTMainForm->m_CS);
		if(m_pExecuteInfo->FlagMTLInfo.bTakeoutCycle == false)
		{
			m_pExecuteInfo->FlagMTLInfo.bTakeoutCycle = true;
			m_pExecuteInfo->FlagMTLInfo.nTakeOut = INIT;
		}
		LeaveCriticalSection(&OHTMainForm->m_CS);


		 if(TmepCheckTakeOut ==SUCCESS)   //배출 OK
		 {
			ADD_LOG_TASKCONTROL("[Out]------배출 OK");
		//  	MakeOHTInOutCmd(0x01);    //빼냄으로 인한 내부 이동 명령 생성  싸이클 정의 필요.

			for(int i=m_pPassPermit->GetPPIndex(); i< nListCount; i++)
			{
				pTempCmd = (PKT_CYCLECMDDATA*) m_pCmdManager->ReferCycle(i);

				if(pTempCmd->IsNoExcuteCycle == true)
					pTempCmd->IsNoExcuteCycle=false;
			}

		   //	m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.TakeOutOK = ON;

			SendOHTOutReq();

			EnterCriticalSection(&OHTMainForm->m_CS);
			m_pExecuteInfo->FlagMTLInfo.nTakeOut = COMPLETE;
			LeaveCriticalSection(&OHTMainForm->m_CS);
		 }else if(TmepCheckTakeOut ==FAIL)   //배출  NG
		 {
			 ADD_LOG_TASKCONTROL("[Out]------배출 NG");
//			 m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.TakeOutNG = ON;

			//작업 초기화
			InitClearCycle();
			InitExcuteOverRun();
			ChangeCmdIDAndCycle(0,0,0,true);

			SendOHTDefReq();  //주회주행 요청

			EnterCriticalSection(&OHTMainForm->m_CS);
			m_pExecuteInfo->FlagMTLInfo.nTakeOut = COMPLETE;
//			m_pExecuteInfo->FlagMTLInfo.bTakeoutCycle = false;
			LeaveCriticalSection(&OHTMainForm->m_CS);


		 }
	 }



}
/**
 @class   CID 에러 확인하는 함수
 @date    2015.11.17
 @author
 @brief   TaskControl Class
 */
void TaskControl::JudgeCIDReportAndIgnor()       //CID 이상유무 판단.
{
	int nCountCycle = m_pCmdManager->GetCycleCount();
	int nCurNode = m_pExecuteInfo->ExecutePositionInfo.CurrNode;
	int nCurOffset = m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset;


	if(nCountCycle != LISTCOUNT_EMPTY)
	{
		 if(m_pExecuteInfo->FlagCIDInfo.bCIDErrorFlag ==true)
		 {
			char Temp[3];
			int CheckCIDError = 0;
			for(int i=0; i< 3; i++)
			{
				//AOHC-196 사전 로그 추가
				Temp[i] = m_pExecuteInfo->FlagCIDInfo.sCIDStatus[i];
				if(Temp[i] == 0x30)     // 수정 필요"0"
					CheckCIDError++;
			}

			if(CheckCIDError != 3)
			{
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_CID_ERROR_ST);
				ADD_MD_LOG("CID_ERROR_ST Cur N:%d,Offset:%d(%x%x%x)",nCurNode,nCurOffset,Temp[0],Temp[1],Temp[2]);

			}
			SendCIDError(m_pExecuteInfo->FlagCIDInfo.cCIDStateResult,Temp);

			EnterCriticalSection(&OHTMainForm->m_CS);
			m_pExecuteInfo->FlagCIDInfo.bCIDErrorFlag = false;
			LeaveCriticalSection(&OHTMainForm->m_CS);
		 }
	 }


	 ////CID 무시 판단
	 CIDInfoOfPassPermit();
 	 if(m_pListCIDUSE.nStopNodeID != 0)
	 {
		if((m_pExecuteInfo->FlagCIDNotuse.nStopNodeID !=m_pListCIDUSE.nStopNodeID) ||
		  (m_pExecuteInfo->FlagCIDNotuse.bCIDUse !=m_pListCIDUSE.bCIDUse)||
		  (m_pExecuteInfo->FlagCIDNotuse.nResetNodeID !=m_pListCIDUSE.nResetNodeID))
		{
			ADD_LOG_TASKCONTROL("CID 무시1 : %d %d %d",m_pListCIDUSE.nStopNodeID,m_pListCIDUSE.bCIDUse,m_pListCIDUSE.nResetNodeID);
			EnterCriticalSection(&OHTMainForm->m_CS);
			m_pExecuteInfo->FlagCIDNotuse.nStopNodeID = m_pListCIDUSE.nStopNodeID;
			m_pExecuteInfo->FlagCIDNotuse.bCIDUse = m_pListCIDUSE.bCIDUse;
			m_pExecuteInfo->FlagCIDNotuse.nResetNodeID = m_pListCIDUSE.nResetNodeID;
			LeaveCriticalSection(&OHTMainForm->m_CS);
		}
	 }
	 else
	 {
		if((m_pExecuteInfo->FlagCIDNotuse.nStopNodeID !=m_pCurrCIDUSE.nStopNodeID) ||
		  (m_pExecuteInfo->FlagCIDNotuse.bCIDUse !=m_pCurrCIDUSE.bCIDUse)||
		  (m_pExecuteInfo->FlagCIDNotuse.nResetNodeID !=m_pCurrCIDUSE.nResetNodeID))
		{
            ADD_LOG_TASKCONTROL("CID 무시2 : %d %d %d",m_pCurrCIDUSE.nStopNodeID,m_pCurrCIDUSE.bCIDUse,m_pCurrCIDUSE.nResetNodeID);
			EnterCriticalSection(&OHTMainForm->m_CS);
			m_pExecuteInfo->FlagCIDNotuse.nStopNodeID = m_pCurrCIDUSE.nStopNodeID;
			m_pExecuteInfo->FlagCIDNotuse.bCIDUse = m_pCurrCIDUSE.bCIDUse;
			m_pExecuteInfo->FlagCIDNotuse.nResetNodeID = m_pCurrCIDUSE.nResetNodeID;
			LeaveCriticalSection(&OHTMainForm->m_CS);
		}
	 }

//	 ADD_LOG_TASKCONTROL("CID Stop Reset : %d %d",
//	 m_pExecuteInfo->FlagCIDNotuse.nStopNodeID,m_pExecuteInfo->FlagCIDNotuse.nResetNodeID);
}
 /**
 @class   진척보고 요청하는 함수
 @date    2015.11.17
 @author  puting
 @brief   TaskControl Class
 */
void TaskControl::JudgeProgressReq()       //진척보고 요청
{
     //임시코드이며 내용 정리 필요.
	if(m_pOHTMainStatus->StandardOHTPreProgress != m_pOHTMainStatus->StandardOHTProgress)
	{
		SendProgressReq(m_pOHTMainStatus->StandardOHTProgress);
		m_pOHTMainStatus->StandardOHTPreProgress = m_pOHTMainStatus->StandardOHTProgress;
	}
	else if(m_pOHTMainStatus->StandardOHTProgress == m_pOHTMainStatus->StandardOHTPreProgress)
	{
		switch(GetExecuteStatus())
		{
		case STS_INIT:
			m_pOHTMainStatus->StandardOHTProgress = PROGRESS::NotProgress;
		break;

		case STS_GOING:
			m_pOHTMainStatus->StandardOHTProgress = PROGRESS::DrivingBegin;
		break;

		case STS_ARRIVED:
			if(m_pOHTMainStatus->bRealGoCycleing == false)
				m_pOHTMainStatus->StandardOHTProgress = PROGRESS::Arrived;
		break;

		case STS_UNLOADING:
		case STS_LOADING:
			m_pOHTMainStatus->StandardOHTProgress = PROGRESS::TransBegin;
		break;

		case STS_UNLOADED:
		case STS_LOADED:
			m_pOHTMainStatus->StandardOHTProgress = PROGRESS::TransCompleted;
		break;

		case STS_AUTOPOSITION:
		case STS_RUNNING:
		case STS_COMPLETE:
			break;
		case STS_AUTORECOVERY:
			m_pOHTMainStatus->StandardOHTProgress = PROGRESS::E84Timeout;
			break;
		case STS_AUTORECOVERY_BLANKTRANS:
			m_pOHTMainStatus->StandardOHTProgress = PROGRESS::EmptyShipment;
			break;
		case STS_AUTORECOVERY_DOUBLETRANS:
			m_pOHTMainStatus->StandardOHTProgress = PROGRESS::DoubleInsert;
			break;
		case STS_ERROR:
		default:
			break;
		}
	}

	/*
	NotProgress				= 0x00,	// 초기 상태
	DrivingBegin	 		= 0x01,	// 이동 개시
	Arrived					= 0x02, // 도착
	TransBegin				= 0x03, // 이재 개시
	TransCompleted			= 0x04, // 이재 완료
	PositionCheckCompleted 	= 0x05, // 위치 확인 완료

	InpositionOverRun  		= 0x08, // 정위치 오버런
	E84Timeout				= 0x09,	// E84 Timeout
	DoubleInsert			= 0x0A, // 이중반입
	EmptyShipment	     	= 0x0B, // 공출하.
	E84InterlockCompleted	= 0x0C, // E84 Interlock 완료
*/


}


/**
@class   데이터 전송지시 진척보고 하는 함수.(맵 파일 적용 후 완료에 대한 보고만 수행함.)
@date    2016.08.31
@author  KJD
@brief   TaskControl Class
*/
void TaskControl::SendDataProgressReport(BYTE SendProgressSuccess, BYTE SendProgressFail)
{
	PKT_ACK_OHT2OCS AckPkt;
	PROTOCOL_BODY_DATA_TRANSPER_PROGRESS_OHT2OCS  TempDataProgress;


	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));
	memset(&TempDataProgress, 0x00, sizeof(PROTOCOL_BODY_DATA_TRANSPER_PROGRESS_OHT2OCS));
 
	AckPkt.AckPkt.TAG.Value = TagID::REPORT_DATATRANSFERPROGRESS_OHT2OCS;
	 
	m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataSuccess = SendProgressSuccess;
	m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataFail = SendProgressFail;
	TempDataProgress.Result.Trans_OK = SendProgressSuccess;
	TempDataProgress.Result.Trans_Fail = SendProgressFail; 

	TempDataProgress.Result.Not_Accept_Class = 0x00;//0x00:데이터 전송처리가 정상적으로 완료
	TempDataProgress.Result.TransOrderID = m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID;

	TempDataProgress.Request.CMDType = g_CMDType;
	TempDataProgress.Request.TestMode = 0x00;//0x00:FTP 서버상의 파일을 VHL에 전송한다.

	if(SendProgressFail)
	{
		TempDataProgress.FailClass = 0xDDDD; //임시 적용 실패에 대한 내용 추가 필요
	}
	else
	{
		TempDataProgress.FailClass = 0x00;
	}

	 ADD_LOG_TASKCONTROL("데이터 전송지시 Complete : %d %d %d %d",m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.DataTransStatus.DataCmdID,
															  SendProgressSuccess, SendProgressFail, g_CMDType);

	 memmove(&AckPkt.AckPkt.Value, &TempDataProgress, sizeof(PROTOCOL_BODY_DATA_TRANSPER_PROGRESS_OHT2OCS));
	 AckPkt.Length = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);
  
	 MakeResponseStandardOHT2OCS(&AckPkt);
}
 /**
 @class   맵 업데이트의 상태를 판단하여여 진척보고 (완료) 하는 함수.
 @date	  2016.08.31
 @author  KJD
 @brief   TaskControl Class
 */
 void TaskControl::JudgeMAPUpdate()
 {
	 if(OHTMainForm->g_DataTransProgressStatus == DATATRANSFERPROGRESS_ACK_TRIGGER_ON)
	 {
		 if(OHTMainForm->g_MAPUpdateStatus == DATA_UPDATE_SUCCESS)
		 {
			 SendDataProgressReport(1, 0);//성공,실패 값 같이 넣어줌.
			 OHTMainForm->g_DataTransProgressStatus = DATATRANSFERPROGRESS_ACK_INIT;
		     OHTMainForm->g_MAPUpdateStatus = DATA_UPDATE_INIT;
		 }
		 else if(OHTMainForm->g_MAPUpdateStatus == DATA_UPDATE_FAIL)
		 {
			 SendDataProgressReport(0, 1);//성공,실패 값 같이 넣어줌.
			 OHTMainForm->g_DataTransProgressStatus = DATATRANSFERPROGRESS_ACK_INIT;
		 	 OHTMainForm->g_MAPUpdateStatus = DATA_UPDATE_INIT;
		 }
		 else
		 {
		 //NOP
		 }
		 
	 }
 }

 /**
 @class   진척보고 하는 함수.
 @date    2015.11.17
 @author  puting
 @brief   TaskControl Class
 */
void TaskControl::SendProgressReq(BYTE SendProgress)
{
	PKT_ACK_OHT2OCS AckPkt;
	PROTOCOL_BODY_JOB_PROGRESS_REPORT_CLW_TO_MCP_ID_103 TempID103;


	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));
	memset(&TempID103, 0x00, sizeof(PROTOCOL_BODY_JOB_PROGRESS_REPORT_CLW_TO_MCP_ID_103));

	AckPkt.AckPkt.TAG.Value = TagID::JOB_PROGRESS_REPORT;

	TempID103.RcvCmdID = m_pOHTMainStatus->CurCmdID;
	TempID103.ProgressType = SendProgress;
//	TempID103.MoveDist = m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.RunningDistance;


	//현재상태값으로 가지고 있어야되는 값
	//현재 사이클 상태 : 지시아이디값.

	memmove(&AckPkt.AckPkt.Value, &TempID103, sizeof(PROTOCOL_BODY_JOB_PROGRESS_REPORT_CLW_TO_MCP_ID_103));
	AckPkt.Length = sizeof(PKT_JOB_PROGRESS_REPORT_ID_103);

	ADD_LOG_TASKCONTROL("JudgeProgressReq PreProgress:%X, CurProgress:%X, Length%d",
	m_pOHTMainStatus->StandardOHTPreProgress , m_pOHTMainStatus->StandardOHTProgress,AckPkt.Length);

	MakeResponseStandardOHT2OCS(&AckPkt);


}

 /**
 @class   투입 보고 하는 함수.
 @date    2015.11.17
 @author  puting
 @brief   TaskControl Class
 */
void TaskControl::SendOHTInReq(BYTE SendData)
{
	PKT_ACK_OHT2OCS AckPkt;
	PROTOCOL_BODY_OHT_INPUT_REQUEST TempOHTInReq;


	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));
	memset(&TempOHTInReq, 0x00, sizeof(PROTOCOL_BODY_OHT_INPUT_REQUEST));

    //Rail 번호
	TempOHTInReq.RailNumber = SendData;

	if(TempOHTInReq.RailNumber == 0x01)
    	m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.Rail_1_InputReq  = ON;
	else if(TempOHTInReq.RailNumber == 0x02)
		m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.Rail_2_InputReq  = ON;


	//현재상태값으로 가지고 있어야되는 값
	//현재 사이클 상태 : 지시아이디값.
    AckPkt.AckPkt.TAG.Value = TagID::CMD_INSERTREQ_OHT_OCS;
	memmove(&AckPkt.AckPkt.Value, &TempOHTInReq, sizeof(PROTOCOL_BODY_OHT_INPUT_REQUEST));
	AckPkt.Length = sizeof(PKT_OHT_INPUT_REQUEST);

//	ADD_LOG_TASKCONTROL("JudgeProgressReq PreProgress:%X, CurProgress:%X, Length%d",
//	m_pOHTMainStatus->StandardOHTPreProgress , m_pOHTMainStatus->StandardOHTProgress,AckPkt.Length);

	MakeResponseStandardOHT2OCS(&AckPkt);


}

/**
 @class   빼냄 보고 하는 함수.
 @date    2015.11.17
 @author  puting
 @brief   TaskControl Class
 */
void TaskControl::SendOHTOutReq()
{
	PKT_ACK_OHT2OCS AckPkt;
	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));


	m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.TakeOutOK  = ON;

	//현재상태값으로 가지고 있어야되는 값
	//현재 사이클 상태 : 지시아이디값.
	AckPkt.AckPkt.TAG.Value = TagID::CMD_TAKEOUTREQ_OHT_OCS;
	AckPkt.Length = sizeof(PKT_TAKEOUT_REPORT_ID_17);


	MakeResponseStandardOHT2OCS(&AckPkt);


}


/**
 @class   주회주행 요청 하는 함수.
 @date    2015.11.17
 @author  puting
 @brief   TaskControl Class
 */
void TaskControl::SendOHTDefReq()
{
	PKT_ACK_OHT2OCS AckPkt;
	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));

	 m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.MaintananceState.TakeOutNG = ON;

	//현재상태값으로 가지고 있어야되는 값
	//현재 사이클 상태 : 지시아이디값.
	AckPkt.AckPkt.TAG.Value = TagID::CMD_DEF_REQ_OHT_OCS;
	AckPkt.Length = sizeof(PKT_DEF_REQ_ID_19);


	MakeResponseStandardOHT2OCS(&AckPkt);


}

/**
 @class   CID 에러 요청 하는 함수.
 @date    2015.11.17
 @author  puting
 @brief   TaskControl Class
 */
void TaskControl::SendCIDError(BYTE cCIDStateResult, char* sCIDStatus)
{
	PKT_ACK_OHT2OCS AckPkt;
	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));

	PROTOCOL_BODY_CID_ERROR_REPORT_OHT2OCS_ID_249  TempCIDError;


	PKT_CYCLECMDDATA * pCurCmd; // Command를 위한 포인터 변수
	pCurCmd = (PKT_CYCLECMDDATA*) m_pCmdManager->ReferCycle(m_pPassPermit->GetPPOIndex());

	memmove(&TempCIDError.PassPermit_CM,&pCurCmd->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON));
	if(cCIDStateResult == CID_MONITOR_INFO_GET_STATUS || cCIDStateResult == CID_MONITOR_INFO_GET_SUCCESS)
		{ TempCIDError.CIDResultType = CID_MONITOR_INFO_GET_SUCCESS_249; }  // ID:249 0:취득 성공 1: 취득실패
	else
		{ TempCIDError.CIDResultType = CID_MONITOR_INFO_GET_FAIL_249; } // ID:249 0:취득 성공 1: 취득실패
	memmove(&TempCIDError.CIDStatus,sCIDStatus, sizeof(TempCIDError.CIDStatus));

	//현재상태값으로 가지고 있어야되는 값
	//현재 사이클 상태 : 지시아이디값.
	AckPkt.AckPkt.TAG.Value = TagID::CID_ERROR_REPORT_OHT2OCS;
	memmove(&AckPkt.AckPkt.Value, &TempCIDError, sizeof(PROTOCOL_BODY_CID_ERROR_REPORT_OHT2OCS_ID_249));
	AckPkt.Length = sizeof(PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249);

	ADD_LOG_TASKCONTROL("[CID] CID Error Info (ID:249) -> to OCS Sended.");
	MakeResponseStandardOHT2OCS(&AckPkt);


}

/**
 @class   대차감지에 의한 통과허가 요청 유무 확인 함수.
 @date    2017.05.14
 @author  puting
 @brief   TaskControl Class
*/
bool TaskControl::IsOHTDetectCheckPassPermit(double dRestDistance)
{
	bool breturn = true;
	//대차감지에 의해 통과허가 요청유무 확인
	//직선일떄만 우선 적용하여 확인
	//단계별 디파인 추가.
	//  5         7      6       2          3        1     없음
	// 400mm    800mm  1850mm  2900mm     3950mm     5000mm

    //곡선도 적용 가능함?? 곡선은 해당 내용 적용시 만족하는 사항이 없음.(그냥 써도됨.)

	static int Templevel = 0;

    static DWORD dwPassPermitCheckTime = timeGetTime();     //함수 진입 시간.

	//감지 Level에 따른 무시 유무


	 //정지노드에 정차한 경우는 무조건 true
	if((GetExecuteStatus() == STS_ARRIVED) || (GetExecuteStatus() == STS_INIT))
	{
		//정지한  경우는 무조건 true
	}
	else
	{
		//정지가 아닐 경우
		switch(m_pHWSet->m_pOHTDetect->GetStatusLevel())
		{
			 case 1:      //  실제 정지 단계로부터 (5500~4700) ~ (3950~3600)mm 떨어진 거리.
			 if(dRestDistance > 5500.0)     //남은거리가 6500이상 남았으나 1단계 감지된경우, 센서 오차가 크므로 간격이 큼.  (3.3m/s 이하는 해당안됨.)
				breturn = false;
			 break;
			 case 3:    //  실제 정지 단계로부터  (3950~3600) ~(2900~2600)mm 떨어진 거리.
			 if(dRestDistance > 4000.0)     //남은거리가 5000이상 남았으나 3단계 감지된경우
				breturn = false;
			 break;

			 case 2:  //  실제 정지 단계로부터 (2900~2600)~ (1750~1400)mm 떨어진 거리.
			  if(dRestDistance > 3000.0)     //남은거리가 4000이상 남았으나 3단계 감지된경우
				breturn = false;
			 break;

			 case 6:  // 실제 정지 단계로부터  (1750~1400) ~(480~400)mm 떨어진 거리.
				if(dRestDistance > 1800.0)     //남은거리가 2500이상 남았으나 3단계 감지된경우
					breturn = false;
			 break;

	//		 case 7:
	//	 		if(dRestDistance > 1500.0)     //남은거리가 1500이상 남았으나 3단계 감지된경우
	//				breturn = false;
	//		 break;

			 case 7:  // 실제 정지 단계로부터 (480~400)~ (121~21)mm 떨어진 거리.
			 case 5:  //정지 단계는 요청하지 않음.
			   //	if(dRestDistance > 1000.0)     //남은거리가 1000이상 남았으나 3단계 감지된경우
					breturn = false;
			 break;

		}
	 }


	//거리 마진이 충분할 경우 요청 주기 확인
	 if(breturn == true)
	 {
//		 if(dRestDistance > 6000)
//		 {
//			if((timeGetTime()-dwPassPermitCheckTime)  < 300)
//				breturn = false;
//		 }
		 if(dRestDistance > 5000.0)
		 {
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwPassPermitCheckTime)  < ((m_pPathSearch->GetPassPermitPriod()*3)+ m_pPassPermit->GetPassPeriodTime()))  // 응답불가일 경우 주기 횟수 변경됨
				breturn = false;
		 }
//		 else if(dRestDistance > 4000)
//		 {
//            if((timeGetTime()-dwPassPermitCheckTime)  < 150)
//               	breturn = false;
//		 }
		 else if(dRestDistance > 3500.0)
		 {
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwPassPermitCheckTime)  < ((m_pPathSearch->GetPassPermitPriod()*2)+ m_pPassPermit->GetPassPeriodTime()))  // 응답불가일 경우 주기 횟수 변경됨
				breturn = false;
		 }
		 else if(dRestDistance > 2000.0)
		 {
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwPassPermitCheckTime) < (m_pPathSearch->GetPassPermitPriod()+ m_pPassPermit->GetPassPeriodTime()))  // 응답불가일 경우 주기 횟수 변경됨
				breturn = false;
		 }
//		 else if(dRestDistance > 1000)
//		 {
//
//		 }
		 //else  무조건 주기대로 요청.

		 if(breturn == true)
			 dwPassPermitCheckTime = timeGetTime();

	 }


	if(Templevel != m_pHWSet->m_pOHTDetect->GetStatusLevel())
	{
//		ADD_LOG_TASKCONTROL("OHTDectetLevel:%d Distance:%6.11f",m_pHWSet->m_pOHTDetect->GetStatusLevel(),dRestDistance);
		Templevel = m_pHWSet->m_pOHTDetect->GetStatusLevel();
	}


	return breturn;

}

///**
// @brief   통과허가 요청해주는 부분.
// @author  puting
// @date    2015.11.14
// @param   pCmdData : 처리할 Commmand
// @return  통과허가 요청할 게 있을 경우, 1 리턴
// */
 void TaskControl::JudgePassPermitReq()     //통과허가 및 개방 요청 판단하는 부분
 {

		int nCountCycle = m_pCmdManager->GetCycleCount();
		int nTempIndex = m_pPassPermit->GetPassOpenCount();
		int TempPPOIndex = 0;
		bool bRollback = false;
		bool bIsRetureTypePPO = false;   //puting 반납시, 통과개방유무 확인하는 부분
		int nError =NO_ERR;

		DATA_PASS 		* pPasspermit = NULL;

		PKT_ACK_OHT2OCS AckPkt;
		PKT_CYCLECMDDATA * pCurPPCmd; // Command를 위한 포인터 변수
		PKT_CYCLECMDDATA * pCurPPOCmd; // Command를 위한 포인터 변수
		PKT_CYCLECMDDATA * pTempPPOCmd; // Command를 위한 포인터 변수
		DATA_PASSOPEN *    pTempPOCmdData;  // Pass Open 확인을 위한 포인터 변수

//		bool bOHTPause = false;   //임시 전역으로 뺴기

//		PKT_CYCLECMDDATA * pTempRollBackPPCmd; // Command를 위한 포인터 변수

		static DWORD dwRollBackTime = timeGetTime();     //함수 진입 시간.
		static DWORD dwRollBackExceptionTime = timeGetTime();     //함수 진입 시간.
		DWORD dwStartTime = timeGetTime();

		PROTOCOL_BODY_PASSPERMIT_OHT2OCS_ID_241 TempPassPermit;
		memset(&TempPassPermit, 0x00, sizeof(PROTOCOL_BODY_PASSPERMIT_OHT2OCS_ID_241));
		memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2OCS));



		if(nTempIndex!= LISTCOUNT_EMPTY)
		{
			for(int Index = 0; Index< nTempIndex; Index++)
			{
			   pTempPOCmdData  = (DATA_PASSOPEN *)m_pPassPermit->ReferPassOpen(Index);

			   if((pTempPOCmdData->bPassOpenType ==NORMAL_TYPE) || (pTempPOCmdData->bPassOpenType ==JOBCHAGE_RETURN_TYPE))
			   {
				   //통과허가개방인 경우

				   if(pTempPOCmdData->bPassOpenType ==JOBCHAGE_RETURN_TYPE)
				   		bIsRetureTypePPO  = true;    //반납완료 처리 유무

				   AckPkt.AckPkt.TAG.Value = TagID::CMD_PASSPERMITOPEN_OHT2OCS;
				   memmove(&AckPkt.AckPkt.Value, &pTempPOCmdData->PassOpneInfo, sizeof(PASSPERMIT_COMMON));
				   AckPkt.Length = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);
				   MakeResponseStandardOHT2OCS(&AckPkt);

					ADD_LOG_TASKCONTROL("Judge 개방요청, PassOpenIndex:%d SQNum%d : Type:%d",
					pTempPOCmdData->PassOpenIndex,
					pTempPOCmdData->PassOpneInfo.SQNum,
					pTempPOCmdData->bPassOpenType);
			   }
//			   else if(pTempPOCmdData->bPassOpenType ==FORCE_TYPE)  // question mark by hjh
//			   {
//					  //강제통과허가개방인 경우 -> 일반통과허가 개방으로 변경
//
//				     AckPkt.AckPkt.TAG.Value = TagID::CMD_PASSPERMITOPEN_OHT2OCS;
//				     memmove(&AckPkt.AckPkt.Value, &pTempPOCmdData->PassOpneInfo, sizeof(PASSPERMIT_COMMON));
//				     AckPkt.Length = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);
//				     MakeResponseStandardOHT2OCS(&AckPkt);
//
//					 ADD_LOG_TASKCONTROL("Judge 강제개방요청,");
//
//			   }
			   Sleep(1);

			}
		}


		if(nCountCycle != LISTCOUNT_EMPTY)
		{

			 pCurPPCmd = (PKT_CYCLECMDDATA*)m_pCmdManager->ReferCycle(m_pPassPermit->GetPPIndex());
			 pCurPPOCmd = (PKT_CYCLECMDDATA*)m_pCmdManager->ReferCycle(m_pPassPermit->GetPPOIndex());


			//Step 1. 통과허가 룰백 부분   --- 롤백하면 통과허가 및 개방 요청은 하지 않는다.
			//정상 -> 롤백(1차 완료)
			 //AMC 일시정지하기.
			 /// 롤백해야되는 조건 사양 결정 필요. 상위쪽에 문의
			 //기동 OFF || PBS 감지 등에 의해 롤백으로 인하여 대기 중인지 확인 -----추가 확인 진행 필요.

               if((((((m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_Detect ==ON) ||(m_pOHTMainStatus->StandardStatusOHT.PauseStatus.LastOHTDetect ==ON) ||  (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OBS_Detect ==ON))) &&         //OBS Detect 상태 //OHT Detect 상태
				 ((m_pExecuteInfo->ExecuteDrivingStep >= DRIVING_CTRL_STEP_GO_PREPARE)&& (m_pExecuteInfo->ExecuteDrivingStep <=DRIVING_CTRL_STEP_GO_RUN))) ||
				(GetExecuteStatus() == STS_ERROR) ||
				(m_pExecuteInfo->FlagCIDReport.bCIDInterlock == true) ||  //CID 인터락 추가.
				(GetExecuteStatus() == STS_AUTORECOVERY_OVERRUN) ||     //오버런 시 조건 추가.
				((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF))))  //기동 Off일 경우 자동에서
				{

				//1)반납은 진행하지 않고, 통과허가 요청만 하지않을 경우
				if(((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF))||(((m_pOHTMainStatus->StandardStatusOHT.PauseStatus.LastOHTDetect ==ON) || (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_Detect ==ON))
				 && (GetExecuteStatus() != STS_ARRIVED) && (GetExecuteStatus() != STS_INIT)))
					bRollback =true;

				//1-1)반납은 진행 및  통과허가 요청만 하지않을 경우(대차의 경우는 빨리 반납하기위해 들어감.)	
				if(m_pOHTMainStatus->bIsOHTStoped == true) // || (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_Detect ==ON))
				{
					//예외적으로 빠져나갔을 경우 Check하는 부분
					  bRollback =true;
					  dwRollBackExceptionTime  = timeGetTime();
//                      bRollback =true;

					if((OHTUTILITY::PreventOverflowGetTimeMs(dwRollBackTime) > m_pPathSearch->GetPassPermitRollbackTime()) &&
					   (m_pOHTMainStatus->bCheckRollbacking == false)) //200msec 이후에롤백 시도.
					{
						m_pOHTMainStatus->bCheckRollbacking = true; //롤백 개시.


						//1. 통과허가를 받지 않은 상태라도  대차 감지전 일 경우 대비하여 통과허가 개방을 1최 진행함.
						if((pCurPPCmd->EXE_CycleDriving_Info.D_PPStep == CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE) &&
						(pCurPPCmd->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint == true)&&
						((double)pCurPPCmd->EXE_CycleDriving_Info.StopDistance > 0.0) &&
//						IsOHTDetectCheckPassPermit((((double)pCurPPCmd->EXE_CycleDriving_Info.StartDistance - (double)m_pPassPermit->GetAbsolutePositon()) - (double)m_pOHTMainStatus->dOHTTotalRunDis)) &&
						(m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag == false) &&  //작업지시 변경으로 추가 puting 20160307
						(((double)pCurPPCmd->EXE_CycleDriving_Info.StopDistance - (double)m_pPassPermit->GetAbsolutePositon()) <= (double)m_pOHTMainStatus->dOHTTotalRunDis))
						{
                          	AckPkt.AckPkt.TAG.Value = TagID::CMD_PASSPERMITOPEN_OHT2OCS;
							memmove(&AckPkt.AckPkt.Value, &pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON));
							AckPkt.Length = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);
							MakeResponseStandardOHT2OCS(&AckPkt);

							ADD_LOG_TASKCONTROL("RollBack_Return")
						}

						ADD_LOG_TASKCONTROL("RollBack_개시");

					}

					if(m_pOHTMainStatus->bCheckRollbacking == true)
					{
						if((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF))
						{
							m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning = OFF;     //보고용
							//기동 off 처리시 조향버전
							int OHTDiv  =0;
							if((m_pHWSet->m_pSteer->IsLeftPos(FRONT) == true) && (m_pHWSet->m_pSteer->IsLeftPos(REAR) == true))
								OHTDiv =0x01;
								if((m_pHWSet->m_pSteer->IsRightPos(FRONT) == true) && (m_pHWSet->m_pSteer->IsRightPos(REAR) == true))
								OHTDiv =0x02;

							int NextNode = m_pPathSearch->GetNextNode(OHTMainForm->GetMap(), m_pExecuteInfo->ExecutePositionInfo.CurrNode, OHTDiv);

							//고민해볼것 조향에 의해서 볼지.
							if(m_pExecuteInfo->nCheckDivInfo ==PATH_DIVERGENCE_NONE)
								m_pExecuteInfo->nCheckDivInfo = m_pPathSearch->GetStationDivInfo(OHTMainForm->GetMap(),m_pExecuteInfo->ExecutePositionInfo.CurrNode,NextNode);
						}
					}
				}
				else
				{
						dwRollBackTime = timeGetTime();
						 //예외처리 부분
						if(m_pOHTMainStatus->bCheckRollbacking  ==true)
						{
							//반납처리 및 개방처리가 완료되었을 경우만 롤백 해지 처리
							if((nTempIndex == LISTCOUNT_EMPTY) || (bIsRetureTypePPO==false))   // || 특수케이스가 없는경우에 (알반적인 케이스는 제외)
							{
								if(OHTUTILITY::PreventOverflowGetTimeMs(dwRollBackExceptionTime) >  m_pPathSearch->GetPassPermitRollbackFinshTime())
								{
									ADD_LOG_TASKCONTROL("RollBack_해지_1");
									m_pOHTMainStatus->bCheckRollbacking  = false;
									m_pPassPermit->SetRollBackFinish(false);
									return;
								}
							}
						}

				}

			}
			else
			{

				dwRollBackTime = timeGetTime();

				//예외처리 부분
				if(m_pOHTMainStatus->bCheckRollbacking  ==true)
				{
					//반납처리 및 개방처리가 완료되었을 경우만 롤백 해지 처리
					if((nTempIndex == LISTCOUNT_EMPTY) || (bIsRetureTypePPO==false))   // || 특수케이스가 없는경우에 (알반적인 케이스는 제외)
					{
						if(OHTUTILITY::PreventOverflowGetTimeMs(dwRollBackExceptionTime) >  m_pPathSearch->GetPassPermitRollbackFinshTime())
						{
							ADD_LOG_TASKCONTROL("RollBack_해지_2");
							m_pOHTMainStatus->bCheckRollbacking  = false;
							m_pPassPermit->SetRollBackFinish(false);
							return;
						}
					}
				}


			}

			//2. 반납 처리하는 부분
			if(m_pOHTMainStatus->bCheckRollbacking  ==true)
			{
				//통과허가 개방을 해야되는게 없을 경우 반납 처리 시도함.
				if((nTempIndex == LISTCOUNT_EMPTY) || (bIsRetureTypePPO==false)) // || 특수케이스가 없는경우에 (알반적인 케이스는 제외)
				{
					//반납 처리.
					if((m_pPassPermit->GetPPIndex() > (m_pPassPermit->GetPPOIndex())) &&
							(bRollback ==true) &&
	//						(pCurPPCmd->IsRollBack == false) &&
							(((pCurPPCmd->CycleType == CYCLETYPE_DRIVING) &&(pCurPPCmd->EXE_CycleDriving_Info.D_PPStep == CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE)) ||
							 ((pCurPPCmd->CycleType == CYCLETYPE_DRIVING) &&(pCurPPCmd->EXE_CycleDriving_Info.D_PPStep == CYCLESTEP_DRIVING_MEANINGLESS) && (m_pPassPermit->GetPPIndex() !=0)) ||
							  (pCurPPCmd->CycleType == CYCLETYPE_TRANS)))
					{


						//주행일경우 인덱스가 뒤로 안가게 할경우와 뒤로 갈경우 분류해서 처리할 방안 검토해볼것.    puting 2018.3.24 //안하는 케이스만 추가해줄것 .
						//넣을지 뺄지 좀더 고민해볼것..puting  1) 통과허가 없는 구간인 경우에 현재거리와 노드가 넘어가지 않는 경우는 즉시 반납 회수 처리
						if(pCurPPCmd->CycleType == CYCLETYPE_DRIVING)
						{
							if(pCurPPCmd->EXE_CycleDriving_Info.D_PPStep == CYCLESTEP_DRIVING_MEANINGLESS)
							{
								if((m_pExecuteInfo->ExecutePositionInfo.CurrNode != pCurPPCmd->EXE_CycleDriving_Info.StartPoint) &&
								   (m_pExecuteInfo->ExecutePositionInfo.CurrPreCheckNode != pCurPPCmd->EXE_CycleDriving_Info.StartPoint) &&
								  ((double)((double)pCurPPCmd->EXE_CycleDriving_Info.StartDistance - (double)m_pPassPermit->GetAbsolutePositon()-m_pPathSearch->GetPassPermitRollbackMargin()) >= (double)m_pOHTMainStatus->dOHTTotalRunDis))
								{
										if(pCurPPCmd->IsExcuteInfo ==true)
										{

											bool bCheckPoint = false;
											bCheckPoint = PathChangeReq2Driving(pCurPPCmd->EXE_CycleDriving_Info.StartPoint, pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum);
											if(bCheckPoint == false)
											{
												if((GetCurrentMode() == MODE_AUTO) &&
												((GetExecuteStatus()== STS_TURN_OFF) || (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF)))
												{
															//NOF
												}
												else
													m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_STOP); //실패할 경우 E-STOP으로 정지할 것. (실패할 확률 제로로 만들기.. )
											}
											m_pOHTMainStatus->FinalNodeOfPathSearch = pCurPPCmd->EXE_CycleDriving_Info.StartPoint;
											m_pOHTMainStatus->FinalEndNodeOfPathSearch = pCurPPCmd->EXE_CycleDriving_Info.StartPoint;
											m_pOHTMainStatus->FinalStationOfPathSearch = 0;
											pCurPPCmd->IsExcuteInfo = false;

										}

                                    nError = m_pPassPermit->SetDecreasePPIndex(); //통과허가 Check 시작
									ADD_LOG_TASKCONTROL("CycleList 반납 처리  PP Index:%d %d %d %d",m_pPassPermit->GetPPIndex(),pCurPPCmd->EXE_CycleDriving_Info.D_PPStep,nError, pCurPPCmd->CycleType);

								}
							}
							else
							{
                              	nError = m_pPassPermit->SetDecreasePPIndex(); //통과허가 Check 시작
								ADD_LOG_TASKCONTROL("CycleList 반납 처리 1 PP Index:%d %d %d %d",m_pPassPermit->GetPPIndex(),pCurPPCmd->EXE_CycleDriving_Info.D_PPStep,nError, pCurPPCmd->CycleType);

							}
						}
						else
						{
							nError = m_pPassPermit->SetDecreasePPIndex(); //통과허가 Check 시작
							ADD_LOG_TASKCONTROL("CycleList 반납 처리 2 PP Index:%d %d %d %d",m_pPassPermit->GetPPIndex(),pCurPPCmd->EXE_CycleDriving_Info.D_PPStep,nError, pCurPPCmd->CycleType);

						}
					}
				   //	2) 통과허가를 받은 경우에 대해서 반납이 필요한 경우에 반납.
					else if((m_pPassPermit->GetPPIndex() >= (m_pPassPermit->GetPPOIndex())) &&
					   (bRollback ==true) &&
					   (pCurPPOCmd->IsListF2B == false) &&
					   (pCurPPCmd->EXE_CycleDriving_Info.D_PPStep == CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE) &&
					   ((pCurPPCmd->EXE_CycleDriving_Info.D_PPOStep == NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE) || (pCurPPCmd->EXE_CycleDriving_Info.D_PPOStep == NEXTCYCLESTEP_DRIVING_MEANINGLESS)) &&
					   (pCurPPCmd->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint == true)&&
					   (m_pExecuteInfo->ExecutePositionInfo.CurrNode != pCurPPCmd->EXE_CycleDriving_Info.StartPoint) &&
					   (((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF))||(m_pExecuteInfo->ExecutePositionInfo.CurrPreCheckNode != pCurPPCmd->EXE_CycleDriving_Info.StartPoint)) &&
					   (((double)((double)pCurPPCmd->EXE_CycleDriving_Info.StartDistance - (double)m_pPassPermit->GetAbsolutePositon()-m_pPathSearch->GetPassPermitRollbackMargin()) >= (double)m_pOHTMainStatus->dOHTTotalRunDis) || ((GetCurrentMode() == MODE_AUTO) && (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF))))
					{

						ADD_LOG_TASKCONTROL("롤백에의한 통과허가 반납 요청  Index: PP:%d, PPO:%d PPStep:%d, D_PPOStep %d,StartDis:%06f,OHTTotalRunDis:%06f, GetAbsolutePositon%06f ",m_pPassPermit->GetPPIndex(),m_pPassPermit->GetPPOIndex(),pCurPPCmd->EXE_CycleDriving_Info.D_PPStep,pCurPPCmd->EXE_CycleDriving_Info.D_PPOStep,pCurPPCmd->EXE_CycleDriving_Info.StartDistance, m_pOHTMainStatus->dOHTTotalRunDis,m_pPassPermit->GetAbsolutePositon());

						bool bCheckPoint = false;
						
						pCurPPCmd->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
                        Sleep(1); //컨텍스트 스위칭 
						
						if(pCurPPCmd->IsExcuteInfo ==true)
						{
							bCheckPoint = PathChangeReq2Driving(pCurPPCmd->EXE_CycleDriving_Info.StartPoint,pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum);
							if(bCheckPoint == false)
							{
								if((GetCurrentMode() == MODE_AUTO) &&
								((GetExecuteStatus()== STS_TURN_OFF) || (m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF)))
								{
										//NOF
								}
								else
									m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_STOP); //실패할 경우 E-STOP으로 정지할 것. (실패할 확률 제로로 만들기.. )
							}

							m_pOHTMainStatus->FinalNodeOfPathSearch = pCurPPCmd->EXE_CycleDriving_Info.StartPoint;
							m_pOHTMainStatus->FinalEndNodeOfPathSearch = pCurPPCmd->EXE_CycleDriving_Info.StartPoint;
							m_pOHTMainStatus->FinalStationOfPathSearch = 0;
							pCurPPCmd->IsExcuteInfo = false;
						}


						

						int TempPassIndex = m_pPassPermit->GetPassPermitCount();

						//2. 통과개방요청을 통해 통과허가 등록된 부분 제거
						if(TempPassIndex!= LISTCOUNT_EMPTY)
						{
								pPasspermit = (DATA_PASS *)m_pPassPermit->PopBackPassPermit();

								   if(!(memcmp(&(pPasspermit->PassInfo), &pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON)) == 0))
								   {
									  ADD_LOG_TASKCONTROL("Check]개방요청  롤백에의한 반납 NG");
								   }

								delete pPasspermit;
						}

						ADD_LOG_TASKCONTROL("Check]개방요청  롤백에의한 반납 ");

						//1. 통과개방 요청 등록
						DATA_PASSOPEN * pDataPassOpen;
						pDataPassOpen = new DATA_PASSOPEN;
						pDataPassOpen->PassOpenIndex = m_pPassPermit->GetPPIndex();
						pDataPassOpen->bPassOpenType = JOBCHAGE_RETURN_TYPE;
						memmove(&pDataPassOpen->PassOpneInfo, &pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData,sizeof(PASSPERMIT_COMMON));
						m_pPassPermit->AddPassOpen(pDataPassOpen);


						m_pPassPermit->SetDecreasePPIndex();
						 	
						/*	
						AckPkt.AckPkt.TAG.Value = TagID::CMD_PASSPERMITOPEN_OHT2OCS;
						memmove(&AckPkt.AckPkt.Value, &pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON));
						AckPkt.Length = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);
						MakeResponseStandardOHT2OCS(&AckPkt);
						*/
						
						//Rollback 개시 플래그  ------중요 puting
						//기동 상태도 Off
						//OHT 일시정지 시키기        puting
						//Rollbacking이 풀리면 일시정지 해제하기 ----다른곳에서

					}
	//				else if(nTempIndex!= LISTCOUNT_EMPTY)
	//				{
	//					ADD_LOG_TASKCONTROL("위치 정보에 의한 에 의한 통과개방 요청  Index: PP:%d, PPO:%d PPStep:%d, D_PPOStep %d,StartDis:%06f,OHTTotalRunDis:%06f, GetAbsolutePositon%06f ",m_pPassPermit->GetPPIndex(),m_pPassPermit->GetPPOIndex(),pCurPPCmd->EXE_CycleDriving_Info.D_PPStep,pCurPPCmd->EXE_CycleDriving_Info.D_PPOStep,pCurPPCmd->EXE_CycleDriving_Info.StartDistance, m_pOHTMainStatus->dOHTTotalRunDis,m_pPassPermit->GetAbsolutePositon());
	//
	//				}
					else if(bRollback == true)
					{
	//					bTempCheckfanal = truSetRollBackFinishe;
						m_pPassPermit->SetRollBackFinish(true);

						// 통과허가 반납 마지막에 도착 플래그 설정
					}else
					{
						//통과허가 반납 or 마지막이 아닐떄 플로그 원복
					}
                }

			}

			//Step 2. 통과허가 및 개방 요청 판단하는 부분

			// 요청 할지 말지 판단 . (VHL이 정지되기 전까지 통과허가 및 개방은 요청함. 위치 넘아갈 경우 등 고려하면 요청하는게 맞음.
			//롤백 상황에서만 요청하지 않음.
			else
			{
                //현재 Link가 허가 안되었을 경우, 허가해 줄것 puting
				if(pCurPPCmd->CycleType ==CYCLETYPE_DRIVING)
				{
					if((pCurPPCmd->EXE_CycleDriving_Info.D_PPStep == CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE) &&
						(pCurPPCmd->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint == true)&&
						(bRollback ==false) &&
						IsOHTDetectCheckPassPermit((((double)pCurPPCmd->EXE_CycleDriving_Info.StartDistance - (double)m_pPassPermit->GetAbsolutePositon()) - (double)m_pOHTMainStatus->dOHTTotalRunDis)) &&
						(m_pOHTMainStatus->JobChangeFlag.bCycleingChangeFlag == false) &&  //작업지시 변경으로 추가 puting 20160307
						(((double)pCurPPCmd->EXE_CycleDriving_Info.StopDistance - (double)m_pPassPermit->GetAbsolutePositon()) <= (double)m_pOHTMainStatus->dOHTTotalRunDis))
						{

							AckPkt.AckPkt.TAG.Value = TagID::CMD_PASSPERMIT_OHT2OCS;
							memmove(&TempPassPermit.PassPermit_CM, &pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON));

							int TempTime = (int)(OHTUTILITY::PreventOverflowGetTimeMs(m_pPassPermit->GetPassPermitTime()));

							try{

                           	for(int Index =0; Index< 3; Index++)
							{
								TempPassPermit.PassDelayTime[2-Index] = (TempTime >> (Index * 8))&0xFF;

							}
							}catch(...)
								 ADD_LOG_TASKCONTROL("Judge 통과허가요청 Exception!!");

							int nStopNode =0;
							int nResetNode =0;
							for (int i = 0; i < 3; i++)
							{
								// Node 비트연산   puting
								int TmempStopNode = TempPassPermit.PassPermit_CM.StopNodeID[i];
								int TmempResetNode = TempPassPermit.PassPermit_CM.ResetNodeID_Main[i];

								if (HIGH_BYTE == 0) {
									nStopNode += TmempStopNode << ((2 - i) * 8);
									nResetNode +=TmempResetNode << ((2 - i) * 8);
								}
								else
								{
									nStopNode += TmempStopNode << (i * 8);
									nResetNode += TmempResetNode << (i * 8);
								}
							}
							m_pCurrCIDUSE.nStopNodeID = nStopNode;
							m_pCurrCIDUSE.nResetNodeID = nResetNode;
							m_pCurrCIDUSE.bCIDUse = false;//CID 사용

							memmove(&AckPkt.AckPkt.Value, &TempPassPermit, sizeof(PROTOCOL_BODY_PASSPERMIT_OHT2OCS_ID_241));
							AckPkt.Length = sizeof(PKT_PASSPERMIT_OHT2OCS_ID_241);
							MakeResponseStandardOHT2OCS(&AckPkt);

							//if(((GetExecuteStatus() == STS_ARRIVED) || (GetExecuteStatus() == STS_INIT)) &&
							if(m_pExecuteInfo->ExecutePositionInfo.CurrNode == pCurPPCmd->EXE_CycleDriving_Info.StartPoint)
								{
									if(m_pExecuteInfo->nCheckDivInfo  != pCurPPCmd->EXE_CycleDriving_Info.AddInfo.Division)
									{
									   m_pExecuteInfo->nCheckDivInfo = pCurPPCmd->EXE_CycleDriving_Info.AddInfo.Division;
									   ADD_LOG_TASKCONTROL("DIVISION %d",m_pExecuteInfo->nCheckDivInfo);
									}
								}
//
//							ADD_LOG_TASKCONTROL("JudgePassPermitReq_RunningDistance %f AboslutePos%f", m_pOHTMainStatus->dOHTTotalRunDis,GetAbsolutePositon());
//
							ADD_LOG_TASKCONTROL("Judge 통과허가요청, Position %f, D_PPStep:%d,D_PPOStep:%d, IsCurLinkPPPoint %d, SQNum%d, StatNID:%d, StopDistance%f, Time : %d OHTTotalRunDis%06f, GetAbsolutePositon%06f, StartDis:%06f",
							(((double)pCurPPCmd->EXE_CycleDriving_Info.StartDistance - (double)m_pPassPermit->GetAbsolutePositon()) - (double)m_pOHTMainStatus->dOHTTotalRunDis),
							pCurPPCmd->EXE_CycleDriving_Info.D_PPStep,
							pCurPPCmd->EXE_CycleDriving_Info.D_PPOStep,
							pCurPPCmd->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint,
							(int)pCurPPCmd->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum,
							(int)pCurPPCmd->EXE_CycleDriving_Info.StartPoint,
							pCurPPCmd->EXE_CycleDriving_Info.StopDistance,
							TempTime,
							m_pOHTMainStatus->dOHTTotalRunDis,
							m_pPassPermit->GetAbsolutePositon(),
							pCurPPCmd->EXE_CycleDriving_Info.StartDistance);


						}
				}


				//	bCheckBreakPoint = true;  //Excuteinfo가 false인 첫번째 명령 확인 후 나감.
				// 통과허가 개방 요청 확인


				if(pCurPPOCmd->CycleType ==CYCLETYPE_DRIVING)
				{
					if((pCurPPOCmd->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint == false)&&
					   (pCurPPOCmd->IsListF2B == false)&&
					   (pCurPPCmd->IsExcuteInfo == true)&&
					   (m_pPassPermit->GetPPIndex() == m_pPassPermit->GetPPOIndex()) &&
					   (nCountCycle == m_pPassPermit->GetPPOIndex()+1))
					{
						if(((m_pOHTMainStatus->FinalStationOfPathSearch == m_pExecuteInfo->ExecutePositionInfo.CurrStation) &&  (m_pExecuteInfo->ExecutePositionInfo.CurrStation !=0)) ||
						  ((m_pOHTMainStatus->FinalNodeOfPathSearch == m_pExecuteInfo->ExecutePositionInfo.CurrNode) &&  (m_pExecuteInfo->ExecutePositionInfo.CurrNode !=0)))
						{
						   	pCurPPOCmd->IsListF2B = true;
							ADD_LOG_TASKCONTROL("Judge 통과개방무시, _Not Meaning_End Proc,(%d)",nError);
							ADD_LOG_TASKCONTROL("PPO_Check Index: PP:%d, PPO:%d",m_pPassPermit->GetPPIndex(),m_pPassPermit->GetPPOIndex());
						}
					}
				}

			}


		if(pCurPPOCmd->CycleType ==CYCLETYPE_DRIVING)
		{
			 if((pCurPPOCmd->EXE_CycleDriving_Info.D_PPOStep == NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE) &&
				(pCurPPOCmd->EXE_CycleDriving_Info.D_PPStep >= CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE) &&
				(pCurPPOCmd->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint == true)&&
				(pCurPPOCmd->IsListF2B == false) &&
				(m_pPassPermit->GetPPIndex() >= m_pPassPermit->GetPPOIndex()) &&
				((((m_pExecuteInfo->ExecutePositionInfo.CurrPreCheckNode == pCurPPOCmd->EXE_CycleDriving_Info.EndPoint) ||
				   (m_pExecuteInfo->ExecutePositionInfo.CurrNode == pCurPPOCmd->EXE_CycleDriving_Info.EndPoint))  &&
				   (pCurPPOCmd->EXE_CycleDriving_Info.ResetPoint != pCurPPOCmd->EXE_CycleDriving_Info.EndPoint)   &&
				   (pCurPPOCmd->EXE_CycleDriving_Info.StartPoint != pCurPPOCmd->EXE_CycleDriving_Info.EndPoint))  ||   //예외처리

				   ((pCurPPOCmd->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint == true) &&
					(GetExecuteStatus() == STS_ARRIVED) && (m_pExecuteInfo->ExecutePositionInfo.CurrStation !=0) && (m_pExecuteInfo->ExecutePositionInfo.CurrStation == pCurPPOCmd->EXE_CycleDriving_Info.AddInfo.TargetStationID))  ||  //2) 반납이 필요한 이적재 태그

				(((((double)pCurPPOCmd->EXE_CycleDriving_Info.ResetNodeDistance - (double)m_pPassPermit->GetAbsolutePositon()) <= (double)m_pOHTMainStatus->dOHTTotalRunDis)) &&
				 ((m_pExecuteInfo->ExecutePositionInfo.CurrPreCheckNode == pCurPPOCmd->EXE_CycleDriving_Info.ResetPoint) ||
				(m_pExecuteInfo->ExecutePositionInfo.CurrNode == pCurPPOCmd->EXE_CycleDriving_Info.ResetPoint)))))     ///3) 일반 통과개방 처리.
				{

					ADD_LOG_TASKCONTROL("!Judge 통과개방 시퀀스 넘버 Same %d,%d %d %d %d %d %d %d %d %d",
					m_pPassPermit->GetPassOpenSeq(),
					pCurPPOCmd->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum,
					m_pExecuteInfo->ExecutePositionInfo.CurrNode,
					m_pExecuteInfo->ExecutePositionInfo.CurrPreCheckNode,
					pCurPPOCmd->EXE_CycleDriving_Info.EndPoint,
					pCurPPOCmd->EXE_CycleDriving_Info.ResetPoint,
					pCurPPOCmd->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint,
					m_pExecuteInfo->ExecutePositionInfo.CurrStation,
					pCurPPOCmd->EXE_CycleDriving_Info.FinalPoint,
					pCurPPOCmd->EXE_CycleDriving_Info.AddInfo.TargetStationID);

					if(m_pPassPermit->GetPassOpenSeq() != pCurPPOCmd->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum)
					{
						TempPPOIndex  = m_pPassPermit->GetPPOIndex();
						nError = m_pPassPermit->SetIncreasePPOIndex(m_pPassPermit->GetPPIndex());

						if((nError !=-1) || (nCountCycle == m_pPassPermit->GetPPOIndex()+1))
						{
							//1. 통과개방 요청 등록
							DATA_PASSOPEN * pDataPassOpen;
							pDataPassOpen = new DATA_PASSOPEN;
							pDataPassOpen->PassOpenIndex = TempPPOIndex;
							pDataPassOpen->bPassOpenType = NORMAL_TYPE;
							memmove(&pDataPassOpen->PassOpneInfo, &pCurPPOCmd->EXE_CycleDriving_Info.CurLinkPassPermitData,sizeof(PASSPERMIT_COMMON));
							m_pPassPermit->AddPassOpen(pDataPassOpen);

								//2. 이전 Path 정보 삭제
							int nListCount = m_pPathSearch->GetPathCount();
							bool bFinalCheck = false;
							EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO = NULL;

							if(nListCount !=0)
							{
								for(int Index=0; Index < nListCount; Index++)
								{
									//예외처리 부분
									if(pCurPPOCmd->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint == false)
										break;

									pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO *) m_pPathSearch->PopFirstPath();

//									ADD_LOG_TASKCONTROL("Path(Delete) EndID:%d",pEXECUTE_DRIVING_INFO->BasicInfo.EndNID);

									if((pEXECUTE_DRIVING_INFO->BasicInfo.EndNID == pCurPPOCmd->EXE_CycleDriving_Info.ResetPoint) ||
									   (pEXECUTE_DRIVING_INFO->BasicInfo.EndNID == pCurPPOCmd->EXE_CycleDriving_Info.StartPoint))
										bFinalCheck = true;

									delete pEXECUTE_DRIVING_INFO;

									//최종완료시 끝내기.
									if(bFinalCheck == true)
										break;
								}
							}

							m_pPassPermit->SetPassOpenSeq(pCurPPOCmd->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum);

							//통과허가 문의내용 변경에 의한 추가
							int TempPassIndex = m_pPassPermit->GetPassPermitCount();
							//2. 통과개방을 통해 통과허가 등록된 부분 제거
							if(TempPassIndex!= LISTCOUNT_EMPTY)
							{
								for (int i = 0; i < TempPassIndex; i++)
								{
									pPasspermit = (DATA_PASS *)m_pPassPermit->ReferPassPermit(i);

									if(pPasspermit != NULL)
									{
										if(memcmp(&(pPasspermit->PassInfo), &pCurPPOCmd->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON)) == 0)
										  {
												ADD_LOG_TASKCONTROL("[Check]통과허가개방 등록에 의한 통과허가 삭제");
												Sleep(1);
												m_pPassPermit->DeletePassPermit(i);
												break;
										  }
									}
								}
							}

						}
					 }

				}
			}
		}



}

bool TaskControl::CheckCmdPassOpen_fromTP_and_AutoChange(PASS_PERMIT_OPEN_TYPE Type, PKT_OCS2OHT* pRcvData, int QuickCmd)
{

        char szType[1024];

        //로그 타입 구분
        switch(Type)
        {
        case FROMTP_TYPE:
            ADD_LOG_TASKCONTROL("[From TP] 통과허가개방 명령");
            sprintf(szType, "From TP");
	    	break;
        case AUTOCHANGE_TYPE:
            ADD_LOG_TASKCONTROL("[Auto Change], 통과허가개방 명령");
            sprintf(szType, "Auto Change");
            break;
        }


        bool bCheckReturn = true;

        DATA_PASS * pPasspermit; // Command를 위한 포인터 변수
        DATA_PASSOPEN * pDataPassOpen;

	    PKT_ACK_OHT2OCS AckPkt;
	    int TempPassIndex = m_pPassPermit->GetPassPermitCount();
	    STATION_DATA StationData; // Station Data를 가져올 구조체 포인터

	    int Error = NO_ERR;

	    int nTempStopNode=0, nTempResetNode=0;

	//BZ Stop 상태 확인함.
    	if(m_pOHTMainStatus->bCheckCycleRunning == true)
    	{
		    ADD_LOG_TASKCONTROL("[%s]통과허가요구 미동록_ No Cycle", szType);

	    	return bCheckReturn;
    	}


    	//2. 통과개방을 통해 통과허가 등록된 부분 제거
		if(TempPassIndex!= LISTCOUNT_EMPTY)
		{
				pPasspermit = (DATA_PASS *)m_pPassPermit->ReferFirstPassPermit();

				PASSPERMIT_COMMON TempPassPermit;
                //메뉴얼에서 개방요청시 리스트에 추가여부 확인 후 PassOpenList에 추가
                if(!m_bCheckPassOpenListCount)
                {
                    pDataPassOpen = new DATA_PASSOPEN;
                    pDataPassOpen->bPassOpenType = FORCE_TYPE;
                    memmove(&pDataPassOpen->PassOpneInfo, &pPasspermit->PassInfo, sizeof(PASSPERMIT_COMMON));
                    m_pPassPermit->AddPassOpen(pDataPassOpen);
                    m_bCheckPassOpenListCount = true;
                }

                TempPassPermit.SQNum = pPasspermit->PassInfo.SQNum;
				for(int i=0; i<3; i++)
				{

					TempPassPermit.StopNodeID[i] = pPasspermit->PassInfo.StopNodeID[i];
					TempPassPermit.ResetNodeID_Main[i] = pPasspermit->PassInfo.ResetNodeID_Main[i];
					TempPassPermit.ResetNodeID_Sub[i] = pPasspermit->PassInfo.ResetNodeID_Sub[i];

					// Node 비트연산   puting
					int Temp1 = pPasspermit->PassInfo.StopNodeID[i];
					int Temp2 = pPasspermit->PassInfo.ResetNodeID_Main[i];


					if (HIGH_BYTE == 0)
					{
						nTempStopNode += Temp1 << ((2 - i) * 8);
						nTempResetNode +=	Temp2 << ((2 - i) * 8);
					}
					else
					{
						nTempStopNode += Temp1 << (i * 8);
						nTempResetNode +=	Temp2 << (i * 8);

					}

				}

				ADD_LOG_TASKCONTROL("[%s]통과허가요구 등록 S:%d R:%d", szType,TempPassPermit.StopNodeID[2],TempPassPermit.ResetNodeID_Main[2]);

				if(TempPassIndex == 1)
				{
					if((m_pExecuteInfo->ExecutePositionInfo.Marked) && (m_pExecuteInfo->ExecutePositionInfo.StationType != MARK_TYPE_NODE))
					{
						Error = GetStationData(&StationData, m_pExecuteInfo->ExecutePositionInfo.CurrStation);
						if(Error)
						{
							//에러일 경우는 반납안함.
							ADD_LOG_TASKCONTROL("[%s]통과허가요구 등록(TP Step1", szType);
							return false;


						}
						ADD_LOG_TASKCONTROL("[%s]통과허가요구 등록(%d %d %d %d", szType,nTempStopNode,nTempResetNode,StationData.ParentNodeID,StationData.NextNodeID);
						if((nTempStopNode == StationData.ParentNodeID) && (nTempResetNode==StationData.NextNodeID))
						{
							ADD_LOG_TASKCONTROL("[%s]통과허가요구 등록(TP Step2",szType);
							return false;
						}
						else
						{
                            ADD_LOG_TASKCONTROL("[%s]통과허가요구 등록(TP Step3",szType);
							 AckPkt.AckPkt.TAG.Value = TagID::CMD_PASSPERMITOPEN_OHT2OCS;
							 memmove(&AckPkt.AckPkt.Value, &TempPassPermit, sizeof(PASSPERMIT_COMMON));
							 AckPkt.Length = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);
							 MakeResponseStandardOHT2OCS(&AckPkt);
						}

					}
                    else                  // Mark안한 경우도 포함?
					{
					     ADD_LOG_TASKCONTROL("[%s]통과허가요구 등록(TP Step4",szType);
						 AckPkt.AckPkt.TAG.Value = TagID::CMD_PASSPERMITOPEN_OHT2OCS;
						 memmove(&AckPkt.AckPkt.Value, &TempPassPermit, sizeof(PASSPERMIT_COMMON));
						 AckPkt.Length = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);
						 MakeResponseStandardOHT2OCS(&AckPkt);
					}

				}
				else
				{
				     ADD_LOG_TASKCONTROL("[%s]통과허가요구 count 1 초과 (TP Step5",szType);
					 AckPkt.AckPkt.TAG.Value = TagID::CMD_PASSPERMITOPEN_OHT2OCS;
					 memmove(&AckPkt.AckPkt.Value, &TempPassPermit, sizeof(PASSPERMIT_COMMON));
					 AckPkt.Length = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);
					 MakeResponseStandardOHT2OCS(&AckPkt);
				}




		}else
		{
			ADD_LOG_TASKCONTROL("[%s]통과허가요구 미동록_ CycleEmpty", szType);
		}

	return bCheckReturn;

}

/**
 @class   CheckAbsolutePosition 함수.
 @date    2015.10.15
 @author  puting
 @brief   포지션 얻어옴
 */
void TaskControl::CheckAbsolutePosition()
{
	bool bCheckFlag = false;
//	bool bCheckRunning = false;
	double dInitPosition = 0.0;

	bCheckFlag =  m_pExecuteInfo->FlagInitPositon.bInitFlag;
	//bCheckRunning = m_pExecuteInfo->FlagRunningPositon;
	dInitPosition = m_pExecuteInfo->FlagInitPositon.dDrivingInitPosition;

	if(bCheckFlag ==true)
	{

			m_pPassPermit->SetSumResetPositon(dInitPosition);
			m_pOHTMainStatus->dOHTTotalRunDis = 0.0;

			EnterCriticalSection(&OHTMainForm->m_CS);
			m_pExecuteInfo->FlagInitPositon.bInitFlag = false;
			m_pExecuteInfo->dCompensatedPosition = 0.0;
			m_pExecuteInfo->FlagInitPositon.dDrivingInitPosition =0.0;
			m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing = false;
			m_pExecuteInfo->dCompensatedPosition   = 0.0;
			LeaveCriticalSection(&OHTMainForm->m_CS);

			ADD_LOG_TASKCONTROL("CheckAbsolutePosition %f",dInitPosition);

	}

   	 //------거리값 갱신 건들지말것.  puting 위치는 TaskControl로 변경 완료
	if(((GetExecuteStatus() ==STS_GOING) && ((m_pExecuteInfo->FlagRunningPositon == true) && (m_pExecuteInfo->FlagInitPositon.bInitFlag == false))) ||
		(GetExecuteStatus()== STS_ERROR) ||
		((GetExecuteStatus()== STS_TURN_OFF)&& (m_pExecuteInfo->FlagRunningPositon == true)))
		//((GetExecuteStatus()== STS_AUTOPOSITION)&& (m_pExecuteInfo->FlagRunningPositon == true)))
	{
		m_pOHTMainStatus->dOHTTotalRunDis = (double)m_pExecuteInfo->dCompensatedPosition;    //오차를 감안한 거리값.
	}
	else
	{
		m_pOHTMainStatus->dOHTTotalRunDis = 0.0;
	}

//	Sleep(1); //컨테스트 스위칭
}


/**
 @brief   원하는 구간 UBG Sequence 정상 동작 확인
 @author  soming
 @date    2018.10.15
 @return  UBG 체크 노드, 현재 노드 확인 및 UBG 체크 시퀀스 결정
 */
 void TaskControl::CheckUBGSequence(int nDistanceSensorType)
 {
	// AOHC-262
	BYTE CurStatus  = m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status;

	bool bUGBumpperCheck = false;
	bool bUBGNodeCheck = false;
	bool bPassingNow = false;


	int nUBGData[UBG_Sensor_Data_Num] = {0,};
	//현재 노드와 Offset
	int nCurNode = m_pExecuteInfo->ExecutePositionInfo.CurrNode;
	int nCurOffset = m_pExecuteInfo->ExecutePositionInfo.CurrNodeOffset;

	// UBG, OBS 감지레벨 확인
	int nUBGLevel = m_pHWSet->m_pOHTDetect->GetSensorLevel2(m_pHWCommon->m_Input.OHTDETECT_Long,m_pHWCommon->m_Input.OHTDETECT_Middle, m_pHWCommon->m_Input.OHTDETECT_Short , m_pHWCommon->m_Input.OHTDETECT_Fail);
	int nOBSLevel = m_pHWSet->m_pOHTDetect->GetSensorLevel2(m_pHWCommon->m_Input.OBSTACLE_Long, m_pHWCommon->m_Input.OBSTACLE_Middle, m_pHWCommon->m_Input.OBSTACLE_Short , m_pHWCommon->m_Input.OBSTACLE_Fail);

	 char szLog[1024];
	 memset(szLog, 0, sizeof(szLog));

	 // 한 구간에서 같은 모니터링 로그가 계속 증가되는 것을 방지하기 위해
	 // 한번 Increase 되면 true가 되고, 구간을 빠져나가면 false가 됨

	 // 현재 직진 노드를 지나는지 확인한다
	if(m_pPathSearch->m_pUBGParamSet != NULL)
	{
		bPassingNow = m_pPathSearch->m_pUBGParamSet->ExistsCheck(nCurNode);
	}else
       bPassingNow = false;


	 const int DEALY_TIME_MS	= 400;

	 //////////////////STEP 1///////////////////
	 //Bumper Trigger
	if((m_pHWSet->m_pBumper->IsBumperPress() == true) && (bprebumpper != m_pHWSet->m_pBumper->IsBumperPress()))
	{
		bUGBumpperCheck = true;
		bUBGOneHourMonitoring = false;
	}
	bprebumpper = m_pHWSet->m_pBumper->IsBumperPress() ;

    //Node UBG 감지 Trigger
	if((bPassingNow == true) && (nUBGLevel == 2))
	{
		bUBGNodeCheck = true;
	}

	//////////////////STEP 2///////////////////
	if((nUBGSensorDetecting == 0) &&
	  ((bUGBumpperCheck ==true) || (bUBGNodeCheck==true)))
	  {
			nUBGSensorDetecting = 1;
	  }

	if(nDistanceSensorType == UBG_OHTDETECT_SENSOR)
	{
		nUBGSensorDetecting = 0;
		bUBGOneHourMonitoring = false;
	}
	//////////////////STEP 3///////////////////
	// AOHC-237 UBG 좌/우 틀어짐 검출 기능 추가

	if((bUBGOneHourMonitoring == false) && (nDistanceSensorType != UBG_OHTDETECT_SENSOR))
	{
		switch(nUBGSensorDetecting)
		{
		case 0:  // Standby
			break;

		case 1:  // Send
			 m_pHWSet->m_pOHTDetect->DataReadStart(nUBGData);
			 ADD_LOG_TASKCONTROL("UCS 1");
			 m_dwUBGTrigerTime = timeGetTime();
			 nUBGSensorDetecting = 2;
			break;

		case 2:  // Receive & Data Read
			m_pHWSet->m_pOHTDetect->UpdataData(nUBGData,false);


			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUBGTrigerTime) >UBGLOGTIME )    //Read Data 시간 1000msec
			{   // AOHC-262  UBG Data Read Sequence 개선
				if(m_bUBGReadCheck)
				{
					ADD_LOG_TASKCONTROL("UCS 2 ->3");
					nUBGSensorDetecting = 3;
					m_bUBGReadCheck = false;
				}
				else
				{
				   nUBGSensorDetecting = 1;
				   ADD_LOG_TASKCONTROL("UCS 2 ->1 , ,bChek = %d, nChke = %d",m_bUBGReadCheck, nCheck );
				   nCheck ++;
				   if(nCheck >=2)
				   {
                    	nCheck = 0;
						m_bUBGReadCheck = true;

				   }
				}
				m_dwUBGTrigerTime = timeGetTime();
			}
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_UBG_PAT);
			if(m_MoniUBGPat!=nCurNode)
			{
				ADD_MD_LOG("UBG_PAT Cur N : %d", nCurNode);
			}
			m_MoniUBGPat = nCurNode;
			break;

		case 3: // End
			 m_LogTime = timeGetTime();  //처음 logging 시간 기록
			 bUBGOneHourMonitoring = true;
			 nUBGSensorDetecting = 0;
			 ADD_LOG_TASKCONTROL("UCS 3");
			break;
		}

	}
	else
	{
		const static int nInterval_ms = PREVIOUS_LOG_WRITE_INTERVAL_MIN*60*1000;     //60분에 한번씩 로깅

		if(OHTUTILITY::PreventOverflowGetTimeMs(m_LogTime) > nInterval_ms)
		{
			bUBGOneHourMonitoring = false;
			ADD_LOG_TASKCONTROL("UBG1h Restart");
		}
	}
	///UBG Data 확인

  	 if(bPassingNow == false)
	 {
		 nCycleCnt = -1;

		 bIncreased_UD = false;	// 다시 초기화
		 bIncreased_UF = false;
		 bIncreased_UC = false;

		 chtTm_UD.OnDelay(false); // 타이머 초기화
		 chtTm_UF.OnDelay(false);

		 return;   // 지나지 않으면 return
	 }

	//UF(UBG Fail): OBS 감지, UBG 미감지 케이스 ***********
	//
	if(bIncreased_UF == false)
	{
		// 감지레벨 2~8 사이면 감지 된것으로 간주. 클수록 Long에 가깝고 작을수록 Short에 가까움
		bool bUBG_Detect = nUBGLevel >= 2 && nUBGLevel < 8;
		bool bOBS_Dected = nOBSLevel >= 2 && nOBSLevel < 8;

		bool bCond = bOBS_Dected && !bUBG_Detect;

		if(chtTm_UF.OnDelay(bCond, DEALY_TIME_MS))    // 일정시간 채터링
 		{
			chtTm_UF.OnDelay(false);

			// UF Increase
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_UF); //monitoring 로그 찍기
			bIncreased_UF = true;

			sprintf(szLog, "Inc UF!! OBS_Lv:%d, N:%d(%d), Cnt:%d", nOBSLevel, nCurNode, nCurOffset, m_DiagManager->GetPreviosValue(PREVIOS_DIAG_UF));
			RapidLog(UBG_ABNORMAL_CHECK_LOG, szLog);
 		}
 	}

	//UD(UBG Dir Check): OBS 미감지, UBG 감지 케이스 => 옆레일 VHL을 봤다고 간주***********
	//
	if(bIncreased_UD == false)
 	{
        // 옆레일 감지는 UBG는 3~6사이 일때까 가장 확실한 범위. 이전에 확장패턴 샘플 돌린 로그 확인 결과
		bool bUBG_Detect = nUBGLevel >= 3 && nUBGLevel <= 6;
		bool bOBS_Dected = nOBSLevel >= 2 && nOBSLevel < 8;

		bool bCond = !bOBS_Dected && bUBG_Detect;

		if(chtTm_UD.OnDelay(bCond, DEALY_TIME_MS))    // 일정시간 채터링
 		{
			chtTm_UD.OnDelay(false);

			// UD Increase
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_UD); //monitoring 로그 찍기
			bIncreased_UD = true;

			sprintf(szLog, "Inc UD!! UBG_Lv:%d, N:%d(%d), Cnt:%d", nUBGLevel, nCurNode, nCurOffset, m_DiagManager->GetPreviosValue(PREVIOS_DIAG_UD));
			RapidLog(UBG_ABNORMAL_CHECK_LOG, szLog);
 		}
 	}

	//UC(UBG Seq Check): 7~2 순서대로 레벨이 오지 않고, 한번에 두단계 이상 건너뛴 경우. **********
	//
	if(bIncreased_UC == false)
	{
		static int prevUBG_Lv = 0;   // 이전 Cycle UBG Level

		 memset(szLog, 0, sizeof(szLog));
		 if(nCycleCnt > 0)
		 {
			bool bErrorCase = false; // 이값이 true면 에러

			if(nUBGLevel == -1)    // fail case
			   bErrorCase = true;
			else
			{
				// 첫진입(2 or 8) 일 경우는 cycle 초기화
				if((prevUBG_Lv == 2 && nUBGLevel == 8))
					nCycleCnt = -1;
				else
				{
				   int nOffset = abs(prevUBG_Lv - nUBGLevel); //이전->현재 절대값 2차이 확인
				   if(nOffset >= 2)
					   bErrorCase = true;
				  
				}
			}

			if(bErrorCase)
			{
				// UD Increase
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_UC); //monitoring 로그 찍기
				bIncreased_UC = true;

				sprintf(szLog, "Inc UC!! UBG_Lv:%d->%d, OBS:%d, N:%d(%d), Cnt:%d", prevUBG_Lv, nUBGLevel, nOBSLevel, nCurNode, nCurOffset, m_DiagManager->GetPreviosValue(PREVIOS_DIAG_UC));
				RapidLog(UBG_ABNORMAL_CHECK_LOG, szLog);
			}
		 }
		 prevUBG_Lv = nUBGLevel;  //이전값 저장
		 nCycleCnt++;
	}

	return;
 }


/**
 @class   Manaual에서 이적재 작업할 경우, Teaching Data에서 불러옴.
 @date    2015.10.05
 @author  puting
 @brief   Teaching Data만 집어넣어줌.
 */
int TaskControl::GetManaulTeachingData(UINT dCurStationID, BYTE cPortType,	TRANS_CMD* pTransData)
{

	int Error = NO_ERR;

	if(bHoistJig == true)
	{
		pTransData->CurStationID = 456;
		pTransData->PortType = 0x01;
//		pTransData->Tposition_Hoist = 30170;
//		pTransData->Tposition_Shift = 0;
		pTransData->Tposition_Rotate = 0;
		pTransData->PIODirection = 0x01;
		pTransData->PIOTimeLevel = 0;
		pTransData->LookDownLevel = 0;
		pTransData->PIOID = 0;
		pTransData->PIOCS = 0;
		pTransData->PIOChannel = 0;
		pTransData->HoistSpeedLevel = 100;
		pTransData->ShiftSpeedLevel = 100;
		pTransData->TransOption.Data = 0;

		pTransData->TransOption.Info.EQPIODisable_TP = false;
		pTransData->TransOption.Info.HandDetectEQ = false;
	}
	return Error;

	TEACHING_DATA *pTeachingData;

	pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(dCurStationID, cPortType);

	// by puting 2015.11.03 : Teaching Data Find 결과 함수.
	if (pTeachingData == NULL)
	{
		Error = ERR_DATA_TEACHING_NOEXIST;
		ADD_LOG_TASKCONTROL("[ERR_DATA_TEACHING_NOEXIST]:CmdType[%02X] %d",
			cPortType,dCurStationID);

	}
	else
	{
		// by zzang9un 2013.08.15 : Node는 빠져 있음
		pTransData->CurStationID = pTeachingData->StationID;
		pTransData->PortType = pTeachingData->PortType;
		pTransData->Tposition_Hoist = pTeachingData->Hoist;
		pTransData->Tposition_Shift = pTeachingData->Shift;
		pTransData->Tposition_Rotate = pTeachingData->Rotate;
		pTransData->PIODirection = pTeachingData->PIODirection;
		pTransData->PIOTimeLevel = pTeachingData->PIOTimeLevel;
		pTransData->LookDownLevel = pTeachingData->LookDownLevel;
		pTransData->PIOID = pTeachingData->PIOID;
		pTransData->PIOCS = pTeachingData->PIOCS;
		pTransData->PIOChannel = pTeachingData->PIOChannel;

		pTransData->HoistSpeedLevel = 100;
		pTransData->ShiftSpeedLevel = 100;


		// by zzang9un 2013.08.26 : TP에서 EQPIO Disable이 On된 경우
		bool bEQPIODisable_TP = pTransData->TransOption.Info.EQPIODisable_TP;
		bool bHandDetectEQ = pTransData->TransOption.Info.HandDetectEQ;

		pTransData->TransOption.Data = pTeachingData->TransOption.Data;

		pTransData->TransOption.Info.EQPIODisable_TP = bEQPIODisable_TP;
		pTransData->TransOption.Info.HandDetectEQ = bHandDetectEQ;
//		pTransData->TransOption.Info.TransSound = ON;

		if (bEQPIODisable_TP || bHandDetectEQ)
			ADD_LOG_TASKCONTROL(
			"[MakeCmd][Trans][TP Setting] EQPIODisable:%d, HandDetectEQ:%d",
			bEQPIODisable_TP, bHandDetectEQ);

		// by zzang9un 2013.07.23 : Log 생성
		ADD_LOG_TASKCONTROL(
			"[MakeCmd][Trans][Find Teaching Data] StationID:%06d, Pd%d, Hoist:%d, Shift:%d, Rotate:%d PIOID:%d, CS:%d)",
			pTransData->CurStationID, pTransData->PortType,
			pTransData->Tposition_Hoist, pTransData->Tposition_Shift,
			pTransData->Tposition_Shift, pTransData->PIOID, pTransData->PIOCS);
		ADD_LOG_TASKCONTROL(
			"[MakeCmd][Trans][Find Teaching Data] PIODir=%d, PIOTimeLevel=%d, LookdownLevel=%d",
			pTransData->PIODirection, pTransData->PIOTimeLevel,
			pTransData->LookDownLevel);
		ADD_LOG_TASKCONTROL(
			"[MakeCmd][Trans][Find Teaching Data] AutoRecover=%d, RFReader=%d, TransSound=%d, Oscillation=%d, HandDetectEQ=%d, EQPIODisable_TP=%d",
			pTransData->TransOption.Info.AutoRecovery,
			pTransData->TransOption.Info.RFReader,
			pTransData->TransOption.Info.TransSound,
			pTransData->TransOption.Info.Oscillation,
			pTransData->TransOption.Info.HandDetectEQ,
			pTransData->TransOption.Info.EQPIODisable_TP);
	}

	return Error;

}
/**
 @brief   OHT의 상태변화를 감시하는 부분
 @author  puting
 @date    2015.10.28
 @param   pCmdData : 처리할 Commmand
 @return  있을 경우, true 없으면 false
 */
bool TaskControl::CheckStandardOHTStatus()
 {
	bool bCheckstatus = false;
	char TempCompareStatus_1[64] = {0,};
	char TempCompareStatus_2[64] = {0,};
	int nDifDis_1= m_pOHTMainStatus->StandardOHTStatusReport.RunningDistance/CHECK_DISTANCE;
	int nDifDis_2= m_PreStatusOHT2OCS.RunningDistance/CHECK_DISTANCE;

	memmove(&TempCompareStatus_1,&m_pOHTMainStatus->StandardOHTStatusReport, (sizeof(PROTOCOL_STATUS_OHT)- CHECK_STATUS_POS));
	memmove(&TempCompareStatus_2,&m_PreStatusOHT2OCS,  (sizeof(PROTOCOL_STATUS_OHT)- CHECK_STATUS_POS));

	if((memcmp(&TempCompareStatus_1, &TempCompareStatus_2, (sizeof(PROTOCOL_STATUS_OHT)- CHECK_STATUS_POS)) != 0) ||
	   (nDifDis_1 !=nDifDis_2))
		bCheckstatus = true;

	return bCheckstatus;
}

/**
 @brief   State 상태와 Trans측 상태 동기화 체크하는 함수
 @author  puting
 @date
 */
void TaskControl::CheckSatusCriticalSection()
{
   switch (GetExecuteStatus())
	{
		case STS_LOADING: // Loading
		case STS_UNLOADING: // Unloading
		case STS_AUTO_TEACHING:
		case STS_AUTO_TEACHING_NULLTYPE:
		case STS_AUTO_TRANS_HOMING:

		if(m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute ==true)
			m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute = false;
		break;


		case STS_AUTOPOSITION: // Auto Position
		case STS_TURN_OFF: // 기도우오프
		case STS_AUTORECOVERY: // AUTORECOVERY
		case STS_AUTORECOVERY_BLANKTRANS: // AUTORECOVERY
		case STS_AUTORECOVERY_DOUBLETRANS: // AUTORECOVERY
		case STS_AUTORECOVERY_OVERRUN: // 오버런은 현재 싸이클 종료.
		case STS_ERROR: // Error
		if(m_pExecuteInfo->FlagCmdExcute.bCmdDrivingCMDExcute ==true)
			m_pExecuteInfo->FlagCmdExcute.bCmdDrivingCMDExcute = false;

		if(m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute ==true)
			m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute = false;
 		break;
	}
}

/**
 @brief   Task가 (기존 CycleList 모두) 완료된 상태인지를 체크하는 함수
 @author  puting
 @date    2015.11.20
 @return  CycleList에 Insert 가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 */
bool TaskControl::CheckTaskSatus()
{
	BYTE chkStatus = GetExecuteStatus();
	PKT_CYCLECMDDATA * LastCycleData;
	UINT CycleListCount = m_pCmdManager->GetCycleCount();
	UINT PassOpenListCount = m_pPassPermit->GetPassOpenCount();
//    UINT PathCount =  m_pPathSearch->GetPathCount();
	bool RtnVal = false;

	if ((m_pExecuteInfo->ListOfCmd.getCount() != LISTCOUNT_EMPTY))
//	    (PathCount != LISTCOUNT_EMPTY))
		return false;

	//동기화 부분 추가
	if(m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute ==true ||  m_pExecuteInfo->FlagCmdExcute.bCmdDrivingCMDExcute ==true )
		return false;

	if (CycleListCount != LISTCOUNT_EMPTY)
	{
		//마지막이 수행명령 전달된 상태인지 확인
		LastCycleData =(PKT_CYCLECMDDATA *) m_pCmdManager->ReferCycle(CycleListCount-1);

		//기존 명령 완료 안됨.
		if((LastCycleData->IsExcuteInfo ==false) ||
		   (LastCycleData->IsListF2B ==false) ||
		   (m_pPassPermit->GetPPIndex() != m_pPassPermit->GetPPOIndex()) ||
		   (CycleListCount != (m_pPassPermit->GetPPOIndex()+1)) ||
		   (PassOpenListCount !=LISTCOUNT_EMPTY))
			return false;

//		return true;
	}


	switch (chkStatus)
	{

	case STS_INIT: // Initialized
	case STS_ARRIVED: // Arrived---- 삭제 통과허가 불가로 섰을 경우는 Going으로 표시??
	case STS_UNLOADED: // Unloaded
	case STS_LOADED: // Loaded
	case STS_COMPLETE: // Manual 수행 완료
	case STS_RUNNING: // Manual 수행 중
	case STS_AUTO_TAUGHT:
		RtnVal = true;
		break;

	case STS_GOING: // Going
	case STS_LOADING: // Loading
	case STS_UNLOADING: // Unloading
	case STS_AUTOPOSITION: // Auto Position
	case STS_TURN_OFF: // 기도우오프
	case STS_AUTORECOVERY_OVERRUN: // 오버런은 현재 싸이클 종료.
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
	case STS_ERROR: // Error
	case STS_AUTO_TRANS_HOMING:
		RtnVal = false;
		break;
	}

	return RtnVal;
}

// ---------------------------------------------------------------------------
/**
 @brief   Cycle로 처리가 가능한 상태인지를 체크하는 함수(Manual 상태에서)
 @author  puting
 @date    2015.11.04
 @return  CycleList에 Insert 가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 */
bool TaskControl::CheckManualForCycle()
{
	BYTE chkStatus = GetExecuteStatus();
	bool RtnVal = false;

	if (m_pCmdManager->GetCycleCount() != 0)
		return false;

	switch (chkStatus)
	{

	case STS_INIT: // Initialized
	case STS_ARRIVED: // Arrived
	case STS_UNLOADED: // Unloaded
	case STS_LOADED: // Loaded
	case STS_COMPLETE: // Manual 수행 완료
	case STS_RUNNING: // Manual 수행 중
	case STS_AUTO_TAUGHT:
		RtnVal = true;
		break;

	case STS_GOING: // Going
	case STS_LOADING: // Loading
	case STS_UNLOADING: // Unloading
	case STS_AUTOPOSITION: // Auto Position
	case STS_TURN_OFF: // 기도우오프
	case STS_ERROR: // Error
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
	case STS_AUTO_TRANS_HOMING:
		RtnVal = false;
		break;
	}

	return RtnVal;

}


// ---------------------------------------------------------------------------
/**
 @brief   Cycle이 가능한 상태인지를 체크하는 함수
 @author  puting
 @date    2015.11.04
 @return  CycleList에 Insert 가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 */
bool TaskControl::CheckStatusForCycle()
{
	BYTE chkStatus = GetExecuteStatus();
	bool RtnVal = false;

  //	ADD_LOG_TASKCONTROL("Step1-------------------");
	if (m_pExecuteInfo->ListOfCmd.getCount() != LISTCOUNT_EMPTY)
		return false;

	if (m_pOHTMainStatus->DataUseFlag.fReadySwapping)
		m_pOHTMainStatus->DataUseFlag.fSwapping = true;

	if (m_pOHTMainStatus->DataUseFlag.fReadySwappingParam)
		m_pOHTMainStatus->DataUseFlag.fSwappingParam = true;

	if (m_pOHTMainStatus->DataUseFlag.fSwapping)
		return false;

	if (m_pOHTMainStatus->DataUseFlag.fSwappingParam)
		return false;


	if(m_pOHTMainStatus->IsOnGoCmd == true)
        return false;

	if (m_pCmdManager->GetCycleCount() != LISTCOUNT_EMPTY)
		return false;
   // ADD_LOG_TASKCONTROL("Step2-------------------");
	switch (chkStatus)
	{

	case STS_INIT: // Initialized
	case STS_ARRIVED: // Arrived
	case STS_UNLOADED: // Unloaded
	case STS_LOADED: // Loaded
	case STS_COMPLETE: // Manual 수행 완료
	case STS_AUTOPOSITION: // Auto Position
	case STS_TURN_OFF: // 기동 off
	case STS_AUTO_TAUGHT:
		RtnVal = true;
		break;

	case STS_GOING: // Going
	case STS_LOADING: // Loading
	case STS_UNLOADING: // Unloading
	case STS_RUNNING: // Manual 수행 중
	case STS_ERROR: // Error
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
	case STS_AUTO_TRANS_HOMING:
	default:
		RtnVal = false;
		break;
	}

	return RtnVal;

}

/**
 @brief   Cycle Clear가 가능한 상태인지를 체크하는 함수
 @author  puting
 @date    2016.03.07
 @return  CycleList에 Insert 가능한 상태이면 true, 그렇지 않은 경우 false를 리턴한다.
 */
bool TaskControl::CheckStatusForCycleClear()
{
	BYTE chkStatus = GetExecuteStatus();
	bool RtnVal = false;
	UINT PassOpenListCount = m_pPassPermit->GetPassOpenCount();



	switch (chkStatus)
	{
 	case STS_INIT: // Initialized
	case STS_ARRIVED: // Arrived
	case STS_UNLOADED: // Unloaded
	case STS_LOADED: // Loaded
	case STS_GOING: // Going
	case STS_AUTO_TAUGHT:
		RtnVal = true;
		break;

	case STS_COMPLETE: // Manual 수행 완료
	case STS_LOADING: // Loading
	case STS_UNLOADING: // Unloading
	case STS_AUTOPOSITION: // Auto Position
	case STS_TURN_OFF: // 기도우오프
	case STS_RUNNING: // Manual 수행 중
	case STS_ERROR: // Error
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
	case STS_AUTO_TRANS_HOMING:
	default:
		RtnVal = false;
		break;
	}

	return RtnVal;

}


/**
 @brief   CycleLIst에서 ExcuteInfo로 넘겨주기위해 감시하는 부분
 @author  puting
 @date    2015.10.28
 @param   pCmdData : 처리할 Commmand
 @return  정상인 경우 0을 리턴, 에러 발생 시 에러 코드 리턴
 */
int TaskControl::SuperviseCycleList2ExcuteInfo()
{

	int Error = NO_ERR;
//	bool bCheckBreakPoint = false;
	int  nListCount =  m_pCmdManager->GetCycleCount();
	PKT_CYCLECMDDATA * pCurCmd; // Command를 위한 포인터 변수
	PKT_CYCLECMDDATA * pCurOpenCmd; // Command를 위한 포인터 변수

	static DWORD dwJudgeTrigerTime = timeGetTime();     //함수 진입 시간.
	DWORD dwStartTime = timeGetTime();
 	int nPathListCount = m_pPathSearch->GetPathCount();




	// Step 1) Cycle List를 확인을 통해 명령 생성(통과허가 및 진척보고??)  //Status에서 확인 할 것.
   //	if ((dwStartTime - dwJudgeTrigerTime > (m_pPathSearch->GetPassPermitPriod() + m_pPassPermit->GetPassPeriodTime())))
	if (OHTUTILITY::PreventOverflowGetTimeMs(dwJudgeTrigerTime) > m_pPathSearch->GetPassPermitPriod())
	{
		//통과허가 관련 요청
		try{
		JudgePassPermitReq();

		if(fabs(m_pPassPermit->GetPPIndex() - m_pPassPermit->GetPPOIndex()) >=20)
			return ERR_STD_CMD_CYCLE_FAIL;

		}catch(...)
			ADD_LOG_TASKCONTROL("JudgePassPermitReq Exception!!!!!!!");
        dwJudgeTrigerTime = dwStartTime;

	}
	try{
	//상시 체크해야될 부분 추가.
	CheckAbsolutePosition();    // 누적거리 계산
	JudgeCIDReportAndIgnor();
	JudgeProgressReq();         //진척보고는 무조건 감시(20151210)
	JudgeOHTInOutReq();         //투입 빼냄에 대한 보고
	JudgeMAPUpdate();


	}catch(...)
		ADD_LOG_TASKCONTROL("Judge Exception!!!!!!!");


	nListCount =  m_pCmdManager->GetCycleCount();

	// Step 2)Cycle List 내용을 확인하여 Excute Thread로 전달해주는 부분
	if(nListCount != LISTCOUNT_EMPTY)
	{
		//앞부분만 확인하는 방향으로도 검토 할것. puting
		//전체 다 하는게 좋다.


			// 내용 확인
			pCurCmd = (PKT_CYCLECMDDATA*) m_pCmdManager->ReferCycle(m_pPassPermit->GetPPIndex());
			pCurOpenCmd = (PKT_CYCLECMDDATA*) m_pCmdManager->ReferCycle(m_pPassPermit->GetPPOIndex());

			//통과허가 유무 판단.   수정 필요 (20160323)

			if(m_pPassPermit->GetPassPermitCount() != LISTCOUNT_EMPTY)
			{
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.PassConfirmGet = ON;
			}
			else
				m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.TunDirection.PassConfirmGet = OFF;

		

			// DTM에 전달되는 곳 처리 부분 puting
			switch (pCurCmd->CycleType)
			{
				case CYCLETYPE_DRIVING: // 주행 관련 명령
					// 1.1 주행명령 처리가 가능한 상태인지 확인.

					if(CheckStatusForDriving())
					{
						//ExcuteInfo가 false인 가장 첫번째만 확인함.

							 //Case 1. 통과허가 명령은 받았으며, 아직 Excueinfo에 전달이 안된 경우,
							 if((pCurCmd->EXE_CycleDriving_Info.D_PPStep >= CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE) &&
								(m_pOHTMainStatus->bCheckRollbacking == false) &&
								(pCurCmd->IsNoExcuteCycle == false))
								{
									//기동 Off에 대한 예외처리.
									if((GetCurrentMode() == MODE_AUTO) &&
									((m_pOHTMainStatus->StandardStatusOHT.PauseStatus.OHT_StartUp ==OFF) || (m_pOHTMainStatus->StandardStatusOHT.StandardOHTStatus.WorkingStatus.NowRunning ==OFF)))
										break;

									if(pCurCmd->IsExcuteInfo == false)
									{
                                        m_pExecuteInfo->FlagCmdExcute.bCmdDrivingCMDExcute = true; //주행명령 하달 유무 Set

										ADD_LOG_TASKCONTROL("SuperviseCycleList2ExcuteInfo_CYCLETYPE_DRIVING_Count%d:Step%d:IsExcute%d",nListCount,pCurCmd->EXE_CycleDriving_Info.D_PPStep, pCurCmd->IsExcuteInfo);
										try{
											Error = MakeGoExcuteInfo(pCurCmd,&m_pExecuteInfo->ListOfCmd);
										}
										catch(...)
											ADD_LOG_TASKCONTROL("SuperviseCycleList2ExcuteInfo Exception!!!");

										if(m_pExecuteInfo->bPopListOfCmd == false)
											m_pExecuteInfo->bPopListOfCmd = true;
									}

									pCurCmd->IsExcuteInfo = true;


//									m_pPassPermit->SetIncreasePPIndex(nListCount-1);

								}

					}

//					else 	// 1.2 주행명령 처리가 불가능하면 나감.
//						   bCheckBreakPoint = true;
					break;

				case CYCLETYPE_TRANS: // 이적재 관련 명령
					//2.1 이적재 명령이 처리가능한 상태인지 확인.
					m_pOHTMainStatus->bRealGoCycleing = false;   //주행 완료 플래그

					 if ((CheckStatusForTrans() &&  bHoistJig == true))
					   //(m_pOHTMainStatus->bCheckRollbacking == false) &&
					   //(m_pExecuteInfo->ExecutePositionInfo.CurrStation == pCurCmd->EXE_Trans_Info.TransCmd.CurStationID))
					{
						ADD_LOG_TASKCONTROL("SuperviseCycleList2ExcuteInfo_CYCLETYPE_TRANS_Count%d:IsExcute%d",nListCount, pCurCmd->IsExcuteInfo);

//						//이재 작업전 현재 위치가 도착위치라는 것을 무조건 재 전송. 	//김승현K 추가
//						ADD_LOG_TASKCONTROL("[UPDOWN] IsExcuteInfo : %d Manual : %d TransType : %d",
//											pCurCmd->IsExcuteInfo, CheckStatusForManual(), pCurCmd->EXE_Trans_Info.TransType);
						//이재 작업전 현재 위치가 도착위치라는 것을 무조건 재 전송.
						if(pCurCmd->IsExcuteInfo == false)
						{
							if (GetCurrentMode() == MODE_MANUAL)
							{
								if((pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_LOAD)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_UNLOAD)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_MOVET)
                                || (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_MOVE_ALL_HOME)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_AUTOTEACHING)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_AUTOTEACHING_NULLTYPE)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_SMALLADD_HOIST_HOME)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_AUTOSET_HOME)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_AUTO_CHANGE))
								   {
										Error = MakeTransExcuteInfo(pCurCmd, &m_pExecuteInfo->ListOfCmd);
										pCurCmd->IsExcuteInfo = true;
										pCurCmd->IsListF2B = true;
										m_pExecuteInfo->bPopListOfCmd = true;
								   }
							}
							else
							{
// 2017-01-03, Add by Labeler
// V7.5 Test Bed에서 임시로 이적재 롱런 돌리기 위해 임시 처리
								if(((m_pOHTMainStatus->StandardStatusOHT.StandardOrderStatus.CraneDownAdmission ==ON) && (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_LOAD)) ||
									((m_pOHTMainStatus->StandardStatusOHT.StandardOrderStatus.CraneUpAdmission ==ON) && (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_UNLOAD)))
								   {

								   		m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute = true; //이적재 하달 유무 Set(Auto시만 적용)
										Error = MakeTransExcuteInfo(pCurCmd, &m_pExecuteInfo->ListOfCmd);
										pCurCmd->IsExcuteInfo = true;
										pCurCmd->IsListF2B = true;
										m_pExecuteInfo->bPopListOfCmd = true;
								   }
							}
						}
					}



					if (CheckStatusForTrans() &&  bHoistJig == false && (m_pOHTMainStatus->bCheckRollbacking == false) &&
					   (m_pExecuteInfo->ExecutePositionInfo.CurrStation == pCurCmd->EXE_Trans_Info.TransCmd.CurStationID))
					{
						ADD_LOG_TASKCONTROL("SuperviseCycleList2ExcuteInfo_CYCLETYPE_TRANS_Count%d:IsExcute%d",nListCount, pCurCmd->IsExcuteInfo);

//						//이재 작업전 현재 위치가 도착위치라는 것을 무조건 재 전송. 	//김승현K 추가
//						ADD_LOG_TASKCONTROL("[UPDOWN] IsExcuteInfo : %d Manual : %d TransType : %d",
//											pCurCmd->IsExcuteInfo, CheckStatusForManual(), pCurCmd->EXE_Trans_Info.TransType);
						//이재 작업전 현재 위치가 도착위치라는 것을 무조건 재 전송.
						if(pCurCmd->IsExcuteInfo == false)
						{
							if (GetCurrentMode() == MODE_MANUAL)
							{
								if((pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_LOAD)
                                || (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_UNLOAD)
                                || (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_MOVET)
                                || (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_MOVE_ALL_HOME)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_AUTOTEACHING)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_AUTOTEACHING_NULLTYPE)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_SMALLADD_HOIST_HOME)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_AUTOSET_HOME)
								|| (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_AUTO_CHANGE))
								   {
										Error = MakeTransExcuteInfo(pCurCmd, &m_pExecuteInfo->ListOfCmd);
										pCurCmd->IsExcuteInfo = true;
										pCurCmd->IsListF2B = true;
										m_pExecuteInfo->bPopListOfCmd = true;
								   }
							}
							else
							{
// 2017-01-03, Add by Labeler
// V7.5 Test Bed에서 임시로 이적재 롱런 돌리기 위해 임시 처리
								if(((m_pOHTMainStatus->StandardStatusOHT.StandardOrderStatus.CraneDownAdmission ==ON) && (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_LOAD)) ||
									((m_pOHTMainStatus->StandardStatusOHT.StandardOrderStatus.CraneUpAdmission ==ON) && (pCurCmd->EXE_Trans_Info.TransType == TRANSTYPE_UNLOAD)))
								   {

								   		m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute = true; //이적재 하달 유무 Set(Auto시만 적용)
										Error = MakeTransExcuteInfo(pCurCmd, &m_pExecuteInfo->ListOfCmd);
										pCurCmd->IsExcuteInfo = true;
										pCurCmd->IsListF2B = true;
										m_pExecuteInfo->bPopListOfCmd = true;
								   }
							}
						}
					}

				   //	bCheckBreakPoint = true;
					break;

				case CYCLETYPE_MANUAL: // 메뉴얼 명령
					if (CheckStatusForManual() &&
					   (pCurCmd->IsExcuteInfo == false))
					 {
						ADD_LOG_TASKCONTROL("SuperviseCycleList2ExcuteInfo_CYCLETYPE_MANUAL_Count%d:IsExcute%d",nListCount, pCurCmd->IsExcuteInfo);
						Error = MakeManualExcuteInfo(pCurCmd, &m_pExecuteInfo->ListOfCmd);
						pCurCmd->IsExcuteInfo = true;
						pCurCmd->IsListF2B = true;

						InitClearCycle(true);
						InitExcuteOverRun();
						ChangeCmdIDAndCycle(0,0,0,true);

						return Error;
					 }
				   //	bCheckBreakPoint = true;
					break;

				default:
					///@todo 에러 코드 정의 필요
					Error = ERR_STD_INNER_DATA_FAIL;
				   //	bCheckBreakPoint = true;
					break;
			}


		// Step 3)통과허가 상태확인 및 최종 위치 확인 중 (작업변경으로 인하여 추가). 주행일 경우만 확인.
		if(m_pOHTMainStatus->bCheckRollbacking  == false)
		{
			if(nListCount !=  m_pPassPermit->GetPPIndex() +1)
			{
				if((pCurCmd->CycleType == CYCLETYPE_DRIVING) &&
				   (pCurCmd->IsExcuteInfo == true) &&
				   (pCurCmd->EXE_CycleDriving_Info.D_PPStep >= CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE))
				   {
						m_pPassPermit->SetIncreasePPIndex(nListCount-1);
						ADD_LOG_TASKCONTROL("SetIncreasePPIndex Checking1");
				   }
			}else
			{
				//마지막 목적지일 경우
				if(m_pHWSet->m_pDrivingAxis->IsTargetPos() == false)
				{
					if(m_pPassPermit->GetPPIndex() == 0)
					{
						if(pCurCmd->CycleType == CYCLETYPE_DRIVING)
						{
							if((m_pExecuteInfo->ExecuteDrivingStep > DRIVING_CTRL_STEP_GO_PREPARE) &&
							  (GetExecuteStatus()==STS_GOING))
							{
								m_pHWSet->m_pDrivingAxis->SetTargetPos();
								ADD_LOG_TASKCONTROL("SetTarget!");
							}
						}
					}
					else
					{
						if((m_pExecuteInfo->ExecuteDrivingStep > DRIVING_CTRL_STEP_GO_PREPARE) &&
						  (GetExecuteStatus()==STS_GOING))
						{
							m_pHWSet->m_pDrivingAxis->SetTargetPos();
							ADD_LOG_TASKCONTROL("SetTarget!");
						}
                    }
				}
			}

			if(nListCount !=  m_pPassPermit->GetPPOIndex() +1)
			{
				if((pCurOpenCmd->CycleType == CYCLETYPE_DRIVING) &&
				   (pCurOpenCmd->IsExcuteInfo == true) &&
				   (pCurOpenCmd->EXE_CycleDriving_Info.D_PPOStep >= NEXTCYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_OPEN_RESPONSE))
				   {
						if((pCurOpenCmd->EXE_CycleDriving_Info.D_PPOStep == NEXTCYCLESTEP_DRIVING_MEANINGLESS) &&
						   (pCurOpenCmd->EXE_CycleDriving_Info.D_PPStep >= CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE) &&
						   (pCurOpenCmd->IsListF2B == false))
						   //(nListCount ==  m_pPassPermit->GetPPOIndex()))
						{
							if((pCurOpenCmd->EXE_CycleDriving_Info.StartPoint == m_pExecuteInfo->ExecutePositionInfo.CurrNode) ||
							(pCurOpenCmd->EXE_CycleDriving_Info.EndPoint == m_pExecuteInfo->ExecutePositionInfo.CurrNode) ||
							((double)((double)pCurOpenCmd->EXE_CycleDriving_Info.StartDistance - (double)m_pPassPermit->GetAbsolutePositon()) < (double)m_pOHTMainStatus->dOHTTotalRunDis))

							{
								pCurOpenCmd->IsListF2B = true;
								m_pPassPermit->SetIncreasePPOIndex(m_pPassPermit->GetPPIndex());
								ADD_LOG_TASKCONTROL("SetIncreasePPOIndex Checking3");
							}

						   //	ADD_LOG_TASKCONTROL("SetIncreasePPOIndex Checking %d",pCurOpenCmd->EXE_CycleDriving_Info.EndPoint);
						}
						else
						{
							m_pPassPermit->SetIncreasePPOIndex(m_pPassPermit->GetPPIndex());
							ADD_LOG_TASKCONTROL("SetIncreasePPOIndex Checking2");
						}


				   }
			}

			//싸이클 정상처리중일때는 항시 On처리.
//			if(m_pHWSet->m_pDrivingAxis->IsTargetPos() == false)
//			{
//				if((m_pExecuteInfo->ExecuteDrivingStep > DRIVING_CTRL_STEP_GO_PREPARE) &&
//				  (GetExecuteStatus()==STS_GOING))
//				{
//					m_pHWSet->m_pDrivingAxis->SetTargetPos();
//					ADD_LOG_TASKCONTROL("SetTarget!");
//				}
//			}
		}

        // Step 4)Cycle List 내용 삭제 하는  부분(Path List 내용도 확인할 것.)_ 현재 작업지시가 완료되었을 경우에 실시.
		if(CheckTaskSatus())
		{
			ADD_LOG_TASKCONTROL("CheckTaskSatus Init");

			//빼냄 싸이클일 경우일 경우, 메뉴얼 처리.
			if(m_pOHTMainStatus->CurCycle == CYCLE::TakeOut)
			{
				ChangeMode(MODE_MANUAL);
				m_pCmdManager->InitTaskList();
				Change_FDC_Manual_Mode(0);
			}

			InitClearCycle();
			InitExcuteOverRun();

			ChangeCmdIDAndCycle(0,0);
		}

		//동기화 처리.
		CheckSatusCriticalSection();

	}

	return Error;

}
/**
@brief	진단에 보낼 Abnormal 데이터를 업데이트
@data   2016.05.10
@author	doori.shin
*/
void TaskControl::UpdateAbnormalDiagData()
{
	if(m_defualtparam->strDiagIpAddr != "")
	{
		int nCount = m_DiagManager->GetAbnormalDiagDataList()->getCount();

		if(nCount > 0)
		{
			for(int i=0; i<nCount; i++)
			{
				DIAG_OHT_ABNORMAL* pSendData = (DIAG_OHT_ABNORMAL*)m_DiagManager->GetAbnormalDiagDataList()->popFront();

				if(pSendData != NULL)
				{
					SendAbnormalDiagData(pSendData);

					Sleep(1);
				}

				delete pSendData;
            }
        }
    }
}

/**
@brief	진단에 보낼 통계 데이터를 업데이트
@data   2016.05.10
@author	doori.shin
*/
void TaskControl::UpdateStatisticsDiagData()
{
	if(m_defualtparam->strDiagIpAddr != "")
	{
		DIAG_STAT_DATA_MAP* pDataMap = m_DiagManager->GetDiagStatMap();

		if(pDataMap != NULL)
		{
			for(DIAG_STAT_DATA_MAP_ITER iter = pDataMap->begin();
				iter!=pDataMap->end(); iter++)
			{
				DIAG_OHT_STATISTICS_DATA* pData = iter->second;

				if(pData->IsReadyToSend() == true)
				{
					DIAG_OHT_STATISTICS* pSendData = pData->MakeSendData();

					if(pSendData != NULL)
					{
						pData->dPreSendData = pSendData->nData;
						SendStatDiagData(pSendData);
						Sleep(1);
						delete pSendData;
                    }
                }
            }
		}
	}



}
/**
@brief	진단에 보낼 OHT상태정보 업데이트
@data   2016.05.10
@author	doori.shin
*/
void TaskControl::UpdateStatusDiagData()
{
	if(m_DiagManager->GetDiagCheckCount() >= DIAG_SEND_INTERVAL)
	{
		if(m_defualtparam->strDiagIpAddr != "")
		{
			SendStatusDiagData(m_DiagManager->GetDiagOHTstatus());
			m_DiagManager->ResetDiagCheckCount();
		}
	}
}

/**
@brief	진단에 보낼 CID정보 업데이트
@data   2016.05.10
@author	doori.shin
*/
void TaskControl::UpdateCIDStatusDiagData()
{
	if(m_defualtparam->strDiagIpAddr != "")
	{
	   if(m_pExecuteInfo->FlagCIDMonitoring.bCIDFlag ==true)
	   {
	   		// char Temp[38] = "OHT0010800001,CID001122240400800000001";
			DIAG_PKT_BODY_CID_REPORT pSendData;
			pSendData.StopNodeID = m_pExecuteInfo->FlagCIDMonitoring.nStopNodeID;
			pSendData.ResetNodeID = m_pExecuteInfo->FlagCIDMonitoring.nResetNodeID;
			pSendData.Reseult = m_pExecuteInfo->FlagCIDMonitoring.cResult;			

			// memmove(pSendData.CIDStatus,&Temp,38);
			memmove(pSendData.CIDStatus,(char *)m_pExecuteInfo->FlagCIDMonitoring.cMonitoring,53);	// CID_Monitoring_PKT, CID_Monitoring_BODY size check
			memmove(pSendData.CIDErrorInfo,(char *)m_pExecuteInfo->FlagCIDInfo.sCIDStatus,3);	// CID_Monitoring_PKT, CID_Monitoring_BODY size check
			ADD_LOG_TASKCONTROL("[CID] Monitoring to Agent ID:124. Stop Node ID : %d  Reset Node ID : %d  Result : %d"
								,m_pExecuteInfo->FlagCIDMonitoring.nStopNodeID
								,m_pExecuteInfo->FlagCIDMonitoring.nResetNodeID
								,m_pExecuteInfo->FlagCIDMonitoring.cResult);
			
			// Conv_DIAG_PKT_BODY_CID_REPORT(pSendData);
			SendCIDStatusDiagData(&pSendData);

			m_pExecuteInfo->FlagCIDMonitoring.bCIDFlag = false;
		}
	}
	else
	{
//		ADD_LOG_TASKCONTROL("[CID] Diag Ip Addr Not Found %s",m_defualtparam->strDiagIpAddr); //timy 불필요한 LOG 삭제
	}
}

/**
@brief	진단에 이상정보를 보냄
@data   2016.05.10
@author	doori.shin
*/
void TaskControl::SendAbnormalDiagData(DIAG_OHT_ABNORMAL* pSendData)
{
	PKT_ACK_OHT2DIAG AckPkt;
	DIAG_PKT_BODY_ERROR_REPORT Temp;

	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2DIAG));

	Temp.ErrorID = pSendData->bDataType;
	Temp.NodeID = pSendData->nNodeID;

	AckPkt.AckPkt.HEADER.CommandID = DiagCmdID::DIAG_INFO_ERROR_REPORT;
	memmove(&AckPkt.AckPkt.Value, &Temp, sizeof(DIAG_PKT_BODY_ERROR_REPORT));
	AckPkt.Length = sizeof(DIAG_PKT_ERROR_REPORT);

	ADD_LOG_TASKCONTROL("SendAbnormalDiagData %d %x %x %x %x %x",Temp.ErrorID,Temp.NodeID,
	AckPkt.AckPkt.Value[1],AckPkt.AckPkt.Value[2],AckPkt.AckPkt.Value[3],AckPkt.AckPkt.Value[4]);

	MakeResponseStandardOHT2DIAG(&AckPkt);
}

/**
@brief	진단에 통계정보를 보냄
@data   2016.05.10
@author	doori.shin
*/
void TaskControl::SendStatDiagData(DIAG_OHT_STATISTICS* pSendData)
{
	PKT_ACK_OHT2DIAG AckPkt;
	DIAG_PKT_BODY_INFO_DATA_RESPONSE Temp;

	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2DIAG));
	Temp.Parameter =  pSendData->bInfoID;
	Temp.Value =  pSendData->nData;

	AckPkt.AckPkt.HEADER.CommandID = DiagCmdID::DIAG_INFO_DATA_RESPONSE;
	memmove(&AckPkt.AckPkt.Value, &Temp, sizeof(DIAG_PKT_BODY_INFO_DATA_RESPONSE));
	AckPkt.Length = sizeof(DIAG_PKT_INFO_DATA_RESPONSE);

	ADD_LOG_TASKCONTROL("SendStatusDiagData %d %d",Temp.Parameter,Temp.Value);

	MakeResponseStandardOHT2DIAG(&AckPkt);
}

/**
@brief	진단에 OHT상태정보를 보냄
@data   2016.05.10
@author	doori.shin
*/
void TaskControl::SendStatusDiagData(DIAG_OHT_STATUS_DATA* pSendData)
{

	PKT_ACK_OHT2DIAG AckPkt;
	DIAG_PKT_BODY_INFO_STATUS_REPORT Temp;

	Temp.IsUSBWrite = pSendData->bIsLogSave;
	Temp.IsUSBCheck = pSendData->bIsEnabledUSB;
	Temp.IsFANCheck = pSendData->bIsEnabledFAN;
	Temp.IsC_Check = pSendData->bIsAvailableCDrive;
	Temp.IsD_Check = pSendData->bIsAvailableDDrive;

	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2DIAG));

	AckPkt.AckPkt.HEADER.CommandID = DiagCmdID::DIAG_INFO_STATUS_REPORT;
	memmove(&AckPkt.AckPkt.Value, &Temp, sizeof(DIAG_PKT_BODY_INFO_STATUS_REPORT));
	AckPkt.Length = sizeof(DIAG_PKT_INFO_STATUS_REPORT);

	ADD_LOG_TASKCONTROL("SendStatusDiagData");

	MakeResponseStandardOHT2DIAG(&AckPkt);

}

/**
@brief	진단에 CID 상태정보를 보냄
@data   2016.05.10
@author	doori.shin
*/
void TaskControl::SendCIDStatusDiagData(DIAG_PKT_BODY_CID_REPORT* pSendData)
{

	PKT_ACK_OHT2DIAG AckPkt;

	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2DIAG));

	AckPkt.AckPkt.HEADER.CommandID = DiagCmdID::DIAG_INFO_CIDMONITORING_REPORT;
	memmove(&AckPkt.AckPkt.Value, pSendData, sizeof(DIAG_PKT_BODY_CID_REPORT));
	AckPkt.Length = sizeof(DIAG_PKT_CID_REPORT);

	ADD_LOG_TASKCONTROL("[CID] Send CID Status Diag Data");

	MakeResponseStandardOHT2DIAG(&AckPkt);

}

/**
@brief	진단에 OHT 통계정보를 요청함
@data   2016.05.10
@author	puting
*/
void TaskControl::SendReqeustDiagStatData()
{
	PKT_ACK_OHT2DIAG AckPkt;

	memset(&AckPkt, 0x00, sizeof(PKT_ACK_OHT2DIAG));

	AckPkt.AckPkt.HEADER.CommandID  = DiagCmdID::DIAG_INFO_STATISTICS_REQUEST;
	AckPkt.Length = sizeof(DIAG_PKT_DATA_INFO_REQUEST);

   //	ADD_LOG_TASKCONTROL("SendReqeustDiagStatData");

	MakeResponseStandardOHT2DIAG(&AckPkt);
}

/*
	Move Start - End Node
*/
int TaskControl::ManualMoving(int iStartNode, int iStopNode, bool iIsStation, int iPortType, int iCycle)
{

    int nError = NO_ERR;
	bool bSearchResult = false;
	int nSTB_M_Offset = 0;

    int iStationID = 0;
    STATION_DATA * pStationData = NULL;
    TEACHING_DATA * pTeachingData = NULL;

	int iTemp, iDirection, iCount, iMarkType;
	unsigned int iPathList[500];
	unsigned char cHigh, cMid, cLow;
	AnsiString strMachineID = "TP001";

    int iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
    PKT_JOB_ORDER_ID_121	SEND_PACKET;
    memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어


	if(m_pExecuteInfo->ExecutePositionInfo.Marked == true)
	{
		// Station으로 이동 시에는, iStopNode 매개변수에, Target Station 정보가 담김
		if(iIsStation == true)
		{
			iStartNode = m_pExecuteInfo->ExecutePositionInfo.CurrNode;
            iStationID = iStopNode;

            pStationData = OHTMainForm->GetStationDataManager()->Find(iStationID);

            if(pStationData == NULL)
            {
                nError = ERR_DATA_STATION_NOEXIST;
                ADD_LOG_TASKCONTROL("[ERR_DATA_STATION_NOEXIST] Station Id: %d", iPortType);
            }
            else
            {
				iStopNode = pStationData->ParentNodeID;
			}

			if(nError == NO_ERR)
			{
				if(pStationData->StationType == MARK_TYPE_QR_STATION_LEFT)
					iPortType = PORT_TYPE_STB_L;
				else if(pStationData->StationType == MARK_TYPE_QR_STATION_RIGHT)
					iPortType = PORT_TYPE_STB_R;
			}

		}
		else
		{
			iStartNode = m_pExecuteInfo->ExecutePositionInfo.CurrNode;
		}
    }
	else
	{
		nError = MAP_MOVE_ERR_NOT_MARKED;
	}


    if(nError == NO_ERR)
	{
		if((iCycle == (int)CYCLE::CraneDown) || (iCycle == (int)CYCLE::CraneUp) || (iCycle == (int)CYCLE::MovingToStation))
		{
			if(iIsStation == false)
			{
                nError = MAP_MOVE_ERR_NOT_STATION_INFO;
			}
			else
			{

				if(pStationData->StationType == MARK_TYPE_QR_STATION_LEFT)
					iPortType = PORT_TYPE_STB_L;
				else if(pStationData->StationType == MARK_TYPE_QR_STATION_RIGHT)

					iPortType = PORT_TYPE_STB_R;

				pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(iStationID, iPortType);

				if(pTeachingData == NULL)
				{
//					nError = ERR_DATA_TEACHING_NOEXIST;
					ADD_LOG_TASKCONTROL("[ERR_DATA_TEACHING_NOEXIST] Station Id: %d, PortType : %d", iStationID, iPortType);
				}

			}
		}

		// 2017-01-21, Add by Labeler, Port Type이 미 입력된 경우에도, Station으로 이동 가능 하도록 변경
		if(nError == NO_ERR)
		{
			if((iCycle == (int)CYCLE::CraneDown) || (iCycle == (int)CYCLE::CraneUp) || (iCycle == (int)CYCLE::TP_CheckPortTeaching))
			{
				if(iIsStation == true)
				{
					if(pStationData->StationType == MARK_TYPE_QR_STATION_LEFT)
						iPortType = PORT_TYPE_STB_L;
					else if(pStationData->StationType == MARK_TYPE_QR_STATION_RIGHT)
						iPortType = PORT_TYPE_STB_R;

					pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(iStationID, iPortType);

					if(pTeachingData == NULL)
					{
//						nError = ERR_DATA_TEACHING_NOEXIST;
						ADD_LOG_TASKCONTROL("[ERR_DATA_TEACHING_NOEXIST] Station Id: %d, PortType : %d", iStationID, iPortType);
					}
				}
			}
		}
	}


     if(nError == NO_ERR)
	{
		// Start Node와 Target Node가 다를 때에만, Path Search 수행
		bSearchResult = ManualMovePathSearch(iStartNode, iStopNode, iDirection, iCount, iPathList);


		ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] StartNID : %d,  StopNID : %d,  IsStation : %d, StopStation : %d, Dir : %d Count : %d  Path[0] : %d  bSearchResult : %d",
				iStartNode, iStopNode, iIsStation, iStationID, iDirection, iCount, iPathList[0], bSearchResult);

		if(bSearchResult == true)
		{
			ADD_LOG_TASKCONTROL("[MOVE_BY_MAP]  Make Packet....");

			// ===============================
			//		Step 1. make packet
    		// ===============================

			// (2) 고정 정보를 채워 넣는다
			// --- Header
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
			memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

			// --- Tag
			SEND_PACKET.TAG.Value = TagID::JOB_ORDER_TP;	// 작업지시

			// --- Status
			// 상태지시 - 기동 ON
			// SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;
			// 정지지시, 정지요인, 유지보수지시, E/M지시, 데이터 전송지시  - 설정 생략

    		// =============================== Body ===============================
            if(m_pOHTMainStatus->CurCmdID >= 9)
			{
    			SEND_PACKET.BODY.CmdID = 1;	// 지시 ID
            }
            else
            {
    			SEND_PACKET.BODY.CmdID = m_pOHTMainStatus->CurCmdID + 1;	// 지시 ID
            }

			iTemp = ORDERDIV::JobOrder; 					// 지시구분 - 작업지시
			memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// 지시구분

			// 이적재 Buffer 위치
			SEND_PACKET.BODY.BufferType.Buffer_1 = 1;			// 제1 수하대

			iTemp = iCycle;
			// iTemp = CYCLE::MovingToStation; 				// 지시사이클 - 이동 사이클
			memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);

			// 우선도
			RIV tmpRv;
			tmpRv.Riv_Value = 64;							// 7비트니까
			tmpRv.Hotlot = 0;								// Hotlot - off
			memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

			if(iDirection == DefaultDirection::DIR_LEFT)
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
			else
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;

            if(iIsStation == true)
            {
                if(m_pExecuteInfo->ExecutePositionInfo.CurrStation == iStationID)
				{
        			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 0;		// Driving flag
                }
                else
                {
					SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag
                }
            }
			else
            {
                if(iStartNode == iStopNode)
                {
        			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 0;		// Driving flag
                }
                else
				{
        			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag
                }
            }

			// 시작 NODE
			Int2Uchar3(iStartNode, cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

			// 목적 NODE
			Int2Uchar3(iStopNode, cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

			// STOP NODE
			SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;


			if(iIsStation == TRUE)
			{
				Int2Uchar3(iStationID, cHigh, cMid, cLow);

				// 목적지 STATION
				SEND_PACKET.BODY.StopStationID[HIGH_BYTE]   = cHigh;
				SEND_PACKET.BODY.StopStationID[MID_BYTE]    = cMid;
				SEND_PACKET.BODY.StopStationID[LOW_BYTE]    = cLow;

				SEND_PACKET.BODY.StationData_MarkType 		= pStationData->StationType;
				SEND_PACKET.BODY.StationData_Offset 		= pStationData->StationOffset;
				SEND_PACKET.BODY.StationData_ParentNodeID 	= pStationData->ParentNodeID;
				SEND_PACKET.BODY.StationData_NextNodeID 	= pStationData->NextNodeID;

				SEND_PACKET.BODY.StationData_StationID 		= iStationID;

				if(pTeachingData != NULL)
				{
					SEND_PACKET.BODY.TeachingData_PortType 	= pTeachingData->PortType;
					SEND_PACKET.BODY.TeachingData_Hoist	 	= pTeachingData->Hoist;
					SEND_PACKET.BODY.TeachingData_Shift 	= pTeachingData->Shift;
					SEND_PACKET.BODY.TeachingData_Rotate 	= pTeachingData->Rotate;

                    SEND_PACKET.BODY.TeachingData_StationID		= pTeachingData->StationID;
					SEND_PACKET.BODY.TeachingData_PIOType		= pTeachingData->PIOType;
					SEND_PACKET.BODY.TeachingData_PIODirect		= pTeachingData->PIODirection;
					SEND_PACKET.BODY.TeachingData_PIOID			= pTeachingData->PIOID;

                    SEND_PACKET.BODY.TeachingData_PIOChannel	= pTeachingData->PIOChannel;
					SEND_PACKET.BODY.TeachingData_PIOTimeLevel	= pTeachingData->PIOTimeLevel;
					SEND_PACKET.BODY.TeachingData_AutoRecovery	= pTeachingData->TransOption.Info.AutoRecovery;
                    SEND_PACKET.BODY.TeachingData_HandDetectEQ  = pTeachingData->TransOption.Info.HandDetectEQ;
					SEND_PACKET.BODY.TeachingData_STBOffset		= pTeachingData->STBOffSet;
					SEND_PACKET.BODY.TeachingData_LookDownLevel	= pTeachingData->LookDownLevel;
					SEND_PACKET.BODY.TeachingData_Reserved[0]   = 0; 	// not use
				}
//				 nSTBOffset =   pStationData->StationOffset;
//				 nSTBQROffset =  pTeachingData->STBOffSet;
			  //	nSTB_M_Offset

				 if(iPortType == PORT_TYPE_STB_L || iPortType == PORT_TYPE_STB_R)
				 {
						 nSTB_M_Offset = pStationData->StationOffset + pTeachingData->STBOffSet;
						 SEND_PACKET.BODY.StationData_Offset =  nSTB_M_Offset;
				 }
			}

			/* 아래는 Map Info */
			SEND_PACKET.BODY.ChangeNode.Map_Num			= 0x01;	// 1 floor
			SEND_PACKET.BODY.AutoRecovery				= 0x00;	// 지정 없음 (CLW가 CLW의 파라미터 설정에 따라 독자적으로 판단한다.)
			SEND_PACKET.BODY.StationData_Reserved[0]	= 0;	// not use


    		// =============================== Path ===============================
			SEND_PACKET.BODY.PathData_ExtraNum = iCount;

			for(int i = 0; i < iCount; i++)
			{
				ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] Count : %d  Path : %d", i, iPathList[i]);

				Int2Uchar3(iPathList[i], cHigh, cMid, cLow);

				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
			}
		}
        else
        {
            nError = MAP_MOVE_ERR_MAP_SEARCH_FAIL;
		}
	}

    if(nError == NO_ERR)
    {
        // ===============================
        //		Step 2. internal process
        // ===============================
        bool bCheck = false;
        bool bCheckInsertTaskList = false;
        int  QuickCmd = 0;

        PKT_ACK_OHT2OCS RtnACKPkt;
        PKT_OHT2TP_RESPONSE RtnACKPktPC;

        PKT_TASKCMDDATA pTempCmdPktData;
        memset(&pTempCmdPktData, 0x00, sizeof(PKT_TASKCMDDATA));
        pTempCmdPktData.CategoryDiv = CATEGORY_TERTIARY;

        bCheck = CheckCmdJobOrder((PKT_OCS2OHT*)&SEND_PACKET, &RtnACKPkt,&pTempCmdPktData.DetailTagSort, QuickCmd, pTempCmdPktData.CategoryDiv);

        ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] Step 2. internal process - bCheck : %d", bCheck);

        // ===============================
        //		Step 3. insert command
        // ===============================
        // Step.3 Task List에 접수 가능 할때 접수하는 부분 처리 .
        if(bCheck)
        {
            ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] Step 3. Insert command..");

            memmove(&(pTempCmdPktData.PKTDATA), &SEND_PACKET, sizeof(PKT_OCS2OHT));
            // TASK CMD LIst에 추가될 경우에 여기 추가
			// 순서에 맞게 저장할 것.
            m_pCmdManager->MakeTaskList_Standard(&pTempCmdPktData);
		}
	}

    return nError;
}

int TaskControl::ManualMovingMapMake(int iStartNode, int iStopNode,BYTE MapMakeType)
{

    int nError = NO_ERR;
	bool bSearchResult = false;
	int nSTB_M_Offset = 0;

    int nMapMakeTargetNode = 0;
    STATION_DATA * pStationData = NULL;
    TEACHING_DATA * pTeachingData = NULL;

	int iTemp, iDirection, iCount, iMarkType;
	unsigned int iPathList[500];
	unsigned char cHigh, cMid, cLow;
	AnsiString strMachineID = "TP001";

    int iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
    PKT_JOB_ORDER_ID_121	SEND_PACKET;
    memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어


	if(m_pExecuteInfo->ExecutePositionInfo.Marked == true)
	{

		iStartNode = m_pExecuteInfo->ExecutePositionInfo.CurrNode;

    }
	else
	{
		nError = MAP_MOVE_ERR_NOT_MARKED;
	}


	 if(nError == NO_ERR)
	{
		// Start Node와 Target Node가 다를 때에만, Path Search 수행
		if(MapMakeType == 0x01)
			nError = ManualMovePathSearchMapMake(iStartNode, iStopNode, iDirection, iCount, iPathList,nMapMakeTargetNode);
        else if(MapMakeType == 0x02)
			nError = ManualMovePathSearchMapMake(iStartNode, iStopNode, iDirection, iCount, iPathList,nMapMakeTargetNode, true);

		ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] StartNID : %d,  StopNID : %d(%d), Dir : %d Count : %d  Path[0] : %d  bSearchResult : %d",
				iStartNode, iStopNode,nMapMakeTargetNode, iDirection, iCount, iPathList[0], bSearchResult);

		if(nError == NO_ERR)
		{
			ADD_LOG_TASKCONTROL("[MOVE_BY_MAP]  Make Packet....");

			// ===============================
			//		Step 1. make packet
    		// ===============================

			// (2) 고정 정보를 채워 넣는다
			// --- Header
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
			memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

			// --- Tag
			SEND_PACKET.TAG.Value = TagID::JOB_ORDER_TP;	// 작업지시

			// --- Status
			// 상태지시 - 기동 ON
			// SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;
			// 정지지시, 정지요인, 유지보수지시, E/M지시, 데이터 전송지시  - 설정 생략

    		// =============================== Body ===============================
			if(m_pOHTMainStatus->CurCmdID >= 9)
			{
    			SEND_PACKET.BODY.CmdID = 1;	// 지시 ID
            }
            else
            {
    			SEND_PACKET.BODY.CmdID = m_pOHTMainStatus->CurCmdID + 1;	// 지시 ID
            }

			iTemp = ORDERDIV::JobOrder; 					// 지시구분 - 작업지시
			memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// 지시구분

			// 이적재 Buffer 위치
			SEND_PACKET.BODY.BufferType.Buffer_1 = 1;			// 제1 수하대

			if(MapMakeType == 0x01)
				iTemp = CYCLE::TP_RunningLearning;
			else if(MapMakeType == 0x02)
				iTemp = CYCLE::TP_TestRunningLearning;

			// iTemp = CYCLE::MovingToStation; 				// 지시사이클 - 이동 사이클
			memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);

			// 우선도
			RIV tmpRv;
			tmpRv.Riv_Value = 64;							// 7비트니까
			tmpRv.Hotlot = 0;								// Hotlot - off
			memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

			if(iDirection == DefaultDirection::DIR_LEFT)
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
			else
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;


			if(iStartNode == nMapMakeTargetNode)
			{
				SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 0;		// Driving flag
			}
			else
			{
				SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag
			}


			// 시작 NODE
			Int2Uchar3(iStartNode, cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

			// 목적 NODE
			Int2Uchar3(nMapMakeTargetNode, cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

			// STOP NODE
			SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;


			/* 아래는 Map Info */
			SEND_PACKET.BODY.ChangeNode.Map_Num			= 0x01;	// 1 floor
			SEND_PACKET.BODY.AutoRecovery				= 0x00;	// 지정 없음 (CLW가 CLW의 파라미터 설정에 따라 독자적으로 판단한다.)
			SEND_PACKET.BODY.StationData_Reserved[0]	= 0;	// not use


    		// =============================== Path ===============================
			SEND_PACKET.BODY.PathData_ExtraNum = iCount;

			for(int i = 0; i < iCount; i++)
			{
				ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] Count : %d  Path : %d", i, iPathList[i]);

				Int2Uchar3(iPathList[i], cHigh, cMid, cLow);

				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
			}
		}
	}

    if(nError == NO_ERR)
    {
        // ===============================
        //		Step 2. internal process
        // ===============================
        bool bCheck = false;
        bool bCheckInsertTaskList = false;
        int  QuickCmd = 0;

        PKT_ACK_OHT2OCS RtnACKPkt;
        PKT_OHT2TP_RESPONSE RtnACKPktPC;

        PKT_TASKCMDDATA pTempCmdPktData;
        memset(&pTempCmdPktData, 0x00, sizeof(PKT_TASKCMDDATA));
        pTempCmdPktData.CategoryDiv = CATEGORY_TERTIARY;

        bCheck = CheckCmdJobOrder((PKT_OCS2OHT*)&SEND_PACKET, &RtnACKPkt,&pTempCmdPktData.DetailTagSort, QuickCmd, pTempCmdPktData.CategoryDiv);

        ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] Step 2. internal process - bCheck : %d", bCheck);

        // ===============================
        //		Step 3. insert command
        // ===============================
        // Step.3 Task List에 접수 가능 할때 접수하는 부분 처리.
        if(bCheck)
        {
			ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] Step 3. Insert command..");

            memmove(&(pTempCmdPktData.PKTDATA), &SEND_PACKET, sizeof(PKT_OCS2OHT));
            // TASK CMD LIst에 추가될 경우에 여기 추가
            // 순서에 맞게 저장할 것.
            m_pCmdManager->MakeTaskList_Standard(&pTempCmdPktData);
		}
	}

    return nError;
}
/*
	Map Search
	return - true : found, false : not found
*/
bool TaskControl::ManualMovePathSearch(int iStartNode, int iStopNode, int& iDirection, int& iCount, unsigned int* iNodeNumList)
{
	int nError = NO_ERR;
	int nCount = 0;
	int nTempTargetNode = 0;

	EXCEPT_PATH_LIST_V80 exceptList;
    memset(&exceptList, 0x00, sizeof(EXCEPT_PATH_LIST_V80));

    if(iStartNode == iStopNode)
    {
		nCount = 0;
	}
    else
    {
    	nError = m_pPathSearch->SearchExceptPathForV80(OHTMainForm->GetMap(), iStartNode, iStopNode, &exceptList,nTempTargetNode);
	    nCount = exceptList.nCnt;
    }

	// 리턴값 생성
	iDirection = DefaultDirection::DIR_LEFT;	// 이 함수는 고정임
	iCount = nCount;

	if(nError == NO_ERR)
	{
		for(int i = 0; i < nCount; i++)
		{
			iNodeNumList[i] = exceptList.ExceptNode[i];
		}

		return true;	// search success
	}
	else
	{
		return false;
	}
}

int TaskControl::ManualMovePathSearchMapMake(int iStartNode, int iStopNode, int& iDirection, int& iCount, unsigned int* iNodeNumList,int &nTargetNode, bool bMapMakeTestMove)
{
	int nError = NO_ERR;
    int nCount = 0;
	int nTempTargetNode = 0;

	EXCEPT_PATH_LIST_V80 exceptList;
    memset(&exceptList, 0x00, sizeof(EXCEPT_PATH_LIST_V80));

    if(iStartNode == iStopNode)
    {
		nCount = 0;
	}
    else
	{
		if(bMapMakeTestMove==false)
		{
			nError = m_pPathSearch->SearchExceptPathForV80(OHTMainForm->GetMap(), iStartNode, iStopNode, &exceptList, nTempTargetNode, 0x01);
			nTargetNode = nTempTargetNode;
		}
		else
		{
        	nError = m_pPathSearch->SearchExceptPathForV80(OHTMainForm->GetMap(), iStartNode, iStopNode, &exceptList, nTempTargetNode, 0x02);
		}

		nCount = exceptList.nCnt;
    }

	// 리턴값 생성
	iDirection = DefaultDirection::DIR_LEFT;	// 이 함수는 고정임
	iCount = nCount;

	if(nError == NO_ERR)
	{
		for(int i = 0; i < nCount; i++)
		{
			iNodeNumList[i] = exceptList.ExceptNode[i];
		}


	}

	return nError;	// search success
}

int TaskControl::LoadUpdateIniFile(AnsiString strIniFilePath, FIRMWARE_INFO* Info)
{
	int Error = NO_ERR;
	AnsiString strTempFileName;
	AnsiString strTempValue;

	// 펌웨어 업데이트 관련한 변수의 초기화
	Info->iMainUpdate	= FALSE;	// OHT Main을 업데이트 할지 여부를 설정한다
	Info->iAmcUpdate 	= FALSE;	// AMC를 업데이트 할지 여부를 설정한다
	Info->iAsUpdate		= FALSE;	// 어시스턴스를 업데이트 할지 여부를 설정한다
	Info->iFmUpdate		= FALSE;	// File Manager를 업데이트 할지 여부를 설정한다
	Info->iParamUpdate	= FALSE;	// 파라미터를 업데이트 할지 여부를 설정한다

	Info->iWirelessConfig = FALSE;	// 무선모듈 업데이트 여부를 설정한다
	Info->iWirelessMacfil = FALSE;
	Info->iWirelessFirmware = FALSE;

	Info->iParamFileCount = 0;		// 업데이트할 파일의 갯수

	ADD_LOG_TASKCONTROL("[Firmware Update] LoadUpdateIniFile()");

	// INI Load
	TIniFile *FwIniFile = new TIniFile(strIniFilePath);

	// 대소문자 구별 확인
	strTempValue = FwIniFile->ReadString("TARGET","MainUpdate"	,"N");
	if(strTempValue.UpperCase() != "N")
	{
		Info->iMainUpdate = TRUE;
		ADD_LOG_TASKCONTROL("[Firmware Update] Main Checked");
	}

	strTempValue = FwIniFile->ReadString("TARGET","AmcUpdate"	,"N");
	if(strTempValue.UpperCase() != "N")
	{
		Info->iAmcUpdate = TRUE;
		ADD_LOG_TASKCONTROL("[Firmware Update] Amc Checked");
	}
	
	strTempValue = FwIniFile->ReadString("TARGET","AsUpdate"	,"N");
	if(strTempValue.UpperCase() != "N")
	{
		Info->iAsUpdate = TRUE;
		ADD_LOG_TASKCONTROL("[Firmware Update] Assistance Checked");
	}

	strTempValue = FwIniFile->ReadString("TARGET","FmUpdate"	,"N");
	if(strTempValue.UpperCase() != "N")
	{
		Info->iFmUpdate = TRUE;
		ADD_LOG_TASKCONTROL("[Firmware Update] File Manager Checked");
	}

	strTempValue = FwIniFile->ReadString("TARGET","ParamUpdate" ,"N");
	if(strTempValue.UpperCase() != "N")
	{
		Info->iParamUpdate = TRUE;
		ADD_LOG_TASKCONTROL("[Firmware Update] Parameter Checked");
	}
	
	strTempValue = FwIniFile->ReadString("TARGET","WirelessConfig" ,"N");
	if(strTempValue.UpperCase() != "N")
	{
		Info->iWirelessConfig = TRUE;
		ADD_LOG_TASKCONTROL("[Firmware Update] Wireless Config Checked");
	}
	
	strTempValue = FwIniFile->ReadString("TARGET","WirelessMacfil" ,"N");
	if(strTempValue.UpperCase() != "N")
	{
		Info->iWirelessMacfil = TRUE;
		ADD_LOG_TASKCONTROL("[Firmware Update] Mac fil Checked");
	}

	strTempValue = FwIniFile->ReadString("TARGET","WirelessFirmware" ,"N");
	if(strTempValue.UpperCase() != "N")
	{
		Info->iWirelessFirmware = TRUE;
		ADD_LOG_TASKCONTROL("[Firmware Update] Wireless Firmware Checked");
	}


	// 파라미터 로딩
	int iTmpCount = 0;
	for (int i = 1; i < 100; i++) 	// 파일은 01부터 99까지임
	{
		strTempFileName.sprintf("ParamFile%02d",i);
		strTempValue = FwIniFile->ReadString("PARAMETER", strTempFileName,"N");

		if(strTempValue.UpperCase() != "N")
		{
			Info->strParamFileList[iTmpCount] = strTempValue;

			ADD_LOG_TASKCONTROL("[Firmware Update] ParamFile = %s Checked", strTempValue);
			iTmpCount++;
		}
	}

	Info->iParamFileCount = iTmpCount;
	ADD_LOG_TASKCONTROL("[Firmware Update] ParamFile Count = %d", iTmpCount);

}

// shkim 2016-11-30
// AMCDownloadFile Update기능
int TaskControl::UpdateAMCDownloadFile(AnsiString strAmcFilePath)
{
	m_pHWSet->m_AMCResponse = 0x01;
	int nError = 0, nRtnValue = 0;
	AnsiString sDownFileName, sFullName, sLogMessage, sOHTPath, scoffbootPath;

	sOHTPath = ExtractFileDir(Application->ExeName);
	// sDownFileName = sOHTPath + "\\DataFiles\\amc.out";		//Temp Path
	// scoffbootPath = sOHTPath + "\\DataFiles\\coffboot.bin"; //Temp Path
	sDownFileName = strAmcFilePath + "amc.out";
	scoffbootPath = strAmcFilePath + "coffboot.bin";

	ADD_LOG_TASKCONTROL("[UpdateAMCDownloadFile]AMC Upload Start");

	if(sDownFileName.Length() <= 0)
	{
		ADD_LOG_TASKCONTROL("[UpdateAMCDownloadFile][%s] Error[Length<=0] Upload Fail",ExtractFileName(sDownFileName));
		nError = 1;
	}
	else if(!FileExists(sDownFileName))
	{
		ADD_LOG_TASKCONTROL("[UpdateAMCDownloadFile][%s] Error[File Not Exist] Upload Fail",ExtractFileName(sDownFileName));
		nError = 2;
	}

	for(int i=0; i <4; i++)
	{
		m_pHWCommon->SetAMPEnable(i,false);
	}

	Sleep(50);
	nError = m_pHWCommon->AMCFileSystemForamt();
	Sleep(50);

	nError = m_pHWCommon->SetAMCDriverOpenByCommon(false);
	nError = m_pHWCommon->ClearWatchDogStatus();
	nError = m_pHWCommon->DisableWatchDog();

	if(nError == NO_ERR) nError = m_pHWCommon->AMCFileSystemInstall(scoffbootPath.c_str(), sDownFileName.c_str(), &nRtnValue, 0, WM_DSP_MSGID);

	if(nRtnValue != NO_ERR)
	{
		ADD_LOG_TASKCONTROL("[UpdateAMCDownloadFile]Flash Error[%d]", nRtnValue);
		if(nRtnValue <= -4 && nError == 0) nError = -1;
	}
	if(nError || nRtnValue)
		nError = m_pHWCommon->SetAMCDriverOpenByCommon(true);

	Sleep(50);
	if(nError == NO_ERR) nError = m_pHWCommon->DSP_Reboot();
	Sleep(5000);

	if(nError == NO_ERR)
	{
		ADD_LOG_TASKCONTROL("[UpdateAMCDownloadFile][%s]:Upload Sucesses",ExtractFileName(sDownFileName));
		m_pHWSet->m_AMCResponse = 0x02; // TP상태전송 0x00대기, 0x01진행, 0x02완료, 0x03실패
		// m_StatusSet.bIsOnProcess = false;
		// m_StatusSet.byStatus = STS_COMPLETE;
		// m_StatusSet.uError == NO_ERR;
	}
	else
	{
		ADD_LOG_TASKCONTROL("[UpdateAMCDownloadFile][%s]:Error[%d]:Upload Fail",ExtractFileName(sDownFileName), nError);
		m_pHWSet->m_AMCResponse = 0x03;
		// m_StatusSet.bIsOnProcess = false;
		// m_StatusSet.byStatus = STS_ERROR;
		// m_StatusSet.uError == nError;
	}

	return nError;
}


/*
	이 함수는 업데이트 지시된 파일이 압축파일에 존재하는지 유무를 확인한다
	리턴값은 0이면 정상. 1또는 그 이상이면 해당 파일이 없는 개수이다.
*/
int TaskControl::FirmwareUpdate_FileExistCheck(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO)
{
	UnicodeString strSourceFile, strDestFile;

	int iErrorCount = 0;

	if(FIRMWARE_UPDATE_INFO.iAsUpdate == TRUE)	// 어시스턴스
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + ASSISTANCE_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 0)	// not found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] file not found  Source = %s", strSourceFile.c_str());
		}
	}
	else
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + ASSISTANCE_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 1)	// not need, but found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] (not need file found)  Source = %s", strSourceFile.c_str());
		}
	}

	if(FIRMWARE_UPDATE_INFO.iFmUpdate == TRUE)	// File Manager 백업
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + FILE_MANAGER_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 0)	// not found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] file not found  Source = %s", strSourceFile.c_str());
		}
	}
	else
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + FILE_MANAGER_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 1)	// not need, but found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] (not need file found)  Source = %s", strSourceFile.c_str());
		}
	}

	if(FIRMWARE_UPDATE_INFO.iMainUpdate == TRUE)	// OHT.exe 백업
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + OHT_MAIN_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 0)	// not found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] file not found  Source = %s", strSourceFile.c_str());
		}
	}
	else
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + OHT_MAIN_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 1)	// not need, but found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] (not need file found)  Source = %s", strSourceFile.c_str());
		}
	}

	if(FIRMWARE_UPDATE_INFO.iAmcUpdate == TRUE)	// amc 백업
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + AMC_BOOT_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 0)	// not found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] file not found  Source = %s", strSourceFile.c_str());
		}


		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + AMC_MAIN_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 0)	// not found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] file not found  Source = %s", strSourceFile.c_str());
		}
	}
	else
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + AMC_BOOT_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 1)	// not need, but found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] (not need file found)  Source = %s", strSourceFile.c_str());
		}


		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + AMC_MAIN_FILE_NAME;
		
		if(FileExists(strSourceFile.c_str()) == 1)	// not need, but found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] (not need file found)  Source = %s", strSourceFile.c_str());
		}
	}

	// wireless module
	if(FIRMWARE_UPDATE_INFO.iWirelessConfig == TRUE)
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + WIRELESS_CONFIG_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 0)	// not found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] file not found  Source = %s", strSourceFile.c_str());
		}
	}
	else
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + WIRELESS_CONFIG_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 1)	// not need, but found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] (not need file found)  Source = %s", strSourceFile.c_str());
		}
	}

	if(FIRMWARE_UPDATE_INFO.iWirelessMacfil == TRUE)
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + WIRELESS_MACFIL_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 0)	// not found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] file not found  Source = %s", strSourceFile.c_str());
		}
	}
	else
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + WIRELESS_MACFIL_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 1)	// not need, but found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] (not need file found)  Source = %s", strSourceFile.c_str());
		}
	}

	if(FIRMWARE_UPDATE_INFO.iWirelessFirmware == TRUE)
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + WIRELESS_FIRMWARE_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 0)	// not found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] file not found  Source = %s", strSourceFile.c_str());
		}
	}
	else
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + WIRELESS_FIRMWARE_FILE_NAME;

		if(FileExists(strSourceFile.c_str()) == 1)	// not need, but found
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] (not need file found)  Source = %s", strSourceFile.c_str());
		}
	}


	// Parameter file
	if(FIRMWARE_UPDATE_INFO.iParamUpdate ==  TRUE)
	{
		for(int i = 0; i < FIRMWARE_UPDATE_INFO.iParamFileCount; i++)
		{
			strSourceFile = FIRMWARE_UPDATE_PATH;
			strSourceFile = strSourceFile + FIRMWARE_UPDATE_INFO.strParamFileList[i];

			if(FileExists(strSourceFile.c_str()) == 0)
			{
				iErrorCount++;
				ADD_LOG_TASKCONTROL("[Firmware Update] file not found  Source = %s", strSourceFile.c_str());
			}
		}
	}

	return iErrorCount;
}


/*
	이 함수는 업데이트 지시된 파일의 업데이트 전 백업을 진행한다.
	리턴값은 0이면 정상. 1또는 그 이상이면 해당 파일이 없는 개수이다.
*/
int TaskControl::FirmwareUpdate_FileBackup(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO)
{
	UnicodeString strSourceFile, strDestFile;

	int iErrorCount = 0;

	ADD_LOG_TASKCONTROL("[Firmware Update] FirmwareUpdate_FileBackup.....");

	if(FIRMWARE_UPDATE_INFO.iAsUpdate == TRUE)	// 어시스턴스 백업
	{
		strSourceFile = OHT_PROGRAM_PATH;
		strSourceFile = strSourceFile + ASSISTANCE_FILE_NAME;
		strDestFile =  strSourceFile + BACKUP_FILE_EXTENSION;

		if(FileExists(strSourceFile.c_str()))   // 덮어쓰기. 되는지는 꼭 확인필요
		{
			CopyFile(strSourceFile.c_str(), strDestFile.c_str(),  false);
		}
	}

	if(FIRMWARE_UPDATE_INFO.iFmUpdate == TRUE)	// File Manager 백업
	{
		strSourceFile = OHT_PROGRAM_PATH;
		strSourceFile = strSourceFile + FILE_MANAGER_FILE_NAME;
		strDestFile =  strSourceFile + BACKUP_FILE_EXTENSION;

		if(FileExists(strSourceFile.c_str()))	// 덮어쓰기. 되는지는 꼭 확인필요
		{
			CopyFile(strSourceFile.c_str(), strDestFile.c_str(),  false);
		}
	}

	if(FIRMWARE_UPDATE_INFO.iMainUpdate == TRUE)	// OHT.exe 백업
	{
		strSourceFile = OHT_PROGRAM_PATH;
		strSourceFile = strSourceFile + OHT_MAIN_FILE_NAME;
		strDestFile =  strSourceFile + BACKUP_FILE_EXTENSION;

		if(FileExists(strSourceFile.c_str()))	// 덮어쓰기. 되는지는 꼭 확인필요
		{
			CopyFile(strSourceFile.c_str(), strDestFile.c_str(),  false);
		}
	}


	if(FIRMWARE_UPDATE_INFO.iAmcUpdate == TRUE)	// amc 백업
	{
		// ---- amc boot
		strSourceFile = AMC_BINARY_PATH;
		strSourceFile = strSourceFile + AMC_BOOT_FILE_NAME;
		strDestFile =  strSourceFile + BACKUP_FILE_EXTENSION;

		if(FileExists(strSourceFile.c_str()))	// 덮어쓰기. 되는지는 꼭 확인필요
		{
			CopyFile(strSourceFile.c_str(), strDestFile.c_str(),  false);
		}

		// ---- amc main
		strSourceFile = AMC_BINARY_PATH;
		strSourceFile = strSourceFile + AMC_MAIN_FILE_NAME;
		strDestFile =  strSourceFile + BACKUP_FILE_EXTENSION;

		if(FileExists(strSourceFile.c_str()))	// 덮어쓰기. 되는지는 꼭 확인필요
		{
			CopyFile(strSourceFile.c_str(), strDestFile.c_str(),  false);
		}
	}

	// wireless
	if(FIRMWARE_UPDATE_INFO.iWirelessConfig == TRUE)
	{
		strSourceFile = OHT_PARAM_FILE_PATH;
		strSourceFile = strSourceFile + WIRELESS_CONFIG_FILE_NAME;
		strDestFile =  strSourceFile + BACKUP_FILE_EXTENSION;

		if(FileExists(strSourceFile.c_str()))	// 덮어쓰기. 되는지는 꼭 확인필요
		{
			CopyFile(strSourceFile.c_str(), strDestFile.c_str(),  false);
		}
	}

	if(FIRMWARE_UPDATE_INFO.iWirelessMacfil == TRUE)
	{
		strSourceFile = OHT_PARAM_FILE_PATH;
		strSourceFile = strSourceFile + WIRELESS_MACFIL_FILE_NAME;
		strDestFile =  strSourceFile + BACKUP_FILE_EXTENSION;

		if(FileExists(strSourceFile.c_str()))	// 덮어쓰기. 되는지는 꼭 확인필요
		{
			CopyFile(strSourceFile.c_str(), strDestFile.c_str(),  false);
		}
	}

	if(FIRMWARE_UPDATE_INFO.iWirelessFirmware == TRUE)
	{
		strSourceFile = OHT_PARAM_FILE_PATH;
		strSourceFile = strSourceFile + WIRELESS_FIRMWARE_FILE_NAME;
		strDestFile =  strSourceFile + BACKUP_FILE_EXTENSION;

		if(FileExists(strSourceFile.c_str()))	// 덮어쓰기. 되는지는 꼭 확인필요
		{
			CopyFile(strSourceFile.c_str(), strDestFile.c_str(),  false);
		}
	}



	// Parameter file
	for(int i = 0; i < FIRMWARE_UPDATE_INFO.iParamFileCount; i++)
	{
		// 2) Param file copy
		strSourceFile = OHT_PARAM_FILE_PATH;
		strSourceFile = strSourceFile + FIRMWARE_UPDATE_INFO.strParamFileList[i];
		strDestFile = strSourceFile + BACKUP_FILE_EXTENSION;

		if(FileExists(strSourceFile.c_str()))	// 덮어쓰기. 되는지는 꼭 확인필요
		{
			CopyFile(strSourceFile.c_str(), strDestFile.c_str(),  false);
		}

		ADD_LOG_TASKCONTROL("[Firmware Update] Param file backup. Source = %s Dest = %s",
									strSourceFile.c_str(), strDestFile.c_str());
	}

	return iErrorCount;
}


int TaskControl::FirmwareUpdate_AS_FM(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO)
{
	AnsiString strSourceFile, strDestFile;

	int iErrorCount = 0;
	int iErrorCode = 0;

	/*
		어시스턴스는 파일매니저가 죽으면 자동으로 실행하는 기능이 있어
		이 부분을 고려하여 끄고 켜야 한다

		1) 끄는 순서 : 어시스턴스 -> 파일 매니저
		2) 켜는 순서 : 파일매니저 -> 어시스턴스
	*/

	// --------------------- Step 1. Program Close ----------------------------
	// 1) 어시스턴스를 종료시킨다
	if(FIRMWARE_UPDATE_INFO.iAsUpdate == TRUE)
	{
		DWORD pid = ProcessTerminator::getInstance().FindPID(ASSISTANCE_FILE_NAME);
		if( pid != NULL )
	    {
	    	ADD_LOG_TASKCONTROL("[Firmware Update] 어시스턴스 running found. try kill process ");

			int result = ProcessTerminator::getInstance().TerminateApp(pid, 1000);
			Sleep(100);

	     	switch(result)
			{
				case TA_FAILED:
					result = ProcessTerminator::getInstance().TerminateApp(pid, 1000);  // retry
					break;
				case TA_SUCCESS_CLEAN:
				case TA_SUCCESS_KILL:
				{
					//FileControl::getInstance().ExecFMProgram();
					// Sleep(100);	//FileManager.exe 실행 시간 확보
				}
					break;
			}
		}
	}

	// 2) File Manager를 종료시킨다
	if(FIRMWARE_UPDATE_INFO.iFmUpdate == TRUE)
	{
		ADD_LOG_TASKCONTROL("[Firmware Update] File Manager running found. try kill process ");

		DWORD pid = ProcessTerminator::getInstance().FindPID(FILE_MANAGER_FILE_NAME);
		if( pid != NULL )
		{
			int result = ProcessTerminator::getInstance().TerminateApp(pid, 1000);
			Sleep(100);

	     	switch(result)
			{
				case TA_FAILED:
					result = ProcessTerminator::getInstance().TerminateApp(pid, 1000);   // retry
					break;
				case TA_SUCCESS_CLEAN:
				case TA_SUCCESS_KILL:
				{
					//FileControl::getInstance().ExecFMProgram();
					// Sleep(100);//FileManager.exe 실행 시간 확보
				}
					break;
			}
		}
	}

	// ------------------------------------------------------------------------

	// --------------------- Step 2.파일을 복사(덮어쓰기) 한다 ----------------
	if(FIRMWARE_UPDATE_INFO.iAsUpdate == TRUE)
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + ASSISTANCE_FILE_NAME;

		strDestFile = OHT_PROGRAM_PATH;
		strDestFile = strDestFile + ASSISTANCE_FILE_NAME;

		iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);

		if(iErrorCode != 1) // result 1 is copy success
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] Assistance file copy error. Source Name = %s Dest Name = %s Err Count %d",
									strSourceFile.c_str(), strDestFile.c_str(), iErrorCount);
		}
	}

	if(FIRMWARE_UPDATE_INFO.iFmUpdate == TRUE)
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + FILE_MANAGER_FILE_NAME;

		strDestFile = OHT_PROGRAM_PATH;
		strDestFile = strDestFile + FILE_MANAGER_FILE_NAME;

		iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);

		if(iErrorCode != 1) // result 1 is copy success
		{
			iErrorCount++;
			ADD_LOG_TASKCONTROL("[Firmware Update] FileManager file copy error. Source Name = %s Dest Name = %s Err Count %d",
									strSourceFile.c_str(), strDestFile.c_str(), iErrorCount);
		}
	}

	// ------------------------------------------------------------------------

	// --------------------- Step 3. 재 실행시킨다 ----------------------------
	if(FIRMWARE_UPDATE_INFO.iFmUpdate == TRUE)
	{
		strSourceFile = OHT_PROGRAM_PATH;
		strSourceFile = strSourceFile + FILE_MANAGER_FILE_NAME;
		ExecuteProgram(strSourceFile);
	}

	Sleep(100);

	if(FIRMWARE_UPDATE_INFO.iAsUpdate == TRUE)
	{
		strSourceFile = OHT_PROGRAM_PATH;
		strSourceFile = strSourceFile + ASSISTANCE_FILE_NAME;
		ExecuteProgram(strSourceFile);
	}

	// 프로그램들을 재 실행한 후 메모리에 올라오기 까지 필요한 시간을 준다
	// 이 시간을 주지 않으면 AS와 메인 동시 업데이트시 AS가 구동전이 되므로 메인 업데이트에 문제가 생긴다
	Sleep(500);
	// ------------------------------------------------------------------------

	return iErrorCount;
}



int TaskControl::FirmwareUpdate_PARAM(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO)
{
	AnsiString strSourceFile, strDestFile, strApplyFileName, strOHTPath;

	int iErrorCount = 0;
	int iErrorCode = 0;

	strOHTPath = ExtractFileDir(Application->ExeName);
	// 1) 파라미터 파일을 복사(반복적으로) 한다
	for(int i = 0; i < FIRMWARE_UPDATE_INFO.iParamFileCount; i++)
	{
		// 2) Param file copy
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + FIRMWARE_UPDATE_INFO.strParamFileList[i];

		strDestFile =  String(strOHTPath + STR_DATAFILE_UPDATE_PATH);
		strDestFile = strDestFile + FIRMWARE_UPDATE_INFO.strParamFileList[i];

		strApplyFileName = FIRMWARE_UPDATE_INFO.strParamFileList[i];
		strApplyFileName = strApplyFileName.Trim();
		strApplyFileName = strApplyFileName.UpperCase();

		iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);


		ADD_LOG_TASKCONTROL("[Firmware Update] %s loading... ",strApplyFileName);

		
	}


	return iErrorCount;
}

int TaskControl::FirmwareUpdate_WIRELESS(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO)
{
	// Step 1. File Copy ----------
	AnsiString strSourceFile, strDestFile;
	int iErrorCode;

	// (1) config file
	if(FIRMWARE_UPDATE_INFO.iWirelessConfig == TRUE)
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + "config";
		strDestFile = WIRELESS_PARAM_FILE_PATH;
		strDestFile = strDestFile + "config";
		iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);
	}

	// (2) MAC file
	if(FIRMWARE_UPDATE_INFO.iWirelessMacfil == TRUE)
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + "macfil";
		strDestFile = WIRELESS_PARAM_FILE_PATH;
		strDestFile = strDestFile + "macfil";
		iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);
	}

	// (3) firmware file
	if(FIRMWARE_UPDATE_INFO.iWirelessFirmware == TRUE)
	{
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + "firmware.bin";
		strDestFile = WIRELESS_PARAM_FILE_PATH;
		strDestFile = strDestFile + "firmware.bin";
		iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);
	}


	// Step 2. Protocol Send ------------
	AnsiString strIpAddr, strUserName, strPassword;

	int iPacketSize = sizeof(PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS);
	PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS SEND_PKT;
	memset(&SEND_PKT, 0x00, iPacketSize);

	SEND_PKT.HEADER.MachineType = MACHINETYPE_VHL;
	SEND_PKT.TAG.Value = TagID::CMD_FIRMWARE_UPDATE_ORDER_WIRELESS_OHT2AS;

	LoadWirelessModuleInfo(strIpAddr, strUserName, strPassword);	// load from OhtParam.ini

	SEND_PKT.BODY.iCmd = WIRELESS_PUT;

	int iHigh, iM1, iM2, iLow;
	IpStrToByteAddr(strIpAddr, iHigh, iM1, iM2, iLow);

	// ip addr
	SEND_PKT.BODY.IpAddress[FOUR_BYTE_HIGH]	= iHigh;
	SEND_PKT.BODY.IpAddress[FOUR_BYTE_M1]	= iM1;
	SEND_PKT.BODY.IpAddress[FOUR_BYTE_M2]	= iM2;
	SEND_PKT.BODY.IpAddress[FOUR_BYTE_LOW]	= iLow;

	// update info
	SEND_PKT.BODY.iWirelessConfig	= FIRMWARE_UPDATE_INFO.iWirelessConfig;
	SEND_PKT.BODY.iWirelessMacfil	= FIRMWARE_UPDATE_INFO.iWirelessMacfil;
	SEND_PKT.BODY.iWirelessFirmware	= FIRMWARE_UPDATE_INFO.iWirelessFirmware;

	memcpy(&SEND_PKT.BODY.UserName, 	strUserName.c_str(), 	strUserName.Length());
	memcpy(&SEND_PKT.BODY.PassWord, 	strPassword.c_str(), 	strPassword.Length());

	m_pUDPDiag->UnicastSend_OHT_2_AS((char*)&SEND_PKT, iPacketSize);

	return iErrorCode;
}

int TaskControl::FirmwareUpdate_AMC(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO)
{
	AnsiString strSourceFile, strDestFile;

	int iErrorCount = 0;
	int iErrorCode = 0;

	// 2) amc file copy
	strSourceFile = FIRMWARE_UPDATE_PATH;
	strSourceFile = strSourceFile + "amc.out";
	strDestFile = AMC_BINARY_PATH;
	strDestFile = strDestFile + "amc.out";
	iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);

	if(iErrorCode != 1) // result 1 is copy success
	{
		iErrorCount++;
		ADD_LOG_TASKCONTROL("[Firmware Update] amc.out file copy error");
	}

	strSourceFile = FIRMWARE_UPDATE_PATH;
	strSourceFile = strSourceFile + "coffboot.bin";
	strDestFile = AMC_BINARY_PATH;
	strDestFile = strDestFile + "coffboot.bin";
	iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);

	if(iErrorCode != 1) // result 1 is copy success
	{
		iErrorCount++;
		ADD_LOG_TASKCONTROL("[Firmware Update] coffboot.bin file copy error");
	}

	// 3) update execute
	if(iErrorCount == 0)
	{
		ADD_LOG_TASKCONTROL("[Firmware Update] AMC Update execute...");
		if (UpdateAMCDownloadFile(AMC_BINARY_PATH) != NO_ERR) 	// error exist
			iErrorCount++;
	}
	else
	{
		ADD_LOG_TASKCONTROL("[Firmware Update] AMC file copy error. Update cancled!");
	}

	return iErrorCount;
}

// amc나 oht.exe가 변경되면 재시작해야 한다.
// 만일 amc만 변경되었으면 재시작만으로 충분하며
// oht.exe변경시 복사 등의 절차가 추가로 들어간다
// (BODY.iOhtMainCopyNeed)
int TaskControl::FirmwareUpdate_RESTART(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO)
{
	int iPacketSize = sizeof(PKT_RESTART_REQUEST_OHT2AS);
	PKT_RESTART_REQUEST_OHT2AS SEND_PKT;
	DWORD OHTpid = GetCurrentProcessId();
 	ADD_LOG_TASKCONTROL("OHTpid : %d",OHTpid);
	
	memset(&SEND_PKT, 0, iPacketSize);

	SEND_PKT.HEADER.MachineType = MACHINETYPE_VHL;
	SEND_PKT.TAG.Value = TagID::CMD_FIRMWARE_UPDATE_RESTART_REQ_OHT2AS;

	SEND_PKT.BODY.ShutDownDelay = 1; //to KYH : Make Parameter~
	SEND_PKT.BODY.OHTPID = OHTpid;

	if(FIRMWARE_UPDATE_INFO.iMainUpdate == TRUE)
		SEND_PKT.BODY.iOhtMainCopyNeed = 1;		// OHT.exe copy request

	if(FIRMWARE_UPDATE_INFO.iParamUpdate == TRUE)
		SEND_PKT.BODY.iOhtParamCopyNeed = 1;	// Param copy request

	m_pUDPDiag->UnicastSend_OHT_2_AS((char*)&SEND_PKT, iPacketSize);
}


int TaskControl::Change2AutoMode(void)
{
	PARSING_TRANS_INFO tmpCmd;
	memset(&tmpCmd, 0, sizeof(PARSING_TRANS_INFO)); 	// 패킷 메모리 클리어
	
	ADD_LOG_TASKCONTROL("[AUTO MODE CHANGE] START");
	
	tmpCmd.CmdID = GetOrderID();	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
	tmpCmd.CmdCycle = CYCLE::TP_AutoMode;
	
	if(CheckStatusForManual())
	{
		ADD_LOG_TASKCONTROL("[AUTO MODE CHANGE] Cycle Insert & Go");
		MakeTransCycleList(&tmpCmd);
		return 0;
	}
	else
	{
		ADD_LOG_TASKCONTROL("[AUTO MODE CHANGE] Now Auto Mode. Cycle Insert Cancled");
		return -1;
	}
}

int TaskControl::ExecuteProgram(String strFileName)
{
	HINSTANCE hResult;
	int nResult;
	// String strFileName;
	String strRes;

	// strFileName = ".\\" + sFileName;

	hResult = ShellExecute(NULL, L"open", strFileName.c_str(), NULL, NULL, SW_SHOW);
	nResult = (int)hResult;
	switch(nResult)
	{
	case 0:
		strRes = "ERROR : The operating system is out of memory or resources.";
		break;
	case ERROR_FILE_NOT_FOUND:
		strRes = "ERROR : The specified file was not found.";
		break;
	case ERROR_PATH_NOT_FOUND:
		strRes = "ERROR : The specified path was not found.";
		break;
	case ERROR_BAD_FORMAT:
		strRes = "ERROR : The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).";
		break;
	case SE_ERR_ACCESSDENIED:
		strRes = "ERROR : The operating system denied access to the specified file.";
		break;
	case SE_ERR_ASSOCINCOMPLETE:
		strRes = "ERROR : The filename association is incomplete or invalid.";
		break;
	case SE_ERR_DDEBUSY:
		strRes = "ERROR : The DDE transaction could not be completed because other DDE transactions were being processed.";
		break;
	case SE_ERR_DDEFAIL:
		strRes = "ERROR : The DDE transaction failed.";
		break;
	case SE_ERR_DDETIMEOUT:
		strRes = "ERROR : The DDE transaction could not be completed because the request timed out.";
		break;
	case SE_ERR_DLLNOTFOUND:
		strRes = "ERROR : The specified dynamic-link library was not found.";
		break;
	case SE_ERR_NOASSOC:
		strRes = "ERROR : There is no application associated with the given filename extension.";
		break;
	case SE_ERR_OOM:
		strRes = "ERROR : There was not enough memory to complete the operation.";
		break;
	case SE_ERR_SHARE:
		strRes = "ERROR : A sharing violation occurred.";
		break;
	default:
		strRes = "OHT Execute ... complete..";
		break;
	}
}


#define TMP_SIZE 50
void TaskControl::getInfoDetailVersion(PKT_VERSION_RESPONSE& SEND_PACKET, FWU_RESPONSE iDirection)
{
	FILE *fi;
	char cTmp[TMP_SIZE];
	int iReturn;

	AnsiString strPathOnly, strFullPath;
	AnsiString strTmp, strReturn;

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;

	if(iDirection == FWU_TO_OCS)
		SEND_PACKET.TAG.Value = TagID::RESPONSE_VERSIONREQ_OHT_OCS;
	else
		SEND_PACKET.TAG.Value = TagID::RESPONSE_VERSIONREQ_OHT_TP;

	// 이 부분에서 실제 들어갈 내용을 써 주면 된다

	// (1) GetMainVer
	strTmp.sprintf("Main=%u.%u.%u.%u ",
		(BYTE)OHTMainForm->VerInfo.Major,
        (BYTE)OHTMainForm->VerInfo.Minor,
        (BYTE)OHTMainForm->VerInfo.Release,
        (BYTE)OHTMainForm->VerInfo.Build);
	strReturn = strReturn + strTmp;

	// (2) GetAMCVer
	int nAMCVer = m_pHWSet->GetAMCVersion();

    int nAMCVerMajor = nAMCVer / 10000;
    int nAMCVerMinor = (nAMCVer - (nAMCVerMajor * 10000)) / 100;
    int nAMCVerRelease = (nAMCVer - (nAMCVerMajor * 10000)) % 100;

	strTmp.sprintf("Amc=%d.%d.%02d ", nAMCVerMajor, nAMCVerMinor, nAMCVerRelease);
	strReturn = strReturn + strTmp;

	// (3) GetAsVer
	strFullPath = AS_VER_FILE_PATH;

	if(FileExists(strFullPath.c_str()) == true) 	// 파일이 존재하면
	{
		if( (fi = fopen( strFullPath.c_str(), "r" )) == NULL )
		{
			strTmp = "As=Unknown ";
		}
		else
		{
			memset(&cTmp, '\0', TMP_SIZE);
			fscanf_s(fi,"%s", &cTmp,40);
			strTmp.sprintf("As=%s ", &cTmp);
			fclose(fi);
		}
	}
	else
	{	// File Not Found
			strTmp = "As=Unknown ";
	}
	strReturn = strReturn + strTmp;


	// (4) GetFmVer
	strFullPath = FM_VER_FILE_PATH;

	if(FileExists(strFullPath.c_str()) == true) 	// 파일이 존재하면
	{
		if( (fi = fopen( strFullPath.c_str(), "r" )) == NULL )
		{
			strTmp = "Fm=Unknown ";
		}
		else
		{
			memset(&cTmp, '\0', TMP_SIZE);
			fscanf_s(fi,"%s", &cTmp,40);
			strTmp.sprintf("Fm=%s ", &cTmp);
			fclose(fi);
		}
	}
	else
	{	// File Not Found
			strTmp = "Fm=Unknown ";
	}
	strReturn = strReturn + strTmp;

	// (5) IniFileVer
	strTmp.sprintf("DiagParam=%s ", ReqIniVersion(PARMA_DIAG));
	strReturn = strReturn + strTmp;

	strTmp.sprintf("Driving=%s ", ReqIniVersion(PARMA_DRIVING));
	strReturn = strReturn + strTmp;

	strTmp.sprintf("EQPIO=%s ", ReqIniVersion(PARMA_EQPIO));
	strReturn = strReturn + strTmp;

	strTmp.sprintf("IOMap=%s ", ReqIniVersion(PARMA_IOMAP));
	strReturn = strReturn + strTmp;

	strTmp.sprintf("PathSearch=%s ", ReqIniVersion(PARMA_PATHSEARCH));
	strReturn = strReturn + strTmp;

	strTmp.sprintf("Trans=%s ", ReqIniVersion(PARMA_TRANS));
	strReturn = strReturn + strTmp;

	memcpy(&SEND_PACKET.BODY.Version, strReturn.c_str(), strReturn.Length());
}

int TaskControl::LoadParamIni(int nIniType)
{
	int nError =NO_ERR;
	bool bCheck =false;
	AnsiString TemStr;

	switch(nIniType)
	{
		case PARMA_DIAG:
			bCheck = m_DiagManager->LoadDiagParamRecorvery("DataFiles\\DiagParam.ini",&m_DiagManager->GetDiagParam());
			if(bCheck == false)
			{
				nError =  ERR_AMC_ILLEGAL_PARAMETER;
			}
		break;
		case PARMA_DRIVING:
			bCheck = m_pDrivingControl->loadParameterRecovery("DataFiles\\Driving.ini",m_pDrivingControl->m_pParamSet);
			if(bCheck == false)
			{
				nError =  ERR_AMC_ILLEGAL_PARAMETER;
			}

		break;
		case PARMA_EQPIO:
			bCheck =m_pHWSet->m_pLeftEQPIO->LoadEQPIOParamRecorvery("DataFiles\\EQPIO.ini",m_pHWSet->m_pLeftEQPIO->m_pParamSet);
			if(bCheck == false)
			{
				nError =  ERR_AMC_ILLEGAL_PARAMETER;
			}
		break;
		case PARMA_PATHSEARCH:
			bCheck = m_pPathSearch->loadParameterRecovery("DataFiles\\PathSearch.ini",m_pPathSearch->m_pParamSet);
            if(bCheck == false)
			{
             	nError =  ERR_AMC_ILLEGAL_PARAMETER;
			}
		break;
		case PARMA_TRANS:
			bCheck = m_pTransControl->loadParameterRecovery("DataFiles\\Trans.ini",m_pTransControl->m_pParamSet);
			if(bCheck == false)
			{
             	nError =  ERR_AMC_ILLEGAL_PARAMETER;
			}
		break;
		case PARMA_UBGCHECKNODES:
			bCheck = m_pPathSearch->loadUBGCheckParam("DataFiles\\UBGCheckNodes.dat", m_pPathSearch->m_pUBGParamSet);
             if(bCheck == false)
			{
             	nError =  ERR_AMC_ILLEGAL_PARAMETER;
			}
		break;
		case PARMA_IOMAP:
		case PARMA_OHTDETECTAREA:
		case PARMA_OBSDETECTAREA:
			nError =  ERR_AMC_ILLEGAL_PARAMETER;
	 	break;
	}


	return  nError;
}

AnsiString TaskControl::ReqIniVersion(int nIniType)
{
	AnsiString strReturn ="00";

	switch(nIniType)
	{
		case PARMA_DIAG:
		strReturn.sprintf("%s", m_DiagManager->GetDiagParam().Version);
		break;
		case PARMA_DRIVING:
		strReturn.sprintf("%s", m_pDrivingControl->m_pParamSet->Version);
		break;
		case PARMA_EQPIO:
		strReturn.sprintf("%s", m_pHWSet->m_pLeftEQPIO->m_pParamSet->Version);
		break;
		case PARMA_PATHSEARCH:
		strReturn.sprintf("%s", m_pPathSearch->m_pParamSet->Version);
		break;
		case PARMA_TRANS:
		strReturn.sprintf("%s", m_pTransControl->m_pParamSet->Version);
		break;
		case PARMA_UBGCHECKNODES:
		break;
		case PARMA_IOMAP:
		strReturn.sprintf("%s", m_strIOVersion);
		break;
		case PARMA_OHTDETECTAREA:
		break;
		case PARMA_OBSDETECTAREA:
		break;
	}


	strReturn.SetLength(2);
	return strReturn;
}


void TaskControl::chkFirmwareUpdateDirectory(void)
{
	AnsiString Dir = FIRMWARE_UPDATE_PATH;

	if (!DirectoryExists(Dir))
	{
	  if (ForceDirectories(Dir))
		  ADD_LOG_TASKCONTROL("[데이터전송지시] %s 디렉토리가 없어 새로 생성함", Dir);
	}
	// 전송지시 시작 전 Down folder clear
	system("del /q C:\\User\\OHT\\DataFiles\\Down\\*.*");
}

void TaskControl::LoadWirelessModuleInfo(AnsiString& IpAddress, AnsiString& UserName, AnsiString& Password)
{
	int Error = NO_ERR;
    int TempID =0;
	String strParamPath = "", strFileName = "";

	strParamPath = OHT_PARAM_FILE_PATH;
	strFileName = strParamPath + "OHTPARAM.ini";
//
	TIniFile *TimeSetIni = new TIniFile (strFileName);
	IpAddress 	= TimeSetIni->ReadString("WIRELESS",	"IpAddress","192.168.110.1");
	UserName		= TimeSetIni->ReadString("WIRELESS",	"UserID",	"admin");
	Password		= TimeSetIni->ReadString("WIRELESS",	"Password",	"pass");

	delete TimeSetIni;

	return;
}

void TaskControl::FirmwareUpdateMessageToTp(AnsiString strMsg)
{
	int iReturn;

	PKT_VERSION_RESPONSE SEND_PACKET;
	int iPacketSize = sizeof(PKT_VERSION_RESPONSE);
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	SEND_PACKET.TAG.Value = TagID::RESPONSE_VERSIONREQ_OHT_TP;
	memcpy(&SEND_PACKET.BODY.Version, strMsg.c_str(), strMsg.Length());

	iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
}


/*
	Mode Change to manual mode

	return -> success : true, fail : false
*/
bool TaskControl::FirmwareUpdate_ModeChange()
{
	if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode==MODE_AUTO)
	{
		switch(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status)
		{
            //업데이트 가능한 상태
            case STS_INIT:
            case STS_ARRIVED:
			//case STS_UNLOADED:
            //case STS_LOADED:
			//case STS_COMPLETE:
			//case STS_ERROR:
			//case STS_TURN_OFF:  //OHT state is STS_TURN_OFF in update process 
				ChangeMode(MODE_MANUAL);
				ADD_LOG_TASKCONTROL("[Param Apply] Mode Change (Auto -> Manual) Ok. Now Mode: 0x%02x, Status: 0x%02x (Manual = 0x01, Auto = 0x10)",
					 					m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode,m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status);
            	Change_FDC_Manual_Mode(0);

			break;

			default:	//error
				ADD_LOG_TASKCONTROL("[Param Apply] Mode Error! Now Mode: 0x%02x, Status: 0x%02x (Manual = 0x01, Auto = 0x10)",
					 					m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode,m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status);

				return -1; // exit
			break;
		}
	}
	else
	{
			ADD_LOG_TASKCONTROL("[Param Apply] Current Mode is Manual. Mode Change Not Need");
	}
}

/*
   (로딩 불필요한 파일목록)
    - OhtParam.ini 개체차 파일
    - DefaultParam.ini amc가 덮어쓰는 파일    
    - AmcParam.ini  이 파일은 변경하면 amc 보드 reboot, Main 재시작까지 해야 하므로 바로 apply 불가능
    - ErrorCode.ini 는 Main에서 직접 사용하는 부분은 없음

    => 로딩할 파일은 6개임
*/
int TaskControl::ParamApply(int iUpdateFile)
{
	int iError = 0;
	AnsiString strFileName;

	// mode check
	if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode==MODE_AUTO)
	{
		switch(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status)
		{
            //업데이트 가능한 상태
            case STS_INIT:
            case STS_ARRIVED:
			case STS_UNLOADED:
            case STS_LOADED:
			case STS_COMPLETE:
            //case STS_ERROR:
				ADD_LOG_TASKCONTROL("[Param Apply] Mode Ok. Now Mode : %d",
					 					m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status);
			break;

			default:	//error
				ADD_LOG_TASKCONTROL("[Param Apply] Mode Error! Now Mode : %d",
	 					m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status);

				return -1; // exit
			break;
		}
	}
	else	// manual mode
	{
		ADD_LOG_TASKCONTROL("[Param Apply] Manual Mode. Update Possible");
	}

	switch(iUpdateFile)
	{
		case UPDATE_DRIVING_INI:
		{
			// (1) Driving.ini --------------------------------
			// 1-1. Data Loading
			ADD_LOG_TASKCONTROL("[Param Apply] my routine");

			// memory save for debug
			strFileName = OHT_PARAM_FILE_PATH;
			strFileName = strFileName + "Driving_mem.INI";
			m_pDrivingControl->saveParameter(strFileName.c_str(), m_pDrivingControl->m_pParamSet);


			// loading
			strFileName = OHT_PARAM_FILE_PATH;
			strFileName = strFileName + "Driving.INI";
			iError = m_pDrivingControl->loadParameter(strFileName.c_str(), m_pDrivingControl->m_pParamSet);

			if(iError== 1)	// success
			{

				ADD_LOG_TASKCONTROL("[Param Apply] Driving control reset....");
				m_pDrivingControl->Reset_DrivingControl();


				if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO)
				{
					ChangeMode(MODE_MANUAL);
					ADD_LOG_TASKCONTROL("[Param Apply] Auto Remark");
					// Sleep(500);
					Change2AutoMode();
					Change_FDC_Auto_Mode();
				}

			ADD_LOG_TASKCONTROL("[Param Apply] Driving.INI Load Result : %d (1 = success)", iError);
			}
		}
		break;

		case UPDATE_TRANS_INI:
		{
			// (2) Trans.ini -----------------------------
			strFileName = OHT_PARAM_FILE_PATH;
			strFileName = strFileName + "Trans.ini";
			iError= m_pTransControl->LoadParam_New(strFileName.c_str(), "DataFiles\\Trans_mem.ini");

			if(iError== 1)
			{
				 m_pTransControl->Reset_TransControl();

			}
			ADD_LOG_TASKCONTROL("[Param Apply] Trans.ini result : %d (1=success)", iError);
		}
		break;

		case UPDATE_EQPIO_INI:
		{
			// (3) EQ PIO.ini -----------------------------
			// (확인요) Right도 있을텐데 이것은 어떻게 처리하나 => 현재 안 쓰므로 불필요하다

//			iError = m_pHWSet->m_pLeftEQPIO->ReadEQPIOTimeover();
			ADD_LOG_TASKCONTROL("[Param Apply] EQ PIO.ini Left result : %d (0=success)", iError);

//			if (iError == 0)
//				m_pHWSet->m_pLeftEQPIO->Reset_EQPIO();
		}
		break;

		case UPDATE_DIAG_PARAM_INI:
		{
			// (4) DiagParam.ini 재 로딩
//			iError = m_DiagManager->LoadDiagParam();

			if(iError == 0)
			{
				m_DiagManager->Reset_DiagManager();
			}
			
			ADD_LOG_TASKCONTROL("[Param Apply] DiagParam.ini result : %d (0=success)", iError);
		}
		break;

		case UPDATE_PATHSEARCH_INI:
		{
			// (5) PathSearch.ini 재 로딩
			iError = m_pPathSearch->loadParameter("DataFiles\\PathSearch.ini", m_pPathSearch->m_pParamSet);
			ADD_LOG_TASKCONTROL("[Param Apply] PathSearch.ini result : %d (1=success)", iError);
		}
		break;


		case UPDATE_IOMAP_INI:
		{
			// (6) IOMap.ini
			strFileName = OHT_PARAM_FILE_PATH;
			strFileName = strFileName + "IOMap.ini";

			strIOMapPath = strFileName;

			// Load IO Map Field
			#if((OHT_NAME == OHT_NAME_STD_V80))
				iError = LoadIOMapField(L"FOUP_R01", L"Ver 7.0");
			#elif(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
				iError = LoadIOMapField("RETICLE", "Ver 8.0");
			#elif(OHT_NAME == OHT_NAME_STD_V81S)
				iError = LoadIOMapField("FOUP", "Ver 8.1S");
			#elif(OHT_NAME == OHT_NAME_STD_V81)
				iError = LoadIOMapField("FOUP", "Ver 8.1");
			#elif(OHT_NAME == OHT_NAME_STD_V85)
				iError = LoadIOMapField("EUV", "Ver 8.5");
			#else
				iError = LoadIOMapField("FOUP", "Ver 7.0");
			#endif

			if (iError) {
				ADD_LOG_TASKCONTROL("[Param Apply] Success to Load IOMap.ini file...");
				m_IOMapField->fIsLoaded = true;
			}
			else
				ADD_LOG_TASKCONTROL("[Param Apply] Fail to Load IOMap.ini file");
		}
		break;
	}
}

void TaskControl::IpStrToByteAddr(AnsiString strAddr, int& iHi, int& iM1, int& iM2, int& iLow)
{
	AnsiString strTmp, strValue;
	int iSep, iLength;

	strTmp = strAddr;
	strTmp = strTmp.Trim();

	// 1		strATDriveData = strATData.SubString(1, (iAT2Length - 1));
	iSep = strTmp.Pos("."); 						// 점의 위치 찾는다
	strValue = strTmp.SubString(1, (iSep - 1)); 	// 점 앞까지 자른다
	iHi = strValue.ToInt();

	iLength = strTmp.Length() - iSep;	// 첫번째 점까지 빼고난 길이
	strTmp = strTmp.SubString((iSep + 1), iLength);  // 점 뒤부터 끝까지 자른다

	// 2
	iSep = strTmp.Pos("."); 						// 점의 위치 찾는다
	strValue = strTmp.SubString(1, (iSep - 1)); 	// 점 앞까지 자른다
	iM1 = strValue.ToInt();

	iLength = strTmp.Length() - iSep;	// 첫번째 점까지 빼고난 길이
	strTmp = strTmp.SubString((iSep + 1), iLength);  // 점 뒤부터 끝까지 자른다

	// 3
	iSep = strTmp.Pos("."); 						// 점의 위치 찾는다
	strValue = strTmp.SubString(1, (iSep - 1)); 	// 점 앞까지 자른다
	iM2 = strValue.ToInt();

	iLength = strTmp.Length() - iSep;	// 첫번째 점까지 빼고난 길이
	strTmp = strTmp.SubString((iSep + 1), iLength);  // 점 뒤부터 끝까지 자른다

	// 4
	iLow = strTmp.ToInt();
}

/**
@brief	Handy TP로 Longrun 명령 발생 시, 지속적으로 롱런 동작 발생 함수
@data   2016.12.25
@author	Labeler
@return Error 발생 시 ErrorCode
        Error 미 발생시 NO_ERR
*/
int TaskControl::MakeLongrunCmdForHandyTP(char* szReadBuff, uINPUT_BITFIELD uInput)
{
    // 2016-12-25, Add by Labeler
    // Longrun 동작을 위해 Logic 추가
    // OHT의 상태가 바뀔 동안의 시간 처리 필요
    static int nCntChangeStatus = 0;
    int nError = NO_ERR;
    STATION_DATA *pStationData;

    if(m_pCmdManager->Is_Executable() == NO_ERR)
    {
        Analysis_IOData();
        HWSet * m_pHWSet = OHTMainForm->GetHWSet();

        if(m_pHWSet == NULL)
        {
            return nError;
        }

        switch(GetHandyTPLongRun())
        {
            case HANDTP_LONGRUN_TRANS:
            {
                if((GetExecuteStatus() == STS_INIT)
				|| (GetExecuteStatus() == STS_ARRIVED)
                || (GetExecuteStatus() == STS_COMPLETE))
                {
                    if((m_pHWSet->m_pHand->IsClose() == true)
                    && (m_pHWSet->m_pHand->IsRightGripOn() == true) && (m_pHWSet->m_pHand->IsLeftGripOn() == true))
                    {
                        if(nCntChangeStatus >= 100)
                        {
                            pStationData = OHTMainForm->GetStationDataManager()->Find(m_pExecuteInfo->ExecutePositionInfo.CurrStation);

                            if(pStationData == NULL)
                            {
                                nError = ERR_DATA_STATION_NOEXIST;
                                ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", m_pExecuteInfo->ExecutePositionInfo.CurrStation);
                                break;
                            }
                            else
                            {
                                szReadBuff[7] = ((pStationData->StationID) >> 16) & 0xFF;
                                szReadBuff[8] = ((pStationData->StationID) >> 8) & 0xFF;
                                szReadBuff[9] = (pStationData->StationID) & 0xFF;

                                szReadBuff[10] = ((pStationData->ParentNodeID) >> 16) & 0xFF;
                                szReadBuff[11] = ((pStationData->ParentNodeID) >> 8) & 0xFF;
                                szReadBuff[12] = (pStationData->ParentNodeID) & 0xFF;

                                szReadBuff[13] = m_nPortType_HandTPLongrun;
                            }

                            szReadBuff[0] = REMOTE_TYPE_RF;
                            szReadBuff[1] = CYCLE_CMD;
                            szReadBuff[5] = (CYC_NOMOVE_LOAD >> 8) & 0xFF;
                            szReadBuff[6] = CYC_NOMOVE_LOAD & 0xFF;

                            nCntChangeStatus = 0;
                            nError = MakeCmdList_HandTP(szReadBuff, true,	uInput);
                        }
						else
                        {
                            nCntChangeStatus++;
                        }
                    }
                    else if((m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false))
                    {
                        if(nCntChangeStatus >= 100)
                        {
                            pStationData = OHTMainForm->GetStationDataManager()->Find(m_pExecuteInfo->ExecutePositionInfo.CurrStation);

                            if(pStationData == NULL)
                            {
                                nError = ERR_DATA_STATION_NOEXIST;
                                ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", m_pExecuteInfo->ExecutePositionInfo.CurrStation);
                                break;
                            }
                            else
                            {
                                szReadBuff[7] = ((pStationData->StationID) >> 16) & 0xFF;
                                szReadBuff[8] = ((pStationData->StationID) >> 8) & 0xFF;
                                szReadBuff[9] = (pStationData->StationID) & 0xFF;

                                szReadBuff[10] = ((pStationData->ParentNodeID) >> 16) & 0xFF;
                                szReadBuff[11] = ((pStationData->ParentNodeID) >> 8) & 0xFF;
                                szReadBuff[12] = (pStationData->ParentNodeID) & 0xFF;

                                szReadBuff[13] = m_nPortType_HandTPLongrun;
                            }

                            szReadBuff[0] = REMOTE_TYPE_RF;
                            szReadBuff[1] = CYCLE_CMD;
                            szReadBuff[5] = (CYC_NOMOVE_UNLOAD >> 8) & 0xFF;
                            szReadBuff[6] = CYC_NOMOVE_UNLOAD & 0xFF;
                            nCntChangeStatus = 0;
                            nError = MakeCmdList_HandTP(szReadBuff, true,	uInput);
                        }
						else
                        {
                            nCntChangeStatus++;
                        }
                    }
                }
                else if(GetExecuteStatus() == STS_UNLOADED)
                {
                    if(nCntChangeStatus >= 100)
                    {
                        pStationData = OHTMainForm->GetStationDataManager()->Find(m_pExecuteInfo->ExecutePositionInfo.CurrStation);

                        if(pStationData == NULL)
                        {
                            nError = ERR_DATA_STATION_NOEXIST;
                            ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", m_pExecuteInfo->ExecutePositionInfo.CurrStation);
                            break;
                        }
                        else
                        {
                            szReadBuff[7] = ((pStationData->StationID) >> 16) & 0xFF;
                            szReadBuff[8] = ((pStationData->StationID) >> 8) & 0xFF;
                            szReadBuff[9] = (pStationData->StationID) & 0xFF;

                            szReadBuff[10] = ((pStationData->ParentNodeID) >> 16) & 0xFF;
                            szReadBuff[11] = ((pStationData->ParentNodeID) >> 8) & 0xFF;
                            szReadBuff[12] = (pStationData->ParentNodeID) & 0xFF;

                            szReadBuff[13] = m_nPortType_HandTPLongrun;
                        }

                        szReadBuff[0] = REMOTE_TYPE_RF;
                        szReadBuff[1] = CYCLE_CMD;
                        szReadBuff[5] = (CYC_NOMOVE_LOAD >> 8) & 0xFF;
                        szReadBuff[6] = CYC_NOMOVE_LOAD & 0xFF;
                        nCntChangeStatus = 0;
                        nError = MakeCmdList_HandTP(szReadBuff, true,	uInput);
					}
                    else
                    {
                        nCntChangeStatus++;
                    }
                }
                else if(GetExecuteStatus() == STS_LOADED)
                {
                    if(nCntChangeStatus >= 100)
                    {
                        pStationData = OHTMainForm->GetStationDataManager()->Find(m_pExecuteInfo->ExecutePositionInfo.CurrStation);

                        if(pStationData == NULL)
                        {
                            nError = ERR_DATA_STATION_NOEXIST;
                            ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", m_pExecuteInfo->ExecutePositionInfo.CurrStation);
                            break;
                        }
                        else
                        {
                            szReadBuff[7] = ((pStationData->StationID) >> 16) & 0xFF;
                            szReadBuff[8] = ((pStationData->StationID) >> 8) & 0xFF;
                            szReadBuff[9] = (pStationData->StationID) & 0xFF;

                            szReadBuff[10] = ((pStationData->ParentNodeID) >> 16) & 0xFF;
                            szReadBuff[11] = ((pStationData->ParentNodeID) >> 8) & 0xFF;
                            szReadBuff[12] = (pStationData->ParentNodeID) & 0xFF;

                            szReadBuff[13] = m_nPortType_HandTPLongrun;
                        }

                        szReadBuff[0] = REMOTE_TYPE_RF;
                        szReadBuff[1] = CYCLE_CMD;
                        szReadBuff[5] = (CYC_NOMOVE_UNLOAD >> 8) & 0xFF;
                        szReadBuff[6] = CYC_NOMOVE_UNLOAD & 0xFF;
                        nCntChangeStatus = 0;
                        nError = MakeCmdList_HandTP(szReadBuff, true,	uInput);
					}
                    else
                    {
                        nCntChangeStatus++;
                    }
                }
                else
                {
                    nCntChangeStatus = 0;
                }
            }
            break;

            case HANDTP_LONGRUN_HAND:
            {
                if(((GetExecuteStatus() == STS_COMPLETE) || (GetExecuteStatus() == STS_INIT) || (GetExecuteStatus() == STS_ARRIVED))
                && (m_pHWSet->m_pHand->IsClose(BOTH)))
                {
                    if(nCntChangeStatus >= 100)
                    {
                        szReadBuff[0] = REMOTE_TYPE_RF;
                        szReadBuff[1] = CYCLE_CMD;
                        szReadBuff[5] = (CYC_HAND_OPEN >> 8) & 0xFF;
                        szReadBuff[6] = CYC_HAND_OPEN & 0xFF;
                        nCntChangeStatus = 0;

                        nError = MakeCmdList_HandTP(szReadBuff, true,	uInput);
                    }
                    else
                    {
                        nCntChangeStatus++;
                    }
                }
                else if(((GetExecuteStatus() == STS_COMPLETE) || (GetExecuteStatus() == STS_INIT) || (GetExecuteStatus() == STS_ARRIVED))
                && (m_pHWSet->m_pHand->IsOpen(BOTH)))
                {
                    if(nCntChangeStatus >= 100)
					{
                        szReadBuff[0] = REMOTE_TYPE_RF;
                        szReadBuff[1] = CYCLE_CMD;
                        szReadBuff[5] = (CYC_HAND_CLOSE >> 8) & 0xFF;
                        szReadBuff[6] = CYC_HAND_CLOSE & 0xFF;
                        nCntChangeStatus = 0;
                        nError = MakeCmdList_HandTP(szReadBuff, true,	uInput);
                    }
                    else
                    {
                        nCntChangeStatus++;
                    }
                }
                else
                {
                    nCntChangeStatus = 0;
                }
            }
            break;

            case HANDTP_LONGRUN_SHUTTER:
            {
                if(((GetExecuteStatus() == STS_COMPLETE) || (GetExecuteStatus() == STS_INIT) || (GetExecuteStatus() == STS_ARRIVED))
                && (m_pHWSet->m_pShutter->IsClose(BOTH)))
                {
                    if(nCntChangeStatus >= 100)
                    {
                        szReadBuff[0] = REMOTE_TYPE_RF;
                        szReadBuff[1] = CYCLE_CMD;
                        szReadBuff[5] = (CYC_SHUTTER_OPEN >> 8) & 0xFF;
                        szReadBuff[6] = CYC_SHUTTER_OPEN & 0xFF;
                        nCntChangeStatus = 0;
                        nError = MakeCmdList_HandTP(szReadBuff, true,	uInput);
                    }
                    else
                    {
                        nCntChangeStatus++;
					}
                }
                else if(((GetExecuteStatus() == STS_COMPLETE) || (GetExecuteStatus() == STS_INIT) || (GetExecuteStatus() == STS_ARRIVED))
                && (m_pHWSet->m_pShutter->IsOpen(BOTH)))
                {
                    if(nCntChangeStatus >= 100)
                    {
                        szReadBuff[0] = REMOTE_TYPE_RF;
                        szReadBuff[1] = CYCLE_CMD;
                        szReadBuff[5] = (CYC_SHUTTER_CLOSE >> 8) & 0xFF;
                        szReadBuff[6] = CYC_SHUTTER_CLOSE & 0xFF;
                        nCntChangeStatus = 0;
                        nError = MakeCmdList_HandTP(szReadBuff, true,	uInput);
                    }
                    else
                    {
                        nCntChangeStatus++;
                    }
                }
                else
                {
                    nCntChangeStatus = 0;
                }
            }
            break;

            case HANDTP_LONGRUN_STEER:
            {
                if(((GetExecuteStatus() == STS_COMPLETE) || (GetExecuteStatus() == STS_INIT) || (GetExecuteStatus() == STS_ARRIVED))
                && (m_pHWSet->m_pSteer->IsLeftPos(BOTH)))
                {
                    if(nCntChangeStatus >= 100)
                    {
                        szReadBuff[0] = REMOTE_TYPE_RF;
                        szReadBuff[1] = CYCLE_CMD;
                        szReadBuff[5] = (CYC_STEER_RIGHT >> 8) & 0xFF;
                        szReadBuff[6] = CYC_STEER_RIGHT & 0xFF;
						nCntChangeStatus = 0;
                        nError = MakeCmdList_HandTP(szReadBuff, true,	uInput);
                    }
                    else
                    {
                        nCntChangeStatus++;
                    }
                }
                else if(((GetExecuteStatus() == STS_COMPLETE) || (GetExecuteStatus() == STS_INIT) || (GetExecuteStatus() == STS_ARRIVED))
                && (m_pHWSet->m_pSteer->IsRightPos(BOTH)))
                {
                    if(nCntChangeStatus >= 100)
                    {
                        szReadBuff[0] = REMOTE_TYPE_RF;
                        szReadBuff[1] = CYCLE_CMD;
                        szReadBuff[5] = (CYC_STEER_LEFT >> 8) & 0xFF;
                        szReadBuff[6] = CYC_STEER_LEFT & 0xFF;
                        nCntChangeStatus = 0;
                        nError = MakeCmdList_HandTP(szReadBuff, true,	uInput);
                    }
                    else
                    {
                        nCntChangeStatus++;
                    }
                }
                else
                {
                    nCntChangeStatus = 0;
                }
            }
            break;

            default:
            {
                nCntChangeStatus = 0;
            }
            break;
		}
    }
    return nError;
}

/**
@brief   OCS로 응답이 없는 경우 Ping 체크를 하여 무선 Client reset 수행
@author  sh17.jo
@date    2016.12.15
*/
#if(WirelessClient_TYPE == XR5_WIRELESS_CLIENT)
void TaskControl::CheckCommfail(void)
{
	int nOcsCnt = 0;
	AnsiString strPri, strSec, strGateway, strLog = "";
	NETWORK_RESET_STATE NetState[3];

    // 임시 코드
	// OCS_P, OCS_S IP 주소 (동일 라인 단말별 파라미터 동일)
    // 파라미터로 관리할지 OCS 송신 데이터를 활용하여 사용할지 결정 필요
	unsigned char szOCSPriIP[20] = "0.0.0.0";
    unsigned char szOCSSecIP[20] = "0.0.0.0";

    // Gateway IP 주소 (동일 라인 단말별 파라미터 동일)
    // 파라미터로 관리할지 직접 랜카드에서 가지고 올지 결정 필요
    unsigned char szGatewayIP[20] = "0.0.0.0";

    // OCS IP 주소 획득 코드
	nOcsCnt = m_pUDPCommunication->OcsConnected();
	if(nOcsCnt == 1)
	{
		m_pUDPCommunication->GetOcsAddress(szOCSPriIP, nOcsCnt);
    }
    else if(nOcsCnt == 2)
    {
		m_pUDPCommunication->GetOcsAddress(szOCSPriIP, nOcsCnt-1);
		m_pUDPCommunication->GetOcsAddress(szOCSSecIP, nOcsCnt);
	}
    else
    {
        // 연결된 OCS 수량이 0인 경우
        Ping_LOG("#0 OCS IP 주소 획득 불가, OCS 접속 수: 0개");
    }

    // Gateway IP 주소  획득 코드
    strcpy(szGatewayIP, m_defualtparam->GatewayIP);

    // 1. OCS_P, OCS_S Ping Check
	NetState[OCS_P] = CheckPingBeforeReset(szOCSPriIP);
	NetState[OCS_S] = CheckPingBeforeReset(szOCSSecIP);

    // 1-1. Gateway Ping Check
    NetState[GATEWAY] = CheckPingBeforeReset(szGatewayIP);

    //2. 결과처리
	strPri = ConvertNetResetToString(NetState[OCS_P]);
	strSec = ConvertNetResetToString(NetState[OCS_S]);
    strGateway = ConvertNetResetToString(NetState[GATEWAY]);

   	//2-1. Ping이 OCS서버 한군데라도 갔다면 서버와 연결이 되어있는 상태
	if(NetState[OCS_P] == NETRESET_PING_SUCCESS || NetState[OCS_S] == NETRESET_PING_SUCCESS)
    {
        m_nComfailCount[0] = 0;
		m_nComOKCount++;
        strLog.sprintf("#1 PingOK Pri[%s]:%s Sec[%s]:%s, Count:%d", szOCSPriIP, strPri, szOCSSecIP, strSec, m_nComOKCount);
        Ping_LOG(strLog.c_str());

		//Ping이 10회 이상 성공할 경우 체크 중지
		if(m_nComOKCount>=10)
		{
			m_bSetCheckCommfail=false;
			m_nComfailCount[0]=0;
			m_nComOKCount=0;
			Ping_LOG("#1-1 Connection with OCS is OK, Stop Ping Check");
			Ping_LOG("#1-1 OCS 이상, 유무선 네트워크 이상 없음");

		}
    }
    //2-2. 연결된 이력이 있었으나 끊어진 경우 Reset시도(Network Reset으로 해결해볼 수 있음)
	else if(NetState[OCS_P] == NETRESET_PING_FAIL || NetState[OCS_S] == NETRESET_PING_FAIL)
    {
        // Gateway로 Ping NG (무선 구간 이슈)
		if(NetState[GATEWAY] == NETRESET_PING_FAIL)
        {
			m_nComOKCount = 0;
			m_nComfailCount[0]++;

			strLog.sprintf("#2 PingFail Pri[%s]:%s, Sec[%s]:%s, Gateway[%s]:%s, Count:%d", szOCSPriIP, strPri, szOCSSecIP, strSec, szGatewayIP, strGateway, m_nComfailCount);
			Ping_LOG(strLog.c_str());

			// 무선 Client Reset 동작 수행 (CheckCommfail 함수가 1초 마다 호출되며 ping time out 발생시 1초 소요)
			// 장비간 Ping 테스트가  연속적으로 Ping Fail 발생 시 무선 Client Reset 수행
			if(m_nComfailCount[0] == 10 || m_nComfailCount[0] == 25 || m_nComfailCount[0] == 40)
			{
				if(m_NetWorkDll != NULL)
				{
					m_NetWorkDll->DisconnectFlag = true;		// 무선랜 정보 요청 중지

					m_NetWorkDll->NetWork_Adapter_Disable();
					Ping_LOG("#2-1 Network adapter disable");

				    Sleep(2000);								// 무선랜 Disable 후 Enable시 2초 지연 필요

				    Ping_LOG("#2-1 Network adapter enable");
					m_NetWorkDll->Network_Adapter_Enable();

					m_NetWorkDll->DisconnectFlag = false;		// 무선랜 정보 요청 시작
                }
			    else
			    {
				    Ping_LOG("#2-2 NetworkDll is NULL...");
				}
		    }
		    //50회를 넘기면 더이상 Reset 시도하지 않음
		    if(m_nComfailCount[0] > 40)
		    {
			    Ping_LOG("2-3 Restarting Wireless Client was failed...");
			    m_bSetCheckCommfail=false;
			    m_nComfailCount[0]=0;
			    m_nComOKCount=0;
	    	}
        }
        // Gateway로 Ping OK
        else
        {
            strLog.sprintf("#3 Error Pri[%s]:%s, Sec[%s]:%s, Gateway[%s]:%s", szOCSPriIP, strPri, szOCSSecIP, strSec, szGatewayIP, strGateway);
		    Ping_LOG(strLog.c_str());
            m_bSetCheckCommfail=false;
		    m_nComfailCount[0]=0;
		    m_nComOKCount=0;

        }
    }
    //2-3. OCS 서버 IP가 모두 0.0.0.0인 경우
	else
	{
		strLog.sprintf("#4 Error Pri[%s]:%s Sec[%s]:%s, Gateway[%s]:%s", szOCSPriIP, strPri, szOCSSecIP, strSec, szGatewayIP, strGateway);
		Ping_LOG(strLog.c_str());

		m_bSetCheckCommfail=false;
		m_nComfailCount[0]=0;
		m_nComOKCount=0;
	}
}
#else
void TaskControl::CheckCommfail(void)
{
	int nOcsCnt = 0;
	AnsiString strPri, strSec, strGateway, strWirelessClient, strOCSGateWay, strLog = "";
	int iResult = 0;
	unsigned int CheckTime[4] = {0, 0, 0, 0};
    static DWORD dwCommfailTime[4];
    static DWORD dwCommOKTime;
    static bool bCheck[3] = {false, false, false};
	DWORD dwStartTime;
	NETWORK_RESET_STATE NetState[4];

	// 임시 코드
	// OCS_P, OCS_S IP 주소 (동일 라인 단말별 파라미터 동일)
	// 파라미터로 관리할지 OCS 송신 데이터를 활용하여 사용할지 결정 필요
	unsigned char szOCSPriIP[20] = "0.0.0.0";
	unsigned char szOCSSecIP[20] = "0.0.0.0";

    // Gateway IP 주소 (동일 라인 단말별 파라미터 동일)
	//unsigned char szGatewayIP[20] = "0.0.0.0";	// 주석: m_defualtparam 멤버변수 사용

    // Wireless Client IP 주소 (단말별 파라미터 다름)
	//unsigned char szWirelessClientIP[20] = "0.0.0.0";		// 주석: m_defualtparam 멤버변수 사용

	// OCS IP 주소 획득 코드
	nOcsCnt = m_pUDPCommunication->OcsConnected();
	if(nOcsCnt == 1)
    {
		m_pUDPCommunication->GetOcsAddress(szOCSPriIP, nOcsCnt);
	}
    else if(nOcsCnt == 2)
    {
		m_pUDPCommunication->GetOcsAddress(szOCSPriIP, nOcsCnt-1);
		m_pUDPCommunication->GetOcsAddress(szOCSSecIP, nOcsCnt);
    }
    else
    {
        // 연결된 OCS 수량이 0인 경우
		Ping_LOG("#0 OCS IP 주소 획득 불가, OCS 접속 수: 0개");
		m_bSetCheckCommfail=false;
		return;
	}

    // 1. OCS_P, OCS_S Ping Check
	NetState[OCS_P] = CheckPingBeforeReset(szOCSPriIP);
	NetState[OCS_S] = CheckPingBeforeReset(szOCSSecIP);

    // 1-1. Gateway Ping Check
	NetState[GATEWAY] = CheckPingBeforeReset(m_defualtparam->GatewayIP);

    // 1-2. Wireless Client Ping Check
	NetState[WIRELESS_CLIENT] = CheckPingBeforeReset(m_defualtparam->WirelessClientIP);

	// OCS의 Gateway Ping Check
	NetState[OCS_GATEWAY] = CheckPingBeforeReset(m_defualtparam->GatewayIP_OCS);

	//2. 결과처리
	strPri = ConvertNetResetToString(NetState[OCS_P]);
	strSec = ConvertNetResetToString(NetState[OCS_S]);
	strGateway = ConvertNetResetToString(NetState[GATEWAY]);
	strWirelessClient = ConvertNetResetToString(NetState[WIRELESS_CLIENT]);
	strOCSGateWay = ConvertNetResetToString(NetState[OCS_GATEWAY]);
	
	//2-1. Ping이 OCS서버 한군데라도 갔다면 서버와 연결이 되어있는 상태
	if(NetState[OCS_P] == NETRESET_PING_SUCCESS || NetState[OCS_S] == NETRESET_PING_SUCCESS)
	{
		//memset(m_nComfailCount, 0, sizeof(m_nComfailCount));

		if(m_nComOKCount == 0)
        {
			dwCommOKTime = timeGetTime();
        }
		m_nComOKCount++;
		strLog.sprintf("#1 PingOK Pri[%s]:%s Sec[%s]:%s, Count:%d", szOCSPriIP, strPri, szOCSSecIP, strSec, m_nComOKCount);
		Ping_LOG(strLog.c_str());
		dwStartTime = timeGetTime();

		if(OHTUTILITY::PreventOverflowGetTimeMs(dwCommOKTime) > m_defualtparam->nMaxPingCheckTime_ETC)
		{
			m_bSetCheckCommfail=false;
			memset(m_nComfailCount, 0, sizeof(m_nComfailCount));
			m_nComOKCount=0;
			Ping_LOG("#1-1 Connection with OCS is OK, Stop Ping Check");
			Ping_LOG("#1-1 OCS 이상, 유무선 네트워크 이상 없음");
		}
    }
	//2-2. 연결된 이력이 있었으나 끊어진 경우 Reset시도(Network Reset으로 해결해볼 수 있음)
	else if(NetState[OCS_P] == NETRESET_PING_FAIL || NetState[OCS_S] == NETRESET_PING_FAIL)
	{
		// Gateway로 Ping NG, 무선 단말 Ping OK (무선 구간 이슈)
		// ppt 문서 3번 케이스 : OHT Gateway 두개 모두 Fail 이지만, OCS Gateway와 무선모듈은 Sucess 상태 ==> 무선모듈 리셋 시도 필요
		if((NetState[GATEWAY] == NETRESET_PING_FAIL || NetState[OCS_GATEWAY] == NETRESET_PING_FAIL) && NetState[WIRELESS_CLIENT] == NETRESET_PING_SUCCESS)
		{
			//AOHC-234 C3850 Switch Fail-Over 시 통신 끊김 개선
			CheckARPTableClear(m_defualtparam->GatewayIP);

			m_nComOKCount = 0;
			if(m_nComfailCount[0] == 0)
			{
				dwCommfailTime[0] = timeGetTime();
                bCheck[0] = true;
				bCheck[1] = true;
                bCheck[2] = true;
            }
			m_nComfailCount[0]++;

			strLog.sprintf("#2 PingFail Pri[%s]:%s, Sec[%s]:%s, Gateway[%s]:%s, WirelessClient[%s]:%s, OCS GateWay[%s]:%s, Count:%d", szOCSPriIP, strPri, szOCSSecIP, strSec, m_defualtparam->GatewayIP, strGateway, m_defualtparam->WirelessClientIP, strWirelessClient, m_defualtparam->GatewayIP_OCS, strOCSGateWay, m_nComfailCount[0]);
			Ping_LOG(strLog.c_str());

            dwStartTime = timeGetTime();

			if(OHTUTILITY::PreventOverflowGetTimeMs(dwCommfailTime[0]) > m_defualtparam->nMaxPingCheckTime_Step3)
			{
				if(bCheck[2] == false)
				  return;
                // Wireless Client Reset 함수 호출
				if(m_NetWorkDll != NULL)
			    {
					Ping_LOG("#2-3 무선 네트워크 장비(AP/무선 Client) 이슈 발생 -> 무선 Client Reset 필요");
					Ping_LOG("#2-3 Wireless Client Reset!");
					m_NetWorkDll->DisconnectFlag = true;		// 무선랜 정보 요청 중지

					// 무선 Client Reset 함수 호출
					iResult = Wireless_Module_Reset((char*)&m_defualtparam->WirelessClientIP);

					strLog.sprintf("#2-3 Wireless Client Reset -> Complete. Reset Result:%d (0=WIRELESS_RESET_OK)", iResult);
			        Ping_LOG(strLog.c_str());
					m_NetWorkDll->DisconnectFlag = false;		// 무선랜 정보 요청 시작
				            bCheck[2] = false;
                }
			    else
			    {
				    Ping_LOG("#2-3 NetworkDll is NULL.");
		    	}

                // Wireless Client Reset 기능 종료
				m_bSetCheckCommfail=false;
				memset(m_nComfailCount, 0, sizeof(m_nComfailCount));
				m_nComOKCount=0;
			}
			else if(OHTUTILITY::PreventOverflowGetTimeMs(dwCommfailTime[0]) > m_defualtparam->nMaxPingCheckTime_Step2)
			{
                if(bCheck[1] == false)
                  return;
                // Wireless Client Reset 함수 호출
                if(m_NetWorkDll != NULL)
				{
                    Ping_LOG("#2-2 무선 네트워크 장비(AP/무선 Client) 이슈 발생 -> 무선 Client Reset 필요");
                    Ping_LOG("#2-2 Wireless Client Reset!");
					m_NetWorkDll->DisconnectFlag = true;		// 무선랜 정보 요청 중지

					// 무선 Client Reset 함수 호출
					iResult = Wireless_Module_Reset((char*)&m_defualtparam->WirelessClientIP);

                    strLog.sprintf("#2-2 Wireless Client Reset -> Complete. Reset Result:%d (0=WIRELESS_RESET_OK)", iResult);
			        Ping_LOG(strLog.c_str());
					m_NetWorkDll->DisconnectFlag = false;		// 무선랜 정보 요청 시작
				            bCheck[1] = false;
                }
			    else
				{
				    Ping_LOG("#2-2 NetworkDll is NULL.");
		    	}
                m_nComfailCount[1] = 0;
                m_nComfailCount[2] = 0;
                m_nComfailCount[3] = 0;
            }
			else if(OHTUTILITY::PreventOverflowGetTimeMs(dwCommfailTime[0]) > m_defualtparam->nMaxPingCheckTime_Step1)
            {
                if(bCheck[0] == false)
                  return;
                // Wireless Client Reset 함수 호출
                if(m_NetWorkDll != NULL)
			    {
                    Ping_LOG("#2-1 무선 네트워크 장비(AP/무선 Client) 이슈 발생 -> 무선 Client Reset 필요");
					Ping_LOG("#2-1 Wireless Client Reset!");
					m_NetWorkDll->DisconnectFlag = true;		// 무선랜 정보 요청 중지

                    // 무선 Client Reset 함수 호출
                    iResult = Wireless_Module_Reset((char*)&m_defualtparam->WirelessClientIP);

                    strLog.sprintf("#2-1 Wireless Client Reset -> Complete. Reset Result:%d (0=WIRELESS_RESET_OK)", iResult);
			        Ping_LOG(strLog.c_str());
				    m_NetWorkDll->DisconnectFlag = false;		// 무선랜 정보 요청 시작
				            bCheck[0] = false;
                }
			    else
			    {
					Ping_LOG("#2-1 NetworkDll is NULL.");
		    	}
                m_nComfailCount[1] = 0;
                m_nComfailCount[2] = 0;
				m_nComfailCount[3] = 0;
            }
		}
        // Gateway로 Ping NG, 무선 단말 Ping NG (무선 단말 전원 OFF 또는 IPC ↔ 무선 Client간 UTP 케이블 이상)
		else if(NetState[GATEWAY] == NETRESET_PING_FAIL && NetState[WIRELESS_CLIENT] == NETRESET_PING_FAIL)
		{
			m_nComOKCount = 0;
			if(m_nComfailCount[1] == 0)
            {
				dwCommfailTime[1] = timeGetTime();
			}
			m_nComfailCount[1]++;
			strLog.sprintf("#3 PingFail Pri[%s]:%s, Sec[%s]:%s, Gateway[%s]:%s, WirelessClient[%s]:%s, OCS GateWay[%s]:%s, Count:%d", szOCSPriIP, strPri, szOCSSecIP, strSec, m_defualtparam->GatewayIP, strGateway, m_defualtparam->WirelessClientIP, strWirelessClient, m_defualtparam->GatewayIP_OCS, strOCSGateWay, m_nComfailCount[1]);
			Ping_LOG(strLog.c_str());
            dwStartTime = timeGetTime();
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwCommfailTime[1]) > m_defualtparam->nMaxPingCheckTime_ETC)
			{
				Ping_LOG("#3-1 무선 Client 전원 OFF 또는 IPC <-> 무선 Client간 UTP 케이블 상태 및 체결 불량 -> 점검 필요");
				m_bSetCheckCommfail=false;
				memset(m_nComfailCount, 0, sizeof(m_nComfailCount));
				m_nComOKCount=0;
				// 유선랜 포트 Reset 기능 추가 검토
			}
		}
		// Gateway로 Ping OK, 무선 단말 Ping OK (유선 구간 이슈 또는 OCS 이슈)
		// 무선 구간 이슈는 아니므로 무선 Client Rebooting 미수행
		else if(NetState[GATEWAY] == NETRESET_PING_SUCCESS && NetState[WIRELESS_CLIENT] == NETRESET_PING_SUCCESS)
		{
			m_nComOKCount = 0;
			if(m_nComfailCount[2] == 0)
            {
				dwCommfailTime[2] = timeGetTime();
            }
			m_nComfailCount[2]++;
			strLog.sprintf("#4 PingFail Pri[%s]:%s, Sec[%s]:%s, Gateway[%s]:%s, WirelessClient[%s]:%s, OCS GateWay[%s]:%s, Count:%d", szOCSPriIP, strPri, szOCSSecIP, strSec, m_defualtparam->GatewayIP, strGateway, m_defualtparam->WirelessClientIP, strWirelessClient, m_defualtparam->GatewayIP_OCS, strOCSGateWay, m_nComfailCount[2]);
			Ping_LOG(strLog.c_str());
			dwStartTime = timeGetTime();
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwCommfailTime[2]) > m_defualtparam->nMaxPingCheckTime_ETC)
			{
				Ping_LOG("#4-1 OCS P/S 동시 Down, 유무선 네트워크 이상 없음");
				m_bSetCheckCommfail=false;
				memset(m_nComfailCount, 0, sizeof(m_nComfailCount));
				m_nComOKCount=0;
			}
		}
		// Gateway로 Ping OK, 무선 단말 Ping NG (특수 Case)
		// 무선 구간 이슈는 아니므로 무선 Client Rebooting 미수행
		// Gateway, 무선 Client IP가 0.0.0.0으로 Ping 테스트 불가
		else
		{
			m_nComOKCount = 0;
            if(m_nComfailCount[3] == 0)
            {
                dwCommfailTime[3] = timeGetTime();
            }
			m_nComfailCount[3]++;
			strLog.sprintf("#5 Error Pri[%s]:%s, Sec[%s]:%s, Gateway[%s]:%s, WirelessClient[%s]:%s, OCS GateWay[%s]:%s", szOCSPriIP, strPri, szOCSSecIP, strSec, m_defualtparam->GatewayIP, strGateway, m_defualtparam->WirelessClientIP, strWirelessClient, m_defualtparam->GatewayIP_OCS, strOCSGateWay);
			Ping_LOG(strLog.c_str());
            dwStartTime = timeGetTime();
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwCommfailTime[3]) > m_defualtparam->nMaxPingCheckTime_ETC)
			{
				Ping_LOG("#5-1 OCS_P/S 동시 Down, 유무선 네트워크 이상 없음");
				Ping_LOG("#5-1 (참고) 실제 무선 Client IP 주소와 OHTParam.ini의 무선 Client IP 주소 불일치 -> 확인 필요");
				m_bSetCheckCommfail=false;
				memset(m_nComfailCount, 0, sizeof(m_nComfailCount));
				m_nComOKCount=0;
			}
		}
	}
    //2-3. OCS 서버 IP가 모두 0.0.0.0인 경우
	else
	{
		strLog.sprintf("#6 Error Pri[%s]:%s, Sec[%s]:%s, Gateway[%s]:%s, WirelessClient[%s]:%s, OCS GateWay[%s]:%s", szOCSPriIP, strPri, szOCSSecIP, strSec, m_defualtparam->GatewayIP, strGateway, m_defualtparam->WirelessClientIP, strWirelessClient, m_defualtparam->GatewayIP_OCS, strOCSGateWay);
		Ping_LOG(strLog.c_str());
		m_bSetCheckCommfail=false;
		memset(m_nComfailCount, 0, sizeof(m_nComfailCount));
		m_nComOKCount=0;
	}
}
#endif

/**
@brief   Commfail 체크 기능 on/off 변수 값 리턴
@author  sh17.jo
@date    2016.12.15
*/
//bool TaskControl::GetCheckCommfail()
//{
//	return m_bSetCheckCommfail;
//}


/**
@brief   OCS IP로 Ping 테스트 수행
@author  sh17.jo
@date    2016.12.15
*/
NETWORK_RESET_STATE TaskControl::CheckPingBeforeReset(unsigned char *ip)
{
    // OCS IP 전달 코드 구현 필요
    AnsiString strIP;

	if(ip == NULL)
		return NETRESET_IP_ABNORMAL;

	bool bIsIP = IsIpAddress(ip, strlen(ip));  // shcho added
	if(bIsIP == false)
	   return NETRESET_IP_ABNORMAL;

	strIP.printf("%s", ip);
	if(gm_Ping->Ping(strIP) < 0)
		return NETRESET_PING_FAIL;


	return NETRESET_PING_SUCCESS;
}

/**
@brief   NETWORK_RESET_STATE를 String으로 변환
@author  doori.shin
@date    2016.11.08
*/
char* TaskControl::ConvertNetResetToString(NETWORK_RESET_STATE state)
{
	AnsiString strState = "";

	switch(state)
	{
		case NETRESET_PING_SUCCESS:
			strState = "OK";
		break;
		case NETRESET_PING_FAIL:
			strState = "NG";
		break;
		case NETRESET_IP_ABNORMAL:
			strState = "Abnormal";
		break;
		case NETRESET_IP_NULL:
			strState = "Null";
		break;
		case NETRESET_ABNORMAL_CONNECTION:
			strState = "Abnormal Connection";
		break;
		case NETRESET_SOCKET_ERROR:
			strState = "Socket Error";
		break;
	}

	return strState.c_str();
}

WIRELESS_RESET_RESULT TaskControl::Wireless_Module_Reset(char* chAddr)
{
	AnsiString strLog = "";

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;
	WIRELESS_RESET_RESULT iResult = WIRELESS_RESET_OK;

	int strLen;
    int i;
    int Login_Packetlen = 0;
    int Password_Packetlen = 0;

    char message[100] = {0,};
    unsigned char UserID[10];
    unsigned char Password[10];
	unsigned char ServerIP[20];

    // FTP 서버 포트
	unsigned char FTP_Port[] = "21";

    // FTP 서버 Reset 명령어
	unsigned char FTP_Rst[] = "rst\r\n";

    // FTP 서버 Login(ID) 명령어
    unsigned char FTP_Login[20] = "USER ";

    // FTP 서버 Login(Password) 명령어
    unsigned char FTP_Password[20] = "PASS ";
	unsigned char newline[3] = "\r\n";

	strcpy(ServerIP, chAddr);
    strcpy(UserID, m_defualtparam->UserID);
    strcpy(Password, m_defualtparam->Password);
    strcat(FTP_Login, UserID);
    strcat(FTP_Login, newline);
    strcat(FTP_Password, Password);
    strcat(FTP_Password, newline);

    // FTP 서버 Login(ID) 패킷 길이 확인
    for(i = 0; i<sizeof(UserID); i++)
    {
        if(UserID[i] == '\0')
        {
            // USER [iD]\r\n
            Login_Packetlen = i+7;
            break;
        }
    }

    // FTP 서버 Login(Password) 패킷 길이 확인
    for(i = 0; i<sizeof(Password); i++)
    {
        if(Password[i] == '\0')
        {
            // PASS [password]\r\n
            Password_Packetlen = i+7;
            break;
        }
    }

	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
	{
		iResult =  WIRELESS_RESET_WSA_ERR;
	}

	if(iResult == WIRELESS_RESET_OK)
    {
	    hSocket = socket(PF_INET, SOCK_STREAM, 0);
        if(hSocket == INVALID_SOCKET)
        {
            iResult = WIRELESS_RESET_INVALID_SOCKET;
        }
    }

    if(iResult == WIRELESS_RESET_OK)
    {
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = inet_addr(ServerIP);
        servAddr.sin_port = htons(atoi(FTP_Port));

        // 블로킹 소켓이지만, 수신시 무한히 기다리지 않도록 하고, 타임아웃을 설정함
        int retval;
        int optval = 250;    // 500ms 만 대기해보고, 수신되는 데이터 없으면 다음 라인 실행
        retval = setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
        if(retval == SOCKET_ERROR)
            return -1;

        // 블로킹 소켓이지만, 송신시 무한히 기다리지 않도록 하고, 타임아웃을 설정함
        optval = 250;    // 500ms 만 대기해보고, 송신문제시  다음 라인 실행
        retval = setsockopt(hSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&optval, sizeof(optval));
        if(retval == SOCKET_ERROR)
            return -1;

        if(connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)
        {
            iResult = WIRELESS_RESET_CONNECT_ERR;
        }
    }

    if(iResult == WIRELESS_RESET_OK)
    {
		memset(message, 0, sizeof(message));
        Sleep(10);
        strLen = recv(hSocket, message, sizeof(message)-1, 0);
        if(strLen == -1)
        {
            iResult = WIRELESS_RESET_NO_CONN_MSG;
        }
        else
        {
            strLog.sprintf("[#1 WirelessClient Connection] Rcv : %s", message);
            Ping_LOG(strLog.c_str());
        }
    }

    if(iResult == WIRELESS_RESET_OK)
    {
        send(hSocket, FTP_Login, Login_Packetlen, 0);
        memset(message, 0, sizeof(message));
        Sleep(10);
        strLen = recv(hSocket, message, sizeof(message)-1, 0);
        if(strLen == -1)
        {
           iResult = WIRELESS_RESET_NO_PASS_MSG;
        }
        else
        {
            strLog.sprintf("[#2 WirelessClient Login] Rcv : %s", message);
            Ping_LOG(strLog.c_str());
        }
    }

    if(iResult == WIRELESS_RESET_OK)
    {
        send(hSocket, FTP_Password, Password_Packetlen, 0);
        memset(message, 0, sizeof(message));
        Sleep(10);
        strLen = recv(hSocket, message, sizeof(message)-1, 0);
		if(strLen == -1)
        {
            iResult = WIRELESS_RESET_NO_PASS_OK_MSG;
        }
        else
        {
            strLog.sprintf("[#3 WirelessClient Password] Rcv : %s", message);
            Ping_LOG(strLog.c_str());
        }
    }

    if(iResult == WIRELESS_RESET_OK)
    {
        send(hSocket, FTP_Rst, sizeof(FTP_Rst)-1, 0);
        memset(message, 0, sizeof(message));
        Sleep(10);
        strLen = recv(hSocket, message, sizeof(message)-1, 0);
        if(strLen == -1)
        {
            iResult = WIRELESS_RESET_NO_RESET_OK_MSG;
        }
        else
        {
            strLog.sprintf("[#4 WirelessClient Reset] Rcv : %s", message);
            Ping_LOG(strLog.c_str());
        }
    }

    if(iResult != WIRELESS_RESET_WSA_ERR)
    {
	    closesocket(hSocket);
        // WSACleanup();     UDP COM에서 초기화해 주므로 이 라인은 넣으면 안된다
    }
	return iResult;
}

/**
@brief   HandTP Command를 내부 Command Data에 저장한 후 Cmd List에 Add하는 함수
@author  ji7412.lee
@date    2013.03.13
@param   pRecvBuf : Receive Buffer
@param   IsConvertEndian : Convert 변경할 Flag
@note    HandTP 테스트 목적을 위한 function : 추후 Parsing,TLV 형태로 변경할것
*/
//PKT_OCS2OHT 포맷의 packet 생성
//CheckCMDData2TaskList()를 호출하여 TaskList에 등록
int TaskControl::MakeCmdList_HandTP(unsigned char* pRecvBuf, bool IsConvertEndian, uINPUT_BITFIELD uInput)
{
	static int nCountCmdID = 1;
    static int nCountTransCmdID = 0;
	static unsigned char run_stop_status=1; // 1: run, 0: stop
	char szGoStopNode[4] = "";
	UINT nStopNode = 0;
	char cTemp[6] = {0,};
	int nSetCmd = 0;
	int Error = NO_ERR;
	unsigned char tx_buf[16];
	unsigned int TP_CMD = 0;
	int Main_VHL_Num;
	int Main_VHL_Type;

	unsigned char TP_CMD_main = 0; //Handy TP 패킷의 CMD main
	unsigned char TP_CMD_sub = 0;  //Handy TP 패킷의 CMD sub
	unsigned char TP_VHL_type = 0;  //Handy TP 패킷의 CMD sub
	unsigned int  TP_VHL_num = 0;  //Handy TP 패킷의 CMD sub
	unsigned char TP_CMD_KeyInput = 0;  //Handy TP 패킷의 CMD sub
	unsigned int TP_Cycle_Number = 0;
	unsigned int TP_Cycle_Station = 0;
	unsigned int TP_Cycle_Node = 0;
	unsigned int TP_Cycle_PortType = 0;
    unsigned int TP_Cycle_LookdownLevel = 0;
	unsigned char HandTP_CMD_Type = 0;  // init : 0, quick command : 1, normal : 2


	uTP_CMD SndTPCmd; // TP Command
    memset(&SndTPCmd, 0x00, sizeof(uTP_CMD));

	int nRecvBufLength = sizeof(pRecvBuf);
	if(nRecvBufLength < HANDTP_DATA_MIN_LENGTH) return HANDTP_ERR_RECV_DATA_ERROR;

	CMD_DATA *tmpCmdData = new CMD_DATA;

	// 1.초기화
	memset(tmpCmdData, 0x00, sizeof(CMD_DATA));
	memset(szGoStopNode, 0x00, sizeof(szGoStopNode));

	TP_CMD_main = pRecvBuf[0];
	TP_CMD_sub = pRecvBuf[1];
	TP_VHL_type = pRecvBuf[2];
	TP_VHL_num  = pRecvBuf[3]<<8 | pRecvBuf[4];
	TP_CMD_KeyInput = pRecvBuf[5];

    if(m_defualtparam == NULL)
    	m_defualtparam = OHTMainForm->GetDefaultParam();

	#if ((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S))
		//memmove(cTemp,&m_defualtparam->VHLName[2],4);//V30001
		memmove(cTemp,&m_defualtparam->VHLName[3],3);//OHT001
		Main_VHL_Num = atoi(cTemp);
		Main_VHL_Type = OHT_FOUP;

	#elif  ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
		memmove(cTemp,&m_defualtparam->VHLName[4],2); // ROHT01
		Main_VHL_Num = atoi(cTemp);
		Main_VHL_Type = OHT_RETICLE;
	#endif


	// 2.Header 자체 생성
	CMD_BIG_HEADER RtnHeader;
	//RtnHeader = GetBIGHeader(pRecvBuf, IsConvertEndian);

	if( TP_CMD_main==REMOTE_TYPE_IR )
	{
		ADD_HANDTP_LOG("[HandyTP]/IR_RX/%d/%d/%d/%d",pRecvBuf[0], pRecvBuf[1], pRecvBuf[2], pRecvBuf[3]);
        if((pRecvBuf[1]==IR_ESTOP_1st) && (pRecvBuf[2]==IR_ESTOP_2nd))
        {
            // Auto Mode 상태에서만 OCS Pause/Resume 가능
    		if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL)
            {
                return Error;
            }

			if(run_stop_status)
			{
				m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_PAUSE_OCS);
			}
			else
			{
				m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_RESUME_OCS);
			}
			run_stop_status = (run_stop_status+1) & 0x01;

/*
             m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_STOP);
             m_pOHTMainStatus->QuickCmdEStopCommander = ESTOP_BY_TP;
             run_stop_status = 1;
*/
        }
		else if((pRecvBuf[1]==IR_VHL_LINK_INFO_REQ) && (pRecvBuf[2]==IR_VHL_LINK_INFO_REQ))
		{
			TP_CMD=TPCMD_OHT_LINK_INFO_REQ;
			tx_buf[0] = REMOTE_TYPE_IR;
			tx_buf[1] = IR_VHL_LINK_INFO_ACK;
// 2016-12-26, Add by Labeler
// OHT Type 별 처리 구분
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S))
			tx_buf[2] = OHT_FOUP;//VHL Type
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
			tx_buf[2] = OHT_RETICLE;//VHL Type
#else
			tx_buf[2] = OHT_FOUP;//VHL Type
#endif
			tx_buf[3] = (Main_VHL_Num>>8) & 0xFF;//VHL Number : High
			tx_buf[4] = Main_VHL_Num & 0xFF;//VHL Number : low
			tx_buf[5] = 0;
			tx_buf[6] = 0;
			tx_buf[7] = 0;
			tx_buf[8] = 0;
			tx_buf[9] = 0;
			tx_buf[10] = 0;
			tx_buf[11] = 0;
			tx_buf[12] = 0;
			tx_buf[13] = 0;
			tx_buf[14] = 0;
			tx_buf[15] = 0;
			OHTMainForm->GetHWSetRemocon()->SendTriger(tx_buf, HANDY_TP_PACKET_SIZE);
			ADD_HANDTP_LOG("[HandyTP]/C1_TX/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d type : %d, num : %d",
					tx_buf[0], tx_buf[1], tx_buf[2], tx_buf[3],
					tx_buf[4], tx_buf[5], tx_buf[6], tx_buf[7],
					tx_buf[8], tx_buf[9], tx_buf[10], tx_buf[11],
					tx_buf[12], tx_buf[13], tx_buf[14], tx_buf[15], OHT_FOUP, Main_VHL_Num );
		}
		else
		{
		}
		return Error;
	}
	else if( TP_CMD_main==REMOTE_TYPE_RF )
	{
		ADD_HANDTP_LOG("[HandyTP]/RF_RX/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",
				pRecvBuf[0], pRecvBuf[1], pRecvBuf[2], pRecvBuf[3],
				pRecvBuf[4], pRecvBuf[5], pRecvBuf[6], pRecvBuf[7],
				pRecvBuf[8], pRecvBuf[9], pRecvBuf[10], pRecvBuf[11],
				pRecvBuf[12], pRecvBuf[13], pRecvBuf[14], pRecvBuf[15]);


		if((TP_VHL_type == Main_VHL_Type) && (TP_VHL_num == Main_VHL_Num))
		{
			//NOP
		}
		else
		{
			return HANDTP_ERR_VHL_INFO_ERROR;
		}

		//CMD_TP_COMMON 명령을 8.0 packet으로 변환
		PKT_OCS2OHT SEND_PACKET;
		PKT_ACK_OHT2OCS pAckPkt;
		PKT_OHT2TP_RESPONSE AckPktPC;
		int QuickCmd = 0;
		int iPacketSize = sizeof(PKT_OCS2OHT);
		memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

		// header(NoteTP, HandyTP 공통)
		SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
		memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);

		//MANUAL_KEY
		switch(TP_CMD_sub)
		{
		case REQ_OHT_STATUS:	//현재 VHL상태를 보내 준다
			int status_error;
			int status_VHL_NUM;
			int status_node;
			int status_station;

            MakeVHLStatusForHandyTP(tx_buf, pRecvBuf[6]);

			OHTMainForm->GetHWSetRemocon()->SendTriger(tx_buf, HANDY_TP_PACKET_SIZE);
			break;

		case REQ_OHT_IO_INFO:
            MakeVHLIOInfoForHandyTP(tx_buf, pRecvBuf[6]);

			OHTMainForm->GetHWSetRemocon()->SendTriger(tx_buf, HANDY_TP_PACKET_SIZE);
			break;

		case REQ_OHT_STATUS_BG:
            MakeVHLStatusBGForHandyTP(tx_buf, pRecvBuf[6]);

			OHTMainForm->GetHWSetRemocon()->SendTriger(tx_buf, HANDY_TP_PACKET_SIZE);
			break;

		case CYCLE_CMD:	//CYCLE 변호에 해당하는 동작 수행
			TP_Cycle_Number = pRecvBuf[5]<<8 | pRecvBuf[6];
			TP_Cycle_Station = pRecvBuf[7]<<16 | pRecvBuf[8]<<8 | pRecvBuf[9];
			TP_Cycle_Node    = pRecvBuf[10]<<16 | pRecvBuf[11]<<8 | pRecvBuf[12];
            TP_Cycle_PortType= pRecvBuf[13];
            TP_Cycle_LookdownLevel = pRecvBuf[14];

            // 2017-01-15, Add by Labeler, Cycle 에 대한 Interlock Check 추가
            Error = CheckInterlock4Cycle(pRecvBuf);
            MakeAckDataForHandTp(tx_buf, CYCLE_CMD, Error);
            if(Error != NO_ERR)
            {
                ADD_HANDTP_LOG("[Hand TP Cycle Cmd Interlock] Interlock Num : %d", Error);
                    return Error;
            }

			//Cycle
			switch(TP_Cycle_Number)
			{
				case CYC_MOVE:
				case CYC_MOVE_UNLOAD:
				case CYC_MOVE_LOAD:
				case CYC_MTL_OUT:
				case CYC_MAINT_DRIVING:
				case CYC_NOMOVE_UNLOAD:
				case CYC_NOMOVE_LOAD:
				case CYC_AUTO_MODE_CHANGE:
				case CYC_MOVE_HOIST_TEACHING:
				case CYC_MOVE_ROTATE_TEACHING:	
				case CYC_MOVE_SHIFT_TEACHING:
				case CYC_TRANS_LONGRUN_START:
				case CYC_TRANS_LONGRUN_STOP:
				case CYC_HAND_LONGRUN_START:
				case CYC_HAND_LONGRUN_STOP:
				case CYC_SHUTTER_LONGRUN_START:
				case CYC_SHUTTER_LONGRUN_STOP:
				case CYC_STEER_LONGRUN_START:
				case CYC_STEER_LONGRUN_STOP:

				if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
				{
					Error = ERR_MAPDATA_LOAD_FAIL;
					return Error;
				}
				break;
				default : 
				break;
			}

			switch(TP_Cycle_Number)
			{
			case CYC_CLEAR:
				break;
			case CYC_MOVE:
            {
                int nMovePacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
                PKT_TP2OHT_MOVE_BY_MAP	MOVE_PACKET;
                memset(&MOVE_PACKET, 0, nMovePacketSize);		// 패킷 메모리 클리어

                MOVE_PACKET.HEADER.MachineType = MACHINETYPE_HANDYTP;
                MOVE_PACKET.BODY.StartNodeID = m_pExecuteInfo->ExecutePositionInfo.CurrNode;

				if(TP_Cycle_Station != 0)
                {
                    MOVE_PACKET.BODY.IsGoStation = true;
                    MOVE_PACKET.BODY.StopNodeID = TP_Cycle_Station;
					MOVE_PACKET.BODY.PortType = TP_Cycle_PortType;
                }
                else
                {
                    MOVE_PACKET.BODY.StopNodeID = TP_Cycle_Node;
				}
				MOVE_PACKET.BODY.IsMapMaker = 0x01;
				MOVE_PACKET.BODY.IsSteerCheck = 0x01;
           		memcpy(&SEND_PACKET, &MOVE_PACKET, nMovePacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TP_MOVE_BY_MAP;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

            // 2016-12-20, Add by Labeler
            // Hand TP 용 Unload 기능 추가
            case CYC_MOVE_UNLOAD:
            {
                int nMovePacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
                PKT_TP2OHT_MOVE_BY_MAP	MOVE_PACKET;
                memset(&MOVE_PACKET, 0, nMovePacketSize);		// 패킷 메모리 클리어

                MOVE_PACKET.HEADER.MachineType = MACHINETYPE_HANDYTP;
                MOVE_PACKET.BODY.StartNodeID = m_pExecuteInfo->ExecutePositionInfo.CurrNode;

                if(TP_Cycle_Station != 0)
                {
                    MOVE_PACKET.BODY.IsGoStation = true;
                    MOVE_PACKET.BODY.StopNodeID = TP_Cycle_Station;
                    MOVE_PACKET.BODY.PortType = TP_Cycle_PortType;
                }
                else
				{
                    Error = HANDTP_ERR_NOT_INPUT_STATION_ID;
                    return Error;
                }

           		memcpy(&SEND_PACKET, &MOVE_PACKET, nMovePacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TP_MOVE_UNLOAD;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_MOVE_LOAD:
            {
                int nMovePacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
                PKT_TP2OHT_MOVE_BY_MAP	MOVE_PACKET;
                memset(&MOVE_PACKET, 0, nMovePacketSize);		// 패킷 메모리 클리어

                MOVE_PACKET.HEADER.MachineType = MACHINETYPE_HANDYTP;
                MOVE_PACKET.BODY.StartNodeID = m_pExecuteInfo->ExecutePositionInfo.CurrNode;

                if(TP_Cycle_Station != 0)
                {
                    MOVE_PACKET.BODY.IsGoStation = true;
                    MOVE_PACKET.BODY.StopNodeID = TP_Cycle_Station;
                    MOVE_PACKET.BODY.PortType = TP_Cycle_PortType;
                }
                else
                {
                    Error = HANDTP_ERR_NOT_INPUT_STATION_ID;
                    return Error;
                }

           		memcpy(&SEND_PACKET, &MOVE_PACKET, nMovePacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TP_MOVE_LOAD;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
			}
            break;

			case CYC_NOMOVE_UNLOAD:
            {
                int transPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
				STATION_DATA *pStationData;
				TEACHING_DATA *pTeachingData;

                PKT_TP2OHT_TRANS_LOAD_UNLOAD	TRANS_PACKET;
                memset(&TRANS_PACKET, 0, transPacketSize);		// 패킷 메모리 클리어

				pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(TP_Cycle_Station, TP_Cycle_PortType);

				if (pTeachingData == NULL)
				{
					Error = ERR_DATA_TEACHING_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] Station Id: %d, PortType : %d", TP_Cycle_Station, TP_Cycle_PortType);
					break;
				}
				else
				{
                    TRANS_PACKET.BODY.CurStationID = TP_Cycle_Station;
                    TRANS_PACKET.BODY.CurNodeID = TP_Cycle_Node;
                    TRANS_PACKET.BODY.PortType = TP_Cycle_PortType;
					TRANS_PACKET.BODY.HoistSpeedLevel = 100;
					TRANS_PACKET.BODY.ShiftSpeedLevel = 100;
					TRANS_PACKET.BODY.TransOption.Info.AutoRecovery = false;
					TRANS_PACKET.BODY.TransOption.Info.HandDetectEQ = false;
					TRANS_PACKET.BODY.TransOption.Info.EQPIODisable_TP = true;
				}

        		memcpy(&SEND_PACKET, &TRANS_PACKET, transPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TRANS_UNLOAD;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
			}
			break;

            // 2016-12-20, Add by Labeler
            // Hand TP 용 Load 기능 추가
			case CYC_NOMOVE_LOAD:
            {
                int transPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
				STATION_DATA *pStationData;
				TEACHING_DATA *pTeachingData;

                PKT_TP2OHT_TRANS_LOAD_UNLOAD	TRANS_PACKET;
                memset(&TRANS_PACKET, 0, transPacketSize);		// 패킷 메모리 클리어

				pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(TP_Cycle_Station, TP_Cycle_PortType);

				if (pTeachingData == NULL)
				{
					Error = ERR_DATA_TEACHING_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] Station Id: %d, PortType : %d", TP_Cycle_Station, TP_Cycle_PortType);
					break;
				}
				else
				{
                    TRANS_PACKET.BODY.CurStationID = TP_Cycle_Station;
                    TRANS_PACKET.BODY.CurNodeID = TP_Cycle_Node;
                    TRANS_PACKET.BODY.PortType = TP_Cycle_PortType;
					TRANS_PACKET.BODY.HoistSpeedLevel = 100;
					TRANS_PACKET.BODY.ShiftSpeedLevel = 100;
					TRANS_PACKET.BODY.TransOption.Info.AutoRecovery = false;
					TRANS_PACKET.BODY.TransOption.Info.HandDetectEQ = false;
					TRANS_PACKET.BODY.TransOption.Info.EQPIODisable_TP = true;
				}

        		memcpy(&SEND_PACKET, &TRANS_PACKET, transPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TRANS_LOAD;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
			}
			break;

            case CYC_MOVE_ALL_HOME:
            {
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TP_MOVE_ALL_HOME;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
			break;

			case CYC_MOVE_AUTO_HOISTHOME:
            {
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TP_AUTO_HOISTHOME;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

            case CYC_AUTO_MODE_CHANGE:
            {
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TP_AUTOMODE_CHANGE;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

            // 2016-12-20, Add by Labeler
            // Hand TP 용 Move Teaching Point 기능 추가
            case CYC_MOVE_TEACHING_POSITION:
            {
                int transPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
				STATION_DATA *pStationData;
				TEACHING_DATA *pTeachingData;

                PKT_TP2OHT_TRANS_LOAD_UNLOAD	TRANS_PACKET;
                memset(&TRANS_PACKET, 0, transPacketSize);		// 패킷 메모리 클리어

                // 임시로 현재 위치한 Stattion 값을 받아오도록 처리
                // 추후 Handy TP에서 받아오도록 변경해야 함

                TRANS_PACKET.BODY.CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;
				pStationData = OHTMainForm->GetStationDataManager()->Find(TRANS_PACKET.BODY.CurStationID);

				if(pStationData == NULL)
				{
					Error = ERR_DATA_STATION_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", TRANS_PACKET.BODY.CurStationID);
					break;
				}
				else
				{
					TRANS_PACKET.BODY.CurNodeID = pStationData->ParentNodeID;
                    TRANS_PACKET.BODY.PortType = TP_Cycle_PortType;
//					TRANS_PACKET.BODY.PortType = pStationData->StationType;
				}

				pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(TRANS_PACKET.BODY.CurStationID, TRANS_PACKET.BODY.PortType);

				if (pTeachingData == NULL)
				{
					Error = ERR_DATA_TEACHING_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] Station Id: %d, PortType : %d", TRANS_PACKET.BODY.CurStationID, TRANS_PACKET.BODY.PortType);
					break;
				}
				else
				{
					TRANS_PACKET.BODY.HoistSpeedLevel = 100;
					TRANS_PACKET.BODY.ShiftSpeedLevel = 100;
					TRANS_PACKET.BODY.TransOption.Info.AutoRecovery = false;
					TRANS_PACKET.BODY.TransOption.Info.HandDetectEQ = false;
					TRANS_PACKET.BODY.TransOption.Info.EQPIODisable_TP = true;
				}

				memcpy(&SEND_PACKET, &TRANS_PACKET, transPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TRANS_MOVE_TEACHING_POINT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
			break;

            case CYC_GET_LOOKDOWN_LEVEL:
            {
                int nCurStationID = 0;
                int nCurPortType = 0;
                int nCurLookDownLevel = 0;

				STATION_DATA *pStationData;
				TEACHING_DATA *pTeachingData;

                pStationData = OHTMainForm->GetStationDataManager()->Find(TP_Cycle_Station);
				if(pStationData == NULL)
				{
					Error = ERR_DATA_STATION_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", TP_Cycle_Station);
					break;
				}

                pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(TP_Cycle_Station, TP_Cycle_PortType);

				if (pTeachingData == NULL)
				{
					Error = ERR_DATA_TEACHING_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] StationID : %d, PortType : %d", nCurStationID, nCurPortType);
					break;
				}
				else
				{
                    tx_buf[0] = REMOTE_TYPE_RF;
                    tx_buf[1] = CYCLE_CMD;
	            	tx_buf[2] = 2;
					tx_buf[3] = (TP_Cycle_Number>>8) & 0xFF;//Cycle Number : High
                    tx_buf[4] = TP_Cycle_Number & 0xFF;//Cycle Number : low
                    tx_buf[5] = (TP_Cycle_Node>>16) & 0xFF;//g_status_node
                    tx_buf[6] = (TP_Cycle_Node>>8) & 0xFF;//g_status_node
                    tx_buf[7] = TP_Cycle_Node & 0xFF;//g_status_node
                    tx_buf[8] = (TP_Cycle_Station>>16) & 0xFF;//g_status_station
                    tx_buf[9] = (TP_Cycle_Station>>8) & 0xFF;//g_status_station
                    tx_buf[10] = TP_Cycle_Station & 0xFF;//g_status_station
                    tx_buf[11] = (status_error>>8) & 0xFF;
                    tx_buf[12] = status_error & 0xFF;
                    tx_buf[13] = 13;
                    tx_buf[14] = pTeachingData->LookDownLevel;
                    tx_buf[15] = 0;
                    OHTMainForm->GetHWSetRemocon()->SendTriger(tx_buf, HANDY_TP_PACKET_SIZE);
				}

                // Handy TP로 LookDown 정보 송신
                // Station ID, Port Type 정보도 같이 보낼 지 협의 필요

            }
            break;

            case CYC_SET_LOOKDOWN_LEVEL:
            {
                int teachingPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
				TEACHING_DATA *pTeachingData;

                PKT_OHT2TP_TEACHING_INFO	TEACHING_PACKET;
                memset(&TEACHING_PACKET, 0, teachingPacketSize);		// 패킷 메모리 클리어

				pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(TP_Cycle_Station, TP_Cycle_PortType);
				if (pTeachingData == NULL)
				{
					Error = ERR_DATA_TEACHING_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] StationID : %d, PortType : %d", TP_Cycle_Station, TP_Cycle_PortType);
					break;
				}
				else
				{
					TEACHING_PACKET.BODY.PortType = TP_Cycle_PortType;
                    // Teaching Save와 동일하나, LookDwonLevel만 Handy TP에서 보내주는 값 사용 토록 처리 필요
//                    TEACHING_PACKET.BODY.EtcData.Data.LookDownLevel = pTeachingData->LookDownLevel;
				}

        		memcpy(&SEND_PACKET, &TEACHING_PACKET, teachingPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TEACHING_LOOKDOWN_LEVEL_SET;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_MTL_OUT:
            {
				TP_CMD = TagID::CMD_TP_OHT_TAKEOUT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;
			case CYC_HAND_HP:
            break;
			case CYC_MAINT_DRIVING:
            break;

			case CYC_TEACHING_SAVE:
            {
                int teachingPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
				STATION_DATA *pStationData;
				TEACHING_DATA *pTeachingData;
                UINT CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;

                PKT_OHT2TP_TEACHING_INFO	TEACHING_PACKET;
                memset(&TEACHING_PACKET, 0, teachingPacketSize);		// 패킷 메모리 클리어

                // 임시로 현재 위치한 Stattion 값을 받아오도록 처리
				// 추후 Handy TP에서 받아오도록 변경해야 함
				pStationData = OHTMainForm->GetStationDataManager()->Find(CurStationID);

				if(pStationData == NULL)
				{
					Error = ERR_DATA_STATION_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", CurStationID);
					break;
				}
				else
				{
                    TEACHING_PACKET.BODY.PortType = TP_Cycle_PortType;
//					TEACHING_PACKET.BODY.PortType = pStationData->StationType;
				}

				pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(CurStationID, TEACHING_PACKET.BODY.PortType);

				if (pTeachingData == NULL)
				{
					Error = ERR_DATA_TEACHING_NOEXIST;
                    ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] StationID : %d, PortType : %d", CurStationID, TEACHING_PACKET.BODY.PortType);
					break;
				}
				else
				{
                    // Hand TP에서 Lookdown Level을 입력하지 않은 경우에는 기존 설정된 Lookdown Level 그대로 사용
                    if(TP_Cycle_LookdownLevel == 0)
                    {
                        TP_Cycle_LookdownLevel = pTeachingData->LookDownLevel;
                    }

					TEACHING_PACKET.BODY.EtcData.Data.PIODirection = pTeachingData->PIODirection;
                    TEACHING_PACKET.BODY.EtcData.Data.PIOTimeLevel = pTeachingData->PIOTimeLevel;
                    TEACHING_PACKET.BODY.EtcData.Data.LookDownLevel = TP_Cycle_LookdownLevel;
                    TEACHING_PACKET.BODY.EtcData.Data.Reserved2 = pTeachingData->TransOption.Info.Reserved2;
                    TEACHING_PACKET.BODY.EtcData.Data.Reserved1 = pTeachingData->TransOption.Info.Reserved1;
                    TEACHING_PACKET.BODY.EtcData.Data.EQPIODisable_TP = pTeachingData->TransOption.Info.EQPIODisable_TP;
                    TEACHING_PACKET.BODY.EtcData.Data.HandDetectEQ = pTeachingData->TransOption.Info.HandDetectEQ;
                    TEACHING_PACKET.BODY.EtcData.Data.Oscillation = pTeachingData->TransOption.Info.Oscillation;
                    TEACHING_PACKET.BODY.EtcData.Data.TransSound = pTeachingData->TransOption.Info.TransSound;
                    TEACHING_PACKET.BODY.EtcData.Data.RFReader = pTeachingData->TransOption.Info.RFReader;
                    TEACHING_PACKET.BODY.EtcData.Data.AutoRecovery = pTeachingData->TransOption.Info.AutoRecovery;
                    TEACHING_PACKET.BODY.PIOID = pTeachingData->PIOID;
                    TEACHING_PACKET.BODY.PIOType = pTeachingData->PIOType;
                    TEACHING_PACKET.BODY.PIOChannel = pTeachingData->PIOChannel;
				}

        		memcpy(&SEND_PACKET, &TEACHING_PACKET, teachingPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TEACHING_INFO_SAVE;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
			break;

            // 2016-12-25, Add by Labeler
            // 단일 Axis Move T 기능 추가
			case CYC_MOVE_HOIST_TEACHING:
            {
                int teachingPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
				STATION_DATA *pStationData;
				TEACHING_DATA *pTeachingData;
                UINT CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;

                PKT_OHT2TP_TEACHING_INFO	TEACHING_PACKET;
                memset(&TEACHING_PACKET, 0, teachingPacketSize);		// 패킷 메모리 클리어

                // 임시로 현재 위치한 Stattion 값을 받아오도록 처리
                // 추후 Handy TP에서 받아오도록 변경해야 함
				pStationData = OHTMainForm->GetStationDataManager()->Find(CurStationID);

				if(pStationData == NULL)
				{
					Error = ERR_DATA_STATION_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", CurStationID);
					break;
				}
				else
				{
                    TEACHING_PACKET.BODY.PortType = TP_Cycle_PortType;
//					TEACHING_PACKET.BODY.PortType = pStationData->StationType;
				}

				pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(CurStationID, TEACHING_PACKET.BODY.PortType);

				if (pTeachingData == NULL)
				{
					Error = ERR_DATA_TEACHING_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] %d", CurStationID);
					break;
				}
				else
				{
                    tmpCmdData->uTP_CMD.CmdTP_Jog_TR.Distance = pTeachingData->Hoist;
				}

				TP_CMD = TPCMD_JOG_HOIST_ABS;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
			}
            break;

			case CYC_MOVE_ROTATE_TEACHING:
            {
                int teachingPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
				STATION_DATA *pStationData;
				TEACHING_DATA *pTeachingData;
                UINT CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;

                PKT_OHT2TP_TEACHING_INFO	TEACHING_PACKET;
                memset(&TEACHING_PACKET, 0, teachingPacketSize);		// 패킷 메모리 클리어

                // 임시로 현재 위치한 Stattion 값을 받아오도록 처리
                // 추후 Handy TP에서 받아오도록 변경해야 함
				pStationData = OHTMainForm->GetStationDataManager()->Find(CurStationID);

				if(pStationData == NULL)
				{
					Error = ERR_DATA_STATION_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", CurStationID);
					break;
				}
				else
				{
                    TEACHING_PACKET.BODY.PortType = TP_Cycle_PortType;
//					TEACHING_PACKET.BODY.PortType = pStationData->StationType;
				}

				pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(CurStationID, TEACHING_PACKET.BODY.PortType);

				if (pTeachingData == NULL)
				{
					Error = ERR_DATA_TEACHING_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] %d", CurStationID);
					break;
				}
				else
				{
                    tmpCmdData->uTP_CMD.CmdTP_Jog_TR.Distance = pTeachingData->Rotate;
				}

				TP_CMD = TPCMD_JOG_ROTATE_ABS;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
			}
            break;

			case CYC_MOVE_SHIFT_TEACHING:
            {
                int teachingPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
				STATION_DATA *pStationData;
				TEACHING_DATA *pTeachingData;
                UINT CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;

                PKT_OHT2TP_TEACHING_INFO	TEACHING_PACKET;
                memset(&TEACHING_PACKET, 0, teachingPacketSize);		// 패킷 메모리 클리어

                // 임시로 현재 위치한 Stattion 값을 받아오도록 처리
                // 추후 Handy TP에서 받아오도록 변경해야 함
				pStationData = OHTMainForm->GetStationDataManager()->Find(CurStationID);

				if(pStationData == NULL)
				{
					Error = ERR_DATA_STATION_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", CurStationID);
					break;
				}
				else
				{
                    TEACHING_PACKET.BODY.PortType = TP_Cycle_PortType;
//					TEACHING_PACKET.BODY.PortType = pStationData->StationType;
				}

				pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(CurStationID, TEACHING_PACKET.BODY.PortType);

				if (pTeachingData == NULL)
				{
					Error = ERR_DATA_TEACHING_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] %d", CurStationID);
					break;
				}
				else
				{
                    tmpCmdData->uTP_CMD.CmdTP_Jog_TR.Distance = pTeachingData->Shift;
				}

				TP_CMD = TPCMD_JOG_SHIFT_ABS;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
            }
            break;

            // 2016-12-27, Add by Labeler
            // Handy TP에서, 데이터 전송 요구 요청 기능 구현
            // Handy TP에서는 Map 만 요청 가능
			case CYC_MAPSET_DOWN_TEACHING:
            {
                PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS REQ_DATA;
                int reqDataSize = sizeof(PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS);
                memset(&REQ_DATA, 0, reqDataSize);		// 패킷 메모리 클리어

                SEND_PACKET.TAG.Value = TagID::REQ_DATATRANSFER_OHT_TP;

                REQ_DATA.Request.CMDType = FTP_CMDTYPE_MAPSET_TEACHING;
                REQ_DATA.Request.TestMode = FTP_TESTMODE_FTP2OHT;

           		memcpy(&(SEND_PACKET.Value), &REQ_DATA, reqDataSize);
                CheckCMDData2TaskList(&SEND_PACKET, &pAckPkt, &AckPktPC, QuickCmd);
                return Error;
            }
			break;

            case CYC_MAPSET_DOWN_OPERATION:
            {
                PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS REQ_DATA;
                int reqDataSize = sizeof(PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS);
                memset(&REQ_DATA, 0, reqDataSize);		// 패킷 메모리 클리어

                SEND_PACKET.TAG.Value = TagID::REQ_DATATRANSFER_OHT_TP;

                REQ_DATA.Request.CMDType = FTP_CMDTYPE_MAPSET_OPERATING;
                REQ_DATA.Request.TestMode = FTP_TESTMODE_FTP2OHT;

           		memcpy(&(SEND_PACKET.Value), &REQ_DATA, reqDataSize);
                CheckCMDData2TaskList(&SEND_PACKET, &pAckPkt, &AckPktPC, QuickCmd);
                return Error;
            }
			break;

			case CYC_MAPSET_UPLOAD:
            {
                PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS REQ_DATA;
                int reqDataSize = sizeof(PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS);
                memset(&REQ_DATA, 0, reqDataSize);		// 패킷 메모리 클리어

                SEND_PACKET.TAG.Value = TagID::REQ_DATATRANSFER_OHT_TP;

                REQ_DATA.Request.CMDType = FTP_CMDTYPE_MAPSET;
                REQ_DATA.Request.TestMode = FTP_TESTMODE_OHT2FTP;

           		memcpy(&(SEND_PACKET.Value), &REQ_DATA, reqDataSize);
                CheckCMDData2TaskList(&SEND_PACKET, &pAckPkt, &AckPktPC, QuickCmd);

//				TP_CMD = TagID::REQ_DATATRANSFER_OHT_TP;
//				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
                return Error;

            }
			break;

            // 2017-01-21, Add by Labeler
            // Map Make의 경우, Target Node가 Driving Tag 일때만 가능
			case CYC_MAP_MAKE_NOFILE:
            {
                int nMovePacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
                PKT_TP2OHT_MOVE_BY_MAP	MOVE_PACKET;
                memset(&MOVE_PACKET, 0, nMovePacketSize);		// 패킷 메모리 클리어

                MOVE_PACKET.HEADER.MachineType = MACHINETYPE_HANDYTP;
                MOVE_PACKET.BODY.StartNodeID = m_pExecuteInfo->ExecutePositionInfo.CurrNode;
                MOVE_PACKET.BODY.StopNodeID = TP_Cycle_Node;
                // Port Type이 100 이상일 경우에는 MapMake 명령
                MOVE_PACKET.BODY.PortType = 0x64;
                MOVE_PACKET.BODY.IsMapMaker = 0x01;
				MOVE_PACKET.BODY.IsSteerCheck = 0x01;
           		memcpy(&SEND_PACKET, &MOVE_PACKET, nMovePacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TP_MOVE_BY_MAP;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

            case CYC_MAP_MAKE_FILE:
				TP_CMD = TagID::CMD_TP_MAPMAKER_FILE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            break;

            case CYC_AUTO_TEACHING_NOFILE:
            {
                int teachingPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
				STATION_DATA *pStationData;
                UINT CurStationID = m_pExecuteInfo->ExecutePositionInfo.CurrStation;

                PKT_OHT2TP_TEACHING_INFO	TEACHING_PACKET;
                memset(&TEACHING_PACKET, 0, teachingPacketSize);		// 패킷 메모리 클리어

                // 임시로 현재 위치한 Stattion 값을 받아오도록 처리
                // 추후 Handy TP에서 받아오도록 변경해야 함
				pStationData = OHTMainForm->GetStationDataManager()->Find(CurStationID);

				if(pStationData == NULL)
				{
					Error = ERR_DATA_STATION_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", CurStationID);
					break;
				}
				else
				{
                    TEACHING_PACKET.BODY.PortType = TP_Cycle_PortType;
				}

        		memcpy(&SEND_PACKET, &TEACHING_PACKET, teachingPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_TP_AUTOTEACHING;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

            case CYC_AUTO_TEACHING_FILE:
                TP_CMD = TagID::CMD_TP_AUTOTEACHING_FILE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            break;

            case CYC_AUTO_TEACHING_DRV_OFFSET:
            {
				STATION_DATA *pStationData;
				TEACHING_DATA *pTeachingData;

                pStationData = OHTMainForm->GetStationDataManager()->Find(TP_Cycle_Station);
				if(pStationData == NULL)
				{
					Error = ERR_DATA_STATION_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] %d", TP_Cycle_Station);
					break;
				}

                pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(TP_Cycle_Station, TP_Cycle_PortType);
				if (pTeachingData == NULL)
				{
					Error = ERR_DATA_TEACHING_NOEXIST;
					ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] StationID : %d, PortType : %d", TP_Cycle_Station, TP_Cycle_PortType);
					break;
				}
				else
				{
                    tx_buf[0] = REMOTE_TYPE_RF;
                    tx_buf[1] = CYCLE_CMD;
	            	tx_buf[2] = 0;
                    tx_buf[3] = (TP_Cycle_Number>>8) & 0xFF;//Cycle Number : High
                    tx_buf[4] = TP_Cycle_Number & 0xFF;//Cycle Number : low
                    tx_buf[5] = (TP_Cycle_Node>>16) & 0xFF;//g_status_node
                    tx_buf[6] = (TP_Cycle_Node>>8) & 0xFF;//g_status_node
                    tx_buf[7] = TP_Cycle_Node & 0xFF;//g_status_node
                    tx_buf[8] = (TP_Cycle_Station>>16) & 0xFF;//g_status_station
                    tx_buf[9] = (TP_Cycle_Station>>8) & 0xFF;//g_status_station
                    tx_buf[10] = TP_Cycle_Station & 0xFF;//g_status_station
                    tx_buf[11] = (status_error>>8) & 0xFF;
                    tx_buf[12] = status_error & 0xFF;
                    tx_buf[13] = 13;
                    tx_buf[14] = Load_AutoTeaching_DrvingData(TP_Cycle_Station, TP_Cycle_PortType);
                    tx_buf[15] = 0;
                    OHTMainForm->GetHWSetRemocon()->SendTriger(tx_buf, HANDY_TP_PACKET_SIZE);
				}
            }
            break;

			case CYC_DRIVING_F_SERVO_OFF:
				TP_CMD = TPCMD_DRIVING_F_SERVO_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_DRIVING_F_SERVO_ON:
				TP_CMD = TPCMD_DRIVING_F_SERVO_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_DRIVING_R_SERVO_OFF:
				TP_CMD = TPCMD_DRIVING_R_SERVO_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_DRIVING_R_SERVO_ON:
				TP_CMD = TPCMD_DRIVING_R_SERVO_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RAILSUPPORT_HOME:
				TP_CMD = TPCMD_RAILSUPPORT_HOME;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RAILSUPPORT_WORK:
				TP_CMD = TPCMD_RAILSUPPORT_WORK;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_SHUTTER_ORIGIN:
				TP_CMD = TPCMD_SHUTTER_ORIGIN;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_GO_MARK_FORWARD:
				TP_CMD = TPCMD_GO_MARK_FORWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_GO_TRANS_MARK_FORWARD:
				TP_CMD = TPCMD_GO_TRANS_MARK_FORWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_GO_QR_LEFT_MARK_FORWARD:
				TP_CMD = TPCMD_GO_QR_LEFT_MARK_FORWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_GO_QR_RIGHT_MARK_FORWARD:
				TP_CMD = TPCMD_GO_QR_RIGHT_MARK_FORWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_HAND_OPEN:
				TP_CMD = TPCMD_HAND_OPEN;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_HAND_CLOSE:
				TP_CMD = TPCMD_HAND_CLOSE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case CYC_HAND_HOMING:
				TP_CMD = TPCMD_HAND_HOMING;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_HAND_ENABLE:
				TP_CMD = TPCMD_HAND_ENABLE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_HAND_FREE:
				TP_CMD = TPCMD_HAND_FREE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

            // 2016-12-21, Add by Labeler
			case CYC_HAND_PIO_SELECT_OFF:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_HAND_PIO_SEL;
                IO_SET_PACKET.BODY.iOnOff = OFF;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_HAND_PIO_SELECT_ON:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_HAND_PIO_SEL;
                IO_SET_PACKET.BODY.iOnOff = ON;

           		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_HAND_PWR_OFF:
				TP_CMD = TPCMD_HAND_PWR_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_HAND_PWR_ON:
				TP_CMD = TPCMD_HAND_PWR_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_HAND_PWR_RST:
				TP_CMD = TPCMD_HAND_PWR_RST;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_HAND_RESET:
				TP_CMD = TPCMD_HAND_RESET;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_HOIST_SERVO_ON:
				TP_CMD = TPCMD_HOIST_SERVO_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_HOIST_SERVO_OFF:
				TP_CMD = TPCMD_HOIST_SERVO_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_HOIST_ORIGIN_SETTING:
				TP_CMD = TPCMD_HOIST_ORIGIN_SETTING;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_JOG_HOIST_HOME:
				TP_CMD = TPCMD_JOG_HOIST_HOME;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				//tmpCmdData->uTP_CMD.CmdTP_Jog_TR.AccelTime= 1000;       //Accel
				//tmpCmdData->uTP_CMD.CmdTP_Jog_TR.DecelTime= 1600;       //Decel
				//tmpCmdData->uTP_CMD.CmdTP_Jog_TR.Velocity= 100;			//Velocity
				//tmpCmdData->uTP_CMD.CmdTP_Jog_TR.Distance= 100;			//Velocity

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_JOG_HOIST_STOP:
				TP_CMD = TPCMD_JOG_HOIST_STOP;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_JOG_SHIFT_HOME:
				TP_CMD = TPCMD_JOG_SHIFT_HOME;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_JOG_SHIFT_STOP:
				TP_CMD = TPCMD_JOG_SHIFT_STOP;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_JOG_ROTATE_HOME:
				TP_CMD = TPCMD_JOG_ROTATE_HOME;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_JOG_ROTATE_STOP:
				TP_CMD = TPCMD_JOG_ROTATE_STOP;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case CYC_LOOKDOWN_DISABLE:
				TP_CMD = TPCMD_LOOKDOWN_DISABLE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LOOKDOWN_ENABLE:
				TP_CMD = TPCMD_LOOKDOWN_ENABLE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_SHIFT_ORIGIN_SETTING:
				TP_CMD = TPCMD_SHIFT_ORIGIN_SETTING;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_SHIFT_SERVO_OFF:
				TP_CMD = TPCMD_SHIFT_SERVO_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_SHIFT_SERVO_ON:
				TP_CMD = TPCMD_SHIFT_SERVO_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case CYC_SHUTTER_OPEN:
				TP_CMD = TPCMD_SHUTTER_OPEN;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_SHUTTER_CLOSE:
				TP_CMD = TPCMD_SHUTTER_CLOSE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_SHUTTER_ALARM_RST:
				TP_CMD = TPCMD_SHUTTER_ALARM_RST;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_SHUTTER_ENABLE:
				TP_CMD = TPCMD_SHUTTER_ENABLE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_SHUTTER_FREE:
				TP_CMD = TPCMD_SHUTTER_FREE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_STEER_F_ENABLE:
				TP_CMD = TPCMD_STEER_F_ENABLE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_STEER_F_FREE:
				TP_CMD = TPCMD_STEER_F_FREE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_STEER_LEFT:
				TP_CMD = TPCMD_STEER_LEFT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_STEER_R_ENABLE:
				TP_CMD = TPCMD_STEER_R_ENABLE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_STEER_R_FREE:
				TP_CMD = TPCMD_STEER_R_FREE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_STEER_RIGHT:
				TP_CMD = TPCMD_STEER_RIGHT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_TRACTION_STEER_LEFT:
				TP_CMD = TPCMD_TRACTION_STEER_LEFT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_TRACTION_STEER_RIGHT:
				TP_CMD = TPCMD_TRACTION_STEER_RIGHT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_SELECT_ON:
				TP_CMD = TPCMD_LEFT_PIO_SELECT_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_SELECT_OFF:
				TP_CMD = TPCMD_LEFT_PIO_SELECT_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_VALID_ON:
				TP_CMD = TPCMD_LEFT_PIO_VALID_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_VALID_OFF:
				TP_CMD = TPCMD_LEFT_PIO_VALID_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_TR_REQ_ON:
				TP_CMD = TPCMD_LEFT_PIO_TR_REQ_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_TR_REQ_OFF:
				TP_CMD = TPCMD_LEFT_PIO_TR_REQ_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_BUSY_ON:
				TP_CMD = TPCMD_LEFT_PIO_BUSY_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_BUSY_OFF:
				TP_CMD = TPCMD_LEFT_PIO_BUSY_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_COMPT_ON:
				TP_CMD = TPCMD_LEFT_PIO_COMPT_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_COMPT_OFF:
				TP_CMD = TPCMD_LEFT_PIO_COMPT_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_LEFT_PIO_ALL_OFF:
				TP_CMD = TPCMD_LEFT_PIO_ALL_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_SELECT_ON:
				TP_CMD = TPCMD_RIGHT_PIO_SELECT_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_SELECT_OFF:
				TP_CMD = TPCMD_RIGHT_PIO_SELECT_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_VALID_ON:
				TP_CMD = TPCMD_RIGHT_PIO_VALID_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_VALID_OFF:
				TP_CMD = TPCMD_RIGHT_PIO_VALID_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_TR_REQ_ON:
				TP_CMD = TPCMD_RIGHT_PIO_TR_REQ_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_TR_REQ_OFF:
				TP_CMD = TPCMD_RIGHT_PIO_TR_REQ_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_BUSY_ON:
				TP_CMD = TPCMD_RIGHT_PIO_BUSY_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_BUSY_OFF:
				TP_CMD = TPCMD_RIGHT_PIO_BUSY_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_COMPT_ON:
				TP_CMD = TPCMD_RIGHT_PIO_COMPT_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_COMPT_OFF:
				TP_CMD = TPCMD_RIGHT_PIO_COMPT_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_RIGHT_PIO_ALL_OFF:
				TP_CMD = TPCMD_RIGHT_PIO_ALL_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

// 2016-12-19, Add by Labeler
// Rotate 관련 수정
			case CYC_ROTATE_0:
			    TP_CMD = TPCMD_JOG_ROTATE_ABS;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
                tmpCmdData->uTP_CMD.CmdTP_Jog_TR.Distance = 0;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_ROTATE_90:
				TP_CMD = TPCMD_JOG_ROTATE_ABS;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
                tmpCmdData->uTP_CMD.CmdTP_Jog_TR.Distance = 90;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_ROTATE_180:
				TP_CMD = TPCMD_JOG_ROTATE_ABS;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
                tmpCmdData->uTP_CMD.CmdTP_Jog_TR.Distance = 180;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_ROTATE_270:
				TP_CMD = TPCMD_ROTATE_270;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_ROTATE_HOMING:
				TP_CMD = TPCMD_JOG_ROTATE_HOME;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_ROTATE_GO_ORIGIN:
				TP_CMD = TPCMD_ROTATE_GO_ORIGIN;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_ROTATE_RESET:
				TP_CMD = TPCMD_ROTATE_RESET;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_ROTATE_FREE:
				TP_CMD = TPCMD_ROTATE_FREE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_ROTATE_ENABLE:
				TP_CMD = TPCMD_ROTATE_ENABLE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_ROTATE_SERVO_OFF:
				TP_CMD = TPCMD_ROTATE_SERVO_OFF;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;
			case CYC_ROTATE_SERVO_ON:
				TP_CMD = TPCMD_ROTATE_SERVO_ON;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

#if((OHT_NAME == OHT_NAME_STD_V80)|| (OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
            // 2016-12-21, Add by Labeler
			case CYC_CID_LEFT_SEL:
			{
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
				int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
				memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Left_Select_ON;
                IO_SET_PACKET.BODY.iOnOff = ON;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_RIGHT_SEL:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Right_Select_ON;
                IO_SET_PACKET.BODY.iOnOff = ON;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;


			case CYC_CID_OCCUPREQ:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Occupancy;
                IO_SET_PACKET.BODY.iOnOff = ON;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_PASSCOMPT:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Pass_Completion;
                IO_SET_PACKET.BODY.iOnOff = ON;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_MANUAL:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Abnormal;
                IO_SET_PACKET.BODY.iOnOff = ON;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_OPERATION:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Operation_Mode;
                IO_SET_PACKET.BODY.iOnOff = ON;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_RESERVE_1:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Common_Tag;
                IO_SET_PACKET.BODY.iOnOff = ON;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_RESERVE_2:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Reserved_OUT2;
                IO_SET_PACKET.BODY.iOnOff = ON;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_LEFT_SEL_OFF:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Left_Select_ON;
                IO_SET_PACKET.BODY.iOnOff = OFF;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_RIGHT_SEL_OFF:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Right_Select_ON;
                IO_SET_PACKET.BODY.iOnOff = OFF;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_OCCUPREQ_OFF:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Occupancy;
                IO_SET_PACKET.BODY.iOnOff = OFF;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_PASSCOMPT_OFF:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Pass_Completion;
                IO_SET_PACKET.BODY.iOnOff = OFF;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_MANUAL_OFF:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Abnormal;
                IO_SET_PACKET.BODY.iOnOff = OFF;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_OPERATION_OFF:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Operation_Mode;
                IO_SET_PACKET.BODY.iOnOff = OFF;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_RESERVE_1_OFF:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Common_Tag;
                IO_SET_PACKET.BODY.iOnOff = OFF;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;

			case CYC_CID_RESERVE_2_OFF:
            {
                PKT_OHT2TP_IO_SET IO_SET_PACKET;
                int ioSetPacketSize = sizeof(PKT_OHT2TP_IO_SET);
                memset(&IO_SET_PACKET, 0, ioSetPacketSize);		// 패킷 메모리 클리어

                IO_SET_PACKET.BODY.iIoNum = OUTPUT::OUT_CID_Reserved_OUT2;
                IO_SET_PACKET.BODY.iOnOff = OFF;

        		memcpy(&SEND_PACKET, &IO_SET_PACKET, ioSetPacketSize);
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				TP_CMD = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
            }
            break;
#endif

			case CYC_OHT_MANUALMODE:
				TP_CMD = TagID::CMD_MANUALMODE_TP_OHT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
				break;
			case CYC_OHT_AUTOMODE:
				TP_CMD = TagID::CMD_AUTOMODE_TP_OHT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
				break;
			case CYC_OHT_FORCE_PP:
				//TP_CMD = TagID::CMD_PASSPERMITFORCEOPEN_OHT2OCS;
				TP_CMD = TagID::CMD_PASSPERMITFORCEOPEN_OHT2TP;
			
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
				break;

            // 2016-12-25, Add by Labeler
            // Longrun 관련 Cycle 추가
            // 우선은 명령 발생 여부만 Check
            // Interlock 부분은 추후 추가
            case CYC_TRANS_LONGRUN_START:
                m_nLongrunTypeHandyTP = HANDTP_LONGRUN_TRANS;
                m_nPortType_HandTPLongrun = TP_Cycle_PortType;
                return Error;

            case CYC_HAND_LONGRUN_START:
                m_nLongrunTypeHandyTP = HANDTP_LONGRUN_HAND;
                return Error;

            case CYC_SHUTTER_LONGRUN_START:
                m_nLongrunTypeHandyTP = HANDTP_LONGRUN_SHUTTER;
                return Error;

            case CYC_STEER_LONGRUN_START:
                m_nLongrunTypeHandyTP = HANDTP_LONGRUN_STEER;
                return Error;

            case CYC_TRANS_LONGRUN_STOP:
            case CYC_HAND_LONGRUN_STOP:
            case CYC_SHUTTER_LONGRUN_STOP:
            case CYC_STEER_LONGRUN_STOP:
                m_nLongrunTypeHandyTP = HANDTP_LONGRUN_NONE;
                return Error;

			case CYC_VHL_ERR_RESET:
				if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL)
				{
                    TP_CMD = TagID::CMD_SOFTRESET_TP_OHT;
                    tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

                    HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
                }
				break;
			}
			if(HandTP_CMD_Type == HANDTP_CMD_TYPE_NORMAL)
			{
				// tag
				SEND_PACKET.TAG.Value = TagID::CMD_TP_COMMON;	// TAGCATEGORY::TAG_CATEGORY_OCSCOMMON;

				// value
				memcpy(&(SEND_PACKET.Value[0]), &TP_CMD, 4);
				memcpy(&(SEND_PACKET.Value[8]), &(tmpCmdData->uTP_CMD), sizeof(uTP_CMD_));

				// CMD add to TaskList
				CheckCMDData2TaskList(&SEND_PACKET, &pAckPkt, &AckPktPC, QuickCmd);
			}
            else if(HandTP_CMD_Type == HANDTP_CMD_TYPE_EXCEPT)
            {
                SEND_PACKET.TAG.Value = TP_CMD;

				// value
				memcpy(&(SEND_PACKET.Value[0]), &TP_CMD, 4);
				memcpy(&(SEND_PACKET.Value[8]), &(tmpCmdData->uTP_CMD), sizeof(uTP_CMD_));

				// CMD add to TaskList
				CheckCMDData2TaskList(&SEND_PACKET, &pAckPkt, &AckPktPC, QuickCmd);
            }
			break;

		case GLOBAL_KEY:	// Handy TP에서 전 메뉴 공통으로 동작하는 Key 처리
			//RF Quick Command 처리
			switch(TP_CMD_KeyInput)
			{
			case 0x21://TPCMD_OHT_RESET
				if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL)
				{
                    TP_CMD = TagID::CMD_SOFTRESET_TP_OHT;
                    tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

                    SEND_PACKET.TAG.Value = TP_CMD;
    				memcpy(&(SEND_PACKET.Value[0]), &TP_CMD, 4);
                    memcpy(&(SEND_PACKET.Value[8]), &(tmpCmdData->uTP_CMD), sizeof(uTP_CMD_));

                    // CMD add to TaskList
                    CheckCMDData2TaskList(&SEND_PACKET, &pAckPkt, &AckPktPC, QuickCmd);
                }
				break;
			}
			break;

		case MANUAL_KEY:	//Handy TP에서 Manual operation 메뉴에서 조그 동작 처리
			//Manual 동작 가능 여부 확인

            Error = CheckInterlock4ManualKey(TP_CMD_KeyInput);
            MakeAckDataForHandTp(tx_buf, MANUAL_KEY, Error);

			if(Error != NO_ERR)
			{
                ADD_HANDTP_LOG("[Hand TP Manual Cmd Interlock] Interlock Num : %d", Error);
                return Error;
            }

			unsigned char KeyInputCount = pRecvBuf[7];

			switch(TP_CMD_KeyInput)
			{
			case MANUAL_KEY_JOG_SLOW_DRIVING_FW://TPCMD_JOG_DRIVING_FORWARD
				TP_CMD = TPCMD_JOG_DRIVING_FORWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Accel = 60;       //Accel
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Decel = 1600;       //Decel
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Velocity = 100;//Velocity

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_JOG_SLOW_DRIVING_BW://TPCMD_JOG_DRIVING_BACKWARD
				TP_CMD = TPCMD_JOG_DRIVING_BACKWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Accel = 60;       //Accel
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Decel = 1600;       //Decel
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Velocity = 100;//Velocity

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_JOG_MIDDLE_DRIVING_FW://TPCMD_JOG_DRIVING_FORWARD : high speed
				TP_CMD = TPCMD_JOG_DRIVING_FORWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Accel = 60;       //Accel
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Decel = 1600;       //Decel
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Velocity = 250;//Velocity

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_JOG_MIDDLE_DRIVING_BW://TPCMD_JOG_DRIVING_BACKWARD : high speed
				TP_CMD = TPCMD_JOG_DRIVING_BACKWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Accel = 60;       //Accel
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Decel = 1600;       //Decel
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.Velocity = 250;//Velocity

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_JOG_HOIST_UP://TPCMD_JOG_HOIST_UP
				TP_CMD = TPCMD_JOG_HOIST_UP;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_JOG_HOIST_DOWN://TPCMD_JOG_HOIST_DOWN
				TP_CMD = TPCMD_JOG_HOIST_DOWN;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

            case MANUAL_KEY_HOIST_HOME:
				TP_CMD = TPCMD_JOG_HOIST_HOME;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

			case MANUAL_KEY_JOG_ROTATE_CCW://TPCMD_JOG_ROTATE_CCW
				TP_CMD = TPCMD_JOG_ROTATE_CCW;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
                SetRotateJogCmd(true);
                m_dwRotateJogMoveStartTime = (DWORD)timeGetTime();
				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_JOG_ROTATE_CW://TPCMD_JOG_ROTATE_CW
				TP_CMD = TPCMD_JOG_ROTATE_CW;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
                SetRotateJogCmd(true);
                m_dwRotateJogMoveStartTime = (DWORD)timeGetTime();
				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

            case MANUAL_KEY_ROTATE_HOME:
				TP_CMD = TPCMD_JOG_ROTATE_HOME;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

            case MANUAL_KEY_ROTATE_ORIGIN:
				TP_CMD = TPCMD_ROTATE_GO_ORIGIN;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

			case MANUAL_KEY_JOG_SLIDE_LEFT://TPCMD_JOG_SHIFT_LEFT
				TP_CMD = TPCMD_JOG_SHIFT_LEFT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;
				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_JOG_SLIDE_RIGHT://TPCMD_JOG_SHIFT_RIGHT
				TP_CMD = TPCMD_JOG_SHIFT_RIGHT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

            case MANUAL_KEY_SLIDE_HOME:
				TP_CMD = TPCMD_JOG_SHIFT_HOME;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

			case MANUAL_KEY_STEER_LEFT://TPCMD_STEER_LEFT
				TP_CMD = TPCMD_STEER_LEFT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_STEER_RIGHT://TPCMD_STEER_RIGHT
				TP_CMD = TPCMD_STEER_RIGHT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_HAND_OPEN://TPCMD_HAND_OPEN
				TP_CMD = TPCMD_HAND_OPEN;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_HAND_CLOSE://TPCMD_HAND_CLOSE
				TP_CMD = TPCMD_HAND_CLOSE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

            case MANUAL_KEY_HAND_ORIGIN:
				TP_CMD = TPCMD_HAND_HOMING;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

            case MANUAL_KEY_HAND_RESET:
				TP_CMD = TPCMD_HAND_RESET;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

			case MANUAL_KEY_SHUTTER_OPEN://TPCMD_SHUTTER_OPEN
				TP_CMD = TPCMD_SHUTTER_OPEN;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_SHUTTER_CLOSE://TPCMD_SHUTTER_CLOSE
				TP_CMD = TPCMD_SHUTTER_CLOSE;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

            case MANUAL_KEY_SHUTTER_ORIGIN:
				TP_CMD = TPCMD_SHUTTER_ORIGIN;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

            case MANUAL_KEY_SHUTTER_RESET:
				TP_CMD = TPCMD_SHUTTER_ALARM_RST;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

			case MANUAL_KEY_OUT_TRIGGER_WORK://TPCMD_RAILSUPPORT_WORK
				TP_CMD = TPCMD_RAILSUPPORT_WORK;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

			case MANUAL_KEY_OUT_TRIGGER_HOME://TPCMD_RAILSUPPORT_HOME
				TP_CMD = TPCMD_RAILSUPPORT_HOME;
				tmpCmdData->uTP_CMD.CmdTP_Jog_DRV.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
				break;

            case MANUAL_KEY_MARK_DRIVING:
				TP_CMD = TPCMD_GO_MARK_FORWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

            case MANUAL_KEY_MARK_TRANS:
				TP_CMD = TPCMD_GO_TRANS_MARK_FORWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

            case MANUAL_KEY_MARK_QR_LEFT:
				TP_CMD = TPCMD_GO_QR_LEFT_MARK_FORWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;

            case MANUAL_KEY_MARK_QR_RIGHT:
				TP_CMD = TPCMD_GO_QR_RIGHT_MARK_FORWARD;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_NORMAL;
                break;
				
			case MANUAL_KEY_FORCE_PP:
				//TP_CMD = TagID::CMD_PASSPERMITFORCEOPEN_OHT2OCS;
				TP_CMD = TagID::CMD_PASSPERMITFORCEOPEN_OHT2TP;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
                break;
				
            case MANUAL_KEY_MODE_TO_AUTO:
				TP_CMD = TagID::CMD_AUTOMODE_TP_OHT;
				tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

				HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
                break;

            case MANUAL_KEY_MODE_TO_MANUAL:
                TP_CMD = TagID::CMD_MANUALMODE_TP_OHT;
                tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

                HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
                break;

            case MANUAL_KEY_ESTOP:
                m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_STOP);
                m_pOHTMainStatus->QuickCmdEStopCommander = ESTOP_BY_TP;
                run_stop_status = 1;
                return Error;
                break;

            case MANUAL_KEY_RESET:
				if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL)
				{
                    TP_CMD = TagID::CMD_SOFTRESET_TP_OHT;
                    tmpCmdData->uTP_CMD.CmdTP_Jog_TR.CmdID = nCountCmdID;

                    HandTP_CMD_Type = HANDTP_CMD_TYPE_EXCEPT;
                }
                break;

// 2016-12-29, Add by Labeler
// TP Pause , Resume 기능 구현
			case MANUAL_KEY_TP_RESUME://start
				if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO)
				{
                    m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_RESUME_TP);
                }
				break;

			case MANUAL_KEY_TP_PAUSE://stop
				if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO)
				{
                    m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_PAUSE_TP);
                }
				break;
			}

			if(HandTP_CMD_Type == HANDTP_CMD_TYPE_NORMAL)
			{
				// tag
				SEND_PACKET.TAG.Value = TagID::CMD_TP_COMMON;	// TAGCATEGORY::TAG_CATEGORY_OCSCOMMON;

				// value
				memcpy(&(SEND_PACKET.Value[0]), &TP_CMD, 4);
				memcpy(&(SEND_PACKET.Value[8]), &(tmpCmdData->uTP_CMD), sizeof(uTP_CMD_));

				// CMD add to TaskList
				CheckCMDData2TaskList(&SEND_PACKET, &pAckPkt, &AckPktPC, QuickCmd);
			}
            else if(HandTP_CMD_Type == HANDTP_CMD_TYPE_EXCEPT)
            {
                SEND_PACKET.TAG.Value = TP_CMD;

				// value
				memcpy(&(SEND_PACKET.Value[0]), &TP_CMD, 4);
				memcpy(&(SEND_PACKET.Value[8]), &(tmpCmdData->uTP_CMD), sizeof(uTP_CMD_));

				// CMD add to TaskList
				CheckCMDData2TaskList(&SEND_PACKET, &pAckPkt, &AckPktPC, QuickCmd);
            }
		}
		return Error;
	}
	else
	{
		//TBD
	}
	return Error;
}

// 2016-12-25, Handy TP용 Longrun Type 반환 함수
int TaskControl::GetHandyTPLongRun()
{
    return m_nLongrunTypeHandyTP;
}

void TaskControl::SetHandyTPLongRun(int nType)
{
    m_nLongrunTypeHandyTP = nType;
}

bool TaskControl::GetRotateJogCmd()
{
    return m_bRotateJogCmdExist;
}
void TaskControl::SetRotateJogCmd(bool bCmd)
{
    m_bRotateJogCmdExist = bCmd;
}

DWORD TaskControl::GetRotateJogMoveStartTime()
{
    return m_dwRotateJogMoveStartTime;
}

/**
@brief   HandTP 에 OHT Status를 보내는 Data Encode Function
@date    2017.01.05
@author  Labeler
@param   char * cBuf : 보낼 Data가 담길 Buffer
         int iIndex  : Handy TP로 부터 요청 받은 정보 Index Page
@return
*/
void TaskControl::MakeVHLStatusForHandyTP(unsigned char * cBuf, int iIndex)
{
    TEACHING_DATA *pTeachingData    = NULL;
	STATION_DATA *pStationData      = NULL;
	
    HWSet * pHWSet = OHTMainForm->GetHWSet();

    if(iIndex == HANDTP_STATUS_FIRST)
    {
        int status_Error = CheckError();

        cBuf[0] = REMOTE_TYPE_RF;
        cBuf[1] = REQ_OHT_STATUS;
        cBuf[2] = HANDTP_STATUS_FIRST;
        cBuf[3] = m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
        cBuf[4] = m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status;
        cBuf[5] = (m_pOHTMainStatus->StatusCommon.CurNodeID>>16) & 0xFF;         //g_status_node
        cBuf[6] = (m_pOHTMainStatus->StatusCommon.CurNodeID>>8) & 0xFF;          //g_status_node
        cBuf[7] = m_pOHTMainStatus->StatusCommon.CurNodeID & 0xFF;               //g_status_node
        cBuf[8] = (m_pOHTMainStatus->StatusCommon.CurStationID>>16) & 0xFF;      //g_status_station
        cBuf[9] = (m_pOHTMainStatus->StatusCommon.CurStationID>>8) & 0xFF;       //g_status_station
        cBuf[10] = m_pOHTMainStatus->StatusCommon.CurStationID & 0xFF;           //g_status_station
        cBuf[11] = (status_Error>>8) & 0xFF;
        cBuf[12] = status_Error & 0xFF;

        if(m_pPassPermit->GetPassPermitCount() != LISTCOUNT_EMPTY)
            cBuf[13] = 1;
        else
	        cBuf[13] = 0;
	        cBuf[14] = 0;
	        cBuf[15] = 0;
    }
    else if(iIndex == HANDTP_STATUS_SECOND)
    {
        int status_LDLV     = 0;
        int status_Station = m_pOHTMainStatus->StatusCommon.CurStationID;

        pStationData = OHTMainForm->GetStationDataManager()->Find(status_Station);
        if(pStationData == NULL)
        {
            ADD_HANDTP_LOG("[ERR_DATA_STATION_NOEXIST] Station Id: %d", status_Station);
        }
        else
        {
            // Lookdown의 경우, EQ, EQ_X, STK, Loader에만 사용
            // 하나의 Tag에 하나의 설비만 있기 때문에, 해당 방식으로 Teaching Data Search 가능
            for(int i = PORT_TYPE_EQ ; i <= PORT_TYPE_LOADER ; i++)
            {
                pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(status_Station, i);
                if(pTeachingData == NULL)
                {
                    ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] Station Id: %d, PortType : %d", status_Station, i);
                }
                else
                {
                    status_LDLV = pTeachingData->LookDownLevel;
                    break;
                }
            }

            // 2017-01-18, Add by Labeler, Station Type이 아닌, Port Type 으로 Teaching Data 탐색 필요
            pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(status_Station, pStationData->StationType);

            if(pTeachingData == NULL)
            {
                ADD_HANDTP_LOG("[ERR_DATA_TEACHING_NOEXIST] Station Id: %d, PortType : %d", status_Station, pStationData->StationType);
            }
            else
            {
                status_LDLV = pTeachingData->LookDownLevel;
            }
        }

        cBuf[0] = REMOTE_TYPE_RF;
        cBuf[1] = REQ_OHT_STATUS;
        cBuf[2] = HANDTP_STATUS_SECOND;
        cBuf[3] = (m_pOHTMainStatus->StatusTP.Hoist.Position>>8) & 0xFF;           // Hoist Cur Pos : High
        cBuf[4] = m_pOHTMainStatus->StatusTP.Hoist.Position & 0xFF;                // Hoist Cur Pos : low
        cBuf[5] = (m_pOHTMainStatus->StatusTP.Shift.Position>>8) & 0xFF;           // Shift Cur Pos : High
        cBuf[6] = m_pOHTMainStatus->StatusTP.Shift.Position & 0xFF;                // Shift Cur Pos : Low
        cBuf[7] = (m_pOHTMainStatus->StatusTP.Rotate.Position>>8) & 0xFF;          // Rotate Cur Pos : High
        cBuf[8] = m_pOHTMainStatus->StatusTP.Rotate.Position & 0xFF;               // Rotate Cur Pos : Low
        cBuf[9] = status_LDLV;                        // LookDown Level;
        cBuf[10] = m_pExecuteInfo->bOcsPause;       // OCS Pause
        cBuf[11] = m_pExecuteInfo->bTpPause;       // TP Pause
        cBuf[12] = pHWSet->m_pCID->GetCIDPause();       // CID Pause
        cBuf[13] = 0;
        cBuf[14] = 0;
        cBuf[15] = 0;
    }
}

/**
@brief   HandTP 에 OHT IO 정보를 보내는 Data Encode Function
@date    2017.01.05
@author  Labeler
@param   char * cBuf : 보낼 Data가 담길 Buffer
         int iIndex  : Handy TP로 부터 요청 받은 정보 Index Page
@return
*/
void TaskControl::MakeVHLIOInfoForHandyTP(unsigned char * cBuf, int iIndex)
{
    if(iIndex == HANDTP_IO_INFO_FIRST)
    {
        cBuf[0] = REMOTE_TYPE_RF;
        cBuf[1] = REQ_OHT_IO_INFO;

        cBuf[2] = iIndex;

        cBuf[3] = (m_pOHTMainStatus->StatusTP.Input[0] >> 24);
        cBuf[4] = (m_pOHTMainStatus->StatusTP.Input[0] >> 16);
        cBuf[5] = (m_pOHTMainStatus->StatusTP.Input[0] >> 8);
        cBuf[6] = m_pOHTMainStatus->StatusTP.Input[0];

        cBuf[7] = (m_pOHTMainStatus->StatusTP.Input[1] >> 24);
        cBuf[8] = (m_pOHTMainStatus->StatusTP.Input[1] >> 16);
        cBuf[9] = (m_pOHTMainStatus->StatusTP.Input[1] >> 8);
        cBuf[10] = m_pOHTMainStatus->StatusTP.Input[1];

        cBuf[11] = (m_pOHTMainStatus->StatusTP.Input[2] >> 24);
        cBuf[12] = (m_pOHTMainStatus->StatusTP.Input[2] >> 16);
        cBuf[13] = (m_pOHTMainStatus->StatusTP.Input[2] >> 8);
        cBuf[14] = m_pOHTMainStatus->StatusTP.Input[2];
    }
    else if(iIndex == HANDTP_IO_INFO_SECOND)
    {
        cBuf[0] = REMOTE_TYPE_RF;
        cBuf[1] = REQ_OHT_IO_INFO;

        cBuf[2] = iIndex;

        cBuf[3] = (m_pOHTMainStatus->StatusTP.Output[0] >> 24);
        cBuf[4] = (m_pOHTMainStatus->StatusTP.Output[0] >> 16);
        cBuf[5] = (m_pOHTMainStatus->StatusTP.Output[0] >> 8);
        cBuf[6] = m_pOHTMainStatus->StatusTP.Output[0];

        cBuf[7] = (m_pOHTMainStatus->StatusTP.Output[1] >> 24);
        cBuf[8] = (m_pOHTMainStatus->StatusTP.Output[1] >> 16);
        cBuf[9] = (m_pOHTMainStatus->StatusTP.Output[1] >> 8);
        cBuf[10] = m_pOHTMainStatus->StatusTP.Output[1];

        cBuf[11] = (m_pOHTMainStatus->StatusTP.Output[2] >> 24);
        cBuf[12] = (m_pOHTMainStatus->StatusTP.Output[2] >> 16);
        cBuf[13] = (m_pOHTMainStatus->StatusTP.Output[2] >> 8);
        cBuf[14] = m_pOHTMainStatus->StatusTP.Output[2];
    }
}

/**
@brief   HandTP 에 OHT Status를 보내는 Data Encode Function
@date    2017.04.21
@author  KJD
@param   char * cBuf : 보낼 Data가 담길 Buffer
         int iIndex  : Handy TP로 부터 요청 받은 정보 Index Page
@return
*/
void TaskControl::MakeVHLStatusBGForHandyTP(unsigned char * cBuf, int iIndex)
{
    TEACHING_DATA *pTeachingData    = NULL;
	STATION_DATA *pStationData      = NULL;
	
    HWSet * pHWSet = OHTMainForm->GetHWSet();

    int status_Error = CheckError();
    unsigned char SteerFront = pHWSet->m_pSteer->GetFrontPos();
    unsigned char SteerRear = pHWSet->m_pSteer->GetRearPos();

    cBuf[0] = REMOTE_TYPE_RF;
    cBuf[1] = REQ_OHT_STATUS_BG;
    cBuf[2] = 0;
    cBuf[3] = m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode;
    cBuf[4] = m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status;
    cBuf[5] = (status_Error>>8) & 0xFF;
    cBuf[6] = status_Error & 0xFF;
    cBuf[7] = m_pExecuteInfo->bOcsPause;      // OCS Pause
    cBuf[8] = m_pExecuteInfo->bTpPause;       // TP Pause
    cBuf[9] = pHWSet->m_pCID->GetCIDPause();  // CID Pause
    cBuf[10] = (SteerFront<<4) | SteerRear;     // Steer Status
}


/**
@brief   Hand TP Cycle Key 동작에 대한 Interlock Check Function
@date    2017.01.10
@author  Labeler
@param   unsigned char* pRecvBuf : Hand TP로 부터 받은 정보
@return  Interlock 불만족 시 Error Code Return
         Interlock 만족 시 NO_ERR Return
*/
int TaskControl::CheckInterlock4Cycle(unsigned char* pRecvBuf)
{
    int nError = NO_ERR;
    int nCycle = pRecvBuf[5]<<8 | pRecvBuf[6];


    // OHT에 Error가 있을 경우, Reset Cycle 제외하고 수행 불가능 처리
    if((CheckError() != NO_ERR) && (nCycle != CYC_VHL_ERR_RESET))
    {
        return HANDTP_ERR_OHT_ERR;
    }

    switch(nCycle)
    {
        case CYC_MOVE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Node 유/무 Check (없을 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurNodeID == 0))
            {
                nError = HANDTP_ERR_CUR_NODE_NOT_EXIST;
            }

            // 3. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 4. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 5. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
			}
#endif


            // 7. 타겟 위치 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
			{
                int nTargetStation = pRecvBuf[7]<<16 | pRecvBuf[8]<<8 | pRecvBuf[9];
                int nTargetNode = pRecvBuf[10]<<16 | pRecvBuf[11]<<8 | pRecvBuf[12];

                if( (nTargetStation == 0) && (nTargetNode == 0) )
                {
                    nError = HANDTP_ERR_NOT_INPUT_TARGET_INFO;
                }
            }

            // 8. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_MOVE_UNLOAD:
        case CYC_MOVE_LOAD:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Node 유/무 Check (없을 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurNodeID == 0))
            {
                nError = HANDTP_ERR_CUR_NODE_NOT_EXIST;
            }

            // 3. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 4. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 5. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR)
			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif


            // 7. Target Station ID 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
			{
                int nTargetStation = pRecvBuf[7]<<16 | pRecvBuf[8]<<8 | pRecvBuf[9];
                if(nTargetStation == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_STATION_ID;
                }
            }

            // 8. Target Port Type 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetPortType = pRecvBuf[13];
                if(nTargetPortType == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_PORT_TYPE;
                }
            }

            // 9. Shutter Origin 상태가 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_ORIGIN;
            }

            // 10. Hand Origin 상태가 아닐 경우 동작 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsReady() == false))
            {
                nError = HANDTP_ERR_HAND_NOT_ORIGIN;
            }

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
            // 11. R-OHT의 경우, Rotate Origin 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == false))
            {
                nError = HANDTP_ERR_ROTATE_NOT_ORIGIN;
            }
#endif

            // 12. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_MTL_OUT:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Node 유/무 Check (없을 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurNodeID == 0))
            {
                nError = HANDTP_ERR_CUR_NODE_NOT_EXIST;
            }

            // 3. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 4. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 5. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif

            // 7. Hand TP Longrun 동작 중일 경우 주행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_NOMOVE_UNLOAD:
        case CYC_NOMOVE_LOAD:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Node 유/무 Check (없을 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurNodeID == 0))
            {
                nError = HANDTP_ERR_CUR_NODE_NOT_EXIST;
            }

            // 3. Current Sation 없을 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurStationID == 0))
            {
                nError = HANDTP_ERR_CUR_STATION_NOT_EXIST;
            }

            // 4. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 5. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 6. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif


            // 8. Station ID 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetStation = pRecvBuf[7]<<16 | pRecvBuf[8]<<8 | pRecvBuf[9];
                if(nTargetStation == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_STATION_ID;
                }
            }

            // 9. Port Type 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetPortType = pRecvBuf[13];
                if(nTargetPortType == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_PORT_TYPE;
                }
            }

            // 10. Shutter Origin 상태가 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_ORIGIN;
            }

            // 11. Hand Origin 상태가 아닐 경우 동작 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsReady() == false))
            {
                nError = HANDTP_ERR_HAND_NOT_ORIGIN;
            }

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 12. R-OHT의 경우, Rotate Origin 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == false))
            {
                nError = HANDTP_ERR_ROTATE_NOT_ORIGIN;
            }
#endif

            // 13. Trans Longrun 외 다른 Longrun 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_TRANS))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_MOVE_TEACHING_POSITION:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Node 유/무 Check (없을 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurNodeID == 0))
            {
                nError = HANDTP_ERR_CUR_NODE_NOT_EXIST;
            }

            // 3. Current Sation 없을 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurStationID == 0))
            {
                nError = HANDTP_ERR_CUR_STATION_NOT_EXIST;
            }

            // 4. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 5. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

            // 6. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif


            // 8. Station ID 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetStation = pRecvBuf[7]<<16 | pRecvBuf[8]<<8 | pRecvBuf[9];
                if(nTargetStation == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_STATION_ID;
                }
            }

            // 9. Port Type 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetPortType = pRecvBuf[13];
                if(nTargetPortType == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_PORT_TYPE;
                }
            }

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 10. R-OHT의 경우, Rotate Origin 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == false))
            {
                nError = HANDTP_ERR_ROTATE_NOT_ORIGIN;
            }
#endif

            // 11. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_MOVE_ALL_HOME:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();
		break;

		case CYC_MOVE_AUTO_HOISTHOME:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();
        break;

        case CYC_AUTO_MODE_CHANGE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();
        break;

        case CYC_TEACHING_SAVE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Sation 없을 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurStationID == 0))
            {
                nError = HANDTP_ERR_CUR_STATION_NOT_EXIST;
            }

            // 3. Station ID 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetStation = pRecvBuf[7]<<16 | pRecvBuf[8]<<8 | pRecvBuf[9];
                if(nTargetStation == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_STATION_ID;
                }
            }

            // 4. Port Type 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetPortType = pRecvBuf[13];
                if(nTargetPortType == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_PORT_TYPE;
                }
            }

            // 5. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_MOVE_HOIST_TEACHING:
        case CYC_MOVE_SHIFT_TEACHING:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Sation 없을 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurStationID == 0))
            {
                nError = HANDTP_ERR_CUR_STATION_NOT_EXIST;
            }

            // 3. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 4. Shutter Close 상태일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false) || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
			{
				nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
			}

            // 5. Station ID 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetStation = pRecvBuf[7]<<16 | pRecvBuf[8]<<8 | pRecvBuf[9];
                if(nTargetStation == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_STATION_ID;
                }
            }

            // 6. Port Type 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetPortType = pRecvBuf[13];
                if(nTargetPortType == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_PORT_TYPE;
                }
            }

            // 7. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_MOVE_ROTATE_TEACHING:
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Sation 없을 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurStationID == 0))
            {
                nError = HANDTP_ERR_CUR_STATION_NOT_EXIST;
            }

            // 3. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 4. Shutter Close 상태일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false) || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
			{
				nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
			}

            // 5. Station ID 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetStation = pRecvBuf[7]<<16 | pRecvBuf[8]<<8 | pRecvBuf[9];
                if(nTargetStation == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_STATION_ID;
                }
            }

            // 6. Port Type 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetPortType = pRecvBuf[13];
                if(nTargetPortType == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_PORT_TYPE;
                }
            }

            // 7. R-OHT의 경우, Rotate Origin 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == false))
            {
                nError = HANDTP_ERR_ROTATE_NOT_ORIGIN;
            }

            // 8. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
#else
            nError = HANDTP_ERR_NO_USE_CMD;
#endif
        break;

        case CYC_MAP_MAKE_NOFILE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Node 유/무 Check (없을 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurNodeID == 0))
            {
                nError = HANDTP_ERR_CUR_NODE_NOT_EXIST;
            }

            // 3. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 4. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.SHIFT_Home == false))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

			// 5. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif


            // 7. 타겟 위치 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetNode = pRecvBuf[10]<<16 | pRecvBuf[11]<<8 | pRecvBuf[12];

                if(nTargetNode == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_TARGET_INFO;
                }
            }

            // 8. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }

            // 8. Map Make 조정용 파일 없으면 Cycle 수행 불가능?
            // 추후 Map Make 기능 반영 할 때, Interlock 함께 반영
        break;

        case CYC_MAP_MAKE_FILE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Node 유/무 Check (없을 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurNodeID == 0))
            {
                nError = HANDTP_ERR_CUR_NODE_NOT_EXIST;
            }

            // 3. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 4. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.SHIFT_Home == false))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

			// 5. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif


            // 7. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_AUTO_TEACHING_NOFILE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Sation 없을 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurStationID == 0))
            {
                nError = HANDTP_ERR_CUR_STATION_NOT_EXIST;
            }

            // 3. Port Type 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetPortType = pRecvBuf[13];
                if(nTargetPortType == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_PORT_TYPE;
                }
            }

            // 4. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 5. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.SHIFT_Home == false))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

			// 6. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif

            // 8. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_AUTO_TEACHING_FILE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.SHIFT_Home == false))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

			// 4. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif

            // 6. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_AUTO_TEACHING_DRV_OFFSET:
            // 1. Station ID 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetStation = pRecvBuf[7]<<16 | pRecvBuf[8]<<8 | pRecvBuf[9];
                if(nTargetStation == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_STATION_ID;
                }
            }

            // 2. Port Type 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetPortType = pRecvBuf[13];
                if(nTargetPortType == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_PORT_TYPE;
                }
            }
        break;

        case CYC_MAPSET_UPLOAD:
        case CYC_MAPSET_DOWN_TEACHING:
        case CYC_MAPSET_DOWN_OPERATION:
            // 해당 Cycle의 경우 Interlock 필요???
        break;

        case CYC_DRIVING_F_SERVO_OFF:
        case CYC_DRIVING_F_SERVO_ON:
        case CYC_DRIVING_R_SERVO_OFF:
        case CYC_DRIVING_R_SERVO_ON:
//        case CYC_RAILSUPPORT_HOME:
//		case CYC_RAILSUPPORT_WORK:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
			}
        break;

        case CYC_SHUTTER_ORIGIN:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand Open 상태일 때, 재하 감지 될 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HAND_Close == false)
            && ((m_pHWCommon->m_Input.FOUP_Detect == true)
            || (m_pHWCommon->m_Input.HAND_Right_Grip == true) || (m_pHWCommon->m_Input.HAND_Left_Grip == true)))
            {
                nError = HANDTP_ERR_HAND_NOT_CLOSE;
            }

            // 3. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_SHUTTER_OPEN:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand Open 상태일 때, 재하 감지 될 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HAND_Close == false)
            && ((m_pHWCommon->m_Input.FOUP_Detect == true)
            || (m_pHWCommon->m_Input.HAND_Right_Grip == true) || (m_pHWCommon->m_Input.HAND_Left_Grip == true)))
            {
                nError = HANDTP_ERR_HAND_NOT_CLOSE;
            }

            // 3. Shutter Origin 상태가 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_ORIGIN;
            }

            // 4. Hand TP Longrun 동작 중(Shutter Longrun 제외)일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_SHUTTER))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_GO_MARK_FORWARD:
        case CYC_GO_TRANS_MARK_FORWARD:
        case CYC_GO_QR_LEFT_MARK_FORWARD:
        case CYC_GO_QR_RIGHT_MARK_FORWARD:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 주행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shift Home Positon Check (Home 아닐 경우 주행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.SHIFT_Home == false))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

            // 4. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 주행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
			{
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif


            // 6. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_HAND_OPEN:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

    		// 2. Hand Push 센서 미감지 상태일 때, 재하 감지 될 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HAND_Push == false)
            &&((m_pHWCommon->m_Input.HAND_Left_Grip == true) || (m_pHWCommon->m_Input.HAND_Right_Grip == true)))
            {
                nError = HANDTP_ERR_CARRIER_EXIST;
            }

            // 3. Hand Origin 상태가 아닐 경우 동작 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsReady() == false))
            {
                nError = HANDTP_ERR_HAND_NOT_ORIGIN;
            }

    		// 4. Hand TP Longrun 동작 중(Hand Longrun은 제외)일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_HAND))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
		break;

        case CYC_HAND_CLOSE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand Origin 상태가 아닐 경우 동작 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsReady() == false))
            {
                nError = HANDTP_ERR_HAND_NOT_ORIGIN;
            }

            // 3. Hand TP Longrun 동작 중(Hand Longrun은 제외)일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_HAND))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_HAND_HOMING:
        case CYC_HAND_ENABLE:
        case CYC_HAND_FREE:
		case CYC_HAND_PIO_SELECT_OFF:
        case CYC_HAND_PIO_SELECT_ON:
        case CYC_HAND_PWR_OFF:
        case CYC_HAND_PWR_ON:
        case CYC_HAND_PWR_RST:
		case CYC_HAND_RESET:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

    		// 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
		break;

        case CYC_HOIST_SERVO_ON:
        case CYC_HOIST_SERVO_OFF:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

    		// 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_HOIST_ORIGIN_SETTING:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

    		// 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

		case CYC_JOG_HOIST_HOME:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Shutter Close 상태일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
			{
				nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
			}

		    // 3. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
		break;

        case CYC_JOG_HOIST_STOP:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable(EXCLUDE_HOIST_RUNNING);

    		// 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
		break;

        case CYC_JOG_SHIFT_HOME:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 주행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shutter Close 상태일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
			{
				nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
			}

            // 4. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
		break;

        case CYC_JOG_SHIFT_STOP:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_SHIFT_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable(EXCLUDE_SHIFT_RUNNING);

    		// 2. Hand TP Trans Longrun 동작 중일 경우 Longrun 해제 해야 함
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
		break;

        case CYC_JOG_ROTATE_HOME:
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

    		// 2. Hoist Home Position Check (Home 아닐 경우 주행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

    		// 3. Shutter Close 상태일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
			{
				nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
			}

            // 4. R-OHT의 경우, Rotate Origin 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == false))
            {
                nError = HANDTP_ERR_ROTATE_NOT_ORIGIN;
            }

    		// 5. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
#else
            nError = HANDTP_ERR_NO_USE_CMD;
#endif
		break;

        case CYC_JOG_ROTATE_STOP:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING);

    		// 2. Hand TP Trans Longrun 동작 중일 경우 Longrun 해제 해야 함
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
		break;

        case CYC_LOOKDOWN_DISABLE:
        case CYC_LOOKDOWN_ENABLE:
            // 미사용 Cycle 처리
            nError = HANDTP_ERR_NO_USE_CMD;
        break;

        case CYC_SHIFT_ORIGIN_SETTING:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

    		// 2. Hand TP Trans Longrun 동작 중일 경우 Longrun 해제 해야 함
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

		case CYC_SHIFT_SERVO_OFF:
		case CYC_SHIFT_SERVO_ON:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

    		// 2. Hand TP Trans Longrun 동작 중일 경우 Longrun 해제 해야 함
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

		case CYC_SHUTTER_CLOSE:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shift Position이 +-2mm 이상일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

            // 4. Rotate Position이 0 or 180도가 아닐 경우 Cycle 수행 불가능
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif

            // 5. Shutter Origin 상태가 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_ORIGIN;
            }

            // 6. Hand TP Longrun 동작 중(Shutter Longrun 제외)일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_SHUTTER))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
		break;

		case CYC_SHUTTER_ALARM_RST:
		case CYC_SHUTTER_ENABLE:
		case CYC_SHUTTER_FREE:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
		break;

		case CYC_STEER_LEFT:
		case CYC_STEER_RIGHT:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중(Steer Longrun 제외)일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_STEER))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
		break;

        case CYC_STEER_F_ENABLE:
        case CYC_STEER_F_FREE:
        case CYC_STEER_R_ENABLE:
        case CYC_STEER_R_FREE:
        case CYC_TRACTION_STEER_LEFT:
        case CYC_TRACTION_STEER_RIGHT:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_LEFT_PIO_SELECT_ON:
        case CYC_LEFT_PIO_SELECT_OFF:
        case CYC_LEFT_PIO_VALID_ON:
        case CYC_LEFT_PIO_VALID_OFF:
        case CYC_LEFT_PIO_TR_REQ_ON:
        case CYC_LEFT_PIO_TR_REQ_OFF:
        case CYC_LEFT_PIO_BUSY_ON:
        case CYC_LEFT_PIO_BUSY_OFF:
        case CYC_LEFT_PIO_COMPT_ON:
        case CYC_LEFT_PIO_COMPT_OFF:
        case CYC_LEFT_PIO_ALL_OFF:
        case CYC_RIGHT_PIO_SELECT_ON:
        case CYC_RIGHT_PIO_SELECT_OFF:
        case CYC_RIGHT_PIO_VALID_ON:
        case CYC_RIGHT_PIO_VALID_OFF:
        case CYC_RIGHT_PIO_TR_REQ_ON:
        case CYC_RIGHT_PIO_TR_REQ_OFF:
        case CYC_RIGHT_PIO_BUSY_ON:
        case CYC_RIGHT_PIO_BUSY_OFF:
        case CYC_RIGHT_PIO_COMPT_ON:
        case CYC_RIGHT_PIO_COMPT_OFF:
        case CYC_RIGHT_PIO_ALL_OFF:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_ROTATE_0:
        case CYC_ROTATE_90:
        case CYC_ROTATE_180:
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
			nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 주행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shutter Open Position이 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
			{
				nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
			}

            // 4. R-OHT의 경우, Rotate Origin 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == false))
            {
                nError = HANDTP_ERR_ROTATE_NOT_ORIGIN;
            }

            // 5. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
#else
            nError = HANDTP_ERR_NO_USE_CMD;
#endif
        break;

        case CYC_ROTATE_HOMING:
        case CYC_ROTATE_GO_ORIGIN:
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
			nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 주행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shutter Open Position이 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
			{
				nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
			}

            // 4. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
#else
            nError = HANDTP_ERR_NO_USE_CMD;
#endif
        break;

        case CYC_ROTATE_270:
        case CYC_ROTATE_RESET:
        case CYC_ROTATE_FREE:
        case CYC_ROTATE_ENABLE:
        case CYC_ROTATE_ORIGIN_SETTING:
            // 미사용 Cycle 처리
            nError = HANDTP_ERR_NO_USE_CMD;
        break;

        case CYC_ROTATE_SERVO_OFF:
        case CYC_ROTATE_SERVO_ON:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_CID_LEFT_SEL:
        case CYC_CID_RIGHT_SEL:
        case CYC_CID_OCCUPREQ:
        case CYC_CID_PASSCOMPT:
        case CYC_CID_MANUAL:
        case CYC_CID_OPERATION:
        case CYC_CID_RESERVE_1:
        case CYC_CID_RESERVE_2:
        case CYC_CID_LEFT_SEL_OFF:
        case CYC_CID_RIGHT_SEL_OFF:
        case CYC_CID_OCCUPREQ_OFF:
        case CYC_CID_PASSCOMPT_OFF:
        case CYC_CID_MANUAL_OFF:
        case CYC_CID_OPERATION_OFF:
        case CYC_CID_RESERVE_1_OFF:
        case CYC_CID_RESERVE_2_OFF:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_OHT_MANUALMODE:
            // 1. Auto Mode 상태에서, OCS Pause 상태 일 경우 Manual 전환 불가
            if((GetCurrentMode() == MODE_AUTO) && (m_pExecuteInfo->bOcsPause != false))
            {
                nError = HANDTP_ERR_OCS_PAUSED;
            }

            // 2. Auto Mode 상태에서, TP Pause 상태일 경우 Manual 전환 불가
            if((nError == NO_ERR) && (GetCurrentMode() == MODE_AUTO) && (m_pExecuteInfo->bTpPause != false))
            {
                nError = HANDTP_ERR_TP_PAUSED;
            }

            // 3. Auto Mode 상태에서, CID Pause 상태일 경우 Manual 전환 불가
            if((nError == NO_ERR) && (GetCurrentMode() == MODE_AUTO) && (m_pHWSet->m_pCID->GetCIDPause() != false))
            {
                nError = HANDTP_ERR_CID_PAUSED;
            }

            // 4. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_OHT_AUTOMODE:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

            // 4. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif



#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 6. R-OHT의 경우, Rotate Origin 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == false))
            {
                nError = HANDTP_ERR_ROTATE_NOT_ORIGIN;
            }
#endif
            // 7. OHT Status가 Init이 아닐 경우 Cycle 수행 불가능
			if((nError == NO_ERR) && (GetExecuteStatus() != STS_INIT))
            {
                nError = HANDTP_ERR_OHT_NOT_INIT;
            }

            // 8. Mark 상태가 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusTP.MarkInfo.Data == 0))
            {
                nError = HANDTP_ERR_NO_MARK;
            }

            // 9. Hand TP Longrun 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_TRANS_LONGRUN_START:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Current Node 유/무 Check (없을 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurNodeID == 0))
            {
                nError = HANDTP_ERR_CUR_NODE_NOT_EXIST;
            }

            // 3. Current Sation 없을 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.CurStationID == 0))
            {
                nError = HANDTP_ERR_CUR_STATION_NOT_EXIST;
            }

            // 4. Port Type 없을 경우 Cycle 수행 불가능
            if(nError == NO_ERR)
            {
                int nTargetPortType = pRecvBuf[13];
                if(nTargetPortType == 0)
                {
                    nError = HANDTP_ERR_NOT_INPUT_PORT_TYPE;
                }
            }

            // 5. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 6. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

            // 7. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif


            // 9. Hand TP Longrun 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_TRANS_LONGRUN_STOP:
            // 1. Trans Longrun 진행 중이 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_TRANS))
            {
                nError = HANDTP_ERR_NOT_TRANS_LONGRUN;
            }
        break;

        case CYC_HAND_LONGRUN_START:
        case CYC_SHUTTER_LONGRUN_START:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

            // 4. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif

            // 5. Hand TP Longrun 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_HAND_LONGRUN_STOP:
            // 1. Hand Longrun 진행 중이 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_HAND))
            {
                nError = HANDTP_ERR_NOT_HAND_LONGRUN;
            }
        break;

        case CYC_SHUTTER_LONGRUN_STOP:
            // 1. Shutter Longrun 진행 중이 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_SHUTTER))
            {
                nError = HANDTP_ERR_NOT_SHUTTER_LONGRUN;
            }
        break;

        case CYC_STEER_LONGRUN_START:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case CYC_STEER_LONGRUN_STOP:
            // 1. Steer Longrun 진행 중이 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_STEER))
            {
                nError = HANDTP_ERR_NOT_STEER_LONGRUN;
            }
        break;

        case CYC_VHL_ERR_RESET:
            // Reset 동작의 경우 Interlock 없음
        break;
    }

    return nError;
}

/**
@brief   Hand TP Manual Key 동작에 대한 Interlock Check Function
@date    2017.01.10
@author  Labeler
@param   unsigned char cKeyCmd : Hand TP로 부터 받은 Manual Key Command
@return  Interlock 불만족 시 Error Return
         Interlock 만족 시 NO_ERR Return
*/
int TaskControl::CheckInterlock4ManualKey(unsigned char cKeyCmd)
{
    int nError = NO_ERR;

    switch(cKeyCmd)
    {
        case MANUAL_KEY_JOG_SLOW_DRIVING_FW:
        case MANUAL_KEY_JOG_SLOW_DRIVING_BW:
        case MANUAL_KEY_JOG_MIDDLE_DRIVING_FW:
        case MANUAL_KEY_JOG_MIDDLE_DRIVING_BW:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_DRIVING_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable(EXCLUDE_DRIVING_RUNNING);

            // 2. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

			// 4. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif

            // 5. 주행 축에 Error 존재 할 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pOHTMainStatus->StatusCommon.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
            || (m_pOHTMainStatus->StatusCommon.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
            || (m_pOHTMainStatus->StatusCommon.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
            || (m_pOHTMainStatus->StatusCommon.ErrorCode == ERR_AXIS1_R_AMP_FAULT)))
            {
                switch(m_pOHTMainStatus->StatusCommon.ErrorCode)
            {
                    case ERR_AXIS1_F_ERROR_LIMIT:
                        nError = HANDTP_ERR_AXIS1_F_ERROR_LIMIT;
                    break;

                    case ERR_AXIS1_R_ERROR_LIMIT:
                        nError = HANDTP_ERR_AXIS1_R_ERROR_LIMIT;
                    break;

                    case ERR_AXIS1_F_AMP_FAULT:
                        nError = HANDTP_ERR_AXIS1_F_AMP_FAULT;
                    break;

                    case ERR_AXIS1_R_AMP_FAULT:
                        nError = HANDTP_ERR_AXIS1_R_AMP_FAULT;
                    break;

                    default:
                break;
            }
            }

            // 6. Error 존재 시, Sttering 방향 다를 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && (m_pOHTMainStatus->StatusCommon.ErrorCode != NO_ERR)
            &&((m_pHWCommon->m_Input.STEER_F_Left != m_pHWCommon->m_Input.STEER_R_Left)
             ||(m_pHWCommon->m_Input.STEER_F_Right != m_pHWCommon->m_Input.STEER_R_Right)))
            {
                nError = HANDTP_ERR_STEER_DIFER_DIR;
            }

            // 7. 내부 재하 감지 중, Hand Close 상태 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.FOUP_Detect == true))
            {
                if((m_pHWCommon->m_Input.HAND_Open == true) || (m_pHWCommon->m_Input.HAND_Close == false))
                {
                    nError = HANDTP_ERR_HAND_NOT_CLOSE;
                }
            }

            // 8. 내부 재하 감지 중, Hand Grip 미 감지일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.FOUP_Detect == true))
            {
                if((m_pHWCommon->m_Input.HAND_Right_Grip == false) || (m_pHWCommon->m_Input.HAND_Left_Grip == false))
                {
                    nError = HANDTP_ERR_CARRIER_MISMATCH;
                }
                }

            // 9. 내부 재하 감지 중, Shutter Close 상태 아닐 경우 Cycle 수행 불가능
#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))&& (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
#if(OHT_NAME == OHT_NAME_STD_V85)
		if ((nError == NO_ERR) && (m_pHWCommon->m_Input.POD_EUV_1 == true)) //&& (m_pHWCommon->m_Input.POD_EUV_2 == true))
#else
        if ((nError == NO_ERR) && (m_pHWCommon->m_Input.POD_EUV_1 == true) && (m_pHWCommon->m_Input.POD_EUV_2 == true))
#endif

			{
                if((m_pHWCommon->m_Input.SHUTTER_F_Middle_Close == false) || (m_pHWCommon->m_Input.SHUTTER_R_Middle_Close == false))
                {
                    nError = HANDTP_ERR_SHUTTER_NOT_CLOSE;
                }
			}
            else if((nError == NO_ERR) && (m_pHWCommon->m_Input.FOUP_Detect == true))
            {
                if((m_pHWCommon->m_Input.SHUTTER_F_Close == false) || (m_pHWCommon->m_Input.SHUTTER_R_Close == false))
                {
                    nError = HANDTP_ERR_SHUTTER_NOT_CLOSE;
                }
			}
            else
            {
//                if(m_pHWCommon->m_Input.POD_EUV_1 != m_pHWCommon->m_Input.POD_EUV_2)
//                {
//                     nError = HANDTP_ERR_CARRIER_MISMATCH;
//                }
            }
#else
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.FOUP_Detect == true))
            {
                if((m_pHWCommon->m_Input.SHUTTER_F_Close == false) || (m_pHWCommon->m_Input.SHUTTER_R_Close == false))
                {
                    nError = HANDTP_ERR_SHUTTER_NOT_CLOSE;
                }
            }
#endif

            // 10. 내부 재하 미감지 중, Hand Close 일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.FOUP_Detect == false))
            {
                if((m_pHWCommon->m_Input.HAND_Open == false) || (m_pHWCommon->m_Input.HAND_Close == true))
                {
                    nError = HANDTP_ERR_HAND_NOT_OPEN;
                }
            }

            // 11. 내부 재하 미감지 중, Hand Close 일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.FOUP_Detect == false))
            {
                if((m_pHWCommon->m_Input.HAND_Right_Grip == true) || (m_pHWCommon->m_Input.HAND_Left_Grip == true))
                {
                    nError = HANDTP_ERR_CARRIER_MISMATCH;
                }
                }

            // 12. 내부 재하 미감지 중, Shutter Close 일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.FOUP_Detect == false))
            {
                if((m_pHWCommon->m_Input.SHUTTER_F_Open == false) || (m_pHWCommon->m_Input.SHUTTER_R_Open == false))
                {
                    nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
                }
            }

            // 14. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_JOG_HOIST_UP:
        case MANUAL_KEY_JOG_HOIST_DOWN:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_HOIST_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable(EXCLUDE_HOIST_RUNNING);

            // 2. Shutter가 Open 상태가 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            ||  (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
            }

            // 3. Ocillation이 미감지 상태일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && (m_pOHTMainStatus->StatusTP.UseInfo.Info.OscillationSensor == true)
            &&(m_pOHTMainStatus->StatusTP.Hoist.Position > 2500)
            &&(m_pHWCommon->m_Input.OSCILLATION == false))
            {
                nError = HANDTP_ERR_OSCILLATION_OFF;
            }

            // 4. Hoist Position이 0 미만일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && (cKeyCmd == MANUAL_KEY_JOG_HOIST_UP)
            && (m_pOHTMainStatus->StatusTP.Hoist.Position < 0))
            {
                nError = HANDTP_ERR_HOIST_N_LIMIT;
            }

            // 5. Hoist Position이 4000mm 이상일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && (cKeyCmd == MANUAL_KEY_JOG_HOIST_DOWN)
            && (m_pOHTMainStatus->StatusTP.Hoist.Position > 40000))
            {
                nError = HANDTP_ERR_HOIST_P_LIMIT;
            }

            // 6. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_HOIST_HOME:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Shutter가 Open 상태가 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            ||  (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
            }

            // 3. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_JOG_ROTATE_CCW:
        case MANUAL_KEY_JOG_ROTATE_CW:
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable(EXCLUDE_ROTATE_RUNNING);

            // 2. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }
            // 3. R-OHT의 경우, Rotate Origin 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == false))
            {
                nError = HANDTP_ERR_ROTATE_NOT_ORIGIN;
            }

            // 4. Shutter가 Open 상태가 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            ||  (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
            }

            // 5. Rotate Position이 0도 미만 위치에 있을 경우 수행 불가능
            if((nError == NO_ERR)
            && (cKeyCmd == MANUAL_KEY_JOG_ROTATE_CCW)
            && (m_pOHTMainStatus->StatusTP.Rotate.Position < 0))
            {
                nError = HANDTP_ERR_ROTATE_N_LIMIT;
            }

            // 6. Rotate Position이 180도 초과 위치에 있을 경우 수행 불가능
            if((nError == NO_ERR)
            && (cKeyCmd == MANUAL_KEY_JOG_ROTATE_CW)
            && (m_pOHTMainStatus->StatusTP.Rotate.Position > 180))
            {
                nError = HANDTP_ERR_ROTATE_P_LIMIT;
            }

            // 7. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
#else
            nError = HANDTP_ERR_NO_USE_CMD;
#endif
        break;

        case MANUAL_KEY_ROTATE_HOME:
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. R-OHT의 경우, Rotate Origin 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == false))
            {
                nError = HANDTP_ERR_ROTATE_NOT_ORIGIN;
            }

			// 4. Shutter Close 상태일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
            }

            // 5. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
#else
            nError = HANDTP_ERR_NO_USE_CMD;
#endif

        case MANUAL_KEY_ROTATE_ORIGIN:
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

			// 3. Shutter Close 상태일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
			}

            // 4. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
#else
            nError = HANDTP_ERR_NO_USE_CMD;
#endif

                break;

        case MANUAL_KEY_JOG_SLIDE_LEFT:
        case MANUAL_KEY_JOG_SLIDE_RIGHT:
            // 1. m_pCmdManager->Is_Executable(EXCLUDE_SHIFT_RUNNING) != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable(EXCLUDE_SHIFT_RUNNING);

			// 2. Shutter Close 상태일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
			{
				nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
            }

            // 3. Shift Position이 N_Limit 위치에 있을 경우 수행 불가능
            if((nError == NO_ERR)
            && (cKeyCmd == MANUAL_KEY_JOG_SLIDE_LEFT)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
            && (m_pOHTMainStatus->StatusTP.Shift.Position < -200))
#else
            && (m_pOHTMainStatus->StatusTP.Shift.Position < -4600))
#endif
            {
                nError = HANDTP_ERR_SHIFT_N_LIMIT;
            }

            // 4. Shift Position이 P_Limit 위치에 있을 경우 수행 불가능
            if((nError == NO_ERR)
            && (cKeyCmd == MANUAL_KEY_JOG_SLIDE_RIGHT)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            && (m_pOHTMainStatus->StatusTP.Shift.Position > 200))
#else
            && (m_pOHTMainStatus->StatusTP.Shift.Position > 4500))
#endif
            {
                nError = HANDTP_ERR_SHIFT_P_LIMIT;
            }

            // 5. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_SLIDE_HOME:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

			// 2. Shutter Close 상태일 경우 Cycle 수행 불가능
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.SHUTTER_F_Open == false)
            || (m_pHWCommon->m_Input.SHUTTER_R_Open == false)))
            {
				nError = HANDTP_ERR_SHUTTER_NOT_OPEN;
            }

            // 3. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 4. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_STEER_LEFT:
        case MANUAL_KEY_STEER_RIGHT:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_SHUTTER_OPEN:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand Close가 아닌 상태 일 때, 재하 감지 될 경우, Command 수행 불가능
            if((nError == NO_ERR)
            && (m_pHWCommon->m_Input.HAND_Close == false)
            &&((m_pHWCommon->m_Input.FOUP_Detect == true)
            || (m_pHWCommon->m_Input.HAND_Right_Grip == true)
            || (m_pHWCommon->m_Input.HAND_Left_Grip == true)))
            {
                nError = HANDTP_ERR_CARRIER_MISMATCH;
            }

            // 3. Hand Close 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HAND_Close == false))
            {
                nError = HANDTP_ERR_HAND_NOT_CLOSE;
            }

            // 4. Shutter Origin 상태가 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_ORIGIN;
            }

            // 5. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
                break;

        case MANUAL_KEY_SHUTTER_ORIGIN:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand Close가 아닌 상태 일 때, 재하 감지 될 경우, Command 수행 불가능
            if((nError == NO_ERR)
            && (m_pHWCommon->m_Input.HAND_Close == false)
            &&((m_pHWCommon->m_Input.FOUP_Detect == true)
            || (m_pHWCommon->m_Input.HAND_Right_Grip == true)
            || (m_pHWCommon->m_Input.HAND_Left_Grip == true)))
            {
                nError = HANDTP_ERR_CARRIER_MISMATCH;
            }

            // 3. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
                break;

        case MANUAL_KEY_SHUTTER_CLOSE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shift 축이 원점이 아닐 경우 수행 불가능
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
			}

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            // 4. Rotate 축이 Inposition 이 아닐 경우 수행 불가능
            if((nError == NO_ERR)
 			&& (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif
            // 5. Shutter Origin 상태가 아닐 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR))
            {
                nError = HANDTP_ERR_SHUTTER_NOT_ORIGIN;
            }

            // 6. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_SHUTTER_RESET:
        case MANUAL_KEY_SHUTTER_FREE:
        case MANUAL_KEY_SHUTTER_ENABLE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_OUT_TRIGGER_WORK:
        case MANUAL_KEY_OUT_TRIGGER_HOME:
        case MANUAL_KEY_OUT_TRIGGER_FREE:
        case MANUAL_KEY_OUT_TRIGGER_ENABLE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_HAND_OPEN:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Carrier 감지 상태에서는 동작 불가능
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HAND_Push == false)
            &&((m_pHWCommon->m_Input.HAND_Left_Grip == true) || (m_pHWCommon->m_Input.HAND_Right_Grip == true)))
            {
                nError = HANDTP_ERR_CARRIER_EXIST;
            }

            // 3. Hand Origin 상태가 아닐 경우 동작 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsReady() == false))
            {
                nError = HANDTP_ERR_HAND_NOT_ORIGIN;
            }

            // 4. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_HAND_CLOSE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand Origin 상태가 아닐 경우 동작 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsReady() == false))
            {
                nError = HANDTP_ERR_HAND_NOT_ORIGIN;
            }

            // 3. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_HAND_ORIGIN:
        case MANUAL_KEY_HAND_RESET:
        case MANUAL_KEY_HAND_FREE:
        case MANUAL_KEY_HAND_ENABLE:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_MARK_DRIVING:
        case MANUAL_KEY_MARK_TRANS:
        case MANUAL_KEY_MARK_QR_LEFT:
        case MANUAL_KEY_MARK_QR_RIGHT:
            // 1. m_pCmdManager->Is_Executable() != NO_ERR 일경우 Cycle 수행 불가능
            nError = m_pCmdManager->Is_Executable();

            // 2. Hoist Home Position Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (m_pHWCommon->m_Input.HOIST_Home == false))
            {
                nError = HANDTP_ERR_HOIST_NOT_HOME;
            }

            // 3. Shift Home Positon Check (Home 아닐 경우 Cycle 수행 불가능)
            if((nError == NO_ERR) && (fabs(m_pOHTMainStatus->StatusTP.Shift.Position) > 20))
            {
                nError = HANDTP_ERR_SHIFT_NOT_HOME;
            }

			// 4. R-OHT의 경우, Rotate Position Check (0도 or 180도 아닐 경우 Cycle 수행 불가능)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
            if((nError == NO_ERR)
            && (fabs(m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
			&& (fabs(ROTATE_POS_90 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
			&& (fabs(ROTATE_POS_180 - m_pOHTMainStatus->StatusTP.Rotate.Position) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
            {
                nError = HANDTP_ERR_ROTATE_NOT_INPOSITION;
            }
#endif

            // 5. 주행 축에 Error 존재 할 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && ((m_pOHTMainStatus->StatusCommon.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
            || (m_pOHTMainStatus->StatusCommon.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
            || (m_pOHTMainStatus->StatusCommon.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
            || (m_pOHTMainStatus->StatusCommon.ErrorCode == ERR_AXIS1_R_AMP_FAULT)))
            {
                switch(m_pOHTMainStatus->StatusCommon.ErrorCode)
            {
                    case ERR_AXIS1_F_ERROR_LIMIT:
                        nError = HANDTP_ERR_AXIS1_F_ERROR_LIMIT;
                    break;

                    case ERR_AXIS1_R_ERROR_LIMIT:
                        nError = HANDTP_ERR_AXIS1_R_ERROR_LIMIT;
                    break;

                    case ERR_AXIS1_F_AMP_FAULT:
                        nError = HANDTP_ERR_AXIS1_F_AMP_FAULT;
                    break;

                    case ERR_AXIS1_R_AMP_FAULT:
                        nError = HANDTP_ERR_AXIS1_R_AMP_FAULT;
                    break;

                    default:
                break;
            }
            }

            // 6. Steering 방향 다를 경우, 명령 수행 불가
            if((nError == NO_ERR)
            && ((m_pHWCommon->m_Input.STEER_F_Left != m_pHWCommon->m_Input.STEER_R_Left)
            || (m_pHWCommon->m_Input.STEER_F_Right != m_pHWCommon->m_Input.STEER_R_Right)))
            {
                nError = HANDTP_ERR_STEER_DIFER_DIR;
            }

            // 8. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;


        case MANUAL_KEY_MODE_TO_AUTO:
            // 1. Auto Mode일 경우 수행 불가
            if(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO)
            {
                nError = HANDTP_ERR_MODE_AUTO;
            }

#if(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
            // 2. R-OHT의 경우, Rotate Origin 상태가 아닐 경우, 수행 불가능
            if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == false))
            {
                nError = HANDTP_ERR_ROTATE_NOT_ORIGIN;
            }
#endif

            // 3. No Mark 상태일 경우 수행 불가능
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusTP.MarkInfo.Data == 0))
            {
                nError = HANDTP_ERR_NO_MARK;
            }

            // 4. Status 가 Init이 아닐 경우 수행 불가능
            if((nError == NO_ERR) && (m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status != STS_INIT))
            {
                nError = HANDTP_ERR_OHT_NOT_INIT;
            }

            // 5. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
        break;

        case MANUAL_KEY_ESTOP:
            // E-Stop의 경우, Interlock 없음
        break;

        case MANUAL_KEY_MODE_TO_MANUAL:
            // 1. Auto Mode 상태에서, OCS Pause 상태 일 경우 Manual 전환 불가
            if((GetCurrentMode() == MODE_AUTO) && (m_pExecuteInfo->bOcsPause != false))
            {
                nError = HANDTP_ERR_OCS_PAUSED;
            }

            // 2. Auto Mode 상태에서, TP Pause 상태일 경우 Manual 전환 불가
            if((nError == NO_ERR) && (GetCurrentMode() == MODE_AUTO) && (m_pExecuteInfo->bTpPause != false))
            {
                nError = HANDTP_ERR_TP_PAUSED;
            }

            // 3. Auto Mode 상태에서, CID Pause 상태일 경우 Manual 전환 불가
            if((nError == NO_ERR) && (GetCurrentMode() == MODE_AUTO) && (m_pHWSet->m_pCID->GetCIDPause() != false))
            {
                nError = HANDTP_ERR_CID_PAUSED;
            }

            // 4. Hand TP Longrun 동작 중일 경우 Cycle 수행 불가능
            if((nError == NO_ERR) && (m_nLongrunTypeHandyTP != HANDTP_LONGRUN_NONE))
            {
                nError = HANDTP_ERR_OHT_LONGRUN;
            }
            break;

        case MANUAL_KEY_RESET:
            // 현재 미사용 Cycle
            nError = HANDTP_ERR_NO_USE_CMD;
        break;

        case MANUAL_KEY_TP_RESUME:
            if(m_pCmdManager->Is_Executable() != NO_ERR)
            {
                if((m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO)
                && (m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status == STS_GOING)
                && (m_pOHTMainStatus->StatusTP.Driving_F.Speed == 0)
                && (m_pOHTMainStatus->StatusTP.Driving_R.Speed == 0)
                && (m_pExecuteInfo->bTpPause == true))
                {
                    nError = NO_ERR;
                }
                else
                {
                    nError = HANDTP_ERR_CANT_PAUSE_RESUME;
                    ADD_HANDTP_LOG("[Manual Key Interlock] HANDTP_ERR_HAND_CANT_CLOSE");
                }
            }
            else
            {
                nError = HANDTP_ERR_CANT_PAUSE_RESUME;
                ADD_HANDTP_LOG("[Manual Key Interlock] HANDTP_ERR_HAND_CANT_CLOSE");
            }
        break;

        case MANUAL_KEY_TP_PAUSE:
            if(m_pCmdManager->Is_Executable() != NO_ERR)
            {
                if((m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO)
                && (m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status == STS_GOING)
                && (m_pOHTMainStatus->StatusTP.Driving_F.Speed == 0)
                && (m_pOHTMainStatus->StatusTP.Driving_R.Speed == 0))
                {
                    nError = NO_ERR;
                }
                else
                {
                    nError = HANDTP_ERR_CANT_PAUSE_RESUME;
                    ADD_HANDTP_LOG("[Manual Key Interlock] HANDTP_ERR_HAND_CANT_CLOSE");
                }
            }
            else
            {
                nError = HANDTP_ERR_CANT_PAUSE_RESUME;
                ADD_HANDTP_LOG("[Manual Key Interlock] HANDTP_ERR_HAND_CANT_CLOSE");
            }
        break;
    }

    return nError;
}


/**
@brief   Hand TP 명령에 대한 응답  Function
@date    2017.01.10
@author  Labeler
@param   unsigned char * pSendBuf : Hand TP로 보낼 Buffer
         int CmdType : Hand Tp로 부터 받은 Command Type
         int nError : Hand TP Check Interlock
*/
void TaskControl::MakeAckDataForHandTp(unsigned char * pSendBuf, int CmdType, int nError)
{
	memset(pSendBuf, 0x00, HANDY_TP_PACKET_SIZE);

	if(nError == NO_ERR)
	{
		pSendBuf[0] = REMOTE_TYPE_RF;
		pSendBuf[1] = CmdType;
		pSendBuf[2] = NO_ERR;
	}
	else
	{
		pSendBuf[0] = REMOTE_TYPE_RF;
		pSendBuf[1] = CmdType;
		pSendBuf[2] = nError;
	}
	OHTMainForm->GetHWSetRemocon()->SendTriger(pSendBuf, HANDY_TP_PACKET_SIZE);
}



//Map MakerFile관련 Update 부분 _ putnig
void TaskControl::StandardMapMakerFileUpdate()
{
	//명령 생성 및 명령 삭제 Map Maker File 상태 확인 필요.
	//flag는 TP에서 명령을 받았을때 Trur처리하고 완료되거나 에러발생시 false 처리해줄것.
    bool bCheckFinish = false;
	if(m_FileMapMakeflag || m_FileTeachingflag)
	{

		ADD_LOG_TASKCONTROL("[StandardMapMakerFileUpdate] Check");
		//Map Maker Load Check
		if(m_FileMapMakeflag)
		{
			UINT nEndNode = 0;
			bool bMapMaker = false;
			if((m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist) &&
			(m_pExecuteInfo->ExecutePositionInfo.Marked) &&
			CheckStatusForCycle())
			{
                Sleep(10);
				bCheckFinish = OHTMainForm->GetMapMakerFileManager_STD()->ProcessingFindMapMakerFileUpdate(m_pExecuteInfo->ExecutePositionInfo.CurrNode, OHTMainForm->GetStationDataManager(),nEndNode,bMapMaker);
				ADD_LOG_TASKCONTROL("[ProcessingFindMapMakerFileUpdate] %d, NID:%d",bCheckFinish , m_pExecuteInfo->ExecutePositionInfo.CurrNode);
				if(bCheckFinish)
				{
				   	MakeMapMakerInfo(nEndNode, bMapMaker);
				}
				else
					m_FileMapMakeflag = false;
			}


		}

		//Teaching Load Check
		if(m_FileTeachingflag)
		{
			//이동 필요 유무 확인 넣을것.
			UINT nTargetStation = 0;
			bool bIsMove = false;
			BYTE cPortType = 0x01;

			if((m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist) &&
				CheckStatusForCycle())
			{
				bCheckFinish = OHTMainForm->GetMapMakerFileManager_STD()->ProcessingFindTeachingFileUpdate(m_pExecuteInfo->ExecutePositionInfo.CurrStation, nTargetStation,bIsMove,cPortType);
				ADD_LOG_TASKCONTROL("[ProcessingFindTeachingFileUpdate] %d %d %d",bCheckFinish,cPortType, nTargetStation);
				if(bCheckFinish)  //티칭 명령 생성(이동유무 확인)
				{
					//Start Node m_pExecuteInfo->ExecutePositionInfo.CurrNode,
					//TargetStation :nTargetStation
					//이동 유무 :  bIsMove
					MakeAutoTeachingInfo(nTargetStation, bIsMove,cPortType);
				}
			}

			m_FileTeachingflag = false;
		}

	}

}

//Map MakerFile 유무 및 Version확인하는 부분_ putnig
//file 유무 및 버전 확인 : 정상 : 0, 버전 X : 1, file Load X : 2
// 1은 Statiuon 2sms teaching
bool  TaskControl::CheckMapMakerFile(int MapCheckType)
{
	bool bReturn = true;

	MAPMAKERFILE_HEADER_STD *pMapMakerheader = NULL; //Station 정보
	FILE_HEADER_STD *pFileheader = NULL; //Station 정보

	pMapMakerheader = OHTMainForm->GetMapMakerFileManager_STD()->GetFileHeader();

	if(MapCheckType == 1)  //Station
		pFileheader = OHTMainForm->GetStationDataManager_STD()->GetFileHeader();
	if(MapCheckType == 2)  //Teahcing
		pFileheader = OHTMainForm->GetTeachingDataManager_STD()->GetFileHeader();

	if((pMapMakerheader !=NULL) && (pFileheader !=NULL))
	{
		//버전 다름
	   if(pMapMakerheader->Version != pFileheader->Version)
		  bReturn = false;

	}
	else
	{
		//파일 없음.
		bReturn = false;
    }


	return bReturn;

}

int TaskControl::MakeAutoTeachingInfo(int TargetStationID, bool bMove, BYTE PortType, bool bTeachingNullType)
{
	int nError = NO_ERR;

    STATION_DATA * pStationData = NULL;
	TEACHING_DATA * pTeachingData = NULL;

	pStationData = OHTMainForm->GetStationDataManager()->Find(TargetStationID);

	if(pStationData == NULL)
	{
		nError = ERR_DATA_STATION_NOEXIST;
		ADD_LOG_TASKCONTROL("[ERR_DATA_STATION_NOEXIST] Station Id: %d", TargetStationID);
	}

	if(nError == NO_ERR)
	{
		if(pStationData->StationType == MARK_TYPE_QR_STATION_LEFT)
			PortType = PORT_TYPE_STB_L;
		else if(pStationData->StationType == MARK_TYPE_QR_STATION_RIGHT)
			PortType = PORT_TYPE_STB_R;

		if(bTeachingNullType == false)
			nError = MakerMapMakerInfo(0, TargetStationID, PortType, CYCLE::TP_PortTeaching, bMove);
		else
			nError = MakerMapMakerInfo(0, TargetStationID, PortType, CYCLE::TP_PortTeaching_NullType, false); //항시 정지상태에서수행할것.
	}
	return nError;


}

int TaskControl::MakeMapMakerInfo(int EndNode, bool bMapMaker)
{
	int nError = NO_ERR;
    //이동관련 내용 구현
	if(bMapMaker)  //Map Maker 주행
	{
		//주행 node -> 주행 Node
		//Start Node m_pExecuteInfo->ExecutePositionInfo.CurrNode, EndNode :nEndNode

		nError = MakerMapMakerInfo(EndNode, 0, 0, CYCLE::TP_RunningLearning);

	}
	else          //일반 주행
	{
		//주행 node -> 주행 Node
		//Start Node m_pExecuteInfo->ExecutePositionInfo.CurrNode, EndNode :nEndNode

		nError = MakerMapMakerInfo(EndNode, 0, 0, CYCLE::MovingToNode);
	   //	nError = MakerMapMakerInfo(EndNode, 0, 0, CYCLE::TP_RunningLearning);

	}

	return nError;


}


int TaskControl::MakerMapMakerInfo(int nEndNode, int nStationNode, int nPortType, int iCycle, bool bMove)
{
    int nError = NO_ERR;
	bool bSearchResult = false;

	int nStationID = nStationNode;
	int nEndNodeID = nEndNode;
	int nStartNodeID = 0;
    STATION_DATA * pStationData = NULL;
    TEACHING_DATA * pTeachingData = NULL;

	int iTemp=0, iDirection, iCount =0, iMarkType =0;
	unsigned int iPathList[500];
	unsigned char cHigh, cMid, cLow;
	AnsiString strMachineID = "TP001";

    int iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
    PKT_JOB_ORDER_ID_121	SEND_PACKET;
    memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어


	if(m_pExecuteInfo->ExecutePositionInfo.Marked == true)
	{
		// Station으로 이동 시에는, iStopNode 매개변수에, Target Station 정보가 담김

		nStartNodeID = m_pExecuteInfo->ExecutePositionInfo.CurrNode;

		if(nStationID != 0)  //이적재
		{
            if(false == bMove)
            {
                if(nError == NO_ERR)
                {
                    nEndNodeID = 0;
                    pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(nStationNode, nPortType);

                    if(pTeachingData == NULL)
                    {
                        nError = ERR_DATA_TEACHING_NOEXIST;
                        ADD_LOG_TASKCONTROL("[ERR_DATA_TEACHING_NOEXIST] Station Id: %d, PortType : %d", nStationNode, nPortType);
                    }
                }
            }
            else
            {
				pStationData = OHTMainForm->GetStationDataManager()->Find(nStationID);
	
				if(pStationData == NULL)
	            {
	                nError = ERR_DATA_STATION_NOEXIST;
					ADD_LOG_TASKCONTROL("[ERR_DATA_STATION_NOEXIST] Station Id: %d", nStationNode);
	            }

	            if(nError == NO_ERR)
            	{
					nEndNodeID = pStationData->ParentNodeID;

					if(pStationData->StationType == MARK_TYPE_QR_STATION_LEFT)
						nPortType = PORT_TYPE_STB_L;
					else if(pStationData->StationType == MARK_TYPE_QR_STATION_RIGHT)
						nPortType = PORT_TYPE_STB_R;

					pTeachingData = OHTMainForm->GetTeachingDataManager()->Find(nStationID, nPortType);

					if(pTeachingData == NULL)
                	{
                   	 	nError = ERR_DATA_TEACHING_NOEXIST;
						ADD_LOG_TASKCONTROL("[ERR_DATA_TEACHING_NOEXIST] Station Id: %d, PortType : %d", nStationNode, nPortType);
                	}
            	}
         	}

		}
    }
	else
	{
		nError = MAP_MOVE_ERR_NOT_MARKED;
	}



    if(nError == NO_ERR)
	{
		//Auto Teaching
		if((iCycle == (int)CYCLE::TP_PortTeaching) || (iCycle == (int)CYCLE::MovingToStation) || (iCycle == (int)CYCLE::TP_PortTeaching_NullType))
		{
			if(nStationNode == 0)
			{
				nError = MAP_MOVE_ERR_NOT_STATION_INFO;
			}
		}
		//Map Maker
		else if((iCycle == (int)CYCLE::MovingToNode) ||  (iCycle == (int)CYCLE::TP_RunningLearning))
		{
			if(nEndNode == 0)
			{
                nError = MAP_MOVE_ERR_NOT_STATION_INFO;
			}

		}
	}

    if(nError == NO_ERR)
    {
		// Start Node와 Target Node가 다를 때에만, Path Search 수행

		if(true == bMove)
			bSearchResult = ManualMovePathSearch(nStartNodeID, nEndNodeID, iDirection, iCount, iPathList);



		ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] StartNID : %d,  EndNID : %d, StopStation : %d, Dir : %d Count : %d  Path[0] : %d  bSearchResult : %d Error%d",
				nStartNodeID, nEndNodeID, nStationID, iDirection, iCount, iPathList[0], bSearchResult,nError);

		if((bSearchResult == true) || (false == bMove))
        {
    		ADD_LOG_TASKCONTROL("[MOVE_BY_MAP]  Make Packet....");

    		// ===============================
			//		Step 1. make packet
    		// ===============================

			// (2) 고정 정보를 채워 넣는다
			// --- Header
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
			memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

			// --- Tag
			SEND_PACKET.TAG.Value = TagID::JOB_ORDER_TP;	// 작업지시

			// --- Status
			// 상태지시 - 기동 ON
			// SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;
			// 정지지시, 정지요인, 유지보수지시, E/M지시, 데이터 전송지시  - 설정 생략

    		// =============================== Body ===============================
            if(m_pOHTMainStatus->CurCmdID >= 9)
            {
    			SEND_PACKET.BODY.CmdID = 1;	// 지시 ID
            }
            else
            {
    			SEND_PACKET.BODY.CmdID = m_pOHTMainStatus->CurCmdID + 1;	// 지시 ID
            }

			iTemp = ORDERDIV::JobOrder; 					// 지시구분 - 작업지시
			memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// 지시구분

			// 이적재 Buffer 위치
			SEND_PACKET.BODY.BufferType.Buffer_1 = 1;			// 제1 수하대

			iTemp = iCycle;
			// iTemp = CYCLE::MovingToStation; 				// 지시사이클 - 이동 사이클
			memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);

			// 우선도
			RIV tmpRv;
			tmpRv.Riv_Value = 64;							// 7비트니까
			tmpRv.Hotlot = 0;								// Hotlot - off
			memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

			if(iDirection == DefaultDirection::DIR_LEFT)
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
			else
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;

            if(nStationID != 0)
            {
				if(m_pExecuteInfo->ExecutePositionInfo.CurrStation == nStationID)
                {
        			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 0;		// Driving flag
                }
                else
                {
					SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag
                }
            }
            else
            {
				if(m_pExecuteInfo->ExecutePositionInfo.CurrNode  == nEndNodeID)
				{
        			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 0;		// Driving flag
                }
                else
                {
        			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag
                }
            }

			// 시작 NODE
			Int2Uchar3(nStartNodeID, cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

			// 목적 NODE
			Int2Uchar3(nEndNodeID, cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;


			if(nStationID != 0)
			{
				Int2Uchar3(nStationID, cHigh, cMid, cLow);

				// 목적지 STATION
				SEND_PACKET.BODY.StopStationID[HIGH_BYTE]   = cHigh;
				SEND_PACKET.BODY.StopStationID[MID_BYTE]    = cMid;
				SEND_PACKET.BODY.StopStationID[LOW_BYTE]    = cLow;


				if((true == bMove)  || (pStationData != NULL))
				{
					SEND_PACKET.BODY.StationData_MarkType 		= pStationData->StationType;
					SEND_PACKET.BODY.StationData_Offset 		= pStationData->StationOffset;
					SEND_PACKET.BODY.StationData_ParentNodeID 	= pStationData->ParentNodeID;
					SEND_PACKET.BODY.StationData_NextNodeID 	= pStationData->NextNodeID;
				}
					SEND_PACKET.BODY.StationData_StationID 		= nStationID;


				if(pTeachingData != NULL)
				{
                    SEND_PACKET.BODY.TeachingData_PortType 	= pTeachingData->PortType;
                    SEND_PACKET.BODY.TeachingData_Hoist	 	= pTeachingData->Hoist;
                    SEND_PACKET.BODY.TeachingData_Shift 	= pTeachingData->Shift;
                    SEND_PACKET.BODY.TeachingData_Rotate 	= pTeachingData->Rotate;

                    SEND_PACKET.BODY.TeachingData_StationID		= pTeachingData->StationID;
                    SEND_PACKET.BODY.TeachingData_PIOType		= pTeachingData->PIOType;
                    SEND_PACKET.BODY.TeachingData_PIODirect		= pTeachingData->PIODirection;
                    SEND_PACKET.BODY.TeachingData_PIOID			= pTeachingData->PIOID;

                    SEND_PACKET.BODY.TeachingData_PIOChannel	= pTeachingData->PIOChannel;
                    SEND_PACKET.BODY.TeachingData_PIOTimeLevel	= pTeachingData->PIOTimeLevel;
                    SEND_PACKET.BODY.TeachingData_AutoRecovery	= pTeachingData->TransOption.Info.AutoRecovery;
                    SEND_PACKET.BODY.TeachingData_HandDetectEQ  = pTeachingData->TransOption.Info.HandDetectEQ;
                    SEND_PACKET.BODY.TeachingData_STBOffset		= pTeachingData->STBOffSet;
                    SEND_PACKET.BODY.TeachingData_LookDownLevel	= pTeachingData->LookDownLevel;
					SEND_PACKET.BODY.TeachingData_Reserved[0]   = 0; 	// not use
				}

				//Auto Teahcing일 경우 Hoist, Shift값은 PortType에  따라 초기값 분류해 줄것.


			}else
			{
				// STOP NODE
				Int2Uchar3(nEndNodeID, cHigh, cMid, cLow);
				SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
				SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
				SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;
			}

			/* 아래는 Map Info */
			SEND_PACKET.BODY.ChangeNode.Map_Num			= 0x01;	// 1 floor
			SEND_PACKET.BODY.AutoRecovery				= 0x00;	// 지정 없음 (CLW가 CLW의 파라미터 설정에 따라 독자적으로 판단한다.)
			SEND_PACKET.BODY.StationData_Reserved[0]	= 0;	// not use


    		// =============================== Path ===============================
			SEND_PACKET.BODY.PathData_ExtraNum = iCount;

			for(int i = 0; i < iCount; i++)
			{
				ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] Count : %d  Path : %d", i, iPathList[i]);

				Int2Uchar3(iPathList[i], cHigh, cMid, cLow);
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
			}
        }
        else
        {
            nError = MAP_MOVE_ERR_MAP_SEARCH_FAIL;
        }
    }

    if(nError == NO_ERR)
    {
        // ===============================
        //		Step 2. internal process
        // ===============================
        bool bCheck = false;
        bool bCheckInsertTaskList = false;
        int  QuickCmd = 0;

        PKT_ACK_OHT2OCS RtnACKPkt;
        PKT_OHT2TP_RESPONSE RtnACKPktPC;

        PKT_TASKCMDDATA pTempCmdPktData;
        memset(&pTempCmdPktData, 0x00, sizeof(PKT_TASKCMDDATA));
        pTempCmdPktData.CategoryDiv = CATEGORY_TERTIARY;

        bCheck = CheckCmdJobOrder((PKT_OCS2OHT*)&SEND_PACKET, &RtnACKPkt,&pTempCmdPktData.DetailTagSort, QuickCmd, pTempCmdPktData.CategoryDiv);

        ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] Step 2. internal process - bCheck : %d %d", bCheck,SEND_PACKET.BODY.TeachingData_STBOffset);

        // ===============================
        //		Step 3. insert command
        // ===============================
        // Step.3 Task List에 접수 가능 할때 접수하는 부분 처리 .
        if(bCheck)
        {
            ADD_LOG_TASKCONTROL("[MOVE_BY_MAP] Step 3. Insert command..");

            memmove(&(pTempCmdPktData.PKTDATA), &SEND_PACKET, sizeof(PKT_OCS2OHT));
            // TASK CMD LIst에 추가될 경우에 여기 추가
            // 순서에 맞게 저장할 것.
			m_pCmdManager->MakeTaskList_Standard(&pTempCmdPktData);
		}
    }

	return nError;
}

int TaskControl::Load_AutoTeaching_DrvingData(UINT CurStationID, BYTE PortType)
{
	int nResult = 0;
	int nTmpValue = 0;

	char strStationID[20] = {0,};
	char strPortType[20] = {0,};

	sprintf(strStationID, "%u",CurStationID);
	sprintf(strPortType, "%d",PortType);


	LIniFile *pIniFile;
    pIniFile = NULL;
	pIniFile = new LIniFile("DataFiles\\AutoTeachingData.txt");

    if(!pIniFile->GetIniFileExist())
    {
        delete pIniFile;
        pIniFile = NULL;
    }

	if(pIniFile != NULL)
	{

		pIniFile->ReadIntegerMinMaxCheck(strStationID, strPortType, 0, -50, 50,&nTmpValue);
		nResult = nTmpValue;
		delete pIniFile;
	}

	return  nResult;
 }


bool TaskControl::IsBeforeAutoCheckPassPermit(bool bStationTag,int CurrNode, int NextNode)
{
	bool breturn =false;

	PKT_OCS2OHT* pRcvData = NULL;
	DATA_PASS * pPasspermit; // Command를 위한 포인터 변수
	int nPassPermitCount = m_pPassPermit->GetPassPermitCount();

	int nTempStopNode=0, nTempResetNode=0;

	if(bStationTag == true)
	{
		int FlowControlIndex = OHTMainForm->GetMap()->FindNodeIndexFromFlowControl(CurrNode,NextNode,OHTMainForm->GetFlowControl_STD());

		if(FlowControlIndex != INDEX_NONE)
			return false;
	}

	 //// 통과허가 가지고있는지 확인하는 부분(가지고 있으면 반납유무 확인
	if(nPassPermitCount != LISTCOUNT_EMPTY)
	{
		ADD_LOG_TASKCONTROL("PassPermitCount: %d",nPassPermitCount);

		if(nPassPermitCount == 1)
		{
			if(bStationTag == true)
			{
				//현재 가지고있어야될 Stop 정보일 경우만
				pPasspermit = (DATA_PASS *)m_pPassPermit->ReferFirstPassPermit();

				for (int i = 0; i < 3; i++)
				{
					// Node 비트연산   puting
					int Temp1 = pPasspermit->PassInfo.StopNodeID[i];
					int Temp2 = pPasspermit->PassInfo.ResetNodeID_Main[i];


					if (HIGH_BYTE == 0)
					{
						nTempStopNode += Temp1 << ((2 - i) * 8);
						nTempResetNode +=	Temp2 << ((2 - i) * 8);
					}
					else
					{
						nTempStopNode += Temp1 << (i * 8);
						nTempResetNode +=	Temp2 << (i * 8);

					}
				}

				//가지고 있어야되는 정보일 경우만.
				if((nTempStopNode == CurrNode) && (nTempResetNode==NextNode))
				{
					return true;
				}
			}
		}
		else   //1개가 아닐경우는 문제있는것으로 간주하고 모두 클리어
		{
			m_pPassPermit->InitPassPermitList();
			return true;
		}


	}
	else
	   return true;


   if(breturn == false)
        CheckCmdPassOpen_fromTP_and_AutoChange(AUTOCHANGE_TYPE, pRcvData, 0);

   return breturn;
}


// Map Make후 진행결과를 OHT에서 TP로 전송
void TaskControl::Send_MapMakeResult(AnsiString strLog)
{
	int iReturn;
	int iPacketSize = sizeof(PKT_OHT2TP_LOG_DATA);
	PKT_OHT2TP_LOG_DATA SEND_PACKET;

	memset(&SEND_PACKET, 0, iPacketSize);	 
	
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	SEND_PACKET.TAG.Value = TagID::CMD_MAP_MAKE_RESULT;	

	memcpy(SEND_PACKET.BODY.Log, strLog.c_str(), strLog.Length());	
	
	iReturn = m_pUDPCommunication->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);
}

void TaskControl::GetServoAlarmCode(int errorCode)

{
	short axis;
	short monitorParam;
	long alarmCode;

	// Current Alarm State
	monitorParam = CURRUNT_ALARM_STATE;
	//ReadMonitorData(WORD wAddress, WORD wMonitorPara, long* lpReadData)
	if(errorCode == ERR_AXIS1_F_AMP_FAULT || errorCode == ERR_AXIS1_F_ABORT_EVENT || errorCode ==ERR_AXIS1_F_ERROR_LIMIT ) axis = 1;
	else if(errorCode == ERR_AXIS1_R_AMP_FAULT || errorCode == ERR_AXIS1_R_ABORT_EVENT|| errorCode ==ERR_AXIS1_R_ERROR_LIMIT) axis = 2;
	else if(errorCode == ERR_AXIS2_AMP_FAULT || errorCode == ERR_AXIS2_ABORT_EVENT) axis = 3;
	else if(errorCode == ERR_AXIS3_AMP_FAULT || errorCode == ERR_AXIS3_ABORT_EVENT) axis = 4;
	else return;

	if(axis ==1 || axis ==2)
	{
        m_pServoControl->ReadMonitorData(1, monitorParam, &alarmCode);
		m_pServoControl->DisplayServoMonitorResult(CURRUNT_ALARM_STATE, alarmCode);

		m_pServoControl->ReadMonitorData(2, monitorParam, &alarmCode);
        m_pServoControl->DisplayServoMonitorResult(CURRUNT_ALARM_STATE, alarmCode);
	}
	else
	{
        m_pServoControl->ReadMonitorData(axis, monitorParam, &alarmCode);
        m_pServoControl->DisplayServoMonitorResult(CURRUNT_ALARM_STATE, alarmCode);
	}
}

 bool TaskControl::GetNodeTypeSlop()
 {
	 bool bretrun =false;

 	 NODE_INFO	*pNODE_INFO = NULL; // Node 정보
	 int		nNodeIndex			= 0;		// Node의 Index


	 if(m_pExecuteInfo->ExecutePositionInfo.CurrNode == 0)
		 return bretrun;

	 nNodeIndex = OHTMainForm->GetMap()->FindNodeIndex(m_pExecuteInfo->ExecutePositionInfo.CurrNode);   ///< 출발 노드 Index
	 pNODE_INFO = &(OHTMainForm->GetMap()->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

	 if(pNODE_INFO != NULL)
	 {
		if(pNODE_INFO->InOutInfo ==SLOPE_NTYPE)
			bretrun =true;
	 }

	 return bretrun;
 }

 /**
@brief   물류 IoT Hub Socket Client Connect 함수
@date    2016.08.03
@author
*/
void TaskControl::IOTHubSocketClientConnect()
{
    // 연결 flag true로 변경
	m_ConnectedIOTHub = true;
	m_pOHTMainStatus->ConnectedIOTHub = true;

	if(gm_LogManager->Get_LogFolder())
	{
		ADD_IOTCOMM_LOG("[IoT Hub Connect Complete], Client Count:%d", m_IOTHubServerSock->GetClientCount() - 1 );
	}
	// OCS Socket 정보 입력
	if(m_IOTHubServerSock->GetClientCount() > 2) // 기본 서버 1개 클라이언트 1개
    {
		// 먼저 연결된 연결을 강제로 해제한다.
		m_IOTHubServerSock->ClientRemove(0); //  먼저 연결된거 제거
		m_IOTHubSocket = (SOCKET)m_IOTHubServerSock->GetClientInfo(0);
    }
	else
    {
		m_IOTHubSocket = (SOCKET)m_IOTHubServerSock->GetClientInfo(0);
	}



}
// ---------------------------------------------------------------------------

/**
@brief   IOTHubSocketClientDisconnect
@date    2016.08.03
@author  puting
*/
void TaskControl::IOTHubSocketClientDisconnect()
{
	m_ConnectedIOTHub = false;
	m_pOHTMainStatus->ConnectedIOTHub = false;


	if(gm_LogManager->Get_LogFolder())
	{

		ADD_IOTCOMM_LOG("[IoT Hub Disconnect Complete], Client Count:%d", m_IOTHubServerSock->GetClientCount() - 1 );
   	}

	if(m_IOTHubServerSock->GetClientCount() > 1)
    {
		m_IOTHubSocket = (SOCKET)m_IOTHubServerSock->GetClientInfo(0);
    }
    else
    {
		m_IOTHubSocket = NULL;
		if(m_IOTHubServerSock->BufferPointer != NULL);
		{
			delete[] m_IOTHubServerSock->BufferPointer;
			m_IOTHubServerSock->BufferPointer = NULL;
        }
		m_IOTHubServerSock->nRestLenth = 0;
	}
}
// ---------------------------------------------------------------------------

/**
@brief   IoT Hub Socket Read 함수
@date    2016.08.04
@author
*/
int TaskControl::IOTHubSocketClientRead(short *nStateIot)
{

	int nError = 0;
	int nRecvLen = 0; // 수신한 bytes
	int SndLen = 0; // 송신할 bytes 수
	char RecvBuf[LEN_TLV_BUFFER] = {0,}; // 수신한 내용이 저장되는 버퍼
	char SendBuf[LEN_TLV_BUFFER] = {0,}; // 송신할 내용이 저장되는 버퍼


	char *pTmpBuffer;
	int nCount = 0;
	int TotalLen = 0;
	int nsumcheck = 0;

	bool bNotFrist = false;
	bool nFinishFlag = false;
	ACK_DATA ACKData; // Ack할 정보를 가진 구조체
	
	//AOHC-203 IoT정보 데이터 취득 및 활용 -> 멤버 변수로 이동됨
	//IOT_HEADER IotHeader;
	//IOT_DEFECT_SIGNAL_COUNT *DefSignalCount;
	int *nBuffOffset = 0;
//	AnsiString strIotLog = "";

	// ACK_DATA 구조체 초기화
	memset(&ACKData, 0x00, sizeof(ACK_DATA));


	if(m_IOTHubServerSock == NULL || m_IOTHubSocket == NULL)
		return 1;

	//Step 0. TLV Socket을 Read하기 위해 호출
	m_IOTHubServerSock->Read(RecvBuf, &nRecvLen, &nCount);

	if(nCount <= 0 || nRecvLen <=0 || nRecvLen > LEN_TLV_BUFFER)
	{
		return 2;// 내용물이 없을 경우  nRecvLen=0;
	}

	//Step 1.PreBuff에 Data가 남아 있을 경우와 없을 경우 분리(초기 Data값 삽입)
	if(m_IOTHubServerSock->BufferPointer != NULL && m_IOTHubServerSock->nRestLenth > 0)
	{
		TotalLen = m_IOTHubServerSock->nRestLenth + nRecvLen;
		pTmpBuffer = new char[TotalLen];
		memset(pTmpBuffer, 0x00, TotalLen);
		memmove(pTmpBuffer, m_IOTHubServerSock->BufferPointer, m_IOTHubServerSock->nRestLenth);
		memmove(pTmpBuffer + m_IOTHubServerSock->nRestLenth, RecvBuf, nRecvLen);
	}
	else
	{
		TotalLen = nRecvLen;
		pTmpBuffer = new char[TotalLen];
		memset(pTmpBuffer, 0x00, TotalLen);
		memmove(pTmpBuffer, RecvBuf, TotalLen);
	}

	// IoT 수신메시지 로그 저장
/*	strIotLog.sprintf("%d, %d, %d, %d", *pTmpBuffer, *RecvBuf, nCount, nRecvLen);
	ADD_LOG_IOTHUB(strIotLog.c_str());       */

	//Step 1-1. pTmpBuffer안에 여러개의 TLV가 있을 경우 반복적으로 실행하기 위한 반복구간
	while(!nFinishFlag)
	{
		int UsedLen = 0;
		int chkroof = 0;
		char pTLV[LEN_TLV_BUFFER] = {0,};


		if(bNotFrist == true)
		{
			TotalLen = m_IOTHubServerSock->nRestLenth;
			pTmpBuffer = new char[TotalLen];
			memset(pTmpBuffer, 0x00, TotalLen);
			memmove(pTmpBuffer, m_IOTHubServerSock->BufferPointer, TotalLen);
		}

		if(m_IOTHubServerSock->BufferPointer != NULL)
		{
			delete[]m_IOTHubServerSock->BufferPointer;
			m_IOTHubServerSock->BufferPointer = NULL;
		}
		m_IOTHubServerSock->nRestLenth =0;

		//  Read Header
		//IotHeader = m_IotManager->GetIotHeader(pTmpBuffer, &UsedLen);
		memset(&IotHeader, 0, sizeof(IotHeader));
		//AOHC-203 IoT정보 데이터 취득 및 활용
		// 14바이트 헤더UP 정보만 날라온 경우
		if(TotalLen==sizeof(IOT_HEADER_UP))
		{
			memcpy(&IotHeader.IotHeaderUp, pTmpBuffer, sizeof(IOT_HEADER_UP)); // 헤더UP 14바이트만 복사
			//AOHC-203 IoT정보 데이터 취득 및 활용
			ADD_IOTCOMM_LOG("[HeaderUp] %d/%d/%d/%d/%d/%d",	IotHeader.IotHeaderUp.nIotID
														, IotHeader.IotHeaderUp.nMachineID
														, IotHeader.IotHeaderUp.cMachineType1
														, IotHeader.IotHeaderUp.cMachineType2
														, IotHeader.IotHeaderUp.nCommandID
														, IotHeader.IotHeaderUp.nIotHubState
														);
		}
		// 전체 정보 다 날라온 경우. Defect 발생 했을 떄
		else if(TotalLen == sizeof(IOT_HEADER))
		{
			memcpy(&IotHeader, pTmpBuffer, sizeof(IOT_HEADER));

			ADD_IOTCOMM_LOG("[Total] %d/%d/%d/%d/%d/%d/xA/%g/yA/%g/zA/%g/gR/%g/gP/%g/gY/%g/xD/%g/yD/%g/t1/%g/t2/%g/t3/%g/t4/%g"
									, IotHeader.IotHeaderUp.nIotID
									, IotHeader.IotHeaderUp.nMachineID
									, IotHeader.IotHeaderUp.cMachineType1
									, IotHeader.IotHeaderUp.cMachineType2
									, IotHeader.IotHeaderUp.nCommandID
									, IotHeader.IotHeaderUp.nIotHubState
									, IotHeader.fXAxisACC
									, IotHeader.fYAxisACC
									, IotHeader.fZAxisACC
									, IotHeader.fGyroRoll
									, IotHeader.fGyroPitch
									, IotHeader.fGyroYaw
									, IotHeader.fXAxisDegree
									, IotHeader.fYAxisDegree
									, IotHeader.f1AxisTorque
									, IotHeader.f2AxisTorque
									, IotHeader.f3AxisTorque
									, IotHeader.f4AxisTorque
							);

		}
		else
		{
			ADD_IOTCOMM_LOG("Wrong Byte Read!! %d", TotalLen);
		}

		//AOHC-238 Memory Leak 개선
		//SonarQube Memory Leakage 검출
		if(IotHeader.IotHeaderUp.nIotHubState != 0)
		{
			*nStateIot = IotHeader.IotHeaderUp.nIotHubState;
			 delete[]pTmpBuffer;
			pTmpBuffer = NULL;
			return 3;
		}

		if(IotHeader.IotHeaderUp.nCommandID == COMMAND_REQINFO_R)    //
		{
			SndLen = m_IotManager->MakeOHTInformation(SendBuf, IotHeader, COMMAND_REQINFO_S);
			ADD_IOTCOMM_LOG("Send OHT Inf: %s[%d]", SendBuf, SndLen);


			if(gm_LogManager->Get_LogFolder())
			{
				AnsiString strTmp = "";
				AnsiString strLog = "";

				// -------------------------------------------
				//strLog = "[AckSub]";
				for(int i = 0; i < SndLen; i++)
				{
					strTmp.sprintf("%02X", SendBuf[i]);
					strLog += strTmp;
				}

				ADD_IOTCOMM_LOG(strLog.c_str());
			}
			// -------------------------------------------

			m_IOTHubServerSock->Write(SendBuf, SndLen, m_IOTHubSocket);
		}
		else if(IotHeader.IotHeaderUp.nCommandID == COMMAND_WARNDEFECT_R)   //AOHC-203 IoT정보 데이터 취득 및 활용
		{
			DefSignalCount = m_IotManager->GetIotDefectSignal(pTmpBuffer, UsedLen);
			ADD_IOTCOMM_LOG("OHT Defect Count: Shock: %d, Particle: %d", DefSignalCount->nCountShockDefect, DefSignalCount->nCountParticleDefect);


			if(DefSignalCount->nCountShockDefect > 3)
			{
				; // action
				m_IotManager->InitializeDefectSigCnt();
			}
			else if(DefSignalCount->nCountParticleDefect > 3)
			{
				; // action
				m_IotManager->InitializeDefectSigCnt();
			}
            else if(DefSignalCount->bDetectSlope == true)
            {
                // 현재 위치 : 경사로
                ADD_IOTCOMM_LOG("현재 위치 : 경사로 구간");

                // 메인 변수에 Update 코드 작성 필요
                bSlope = true;

                OHTMainForm->pnSlope->Caption = "Slope";
                OHTMainForm->pnSlope->Color = clLime;
            }
            else if(DefSignalCount->bDetectSlope == false)
            {
                // 현재 위치 : 평지
                ADD_IOTCOMM_LOG("현재 위치 : 평지 구간");

                // 메인 변수에 Update 코드 작성 필요
                bSlope = false;

                OHTMainForm->pnSlope->Caption = "normal";
                OHTMainForm->pnSlope->Color = clGray;
            }

			// Make and Send a Message
			SndLen = m_IotManager->MakeOHTInformation(SendBuf, IotHeader, COMMAND_WARNDEFECT_S);

			if(gm_LogManager->Get_LogFolder())
			{
				AnsiString strTmp = "";
				AnsiString strLog = "";

				// -------------------------------------------
				//strLog = "[AckSub]";
				for(int i = 0; i < SndLen; i++)
				{
					strTmp.sprintf("%02X", SendBuf[i]);
					strLog += strTmp;
				}

			 //	ADD_IOTCOMM_LOG(strLog.c_str());        20190419 로그 용량 감소위해 주석
			}
			// [END] 응답 Data 통신 로그
			// -------------------------------------------

			m_IOTHubServerSock->Write(SendBuf, SndLen, m_IOTHubSocket);
		}



		// 다른 명령이 남아있나 확인
		if(m_IOTHubServerSock->nRestLenth <= 0 || UsedLen <=0)
		{
			if(m_IOTHubServerSock->nRestLenth < 0 || UsedLen <=0)
			{    //Warning : [nIotHubRestLenth:%d][UsedLen:%d][TotalLen:%d] !! 아래 로그 남긴것
				ADD_IOTCOMM_LOG("%d/%d/%d/", m_IOTHubServerSock->nRestLenth,UsedLen,TotalLen);
			}

			if(m_IOTHubServerSock->BufferPointer != NULL)
			{
				delete[]m_IOTHubServerSock->BufferPointer;
				m_IOTHubServerSock->BufferPointer = NULL;
			}
			m_IOTHubServerSock->nRestLenth =0;

			delete[]pTmpBuffer;
			pTmpBuffer = NULL;

			nFinishFlag = true;
			break;
		}
		else
		{
				// Step 4. 1개의 TLV를 제외 후 저장.
			m_IOTHubServerSock->nRestLenth = TotalLen - UsedLen;
			m_IOTHubServerSock->BufferPointer = new char[m_IOTHubServerSock->nRestLenth];
			memset(m_IOTHubServerSock->BufferPointer, 0x00, m_IOTHubServerSock->nRestLenth);
			memmove(m_IOTHubServerSock->BufferPointer, pTmpBuffer + UsedLen, m_IOTHubServerSock->nRestLenth);

			delete[]pTmpBuffer;
			pTmpBuffer = NULL;
		}


		bNotFrist = true;
		nsumcheck++;
		if(nsumcheck == 820)
		{
			ADD_IOTCOMM_LOG("Warning : IotHub While Loop [nIotHubRestLenth:%d][UsedLen:%d][TotalLen:%d]", m_IOTHubServerSock->nRestLenth,UsedLen,TotalLen);
			if(m_IOTHubServerSock->BufferPointer != NULL)
			{
				delete[]m_IOTHubServerSock->BufferPointer;
				m_IOTHubServerSock->BufferPointer = NULL;
			}
			m_IOTHubServerSock->nRestLenth =0;

			delete[]pTmpBuffer;
			pTmpBuffer = NULL;

			nFinishFlag = true;
			break;
		}

		Sleep(1);
	}

	return 0;
}
//---------------------------------------------------------------------------


/**
@brief   Socket을 Open하기 위한 함수
@author  puting
@date    2013.05.23
*/
void TaskControl::ServerSocketOpen()
{

	bool chkIOTHubSocketOpen = false;

	 // Monitor Server
	if(m_IOTHubServerSock != NULL)
	{
		chkIOTHubSocketOpen = m_IOTHubServerSock->SocketOpen(PORT_IOTHUB);
	}
	else
    {
        m_IOTHubServerSock = new ServerSocket();
        if(m_IOTHubServerSock != NULL)
        {
            chkIOTHubSocketOpen = m_IOTHubServerSock->SocketOpen(PORT_IOTHUB);
        }
		else  //AOHC-238 Memory Leak 개선
        {
			delete m_IOTHubServerSock;
            m_IOTHubServerSock = NULL;
			ADD_LOG_TASKCONTROL("IOT Socket Create Fail_1");
        }
	}

    if(chkIOTHubSocketOpen == false)
    {
		delete m_IOTHubServerSock;
        m_IOTHubServerSock = NULL;
		   ADD_LOG_TASKCONTROL("IOT Socket Open Fail_2");
    }

}

/**
@brief   Socket을 Close하기 위한 함수
@author  puting
@date    2013.05.23
*/
void TaskControl::ServerSocketClose()
{

	//2016.08.03 물류 IoT Hub
	if(m_IOTHubServerSock != NULL)
	{
		m_IOTHubServerSock->SocketClose();

		delete m_IOTHubServerSock;
		m_IOTHubServerSock = NULL;
	}
}

/**
@brief   TP Update하기 위한 함수
@author  puting
@date    2013.05.23
*/
void TaskControl::IOTHubSocketClientUpdate()
{
	int nStateRead = 2; // 0: normal, 1: null socket, 2: comm fail
	static int IoTCommState = 0;     // 0: disconnect, 1: connect, 2: Comm fail over setting interval(1min)
	static DWORD IotCommPreTime = timeGetTime();
	static bool bWriteCommfailLog = false;
	static bool bIotHubFail = false;
	short nErrIot;

//	AnsiString strLog = "";

	if(m_defualtparam->IsIOTHub ==true)
	{
		if(m_IOTHubServerSock != NULL)
		{
			m_IOTHubServerSock->Update();

			// Client 접속 Check
			if(m_IOTHubServerSock->IsClientConnect() == true)
			{
				IOTHubSocketClientConnect();

				IoTCommState = 1;
				OHTMainForm->pnIotHubState->Caption = "OK";
				OHTMainForm->pnIotHubState->Color = clLime;
				ADD_IOTCOMM_LOG("Connect IoT Hub!");
                IotCommPreTime = timeGetTime();   // Check Commfail time, if > 1min, then action
			}

			if(m_IOTHubServerSock->IsClientDisConnect() == true)
			{
				IOTHubSocketClientDisconnect();

				IoTCommState = 0;

                OHTMainForm->pnIotHubState->Caption = "NC";
			    OHTMainForm->pnIotHubState->Color = clGray;

				ADD_IOTCOMM_LOG("Disconnect IoT Hub!");

				return;
			}

			// Read Data Check
			bool bReadOK = m_IOTHubServerSock->IsClientReadData();
			if(bReadOK)
			{
				// IOT 데이터 Reading
				nStateRead = IOTHubSocketClientRead(&nErrIot);
			}

			// IOT 정보 저장 --> m_IoTData  동기화 처리
			EnterCriticalSection(&g_csIOT);

			if(bReadOK)
			{	memcpy(&m_IoTData.Info1, &IotHeader, sizeof(IOT_HEADER));

				if(DefSignalCount != NULL)
					memcpy(&m_IoTData.Info2, DefSignalCount, sizeof(IOT_DEFECT_SIGNAL_COUNT));
				else
					memset(&m_IoTData.Info2, 0, sizeof(IOT_DEFECT_SIGNAL_COUNT));
			}
			else
				memset(&m_IoTData, 0, sizeof(m_IoTData));

			LeaveCriticalSection(&g_csIOT);



			// 이슈 상황 처리
			if(nStateRead == 2)      // 연결 후 통신이 없으면 상태 표시
			{
				if(m_IotManager->PreventOverflowGetTimeMs(IotCommPreTime) > 60000)
				{
					if(bWriteCommfailLog == false)
					{
						OHTMainForm->pnIotHubState->Caption = "Cfail";
						OHTMainForm->pnIotHubState->Color = clYellow;
						ADD_IOTCOMM_LOG("Commfail: IoT Hub don't reply over 1min!");
                        bWriteCommfailLog = true;
						IotCommPreTime = timeGetTime();   // Check Commfail time, if > 1min, then action
					}

				}
			}
			else if(nStateRead == 3)     // Iot 보드 에러 상태
			{
				if(bIotHubFail == false)
				{
                    OHTMainForm->pnIotHubState->Caption = "Error";
				    OHTMainForm->pnIotHubState->Color = clLime;
					ADD_IOTCOMM_LOG("IoT Hub Error(%d)!", nErrIot);

					bIotHubFail = true;
				}
			}
			else
			{
				IotCommPreTime = timeGetTime();   // Check Commfail time, if > 1min, then action
				if((bWriteCommfailLog == true) || (bIotHubFail == true))
				{
                    OHTMainForm->pnIotHubState->Caption = "OK";
				    OHTMainForm->pnIotHubState->Color = clLime;
					ADD_IOTCOMM_LOG("Start IoT Hub Comm!");
					bWriteCommfailLog = false;
					bIotHubFail = false;
				}
			}
		}
		else
		{
			IoTCommState = 0;
		}
	}
}



void TaskControl::ExecuteResetSteps()
{
	if(m_StepAutoChangeReset.bAllDone)
		return;

	if((CheckError() == NO_ERR))
	{
		m_StepAutoChangeReset.bAllDone = true;
		m_StepAutoChangeReset.nRetryCnt = 0;
		ADD_LOG_TASKCONTROL("ExecuteResetSteps() : Reset Complete!");
	}
	else
	{
		if(m_StepAutoChangeReset.nRetryCnt < 10)
		{		
			ADD_LOG_TASKCONTROL("ExecuteResetSteps() : Reset Retry : %d", m_StepAutoChangeReset.nRetryCnt);		
			m_StepAutoChangeReset.nRetryCnt++;
			
			if (CheckStatusForReset() && (GetCurrentMode() == MODE_MANUAL) && (m_pServoControl->Fn009ing()==false) )
			{

				// 2017-01-15, Add by Labeler
				// OCS, TP, CID Pause 걸린 상태에서 Error 발생 할 경우, Reset 시 Pause 해제 처리 추가
				if((m_pExecuteInfo->bTpPause != false) || (m_pExecuteInfo->bOcsPause != false) || (m_pHWSet->m_pCID->GetCIDPause() != false))
				{
					m_pExecuteInfo->bTpPause = false;
					m_pExecuteInfo->bOcsPause = false;
					m_pHWSet->m_pCID->SetCIDPause(false);
				}
			
				SendQuickCmd2Execute(EXECUTE_QUICK_CMD_RESET);
				ADD_LOG_TASKCONTROL(
					"[TPCMD_OHT_RESET][SendQuickCmd2Execute:%d]",
					EXECUTE_QUICK_CMD_RESET);
			
				// by zzang9un 2013.03.04 : Reset 명령의 경우 Main의 에러를 클리어 한다.
				// 임시로 이곳에 구현함... 근본적인 해결 필요함.
				ErrorCode_Main = NO_ERR;
			
			
				// by zzang9un 2013.10.03 : 최종 목적지 변수를 초기화
				m_pOHTMainStatus->FinalNodeOfPathSearch = 0;
				m_pOHTMainStatus->FinalEndNodeOfPathSearch = 0;
				m_pOHTMainStatus->FinalStationOfPathSearch = 0;
			
				// 2016-12-26, Add by Labeler
				// Handy TP Longrun 동작 중일 경우, Lognrun 동작 Clear
				if(GetHandyTPLongRun() != HANDTP_LONGRUN_NONE)
				{
					SetHandyTPLongRun(HANDTP_LONGRUN_NONE);
				}
			
				InitClearCycle();
				bAutoChange = false;
				OHTMainForm->m_MapUpdateResult = NO_ERR;
			}
			
			
			Sleep(1000);
		}
		else
		{			
			m_StepAutoChangeReset.bAllDone = true;
			m_StepAutoChangeReset.nRetryCnt = 0;
			ADD_LOG_TASKCONTROL("ExecuteResetSteps() : Reset Fail!");
		}
	}

}

// 동기화 처리해서 IOT 데이터 복사
void TaskControl::GetIotData(sIoT_Data *pBuf)
{
	EnterCriticalSection(&g_csIOT);

	memcpy(pBuf, &m_IoTData, sizeof(sIoT_Data));

	LeaveCriticalSection(&g_csIOT);
}

void TaskControl::GetMCCCount(int &JobO_NAK,int &JobC_NAK,int &JobD_NAK,int &OCSComm)
{
	JobO_NAK = m_mccparam.nJobO_NAK_Count;
	JobC_NAK = m_mccparam.nJobC_NAK_Count;
	JobD_NAK = m_mccparam.nJobD_NAK_Count;
	OCSComm = m_mccparam.nOCS_Comm;

	m_mccparam.nJobO_NAK_Count = 0;
	m_mccparam.nJobC_NAK_Count = 0;
	m_mccparam.nJobD_NAK_Count = 0;
	m_mccparam.nOCS_Comm = 0;
}


//AOHC-234 C3850 Switch Fail-Over 시 통신 끊김 개선
void TaskControl::CheckARPTableClear(unsigned char *szAddr)
{
 unsigned char szCmd[30]="";

	 sprintf(szCmd, "arp -d %s", szAddr);
	 Ping_LOG("%s", szAddr);
	if(m_bflagARPClear == false)
	{
		//system("netsh interface ip delete arpcache");
		//system("arp -d");    // ARP 테이블 Clear
		Ping_LOG("ARP Table Cleared. Err: %d", WinExec(szCmd, SW_SHOWNORMAL)); // ARP 테이블 Clear
		/* 0 -- The system is out of memory or resources.
			ERROR_BAD_FORMAT -- The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).
			ERROR_FILE_NOT_FOUND -- The specified file was not found.
			ERROR_PATH_NOT_FOUND -- The specified path was not found. */

		m_SWArpInterval.Start();
//        m_dwARPLastClearTime = timeGetTime();
		m_bflagARPClear = true;

	}
	else
	{  //Ping_LOG("m_bflagARPClear = true");
		if(m_SWArpInterval.GetElipseTime(timeGetTime()) >= 10000)
		{   //  Ping_LOG("m_bflagARPClear = true/ 10초 초과 -> m_bflagARPClear = false 전환!!  ");
			m_bflagARPClear = false;
		}

	}
}


bool TaskControl::HoistLongRun(int nAction, int nHoistTeachingPos, int nHoistHomePos)
{
	static unsigned int iManualTransCmdCnt = 0;


	int nError = 0;
	char TmpCmdBuf[LEN_TLV_BUFFER] = {0,}; // Load/Unload 명령이 임시 저장되는 버퍼
	static int nCmdCount = 0;
	PARSING_TRANS_INFO TempTransInfo;
	memset(&TempTransInfo, 0x00, sizeof(PARSING_TRANS_INFO));
	TRANS_CMD   m_TransCmd; ///< 이적재 관련 Command
	CMD_HEADER header;

	int step = 0;

	if(GetExecuteStatus() != STS_INIT
	&& GetExecuteStatus() != STS_ARRIVED
	&& GetExecuteStatus() != STS_LOADED
	&& GetExecuteStatus() != STS_UNLOADED
	&& GetExecuteStatus() != STS_COMPLETE)
	{
		return false;
	}
	if(((GetExecuteStatus() == STS_INIT) && nAction == 2)
	|| GetExecuteStatus() == STS_LOADED)
	{
		step = 1;
	}
	else if(((GetExecuteStatus() == STS_INIT) && nAction == 1)
	 ||(GetExecuteStatus() == STS_UNLOADED))
	{
		step = 1;
	}
	else
		return false;


	if(nCmdCount >= 9)
	{
		nCmdCount = 1;
	}
	else
	{
		nCmdCount++;
	}

	// Long Run Command Parsing 및 할당
	int  nListCount =  m_pCmdManager->GetCycleCount();

	if(nListCount > 0)
		return false;


	switch(step)
	{
		case 1: 	// 재하 LOAD (장비로 내려놓음)
		{
			int iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
			int iReturn;
			AnsiString strTmp;

			if(m_pExecuteInfo->ExecutePositionInfo.Marked == false && bHoistJig == false)
			{
				break;
			}

			PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어

			ADD_LOG_TASKCONTROL("[UPDOWN] TRANS_LOAD");


			PARSING_TRANS_INFO tmpCmd;

			tmpCmd.CmdID = iManualTransCmdCnt++;	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
			tmpCmd.CmdCycle = CYCLE::CraneDown;
			if(SEND_PACKET.BODY.TransOption.Info.AutoRecovery == 1)	// this is bit field
				tmpCmd.AutoRecovery = 1;	// this is 1 byte
			else
				tmpCmd.AutoRecovery = 0;
			tmpCmd.TrasType = 0; 				// Not Used!!
			// Not Used!! tmpCmd.StationData_PortID[]

			memcpy(&tmpCmd.TransCmd, (TRANS_CMD *)&SEND_PACKET.BODY, sizeof(TRANS_CMD));
			tmpCmd.TransCmd.Tposition_Hoist =  nHoistTeachingPos * 10;
			tmpCmd.TransCmd.Tposition_HomePos = nHoistHomePos * 10;

			ADD_LOG_TASKCONTROL("Teaching: %d - %d", tmpCmd.TransCmd.Tposition_Hoist, nHoistTeachingPos);
			ADD_LOG_TASKCONTROL("Home: %d - %d", tmpCmd.TransCmd.Tposition_HomePos, nHoistHomePos);
			if(CheckStatusForManual())
				MakeTransCycleList(&tmpCmd);
		}
		break;

		case 2: 	// 재하 UNLOAD (장비에서 꺼내 OHT로 이동)
		{
			int iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
			int iReturn;
			AnsiString strTmp;

			PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어

			ADD_LOG_TASKCONTROL("[UPDOWN] TRANS_UNLOAD");

			if(m_pExecuteInfo->ExecutePositionInfo.Marked == false && bHoistJig == false)
			{
				break;
			}

			PARSING_TRANS_INFO tmpCmd;

			tmpCmd.CmdID = iManualTransCmdCnt++;	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
			tmpCmd.CmdCycle = CYCLE::CraneUp;
			if(SEND_PACKET.BODY.TransOption.Info.AutoRecovery == 1) // this is bit field
				tmpCmd.AutoRecovery = 1;	// this is 1 byte
			else
				tmpCmd.AutoRecovery = 0;
			tmpCmd.TrasType = 0;				// Not Used!!
			// Not Used!! tmpCmd.StationData_PortID[]

			memcpy(&tmpCmd.TransCmd, (TRANS_CMD *)&SEND_PACKET.BODY, sizeof(TRANS_CMD));
			tmpCmd.TransCmd.Tposition_Hoist = nHoistTeachingPos*10;
			ADD_LOG_TASKCONTROL("Teaching: %d - %d", tmpCmd.TransCmd.Tposition_Hoist, nHoistTeachingPos);
			if(CheckStatusForManual())
				MakeTransCycleList(&tmpCmd);

		}
		default:
		break;
	}

	return true;

}

//
//bool TaskControl::SlideLongRun(int nAction, int nSlideTeachingPos)
//{
//	static unsigned int iManualTransCmdCnt = 0;
//
//
//	int nError = 0;
//	char TmpCmdBuf[LEN_TLV_BUFFER] = {0,}; // Load/Unload 명령이 임시 저장되는 버퍼
//	static int nCmdCount = 0;
//	PARSING_TRANS_INFO TempTransInfo;
//	memset(&TempTransInfo, 0x00, sizeof(PARSING_TRANS_INFO));
//	TRANS_CMD   m_TransCmd; ///< 이적재 관련 Command
//	CMD_HEADER header;
//
//	int step = 0;
//
//	if(GetExecuteStatus() != STS_INIT
//	&& GetExecuteStatus() != STS_ARRIVED
//	&& GetExecuteStatus() != STS_LOADED
//	&& GetExecuteStatus() != STS_UNLOADED
//	&& GetExecuteStatus() != STS_COMPLETE)
//	{
//		return false;
//	}
//	if(((GetExecuteStatus() == STS_INIT) && nAction == 2)
//	|| GetExecuteStatus() == STS_LOADED)
//	{
//		step = 1;
//	}
//	else if(((GetExecuteStatus() == STS_INIT) && nAction == 1)
//	 ||(GetExecuteStatus() == STS_UNLOADED))
//	{
//		step = 1;
//	}
//	else
//		return false;
//
//
//	if(nCmdCount >= 9)
//	{
//		nCmdCount = 1;
//	}
//	else
//	{
//		nCmdCount++;
//	}
//
//	// Long Run Command Parsing 및 할당
//	int  nListCount =  m_pCmdManager->GetCycleCount();
//
//	if(nListCount > 0)
//		return false;
//
//
//	switch(step)
//	{
//		case 1: 	// 재하 LOAD (장비로 내려놓음)
//		{
//			int iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
//			int iReturn;
//			AnsiString strTmp;
//
//			if(m_pExecuteInfo->ExecutePositionInfo.Marked == false)
//			{
//				break;
//			}
//
//			PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;
//			memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
//
//			ADD_LOG_TASKCONTROL("[UPDOWN] TRANS_LOAD");
//
//
//			PARSING_TRANS_INFO tmpCmd;
//
//			tmpCmd.CmdID = iManualTransCmdCnt++;	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
//			tmpCmd.CmdCycle = CYCLE::CraneDown;
//			if(SEND_PACKET.BODY.TransOption.Info.AutoRecovery == 1)	// this is bit field
//				tmpCmd.AutoRecovery = 1;	// this is 1 byte
//			else
//				tmpCmd.AutoRecovery = 0;
//			tmpCmd.TrasType = 0; 				// Not Used!!
//			// Not Used!! tmpCmd.StationData_PortID[]
//
//			memcpy(&tmpCmd.TransCmd, (TRANS_CMD *)&SEND_PACKET.BODY, sizeof(TRANS_CMD));
//			tmpCmd.TransCmd.Tposition_Shift = nSlideTeachingPos*10;
//			ADD_LOG_TASKCONTROL("Teaching: %d - %d", tmpCmd.TransCmd.Tposition_Shift, nSlideTeachingPos);
//
//			if(CheckStatusForManual())
//				MakeTransCycleList(&tmpCmd);
//		}
//		break;
//
//		case 2: 	// 재하 UNLOAD (장비에서 꺼내 OHT로 이동)
//		{
//			int iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
//			int iReturn;
//			AnsiString strTmp;
//
//			PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;
//			memset(&SEND_PACKET, 0, iPacketSize);		// 패킷 메모리 클리어
//
//			ADD_LOG_TASKCONTROL("[UPDOWN] TRANS_UNLOAD");
//
//			if(m_pExecuteInfo->ExecutePositionInfo.Marked == false)
//			{
//				break;
//			}
//
//			PARSING_TRANS_INFO tmpCmd;
//
//			tmpCmd.CmdID = iManualTransCmdCnt++;	// 매뉴얼 동작시 커맨드 번호를 하나씩 추가로 생성하고, 마지막에는 0으로 오도록 한다.
//			tmpCmd.CmdCycle = CYCLE::CraneUp;
//			if(SEND_PACKET.BODY.TransOption.Info.AutoRecovery == 1) // this is bit field
//				tmpCmd.AutoRecovery = 1;	// this is 1 byte
//			else
//				tmpCmd.AutoRecovery = 0;
//			tmpCmd.TrasType = 0;				// Not Used!!
//			// Not Used!! tmpCmd.StationData_PortID[]
//
//			memcpy(&tmpCmd.TransCmd, (TRANS_CMD *)&SEND_PACKET.BODY, sizeof(TRANS_CMD));
//			tmpCmd.TransCmd.Tposition_Shift = nSlideTeachingPos*10;
//			ADD_LOG_TASKCONTROL("Teaching: %d - %d", tmpCmd.TransCmd.Tposition_Shift, nSlideTeachingPos);
//			if(CheckStatusForManual())
//				MakeTransCycleList(&tmpCmd);
//
//		}
//		default:
//		break;
//	}
//
//	return true;
//
//}


#pragma package(smart_init)
