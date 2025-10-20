#ifndef AxisParamH
#define AxisParamH

#include <stdio.h>
#include "MinMaxCtrl.h"	//MIN_MAX_VALUE


/**
@brief	 비례 기어비 축 파라미터
@author  ehoto
@date	 2013.06.23
*/
struct PROPOTINAL_GEAR_RATIO
{
	double	dIdealGearRatio;		///< 기어비 ( Ideal 기구 수치 : cts/mm )
	double	dIdeal2RealRatio;		///< 변형 비율 ( 측정 값 / Ideal 값 )
	double	dDeviceDifference;		///< 타 호기와의 차이 =  기준면에서 의 값 - 기기의 값 ( 기기에는 티칭 값에서 이 값을 빼서 적용 : UserUnit )
	int		nHomeSearchCtsOffset;	///< 센서 기준점에서의 기구적 원점까지의 Offset (펄스)
	
	PROPOTINAL_GEAR_RATIO()
	{
		dIdealGearRatio = 1.0;
		dIdeal2RealRatio = 1.0;
        nHomeSearchCtsOffset = 0;
	};
};

struct PROPOTINAL_GEAR_ROTATE
{
	int	nDeviceDifference;			///< 타 호기와의 차이 =  기준면에서 의 값 - 기기의 값 ( 기기에는 티칭 값에서 이 값을 빼서 적용 : UserUnit )
	int	nDeviceDifference_90;		////< 타 호기와의 차이 =  기준면에서 의 값 - 기기의 값 ( 기기에는 티칭 값에서 이 값을 빼서 적용 : UserUnit )
	int	nDeviceDifference_180;		///< 타 호기와의 차이 =  기준면에서 의 값 - 기기의 값 ( 기기에는 티칭 값에서 이 값을 빼서 적용 : UserUnit )
	PROPOTINAL_GEAR_ROTATE()
	{
		nDeviceDifference = 0;
		nDeviceDifference_90 = 0;
		nDeviceDifference_180 = 0;
	};
};

/**
@brief	위치 설정 구조체
@date    2013.07.28
@author  임태웅
*/
struct POSITION_VALUE
{
	double	dPulse;		///< Pulse 거리 단위
	double	dUserUnit;	///< 사용자 거리 단위
};

/**
@brief	 3차 방정식 기어비 축 파라미터
@author  ehoto
@date	 2013.07.28
*/
struct THREE_LINEAR_EQUATION_GEAR_RATIO
{
	POSITION_VALUE	PositionValue[10];		///< 위치 값 ( Pulse / 사용자 단위 ) 4개
	double			dDeviceDifference;		///< 타 호기와의 차이 =  기준면에서 의 값 - 기기의 값 ( 기기에는 티칭 값에서 이 값을 빼서 적용 : UserUnit )
	int				nHomeSearchCtsOffset;	///< 센서 기준점에서의 기구적 원점까지의 Offset (펄스)
};

/**
@brief	 축 Limit 파라미터
@author  ehoto
@date	 2013.06.23
*/
struct AXIS_LIMIT_PARAM
{
	MIN_MAX_VALUE<double>	Accel;
	MIN_MAX_VALUE<double>	Decel;
	MIN_MAX_VALUE<double>	Speed;
	double					dAbnormalMoveSpeedMargin;
	MIN_MAX_VALUE<double>	Position;
	MIN_MAX_VALUE<double>	JogPosition;
};

/**
@brief	 축 Default 파라미터
@author  ehoto
@date	 2013.06.23
*/
struct AXIS_DEFULT_PARAM
{
	double	dDefaultAccel;		///< 기본 가속도
	double	dDefaultDecel;		///< 기본 감속도
	double	dEmergencyDecel;	///< 긴급 감속도
};


/**
@brief	 주행 축 파라미터
@author  ehoto
@date	 2013.06.23
*/
struct PROPOTINAL_AXIS_PARAM
{
	PROPOTINAL_GEAR_RATIO	GearRatio;
	AXIS_LIMIT_PARAM		Limit;
	AXIS_DEFULT_PARAM		Default;
};


/**
@brief	 주행 축 파라미터
@author  ehoto
@date	 2013.07.08
*/
struct THREE_LINEAR_EQUATION_AXIS_PARAM
{
	THREE_LINEAR_EQUATION_GEAR_RATIO	GearRatio;
	AXIS_LIMIT_PARAM					Limit;
	AXIS_DEFULT_PARAM					Default;
};

#endif
