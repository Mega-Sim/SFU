/**
@file	 DrivingControl.h
@version OHT 7.0
@brief	 DrivingControl Class Header File
*/
#ifndef DrivingControlH
#define DrivingControlH

#include <windows.h>
#include <stdio.h>
#include "List.h"
#include "ExecuteInfo.h"
#include "Def_DrivingControl.h"
#include "VirtualDrivingControl.h"
#include "HWSet.h"
#include "MAP.h"
#include "LogHandler.h"	// Log 작성 목적
#include "DiagManager.h"
#include "Def_DefaultIniParam.h"
#include "STD_PathData.h"


#if(OHT_NAME == OHT_NAME_STD_V81S)
#define DRIVING_GEAR_RATIO      114.7341619 * 0.8
#define AMCDECEL_VHLDETECT		297318     //3.2m/s^2
#else
#define DRIVING_GEAR_RATIO      114.7341619
#define AMCDECEL_VHLDETECT		397009     //3.5m/s^2
#endif
#include "Def_MCCParam.h"

/**
@class	 DrivingControl
@date	 2012.10.17
@author  ehoto
@brief	 주행 관련 인터페이스 클래스

		1. Node Update - Node Count + BCR
		2. 주행 Profile 생성 (거리, 조향, 속도)
		3. 주행 속도 조절 (근접제어, 장애물 등..)
		4. 정위치 - 주행 Tag / 이적재 Tag
*/
class DrivingControl:public VirtualDrivingControl//VirtualModuleControl
{
private:
	LogUnit *pLogDrivingError;      ///< DrivingError 로그 생성을 위한 변수
	LogUnit *pLogBCR;      			///< BCR 로그 생성을 위한 변수
	LogUnit *pLogDetect;      ///< Detect 로그 생성을 위한 변수
	LogUnit *pLogCIDMonitor;      			///< CID Monitoring 로그 생성을 위한 변수
	LogUnit *pLogMCC;
	FDCUnit *pFDCUnit;
	// 사용하는 HWSet
	BCR				*m_pTransBCR;
	BCR				*m_pDrivingBCR;
	Driving			*m_pDrivingAxis;
	DrivingTag		*m_pDrivingTag;
	Obstacle		*m_pObstacle;
	OHTDetect		*m_pOHTDetect;
	QR_Reader		*m_pLeftQRR;
	QR_Reader		*m_pRightQRR;
    Steer			*m_pSteer;
	TransStopTag	*m_pTransStopTag;
	PassBoxPIO		*m_pPassBoxPIO;
	MTLPIO          *m_pMTLPIO;
	ServoMonitoring	*m_pServoControl;
	OHTMainStatus *m_pOHTMainStatus;

	EXECUTE_INFO *m_pExecuteInfo;
	HWSet *pHWSet;
	DEFAULT_PARAM_SET *m_defualtparam;
	MCC_DRIVING_PARAM_SET	m_mccparam;

	DiagManager*	m_DiagManager;
	PATH_DIRECTION_INFO m_Dectect_Dir;
	PATH_OBS_DIRECTION_INFO m_Dectect_OBS_Dir;

private:	// 주행 시 사용하는 내부 변수 // 기본 상태 변수
	USER_DETECT_INFO				m_Detect;	///< 제어기로 보내는 Detect 정보
	int	m_nPassBoxPIOStep;
	int m_nElevetorRideStep;
	int m_nElevetorDirection;
	int m_nElevetorOffStep;
	bool m_bsecond;  // CID Common Node TG
	bool m_CID_Pause; // CID Pause Check TG  ture : pause, false : resume
	bool m_CID_Status_Pause;
	bool m_CIDComptFlag;
	bool m_CIDMonitoringFlag; // CID Monitoring Flag
	bool m_CIDLinkFlag;  //CID Link 결과 확인
	bool m_CIDStatusFlag; //CID Status Flag
	bool m_CIDNameReadFlag;  // CID장치에 이름을 설정하고 (O명령) 읽어야 한다는 것을 명시
	
	bool m_Igro_CIDStatusPuaseFlag;
	bool m_CIDComptTryFlag; //CID Compt 시도 확인 Flag
	bool m_CIDStatusPauseFlag;//CID-R이상 또는 Link TimeOut으로 OCS보고하고 pause상태임을 나타내는 변수
	int m_CIDPauseTime;//CID-R이상 또는 Link TimeOut으로 OCS보고하고 pause상태임을 나타내는 변수

	bool m_CID_COMMON_bit_off_need;

	// CID 관련 변수 추가, Kunjin, 20161030   /////////////////////////
	bool m_bCIDNotUse; // CID 상태 정보 (Select On 동작 여부 판단용)
	double m_dwCIDSelectOnTime;
	int m_CID_Offet_Target;  // 주행 Tag부터 광 Fiber까지의 Offset
	bool m_bLinkFail;
	bool m_bLinkDisconnected;
	bool m_bInitialLink;
	CID_DIRECTION m_CIDDirection;
	int m_tmpCurrentResetNode;
	int m_tmpCurrentStopNode;
	
	unsigned char m_Diag_CID_R_ID[9][6];//CID-R 정보 수신 후 처리 보존용,  0~7: 실제 데이터, 8: Not found data 0
	int m_Diag_ResetNode[9];//CID-R 정보 수신 후 처리 보존용
	int m_Diag_StopNode[9];//CID-R 정보 수신 후 처리 보존용
	unsigned char m_Diag_CID_index;//CID-R 정보 수신 후 처리 보존용

