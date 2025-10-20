#ifndef Def_PathInfoH
#define Def_PathInfoH

#include <stdio.h>
#include "Def_Variables.h"

#define ID_NONE 0


typedef enum DETECTINFOTYPE_
{
	DETECT_INFO_TYPE_ONLY_UBG 		= 0,
	DETECT_INFO_TYPE_ONLY_PBS 		= 1,
	DETECT_INFO_TYPE_BOTH	 		= 2
}DETECTINFOTYPE;


/**
@brief   조향 H/W Define
@author  임태웅
@date    2013.08.05
*/
typedef enum STEER_HW_INFO_POSITION_
{
	STEER_HW_INFO_POSITION_NONE		= 0,
	STEER_HW_INFO_POSITION_LEFT		= 1,
	STEER_HW_INFO_POSITION_RIGHT	= 2,
	STEER_HW_INFO_POSITION_BOTH		= 3
}STEER_HW_INFO_POSITION;

/**
@brief   조향 H/W 명령 Define
@author  임태웅
@date    2013.08.05
*/
typedef enum STEER_HW_INFO_CMD_
{
	STEER_HW_INFO_CMD_LEFT	= 0,
	STEER_HW_INFO_CMD_RIGHT	= 1
}STEER_HW_INFO_CMD;

//1	↓추후  PATH_STEERING_INFO_로 통합 예정 
/**
@brief   조향 Define
@author  임태웅
@date    2012.11.12
*/
typedef enum STEERING_INFO_
{
	STEERING_NONE			= 0,	///< Steering 방향 없음
	STEERING_LEFT			= 1,	///< Left Steering
	STEERING_RIGHT			= 2,	///< Right Steering
	STEERING_N_BRANCH_L2R	= 3,	///< N분기 Left to Right
	STEERING_N_BRANCH_R2L	= 4,	///< N분기 Right to Left
	STEERING_MOVING			= 5		///< Steering 이동 중
}STEERING_INFO;

/**
@brief   Path용 조향 Define
@author  임태웅
@date    2012.11.12
*/
typedef enum PATH_STEERING_INFO_
{
	PATH_STEERING_NONE			= STEERING_NONE,			///< Steering 방향 없음
	PATH_STEERING_LEFT			= STEERING_LEFT,			///< Left Steering
	PATH_STEERING_RIGHT			= STEERING_RIGHT,			///< Right Steering
	PATH_STEERING_N_BRANCH_L2R	= STEERING_N_BRANCH_L2R,	///< N분기 Left to Right
	PATH_STEERING_N_BRANCH_R2L	= STEERING_N_BRANCH_R2L,	///< N분기 Right to Left
}PATH_STEERING_INFO;

///**
//@brief   Path용 분기 Define
//@author  박태욱
//@date    2016.11.09
//*/
//typedef enum PATH_DIV_INFO_
//{
//	PATH_STEERING_NONE			= 0,			///< Steering 방향 없음
//	PATH_STEERING_LEFT			= 1,			///< Left Steering
//	PATH_STEERING_RIGHT			= 2,			///< Right Steering
//}PATH_DIV_INFO;

/**
@brief   Path용 방향 Define
@author  임태웅
@date    2012.11.12

  대차, 장애물 패턴 정보로 사용 중...

 */
