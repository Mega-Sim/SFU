//---------------------------------------------------------------------------
/**
@file    OHTMain.h 
@version OHT 7.0
@brief   OHT Main Program Main Form Header File

         Sub Cpp File 목록
          - Communication.cpp
*/

#ifndef OHTMainH
#define OHTMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <winsock2.h>
//#include <ScktComp.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <jpeg.hpp>
#include <Graphics.hpp>

#pragma comment(lib,"ws2_32.lib")

#include "Def_OHT.h"
#include "Def_Protocol.h"
#include "Def_DefaultIniParam.h"


#include "UdpCom.h"
#include "UdpDiag.h"   //
#include "PassPermit.h"


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
#include "TaskControl.h"
#include "Utility.h"
#include "LogHandler.h"
#include "LogThread.h"
#include "VersionInfo.h"
//#include "MonitorControlThread.h"
//#include "AvsControlThread.h"

//2013.07.09 doori.shin 헤더파일 추가
#include "TeachingData.h"
#include "StationData.h"

#include "ExtraJobThread.h"
#include "TaskControlThread.h"


#include "MonitoringUnit.h"

#include <ExtCtrls.hpp>

//2016.04.20 KJD 헤더파일 추가
#include "STD_CID.h"
#include "STD_Collision.h"
#include "STD_FlowControl.h"
#include "STD_MAP.h"
#include "STD_Station.h"
#include "STD_Teaching.h"
#include "STD_PathData.h"

#include "STD_MapMakerFile.h"

//2019-09-03 Puting
#include "MapMakeProcess.h"
//2017.11.29 IJK 헤더파일 추가
#include "OHTDetect.h"
//#include "AvsControl.h"
//---------------------------------------------------------------------------
#define USE_LOG_MEMO
#define ADD_MEMO_T(STR)   OHTMainForm->memoMain->Lines->Add(_T(STR))
#define ADD_MEMO_S(STR)   OHTMainForm->memoMain->Lines->Add(STR)

//혼용 PIO
typedef enum EQPIO_TYPE_INFO_
{
    TYPE_PIO_IR   = 0,
    TYPE_PIO_RF   = 1,
    TYPE_PIO_HYBRID = 2
}EQPIO_TYPE_INFO;

