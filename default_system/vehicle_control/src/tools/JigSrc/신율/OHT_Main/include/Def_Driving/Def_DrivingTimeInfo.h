#ifndef Def_DrivingTimeInfoH
#define Def_DrivingTimeInfoH

#include <stdio.h>
#include "Def_TimeCheck.h"	//TIME_CHECK

/**
@brief	 전방 감시 시각 관리 항목
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_TIME_FRONT_OBSERVE_
{
	TIME_CHECK	OHTDetectFail;	///< 대차 감지 센서 Fail Flag 및 감지 시각
	TIME_CHECK	OBSDetectFail;	///< 장애물 감지 센서 Fail Flag 및 감지 시각
	TIME_CHECK	OBSDetect;		///< 장애물 감지 Flag 및 감지 시각
}DRIVING_TIME_FRONT_OBSERVE;

/**
@brief	 조향 시각 관리 항목
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_TIME_STEER_
{
	TIME_CHECK	SteeringChangeCmded;	///< 조향 변경 명령 시작 Flag 및 감지 시각: 채터링까지 동작 완료 시 false로 변경 
	TIME_CHECK	SteeringNotMatch;		///< 조향 상태 이상 Flag 및 감지 시각
	TIME_CHECK	FrontSteeringArrived;	///< 전방 조향 도착 Flag 및 감지 시각: 채터링 및 Timeout 처리
	TIME_CHECK	RearSteeringArrived;	///< 후방 조향 도착 Flag 및 감지 시각: 채터링 및 Timeout 처리
	TIME_CHECK	FrontSteeringDeparted;	///< 전방 조향이 명령을 받고 위치 이동 시작 Flag 및 감지 시각( 토크 감도 변경 목적 )
	TIME_CHECK	RearSteeringDeparted;	///< 후방 조향이 명령을 받고 위치 이동 시작 Flag 및 감지 시각( 토크 감도 변경 목적 )
}DRIVING_TIME_STEER;

/**
@brief	 이상 정지 시각 관리 항목
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_TIME_STOP_
{
	TIME_CHECK	OnAbnormalStop;	///< 이상 정지 Flag 및 감지 시각
	TIME_CHECK	OnAbnormalRetry;	///< 이상 정지 Flag 및 감지 시각
}DRIVING_TIME_STOP;

/**
@brief	 시각 집합 관리 항목
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_TIME_INFO_SET_
{
	DRIVING_TIME_FRONT_OBSERVE		FrontObserve;	///< 전방 감시
	DRIVING_TIME_STEER				Steer;			///< 조향 감시
	DRIVING_TIME_STOP				Stop;			///< 정지 감시
}DRIVING_TIME_INFO_SET;

#endif