typedef enum PATH_DIRECTION_INFO_
{
	PATH_DIRECTION_NO_USE	= 0,	///< 미사용: 예전 없음
	PATH_DIRECTION_NORMAL	= 1,	///< 직진 구간: 예전 N
	PATH_DIRECTION_LEFT		= 2,	///< 왼쪽: 예전 L
	PATH_DIRECTION_RIGHT	= 3,	///< 오른쪽: 예전 R
	PATH_DIRECTION_SHORT	= 4,	///< N분기 사용: 예전 S
	PATH_DIRECTION_BOTH		= 5,	///< 짧은 S자형 곡선: 예전 B
	PATH_DIRECTION_NARROW	= 6,		///< 레일 간격이 900mm 이하일때의 짧은 S자형 곡선: 예전 V
	PATH_DIRECTION_HILL		= 7,		///< Slope 구간: 예전 V
	PATH_DIRECTION_SPECIAL_L = 11,		///< 특수 분기(N분기)
	PATH_DIRECTION_SPECIAL_R = 12,		///< 특수 분기(N분기)
	PATH_DIRECTION_SPECIAL_C_L = 13,		///< 특수 분기(4채널 곡선)
	PATH_DIRECTION_SPECIAL_C_R = 14,		///< 특수 분기(4채널 곡선)
	//validation 추가로 실제 추가시 번호 순서대로 기입해야 함.
}PATH_DIRECTION_INFO;

/**
@brief   Path용 방향 Define
@author  임태웅
@date    2012.11.12

  대차, 장애물 패턴 정보로 사용 중...

 */
typedef enum PATH_OBS_DIRECTION_INFO_
{
	PATH_OBS_DIRECTION_NO_USE	= 0,	///< 미사용: 예전 없음
	PATH_OBS_DIRECTION_NORMAL	= 1,	///< 직진 구간: 예전 N
	PATH_OBS_DIRECTION_LEFT		= 2,	///< 왼쪽: 예전 L
	PATH_OBS_DIRECTION_RIGHT	= 3,	///< 오른쪽: 예전 R
	PATH_OBS_DIRECTION_SHORT	= 4,	///< N분기 사용: 예전 S
	PATH_OBS_DIRECTION_BOTH		= 5,	///< 짧은 S자형 곡선: 예전 B
	PATH_OBS_DIRECTION_NARROW	= 6,		///< 레일 간격이 900mm 이하일때의 짧은 S자형 곡선: 예전 V
	PATH_OBS_DIRECTION_STB_L 	= 7,		///< 특수 분기(N분기)
	PATH_OBS_DIRECTION_STB_R 	= 8,		///< 특수 분기(N분기)
	PATH_OBS_DIRECTION_NBreanch_L = 9,		///< 특수 분기(4채널 곡선)
	PATH_OBS_DIRECTION_NBreanch_R = 10,		///< 특수 분기(4채널 곡선)
	PATH_OBS_DIRECTION_UTrun_L = 11,		///< 특수 분기(4채널 곡선)
	PATH_OBS_DIRECTION_UTrun_R = 12,		///< 특수 분기(4채널 곡선)
	PATH_OBS_DIRECTION_Bay = 13,		///< 특수 분기(4채널 곡선)
	//validation 추가로 실제 추가시 번호 순서대로 기입해야 함.
}PATH_OBS_DIRECTION_INFO;

/**
@brief   Path용 분기 방향 Define
@author  박태욱
@date    2016.11.12
*/
typedef enum PATH_DIVERGENCE_INFO_
{
	PATH_DIVERGENCE_NONE	= 0,   	///< 분기 없음
	PATH_DIVERGENCE_LEFT	= 1,	///< 왼쪽분기
	PATH_DIVERGENCE_RIGHT	= 2		///< 오른쪽분기

}PATH_DIVERGENCE_INFO;

///**
//@brief   Path용 경사로  Define
//@author  임태웅
//@date    2012.11.12
//*/
//typedef enum _SLOPE_NODE_INFO_
//{
//	NODE_INIT		= 0,	///< 초기값
//	NODE_NORMAL		= 1,   	///< 일반 노드
//	NODE_SLOPE		= 2   	///< 슬로프 노드
//
//}SLOPE_NODE_INFO;



/**
@brief   전후 Path의 상태에 따른 Node의 Type
@author  puting
@date    2015.11.09
*/
typedef enum PASSPERMIT_NODE_TYPE_
{
	NORMAL_NTYPE		= 1,		///< NORMAL_POINT = 일반
	STOP_NTYPE			= 2,		///< 정지 Node
	RESET_NTYPE			= 3,		///< RESET Node
	COMMON_NTYPE		= 4		///< 공용 Node
}PASSPERMIT_NODE_TYPE;

