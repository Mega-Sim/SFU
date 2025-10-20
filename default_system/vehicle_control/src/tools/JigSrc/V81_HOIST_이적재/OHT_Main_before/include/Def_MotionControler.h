#ifndef Def_MotionControlerH
#define Def_MotionControlerH

#include <stdio.h>
#include "Def_OHT.h"

// CMD_MOVE의 CmdType define
#define	MMC_CMD_AXIS_NONE				0	///< 명령 없음
#define	MMC_CMD_AXIS_RESET				1	///< 축 초기화
#define	MMC_CMD_AXIS_AMP_ENABLE			2	///< Servo On --> 입력 인자 없음
#define	MMC_CMD_AXIS_AMP_DISABLE		3	///< Servo Off --> 입력 인자 없음

#define	MMC_CMD_AXIS_MOVE_P				4
#define	MMC_CMD_AXIS_MOVE_N				5
#define	MMC_CMD_AXIS_MOVE_S				6
#define	MMC_CMD_AXIS_MOVE_DS			7

#define	MMC_CMD_AXIS_MOVE_TIME_P		8
#define	MMC_CMD_AXIS_MOVE_TIME_N		9
#define	MMC_CMD_AXIS_MOVE_TIME_S		10
#define	MMC_CMD_AXIS_MOVE_TIME_DS		11

#define	MMC_CMD_AXIS_SET_HOME			12	///< 현재 위치를 0으로 만듬 --> 입력 인자 없음
#define	MMC_CMD_AXIS_SET_OFFSET			13	///< Offset 값 적용 (절대치 엔코더: 입력되는 위치를 0으로 만듬) --> Distance 에 값 입력

#define	MMC_CMD_AXIS_SET_POS_LIMIT		14	///< Positive SW Limit --> Distance 에 값 입력
#define	MMC_CMD_AXIS_SET_NEG_LIMIT		15	///< Negative SW Limit --> Distance 에 값 입력
#define	MMC_CMD_AXIS_RESET_LIMIT		16	///< Reset Limit --> Limit 사용하지 않음(또는 제일 큰 값으로 설정) --> 입력 인자 없음

// CMD_ADDITIONAL_INFO의 CmdType define
#define	MMC_CMD_PATH_DIRECTION_NO_USE 	0	// 미사용: 예전 없음
#define	MMC_CMD_PATH_DIRECTION_NORMAL	1	// 직진 구간: 예전 N
#define	MMC_CMD_PATH_DIRECTION_LEFT		2	// 왼쪽: 예전 L
#define	MMC_CMD_PATH_DIRECTION_RIGHT	3	// 오른쪽: 예전 R
#define	MMC_CMD_PATH_DIRECTION_SHORT	4	// N분기 사용: 예전 S
#define	MMC_CMD_PATH_DIRECTION_BOTH		5	// 짧은 S자형 곡선: 예전 B
#define	MMC_CMD_PATH_DIRECTION_NARROW	6	// 레일 간격이 900mm 이하일때의 짧은 S자형 곡선: 예전 V

// -------------------------------------------
//1 Parameter
// -------------------------------------------
/**
@brief	 Motion 제어기에 전달할 주행 파라미터
@author  임태웅
@date	 2013.05.15
*/
typedef struct _DRIVING_PARAM
{
	//2 주행 기본 설정값
	volatile int	GearRatioOfDriving;		///< 주행 Gear 비(Cts/m) --> 1m 에 1mm 이내의 오차
	
	//2 근접제어
	volatile int	OHTDetectAreaDist[6];		///< 대차 감지 센서 영역당 설정 거리 (단위 Cts)
	volatile int	OHTDetectAreaSpeed[6];	///< 대차 감지 센서 영역당 설정 속도 (단위 Cts/s)
	volatile int	OBSDetectAreaDist[6];		///< 장애물 감지 센서 영역당 설정 거리 (단위 Cts)
	volatile int	OBSDetectAreaSpeed[6];	///< 장애물 감지 센서 영역당 설정 속도 (단위 Cts/s)
	volatile int	AccelOfNormalPath;		///< 직진구간에서의 가속 값
	volatile int	DecelOfNormalPath;		///< 직진구간에서의 감속 값
	volatile int	AccelOfCurvePath;		///< 곡선구간에서의 가속 값
	volatile int	DecelOfCurvePath;		///< 곡선구간에서의 감속 값
}DRIVING_PARAM;


