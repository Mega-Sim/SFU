#ifndef Def_DrivingPositionInfoH
#define Def_DrivingPositionInfoH

#include <stdio.h>
#include "Def_Variables.h"		// BYTE, WORD, UINT, UINT64
#include "Def_PathInfo.h"		// NODE_TYPE, PATH_STEERING_INFO, PATH_DIRECTION_INFO, MARK_TYPE
#include "Def_PositionInfo.h"	// POSITION_INFO, POSITION_STATUS, CURR_AND_TARGET_POSTION_STATUS


/**
@brief	 정위치 완료 후 저장하는 값
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_MARK_INFO_
{
	POSITION_STATUS		PositionStatus;	///< Mark 완료시 의 map 기준 위치 정보
	double          	dFrontPosition;	///< 정위치 때 전륜의 엔코더 값 (정위치 후 이상 이동 감지 시 사용되는 기준 값)
	double          	dRearPosition;	///< 정위치 때 후륜의 엔코더 값 (정위치 후 이상 이동 감지 시 사용되는 기준 값)
}DRIVING_MARK_INFO;	



/**
@brief	 거리 보정 항목
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_COMPENSATION_INFO_
{
	double	dDistErrorSum;			///< 누적 거리 오차: 엔코더값 - Map 기준 값
	double  dLinkDistError;			///< Link 사이의 거리값 오차( 엔코더값 - Map기준 값 ) ( 누적 거리 오차 - 이전 누적 거리 오차 )

	double  dPreArriveOffset;		///< 출발 Station Offset( 이전 주행 완료 시 저장 : MARK_INFO.PositionInfo.Offset)
	
	double  dCompensatedPosition;	///< Map 기준 위치( 엔코더값 - 누적 거리 오차값 + 출발 Station Offset )
	
	double  dWearRatio;				///< 마모율: 장거리 주행 시 주행 거리 대비 발생한 누적 오차 ( 누적 거리 오차 / Map 기준 주행 거리 : 주행 거리 250m 이상 -: 기준 대비 Wheel 과대 / +: 기준 대비 Wheel 마모 발생 )
}DRIVING_COMPENSATION_INFO;


/**
@brief	 명령 처리를 위한 기준 정보(이전 명령에서 생성되는 값
@author  임태웅
@date	 2013.08.08
*/
typedef struct DRIVING_CTRL_REFERENCE_
{
	double	dCmdSpeed;		///< 명령 내려진 속도
	double	dRefSpeed;		///< 기준 속도 (Map)
	double	dCmdDecel;		///< 명령 내려진 가속도
	int		nKeyCount;		///< 명령 내려진 위치의 Node Count
	int		nCount;			///< 명령 내려진 위치의 Node Count
	double	dDist;			///< 명령 내려진 실 거리 (모터 값)
	double	dSpeedDownPos;	///< 감속 시작 실 위치 (모터 값)
}DRIVING_CTRL_REFERENCE;


#endif
