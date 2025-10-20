#ifndef Def_DrivingControlH
#define Def_DrivingControlH

#include <mmsystem.h>	// To Use "timeGetTime()"
#include <stdio.h>

#include "MAP.h"

#include "Def_DrivingCmdInfo.h"		// 명령 List 정리 Frame
#include "Def_DrivingHWInfo.h"		// HW 정보 수집하는 Frame
#include "Def_DrivingTimeInfo.h"	// 각종 시각 확인 Frame
#include "Def_DrivingPositionInfo.h"	// 위치 정보 수집 Frame
#include "Def_DrivingSteerInfo.h"	// 조향 정보 수집 Frame
#include "Def_DrivingEventState.h"	// 이벤트 및 상태 정보 Frame
#include "Def_DrivingCtrlInfo.h"		// 주행 HW 제어 Frame
#include "Def_DrivingIniParam.h"		// 주행 HW 제어 Frame
#include "List.h"

#include "Def_CID_Protocol.h"			// VHL 엘리베이터 구현 관련 통신패킷 정의

typedef enum DRIVING_MARK_FINAL_INFO_
{
	DRIVING_MARK_FINAL_INFO_NONE				= 0,		///< 상태 없음
	DRIVING_MARK_FINAL_INFO_SLOW_REACH			= 1,		///< 모자른 위치에서 접근
	DRIVING_MARK_FINAL_INFO_JUST_REACH			= 2,		///< 한번에 정확히 들어감
	DRIVING_MARK_FINAL_INFO_OVER_RUN_ESTIMATED	= 3,		///< 넘어감
	DRIVING_MARK_FINAL_INFO_OVER_RUN			= 4,		///< 넘어감
	DRIVING_MARK_FINAL_INFO_LAST_CMD_START		= 5,		///< 마지막 저속 보정: 정확히 들어감
	DRIVING_MARK_FINAL_INFO_LAST_CMD_RUNNING	= 6,		///< 마지막 저속 보정: 정확히 들어감
	DRIVING_MARK_FINAL_INFO_GOOD_ARIVED			= 7		///< 종은 위치 Mark 완료
}DRIVING_MARK_FINAL_INFO;

/**
@brief	 Path용 조향 Define
@author  ehoto
@date	 2012.11.12
*/
typedef enum DRIVING_STEERING_INFO_
{
	DRIVING_STEERING_NONE			= STEERING_NONE,		///< Steering 방향 없음
	DRIVING_STEERING_LEFT			= STEERING_LEFT,		///< Left Steering
	DRIVING_STEERING_RIGHT			= STEERING_RIGHT,	///< Right Steering
	DRIVING_STEERING_MOVING			= STEERING_MOVING	///< Steering 이동 중
}DRIVING_STEERING_INFO;

typedef enum DRIVING_STEERING_CMD_
{
	DRIVING_STEERING_CMD_NONE		= STEERING_NONE,		///< Steering 방향 없음
	DRIVING_STEERING_CMD_LEFT		= STEERING_LEFT,		///< Left Steering
	DRIVING_STEERING_CMD_RIGHT		= STEERING_RIGHT	///< Right Steering
}DRIVING_STEERING_CMD;

typedef enum TRANS_MARK_START_STATUS_
{
	FRONT_OFF_REAR_OFF		= 0,		// 앞으로 전진하며 마킹
	FRONT_ON_REAR_OFF		= 1,		// 뒤로 50mm 이동 후 마킹
	FRONT_OFF_REAR_ON		= 2,		// Error
	FRONT_ON_REAR_ON		= 3		// 뒤로 20mm 이동 후 마킹
}TRANS_MARK_START_STATUS;

//---------------------------------------------------------------------------
/**
@brief	 주행 현재 위치를 나타내는  Data
@author  ehoto
@date	 2012.12.12
*/
typedef struct POSITION_
{
	UINT	CurrNodeID;			///< 마지막 Update 된 주행 Node
	UINT	NextNodeID;			///< 이적재 TAG 정차 시 다음 주행 Node
	double	CurrNodeOffset;		///< 마지막 Update 된 주행 Node 로부터의 상대 거리
	UINT	CurrStationID;		///< 마지막 Update 된 이적재 Node

	bool			Marked;		///< 마킹 완료 여부: true /false
	MARK_TYPE	StationType;	///< 이적재 Type
} POSITION;


//---------------------------------------------------------------------------
/**
@brief	이상 상태 발생 여부 및 발생 최초 시간 기록 하는 구조체
@author  임태웅
@date	 2013.04.12
*/
typedef class EVENT_TIME_CHECK_
{
public:
	bool	FlagOfAbnormal;	///< 이상 상황 발생여부 
	DWORD	TimeOfAbnormal;	///< 이상 상황 발생 시간

	void init()
	{
		FlagOfAbnormal	= false;
		TimeOfAbnormal	= timeGetTime();
	}

	EVENT_TIME_CHECK_()
	{
		init();
	}	
}EVENT_TIME_CHECK;