	int m_nCIDTerminateCount;//통과 완료 신호 유지를 위해 사용 
	unsigned char m_CIDOperationStarted;
	unsigned char m_job_change_flag_no_cid_area;
	unsigned char m_job_change_flag_exception_CommonTag; //작업 변경 시, common tag 완료처리 exception flag 
	int m_CurrCIDStartnode;  //현재 CID 구간에 진입할 당시의 Start node
	unsigned char m_sCIDStatus[3];
	int m_ntempNodeOffset;
		
//	int m_CID_Offet_Target_Left;
//	int m_CID_Offet_Target_Right;
	/////////////////////////////////////////////////////////
	
	CID_CTRL_INFO Info;
	CID_4WAY_INFO CID_4WAY_CONTROL_INFO;
	int m_nMTLTakeOutPIOStep;
	int m_nMTLTakeInPIOStep;
	double m_RunOffsetDistance;//남은거리
	int m_nPrePathType;

	AnsiString m_strOhtName;	// OHT NAME - ex) "OHT001"
	//int m_nCidControlStep;
	int m_nCidManualStep;
	bool m_bJobChange;
	bool m_job_change_without_link_complete;

	int     bCountOfCID_Delay;			// CID Info - ID 124 데이터 수신 delay을 위한 변수
	int     bCountOfCIDLink;        //링크 연결 관련
	int     bCountOfCIDDualOccup;  	//동신진입 관련
	int     bCountOfCIDComptACK;  	//완료처리 응답 관련

	/*
	리프터 사전 진입허가 신청상태
	만일 Lifter In Node 도달 전 (Lifter Pre Node 구간) VHL_ELEVETOR_PREPASS_STEP_COMPLETE 가 되면
	Lifter In 스테이터스 머신은 타지 않고 바로 다음단계로 넘어간다

	VHL_ELEVETOR_PREPASS_STEP_NONE		 = 0,	// 진입허가 진행하지 않음
	VHL_ELEVETOR_PREPASS_STEP_TOUCH			,	// 진행중이었음 (완료까지는 못감)
	VHL_ELEVETOR_PREPASS_STEP_COMPLETE		,	// 진입허가 완료. RIDE동작 불필요. 바로 진입할 것
*/
	int m_iLifterPrePass;
	bool m_IsLifterCheck;

	bool m_IsMTLCheck;
	int nEnforcedSelectOffNodeID; 

	int m_OverRunCount;  //OverRun 횟수 카운트 변수
//	int m_OverRunCount_E2313; // ERR_NOTMATCH_TAGLENGTH_FINALSTATION_TAG_MOVE_BACK_LIMIT OverRun 횟수 카운트 변수
//	int m_OverRunCount_E2314; // ERR_NOTMATCH_TAGLENGTH_FINALSTATION_IN_POSITION_ERROR   OverRun 횟수 카운트 변수
//	int m_OverRunCount_E250; // ERR_MARK_RSTOPSEN_OFF OverRun 횟수 카운트 변수
	int m_MoniCFNode;
	int m_MoniCFNode2;

	bool m_bFDCMarkFailFlag;
	bool m_bFDCEarlyPathFlag;
	bool m_bFDCLatePathFlag;
protected:
	virtual	EXECUTE_ACK	Stop(void);	///< 비상 정지
	virtual	EXECUTE_ACK	Reset(void);	///< 에러 리셋
	virtual	EXECUTE_ACK	TurnOff(void);	///< 기동 Off
	virtual	EXECUTE_ACK	TurnON(void);	///< 기동 ON


public:
		DrivingControl();
		~DrivingControl(void);

private:
	void	init(void);
	void	stop();
	void    turnoff();
	double	getOptimalSpeed(
					double Distance, double SpeedIn, double AccelIn,
					double AccelOut, double SpeedOut, double UniformSpeedRatio
					);	// 전부 양수 값;


//------------------------------------------------------------------------------
//1	Execute에서 확인하는 돌리는 함수
//1	Return 하기 위한 함수
//------------------------------------------------------------------------------
public:
	EXECUTE_POSITION_INFO	GetPositionInfo();
	int						GetMapData(List<EXECUTE_RESPONSE_MAPMAKING_INFO*>* pListOfMapData,
												volatile MAP_INFO_ON_ADDING_UNION *pMapInfoOnAdding);		///< MapMaking 정보를 받아오는 함수

private:
	void		updateTagInfo();							///< Tag정보 업데이트

//------------------------------------------------------------------------------
public:
	UINT		CheckAbnormalOnWait(void);				///< 대기 시 이상 현상 확인 함수 (Step 형태로 구현 - Thread가 돌림)
	UINT		CheckAlways(void);				///< 항시 이상 현상 확인 함수 (Step 형태로 구현 - Thread가 돌림)
	bool		DeleteCmd(void *pCmd);						///< 할당 된 명령을 소멸함
	
//------------------------------------------------------------------------------

//1 --- SEMI WAY -----------------------------------


public:
	int		Manual_Mark(MANUAL_MARK_CMD_TYPE pType);				///< 외부에서 호출 되는 함수: Mark 관련
	int		Manual_Driving	(MANUAL_DRIVING_CMD_INFO Info);		///< 외부에서 호출 되는 함수: Jog 이동 관련
	int		AutoTaeching_Driving	(MANUAL_DRIVING_CMD_INFO Info);
//	void	Manual_DrivingAbnormalCheck();
	int 	Manual_IdReader(DRIVING_MANUAL_CMD_MARK_VALUE MarkType
									, DRIVING_MANUAL_CMD_FRONT_ID_READER_VALUE CmdType
									, int* pnResultID, double* pdResultOffset);	///< ID Reader 제어 명령
	int 	Manual_FrontObservation(DRIVING_MANUAL_CMD_FRONT_OBSERVATION_VALUE Level);	///< 전방 감지 제어 명령

private:
	int 	manual_Mark(MARK_TYPE MarkType);

//------------------------------------------------------------------------------
//1 --- NEW WAY -----------------------------------

private:
	LogUnit		*pLogUnit;	///< 로그 저장 효율을 높이기 위해 가지고 있는 포인터 : 파일 이름으로 포인터를 찾는 작업을 미실시 

public:
	DRIVING_PARAM_SET		*m_pParamSet;		///< 파라미터 집합: 현재 사용되는 값
	CID_Monitoring_PKT 		TEST_PKT_Monitor;

private:
	//2	파라미터 집합
	// DRIVING_PARAM_SET		*m_pParamSet;		///< 파라미터 집합: 현재 사용되는 값
	DRIVING_PARAM_SET		*m_pTmpParamSet;	///< 파라미터 집합: 업데이트 할 때 사용되는 값