// -------------------------------------------
//1 운영정보 (Receive)
// -------------------------------------------
/**
@brief	 Motion 제어기로 부터 받는 Node 정보
@author  임태웅
@date	 2013.05.15
*/
typedef struct _NODE_CHECK_DATA
{
	volatile int NodeCountOfPreCheck;	///< 노드 개수
	volatile int NodeCount;				///< 노드 개수
	
	// SetHome, setOffset 시 하면 현재 위치를 뺀 값으로 재저장
	volatile int PositionOfDrivingTagPreCheck;	///< 주행 노드 사전 감지된 위치		
	volatile int PositionOfDrivingTag;			///< 주행 노드 감지된 위치			 
	volatile int PositionOfTransTagFirst;		///< 이적재 노드 감지된 위치(Front)
	volatile int PositionOfTransTagFinal;		///< 이적재 노드 감지된 위치(Rear)
	
}NODE_CHECK_DATA;

// -------------------------------------------
//1 운영정보 (Send)
// -------------------------------------------
/**
@brief	 Motion 제어기에 전달할 근접제어 참고 data
@author  임태웅
@date	 2013.05.15
*/
#if(USE_CHANGENODE == 1)
typedef struct _DETECT_INFO
{
	volatile char	UseFollowControlFlag;	///< 근접 제어 사용 여부: true 면 사용, false면 미사용

	volatile char	OHTDetectType;				///< Link의 OHTDetectType
	volatile char	OBSDetectType;				///< Link의 OBSDetectType

	volatile int	NodeCount;				///< 노드 개수
	volatile int	DistSumOfStraightPath;	///< 직진 길이 이어지는 지점까지의 위치 (Cts)
	volatile int	FollowCommandType;		
	volatile int	PBSIgnoreFlag;

	volatile int	Reserved_1;
	volatile int	Reserved_2;
}DETECT_INFO;
#else
typedef struct _DETECT_INFO
{
	volatile char	UseFollowControlFlag;	///< 근접 제어 사용 여부: true 면 사용, false면 미사용

	volatile char	Type;				///< Link의 OHTDetectType

	volatile int	NodeCount;				///< 노드 개수
	volatile int	DistSumOfStraightPath;	///< 직진 길이 이어지는 지점까지의 위치 (Cts)
	volatile int	FollowCommandType;
	volatile int	PBSIgnoreFlag;

}DETECT_INFO;
#endif

typedef struct _PAUSE_INFO
{
	volatile int	Pause;		
	volatile int	Commander;	
}PAUSE_INFO;

// for change node (jhoun.lim)
struct OPTIONAL_NODE_INFO
{
	volatile char NodeType;				//	0x05 : Tag-type Optional Node In, 0x06 : Distance-type Optional Node In, 0x07 : Combo-type Optional Node In
	volatile int DistON;				// Optional Node 까지의 거리
	volatile int DistAfterON;			// Optional Node 이후에 패턴이 바뀌는 위치
	volatile char OHTDetectTypeOpt;		// Optional Node 에서 변경할 Type(대차)
	volatile char OBSDetectTypeOpt;		// Optional Node 에서 변경할 Type(장애물)