/**
@brief   전후 Path의 상태에 따른 Node의 Type
@author  puting
@date    2015.11.09
*/
typedef enum DEVICE_NODE_TYPE_
{
/*
	NONE_TYPE		= 0,		///< None = 일반
	LIFTIN_NTYPE 	= 1,		///< lift In Node
	LIFTOUT_NTYPE  	= 2,		///< Lift Out Node
	MTLIN_NTYPE		= 3,		///< MTL In Node
	MTL_NTYPE   	= 4,		///< MTL 내부 Node
	MTLOUT_NTYPE   	= 5,		///< MTL Out Node
	SLOPE_NTYPE   	= 6			///< Slope
*/

	NONE_TYPE			= 0x00,		///< None = 일반
	SLOPE_NTYPE   		= 0x01,	 	///< Slope

	// for change node (jhoun.lim)
	OPT_TAG				= 0x05,		///< Optional Node (Tag-type)
	OPT_DISTANCE		= 0x06,		///< Optional Node (Distance-type)
	OPT_COMBO			= 0x07,		///< Optional Node (Combo-type)

	LIFTPRE_NTYPE_LEFT 	= 0x10,		///< lift In Node
	LIFTPRE_NTYPE_RIGHT	= 0x11,		///< lift In Node
	LIFTIN_NTYPE_LEFT 	= 0x12,		///< lift In Node
	LIFTIN_NTYPE_RIGHT 	= 0x13,		///< lift In Node
	LIFTOUT_NTYPE  		= 0x14,		///< Lift Out Node

	MTL_SEL_ON_LEFT		= 0x20,		///< MTL In Node
	MTL_SEL_ON_RIGHT	= 0x21,		///< MTL In Node
	MTL_STOP_LEFT		= 0x22,		///< MTL In Node
	MTL_STOP_RIGTH		= 0x23,		///< MTL In Node

	INIT_NTYPE			= 0xFF,		///< None = 일반


}DEVICE_NODE_TYPE;



/**
@brief   전후 Path의 상태에 따른 Node의 Type
@author  임태웅
@date    2012.11.12
*/
typedef enum NODE_TYPE_
{
	NORMAL_POINT		= 0,		///< NORMAL_POINT = 일반
	BRANCH_POINT		= 1,		///< BRANCH_POINT = 분기
	MERGE_POINT			= 2,		///< MERGE_POINT = 합류
	MERGE_BRANCH_POINT	= 3			///< 합류, 분기
}NODE_TYPE;

/**
@brief   Mark하는 지점의 Type
@author  임태웅
@date    2012.11.12
*/
typedef enum MARK_TYPE_
{
	MARK_TYPE_NODE				= 0,		///< DRIVING_TAG = 주행 TAG
	MARK_TYPE_TAG_STATION		= 1,		///< TRANS_POINT_TAG = 이적재 TAG
	MARK_TYPE_QR_STATION_LEFT	= 2,		///< TRANS_POINT_QR_LEFT = 이적재 QR TAG Left
	MARK_TYPE_QR_STATION_RIGHT	= 3, 		///< TRANS_POINT_QR_RIGHT = 이적재 QR TAG Right
	MARK_TYPE_OFFSET  	 		= 4,
	MARK_TYPE_TAG_STATION_BACK	= 5			///< TRANS_POINT_TAG = 이적재 TAG
}MARK_TYPE;

/**
@brief   Mark하는 Sensor Maker Type
@author  puting
@date    2016.02.03
*/
typedef enum MAKER_TYPE_
{
	MAKER_TYPE_KEYENCE	   	= 0,	///< 키앤스사 센서
	MAKER_TYPE_OMRON 		= 1,		///< 옴론사 센서
	MAKER_TYPE_DM50 		= 2,		///< 코드닉스사 센서
	MAKER_TYPE_MS	 		= 3		///< 옴론사 센서

}MAKER_TYPE;

#endif