/**
@brief	 주행 현재 위치를 나타내는  Data
@author  ehoto
@date	 2012.12.12
*/
typedef class DRIVING_ABNORMAL_CHECK_INFO_
{
public:
	EVENT_TIME_CHECK	OHTDetectFail;		///< 대차 감지 센서 이상 여부 확인(Fail Signal)
	EVENT_TIME_CHECK	OBSDetectFail;		///< 장애물 감지 센서 이상 여부 확인(Fail Signal)
	EVENT_TIME_CHECK	OBSDetect;			///< 장애물 감지 상태 확인: 장애물 감지가 일정 시간 유지 될 경우 에러 처리
	EVENT_TIME_CHECK	SteeringChange;		///< 조향 변경 상태 확인 : 조향 명령이 지연 실행 될 경우 에러 처리
	EVENT_TIME_CHECK	OnRunning;			///< 정상 주행 확인 : 특별한 이유 없이 정지 되었을 경우 에러 처리
	EVENT_TIME_CHECK	Steering;			///< 조향 상태 확인 : 현재 상황과 맞지 않는 조향 상태가 올라올 경우 에러 처리
	
	void	init()
	{
		OBSDetect.init();
		SteeringChange.init();
		OnRunning.init();
		Steering.init();
	}

	DRIVING_ABNORMAL_CHECK_INFO_()
	{
		init();
	}	
} DRIVING_ABNORMAL_CHECK_INFO;


//---------------------------------------------------------------------------
/**
@brief	 주행 Motion을 위한 Refference Data
@author  ehoto
@date	 2012.11.26
*/
typedef struct CMD_INFO_
{
	// 조향 
	DRIVING_STEERING_CMD SteeringCmd;			///< 기준 조향( 0:N, 1:L, 2:R )

	// 속도
	double	SpeedOfRef;				///< 산출한 기준 속도 (!=map Speed)
	double	SpeedOfCmd;				///< 명령 속도
	double	SpeedOfLimit;			///< 제한 속도

	// 거리
	UINT	CountOfCmdNode;			///< 이동 명령 준 노드까지의 노드 개수
	double	DistCmdOfRef;			///< 명령 기준 거리
	double	PositionOfSpeedDown;		///< 감속을 시작하는 위치
	double	DistOfOffset;			///<명령 인가 시 반영하는 추가 값 - 마킹 등의 추가거리 + 거리 보정 값 등등... 
} CMD_INFO;

/**
@brief	 Node의 정보 모음
@author  ehoto
@date	 2012.11.26
*/
typedef struct LINK_INFO_
{
	UINT	EndNodeID;			///< Link 끝의 EndNodeID
	int		CountOfNode;			///< Node 개수 (시작부터 ~ Node위치)
	
	PATH_DIRECTION_INFO	PathDirectionOfMap;	///< 기준으로 전방 감지 방향 설정 
	PATH_STEERING_INFO	SteeringOfMap;		///< 기준 조향	L, R, N, L2R, R2L

	double	SpeedOfMap;			///< Map 기준 속도
	double	DistSumOfMap;		///< End Node 절대 위치 (시작부터 ~ Node위치)
	double	DistOfThisPath;		///< 이 Path의 길이

	bool	FlagOfMapMake;		///< MapMake Path Flag ( true:MapMake 영역 / false: 일반 주행 영역 )
	EXECUTE_DRIVING_CMD_TYPE	CmdType;	///< Go, MapMake, Clean, Mark(?)
} LINK_INFO;

/**
@brief	 주행 상태 정보
@author  ehoto
@date	 2012.11.26
*/
typedef struct MARK_STATUS_
{
	bool	StateOnSmallAddPossible;		///< 00. 정지 Routine에들어가기 위해 감속 시작하였음
	bool	StateOnStopRoutineIn;		///< 0. StopRoutine 진입 여부
	bool	StateOnFirstMet;				///< 1. StopTag 진입 여부
	bool	StateOnSecondMet;			///< 2. StopTag 정지 위치 진입 여부
	bool	StateOnStoped;				///< 3. 동작 완료 여부
	bool	StateOnOverRunBack;			///< 4. OverRun된 경우

	UINT	FinalReadedTargetID;			///< 읽혀진 도착 ID 값
}MARK_STATUS;

/**
@brief	 주행 상태 정보
@author  ehoto
@date	 2012.11.26
*/
typedef struct STATUS_
{
	//1 1. ID, 속도, 조향, 거리 ----------------------
	// ID
	int		CountOfNode;					///< Node 개수 (시작부터 ~ Node위치)
	UINT	IDOfNode;					///< Node IDOfNode
	UINT	IDOfNodeBCR;					///< Node IDOfNode
	double	RealPositionOfNode;			///< Position Of Tag Station(For MapMaking)
	UINT	IDOfTagStationBCR;			///< Node ID Of Tag Station(For MapMaking)
	double	RealPositionOfTagStation;	///< Position Of Tag Station(For MapMaking)
	
	// 조향 
	DRIVING_STEERING_INFO	SteeringStatus;			///< L, R, M
	EVENT_TIME_CHECK	SteeringComplete;
	bool	SteeringFirstHit;					//1	추후 통합: SteeringComplete
	DWORD	TimeOfSteeringFirstHit;				//1	추후 통합: SteeringComplete
	bool	PreSteeringAvailable;

	DRIVING_STEERING_INFO	FrontSteeringStatus;		///< L, R, M
	EVENT_TIME_CHECK FrontSteeringComplete;
	bool	FrontSteeringFirstHit;				//1	추후 통합: FrontSteeringComplete
	DWORD 	TimeOfFrontSteeringFirstHit;			//1	추후 통합: FrontSteeringComplete
	
	// 속도 
	double	SpeedOfNow;						///< 현재 속도
	bool	StopWithoutFrontDetect;			///< 전방 감지 없이 멈췄을 경우 true

	// 거리 
	double	PositionOfNow;					///< 현재의 위치
	double	RealPositionOfNodePreChecked;	///< PreCheck Node의 실제 엔코더 위치값

	UINT	CountOfLastCmd;					///< 명령 받은 마지막 NodeCount
	UINT	TargetNode;						///< 목적지

#if 0
	//1 2. 근접제어 루틴 ----------------------
	BYTE	TypeOfFrontObject;   		///< 감지 타입 (
	double	DistToFrontObject;			///< 전방 물체까지 남은 거리
#endif

	//1 3. Mark Routine ----------------------	: 추후 구조체로 통합 
	MARK_STATUS	MarkStatus;
} STATUS;

