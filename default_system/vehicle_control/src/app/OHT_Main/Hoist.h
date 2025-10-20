/**
@file    Hoist.h
@version OHT 7.0
@brief   Hoist Class Header File
*/

#ifndef HoistH
#define HoistH

#include "HWCommon.h"				// HW 정보 사용
#include "MotionAxis.h"				// Motion축 상속 
#include "HoistDistanceConverter.h"	// 단위 표준화: 3차 방벙식 근사 
#include "AxisParam.h"				// 3차 방정식 기어비 축 파라미터 상용 목적 

//AOHC-286
#if(OHT_NAME == OHT_NAME_STD_V85)      //임시
#define HOIST_SMALL_ADD_HOISTHOME    	76
#define HOIST_SMALL_ADD_HOISTHOME_DISTANCE	6.0
#elif((OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))  //V86 여기가 맞는지 확인 필
#define HOIST_SMALL_ADD_HOISTHOME    	73
#define HOIST_SMALL_ADD_HOISTHOME_DISTANCE	6.0
#else
#define HOIST_SMALL_ADD_HOISTHOME		74
#define HOIST_SMALL_ADD_HOISTHOME_DISTANCE	6.0
#endif

/**
@class   Hoist
@date    2012.10.17
@author  임태웅
@brief   Hoist Motor 관련 Class
*/
class Hoist : public MotionAxis
{
//1	변수(Member)
private:
	HWCommon *m_pHWCommon;									///< HW정보를 사용
	HoistDistanceConverter *m_pHoistDistanceConverter;	///< 단위 표준화 : 3차 방정식으로 근사화
	int m_nAxisNum;											///< 축 정보
	//double m_dHomeOffsetOfRailToCarrierBottomOnUserUnit;	///< Hoist Home 위치 경우, Rail 아래에서 반송체 바닥까지의 거리
	THREE_LINEAR_EQUATION_GEAR_RATIO m_HoistParameter;

	bool bHoistOrigin;


//1	생성자/소멸자 (Constructor/Destructor)	
public:
    Hoist(HWCommon *pHWCommon, int nAxisNum);	///< 생성자
    ~Hoist();									///< 소멸자 


//1	함수(Method)
protected:
	double ConvertSpeedUnit(double dInput, bool bUserUnit);				///< 단위 표준화 : 속도 단위 변환
	double ConvertAccelerationUnit(double dUser, bool bUserUnit);	///< 단위 표준화 : 가속도 단위 변환


public:
	bool IsHoistHome();												///< IO 정보 : 홈 위치 확인
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)  || (OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86))
	bool IsHoistBeltDetect();
//	int HoistExternalBrakeReleaseOnOFF(bool);                //미사용
#endif
	bool EnableUnitConverter(void);							///< 단위 변화 활성화
    bool SetHoistParam(THREE_LINEAR_EQUATION_GEAR_RATIO HoistParam);	///< 단위 변환을 위한 파라미터 입력

	double GetRelativeDistanceFromSmalladd();
	bool   IsHoistOriginFromSmalladd();
	void   SetHoistOriginFromSmalladd(bool bOrigin);

	double GetHoistMaxUserPosition();
	double GetHoistMinUserPosition();

	double GetRelativeDistanceFromSmalladdCurrentPosition();
	double GetHoistPulseValue();
	int SetNegativeLimit4PulseMoving(double dMinCtsValue);

#if(OHT_NAME == OHT_NAME_P4_MASK)
	/// mgchoi 2022-08-19. P4 MaskOHT용 추가함수 ///
	bool IsHoistLowerHome();
	bool IsHoistEUVHome();
	bool IsHoistRSP200Home();
	bool IsHoistRSP150Home();
	////////////////////////////////////////////////
#endif


};

#endif  // HoistH

