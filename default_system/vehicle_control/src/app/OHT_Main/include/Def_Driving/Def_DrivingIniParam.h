#ifndef Def_DrivingIniParamH
#define Def_DrivingIniParamH

#include <stdio.h>
#include "AxisParam.h"		// PROPOTINAL_AXIS_PARAM
#include "Def_PathInfo.h"	// NODE_TYPE, PATH_STEERING_INFO, PATH_DIRECTION_INFO, MARK_TYPE
#include "Def_MarkParam.h"	// MARK_DIST_PARAM, MARK_SPEED_PARAM, MARK_ACCEL_PARAM

/**
@brief	 주행 Mark 파라미터
@author  ehoto
@date	 2013.06.23
*/
typedef struct DRIVING_MARK_PARAM_
{
	MARK_TYPE			Type;	///< Mark 타입: 최종 도착지의 Type ( MARK_TYPE 0: MARK_TYPE_NODE (Node) / 1: MARK_TYPE_TAG_STATION (TagStation) / 2: MARK_TYPE_QR_STATION_LEFT (QR Left) / 3: MARK_TYPE_QR_STATION_RIGHT (QR Right) )
	MAKER_TYPE          MakerType;  ///< QR Sensor Maker Type
	MARK_DIST_PARAM		Dist;	///< 거리
	MARK_SPEED_PARAM	Speed;	///< 속도
	MARK_ACCEL_PARAM	Accel;	///< 가감속
}DRIVING_MARK_PARAM;

/**
@brief	 주행 Steer 파라미터
@author  ehoto
@date	 2013.06.23
*/
typedef struct DRIVING_STEER_PARAM_
{
	DWORD	dwChangeTime;			///< 조향이 변경되는데 필요한 시간
	DWORD	dwMovingTorqueTime;		///< MovingTorque 유지 시간
	DWORD	dwCompleteCheckTime;	///< 조향 변경이 완료 후 채터링 시간
	DWORD	dwCompleteLimitTime;	///< 조향 변경 시간 Limit
}DRIVING_STEER_PARAM;

/**
@brief	 주행 Link 파라미터
@author  ehoto
@date	 2013.06.23
*/
typedef struct DRIVING_LINK_PARAM_
{
	double	dOutNodeAreaMargin;				///< Link의 OutNode 근접 기준 마진
	double	dNBranchMiddleAreaInMargin;		///< N분기 중간부 진입 기준 마진		
}DRIVING_LINK_PARAM;

/**
@brief	 주행 전방감지 파라미터
@author  ehoto
@date	 2013.06.23
*/
typedef struct DRIVING_FRONT_DETECT_PARAM_
{
	DWORD	dwDetectFailChatteringTime;	///< Detect Fail Error Chattering 시간
	DWORD	dwObsFailChatteringTime;	///< OBS Fail Error Chattering 시간
	DWORD	dwObsTimeout;				///< OBS 감지 시간 초과 기준
	DWORD	dwObsChangeTime;				///< OBS 감지 시간 초과 기준
	double	dObsStraightDetectMargin;	///< OBS 직진 구간 감지 시 조금 더 보는 마진
}DRIVING_FRONT_DETECT_PARAM;

/**
@brief	 UBG 신호 헌팅 (Fail 3) 에 의한 추돌 사전 진단 및 Error 처리를 위한 파라미터
@see	 AMOHTV80F-1756
*/
typedef struct DRIVING_UBG_FAIL3_ERROR_PARAM_
{
	bool bHourlyCheck;	///< 매시간 사전 진단 및 Error 처리 여부
	bool bWeeklyCheck;	///< 매시간사전 진단 및 Error 처리 여부
	int nHourCount;		///< 매시간 발생한 `UBG FAIL 3` Error 기준
	int nWeekCount;		///< 매주 발생한 `UBG FAIL 3` Error 기준
}DRIVING_UBG_FAIL3_ERROR_PARAM;

/**
@brief	 주행 Link 파라미터
@author  ehoto
@date	 2013.06.23
*/
typedef struct DRIVING_FOUPCOVER_PARAM_
{
	UINT	nLineFoupChattering;		///< Link의 OutNode 근접 기준 마진
	UINT	nCurveFoupChattering;		///< 분기 중간부 진입 기준 마진
	UINT	nSPCurveFoupChattering;		///< 특수 분기 중간부 진입 기준 마진
}DRIVING_FOUPCOVER_PARAM;

/**
@brief	 주행 전방감지 파라미터
@author  ehoto
@date	 2013.06.23
*/
struct DRIVING_PARAM_SET
{
	PROPOTINAL_AXIS_PARAM		    Axis;			///< 주행 축 기준 정보
	
	int 						    nMarkTypeCount;	///< Mark 가지 수
	DRIVING_MARK_PARAM			    Mark[4];		///< Mark 파라미터: [0]배열이 주행 기준
	
	DRIVING_STEER_PARAM			    Steer;			///< 조향 기준 정보
	DRIVING_LINK_PARAM			    Link;			///< Path 기준 정보
	DRIVING_FRONT_DETECT_PARAM	    FrontDetect;	///< 전방 감지
	DRIVING_UBG_FAIL3_ERROR_PARAM   UbgFail3Err;	////< UBG 신호 헌팅 (UBG Fail 3) 에 의한 추돌 사전 진단

	DRIVING_FOUPCOVER_PARAM         FoupCover;      //Foup Cover 채터링

	int CID_PAUSE_TIMEOVER; 					///< 전원Off시 CID PAUSE TIMEOVER
	double StationMarkInPos;					//이적재 Tag 정위치 마크이후 Inpos 범위
	int MapMakerCount_Driving;					//Mark시 MapMaker 반사판인식후 확인 거리값
	int MapMakerCount_Trans;					//Mark시 MapMaker 반사판인식후 확인 거리값

	double MapMakerMargin_Gap;					//Mark시 MapMaker 반사판인식후 확인 거리값
	bool  bVHLLifterDir;

	double ScatteredReflection;					//Station Mark 시 난반사관련 인터락 거리
	double ScatteredReflectionMax;					//Station Mark 시 난반사관련 인터락 거리

	int CIDWaitingTimeout; //작업변경시 CID Occup 반납 처리 Count

	bool CID_Fiber_Monitoring;
	int	 CID_SERIAL_CMD_DELAY_TURN;
	int LightGuide_TIME_OUT_MS_MTL;
	int LightGuide_TIME_OUT_MS_VHLLifter;
	int LightGuide_TIME_OUT_MS_AutoDoor;

	char	Version[8];

};



#endif