	//2	명령의 분류 및 저장 (초기화 시 전부 삭제)
	EXECUTE_DRIVING_INFO m_LastDrivingCmdInfo;
	CLASSIFIED_DRIVING_CMD_SET	m_ClassifiedCmd;	///< 항목에 따라 분류된 명령 ( 1. Node  / 2. Steer / 3. 방향 / 4. 속력 / 5. Path 정보  / 6. 목적지 정보 )
	EXECUTE_QUICK_CMD			m_QuickCmd; 		///< 받은 QuickCmd 저장: 긴급 정지 / Pause / Resume / Reset  : 주행 명령 주는 곳에서 최종 반영 --> Quick Cmd 변경에 의한 Event 처리 추가 필요.

	//2	측정 결과 저장
	bool m_bOnMapMake;	///< MapMake 여부 Flag
	bool m_bOnBackMove;	///< BackMove 여부 Flag
	MAP_MAKER_TYPE m_bMapMakeType;	///< MapMake Type    //0x00 전체 Map Maker, 0x01 : 주행 MapMaker, 0x10: 이적재 MapMaker
	List<EXECUTE_RESPONSE_MAPMAKING_INFO*>	m_ListOfTmpStationData;	///< 다음 노드를 만나기 전까지 Station 정보를 저장하는 List
	List<EXECUTE_RESPONSE_MAPMAKING_INFO*>	m_ListOfMapData;		///< MapMake 결과를 입력하는 List : Execute Thread 에서 함수를 호출하여 정보를 얻어감: Manual Reset 시 정보 삭제

	//2	정보 집합 (초기화 시 조건부 삭제)
	DRIVING_INFO_SET		m_InfoSet;				///< 주행  관련 정보 집합: 초기화 시 전부 삭제
	POSITION_STATUS			m_PreviousArrivalInfo;	///< 사전 도착 정보: 주행 완료 시 "주행  관련 정보 집합"(m_InfoSet)에서 Position.Mark 정보를 받아옮: Manual Reset 시 정보 삭제
	EXECUTE_POSITION_INFO	m_PositionInfo;			///< 보고용 정보: ① 주행 시: PreCheck 감지 시 업뎃, ② 주행 Mark 완료 시, ③ Manual Mark 완료 시, ④ Manual 에서 노드 읽혔을 때 처리
	
public:	//2	 외부에서 호출 되는 함수: 함수명의 시작을 대문자로 표시
	EXECUTE_ACK InsertGeneralCmd(void *pDrivingInfo);	//EXECUTE_DRIVING_INFO
	UINT		ExecuteJob();

	int 	ExecuteManual(DRIVING_MANUAL_CMD_INFO Info);		///< 외부에서 호출 되는 함수: Manual 명령 실행

	//2 Parameter  관련
	void*	LoadParam(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory);
	bool	RequestParamUpdate(void* pParameter);
	bool	IsParamUpdated();


	//2	파라미터 관련 함수
	bool loadParameter(char* szFilePath, DRIVING_PARAM_SET* pParamSet);	///< 파라미터 불러오는 함수
	bool saveParameter(char* szFilePath, DRIVING_PARAM_SET* pParamSet);	///< 파라미터 저장하는 함수
	bool updateParameter();											///< 파라미터 업데이트 하는 함수

	bool loadParameterRecovery(char* szFilePath, DRIVING_PARAM_SET* pParamSet);
	int  m_ParamChehckError;

private:											

	//2	초기화 함수
	void initStatusSet();					///< 상태 정보를 초기화 하는 함수
	void initCommandSet();				///< 명령 정보를 초기화 하는 함수
	void initInfoSet();						///< 주행 HW 관련 정보를 초기화 하는 함수
	void initPreviousArrivalInfo();	///< 이전 도착 정보를 초기화 하는 함수
	void setPreviousArrivalInfo(POSITION_STATUS PositionStatus);	///< 이전 도착 정보를 설정하는 함수: Station 에서 출발 목적으로 Path 정보를 설정하는 함수 
	void setCompensationInfo(POSITION_STATUS PreviousArrivalInfo);	///< 거리 보정의 요소인 '출발 Station Offset'을 설정함 : m_InfoSet.Position.Compensation.dPreArriveOffset