/**
@class   TOHTMainForm
@date    2012.11.15
@author  zzang9un
@brief   OHTMainForm Class
*/
class TOHTMainForm : public TForm
{
__published:	// IDE-managed Components
  //  TTimer *tmMainTask;
	//TTimer *tmOCSStatus;
    TTimer *tmInint;
	TMemo *memoMain;
	TTimer *tmUIupdate;
    TPanel *panTop;
    TLabel *lbMode;
    TPanel *panMode;
    TLabel *lbStatus;
    TPanel *panStatus;
    TPanel *panCurNodeID;
	TLabel *lbCurrentNodeID;
    TPanel *panCurStationID;
    TLabel *lbStopNodeID;
    TPanel *panErrorCode;
    TLabel *lbErrorCode;
    TPageControl *pgcMain;
    TTabSheet *tabProgramInfo;
    TTabSheet *tabIOTest;
    TComboBox *cbbAxisNum;
    TComboBox *cbbFnType;
    TCheckBox *chkAxis;
    TCheckBox *chkInputMask1;
    TCheckBox *chkInputMask2;
    TCheckBox *chkInputMask3;
    TCheckBox *chkInputMask4;
    TCheckBox *chkInputMask5;
    TCheckBox *chkInputMask6;
    TCheckBox *chkInputMask7;
    TCheckBox *chkInputMask8;
    TEdit *edtAxisMask_Value;
    TEdit *edtInputMask_Num1;
    TEdit *edtInputMask_Num2;
    TEdit *edtInputMask_Num3;
    TEdit *edtInputMask_Num4;
    TEdit *edtInputMask_Num5;
    TEdit *edtInputMask_Num6;
    TEdit *edtInputMask_Num7;
    TEdit *edtInputMask_Num8;
    TEdit *edtInputMask_Value1;
    TEdit *edtInputMask_Value2;
    TEdit *edtInputMask_Value3;
    TEdit *edtInputMask_Value4;
    TEdit *edtInputMask_Value5;
    TEdit *edtInputMask_Value6;
    TEdit *edtInputMask_Value7;
    TEdit *edtInputMask_Value8;
    TImage *imgProgramIcon;
    TImage *imgIOTest;
    TLabel *lbOHTVer;
    TLabel *lbAMCVer;
	TTimer *tmMinimizer;
	TTimer *tmChkThreadStatus;
	TCheckBox *chkAxisMonitor;
	TComboBox *cbbAxisNum4Monitor;
	TComboBox *cbbMonitorRegister;
	TButton *btnReadMonitorReg;
	TButton *servo;
	TPanel *pnIotHubState;
	TPanel *pnSlope;
	TButton *btn1;
	TTabSheet *Tab81S;
	TLabel *lbl2;
	TEdit *Edit_front_torque;
	TButton *btn_limit_front;
	TButton *btn_limit;
	TEdit *Edit_frontName;
	TLabel *lbl1;
	TEdit *Edit_rear_torque;
	TButton *btn_limit_rear;
	TButton *btn_limit1;
	TEdit *Edit_rearName;
	TCheckBox *CheckBox_ignoreOHT;
	TCheckBox *CheckBox_ignoreOBS;
	TLabel *lblFPGA;
	TTimer *tmAvsControl;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);

    // Timer 함수
    void __fastcall tmInintTimer(TObject *Sender);
	void __fastcall tmUIupdateTimer(TObject *Sender);
	void __fastcall AppException(TObject *Sender, Exception *E);
	void __fastcall chkAxisClick(TObject *Sender);
    void __fastcall chkInputMask1Click(TObject *Sender);
    void __fastcall chkInputMask2Click(TObject *Sender);
    void __fastcall chkInputMask3Click(TObject *Sender);
	void __fastcall chkInputMask4Click(TObject *Sender);
    void __fastcall chkInputMask5Click(TObject *Sender);
    void __fastcall chkInputMask6Click(TObject *Sender);
    void __fastcall chkInputMask7Click(TObject *Sender);
	void __fastcall chkInputMask8Click(TObject *Sender);
	void __fastcall imgIOTestDblClick(TObject *Sender);
	void __fastcall tmMinimizerTimer(TObject *Sender);
	void __fastcall OnRestore(TObject *Sender);
	void __fastcall tmChkThreadStatusTimer(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);

	void __fastcall btnReadMonitorRegClick(TObject *Sender);
	void __fastcall chkAxisMonitorClick(TObject *Sender);
	void __fastcall servoClick(TObject *Sender);
	void __fastcall btn1Click(TObject *Sender);
	void __fastcall btn_limit_frontClick(TObject *Sender);
	void __fastcall btn_limitClick(TObject *Sender);
	void __fastcall btn_limit_rearClick(TObject *Sender);
	void __fastcall btn_limit1Click(TObject *Sender);
	void __fastcall tmAvsControlTimer(TObject *Sender);



private:	// User declarations
	// -------------------------------------------
	LogUnit *pLogTaskControl;   ///< Main Log 저장을 위한 변수 선언
	LogUnit *pLogOCSComm;       ///< OCS Comm Log 저장을 위한 변수 선언
	LogUnit *pLogTPComm;       ///< TP Comm Log 저장을 위한 변수 선언
    
	// 1. 공유 변수, Data 관련 Class
	LogThread *m_LogThread;

	//Server 소켓 동작및 MainControlTask을 위한 Thread
	TaskControl	*m_ControlTask;
	TaskControlThread   *m_TaskControlThread;

	// Main Command List