/**
@brief	 주행 Refference 확인 필요 Event Flag
@author  ehoto
@date	 2012.11.26
*/
typedef struct EVENT_FLAG_
{
	// Point Event : 16
	UINT	Event_Node_PreChecked				: 1;	// Node 가 미리 감지되었을 경우 --> 선조향 가능
	UINT	Event_Steering_Completed			: 1;	// 조향 변경 완료 -> 다음 조향 Limit 변경 가능
	UINT	Event_Front_Steering_Completed		: 1;	// 앞 조향 변경 완료 -> 다음 조향 Limit 변경 가능
	UINT	Event_Steering_Changed_OnNBranch	: 1;	// N분기에서 조향 변경 완료 -> 다음 조향 Limit 변경 가능

	UINT	Event_NewNode_Arrived				: 1;	// 새로운 Node 만났을 때: NodeCount 중가 / BCR 값 갱신
	UINT	Event_SpeedDownPoint_Passed			: 1;	// 감속 지점 통과했을 경우

	UINT	Event_Cmd_Added						: 1;	// 명령 추가되었을 경우
	UINT	Event_Target_Updated				: 1;	// 타겟 업데이트 되었을 경우

	UINT	Event_Safety_Factor_Changed			: 1;	// 안전 영역 지날 경우
	
	UINT	Event_Move_Dist_Not_Match			: 1;	// 명령 준 거리와 AMC가 받은 거리가 다를 경우 
	UINT	Event_Stop_With_No_Reason			: 1;	// 무언정지 시

	UINT	Event_OutInNodeArea_Entered			: 1;	//Out/In Node 저속 구간 진입 시

	UINT	Event_Reserved						: 4;

	// Point Event Of Mark : 8

	UINT	MarkEvent_NextIsStop_and_SpeedDown	: 1;	// 타겟 정지 루틴 1. --> 이전의 GoMoreTagLength 와 유사한 명령 실행 : (State_NextIsTarget||State_NextIsBeforeMergeNode) && State_OnSpeedDown
	UINT	MarkEvent_FirstMet					: 1;	// 타겟 정지 루틴 2. --> Target 만난 경우 Stop 위치로 보냄 : 위 조건에서 정지 처음 조건 만난 경우
	UINT	MarkEvent_SecondMet					: 1;	// 타겟 정지 루틴 3. --> 최종 정지 및 최후 보정 : 위 조건에서 정지 마지막 조건 만난 경우
	UINT	MarkEvent_BackFromOverRun			: 1;	// 타겟 정지 루틴 4. --> Overrun되었을 경우 Back 명령

	UINT	Mark_Reserved						: 4;

	// State Event: 8
	UINT	State_OnSteeringChanging			: 1;	// 스티어링 변경 중 --> 감속 거리에 감소에 의한 속도 기준 연속적 방지 목적
	UINT	State_OnFrontSteeringCompleted		: 1;	// 스티어링 변경 중 --> 감속 거리에 감소에 의한 속도 기준 연속적 방지 목적
	UINT	State_OnPreChecked					: 1;	//Node가 PreCheck 되어 있는 상황( 실노드 만나기 전 )
//	UINT	State_OnOutInNodeArea				: 1;	// InOut 노드 영역에 존재 : 선조향 + 미정지 통과
	
	UINT	State_OnSpeedDown					: 1;	// 현재 감속 중일 경우
	
	UINT	MarkState_NextIsTarget				: 1;	// 다음 노드가 타겟일 경우 --> 정지루틴
	UINT	MarkState_NextIsPreMergeNode		: 1;	// 다음 노드가 합류 직전 노드일 경우 --> 충돌 방지 및 흐름 제어
	UINT	MarkState_AvailabeToPass			: 1;	// 합류 전 Node를  Pass 가능하도록 윤허 받은 경우

	UINT	State_Reserved						: 1;
}EVENT_FLAG;

/**
@brief	 주행 Refference 확인 필요 Event 집합 구조
@author  ehoto
@date	 2012.11.26
*/
typedef struct EVENT_CHECK_
{
	WORD	EventExist;
	BYTE	MarkEventExist;
	BYTE	StateExist;

}EVENT_CHECK;

/**
@brief	 주행 Refference 확인 필요 Event Flag 와 집합 구조 모음
@author  ehoto
@date	 2012.11.26
*/
typedef union EVENT_
{
	EVENT_FLAG	Flag;
	EVENT_CHECK	Check;
}EVENT;

/**
@brief	 안전 주행 을 위한 변수들 
@author  ehoto
@date	 2012.11.26
*/
typedef struct SAFETY_
{
	// 이전 Target 에 보정 값 
	double	DistOffsetOfPreDriving;		///< 이전 정지 타겟에 사용된 Offset 거리 : 도착시에는 더해져 사용되고 출발시엔 감해서 사용됨

	// 오차 처리 
	double	DistOfTotalError;			///< 총 거리 오차 : 엔코더 값 - 이상(ideal) 거리 
	double	DistOfThisError;				///< 최근 링크에서 발생한 거리 오차

	// 속도 리밋
	double	SpeedLimitBySteering;		///< 조향을 구현하기 위한 최소 속도
	double	DistLimitBySteering;			///< 같은 조향의 최대 거리 
	UINT	CountLimitBySteering;		///< 조향에 의한 속도 리밋을 구한 최종 노드의 타겟이 같을 경우 리밋 속도를 기존 유지... 번복하면서 속도를 더 낮출 필요는 없다

	double	SpeedLimitByObstacle;		///< 조향을 구현하기 위한 최소 속도
	double	DistLimitByObstacle;			///< 장애물 감지후 진행 할 수 있는 최대 거리
//	UINT CountLimitByObstacle;		  			///< 조향에 의한 속도 리밋을 구한 최종 노드의 타겟이 같을 경우 리밋 속도를 기존 유지... 번복하면서 속도를 더 낮출 필요는 없다  : 구할 수 없는 위치	
}SAFETY;

