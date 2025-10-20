//---------------------------------------------------------------------------
/**
@file    MainControlTask.h
@version OHT 7.0
@brief   OHT에서 MainControl 역할을 담당하는 Header File
*/

#ifndef TaskControlH
#define TaskControlH
//---------------------------------------------------------------------------

#include "../Common/Protocol_Define_Modify.h"
#include "../Common/UdpCom.h"
#include "../Common/UdpDiag.h"
#include "PassPermit.h"

#include "Def_OHT.h"
#include "../Common/Def_Protocol.h"
#include "Def_HW.h"
#include "Def_DefaultIniParam.h"
#include "Def_MCCParam.h"

#include "ServerSocket.h"
#include "ExecuteInfo.h"
#include "ExecuteThread.h"
#include "OHTMainStatus.h"
#include "CmdManager.h"
#include "PathSearch.h"
#include "HWSetStatus.h"
#include "HWCommon.h"
#include "HWSet.h"
#include "HWControlThread.h"
#include "DrivingControl.h"
#include "TransControl.h"
#include "ManualControl.h"
#include "StatusCheckThread.h"
#include "Utility.h"
#include "LogHandler.h"
#include "LogThread.h"
#include "VersionInfo.h"
#include "STD_PathData.h"
#include "StationData.h"
#include "TeachingData.h"
#include "NetworkSignallib.h"

#include "DiagManager.h"

#include "IoTManager.h"
#include "MyTools.h"
//#include "AvsControl.h"
//---------------------------------------------------------------------------
// Task State Definition
//---------------------------------------------------------------------------
#define STEP_NO_DETECT_CHECK            0
#define STEP_LONG_CHECK     			1
#define STEP_LONG_MIDDLE_CHECK          2
#define STEP_MIDDLE_CHECK               3
#define STEP_MIDDLE_SHORT_CHECK         4
#define STEP_LONG_MIDDLE_SHORT_CHECK    5
#define STEP_LONG_SHORT_CHECK           6



#define PARMA_DIAG    		0
#define PARMA_DRIVING    	1
#define PARMA_EQPIO    		2
#define PARMA_PATHSEARCH    3
#define PARMA_TRANS    		4
#define PARMA_UBGCHECKNODES	5
#define PARMA_IOMAP    		6
#define PARMA_OHTDETECTAREA	7
#define PARMA_OBSDETECTAREA 8



/**
@brief   Main Thread 동작 상태 변수
@author  zzang9un
@date    2012.11.16
@note    Long Run 이적재 Test를 위해 추가
*/
enum MAIN_TASK_STATE
{
	MAIN_TASK_INITSOCKET = 0,   ///< Task 상태 : Node
	MAIN_TASK_DATALOAD,         ///< Task 상태 : 통신 상태 완료
	MAIN_TASK_SOCKETENABLE,     ///< Task 상태 : 통신 Open
	MAIN_TASK_READY,            ///< Task 상태 : OHT Init(Mode, Status)
//	MAIN_TASK_CHECKERROR,       ///< Task 상태 : Error를 체크한다.
	MAIN_TASK_CHECKCMD,         ///< Task 상태 : Command를 체크
	MAIN_TASK_SHUTDOWN,         ///< Task 상태 : 종료
	MAIN_TASK_ERROR             ///< Task 상태 : 에러 상태
};

// 20161216 sh17.jo : Commfail 체크 기능 추가
enum NETWORK_RESET_STATE
{
	NETRESET_SOCKET_ERROR = -4,
	NETRESET_IP_ABNORMAL	= -3,
	NETRESET_IP_NULL = -2,
	NETRESET_ABNORMAL_CONNECTION = -1,
	NETRESET_PING_FAIL = 0,
	NETRESET_PING_SUCCESS
};
//---------------------------------------------------------------------------

typedef struct HTAG_DATA_
{
	char CmdData2       : 8;
	char CmdData1       : 8;
	char ProtoCmdType   : 8;
	char SenderReceiver : 8;
}HTAG_DATA;

typedef union _uHTagData
{
	HTAG_DATA Htag_Data;
	int nWholeTagData;

}uHTagData;

// ============================= Firmware Update =======================================
typedef struct 
{
	int iMainUpdate;		// TRUE, FALSE. OHT Main을 업데이트 할지 여부를 설정한다
	int iAmcUpdate;			// TRUE, FALSE. AMC를 업데이트 할지 여부를 설정한다 
	int iAsUpdate;			// TRUE, FALSE. 어시스턴스를 업데이트 할지 여부를 설정한다
	int iFmUpdate;			// TRUE, FALSE. File Manager를 업데이트 할지 여부를 설정한다
	int iParamUpdate;		// TRUE, FALSE. 파라미터를 업데이트 할지 여부를 설정한다
	
	int iWirelessConfig;	// TRUE, FALSE. 무선모듈 Config 업데이트 여부를 설정한다
	int iWirelessMacfil;	// TRUE, FALSE. 무선모듈 Macfil 업데이트 여부를 설정한다
	int iWirelessFirmware;	// TRUE, FALSE. 무선모듈 Firmware 업데이트 여부를 설정한다

	int iParamFileCount;	// 업데이트 할 파라미터 파일의 갯수
	AnsiString strParamFileList[100]; // 업데이트 할 파라미터 파일명을 기술하는 섹션 (최대 99개)

} FIRMWARE_INFO;