	volatile int Reserved1;
	volatile int Reserved2;
};
// -------------------------------------------
// Axis Command Structure --> AMC CMD Structure
// NONE : 0
// MoveP: 1 // MoveN: 2 // MoveS: 3 // MoveDS: 4
// MoveP_T: 5 // MoveN_T: 6 // MoveS_T: 7 // MoveN_T: 8
// Reset: 9 // setHome: 10
typedef struct _CMD_AXIS
{
	volatile char	CmdType;			///< Move (P,N,S,DS - TimeBase, AccelBase)	/ reset / SetHome 등등 
	volatile int	Accel;			///< 가속도(Cts/s2) or 시간(msec)
	volatile int	Decel;			///< 가속도(Cts/s2) or 시간(msec)
	volatile int	Speed;			///< 속도(Cts/s)
	volatile int	Distance;		///< Target 거리(Cts)

	volatile int	TriggerCtrlInfo;	///< Trigger Control 하는 정보: 최대 Target까지 일정 값 이동 명령을 1ms 단위로 인가 --> 특정 트리거에 종료 내용은 TRIGGER_CTRL_INFO 구조체 참조
}CMD_AXIS;


// -------------------------------------------
//1 Analyze & Make Data
// -------------------------------------------

// AMC_CMD.Cmd의 bitfield (AMC Command Type Structure)
// 처리 방식이 2가지가 있다
//  ① 전달해야 하는 값이 있는 경우: 이전 값과 다르면 ON
//  ② 전달해야 하는 값이 없는 경우: 명령이 들어오면 ON
typedef struct _CMD_ANALYSIS
{	
	unsigned int SetOutBit					: 1;		///< I/O의 Output 설정 : ①
	unsigned int ClearNodeCount				: 1;		///< Node Count Clear : ②
	unsigned int MoveAxis1					: 1;		///< 1축 이동 명령 : ①
	unsigned int MoveAxis2					: 1;		///< 2축 이동 명령 : ①
	unsigned int MoveAxis3					: 1;		///< 3축 이동 명령 : ①
	unsigned int MoveAxis4					: 1;		///< 4축 이동 명령 : ①
	unsigned int SetDetectInfo				: 1;		///< 전방 감지 Type 설정 : ②
	unsigned int SetToreqLimit				: 1;		//1	제거 예정 /< 주행 Gear 비 설정 : ①
	unsigned int SetToreqLimitRelease		: 1;		//1	제거 예정 /< 감속 Limit 설정 : ①
	unsigned int CompensateRearEncoder		: 1;		///< Rear값을 Front 값으로 맞춰주는 명령 : ②

	unsigned int PauseControl				: 1;		///< Pause명령  : ②
	unsigned int SetSlopeTime				: 1;		///< 경사로 구간 설정 명령 : ②
	unsigned int SetSlopeTimeRelease		: 1;		///< 경사로 구간 해제  명령 : ②
	unsigned int SetTargetPos				: 1;		///< 타겟위치가 근처일경우 설정 명령 : ②
	unsigned int SetTargetPosRelease		: 1;		///< 타겟위치 해제 명령 : ②
	unsigned int SetFirstNodeCountAdd		: 1;        ///< 강제로 첫번쨰 노드 카운트 1로 증가 명령
	unsigned int SetVHLDetectIgnor		 	: 1;        ///< 근접제어 무시 설정 명령
	unsigned int SetVHLDetectIgnorRelease	: 1;        ///< 근접제어 무시 해제 명령
	unsigned int SetDetectStopDecelDownRelease		: 1;///< 근접 정지 감속도(0.5m/s^2) 변경 해제 명령
	unsigned int SetDetectStopDecelDown	 			: 1;///< 근접 정지 감속도(0.5m/s^2) 변경 설정 명령
	unsigned int SetVHLDetectIOSet	 			: 1;///< 근접 정지 감속도(0.5m/s^2) 변경 설정 명령

	unsigned int Reserved					: 11;
}CMD_ANALYSIS;

// AMC_CMD.Cmd 를 쉽게 만들고 분석하기 위한 union
typedef union _CMD_ANALYSIS_UNION
{
	CMD_ANALYSIS CmdAnalysis;	///< AMC_CMD.Cmd의 bitfield
	int Cmd;
}CMD_ANALYSIS_UNION;