//	LList *m_CmdList;                  ///< Main Command List
	LList *m_TaskList;                  ///< Main Command List
	LList *m_ResponseList;                  ///< Main Command List
	LList *m_CycleList;                  ///< Main Command List
	LList *m_PathList;                  ///< Main Command List
	LList *m_PassOpenList;                  ///< Pass Permit Open Command List
	LList *m_PassPermitList;                  ///< Pass Permit Open Command List

	/// Execute Info(MainThread와 ExecuteThread 공유)
    EXECUTE_INFO *gm_ExecuteInfo;

	/// OHT Main Status(MainThread와 StatusThread, LogThread 공유)
	OHTMainStatus *gm_OHTMainStatus;

    /// HWSet Status(StatusThread와 HWSetThread 공유)
    HWSetStatus *gm_HWSetStatus;

    //2013.07.11 doori.shin
    //Function flag(Task Thread와 ExtraJobThread 공유)
	MAPMAKE_STATUS *m_MapMake_Status;

	MAP *m_MapData;                     ///< Map Data
	MAP *m_TmpMapData;                  ///< Temp Map Data(for Map Make)
	MapDataManager *m_MapDataManager;   ///< MapDataManager

	//2013.07.09 doori.shin 클래스 선언
	StationDataManager *m_StationDataManager;   ///< Station Data Manager
	TeachingDataManager *m_TeachingDataManager; ///< Teaching Data Manager

	MapMakeProcess *m_MapMakeProcess;

	ServoMonitoring	*m_pServoControl;
	
	// Data
	STD_CID *m_CIDData_STD;
	STD_Collision *m_CollisionData_STD;
	STD_FlowControl *m_FlowControlData_STD;
	STD_MAP *m_MapData_STD;
	STD_Station *m_StationData_STD;
	STD_Teaching *m_TeachingData_STD;
	STD_PathData *m_PathData_STD;

	// Data Manager
	CIDManager_STD *m_CIDManager_STD;
	CollisionManager_STD *m_CollisionManager_STD;
	FlowControlManager_STD *m_FlowControlManager_STD;
	MapDataManager_STD *m_MapManager_STD;
	StationManager_STD *m_StationManager_STD;
	TeachingManager_STD *m_TeachingManager_STD;
	PathDataManager_STD *m_PathDataManager_STD;

	

	//Auto Map Teaching
//	STD_MapMakerFile *m_MapMakerFile_STD;
	MapMakerFileManager_STD *m_MapMakerFileManager_STD;

	//2013.08.07 doori.shin
	ExtraJobThread *m_ExtraJobThread;

	// -------------------------------------------
	// 2. Command, Communication Class
	// by zzang9un 2013.01.23 : Socket을 모두 ServerSocket으로 변경
	CmdManager *m_CmdManager;       ///< Command Manager Class
	PathSearch *m_PathSearch;
    PassPermit *m_PassPermit;

	bool checkLogDrive;
	bool checkLogFolder;

//	SOCKET ClientInfo;
	//혼용 PIO
	EQPIO_TYPE_INFO m_nEQPIO_Type;

	UdpCom  *m_UDPComm;
	UdpDiag *m_UDPDiag;

	// -------------------------------------------
	// 3. HW Set
	HWCommon *m_HWCommon;               ///< HWCommon Class
	HWSet *m_HWSet;                     ///< HWSet Class
	HWControlThread *m_HWControlThread; ///< HWControlThread Class
	HwMask	*m_MaskOfHW;					///< HW_Common Mask

    // -------------------------------------------
	// 4. D,T, M & Execute Thread
	DrivingControl *m_DrivingControl; ///< DrivingControl Class
    TransControl *m_TransControl;     ///< TransControl Class
    ManualControl *m_ManualControl;   ///< ManualControl Class
	ExecuteThread *m_ExecuteThread;   ///< ExecuteThread Class

    // -------------------------------------------
	// 5. Status Thread
	StatusCheckThread *m_StatusCheckThread; ///< StatusCheckThread Class


	DEFAULT_PARAM_SET		*m_pParamSet;		///< 파라미터 집합: 현재 사용되는 값
	OHTMainStatus *m_pOHTMainStatus;
	OHTDetect		*m_pOHTDetect;
//	AvsControl *m_AvsControl;
	// 6. Monitoring Thread
//	HID_VII_ZR *m_HID;
//	IPCLibrary *m_IPCLoadLib;
//	NetworkSinalLib *m_NetworkDll;
//	CPUandMemInfo *m_CPUandMemInfo;
//	MONITORING *m_EtcMonitoring;
	//m_SigmaMonitor = NULL;	//new SigmaMonitor();