#define FIRMWARE_UPDATE_PATH		"C:\\User\\OHT\\DataFiles\\Down\\"
#define OHT_PARAM_FILE_PATH			"C:\\User\\OHT\\DataFiles\\"
#define OHT_PROGRAM_PATH			"C:\\User\\OHT\\"
#define AMC_BINARY_PATH				"C:\\User\\OHT\\DataFiles\\"
#define WIRELESS_PARAM_FILE_PATH	"C:\\User\\OHT\\DataFiles\\"


#define OHT_MAIN_FILE_NAME 		L"OHT.exe"
#define ASSISTANCE_FILE_NAME    L"OHTAssistant.exe"
#define FILE_MANAGER_FILE_NAME  L"FM.exe"

#define AMC_BOOT_FILE_NAME		L"coffboot.bin"
#define AMC_MAIN_FILE_NAME		L"amc.out"

#define WIRELESS_CONFIG_FILE_NAME	L"CONFIG"
#define WIRELESS_MACFIL_FILE_NAME	L"MACFIL"
#define WIRELESS_FIRMWARE_FILE_NAME L"FIRMWARE.BIN"


#define BACKUP_FILE_EXTENSION	".bak"


#define AS_VER_FILE_PATH		"C:\\User\\OHT\\DataFiles\\AsInfo.dat"
#define FM_VER_FILE_PATH		"C:\\User\\OHT\\DataFiles\\FmInfo.dat"


// Firmware Update Response Direction (OHT -> TP, OHT -> OCS)
enum FWU_RESPONSE
{
	FWU_TO_OCS = 0,   
	FWU_TO_TP,
};

// wireless reset msg 
typedef	enum	WIRELESS_RESET_RESULT_
{
	WIRELESS_RESET_OK			= 0,
	WIRELESS_RESET_WSA_ERR,
	WIRELESS_RESET_INVALID_SOCKET,
	WIRELESS_RESET_CONNECT_ERR,
	WIRELESS_RESET_SOCKET_OPT_ERR,
	WIRELESS_RESET_NO_CONN_MSG,
	WIRELESS_RESET_NO_PASS_MSG,
	WIRELESS_RESET_NO_PASS_OK_MSG,
	WIRELESS_RESET_NO_RESET_OK_MSG,

}WIRELESS_RESET_RESULT;

enum TARGET_DEVICE
{
    OCS_P = 0,
    OCS_S,
    GATEWAY,
	WIRELESS_CLIENT,
	OCS_GATEWAY
};

enum PASS_PERMIT_OPEN_TYPE
{
    FROMTP_TYPE = 0,
    AUTOCHANGE_TYPE
};

typedef struct sIoT_Data
{
	IOT_HEADER 				Info1;
	IOT_DEFECT_SIGNAL_COUNT Info2;

	sIoT_Data()	{	memset(this, 0, sizeof(*this));	}

} sIoT_Data;

#define UPDATE_DRIVING_INI		0
#define UPDATE_TRANS_INI		1
#define UPDATE_EQPIO_INI		2
#define UPDATE_DIAG_PARAM_INI	3
#define UPDATE_PATHSEARCH_INI	4
#define UPDATE_IOMAP_INI		5


#define PREVIOUS_LOG_WRITE_INTERVAL_MIN		   	 60 // 대차패턴  남기는 시간 간격(단위 : 분) AOHC-237 UBG 좌/우 틀어짐 검출 기능 추
// =====================================================================================

/**
@class   TaskControl
@date    2013.05.24
@author  puting
@brief   TaskControl Class
*/
class TaskControl
{
private:
	LogUnit *pLogTaskControl;      ///< TaskControl 로그 생성을 위한 변수
	LogUnit *m_pLogOCSComm; ///< OCS Comm Log 저장을 위한 변수 선언
	LogUnit *m_pLogTPComm;  ///< TP Comm Log 저장을 위한 변수 선언
	LogUnit *pLogMapMake; ///< Map Make 관련 로그 변수
	LogUnit *pLogIotComm; ///< IOT 통신 로그 변수
	LogUnit *pLogError; ///< Error log관련 로그 변수
	LogUnit *pLogPing; ///< PING 통신 로그 변수
	LogUnit *pLogHandTP; ///< HandTP 통신 로그 변수
	LogUnit *pLogMCC;

	FDCUnit *pFDCUnit;