/**
@brief	 이적재를 위한 변수들 

	Node 추가 시 타겟의 값을 반영
@author  ehoto
@date	 2012.11.26
*/
typedef struct TAG_
{
	bool	bGotMarkInfo;					///< false: 마크 정보를 받지 못하였음, true: 마크 정보를 받았음

	UINT	IDofStation;						///< Station ID -- 없을 경우 0
	
	UINT	IDofPreNode;						///< 이전의 주행 노드
	UINT	IDofNextNode;					///< 이후의 주행 노드	- 거리 조정의 키로 사용될 가능성 존재
	UINT	CountOfFinalNode;				///< 목적지로 보고 달려가는 마지막 Node ( Staion Target 일 경우 Next Node / Node Target 일 경우 PreNode )
	MARK_TYPE	StationType;				///< 이적재 Type

	double	DistSumToTarget;					///< 출발부터 Target까지의 거리

	double	DistOffsetFromPreNode;			///< 이전의 주행 노드로부터의 Offset
	double	DistBetweenNode;					///< 이적재 노드가 주행 노드 사이에 존재 시, 그 주행노드 간의 사이 거리
	
	double	LengthBeforeFirst;				///< 정위치 도착 전 거리 보정을 위해 더 보내는 이동 거리 (-값으로 Final Node에서 감소시키는 거리)
	double	LengthFromFirst;					///< 정위치 도착 전 사전 감지 거리 : Front 2 Rear 길이 135mm
	double	LengthOfSecond;					///< 정위치 Tag의 크기 : Rear 길이 5mm
	
	double	AddLengthForFirst;				///< 정위치 Tag 미도달 시 조금씩 전진하는 거리 : Tag 길이의 2/3 정도 (이적재 Tag: 90mm)
	double	AddLengthForSecond;				///< 정위치 미도달 시 조금씩 전진하는 거리 : 정위치 센서의 1/3 길이  (이적재 Tag: 5*1/3mm)
	double	DistLimitOfAddLengthForFirst;	///< 정위치 Tag 미도달 시 조금씩 전진하는 회수의 최대값
	double	DistLimitOfAddLengthForSecond;	///< 정위치 미도달 시 조금씩 전진하는 회수의 최대값

	double	SpeedBeforeFirst;				///< 사전 감지 거리 진행 속도 
	double	SpeedOfFirst;					///< 사전 감지 거리 진행 속도 
	double	SpeedOfSecond;					///< 정위치 도착을 위한 속도 

	double	AccelBeforeFirst;				///< 사전 감지 거리 진행 가속도 : 0 이면 사용안함 
	double	AccelOfFirst;					///< 사전 감지 거리 진행 가속도 : 0 이면 사용안함  
	double	AccelOfSecond;				///< 정위치 도착을 위한 가속도 : 0 이면 사용안함	

	// 정위치를 위해 함고 위치를 기록하는 장소  : todo:제거
	double	RealPositionOfFirstEdgeCheck;	///< 정위치 사전 감지가 처음 감지된 위치 
	double	RealPositionOfSecondEdgeCheck;	///< 정위치 Tag가 처음 감지된 위치
}TAG;

/**
@brief	 동작을 위한 파라미터
@author  임태웅
@date	 2013.03.29
*/
typedef struct PARA_FOR_MOVE_
{
	double	Accel;	///< 가속도 (mm/msec2)
	double	Decel;	///< 감속도 (mm/msec2)
	double	Speed;	///< 거리 (mm/msec)
	double	Dist;	///< 거리 (mm)

	USER_MOVE_DS_ADDITIONAL_INFO	AdditionalInfo;	///< Move DS 의 Extension 정보를 사용하기 위한 변수
}PARA_FOR_MOVE;


/**
@brief	 INI 파라미터
@author  임태웅
@date	 2013.05.30
*/
typedef struct INI_PARA_FOR_DRIVING_
{

}INI_PARA_FOR_DRIVING;


//1	NEW WAY ADDITION

/**
@brief	분류된 명령 정보 집합
@author  임태웅
@date	 2013.07.23
*/
typedef struct CLASSIFIED_DRIVING_CMD_SET_
{
	//2 제어를 위한 List Of Information
	List<DRIVING_CMD_INFO_OF_NODE*> 		NodeList;		///< Node 만났을 떄 정상 범위 확인 시 사용
	List<DRIVING_CMD_INFO_OF_STEER*>		SteerList;		///< 조향 제어 및 조향에 의한 거리 속도 제한
	List<DRIVING_CMD_INFO_OF_DIV*>			DivList;		///< 분기 정보(CID관련 제어)
	List<DRIVING_CMD_INFO_OF_DIRECTION*>	DirectionList;	///< 근접제어의 기준 정보: Node가 업데이트 되기 전 Path의 방향 정보와 위치 정보(Count/거리)를 전달
	List<DRIVING_CMD_INFO_OF_SPEED*>		SpeedlList;		///< 속도 변화에 따른 기준 정보
	List<DRIVING_CMD_INFO_OF_PATH*> 		PathList;		///< 명령의 저장
	
	//2 최종 목적지 정보
	DRIVING_CMD_INFO_OF_TARGET			TargetInfo;		///< 최종 목적지에 대한 정보
}CLASSIFIED_DRIVING_CMD_SET;


