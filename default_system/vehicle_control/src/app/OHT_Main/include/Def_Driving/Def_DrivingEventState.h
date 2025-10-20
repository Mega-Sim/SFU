#ifndef Def_DrivingEventStateH
#define Def_DrivingEventStateH

#include <stdio.h>
#include "Def_Variables.h"	// BYTE, WORD, UINT, UINT64

/**
@brief	 주행 Event
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_EVENT_
{
	UINT	CmdAdded			: 1;	///< 1-1. 명령 추가
	UINT	TargetChanged		: 1;	///< 1-2. 목적지 변경
	UINT	PreCheck			: 1;	///< 1-4. Node가 350mm 전 감지되었을 경우 ( NodeCount: PreCheck = Check +1 / 현재 위치에서 (350-마진mm) 로 이동 / 속도 증가는 추가 검토 필요 )
	UINT	Check				: 1;	///< 1-8. Node가 감지된 경우 ( NodeCount: Check = PreChec )

	UINT	SpeedDownPassed		: 1;	///< 2-1. 감속 지점을 통과하는 순간 발생
	UINT	FrontSteerMoved		: 1;	///< 2-2. 명령에 전방 조향이 떨어지고 일정시간 경과 시 ( Moving Torque → Stop Torque )
	UINT	RearSteerMoved		: 1;	///< 2-4. 명령에 후방 조향이 떨어지고 일정시간 경과 시 ( Moving Torque → Stop Torque )
	UINT	FrontSteerCompleted	: 1;	///< 2-8. 전방 조향이 조향 변경이 완료 되는 순간 ( Chattering: 100ms / 조향 제한 거리 증가 → +(350 - 마진)mm )
	
	UINT	SteerAllCompleted	: 1;	///< 3-1. 전후방 조향 변경이 완료 되는 순간 ( Chattering: 100ms )
	UINT	PushedByGuide		: 1;	///< 3-2. ChangeAreaByGuide 에서 ( 조향이 변경되는 순간 )
	UINT	StopWithNoReason	: 1;	///< 3-4. 명령 준 거리에 도착하지 전 이상 정지 ( 장애물/대차 감지가 되지 않은 경우 / Small AddMove 시 제외 ) → 재명령
	UINT	DistNotMatch		: 1;	///< 3-8. 명령 준 거리와 반영되지 않은 경우 → 재명령

	UINT	MarkCmd				: 1;	///< 4-1. 마크 루틴 들어가면서 부터 주행 명령 발생 시

	UINT	FrontDetectionChanged: 1;	///< 5-1. 전방 감지 상태 변경 시
	
	UINT	Reserved			: 18;	///< 예비

}DRIVING_EVENT;


/**
@brief	 주행 State
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_MARK_STATE_
{
	WORD	OnRoutineIn         : 1;	///< Mark Routine 시작
	WORD	FirstMet            : 1;	///< Node: PreCheck 감지 시(NodeCount) / Tag Station: Front 감지 시(I/O) / QR: QR 처음 만났을 경우
	WORD	SecondMet           : 1;	///< Node: Check 감지 시(NodeCount) / Tag Station: Rear 감지 시(I/O) / QR: QR 거리 보정
	WORD	IdCheck             : 1;	///< ID  확인 실시 
	WORD	Arrived             : 1;	///< 최종 위치 도착하여 정지 시

	WORD	OverRun             : 1;	///< 넘어갔을 경우
	WORD	Recovery            : 1;	///< 넘어간 후 복구

	WORD	OnMoveCmd           : 1;	///< 움직이는 명령 인가 시 true, 움직이면 false로 변경

	WORD	FollowCtrlOffArea	: 1;	///< 근접제어를 Off 하는 영역

	WORD	Reserved			: 7;
}DRIVING_MARK_STATE;


/**
@brief	 주행 State
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_STATE_
{
	UINT	NodePreChecked			: 1;	///< Node Count가 PreCheck>Check 경우 / 마지막 Mark 되었을 경우 ( Node Mark: 다음 Node가 350+40mm 이내 존재 ) / Station Mark: 다음 Node가 PreCheck와 Check 사이 존재
	UINT	OutInNodeDiffSteer      : 1;	///< Out/In Node 에서 조향이 다른 명령인 경우

	UINT	SteerMoving             : 1;	///< 조향 변경 중
	UINT	SteerFrontCompleted     : 1;	///< 앞 조향 이동이 완료 ( Mark아닐 경우: 조향 제한 거리 증가 → +(350 - 마진)mm )
	UINT	SteerAllCompleted       : 1;	///< 전후 조향 이동 완료 ( 시작 시 이조건 미 충족 시 주행 명령 미 실행 )
	UINT	SteerChangeAreaByGuide  : 1;	///< 가이드 레일에 의해 조향이 변경 가능 구간 ( N분기 중간( 진입+700 ~ 진출-500 ) )
	
	UINT	StopByFrontDetect       : 1;	///< 전방 감지에 의해 정지 시 ( 장애물, 대차 감지에 의해 정지 시 )
	UINT	SpeedDownByFrontDetect  : 1;	///< 전방 감지에 의해 감속 시 ( 장애물, 대차 감지에 의해 감속 시 )
	
	UINT	SpeedDownPassed        	: 1;	///< 감속시작 지점 통과여부
	UINT	MarkArea	        	: 1;	///< Mark시작 지점 통과여부
	UINT 	DualStageStbArea		: 1;
	UINT	Reserved            	: 21;	///< 예비
}DRIVING_STATE;


/**
@brief	 주행 State
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_EVENT_AND_STATE_
{
	DRIVING_EVENT		Event;
	DRIVING_STATE		State;
	DRIVING_MARK_STATE	MarkState;		
}DRIVING_EVENT_AND_STATE;


#endif
