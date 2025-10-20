#ifndef Def_DrivingSteerInfoH
#define Def_DrivingSteerInfoH

#include <stdio.h>
#include "Def_Variables.h"	// BYTE, WORD, UINT, UINT64
#include "Def_PathInfo.h"


/**
@brief	 주행 정적 명령 (주행 준비/완료 시)
@author  ehoto
@date	 2013.06.19
*/
typedef struct STEER_INFO_
{
	WORD	PathSteer			: 4;	///< 0: PATH_STEERING_NONE (None) / 1: PATH_STEERING_LEFT (Left) / 2: PATH_STEERING_RIGHT (Right) / 3: PATH_STEERING_N_BRANCH_L2R (L2R) / 4: PATH_STEERING_N_BRANCH_R2L (R2L)
	WORD	PathDirection		: 4;	///< 0: PATH_DIRECTION_NO_USE (미사용) / 1: PATH_DIRECTION_NORMAL (직진) / 2: PATH_DIRECTION_LEFT (Left) / 3: PATH_DIRECTION_RIGHT (Right) / 4: PATH_DIRECTION_SHORT (N분기 사용: 예전 S) / 5: PATH_DIRECTION_BOTH (짧은 S자형 곡선: 예전 B) / 6: PATH_DIRECTION_NARROW (레일 간격이 900mm 이하일때의 짧은 S자형 곡선: 예전 V)
	WORD	FrontPosition		: 2;	///< 0: None / 1:Left / 2:Right / 3:Both(Error) (STEER_HW_INFO_POSITION)
	WORD	FrontCmdDirection	: 1;	///< 0:왼쪽 / 1:오른쪽 (STEER_HW_INFO_CMD)
	WORD	FrontMoving			: 1;	///< 0: None / 1:Moving ~ 명령 인가 시 Moving, 채터링까지 관료 하면 None
	WORD	RearPosition		: 2;	///< 0: None / 1:Left / 2:Right / 3:Both(Error) (STEER_HW_INFO_POSITION)
	WORD	RearCmdDirection	: 1;	///< 0:왼쪽 / 1:오른쪽 (STEER_HW_INFO_CMD)
	WORD	RearMoving			: 1;	///< 0: None / 1:Moving ~ 명령 인가 시 Moving, 채터링까지 관료 하면 None
}STEER_INFO;


#endif