/**
@brief	경로 변경시 정보 집합
@author  박태욱
@date	 2016.06.03
*/
typedef struct CLASSIFIED_DRIVING_CHANGE_
{
	//2 최종 목적지 정보
	int 								nCheckNodeCount;
	double								dEncoderFinal;

	DRIVING_CMD_INFO_OF_TARGET			TargetInfo;		///< 최종 목적지에 대한 정보
}CLASSIFIED_DRIVING_CHANGE;


/**
@brief	위치 정보 집합
@author  임태웅
@date	 2013.07.23
*/
typedef struct DRIVING_POSITION_INFO_SET_
{
	CURR_AND_TARGET_POSTION_STATUS	CurrentAndTarget;	///< 실재 OHT 위치 정보, 목적지 정보
	DRIVING_COMPENSATION_INFO		Compensation;		///< 거리 보상 정보
	DRIVING_MARK_INFO				Mark;				///< 상위 보고용 Mark 정보:	정지 시 CurrentAndTarget.CurrPositionStatus 정보를 받음 
	DRIVING_CTRL_REFERENCE			CmdRefInfo;	
	bool							bFirstNIDIsCorrect;		///< 처음 만난 Node 의 ID 정합 확인 결과: Node의 0~10mm Offset의 Station  경우 / 추후(Station 강제 설졍 경우)
	bool							bPreCheckIsOverFirstNode;	///< 출발 시 PreCheck 센서가  첫 Node를 넘어가 있는 경우 
}DRIVING_POSITION_INFO_SET;

/**
@brief	주행 정보 집합
@author  임태웅
@date	 2013.07.23
*/
typedef struct DRIVING_INFO_SET_
{
	DRIVING_HW_INFO_SET				HwInfo;	

	DRIVING_TIME_INFO_SET			Time;
	DRIVING_POSITION_INFO_SET		Position;
	STEER_INFO						Steer;

	DRIVING_EVENT_AND_STATE			EventAndState;
	DRIVING_CTRL_INFO_SET			Control;
}DRIVING_INFO_SET;


/**
@brief	주행 Step 정보 집합
@author  임태웅
@date	 2013.07.23
*/
typedef enum DRIVING_CTRL_STEP_
{
	DRIVING_CTRL_STEP_NONE			= 0,	///< 상태 없음

	DRIVING_CTRL_STEP_GO_PASSBOX	,
	DRIVING_CTRL_STEP_ELEVETOR_RIDE 	,	///< Elevetor 명령 : 올라감
	DRIVING_CTRL_STEP_ELEVETOR_OFF		,
	DRIVING_CTRL_STEP_GO_INIT		,	///< 주행 명령 : 초기화
	DRIVING_CTRL_STEP_GO_PREPARE	,	///< 주행 명령 : 주행 준비
	DRIVING_CTRL_STEP_GO_FIRST_NODE	,	///< 주행 명령 : Node 근처 Station 경우 Node Count 꼬임을 방지하기 위한 처리
	DRIVING_CTRL_STEP_GO_RUN		,	///< 주행 명령 : 주행
	DRIVING_CTRL_STEP_GO_MARK		,	///< 주행 명령 : 정지 루틴
	DRIVING_CTRL_STEP_GO_CHECK_STOP	,	///< 주행 명령 : 정지 확인 루틴
	DRIVING_CTRL_STEP_GO_COMPLETE	,	///< 주행 명령 : 완료 확인 

	DRIVING_CTRL_STEP_MANUAL_INIT		,	///< Manual 명령 : 초기화
	DRIVING_CTRL_STEP_MANUAL_AMP_ON		,	///< Manual 명령 : AMP ON
	DRIVING_CTRL_STEP_MANUAL_MOVE_CMD	,	///< Manual 명령 : 동작 명령 
	DRIVING_CTRL_STEP_MANUAL_CHECK_STOP	,	///< Manual 명령 : 정지 확인

	DRIVING_CTRL_STEP_MANUAL_MARK_INIT			,	///< Manual Mark 명령 : 초기화
	DRIVING_CTRL_STEP_MANUAL_MARK_PREPARE		,	///< Manual Mark 명령 : 주행 준비
	DRIVING_CTRL_STEP_MANUAL_MARK_RUN			,	///< Manual Mark 명령 : 동작 명령 
	DRIVING_CTRL_STEP_MANUAL_MARK_CHECK_STOP	,	///< Manual Mark 명령 :  정지 확인 루틴
	DRIVING_CTRL_STEP_MANUAL_MARK_COMPLETE		,	///< Manual Mark 명령 : 완료 확인 
	

}DRIVING_CTRL_STEP;

typedef enum PASSBOX_CTRL_STEP_
{
	PASSBOX_CTRL_STEP_NONE						= 0,
	PASSBOX_CTRL_STEP_INIT						,
	PASSBOX_CTRL_STEP_GO_SIGNAL_CHECK			,	
	PASSBOX_CTRL_STEP_SIGNAL_OFF				,	
	PASSBOX_CTRL_STEP_ES_SIGNAL_CHECK			,		
	PASSBOX_CTRL_STEP_PB_AVBL_SIGNAL_CHECK		,		
	PASSBOX_CTRL_STEP_ABNORMAL_SIGNAL_CHECK		,			
	PASSBOX_CTRL_STEP_OHT_STATUS_ON				,
	PASSBOX_CTRL_STEP_CLOSE_ON_CHECK			,	
	PASSBOX_CTRL_STEP_DOOR_SELECT				,		
	PASSBOX_CTRL_STEP_OPEN_REQ					,		
	PASSBOX_CTRL_STEP_BUSY_ON_SIGNAL_CHECK		,			
	PASSBOX_CTRL_STEP_READY_ON					,				
	PASSBOX_CTRL_STEP_CLOSE_OFF_CHECK			,					
	PASSBOX_CTRL_STEP_BUSY_OFF_CHECK			,						
	PASSBOX_CTRL_STEP_PASSBOX_DOOR_OPEN_CHECK	,								
	PASSBOX_CTRL_STEP_COMPLETE	
}PASSBOX_CTRL_STEP;

