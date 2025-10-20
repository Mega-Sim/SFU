//---------------------------------------------------------------------------
/**
@file    VirtualDetect.h
@version OHT 7.0
@brief   Detect Class Header File
*/

#ifndef VirtualDetectH
#define VirtualDetectH

typedef enum DETECT_AREA_TYPE_
{
	DETECT_AREA_NO_USE	= 0,	// 미사용: 예전 없음
	DETECT_AREA_NORMAL	= 1,	// 직진 구간: 예전 N
	DETECT_AREA_LEFT	= 2,	// 왼쪽: 예전 L
	DETECT_AREA_RIGHT	= 3,	// 오른쪽: 예전 R
	DETECT_AREA_SHORT	= 4,	// N분기 사용: 예전 S
	DETECT_AREA_BOTH	= 5,	// 짧은 S자형 곡선: 예전 B
	DETECT_AREA_NARROW	= 6		// 레일 간격이 900mm 이하일때의 짧은 S자형 곡선: 예전 V
}DETECT_AREA_TYPE;

/**
@brief   AREA 설정 Define
@author  ehoto
@date    2012.11.12
*/
typedef enum DETECT_STATUS_
{
	DETECT_NONE			= 0,		///< 감지된 것 없음
	DETECT_AND_MOVE		= 1,		///< 감지되었고 그에 따른 이동 조정 필요
	DETECT_AND_STOP		= 2,		///< 감지되어 정지 필요
	DETECT_UNIT_FAIL	= 3 	///< 감지 장치 이상
//	DETECT_UNIT_STEPOUT	= 4		///< 감지 장치 탈조 
}DETECT_STATUS;

/**
@class   VirtualDetect
@date    2013.01.22
@author  임태웅
@brief   Detect Sensor 관련 Class
@note    Detect Sensor의 Area를 선택하고 감지하기 위한 함수 구현
*/
class VirtualDetect
{
private:

public:

	VirtualDetect(){;}
	~VirtualDetect(){;}

	virtual	int				Enable()						= 0;	// 활성화(전원 ON)
	virtual	int				Disable()						= 0;	// 비활성화(전원 OFF)

	virtual	DETECT_STATUS	GetStatus()						= 0;	// 상태를 받아오는 함수
	virtual	void			SetArea(DETECT_AREA_TYPE Type)	= 0;	// 영역을 설정하는 함수
	virtual	double			GetMovableDist()				= 0;	// 이동 가능한 거리를 받아오는 함수
};

#endif  //VirtualDetectH