// AMC_CMD_BIT_ANALYSIS
typedef enum _MOTION_CONTROLLER_CMD_TYPE
{
	CMD_TYPE_SET_OUTBIT							= 0,
	CMD_TYPE_CLEAR_NODE_COUNT					,
	CMD_TYPE_MOVE_AXIS1							,
	CMD_TYPE_MOVE_AXIS2							,
	CMD_TYPE_MOVE_AXIS3							,
	CMD_TYPE_MOVE_AXIS4							,
	CMD_TYPE_SET_DETECT_INFO					,
	CMD_TYPE_SET_TORQUE_LIMIT_RESPONSE			,
	CMD_TYPE_SET_TORQUE_LIMIT_REALEASE_RESPONSE	,
	CMD_TYPE_COMPENSATE_REAR_ENCODER			,
	CMD_TYPE_PAUSE_CONTROL                      ,
	CMD_TYPE_SET_SLOPE_TIME						,
	CMD_TYPE_SET_SLOPE_TIME_RELEASE             ,
	CMD_TYPE_SET_TARGET_POS						,
	CMD_TYPE_SET_TARGET_POS_RELEASE             ,
 	CMD_TYPE_SET_FIRSTNODE_ADD		            ,
	CMD_TYPE_SET_VHLDETECTIGNOR		            ,
	CMD_TYPE_SET_VHLDETECTIGNOR_RELEASE         ,
	CMD_TYPE_SET_DETECTSTOPDECELDOWN_RELEASE    ,
	CMD_TYPE_SET_DETECTSTOPDECELDOWN            ,
}MOTION_CONTROLLER_CMD_TYPE;


//2	Trigger Command
typedef struct _TRIGGER_CTRL_INFO
{	
	unsigned int StartOnSpeedDown		: 1;		///<명령 인가 시점( 0:바로 시작, 1:이전 프로파일의 감속 시점에 명령 인가
	unsigned int UseCurrSpeed			: 2;		///<사용할 속도( 0: 명령 받은 속도, 1: 현재 속도, 2:Min(명령속도, 현재속도), 4:MAX(명령속도, 현재속도) )
	unsigned int UseSmallAdd			: 1;		///<Small Add ( 0: NoUse, 1: Use )
	unsigned int TriggerIONumber		: 8;		///< I/O 번호 (0~255): ②
	unsigned int IsTriggerOnRisingEdge	: 1;		///<Triger Type ( 0: Falling Edge, 1: Rising Edge )
	unsigned int AdditionalDist		: 19;	///< 1회 추가 거리 ( 0~2^19 / 방향 정보는 최종 타겟 거리에서 획득): 524288cts --> 주행: 4599mm
}TRIGGER_CTRL_INFO;


// AMC_CMD.Cmd 를 쉽게 만들고 분석하기 위한 union
typedef union _TRIGGER_CTRL_INFO_UNION
{
	TRIGGER_CTRL_INFO TriggerInfo;	///< AMC_CMD.Cmd의 bitfield
	int Cmd;
}TRIGGER_CTRL_INFO_UNION;


//2	VoltageMonitor
typedef struct VOLTAGE_MONITOR_INFO_
{	
	unsigned int Percent_5V	: 8;		///< 5볼트 접압의 Percent: 0~255%
	unsigned int Percent_M12V	: 8;		///< -12볼트 접압의 Percent: 0~255%
	unsigned int Percent_P12V	: 8;		///< 12볼트 접압의 Percent: 0~255%
	unsigned int Percent_3P3V	: 8;		///< 24볼트 접압의 Percent: 0~255%
}VOLTAGE_MONITOR_INFO;


// AMC_CMD.Cmd 를 쉽게 만들고 분석하기 위한 union
//typedef union VOLTAGE_MONITOR_INFO_UNION_
//{
//	VOLTAGE_MONITOR_INFO VoltageInfo;	///< 전압 정보의 bitfield
//	int Data;
//}VOLTAGE_MONITOR_INFO_UNION;
//


#endif
