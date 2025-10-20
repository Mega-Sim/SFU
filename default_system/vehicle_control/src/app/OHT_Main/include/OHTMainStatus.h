/**
@file    OHTMainStatus.h
@version OHT 7.0
@brief   공유 구조체 선언 Header File
*/

#ifndef OHTMainStatusH
#define OHTMainStatusH

#include "../../Common/Def_DataType.h"
#include "../Common/Protocol_Define_Modify.h"

// by zzang9un 2013.04.10 : 공유변수를 위한 Critical Section
extern TCriticalSection *CS_CmdList; ///< CmdList를 위한 Critical Section 변수
extern TCriticalSection* CS_TaskList; ///< CmdList를 위한 Critical Section 변수
extern TCriticalSection* CS_CycleList; ///< CmdList를 위한 Critical Section 변수
extern TCriticalSection* CS_ResponceList; ///< CmdList를 위한 Critical Section 변수
extern TCriticalSection *CS_PassOpenList; ///< CmdList를 위한 Critical Section 변수
extern TCriticalSection *CS_OHTMainStatus; ///< OHTMainStatus를 위한 Critical Section 변수

/**
@brief   Quick Command 관련 Enum 변수
@author  zzang9un
@date    2013.04.10
*/
typedef enum QUICK_CMD_
{
    QUICK_CMD_NONE      	= 0,    ///< Command None 상태
    QUICK_CMD_STOP      	= 1,    ///< E-Stop
    QUICK_CMD_PAUSE_OCS		= 2,
    QUICK_CMD_RESUME_OCS   	= 3,    
    QUICK_CMD_CANCEL    	= 4,    ///< 명령 Cancel
    QUICK_CMD_IDRESET   	= 5,    ///< Reset 명령
	QUICK_CMD_PAUSE_TP		= 6,
	QUICK_CMD_RESUME_TP    	= 7,
	QUICK_CMD_TURN_OFF		= 8,		///< 기동 Off
	QUICK_CMD_TURN_ON		= 9
} QUICK_CMD;


/**
@brief   Data Update Check Flag
@author  doori.shin
@date    2013.08.07
*/
typedef struct DATA_USE_FLAG_
{
	bool fStation;
	bool fTeaching;
	bool fMap;
	
	bool fSwapping;
	bool fReadySwapping;
	bool fMapsetLoad;  //1: Mapset 정상 로드 완료, 0: 맵 로드 안됨
	unsigned char fUBGLoad; //0:false(UBG fail), 1:true(UBG Pass) 0xFF:Init


	bool fSwappingParam;
	bool fReadySwappingParam;

}DATA_USE_FLAG;

/**
@brief   Job Chang Flag
@author  doori.shin
@date    2013.08.07
*/
typedef struct JOB_CHANGE_FLAG_
{
	bool bCycleingChangeFlag;
	int StartStationID;
	double dChangDistance;

}JOB_CHANGE_FLAG;


/**
@brief   Data 저장을 위한 Flag
@author  zzang9un
@date    2013.07.23
*/
typedef struct MAPMAKE_STATUS_
{
	// MapMake 관련 Flag
	bool bReady;        ///< Map Make 준비 Flag
	bool bStart;        ///< Map Make 시작 Flag
    bool bComplete;     ///< Map Make 종료 Flag

    // Map 관련 Flag
    bool bStartBackup_Map;   ///< Map Data Backup을 위한 flag
	bool bMapChanged;        ///< Map Data가 변경된 경우

    // Station Data 관련 Flag
    bool bStartBackup_Station;   ///< Station Data Backup을 위한 flag

    // Map Make 부가 정보
    UINT StartNodeID;           ///< Map Make 시작 Node ID
    UINT EndNodeID;             ///< Map Make 종료 Node ID
    int Speed_MapMake;          ///< Map Make 주행 속도    
} MAPMAKE_STATUS;

/**
@brief   Map주행검증을 위한 Flag
@author  puting
@date    2013.07.23
*/
typedef struct MAPMAKE_PROCESS_
{
	// MapMake 관련 Flag
	bool bReady;        ///< Map Make 준비 Flag
	bool bStart;        ///< Map Make 시작 Flag
    bool bComplete;     ///< Map Make 종료 Flag

	// Map 관련 Flag
	bool bMapChanged;        ///< Map Data가 변경된 경우

    // Map Make 부가 정보
    UINT StartNodeID;           ///< Map Make 시작 Node ID
    UINT EndNodeID;             ///< Map Make 종료 Node ID
} MAPMAKE_PROCESS;