	//생성자에서 받기위한 포인터
	UdpCom *m_pUDPCommunication;       ///< Communication Class
	UdpDiag *m_pUDPDiag;  ///<Communication Class
	CmdManager *m_pCmdManager;       ///< Command Manager Class
	PassPermit *m_pPassPermit;
	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;
	PathSearch    *m_pPathSearch;
	DEFAULT_PARAM_SET *m_defualtparam;
	MCC_TASK_PARAM_SET	m_mccparam;



	// by zzang9un 2013.03.22 : TP에게 보내기 위한 OHT Info 정보(Union 사용)
	uTP_CMD m_OHTInfo;      ///< OHT Info


	// OCSStatus Send 유무 및 TaskControl 쓰레드 작동유무 Check

	bool TaskControlfalg;
	int  nCheckErrorOHTType;

	MAIN_TASK_STATE TaskState_Cur;     ///< Main Thread Task State(현재)
	MAIN_TASK_STATE TaskState_Pre;     ///< Main Thread Task State(과거)

	// Status를 위한 변수
	MSG_STATUS_OHT2TP m_StatusOHT2TP;

	MAPMAKE_STATUS *m_pMapMake_Status; //2013.07.11 추가

	MapMakeProcess *m_pMapMake_Process; //2019.09.15 추가
	MAPMAKE_PROCESS m_MapMakeMoveProcess; //2019.09.15 추가

	// Firmware Update Response Direction
	FWU_RESPONSE m_iFWU_Direction;

	AnsiString m_strMonitor;

	// doori.shin 2014.07.08 접속한TP 종류 : 유선TP접속시 무선TP접속 차단
	int m_nTPType;


	// 2016.08.03 물류 IoT Socket 추가
	SOCKET m_IOTHubSocket;
	ServerSocket *m_IOTHubServerSock;
	bool m_ConnectedIOTHub;


    IotManager *m_IotManager;

	// Data 관련 변수
	bool m_bLoadedMap;          ///< Map File Load 상태 변수
	bool m_bLoadedStationData;  ///< Station Data File Load 상태 변수
	bool m_bLoadedTeachingData; ///< Teaching Data File Load 상태 변수

	bool m_bLoadedCID_STD;
	bool m_bLoadedCollision_STD;
	bool m_bLoadedFlowControl_STD;
	bool m_bLoadedMap_STD;
	bool m_bLoadedStation_STD;
	bool m_bLoadedTeaching_STD;
	bool m_bLoadedPathData_STD;
	DWORD m_dwUBGTrigerTime; //AOHC-237 UBG 좌/우 틀어짐 검출 기능 추가
	DWORD m_LogTime;  //AOHC-237 UBG 좌/우 틀어짐 검출 기능 추가

	int m_MoniUBGPat;
public:
	// 20161216 sh17.jo : Commfail 체크 기능 추가
	bool m_bCheckRcvData;
	bool m_bSetCheckCommfail;	//OCS소켓 Commfail 체크 플래그
	int m_nComfailCount[4];
	int m_nComOKCount;
    bool m_bCheckPassOpenListCount;
//    PATHSEARCH_UBG_CHECK_PARAM_SET  *m_pUBGParamSet;     //soming UBG 감지 단계 측정
	double m_dMinAccel;


private:

	int m_nOHTDetect_Inpunt_Prev;	// 이전 UBG 대차 값 8~2

	// by zzang9un 2013.02.06 : OHT Error Code(현재 발생한 에러 코드를 의미함)
	int OHTError_Current;      ///< OHT의 최종 에러값
	int ErrorCode_Main;       ///< Main Thread 에러 변수
	//2015.12.10 doori.shin : DiagManager
	DiagManager* m_DiagManager;


	//2013.08.14 doori.shin IO Map Field 저장하는 구조체
	IO_MAP_FIELD *m_IOMapField;
	TIniFile *m_IOMapIni;
	String strIOMapPath;
	AnsiString m_strIOVersion;

	//2014.07.15 doori.shin : Network Device
	NetworkSinalLib* m_NetWorkDll;

	// 2016.06.27. shkim. Lifter Name add
	DrivingControl	*m_pDrivingControl;

    // 2017-01-13, Add by Labeler, Auto Change 용 변수 추가
    bool bAutoChange;
    bool m_bUBGReadCheck;
	// Map Make 처리 함수
//	int SendMapMakeInfoOHT2TP(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo);
	int UpdateMapMakeInfo(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo);
	int HandleMapMake();

	// Task 관련 함수
	void ChangeMainTaskState(MAIN_TASK_STATE MainState);

