#ifndef Def_DrivingHWInfoH
#define Def_DrivingHWInfoH

#include <stdio.h>
#include "Def_Variables.h"		// BYTE, WORD, UINT, UINT64

typedef enum DRIVING_IO_STEER_DIRECTION_CMD_
{
	DRIVING_IO_STEER_DIRECTION_CMD_LEFT	=0,
	DRIVING_IO_STEER_DIRECTION_CMD_RIGHT
}DRIVING_IO_STEER_DIRECTION_CMD;

typedef enum DRIVING_IO_STEER_TORQUE_CMD_
{
	DRIVING_IO_STEER_TORQUE_CMD_FREE		=0,
	DRIVING_IO_STEER_TORQUE_CMD_STOP		,
	DRIVING_IO_STEER_TORQUE_CMD_MOVING		,
	DRIVING_IO_STEER_TORQUE_CMD_TRACTION	,
}DRIVING_IO_STEER_TORQUE_CMD;

/**
@brief	 주행 부 H/W I/O 정보
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_IO_
{
	UINT	BrakeRelease			: 1;	///< 브레이크 풀렸으면 true

	UINT	Axis1AmpOn          	: 1;	///< Motor Enable 되어 있으면 true
	UINT	Axis1Running        	: 1;	///< 구동중이면 true
	UINT	Axis1InPosition     	: 1;	///< 명령 위치에 도착하면 true
	UINT	Axis1AxisDone       	: 1;	///< 구동 명령 완료면 true

	UINT	Axis2AmpOn          	: 1;	///< Motor Enable 되어 있으면 true
	UINT	Axis2Running        	: 1;	///< 구동중이면 true
	UINT	Axis2InPosition     	: 1;	///< 명령 위치에 도착하면 true
	UINT	Axis2AxisDone       	: 1;	///< 구동 명령 완료면 true

	UINT	NodePreCheckOn      	: 1;	///< Node 사전 감지 센서 On되면 true
	UINT	NodeOn              	: 1;	///< Node 감지 센서 On되면 true
	UINT	TagStationFrontOn   	: 1;	///< Front 감지되면 true
	UINT	TagStationRearOn    	: 1;	///< Rear 감지되면 true

	UINT	ObsStatus           	: 2;	///< 0:미감지 / 1:감지 저속 / 2:감지 정지 / 3:이상 (DETECT_STATUS)
	UINT	DetectStatus        	: 2;	///< 0:미감지 / 1:감지 저속 / 2:감지 정지 / 3:이상 (DETECT_STATUS)

	UINT	SteerFrontLeftInOn      : 1;	///< 조향 Input 왼쪽앞 감지
	UINT	SteerFrontRightInOn     : 1;	///< 조향 Input 오른쪽앞 감지
	UINT	SteerRearLeftInOn       : 1;	///< 조향 Input 왼쪽뒤 감지
	UINT	SteerRearRightInOn      : 1;	///< 조향 Input 오른쪽뒤 감지
	UINT	SteerFrontTorque        : 2;	///< 0:Free / 1:Stop / 2:Moving / 3:견인
	UINT	SteerFrontDirCmd        : 1;	///< 조향 명령 ( 0:왼쪽 / 1:오른쪽 )
	UINT	SteerRearTorque         : 2;	///< 0:Free / 1:Stop / 2:Moving / 3:견인
	UINT	SteerRearDirCmd         : 1;	///< 조향 명령 ( 0:왼쪽 / 1:오른쪽 )
	
	UINT	Reserved				: 5;	///< 예비
}DRIVING_IO;



/**
@brief	 주행 명령
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_AXIS_
{
	double	dCurrPos;			///< 전륜 현재 위치
	double  dFinalPos;           ///< 전륜 목표 위치
	double  dSpeed;              ///< 전륜 속도
	double  dFollowError;        ///< 전륜 Following Error
	double  dSmallAddCheckPos;   ///< 전륜 소량 이동 명령 값
	int     nError;              ///< 전륜 에러
}DRIVING_AXIS;



/**
@brief	 Node 정보
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_NODE_
{
	int		nNodeCount;			///< Node 발견 시점의 누적 노드 개수 (Rising Edge 검출)
	double  dNodePosition;       ///< Node 발견 시점의 엔코더 값
	int     nPreCheckCount;      ///< Node 사전 발견 시점의 누적 노드 개수 (Rising Edge 검출)
	double  dPreCheckPosition;   ///< Node 사전 발견 시점의 엔코더 값
	int     nBcrId;             	///< Node ID 읽힌 값
}DRIVING_NODE;


/**
@brief	 Station 정보
@author  ehoto
@date	 2013.06.19
*/
typedef struct TAG_STATION_
{
	double	dFrontPosition;	///< Tag Front 발견 시점의 엔코더 값 (Rising Edge 검출)
	double  dRearPosition;   ///< Tag Rear 발견 시점의 엔코더 값 (Rising Edge 검출)
	int     nBcrId;        	///< Tag ID 읽힌 값
}TAG_STATION;


/**
@brief	 Station 정보
@author  ehoto
@date	 2013.06.19
*/
typedef struct QR_STATION_
{
	int		nQrId;		///< QR Left ID
	double  dQrOffset;   ///< QR Left ID 발견 시 Offset
}QR_STATION;

/**
@brief	 주행 명령
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_HW_INFO_SET_
{
	DRIVING_IO		IO;
	DRIVING_AXIS	Axis1;
	DRIVING_AXIS	Axis2;
	DRIVING_NODE	Node;
	TAG_STATION		TagStaion;
	QR_STATION		QrStaionLeft;
	QR_STATION		QrStaionRight;
}DRIVING_HW_INFO_SET;


#endif
