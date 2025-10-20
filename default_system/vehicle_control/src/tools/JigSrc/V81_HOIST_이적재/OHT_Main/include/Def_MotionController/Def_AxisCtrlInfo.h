#ifndef Def_AxisCtrlInfoH
#define Def_AxisCtrlInfoH

#include <stdio.h>
#include "Def_Variables.h"	// BYTE, WORD, UINT, UINT64
#include "MotionAxis.h"		// USER_MOVE_DS_ADDITIONAL_INFO

/**
@brief	 조향 명령
@author  임태웅
@date	 2013.06.19
*/
typedef enum AXIS_CTRL_CMD_TYPE_
{
	AXIS_CTRL_CMD_NONE		= 0,	///< 명령 없음
	AXIS_CTRL_CMD_STATIC	= 1, 	///< 정적 명령
	AXIS_CTRL_CMD_MOVE_DS	= 2, 	///< moveDS
	AXIS_CTRL_CMD_MOVE_S	= 3, 	///< moveS
	AXIS_CTRL_CMD_MOVE_P	= 4, 	///< moveP
	AXIS_CTRL_CMD_MOVE_N	= 5 	///< moveN
}AXIS_CTRL_CMD_TYPE;

/**
@brief	 비동기 축 제어 정적 명령 값 (주행 준비/완료 시)
@author  임태웅
@date	 2013.06.19
*/
typedef struct STATIC_ASYNC_AXIS_CTRL_INFO_
{
	WORD	NodeCountReset		: 1;	///< 0: 명령 없음 / 1: 명령 있음

	WORD	ResetCmdOn		: 1;	///< 0: 명령 없음 / 1: 명령 있음
	WORD	AmpCmdOn		: 1;	///< 0: 명령 없음 / 1: 명령 있음
	WORD	AmpOn			: 1;	///< 0: Amp Off / 1: Amp On
	WORD	SetHomeCmdOn	: 1;	///< 0: 명령 없음 / 1: 명령 있음
	
	WORD	Reserved	: 11;
}STATIC_ASYNC_AXIS_CTRL_INFO;

/**
@brief	 동기 축 제어 정적 명령 값 (주행 준비/완료 시)
@author  임태웅
@date	 2013.06.19
*/
typedef struct STATIC_SYNC_AXIS_CTRL_INFO_
{
	WORD	NodeCountReset		: 1;	///< 0: 명령 없음 / 1: 명령 있음

	WORD	Axis1ResetCmdOn		: 1;	///< 0: 명령 없음 / 1: 명령 있음
	WORD	Axis1AmpCmdOn		: 1;	///< 0: 명령 없음 / 1: 명령 있음
	WORD	Axis1AmpOn			: 1;	///< 0: Amp Off / 1: Amp On
	WORD	Axis1SetHomeCmdOn	: 1;	///< 0: 명령 없음 / 1: 명령 있음
	
	WORD	Axis2ResetCmdOn		: 1;	///< 0: 명령 없음 / 1: 명령 있음
	WORD	Axis2AmpCmdOn		: 1;	///< 0: 명령 없음 / 1: 명령 있음
	WORD	Axis2AmpOn			: 1;	///< 0: Amp Off / 1: Amp On
	WORD	Axis2SetHomeCmdOn	: 1;	///< 0: 명령 없음 / 1: 명령 있음

	WORD	AxisSetTorqueLimit			: 1;	///< 0: 명령 없음 / 1: 명령 있음
	WORD	AxisSetTorqueLimitRelease	: 1;	///< 0: 명령 없음 / 1: 명령 있음

	WORD	Reserved			: 5;
	
}STATIC_SYNC_AXIS_CTRL_INFO;

/**
@brief	 동작 명령 값
@author  임태웅
@date	 2013.06.19
*/
typedef struct DYNAMIC_AXIS_CTRL_INFO_
{
	double                      dAccel;              ///< 가속도(단위: mm/msec2)
	double                      dDecel;              ///< 감속도(단위: mm/msec2)
	double                      dSpeed;              ///< 속도(단위: mm/msec)
	double                      dDist;               ///< 거리(단위: mm)
	USER_MOVE_DS_ADDITIONAL_INFO     AdditionalInfo;    ///< 추가 정보
}DYNAMIC_AXIS_CTRL_INFO;


/**
@brief	 주행 명령
@author  임태웅
@date	 2013.06.19
*/
typedef struct CTRL_INFO_OF_ASYNC_AXIS_MOVE_
{
	AXIS_CTRL_CMD_TYPE				Type;		///< 0: AXIS_CTRL_CMD_NONE (명령 없음) 1: AXIS_CTRL_CMD_STATIC (정적 명령) 2: AXIS_CTRL_CMD_MOVE_DS (moveDS) 3: AXIS_CTRL_CMD_MOVE_S (moveS) 4: AXIS_CTRL_CMD_MOVE_P (moveP) 5: AXIS_CTRL_CMD_MOVE_N (moveN)
	STATIC_ASYNC_AXIS_CTRL_INFO		Static;		///< 정적 명령
	DYNAMIC_AXIS_CTRL_INFO			Dynamic;	///< 동적 명령
}CTRL_INFO_OF_ASYNC_AXIS_MOVE;

/**
@brief	 주행 명령
@author  임태웅
@date	 2013.06.19
*/
typedef struct CTRL_INFO_OF_SYNC_AXIS_MOVE_
{
	AXIS_CTRL_CMD_TYPE				byType;		///< 0: AXIS_CTRL_CMD_NONE (명령 없음) 1: AXIS_CTRL_CMD_STATIC (정적 명령) 2: AXIS_CTRL_CMD_MOVE_DS (moveDS) 3: AXIS_CTRL_CMD_MOVE_S (moveS) 4: AXIS_CTRL_CMD_MOVE_P (moveP) 5: AXIS_CTRL_CMD_MOVE_N (moveN)
	STATIC_SYNC_AXIS_CTRL_INFO		Static;		///< 정적 명령
	DYNAMIC_AXIS_CTRL_INFO			Dynamic;	///< 동적 명령
}CTRL_INFO_OF_SYNC_AXIS_MOVE;

#endif