//typedef	enum	MANUAL_DRIVING_STEP_TYPE_
//{
//	DRIVING_CTRL_STEP_MANUAL_INIT			= 0,
//	DRIVING_CTRL_STEP_MANUAL_AMP_ON			= 1,
//	DRIVING_CTRL_STEP_MANUAL_MOVE_CMD		= 2,
//	DRIVING_CTRL_STEP_MANUAL_CHECK_STOP		= 3
//}MANUAL_DRIVING_STEP_TYPE;


// 엘리베이터에 들어갈 때의 제어 스탭
	/* [CID를 초기 설정]
	   CID-O의 Operating Mode (O6)를 “1(CID)” 설정 후
	   Select On, 위치를 모르지만 O6을 ‘1’ 설정 후 우선 Select On.
	   CID의 초기모드는 1(CID)이고, VHL이 이동하여 LIFTER영역에 진입하면
	   CID-O의 I5가 '1'로 설정됨을 보고, 리프터 영역 진입을 알 수 있다 */

// Select Off한 후  Operating Mode (O6)를 “0(MTL/Lifter)” 설정 후 Select On.
// Select ON-> OFF-> ON하실 때,OFF시간은 최소 10ms 이상은 유지.
typedef enum VHL_ELEVETOR_RIDE_CTRL_STEP_
{
	VHL_ELEVETOR_RIDE_CTRL_STEP_NONE						= 0,	// 진행하고 있지 않다
	VHL_ELEVETOR_RIDE_CTRL_STEP_CID_INIT					,		// IO를 통해 리프터 모드로 전환시킨다 
	
	VHL_ELEVETOR_RIDE_CTRL_STEP_SERIAL_INIT					,		// 시리얼 통신의 초기화 Start
	VHL_ELEVETOR_RIDE_CTRL_STEP_SERIAL_WAIT					, 		// 시리얼 통신의 초기화 진행중

	VHL_ELEVETOR_RIDE_CTRL_STEP_INPUT_REQUEST				,		// 투입허가 요청함
	VHL_ELEVETOR_RIDE_CTRL_STEP_INPUT_CONFIRM_WAIT		    ,		// 투입허가 승인대기
	VHL_ELEVETOR_RIDE_CTRL_STEP_COMPLETE				    ,		// Process OK.
	

} VHL_ELEVETOR_RIDE_CTRL_STEP;


// 리프터 사전 진입허가 신청상태
typedef enum VHL_ELEVETOR_PREPASS_STEP_
{
	VHL_ELEVETOR_PREPASS_STEP_NONE		 = 0,	// 진입허가 진행하지 않음
	VHL_ELEVETOR_PREPASS_STEP_TOUCH			,	// 진행중이었음 (완료까지는 못감)
	VHL_ELEVETOR_PREPASS_STEP_COMPLETE		,	// 진입허가 완료. RIDE동작 불필요. 바로 진입할 것
} VHL_ELEVETOR_PREPASS_STEP;


// 엘리베이터에서 내릴 때의 제어 스탭
typedef enum VHL_ELEVETOR_OFF_CTRL_STEP_
{
	VHL_ELEVETOR_OFF_CTRL_STEP_NONE							= 0,	// 진행하고 있지 않다
	VHL_ELEVETOR_OFF_CTRL_STEP_INIT							,
	VHL_ELEVETOR_OFF_CTRL_STEP_OUTPUT_REQUEST_INIT			,		// 나가려는 방향으로 가고싶다고 요청한다 (우/좌 선택 등 정보전송)
	VHL_ELEVETOR_OFF_CTRL_STEP_OUTPUT_RESPONSE				,		// 요청에 대한 응답을 체크한다
	VHL_ELEVETOR_OFF_CTRL_STEP_OUTPUT_COMPLETE				,		// 나가도 된다고 응답을 받음. 스테이트 머신 종료
} VHL_ELEVETOR_OFF_CTRL_STEP;

typedef enum MTL_TakeOut_CTRL_STEP_
{
	MTL_TakeOut_CTRL_STEP_NONE						= 0,
	MTL_TakeOut_CTRL_STEP_INIT						,
	MTL_TakeOut_CTRL_STEP_OCSReport_CHECK			,
	MTL_TakeOut_CTRL_STEP_TakeOutREQ_ON				,
	MTL_TakeOut_CTRL_STEP_TakeOutOk_SIGNAL_CHECK	,
	MTL_TakeOut_CTRL_STEP_TakeOutREQ_OFF     		,
	MTL_TakeOut_CTRL_STEP_FrontMoveOK_SIGNAL_CHECK	,
	MTL_TakeOut_CTRL_STEP_OHT_MTL_Insert_Start		,
	MTL_TakeOut_CTRL_STEP_OHT_MTL_Insert_End		,
	MTL_TakeOut_CTRL_STEP_OHT_Inposition_CHECK		,
	MTL_TakeOut_CTRL_STEP_TakeOutCompt_CHECK		,
	MTL_TakeOut_CTRL_STEP_COMPLETE                  ,
	MTL_TakeOut_CTRL_NG_COMPLETE                    ,

}MTL_TakeOut_CTRL_STEP;

