#ifndef Def_PositionInfoH
#define Def_PositionInfoH

#include <stdio.h>
#include "Def_Variables.h"	// BYTE, WORD, UINT, UINT64
#include "Def_PathInfo.h"	// NODE_TYPE, PATH_STEERING_INFO, PATH_DIRECTION_INFO, MARK_TYPE

/**
@brief	 위치 정보
@author  ehoto
@date	 2013.06.19
*/
typedef struct POSITION_INFO_
{
	MARK_TYPE	MarkType;	///< 0:Node / 1:TagStation / 2:QRLeft / 3:QRRight
	UINT		uNode;		///< 보고용 Node ID
	UINT		uNextNode;	///< 보고용 Next Node ID
	UINT		uStation;	///< 보고용 Station ID
	double		dOffset;		///< Node 에서 Station으로의 거리, 다음 명령 수행 시 기준이 되는 Offset"
}POSITION_INFO;

/**
@brief	 위치 상태 ( 정보 + Mark 여부 )
@author  ehoto
@date	 2013.06.19
*/
typedef struct POSITION_STATUS_
{
	bool			bMarked;				///< true: 정위치 완료 / false: 정위치 미완료
	UINT			uRealArrivedNode;	///< 주행 완료 후 마지막 만난 Node (Node 주행 경우: Node와 일치 / Staion 주행 경우: Node or NextNode) --> 다음 명령 수행 시 기준이 되는 Node
	POSITION_INFO	Info;				///< 현재 위치 정보
}POSITION_STATUS;

/**
@brief	 위치 정보
@author  ehoto
@date	 2013.08.06
*/
//typedef struct COMMANDED_POSITION_INFO_
//{
//	MARK_TYPE	MarkType;	///< 0:Node / 1:TagStation / 2:QRLeft / 3:QRRight
//	double		dCmdPos;	///< 명령 위치: 보정하지 않은 위치
//	double		dDecelDist;	///< 감속 거리: Map 속도 기준으로 필요한 감속 거리
//	double		dSpeed;		///< 속도: Map 속도
//	double		dDecel;		///< 감속도
//	int			nCount;		///< 명령 준 지점의 NodeCount(Station 으로의 명령 일 경우 Next Node의 Count)
//}COMMANDED_POSITION_INFO;


/**
@brief	 현재 위치 상태 + 목적지 정보
@author  ehoto
@date	 2013.06.19
*/
typedef struct CURR_AND_TARGET_POSTION_STATUS_
{
	POSITION_STATUS			CurrPositionStatus;	///< 현재 위치 정보 및 상태
	POSITION_INFO			TargetPosition;		///< 목적지 위치 정보
//	COMMANDED_POSITION_INFO	CommandedPosition;	///< 최근 인가된 명령 위치 정보
}CURR_AND_TARGET_POSTION_STATUS;

#endif