	// Mode, Status 관련 함수
	void ChangeMode(BYTE mode);
	void ChangeStatus(BYTE status);
	void ChangeCmdID(BYTE PreCmdID, BYTE CurCmdID);
	BYTE GetCurrentMode();
	BYTE GetExecuteStatus();
    BYTE GetExecutePreCmdID();
    BYTE GetExecuteCurCmdID();
    bool CheckStatusForDriving();
	bool CheckStatusForTrans();
//    bool CheckStatusForManual();
    bool CheckStatusForCommon();
    bool CheckStatusForReset();
	bool CheckStatusForAuto();
	bool CheckStatusForTask();

	// Map 관련 함수
	int LoadMapData();
	int UpdateMapData(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo);

//	int GetPathData(NODE_INFO_STD_PATHDATA *pPathdata, UINT NodeID);

	// Station 관련 함수
//	int LoadStationData();
	int UpdateStationData(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo);
	int GetStationData(STATION_DATA *pStationData, UINT StationID);        	// by zzang9un 2013.08.19 : 특정 Station ID의 Station Data를 가져오는 함수 추가

    // Teaching Data 관련 함수
//    int LoadTeachingData();
	//int UpdateTeachingData(uTP_CMD *uTPCmd);
	int UpdateTeachingData(PKT_OHT2TP_TEACHING_INFO *pTPPacket, bool bOnlyLookDownChange);
//	int SendTeachingData(uTP_CMD* uSndTPCmd);
	int GetManaulTeachingData(UINT CurStationID, BYTE PortType, TRANS_CMD* pTransData);   //상태값 확인 및 기타 호출 함수 ---puting

	// STD Map Load 관련 함수
	int LoadCIDData_STD();
	int LoadCollisionData_STD();
	int LoadFlowControlData_STD();
	int LoadMapData_STD();
	int LoadStationData_STD();
	int LoadTeachingData_STD();
	int LoadPathData_STD();

	// 7.0버전의  LoadMapData, LoadTeachingData, LoadStationData에서 사용하는 자료구조 생성 함수
	int MakeMapData();
	int MakeStationData();
	int MakeTeachingData();
	
	// Command 관련 함수

	int MakeSetStationOffset(STATION_DATA *pStationData);

	//2013.09.10 ji7412.lee HandTP 기능추가
	Remocon *m_pRemocon;
	bool bOpenRemocon;
	UINT m_nInputData[4][32];
	uINPUT_BITFIELD uInput;

	int ComOpenForRemocon();
//	int GetReadRemoconData();
	int MakeAckDataRemocon(char* szReadBuff, int Error);
	void Analysis_IOData();

    // 2016-12-25, Add by Labeler
    // Handy TP 용 Longrun 명령 발생 함수 추가
    int MakeLongrunCmdForHandyTP(char* szReadBuff, uINPUT_BITFIELD uInput);


	///----------Auto to Manual Check ---//
	bool m_bCheckAutoToManual;
    NODE_INFO_STD_STATION m_OCSStaionInfo;


   ////-----------------------------표준화 OHT 관련 함수들 추가 내용.-------------------------------------------//puting 2015.11.02
	PROTOCOL_STATUS_OHT m_PreStatusOHT2OCS;    ///< 이전 상태값 확인을 위한 변수
//	DWORD m_dwJobChangeCheckTime;   //작업변경 가능한 조건을 확인 처리하기 위한 변수
//	bool m_bJobChangeCheckFlag;   //작업변경 가능한 조건을 확인 처리하기 위한 변수

	//by puting 2015 10.15 상태값 변경
	void ChangeCmdIDAndCycle(BYTE CurCycle, unsigned short CurCmdID, UINT StationNID =0,bool bWaitCycleClear =false);  //현재 사이클 명령 변경


	bool CheckStandardOHTStatus();					///< Status 상태 변화 확인
	bool ChangePreStandardOHTStatus(PROTOCOL_STATUS_OHT* pPreStatusOHT2OCS);		///< 이전상태값 갱신

	bool CheckStatusForCycleClear();       ///<작업 지시 변경 및 에러발생시, 작업 클리어를 위한 부분 puting 20160307
	bool CheckTaskSatus();
	bool CheckStatusForCycle();
	bool CheckManualForCycle();

	void CheckSatusCriticalSection();


	// by puting 2015.10.16 : Command를 Parsing하는 함수를 별도 분리함
	int StandardParsingOCSCmd(PKT_OCS2OHT* RcvPkt);
	int StandardParsingPCCmd(PKT_OCS2OHT* RcvPkt);
	int StandardParsingASTCmd(PKT_OHT2OCS* RcvPkt);
	int StandardParsingMonitoringCmd(PKT_OHT2DIAG* RcvPkt);
	
	int CheckASTCMDData(PKT_OHT2OCS* pRcvData, PKT_ACK_OHT2OCS* AckPkt);
  	int CheckDIAGCMDData(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* AckPkt);

	//puting 2015. 10. 15 : Cyclist의 값을 수정 및 변경하는 부분
	int MakeCycleList(PKT_TASKCMDDATA *pCmdData);
	int CheckCycleList(PKT_TASKCMDDATA *pCmdData);
	int ChangeCycleList(PKT_TASKCMDDATA *pCmdData);
	void InitClearCycle(bool bcheckManual = false);
	void InitExcuteOverRun();