typedef enum MTL_TakeIn_CTRL_STEP_
{
	MTL_TakeIn_CTRL_STEP_NONE						= 0,
	MTL_TakeIn_CTRL_STEP_INIT						,
	MTL_TakeIn_CTRL_STEP_OHTTakeIn_OK_CHECK			,
	MTL_TakeIn_CTRL_STEP_TakeIn_SIGNAL_CHECK		,
	MTL_TakeIn_CTRL_STEP_OHT_MTL_TakeIn_Start		,
	MTL_TakeIn_CTRL_STEP_OHT_MTL_TakeIn_End	   		,
	MTL_TakeIn_CTRL_STEP_OHT_Inposition_CHECK  		,
	MTL_TakeIn_CTRL_STEP_TakeInCompt_CHECK	   		,
	MTL_TakeIn_CTRL_STEP_COMPLETE					,
	MTL_TakeIn_CTRL_STEP_OCSReport_Error			,
}MTL_TakeIN_CTRL_STEP;



// shkim delete_reserve
// 2016.10.15  CID제어를 위한 State정의 (auto)
// shkim
#if 0
typedef enum CID_STEP_
{
	CID_STEP_NONE								= 0,   	// 아무것도 하지 않고 있으며 상태체크만 하는 중이다
	// CID_STEP_LR_SETTING							 ,		// Left, Right를 설정한다. 위 상태와 분리한 이유는 목적지 변경 (5.1.2.15)시에는 Link는 유지하고
														// 점유반납 및 ACK확인후 좌우 Select를 해야 하는 바, 위 상태와 함께 있을수가 없기 때문이다.
	// CID_STEP_LINK_WAITING						 ,		// 링크 ON한 후, Link가 이루어지는지 지켜보는 중이다
														//      (예외처리 : 일정시간 또는 거리동안 연결되지 않으면 정차한다)
														//      (예외처리 : 대차 감지시에는 일정시간이 가는것의 카운트를 유예하여 기다리도록 한다)

	CID_STEP_CONDITIONAL_CHK_RESUME					 ,  // 3
	CID_STEP_CONDITIONAL_CHK_PAUSE					 ,  // 4
	CID_STEP_CONDITIONAL_CHK_RESERVE				 ,  // 5
	// CID_STEP_MOVING_PLAN_RESERV_SLOPE			 ,	// 6

	CID_STEP_MOVING_PLAN_CHK					 ,   // 7   // 주행 목적지가 CID End노드 또는 그 이후까지인지 판단하여 목적지가 CID Start노드에
														// 한정된 것이라면 다음의 점유 관련동작은 불필요하므로 새로 명령이 오는지를
														// 이 단계에서 계속 확인하며 대기한다

	CID_STEP_CONDITIONAL_CHK_RUNNING				,	 //8   // (1) 주행중인 상태로서, 현재 위치가 CID End Node인지 여부를 반복적으로 확인한다
														// (2) 주행 중 상태에서 목적지 변경이 일어나면 점유반납 및 반납 확인하고, LR설정으로 가야 한다

	CID_STEP_N_BRANCH_SELECT_OFF				 ,   // 9
	CID_STEP_N_BRANCH_SELECT_ON				 	 ,   // 10

	// CID_STEP_COMPLETE							 ,	 // 11	// CID동작이 모두 종료되어 I/O를 클리어하고 상태변수를 리셋한다

}CID_STEP;
#endif
// ---------------------------------------------------------
/**
@brief	CID 동작 Step 정보 집합
@author  류건진
@date	 2016.10.30
*/
typedef enum CID_REVISED_STEP_
{
	// CID_STEP_INITIALIZATION							= 0,
	CID_STEP_CONNECTING							= 1,	// 1
	CID_STEP_CHECKING_PATH						,		// 2
	CID_STEP_OCCUPYING_PATH						,		// 3
	CID_STEP_COMPLETION							,		// 4
	// CID_STEP_DISCONNECTING						,	// 5
	CID_STEP_ABNORMAL							,	// 5
	CID_STEP_WAITING_FOR_PATH					,		// 7
	CID_STEP_COMPLETION_DUE_TO_JOB_CHANGE		,
	
} CID_REVISED_STEP;


/*
	CID Status머신 관련 정의
*/
typedef enum executeCID_CMD_
{
	executeCID_NONE					= 0,
	executeCID_N_Branch_Select_Off	   ,
	executeCID_N_Branch_Select_ON	   ,

} executeCID_CMD;


/*
	좌우 일반 CID, 좌우 공용 CID의 구분
*/
typedef enum CID_DIRECTION_
{
	CID_DIRECTION_LEFT			= 0, // 0
	CID_DIRECTION_RIGHT			 	,// 1
	CID_DIRECTION_COMMON_LEFT		,// 2
	CID_DIRECTION_COMMON_RIGHT		,// 3
	CID_DIRECTION_NOT_LINK			,// 4
} CID_DIRECTION;

#define CID_LAYOUT_TYPE_NORMAL	1
#define CID_LAYOUT_TYPE_SLOPE	2


// 2016.10.15  CID제어를 위한 State정의 (auto)
// shkim
// 시나리오 : OHT Manual Mode 일때 (76p)
/*
	-  현재 Node를 보고, CID구간이면 L 또는 R을 켠다 (O1 or O2) 
	-  Operation Mode를 CID로 변경 (O6는 1로 설정)	
	-  Abnormal (O5는 1로 설정) 
	- Link가 확인되면 점유를 ON
*/

