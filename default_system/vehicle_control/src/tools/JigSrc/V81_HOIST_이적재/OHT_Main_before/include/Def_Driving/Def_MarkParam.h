#ifndef Def_MarkParamH
#define Def_MarkParamH

#include <stdio.h>

/**
@brief	 Mark 거리 파라미터
@author  임태웅
@date	 2013.06.23
*/
typedef struct MARK_DIST_PARAM_
{
	double		dMarginToTarget;			///< Target으로의 마진	
	double		dDistFromFirstToSecond;		///< First 만난 후 Second까지의 거리	
	double		dLengthOfSecond;			///< Second 감지 거리	
	double		dAddDistAfetrFirstMet;		///< First 만난 후 이동 거리(Small Add)	
	double		dAddDistAfterSecondMet;		///< Second 만난 후 이동 거리(Small Add)	
	double		dForwardMoveDistLimit;		///< 추가 전진 Limit	
	double		dBackMoveDistLimit;			///< 후진 Limit	
	double		dMarkInPosLimit;			///< 정위치 위치 이탈 기준 Limit	
	double		dMinLengthToNext;			///< 같은 종류 Node/Station간 최소 간격	
	double		dIgnoreTagDist;				///< 무시Tag 거리
}MARK_DIST_PARAM;

/**
@brief	 Mark 속도 파라미터
@author  임태웅
@date	 2013.06.23
*/
typedef struct MARK_SPEED_PARAM_
{
	double		dOfFirstMet;			///< First 만난 지점에서의 속도	
	double		dOfSecondMet;		///< Second 만난 지점에서의 속도	
}MARK_SPEED_PARAM;

/**
@brief	 Mark 가감속도 파라미터
@author  임태웅
@date	 2013.06.23
*/
typedef struct MARK_ACCEL_PARAM_
{
	double		dOfFirstMet;			///< First 만난 지점에서의 가감속도	
	double		dOfSecondMet;		///< Second 만난 지점에서의 가감속도	
}MARK_ACCEL_PARAM;

#endif