	//2	명령 분류 함수
	EXECUTE_ACK setStationOffset(void *pDrivingInfo);		///< 명령으로 부터 Station Offset 정보를 설정함
	EXECUTE_ACK makeCmdInfoLists(void *pDrivingInfo);		///< 명령으로 부터 List 정보를 만들어 냄
	EXECUTE_ACK makeCmdInfoListsBack(void *pDrivingInfo);		///< 명령으로 부터 List 정보를 만들어 냄
	bool isMapMake(void *pDrivingInfo);							///< 명령이 Map Make 인지 확인
	bool isBackMove(void *pDrivingInfo);							///< 명령이 Map Make 인지 확인
	bool clearCmdInfoLists();								///< List의 정보를 전부 제거함
	bool clearCmdInfoListsLessThan(int nNodeCount);	///< nNodeCount 이전의 List의 정보를 제거함
	bool clearCmdInfoListsLessAfter(CLASSIFIED_DRIVING_CHANGE TempDrivnigInfo);	///< nNodeCount 이후의 List의 정보를 제거함
	bool checkCmdInfoNodeList(List<DRIVING_CMD_INFO_OF_NODE*> *pList, int CheckNodeID, CLASSIFIED_DRIVING_CHANGE* ChagneInfo);  ///< List 정보로부터 Node위치 확인
	bool clearDrivingCtrlInfoSet();						///< m_InfoSet의 명령 Flag를 초기화 함
	// makeCmdInfoLists sub Function
	bool makeList(List<DRIVING_CMD_INFO_OF_NODE*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< 명령으로 부터 Node Info List 정보를 만들어 냄
	bool makeList(List<DRIVING_CMD_INFO_OF_STEER*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< 명령으로 부터 Steer Info List 정보를 만들어 냄
	bool makeList(List<DRIVING_CMD_INFO_OF_DIV*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< 명령으로 부터 Steer Info List 정보를 만들어 냄
	bool makeList(List<DRIVING_CMD_INFO_OF_DIRECTION*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);		///< 명령으로 부터 Direction Info List 정보를 만들어 냄
	bool makeList(List<DRIVING_CMD_INFO_OF_SPEED*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo, DRIVING_CMD_INFO_OF_TARGET TargetInfo);			///< 명령으로 부터 Speed Info List 정보를 만들어 냄
	bool makeList(List<DRIVING_CMD_INFO_OF_PATH*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< 명령으로 부터 Path Info List 정보를 만들어 냄

	bool CancelList(List<DRIVING_CMD_INFO_OF_NODE*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);			///< 명령으로 부터 Node Info List 정보를 삭제함.
	bool CancelList(List<DRIVING_CMD_INFO_OF_STEER*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);			///< 명령으로 부터 Steer Info List 정보를 삭제함.
  	bool CancelList(List<DRIVING_CMD_INFO_OF_DIV*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);			///< 명령으로 부터 Steer Info List 정보를 삭제함.
	bool CancelList(List<DRIVING_CMD_INFO_OF_DIRECTION*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);		///< 명령으로 부터 Direction Info List 정보를 삭제함.
	bool CancelList(List<DRIVING_CMD_INFO_OF_SPEED*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);			///< 명령으로 부터 Speed Info List 정보를 삭제함.
	bool CancelList(List<DRIVING_CMD_INFO_OF_PATH*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);			///< 명령으로 부터 Path Info List 정보를 삭제함.

	bool makeBackList(List<DRIVING_CMD_INFO_OF_NODE*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< 명령으로 부터 Node Info List 정보를 만들어 냄
	bool makeBackList(List<DRIVING_CMD_INFO_OF_SPEED*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo, DRIVING_CMD_INFO_OF_TARGET TargetInfo);			///< 명령으로 부터 Speed Info List 정보를 만들어 냄
	bool makeBackList(List<DRIVING_CMD_INFO_OF_PATH*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< 명령으로 부터 Path Info List 정보를 만들어 냄


	//1	★★★ ExecuteJob 내부 처리 함수 ★★★

	int	checkError(DRIVING_INFO_SET *pInfoSet);	///<00. 에러 확인

	UINT executeOpenPassBox	(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);
	UINT executePreEvevetor(DEVICE_NODE_TYPE DirType);
	UINT executeRideEvevetor(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);
	UINT executeOffEvevetor(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);
	
	UINT executeGoInit		(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 1. 초기 설정
	UINT executeGoPrepare	(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 2. 주행 준비: 축, 조향, 전방, ID
	UINT executeGoFirstNode	(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 3. 주행: 축, 조향, ID
	UINT executeGoRun			(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 3. 주행: 축, 조향, ID
	UINT executeGoMark			(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 4. 마크: 축, 조향, ID
	UINT executeGoCheckStop	(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 5. 정지 확인: 축, 조향, ID 
	UINT executeGoComplete	(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 6. 완료: 축, 조향, ID


	UINT checkPosition					(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet);		///< 위치 정보 확인 및 지나간 Classified List 정리
	UINT checkSteer						(DRIVING_INFO_SET *pInfoSet);	///< 조향 상태 설정 및 명령 생성
	UINT checkFrontObservation		(DRIVING_INFO_SET *pInfoSet);	///< 전방 감지 상태 설정 및 명령 생성
	UINT checkAxisFirstNode			(DRIVING_INFO_SET *pInfoSet);	///< 축 명령 생성
	UINT checkAxisRun					(DRIVING_INFO_SET *pInfoSet);	///< 축 명령 생성
	UINT checkAxisMark					(DRIVING_INFO_SET *pInfoSet);	///< 축 명령 생성
	UINT checkOBSAreaforNoFollowControl (DRIVING_INFO_SET *pInfoSet);
	UINT checkServoTorqueFlag			(DRIVING_INFO_SET *pInfoSet);  /// 서보토크 확인

	UINT checMTLTakeOut			(DRIVING_HW_INFO_SET HwInfo);  /// 빼냄 확인
	UINT checMTLTakeIn			(DRIVING_HW_INFO_SET HwInfo);  /// 투입 확인

	UINT checMTLTakeOut_PIO8			(DRIVING_HW_INFO_SET HwInfo);  /// 빼냄 확인
	UINT checMTLTakeIn_PIO8			(DRIVING_HW_INFO_SET HwInfo);  /// 투입 확인
	UINT checkJobChange          (DRIVING_INFO_SET *pInfoSet);

	void CID_LR_ON(bool LightGuideDirectionIsLeft);
	int CID_Comm_Time2Dist(CID_CTRL_INFO* Info);
	int controlCID(CID_CTRL_INFO* Info, executeCID_CMD iCmd, DRIVING_INFO_SET *pInfoSet, DRIVING_HW_INFO_SET HwInfo);
	
	int completeCIDComm(CID_CTRL_INFO* Info, DRIVING_INFO_SET *pInfoSet);
	bool checkCommandTarget4CID(CID_CTRL_INFO* Info);

	bool IO_INFO_CID(void);
	void IO_INFO_CID_ALL(void);
	UINT Manual_CID(int iManualCtrl);

	CID_DIRECTION CID_Direction_Chk(CID_CTRL_INFO* Info,  DRIVING_INFO_SET *pInfoSet, int CID_NearestResetNode, int  CID_NearestStopNode);
	
	UINT setCID_VHL_Name(char* OHTNAME);
	UINT requestCID_VHL_Name();
	
	int checkCID(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet);
		
	bool NormalCid_Return(CID_CTRL_INFO* Info);
	bool CommonCid_Return(CID_CTRL_INFO* Info, DRIVING_INFO_SET *pInfoSet);

	unsigned char CID_ID_FIND(unsigned char* CID_UniqueID);
	void CID_ID_Buffer_Clear(unsigned char index);
	void CID_Monitor_Status(CID_CTRL_INFO* Info);
	void CID_R_Status_Chk();

	void CID_N_BRANCH_SELECT_OFF();

	bool MarkFail_Recovery_Driving();
	bool m_bIsRecovery;
		
	bool bDirCIDChage;		// 방향전환 완료여부 (true : 진행중. false : 완료함)
	int tmpStopID;

	UINT checkBackPosition					(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet);		///< 위치 정보 확인 및 지나간 Classified List 정리
	UINT checkBackSteer						(DRIVING_INFO_SET *pInfoSet);	///< 조향 상태 설정 및 명령 생성
	UINT checkAxisBackRun					(DRIVING_INFO_SET *pInfoSet);	///< 축 명령 생성
	void measureDist	(DRIVING_INFO_SET InfoSet, List<EXECUTE_RESPONSE_MAPMAKING_INFO*> *pListOfTmpStationData, List<EXECUTE_RESPONSE_MAPMAKING_INFO*> *pListOfMapData);	///< MapMake 시 사용 되는 함수 

	EXECUTE_RESPONSE_MAPMAKING_INFO* measureNodeDist				(DRIVING_INFO_SET InfoSet, UINT *puCurrNID, double *pdCurrNodeMarkPosition, bool *pbInit);			///< Node MapMake 시 사용 되는 함수 
	EXECUTE_RESPONSE_MAPMAKING_INFO* measureTagStatationDist	(DRIVING_INFO_SET InfoSet, UINT uCurrNID, double dCurrNodeMarkPosition, bool bInit);				///< Tag Station MapMake 시 사용 되는 함수 
	EXECUTE_RESPONSE_MAPMAKING_INFO* measureQRStatationDist	(bool bIsLeft, DRIVING_INFO_SET InfoSet, UINT uCurrNID, double dCurrNodeMarkPosition, bool bInit);	///< QR Station MapMake 시 사용 되는 함수 
	void insertStationInfo(EXECUTE_RESPONSE_MAPMAKING_INFO NodeInfo, List<EXECUTE_RESPONSE_MAPMAKING_INFO*> *pListOfTmpStationData, List<EXECUTE_RESPONSE_MAPMAKING_INFO*> *pListOfMapData);	///< Node 만날 때 마다 이전 Station 정보를 정리하여 List 에 투입

	//2	 H/W 제어 명령
	DRIVING_HW_INFO_SET	getHWInfo();
	
	int controlAll(DRIVING_CTRL_INFO_SET CtrlInfo);			///< Axis, Steer, Front
	
	int	controlAxis(CTRL_INFO_OF_SYNC_AXIS_MOVE AxisInfo);		///< 축 제어 명령	// 2륜 주행 설비를 위한 함수: 추후 구현 : void	controlAxis(CTRL_INFO_OF_ASYNC_AXIS_MOVE AxisInfo); // 2륜 주행 설비를 위한 함수loadParameter(char * szFullPath)
	int	controlSteer(CTRL_INFO_OF_STEER SteerInfo);				///< 조향 제어 명령
	int	controlFrontObservation(CTRL_INFO_OF_FRONT_DETECT DirectionCmd);			///< 전방 감지 제어 명령
	int controlIdReader(MARK_TYPE MarkType, CTRL_INFO_OF_ID_READER_CMD_TYPE CmdType);		///< ID Reader 제어 명령
	
	int	controlIdReader(MARK_TYPE MarkType, CTRL_INFO_OF_ID_READER_CMD_TYPE CmdType, int* pnResultID);		///< ID Reader 제어 명령
	int	controlIdReader(MARK_TYPE MarkType, CTRL_INFO_OF_ID_READER_CMD_TYPE CmdType, int* pnResultID, double* pdResultOffset);		///< ID Reader 제어 명령

	//2	Mark 제어 명령
	int markOnNode			(				DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID, int nNodeCount
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID);
		
	int markOnTagStation	(				DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID 
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID,bool bOverrunIngn);

	int markOnTagStationBack(				DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID);


	int markOnQRStation	(bool bIsLeft, 	DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID);

	int markOnQRStationDiffType	(bool bIsLeft, 	DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID);

	int markOnQRStationDiffType2	(bool bIsLeft, 	DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID, bool bOverrunIngn, double dSTBOffset = 0);

	//2	주행 준비 / 마무리
	bool checkReadyToRun	(DRIVING_HW_INFO_SET HwInfo, USER_DETECT_INFO DetectCtrl
		, CTRL_INFO_OF_SYNC_AXIS_MOVE* pAxisCmd, CTRL_INFO_OF_STEER* pSteerCmd, CTRL_INFO_OF_FRONT_DETECT* pDetectCtrl, bool bIsManual=false);	// 주행을 시작 가능한 상태 여부 확인 ( 예: 서보 On )

	bool checkCompletion	(DRIVING_HW_INFO_SET HwInfo
		, CTRL_INFO_OF_SYNC_AXIS_MOVE* pAxisCmd, CTRL_INFO_OF_STEER* pSteerCmd, CTRL_INFO_OF_FRONT_DETECT* pDetectCtrl); // 주행 완료 상태 확인 ( 예: 서보 Off )


	//2	Steer Command Maker
	CTRL_INFO_OF_STEER setSteerCmdToCurrPosition(DRIVING_HW_INFO_SET	HwInfo);

	//2	ETC Function
	STEER_HW_INFO_POSITION getSteerPosition(bool bLeft, bool bRight);		///< HW I/O 정보로 Steer 위치 정보로 변환
	STEER_HW_INFO_POSITION getSteerTotalPosition(bool bFrontLeftOn, bool bFrontRightOn, bool bRearLeftOn, bool bRearRightOn);
	DRIVING_CMD_INFO_OF_NODE* getNodeInfo(int nNodeCount);	///< Node Count에 해당하는 Node 정보 포인터 반환
	DRIVING_CMD_INFO_OF_PATH* getPathInfo(int nNodeCount);	///< Node Count에 해당하는 Path 정보 포인터 반환

	double compensate(double dRealPos);			///< 거리 보정 ( 모터 엔코더 위치 → Map 기준 위치 정보로 변환 )
	double getRealPos(double dCompensatedPos);	///< Real 위치로 변환 ( Map 기준 위치 → 모터 엔코더 위치로 변환 )

//	double getSpeedDownPos();	///< 감속 시작하는 위치 Return
	double getSpeedDownPos(double dAccel, double dSpeed, double dDist);	///< 감속 시작하는 위치 Return
	bool isMarkArea();													///< Mark 시작하는 Real 위치 Return
 	bool getSteerCmd(int nNodeCount, STEER_HW_INFO_CMD *pSteerCmd);	///< 입력하는 Node Count 다음의 Path들에서 추후 조향 방향 구하는 함수: 특별히 없으면 false Return
 	bool getSteerCmdOnNBranch(STEER_HW_INFO_CMD *pSteerCmd);		///< N분기에서 가이드에 의해 조향 변경 시 조향 방향 구하는 함수: 특별히 없으면 false Return
	bool getSteerCmdTypeFrom(PATH_STEERING_INFO PathSteerInfo, STEER_HW_INFO_CMD *pSteerCmd);	///< Path 정보에서 Cmd 명령을 받아낼 때 사용
	bool getSteerCmdTypeFrom(PATH_STEERING_INFO PathSteerInfo, STEER_HW_INFO_CMD *pSteerCmd, STEER_HW_INFO_CMD *pSteerKey);	///< Path 정보에서 Cmd 명령을 받아낼 때 사용
	bool isSteerChangeAreaByGuide();	///< N분기에서 가이드 레일에 의해 조향이 바뀌는 영역 내에 있는지를 확인하는 함수
	bool isSteerChangeAreaByGuideNodeCountAdd();	///< N분기에서 가이드 레일에 의해 조향이 바뀌는 영역 내에 있는지를 확인하는 함수
	bool getFrontObservation(int nNodeCount, USER_DETECT_INFO* pDetectInfo);	///< 입력하는 Node Count 다음의 Path들에서 추후 진행 방향 감시 정보 구하는 함수: 특별히 없으면 false Return
	bool getFrontObservationNodeCount(int nNodeCount);	///< 입력하는 Node Count 다음의 Path들에서 추후 진행 방향 감시 정보 구하는 함수: 특별히 없으면 false Return

	bool getDivergenceInfo(int nNodeCount, PATH_DIVERGENCE_INFO* pDivInfo, PATH_DIVERGENCE_INFO* pPreDivInfom, int *NodeCount);	///< 입력하는 Node Count 다음의 Path들에서 추후 진행 방향 감시 정보 구하는 함수: 특별히 없으면 false Return


	bool getAxisCmdInfoBySpeed(int nNodeCount, double *pdSpeed, int *pnStopNodeCount);	///< 속도에 의한 주행 정보 획득
	bool getAxisCmdInfoBySteer(int nNodeCount, STEER_HW_INFO_POSITION CurTotalSteerPos, int *pnStopNodeCount1, int *pnStopNodeCount2, STEER_HW_INFO_POSITION RealCurTotalSteerPos);		///< 조향에 의한 주행 정보 획득
	bool getAxisCmdInfo(DRIVING_INFO_SET *pInfoSet, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo);	///< 축의 구동 정보를 받는 함수
	bool getAxisCmdInfoBack(DRIVING_INFO_SET *pInfoSet, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo);	///< 축의 구동 정보를 받는 함수
	double getDist(int nFromNodeCount, int nToNodeCount);	///< nFromNodeCount ~ nToNodeCount 의 거리를 구한다.
	double	getSmoothSpeed(double dDecel, double dDist, DWORD dwTime);///< 조향 사이의 Normal Path를 부드럽게 진행하기 위한 속도
	double getMaxSpeed(double dAccel, double dDecel, double dDist, double dCurSpeed = 0.0, double dTargetSpeed = 0.0);	///< 가감속 거리 조건에서의 최대 속도를 구한다.
	double	getMarkSpeedLimitOnGoing(int nCurrNodeCount);	///< Going 중에 Path 속도를 반영하여, Mark 속도 Limit을 돌려주는 함수

	double getAccelDivisor(int nStartNodeCount);	///< 현재 NodeCounter를 입력하면 감속 줄임 비율이 나온다
	double getDecelDivisor(int nStartNodeCount);	///< 현재 NodeCounter를 입력하면 감속 줄임 비율이 나온다

	bool getMarkStatus();
	void setMarkStatusFalse();	///< 마크 아닌 상태로 변경 시키는 함수
	void setMarkStatusFalse_AutoTeahcing();	///< 마크 아닌 상태로 변경 시키는 함수

	void getPostionInfoOnRunning(DRIVING_INFO_SET InfoSet, EXECUTE_POSITION_INFO *pCurrPositionInfo);
	void getPostionInfoOnMarked(DRIVING_INFO_SET InfoSet, EXECUTE_POSITION_INFO *pCurrPositionInfo);
	void getPostionInfoOnManual(DRIVING_HW_INFO_SET HwInfo, EXECUTE_POSITION_INFO *pCurrPositionInfo);

	void checkStartCmd(EXECUTE_DRIVING_INFO *pTmpDrivingInfo);	///< 마크 상태가 Station 인데, 다음 명령이 Next Node 로의 명령이 아닌 경우 처리
	EXECUTE_ACK checkStartCmdStandard(EXECUTE_DRIVING_INFO *pTmpDrivingInfo);	///< 마크 상태가 Station 인데, 다음 명령이 Next Node 로의 명령이 아닌 경우 처리
	int Set_Node_XorY(EXECUTE_RESPONSE_MAPMAKING_INFO *pMapMakeNodeInfo, UINT nStNode, UINT nEdNode, UINT nDist, bool bSkip);

	int AutoRecovery_Driving(int Error_AV);

	// VHL 엘리베이터 구현 관련 추가 함수 정의
	// bool mbVHL_Lifter_Ride;
	// bool mbVHL_Lifter_Off;
	BYTE cRunNextDivInfo;
	UINT nRunNextNodeID;
	double m_dStationMarkInpos;

	bool   m_bFirstNodeStep;
	int    m_dPreNodecount;
	double m_dPreNodecountPosition;
	double m_dPreNodecountSpeed;
	double m_dPrePrecheckNodecountPosition;
	double m_dCheckPrecheckNodecountPosition;


	//AMC<-> Main 감속도 동기화 처리. puting 20191023
	double m_dCheckAMCDecelPosition;   ///<감속도 변경에 의히 이동하는 최초 위치
	double m_dPreTargetPosition;       ///<주행 중 변경하는 타겟 위치 저장.

	//경사로 구간 Puting 20160811
	BYTE m_cSlopeNodeInfo;  //0xFF : 초기화 , 0x00 일반 구간 : 0x01 : 슬로프구간
	//AnsiString CheckSum_StringToHex(AnsiString sData);
	//AnsiString CheckSum(char* buf, int len);
	AnsiString CheckSum(char* buf, int len, char* check);
	bool Lifter_Prenode_Check(UINT iNodeNum);
	bool bReqLifterCounterReset;

	UINT m_nMarkFailRetryCheckID;

	bool m_bIsFirstNodeCountAdd;
	bool m_bIsAMCVHLDetectIgnore; //대차 무시 플래그 명령 전달 유무 플래그(새로 주행 시 초기화 부분에서 초기화 처리)

	bool b_ReadBcr;  //BCR READ 여부 Check
	bool b_WaitTrg;  //BCR Data가 존재했었는지 Check

	//동기화 처리 관련 함수
	void DrivingCMDExuteFunc(bool bFlag);

public:
	void getCID_Info(CID_CTRL_INFO* Info, int nCurrentNodeID);

	int VHL_OPC_RESET();
	int SEND_SERIAL(char* cProtocol, int iSize);
	int SEND_SERIAL_NOT_CLEAR(char* cProtocol, int iSize);
	int SendLowSerial(char* cTemp);

	int VHL_OPC_RESET_RESPONSE();
	void VHL_OPC_RESET_RESPONSE_SIMUL(OPC_INIT_PKT RECV_PKT);
	int VHL_OPC_PASS_REQUEST (char* OHTNAME, unsigned char Priority, unsigned char Turn, unsigned char LightGuideID);
	int  GetCID_ELEVETORMODE_VALUE( );

	bool Check_Elevetor_LG1_START_AREA_Tag();
	bool Check_Elevetor_LG1_WAIT_AREA_Tag();
	bool Check_Elevetor_LG2_INSIDE_WAIT_AREA_Tag();
	bool MotionOnOff(bool bOnOff);

	int VHL_OPC_PASS_RESPONSE(unsigned char LightGuideID);
	void CID_IO_SETTING_LIFTER(bool LightGuideDirectionIsLeft);
	int CID_SELECT_ALL_OFF();
	int CID_OPERATION_MODE_LIFTER();
	int CID_SELECT_DIRECTION_LEFT();

	bool CID_Check_Mode_Lifter(bool LightGuideDirectionIsLeft);

	int CID_OPERATION_MODE_CID();
	int CID_SELECT_DIRECTION_RIGHT();
	int CID_Find_Stop_Reset(int callNum, DRIVING_HW_INFO_SET HwInfo, int* StopNode, int* ResetNode);
	int CID_Find_Stop_Reset_Change(int callNum, DRIVING_HW_INFO_SET HwInfo, int* StopNode, int* ResetNode, int Num);

	int ModeChg_CALL();
	int ModeChg_UP();
	int ModeChg_DOWN();

	void ADD_CID_LOG(AnsiString strLog);

//public:
	void SetOhtName(AnsiString strName);

	int m_nCidControlStep;
	int m_Continue_CID_MODE;//0: init, 1:CID 상태 유지
	int m_Continue_CID_MODE_StopNode;
	int m_Continue_CID_MODE_ResetNode;
	int m_Continue_CID_MODE_ResetNodeType;

	//사전작업 관련 함수(Shutter, EQ PIO) puting
	bool m_bShutterCloseFlag;     //false 확인 미필요, true CLose 확인 필요    //일정시간 지나면 초기화.
	bool m_bShutterOpenFlag;     //false 확인 미필요, true CLose 확인 필요    //출발시 초기화

	bool m_bShutterCloseMovingFlag;     //Inposition 확인 후 Shutter Close 처리. //일정시간 지나면 초기화.

	bool IsInnerFoup();   //재하 유무 확인 플래그
	int CheckShutterAlarm();

	int CheckShutterClose(double dSpeedOfNow);  //일정시간 도달할떄까지 셔터가 close 안되면 에러.. Shutter Close 명령은 Trans에서 발생시키며, Check만 주행에서 확인.
								//내림싸이클에서 Station목적지의 Target 거리가 400mm 이상일 경우만
								//상시 체크는 무시하고 이체크 끝날시 상시 체크 할것.

	int ExeCmdShutterClose(double dSpeedOfNow);    //출발시 무조건 명령 한번씩 줌.(출발시, 내림싸이클(Load)명령으로 출발할 때(무조건 명령줌) 예외)  Target 거리가 400mm 미만인 경우는 안줌)
	int ExeCmdShutterOpen(double dSpeedOfNow);     //Shutter Open 사전 명령.(내림싸이클(Load)  명령으로 도착 할때, Mark 단계에서 무조건 명령 줌. Target거리가 400mm 남은 경우에 )
								  //상시 체크는 무시하고 이체크 끝날시 상시 체크 할것.

	int ExeCmdEQPIOClose();     //출발시 무조건 명령 한번씩 줌.(PIO Disable 끝 )
	int ExeCmdEQPIOOpen(int nPIOType, int nPIOID, int nPIOChannel,int nPIOCS);      //EQPIO Open 사전 동작(RF인 경우만)(내림 싸이클, 올람씨이클에서 Port Type이 EQ 인경우만 사전동작함, Mark 단계에서)

	void Reload_QR_Data();
	void Reset_DrivingControl();

	double GetDefaultDecel();
	double GetDefaultAccel();

	void CID_Save_Start_Stop_Reset(CID_CTRL_INFO* Info);
	bool CID_Compare_List(DRIVING_HW_INFO_SET HwInfo);
	int CID_Save_Node[6];

	static double calcMaxSpeedDynamic(double velIn, const double* dynamicVel, const double* dynamicAcc, const int numDynamicArea, const double decel, const double constTime, double dist);
	DWORD CheckReadyToRunTime_ExcuteGo;
	DWORD CheckReadyToRunTime_ManualMark;
	DWORD CheckComplete_ExcuteGo;
	DWORD CheckComplete_ManualMark;
};


#endif	// DrivingControl