//	MonitorControlThread *m_MonitorControlThread;	///< MonitorControlThread Class

    // 초기화 관련 함수
    void OHTInit();
	void CreateClass();
	void DestroyClass();
	void CaptureScreen();
	
	//2014.04.15 doori.shin Thread check를 위한 변수, 각 스레드별 변수가 있음
	int m_nPreExecuteThTick;	///< ExecuteThread
	int m_nPreExtraJobThTick;	///< ExtraJobThread
	int m_nPreHWCtrlThTick;		///< HWControlThread
	int m_nPreTaskCtrlThTick;	///< TaskControlThread
	int m_nPreStatusChkThTick;	///< StatusCheckThread
	int m_nPreLogThTick;	///< StatusCheckThread
	inline BYTE CheckThreadStatus(ThreadCtrl* thread, int& tick);
	inline String GetThreadStatusToString(BYTE status);

	bool    LoadParameter(char* szFilePath, DEFAULT_PARAM_SET* pParamSet);    ///< 파라미터 불러오는 함수
	bool    LoadHighSpeedParameter(char* szFilePath);    ///< parameter for 8.1s jhoun.lim

public:		// User declarations
    __fastcall TOHTMainForm(TComponent* Owner);

    CRITICAL_SECTION	 m_CS;

    // by zzang9un 2014.02.06 : Getter 함수
    TaskControl* GetTaskControl();
//	LList* GetCmdList();
	LList* GetTaskList();
	LList* GetCycleList();
	LList* GetResponseList();
	LList* GetPathList();
	LList* GetPassOpenList();
	LList* GetPassPermitList();

	EXECUTE_INFO* GetExecuteInfo();
    OHTMainStatus* GetOHTStatus();
    HWSetStatus* GetHWSetStatus();
    MAPMAKE_STATUS* GetMapMakeStatus();
    MAP* GetMap();
    MapDataManager* GetMapDataManager();
    StationDataManager* GetStationDataManager();
	TeachingDataManager* GetTeachingDataManager();

	//STD MAP 관련 
	STD_CID* GetCID_STD();
	STD_Collision* GetCollision_STD();
	STD_FlowControl* GetFlowControl_STD();
	STD_MAP* GetMap_STD();
	STD_Station* GetStation_STD();
	STD_Teaching* GetTeaching_STD();
	STD_PathData* GetPathData_STD();

	NODE_INFO_STD_MAP* GetNodeObj(int nNodeNum);	// //AOHC-125 Auto 전환 전, 자가 진단 기능 추가
													//노드 번호를 알때 노드 객체 포인터를 얻어 정보를 얻기위함

	CIDManager_STD* GetCIDDataManager_STD();
	CollisionManager_STD* GetCollisionDataManager_STD();
	FlowControlManager_STD* GetFlowControlDataManager_STD();
	MapDataManager_STD* GetMapDataManager_STD();
	StationManager_STD* GetStationDataManager_STD();
	TeachingManager_STD* GetTeachingDataManager_STD();
	PathDataManager_STD* GetPathDataManager_STD();

    //Auto Map Maker Teaaching
//	STD_MapMakerFile* GetMapMakerFile_STD();
	MapMakerFileManager_STD* GetMapMakerFileManager_STD();

	MapMakeProcess* GetMapMakeProcess();

	DEFAULT_PARAM_SET* GetDefaultParam();
	
	CmdManager* GetCmdManager();
	PathSearch* GetPathSearch();
	PassPermit* GetPassPermit();
    HWCommon* GetHWCommon();
    HWSet* GetHWSet();
    DrivingControl* GetDrivingControl();
    TransControl* GetTransControl();
	ManualControl* GetManualControl();
	StatusCheckThread* GetStatusCheckThread();

	UdpCom* GetUDPCom();
	UdpDiag* GetUDPDiag();

    
	uVERSION_DATA GetOHTProgramVersion();