	//puting : TaskList ->Cyclist로 값 저장.
	int MakeGoCycleList(PARSING_GO_INFO *pCmdData,int StartNode=0, int StationNode =0, int noffset =0);   ///<수동 작업지시도 여기서 해줌.
	int ChangeGoCycleList(PARSING_GO_INFO *pCmdData,int StartNode=0, int StationNode =0);   ///<수동 작업지시도 여기서 해줌.
	int BackGoCycleList(PARSING_GO_INFO *pCmdData,int StartNode=0, int StationNode =0, int nDiv =0);   ///<후진 동작을 위한 부분.
//	int MakeTransCycleList(PARSING_TRANS_INFO *pCmdData);
	int MakeManualCycleList(EXECUTE_MANUALCONTROL_INFO *pCmdData);      ///< 조그, IO동작 등 순수 수동명령만.

	//puting 2015.11.05  : Cyclelist -> Excute List로 값 저장.
	int MakeGoExcuteInfo(PKT_CYCLECMDDATA *pCmdData, LList *pExeCmd);
	int MakeTransExcuteInfo(PKT_CYCLECMDDATA *pCmdData, LList *pExeCmd);
	int MakeManualExcuteInfo(PKT_CYCLECMDDATA *pCmdData, LList *pExeCmd);


	void MakeResponseStandardOHT2OCS(PKT_ACK_OHT2OCS* AckData);
	void MakeResponseStandardOHT2TP(PKT_OHT2TP_RESPONSE* AckDataPC);
	void MakeResponseStandardOHT2DIAG(PKT_ACK_OHT2DIAG* AckDataMonitoring);

	//--------------작업변경 관련 함수---------------------- puting
	bool CheckJobChange(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121 *Cmd121,BYTE &chNackToAckType);
	bool PathChangeReq2Driving(int ChangeTagID,int SQNum, bool bJobChage =false);   ///< 작업변경 요청하는 부분(주행노드)
	int  CheckPathChange2Driving();  ///< 작업변경 완료유무 확인하는 부분
	bool CheckChangeCycle(int ChangeTagID);     //<  속도에 따른 작업변경 거리 확인 및 가능 유무 Check
	bool CancelPathInfo(int ChangeTagID, int SQNum);

	void JobOrderDataCancel();     //작업지시에 대해 내림 or 올림싸이클일 경우 이동싸이클로 변경하는 처리 부분
	void WaitCycleDataCancel();    //대기싸이클 정보가 있는 경우, 삭제 처리(에러가 나는 경우는 대기싸이클 정보도 삭제됨) (이동싸이클로 변경되고, 동일한 위치의 목적지일 경우 대기싸이클 처리 안함.)



	//-----Auto Teaching 관련 함수 --------------- puting 20161220

//	bool CheckCmdMapMakerResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);     ///< file이동관련된 Map Map Maker TP 명령에 대한 응답
//	bool CheckCmdAutoTeachingResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);    ///< file Teaching TP 명령에 대한 응답


	int MakerMapMakerInfo(int nEndNode, int nStationNode, int nPortType, int iCycle, bool bMove=true);
	int MakeAutoTeachingInfo(int TargetStationID, bool bMove, BYTE PortType,bool bTeachingNullType = false);     // 확인후 진행.
	int MakeMapMakerInfo(int EndNode, bool bMapMaker);

	bool m_FileMapMakeflag;   //Flag On 시점은 TP에서 명령 받았을때, Off시점은 File에 더이상 읽을 데이터가 없거나, 에러발생했을 경우
	bool m_FileTeachingflag;  //Flag On 시점은 TP에서 명령 받았을때, Off시점은 File에 더이상 읽을 데이터가 없거나, 에러발생했을 경우(현재 미사용 Auto Teaching Mudule 개발중)

	bool  CheckMapMakerFile(int MapCheckType); //file 유무 및 버전 확인 : 정상 : 0, 버전 X : 1, file Load X : 2
//	int  UpdateMapMakerFile(); //정보가 갱신되었을떄 파일 Update MapMaker인 경우만 확인할것.
//	bool CheckMapMakerData(); //플래그 설정시 map maker파일 있는지 확인하는 부분
//	bool CheckTeachingData(); //플래그 설정시 teaching Data 잇는지 확인하는 부분
//	int DeleteMapMakerFile(); //완료되었을떄 파일 삭제 및 초기화
	int Load_AutoTeaching_DrvingData(UINT CurStationID, BYTE PortType);

	void Send_MapMakeResult(AnsiString strLog);