typedef enum MANUAL_CID_
{
	MANUAL_CID_MODE			 = 0,	// 1) 초기상태에서 CID모드로 전환.  Operation Mode를 CID로 변경 (O6는 1로 설정).
	
	MANUAL_CID_SETTING			,	// 2) 기본설정 실시. Abnormal 세트 (O5는 1로 설정)
	
	MANUAL_CID_DIRECTION_CHK	,	// 조향정보를 감시한다 (MAP또는 조향을 이용)
									// - Left or Right설정 (Left = O1, Right = O2) : AllOff후 해당방향을 켠다
									
	MANUAL_CID_RESERVED_ON		,	// 점유를 켠다 (O3 = 1). 
									// 조향확인 단계 (MANUAL_CID_DIRECTION_CHK) 로 이동하여 반복 진행한다
	MANUAL_CID_RESERVED_OFF		,

	MANUAL_CID_ABNORMAL_MODE_ON	,

	MANUAL_CID_ABNORMAL_MODE_OFF,

	MANUAL_CID_LEFT_SELECT		,

	MANUAL_CID_RIGHT_SELECT		,
	
	MANUAL_CID_COMMON_TAG		,
}MANUAL_CID;

#define DELAY_SERIAL_READ	    			70




// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack

// CID확인 동작을 위한 정보를 넣어둘 구조체 정의
typedef struct
{
	int nDetectStatus;			//UBG 감지 여부
	int tmpCurrentNodeID;
	int tmpPreNodeID;	//(int)m_PositionInfo.CurrPreCheckNode : 프리센서 노드 ID
	int tmpNextNodeID;	//다음 이동할 노드가 존재하면 NOde ID, 없으면 0
	int tmpCurrentCMDID;
	int tmpCurrentStationID;
	UINT uLimitNode;
	UINT CMD_StationID;
	unsigned char Safety_Level;
	bool bIgnolOverRun;
//	int tmpNextNodeIDafterResetNode;

	//int tmpStopID;

	// DWORD   dwCIDMonitoringTrigerTime;
	//int     bCountOfCID;
	//int     bCountOfCIDLink;        //링크 연결 관련
	//int     bCountOfCIDDualOccup;  	//동신진입 관련
	//int     bCountOfCIDComptACK;  	//완료처리 응답 관련

	unsigned int CIDStopNode1_arr;
	unsigned int CIDStopNode2_arr;
	unsigned int CIDStartNode1_arr;
	unsigned int CIDStartNode2_arr;
	unsigned int CIDResetNode1_arr;
	unsigned int CIDResetNode2_arr;
	unsigned int CIDResetNode_arr;
	unsigned int CIDResetNode_Pre_arr;
	int CIDStopNode1;		//현재 노드가 Stop Node인지. Stop node이면 node ID 가 들어감.
	int CIDStopNode2;		//현재 노드가 Stop Node인지. Stop node이면 node ID 가 들어감.
	int CIDStartNode1;
	int CIDStartNode2;
	int CIDResetNode1;      //StopNode_1에 대한 Reset Node_1
	int CIDResetNode2;      //StopNode_2에 대한 Reset Node_2
	int CIDResetNode;       //현재 노드가 Reset Node인지. Reset node이면 node ID 가 들어감.
	int CIDResetNode_Pre;       //Pre 노드가 Reset Node인지. Reset node이면 node ID 가 들어감.
	int CIDResetNodeType;	//현재 노드가 Reset Node 인 경우 Reset Node Type.
	int CIDResetNodeType_Pre; //Pre 노드가 Reset Node 인 경우 Reset Node Type.

	int LayoutType_Stop1;	//Stop1에서 찾은 LayoutType
	int LayoutType_Stop2;	//Stop2에서 찾은 LayoutType
	int LayoutType_Reset;	//Reset에서 찾은 LayoutType

	//L
	int tmpStartNode1_1;
	int tmpStartNode1_2;
	int tmpStartNode1_3;
	int tmpStartNode1_4;
	int tmpStartNode1_5;
	int	tmpStopNode1;    // CID 무시 신호 처리를 위해 추가 (2016.12.2)
	int tmpResetNode1;
	int tmpResetNodeType1;
	int tmpCIDStopNode1;
	int CommStartNode1_Offset;

	//R
	int tmpStartNode2_1;
	int tmpStartNode2_2;
	int tmpStartNode2_3;
	int tmpStartNode2_4;
	int tmpStartNode2_5;
	int	tmpStopNode2;    // CID 무시 신호 처리를 위해 추가 (2016.12.2)
	int tmpResetNode2;
	int tmpResetNodeType2;
	int tmpCIDStopNode2;
	int CommStartNode2_Offset;

	int tmpCurrentResetNode;//미사용...
	int tmpCurrentResetNodeType;// 값 대입 부분 주석 해제 
	
	// 4분기 처리용 정보
	bool bLoadUnload;	// 이적재 유무. true : 이적재 한다. false : 이적재 하지 않음 
	int  tmpLastCMDID;	// 최종 타겟

	// 0:분기 없음, 1:왼쪽분기, 2:오른쪽분기
	PATH_DIVERGENCE_INFO NodeDivInfo;	// 현재 노드의 분기정보
	PATH_DIVERGENCE_INFO NextDivInfo;	// 다음 노드의 분기정보

} CID_CTRL_INFO;
#pragma pack(pop) // Pack 해제



typedef struct
{
	// 4분기 처리용 정보
	bool bLoadUnload;	// 이적재 유무. true : 이적재 한다. false : 이적재 하지 않음 
	int  tmpLastCMDID;	// 최종 타겟

	// 0:분기 없음, 1:왼쪽분기, 2:오른쪽분기
	PATH_DIVERGENCE_INFO NodeDivInfo;	// 현재 노드의 분기정보
	PATH_DIVERGENCE_INFO NextDivInfo;	// 다음 노드의 분기정보
} CID_4WAY_INFO;

// -------------------------------------------
#endif
