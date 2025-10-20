#ifndef Def_DrivingCtrlInfoH
#define Def_DrivingCtrlInfoH


#include <stdio.h>
#include "Def_Variables.h"		// BYTE, WORD, UINT, UINT64
#include "Def_AxisCtrlInfo.h"	// AXIS_CTRL_CMD_TYPE, STATIC_ASYNC_AXIS_CTRL_INFO, DYNAMIC_AXIS_CTRL_INFO, CTRL_INFO_OF_ASYNC_AXIS_MOVE, CTRL_INFO_OF_SYNC_AXIS_MOVE
//#include "Def_PathInfo.h"	// NODE_TYPE, PATH_STEERING_INFO, PATH_DIRECTION_INFO, MARK_TYPE
#include "Def_Driving.h"	//USER_DETECT_INFO


/**
@brief	 조향 명령
@author  임태웅
@date	 2013.06.19
*/
typedef struct CTRL_INFO_OF_STEER_
{
	BYTE	FrontCmdOn		: 1;	///< 0: 명령 없음 / 1: 명령 있음
	BYTE	FrontDirection	: 1;	///< 0:왼쪽 / 1:오른쪽
	BYTE	FrontTorque		: 2;	///< 0:Free / 1:Stop / 2:Moving / 3:견인
	
	BYTE	RearCmdOn		: 1;	///< 0: 명령 없음 / 1: 명령 있음
	BYTE	RearDirection	: 1;	///< 0:왼쪽 / 1:오른쪽
	BYTE	RearTorque		: 2;	///< 0:Free / 1:Stop / 2:Moving / 3:견인
	
}CTRL_INFO_OF_STEER;


/**
@brief	 ID Reader 명령
@author  임태웅
@date	 2013.06.19
*/
typedef enum CTRL_INFO_OF_ID_READER_CMD_TYPE_
{
	CTRL_INFO_OF_ID_READER_CMD_NONE    				= 0,
	CTRL_INFO_OF_ID_READER_CMD_ENABLE				,
	CTRL_INFO_OF_ID_READER_CMD_DISABLE				,
	CTRL_INFO_OF_ID_READER_CMD_RESTART				,
	CTRL_INFO_OF_ID_READER_CMD_TRIGGER_ON 			,
	CTRL_INFO_OF_ID_READER_CMD_READ_DATA			,
	CTRL_INFO_OF_ID_READER_CMD_TRIGGER_OFF			,
	CTRL_INFO_OF_ID_READER_CMD_READ_SEQUENTIALLY	,			///< 100ms 의 delay 있음
}CTRL_INFO_OF_ID_READER_CMD_TYPE;



/**
@brief	 전방 감지 명령
@author  임태웅
@date	 2013.07.31
*/
typedef struct CTRL_INFO_OF_FRONT_DETECT_
{
	bool bCmdOn;				///< false: 명령 없음 / true: 명령 있음
	USER_DETECT_INFO Info;	///<설정 값
	
}CTRL_INFO_OF_FRONT_DETECT;



/**
@brief	 주행부 명령 집합
@author  임태웅
@date	 2013.06.19
*/
typedef struct DRIVING_CTRL_INFO_SET_
{
	CTRL_INFO_OF_STEER				Steer;		///< 조향 명령
	CTRL_INFO_OF_SYNC_AXIS_MOVE		Axis;		///< 주행 명령
	CTRL_INFO_OF_FRONT_DETECT		FrontObservation;	///< 전방 감시 명령
	CTRL_INFO_OF_ID_READER_CMD_TYPE	IdReader[14];	///< ID Reader
}DRIVING_CTRL_INFO_SET;

#endif