	//----------------------------통과허가관련 내용-----------------------------------------//
//	unsigned int dwPassPermitTime;                ///통과허가 요청을 위한 시간 설정

//	DWORD dwPassPermitTime;                ///통과허가 요청을 위한 시간 설정
	bool IsBeforeAutoCheckPassPermit(bool bStationTag, int CurrNode =0,int NextNode =0);       //Auto 전환 전 통과허가 확인.
	bool IsOHTDetectCheckPassPermit(double dRestDistance);//,int nDectetType);       //대차감지에 의한 통과허가 요청 유무 확인  puting 2017.05.11

	void CheckAbsolutePosition();
public:
	void CheckUBGSequence(int nDistanceSensorType); //soming UBG 체크

private:

	void JudgePassPermitReq();     //통과허가 및 개방 요청 판단하는 부분

	void JudgeProgressReq();       //진척보고 요청
	void SendProgressReq(BYTE SendProgress);

	void JudgeOHTInOutReq();       //투입 빼냄 보고 요청
	void SendOHTInReq(BYTE SendData);  //투입 요구
	void SendOHTOutReq();              //빼냄 요구
	void SendOHTDefReq();              //주회주행 요구

	void JudgeCIDReportAndIgnor();       //CID 상태 보고 및 CID 무시처리 확인
	void SendCIDError(BYTE cCIDStateResult, char* sCIDStatus);              //주회주행 요구
	void CIDInfoOfPassPermit();              //통과허가리스트에 남은거에 따른 CID 정보

	void SendQuickCmd2Execute(EXECUTE_QUICK_CMD QuickCmd);

	FLAG_CID_NOT_USE m_pCurrCIDUSE;
	FLAG_CID_NOT_USE m_pListCIDUSE;

	void JudgeMAPUpdate();
	void SendDataProgressReport(BYTE SendProgressSuccess, BYTE SendProgressFail);

	void MakeOHTInCmd(BYTE Type);              //빼냄/투입에 따른 내부 명령 생성.

	//세부적으로 만들어야됨.  Cmd에 대한 내용 확인

	bool CheckCmdJobOrder(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, TAGTYPE* TagType,  int QuickCmd, STANDARDCATEGORY Catagory);       ///< 작업지시 명령에 대한 응답 체크
	bool CheckCmdPPResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);     ///< 통과허가에 대한 명령에 대한 응답 체크
	bool CheckCmdPPOResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);    ///< 통과허가개방에 대한 명령에 대한 응답 체크
	bool CheckCmdForcePPOResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);    ///< 통과허가개방에 대한 명령에 대한 응답 체크
//	bool CheckCmdForcePPO(PKT_OCS2OHT* pRcvData, int QuickCmd);    ///< 통과허가개방에 대한 명령에 대한 응답 체크
//    기존 통과허가 강제 개방 요청 -> 일반 개방으로 변경된 부분
//      PASS_PERMIT_OPEN_TYPE
//      FROMTP_TYPE = 0
//      AUTOCHANGE_TYPE =1
    bool CheckCmdPassOpen_fromTP_and_AutoChange(PASS_PERMIT_OPEN_TYPE Type, PKT_OCS2OHT* pRcvData, int QuickCmd);
    bool CheckCmdPPREQResponse(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, int QuickCmd);    ///< 통과문의에 대한 명령에 대한 응답 체크
	bool CheckCmdDataTransfer(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, int QuickCmd);   ///< 데이터 지시에 대한 응답 체크
    bool CheckCmdVersion(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, int QuickCmd);   ///< 버전 정보 요청에 대한 응답 체크
	bool CheckCmdInsertResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);     ///< 투입요구에 대한 명령에 대한 응답 체크
	bool CheckCmdMoveRestart(PKT_OCS2OHT* pRcvData, int QuickCmd);    ///< 주행재게 명령에 대한  체크
	bool CheckStaionInfoReqResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);

	bool CheckCmdAutoChangeResponse(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, int QuickCmd);    //AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현 
																									 ///< 오토전환  대한 명령에 대한 응답 체크

	//진단관련 부분
	bool CheckCmdDiagConnect(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt);   ///< 통계정보 보고 요청
	bool CheckCmdDiagInfoReq(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt);   ///< 통계정보 보고 요청
	bool CheckCmdDiagInitReq(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt);   	 ///< 통계정보 초기화 요청
	bool CheckCmdDiagInfoResponse(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt);  	 ///< 통계정보 응답

	bool ConvertBigLitte(PKT_ACK_OHT2OCS *Temp);
	bool ConvertBigLitteDIAG(PKT_ACK_OHT2DIAG *Temp);

	int m_nCountCheckPathSearch;   			//Path Search NACK가 일정횟수지나면 에러(무언대기방지)