//	AvsControl* GetAvsControl();
//	AvsControlThread *m_AvsControlThread;

	//2015.03.08 doori.shin : Data 업데이트 변경으로 추가
	void SetMap(MAP* mapData);
	void SetMapDataManager(MapDataManager* mapDataManager);
	void SetStationDataManager(StationDataManager* stationDataManager);
	void SetTeachingDataManager(TeachingDataManager* teachingDataManager);


	void SetCID_STD(STD_CID* CIDData);
	void SetCollision_STD(STD_Collision* CollisionData);
	void SetFlowControl_STD(STD_FlowControl* FlowControlData);
	void SetMap_STD(STD_MAP* MapData);
	void SetStation_STD(STD_Station* StationData);
	void SetTeaching_STD(STD_Teaching* TeachingData);
	void SetPathData_STD(STD_PathData* PathData);



	void SetCIDDataManager_STD(CIDManager_STD* CIDDataManager);
	void SetCollisionDataManager_STD(CollisionManager_STD* CollisionDataManager);
	void SetFlowControlDataManager_STD(FlowControlManager_STD* FlowControlDataManager);
	void SetapDataManager_STD(MapDataManager_STD* MapDataManager);
	void SetStationDataManager_STD(StationManager_STD* StationDataManager);
	void SetTeachingDataManager_STD(TeachingManager_STD* TeachingDataManager);
	void SetPathDataManager_STD(PathDataManager_STD* PathDataManager);

	void SetMapMakerFileManager_STD(MapMakerFileManager_STD* MapMakerFileData);

    void Update_UI();
	String StatusToString(BYTE status);
	void __fastcall SetInputMask(TCheckBox *CB_InputMask, TEdit *Edt_Mask_Num, TEdit *Edt_Mask_Value);

    // 이적재 롱런 테스트를 위해 추가
	//EXECUTE_TRANS_INFO testTrans;
	//EXECUTE_MANUALCONTROL_INFO testJog;

	//2013.05.27 ji7412.lee : Remocon객체를 반환하기 위한 함수
	Remocon* GetHWSetRemocon();

	LCD* GetHWSetLCD();

	// By elibeli2 2014.01.23 Origin Check
	int GetReadyToOperation();

	void Set_LogFolder(bool bflag);

	 // by zzang9un 2013.02.19 : 버전 변수 추가
	TVersionInfo VerInfo;

	void Set_EQPIO_Type(int);
	int  Get_EQPIO_Type();
	
	int g_DataTransProgressStatus;//데이터 전송 진척 보고 관련 OHT 내부 상태 저장용
	int g_MAPUpdateStatus;// 맵 업데이트 관련 상태 저장용

	// Map UpdateResult
	unsigned int m_MapUpdateResult;
	unsigned int m_ParamUpdateResult;

	void ForceClose(void);
	
	void HoistParam_Read(int iIndex, double &dbl_Pluse, double &dbl_User);
	void DeviceDifference_Read(double &dbl_Pluse);
	bool HoistParamSaveParameter(PKT_TP2OHT_HOIST_TEACH_COMMAND PKT);
	
	
	//  UBG 패턴 관련 함수
	//	UBG 패턴 한개당 48개 ascii 리턴됨.
	static const int COUNTOF_PATTERN_ASCII = 48;
	static const int COUNTOF_PATTERN = 16;
	bool m_bUBGINILoaded;
	char m_szUBGInfo_INI[COUNTOF_PATTERN][COUNTOF_PATTERN_ASCII];  // INI에서 읽은 값
	char m_szUBGInfo_Comm[COUNTOF_PATTERN][COUNTOF_PATTERN_ASCII]; // 대차센서 Comm으로 받은 값
	bool LoadPatternData(char *path);
	std::vector<int> m_nArrCheckPattern;	// 사용자가 체크하고자 하는 패턴 배열

	void AddUBGPattern(int nPatternNo1234, char *szRecv);
	bool DoCompUBGPattern(int nPatternNo0to15);	//패턴 비교
	bool WriteUBGRecv_INI();

	// IP 정보를 로그로 기록함. 프로그램 실행시 한번 동작
	void WriteLog_NetworkInfo();
	bool IoTGatewayCheck();
	bool m_bErrIotGateway; // IoT 게이트웨이 세팅에 문제있다는 플래그
	bool m_bErrOHTTypeMatching; // OHTType Matching이 문제 있다는 플래그
	void ThreadCycleLog(char *msg);

};
//---------------------------------------------------------------------------
extern PACKAGE TOHTMainForm *OHTMainForm;
//---------------------------------------------------------------------------
//AOHC-204 Thread 별 수행 시간 감시 기능
// 스레드 주기 감시. 일정시간(1~3초) 동안 모든 Cycle 시간에 대해서 min, max, avr 구함
// 몇주기 돌았는지도 기록
class ThreadTimeMonitor
{

	DWORD min, max, sum;
	int nIdx;
	DWORD dwStart;

public:
	ThreadTimeMonitor();
	void Execute(char *name, DWORD time_ms);
};
#endif