/**
@brief   Main Thread와 Status Thread가 사용할 공유 구조체
@author  zzang9un
@date    2012.10.18
*/
typedef struct OHTMainStatus_
{
    // by zzang9un 2013.03.21 : Mode, Status, CmdID 통합 구조체 사용으로 개별 변수 사용 안함
    //BYTE Mode;
    //BYTE Status;              ///< 상위로 보고하는 Status: Execute 로 보내기 전 먼저 업데이트, 에러 발생 시 에러로 상태 변경
    
	int ErrCode_StatusChk;  ///< StatusCheckThread의 에러 코드
	int InnerErrCode;  ///< StatusCheckThread의 에러 코드

    // 주행을 위한 변수 추가: 임태웅 
    bool IsOnGoCmd;             ///< Go 명령 인가 시 True, Go 명령이 없을 때 Status가 Arived가 되면 false로 전환
	UINT FinalNodeOfPathSearch; ///< 주행 중 Go 명령 인가 시 기준으로 사용되는 시작 값. 주행 중이 아닐 때는 현재의 Node 값을 반영한다.
	UINT FinalEndNodeOfPathSearch; ///< 주행 중 Go 명령 인가 시 기준으로 사용되는 시작 값. 주행 중이 아닐 때는 현재의 Node 값을 반영한다.
	UINT FinalStationOfPathSearch; ///< 주행 중 Go 명령 인가 시 기준으로 사용되는 마지막 스테이션 위치 정보
	UINT FinalSeqNumOfPP; ///< 마지막 시퀀스 넘버


	//puting 2015.11.05
	 //----------------------------------------------------------------------------------------
	INNER_STATUS_OHT 	StandardStatusOHT; ///<OHT에서 OCS에게 보내 Status
	PROTOCOL_STATUS_OHT 	StandardOHTStatusReport;
	bool bOCSReportFlag;

	 //작업지시 받아서 실행해주는 곳에서 갱신해주기......
	 //클리어는 남은 작업지시가 없을 경우 작업지시ID 는 0으로 cycle은 없음으로 초기화.
    double dOHTTotalRunDis;
	BYTE StandardOHTProgress; 			///<내부 OHT 진척 상태
	BYTE StandardOHTPreProgress; 			///<내부 OHT 진척 상태
	bool bCheckRollbacking; ///< OHT 기동 On/Off에 따른 Pause 조건 유무 수동상태에서 기동 off 상태에서는 Pasuse 아님
	OHT_TURN_OFF_STATUS TurnOffInfo;     ///< 기동 Off시 저장된 정보.

	DWORD CheckCommfailtime;

	unsigned short CurCmdID;
	BYTE CurCycle;
	UINT TargetStationNID;
	bool bRealGoCycleing;
	bool bCycleing;

	JOB_CHANGE_FLAG JobChangeFlag;   ///<작업지시 변경이 왔을때 통과허가를 중지하기 위한 플래그 puting 20160307

	BYTE WaitCycle;
	UINT WaitTargetStationNID;

	bool bCheckCycleRunning;    //사이클 상태를 확인하는 부분 
	bool bIsOHTStoped;                     //현재 OHT 정지된상태인지? 주행중인지 확인하는 플래그


    STATUS_NODE_CHECK_DATA CurNodeCheckData;  ///현재 Node Count 및 위치 정보.
  //	MARK_TYPE FinalMarkType;         ///< 마지막 Mark Type 정의, 주행중 Go 명령인가시 기준으로 사용되는 시작 Type, 주행중이
//									///< 0: 없음, 1: 주행 Node, 2 :이적재 Node(QR, 이적재)
 //	UINT FinalStationDist;				///< 마지막이 Station일 경우 Dist 거리값


	//------------------------------------------------------------------------------------------


	// OCS, TP, 자가진단 서버 연결 확인용 변수

//	bool 			CID; ///< CID 감지 될 경우
//	bool ConnectedOCS;        ///< OCS와 연결 여부 flag
//	bool ConnectedOCSSub;        ///< OCS와 연결 여부 flag
//	bool ConnectedTP;         ///< TP와 연결 여부 flag
//	bool IsConnectMonitor;    ///< 자가진단 연결 여부 flag

	// by zzang9un 2013.04.10 : Main 내부 Quick Cmd 변수
    QUICK_CMD QuickCmd;     ///< 내부 Quick Cmd

    int QuickCmdEStopCommander;
    int QuickCmdEStopType;
    
	OHT_COMMON_STATUS StatusCommon; ///< 공통 Status

	MSG_STATUS_OHT2OCS StatusOCS; ///< OCS에게 보낼 OHT Status
	MSG_STATUS_OHT2TP  StatusTP;  ///< TP에게 보낼 OHT Status

	MSG_STATUS_OHT2VMS  StatusVMS;  ///< VMS에게 보낼 OHT Status
	MSG_STATUS_OHT2BLACKBOX  StatusBLACKBOX;  ///< BLACK BOX에게 보낼 OHT Status

	//2013.08.07 doori.shin OHT Data Update를 확인하는 플래그
	DATA_USE_FLAG DataUseFlag;

	bool fMainReady; ///< Main이 Ready상태인지 확인

	MONITOR_DATA_INFO m_cpMonitorData; ///< Monitoring -> OHT Data 동적 배열을 가리키는 char형 포인터
	//char *m_cpNODE; ///<  OHT -> Monitoring  동적 배열을 가리키는 char형 포인터

	bool fMemoryReset; ///< Reset 여부
	SIZE_T nfMemorySize; ///< Reset 여부
	DWORD dwLogThreadTime;

    // by zzang9un 2014.01.22 : Trace 시작을 알리는 Flag
    bool bTraceStart;   ///< StatusCheckThread에서 조건 성립 시 true로 변경하고 ExtraJobThread에서 Trace수행
	AnsiString strCIDMachineId;

	bool ConnectedIOTHub;
	bool bTourMode;
	BYTE CheckMaterialType;
    bool bIsHoistHomeCommand;

	int nStatusVMS;
	AnsiString strVMSBTId_1;
	AnsiString strVMSBTId_2;

} OHTMainStatus;


#endif  // OHTMainStatusH