//	char strOHTID[7];                       /// OHT ID 저장하는 부분
//	int LoadOHTData();
	AnsiString strBCmask;					// BoradCast Send Mask
	void SendIoStatus_2_OHT();
	//void SendTeachingInfo_2_OHT();
	void Send_CID_INFO_2_OHT();
	void ReadMCommandResult();
	void SendTeachingInfo_2_OHT(BYTE portType);
	void SendDrivingMapInfo_2_OHT(int StartNID, int EndNID);
	
	int LoadUpdateIniFile(AnsiString strIniFilePath, FIRMWARE_INFO* Info);
	int UpdateAMCDownloadFile(AnsiString strAmcFilePath);

	int FirmwareUpdate_FileExistCheck(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_FileBackup(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_AS_FM(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_PARAM(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_WIRELESS(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_AMC(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_RESTART(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int ExecuteProgram(String strFileName);
	void getInfoDetailVersion(PKT_VERSION_RESPONSE& SEND_PACKET, FWU_RESPONSE iDirection);
	AnsiString ReqIniVersion(int nIniType);
	void chkFirmwareUpdateDirectory(void);
	void LoadWirelessModuleInfo(AnsiString& IpAddress, AnsiString& UserName, AnsiString& Password);
	void FirmwareUpdateMessageToTp(AnsiString strMsg);
	bool FirmwareUpdate_ModeChange();
	int Change2AutoMode(void);

	bool bMainRestart;
	bool bMainCopy;
	FIRMWARE_INFO m_FIRMWARE_UPDATE_INFO;
	// ------ Parameter apply ------
	int ParamApply(int iUpdateFile);

//	DRIVING_PARAM_SET *m_pTempDrivingParam;
//	TRANS_PARAM_SET *m_pTempTransParam;

	TransControl	    *m_pTransControl;
	// -----------------------------
	
	void IpStrToByteAddr(AnsiString strAddr, int& iHi, int& iM1, int& iM2, int& iLow);
	
	int ManualMoving(int iStartNode, int iStopNode, bool iIsStation, int iPortType, int iCycle);
	bool ManualMovePathSearch(int iStartNode, int iStopNode, int& iDirection, int& iCount, unsigned int* iNodeNumList);


	int ManualMovingMapMake(int iStartNode, int iStopNode, BYTE MapMakeType);
	int ManualMovePathSearchMapMake(int iStartNode, int iStopNode, int& iDirection, int& iCount, unsigned int* iNodeNumList, int &nTargetNode, bool bMapMakeTestMove= false);

    // 20161216 sh17.jo : Commfail 체크 기능 추가
    NETWORK_RESET_STATE CheckPingBeforeReset(unsigned char *ip);
    char* ConvertNetResetToString(NETWORK_RESET_STATE state);

    // 2016-12-25, Add by Labeler
    // Handy TP Longrun 명령 발생 변수 추가
    int m_nLongrunTypeHandyTP;
    bool m_bRotateJogCmdExist;
    DWORD m_dwRotateJogMoveStartTime;
    int m_nPortType_HandTPLongrun;

	DWORD dwTurnOffJobOrderTime;
	/*
	데이터 전송명령을 OCS로부터 받아 진행중인지 확인하는 플래그.
	true면 진행중으로, OCS에게 재 요구해서는 안된다.
	false면 TP로 온 요구를 OCS로 전달해도 된다
	*/
	bool m_bDataTransper_InProgress;
	void GetServoAlarmCode(int errorCode);
	ServoMonitoring *m_pServoControl;


    void ServerSocketOpen();
	void ServerSocketClose();

	// 2016.08.03  물류 IoT Hub 관련 함수(서버)
	void IOTHubSocketClientConnect();
	void IOTHubSocketClientDisconnect();
	int IOTHubSocketClientRead(short *StateIot);


	bool bCheck;
	int nCheck;
	DWORD dwUBGCheckTime;
	bool bprebumpper;
	int nUBGSensorDetecting;
	bool bUBGOneHourMonitoring;
	int nCycleCnt;

	bool bIncreased_UD;
	bool bIncreased_UF;
	bool bIncreased_UC;

	 // 채터링 타이머
	CStepTimer chtTm_UD;
	CStepTimer chtTm_UF;


public:
	TaskControl();
	~TaskControl();

	//////////////////////////////////////////////
	void SetOHTInfo(uTP_CMD m_pOHTInfo);
	int GetOHTError();

	//////쓰레드에 들어가야될 Update 함수///////
	void ExcuteTaskUpdate();

	//putig 20151117 표준화 OHT 쓰레드에 들어가야될 함수들
	void ExcuteStandardTaskUpdate();
	void ExecuteSteps();
	void ExecuteResetSteps();
	void StandardOCSReadDataUpdate();
	void StandardPCReadDataUpdate();
	void StandardASTReadDataUpdate();
	void StandardMonitoringReadDataUpdate();
	void SuperviseSendCmdDataOHT2OCS();
	int SuperviseTaskList2CycleList();
	int SuperviseCycleList2ExcuteInfo();

	CStepClass m_StepDetectAreaCheck;     // Detect Area Check 비동기 스텝	
	CStepClass m_StepAutoChangeReset;     // jhoun.lim
	
	//puting 20170108 MapMakerFile Check하는 부분.
	void StandardMapMakerFileUpdate();

	void StandardAddress_Broadcast();

    // 2016-12-19, Add by Labeler, Handy TP 관련  Update 함수
    int GetReadRemoconData();



	//IOMap Loading 함수
	int LoadIOMapField(String, String);

	// Virus check 함수
	void CheckVirus();

	int LoadParamIni(int nIniType);


	CMD_HEADER* MakeMonitoringHeader(const UINT& data, int size);


	//doori.shin 2015.12.10 : 진단Agent 용 함수
	void UpdateStatisticsDiagData();
	void UpdateAbnormalDiagData();
	void UpdateStatusDiagData();
	void UpdateCIDStatusDiagData();

	void IOTHubSocketClientUpdate();

	void SendAbnormalDiagData(DIAG_OHT_ABNORMAL* pSendData);
	void SendStatDiagData(DIAG_OHT_STATISTICS* pSendData);
	void SendStatusDiagData(DIAG_OHT_STATUS_DATA* pSendData);
	void SendReqeustDiagStatData();
	void SendCIDStatusDiagData(DIAG_PKT_BODY_CID_REPORT* pSendData);

	int CheckCMDData2TaskList(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* AckPkt, PKT_OHT2TP_RESPONSE* AckPktPC, int QuickCmd);
	int CheckError();

    // Handy TP 위해 Public으로 변환
    bool CheckStatusForManual();
	int MakeTransCycleList(PARSING_TRANS_INFO *pCmdData);
	int CheckStatusForAutoChange();  //AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현 -> Auto 전환 자동화를 위한 점검 함수
	bool CheckAutoChangeFullSemi();  //AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현 -> Auto 전환 시 풀로해주냐 일부만 해주냐

    // 20161216 sh17.jo : Commfail 체크 기능 추가
    void CheckCommfail(void);
    //bool GetCheckCommfail(void);
    int SendSnmpWirelessClient(void);

	// 2016.12.27. comm fail reset func add
	WIRELESS_RESET_RESULT Wireless_Module_Reset(char* chAddr);

	// auto teaching result send (OHT -> TP)
	void Send_AutoTeachResult(int iResult);

    // 2016-12-25, Handy TP용 Longrun Type 반환 함수
    int GetHandyTPLongRun();
    void SetHandyTPLongRun(int nType);

    bool GetRotateJogCmd();
    void SetRotateJogCmd(bool bCmd);
    DWORD GetRotateJogMoveStartTime();

	//2013.05.27 ji7412.lee : HandTP를 위한 MakeCmdList(HandTPTest 임시함수)
	int MakeCmdList_HandTP(unsigned char* pRecvBuf, bool IsConvertEndian, uINPUT_BITFIELD uInput);

    void MakeVHLStatusForHandyTP(unsigned char * cBuf, int iIndex);
    void MakeVHLIOInfoForHandyTP(unsigned char * cBuf, int iIndex);
	void MakeVHLStatusBGForHandyTP(unsigned char * cBuf, int iIndex);
	void MakeAckDataForHandTp(unsigned char * pSendBuf, int nCmdType, int nError);    

    int CheckInterlock4Cycle(unsigned char* pRecvBuf);
	int CheckInterlock4ManualKey(unsigned char cKeyCmd);

	bool GetNodeTypeSlop();
	bool FirmwareUpdateFlag;
	bool bSlope;

	MyTPAddrMan m_TPAddrMan;        // TP 다중접속 처리하기 위해서 TP 주소 관리 객체
	bool m_bMultiTPConnected;      // TP가 2개 이상 접속되었음
	bool m_bAutoChaning;			// 현재 Auto 변환중

	void GetIotData(sIoT_Data *pBuf);
	void GetMCCCount(int &JobO_NAK,int &JobC_NAK,int &JobD_NAK,int &OCSComm);

	bool m_bflagARPClear;	//AOHC-234 C3850 Switch Fail-Over 시 통신 끊김 개선
private:
	sIoT_Data m_IoTData;  // critical section 때문에 GetIotData() 함수 통해서 공개
	IOT_HEADER IotHeader;
	IOT_DEFECT_SIGNAL_COUNT *DefSignalCount;

	//AOHC-234 C3850 Switch Fail-Over 시 통신 끊김 개선
//	DWORD m_dwARPLastClearTime;
	void CheckARPTableClear(unsigned char *szAddr);
	SWTimer m_SWArpInterval;

public:
	bool bHoistJig;
	bool HoistLongRun(int nAction, int nHoistTeachingPos);
	HWSet 		*m_pHWSet;
	HWCommon 	*m_pHWCommon;
//
//	bool bSlideJig;
//	bool SlideLongRun(int nAction, int nHoistTeachingPos);

};

#endif
