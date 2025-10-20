/**
@file    PropotionalMotionAxis.h
@version OHT 7.0
@brief   PropotionalMotionAxis Class Header File
*/

#ifndef PropotionalMotionAxisH
#define PropotionalMotionAxisH

#include "PropotionalConverter.h"

#include "HWCommon.h"	// HW 정보 사용
#include "MotionAxis.h"	// Motion축 상속 
#include "Def_Driving.h"	// 정보 전달 구조체
#include "AxisParam.h"	// 기어비 축 파라미터 사용 목적 

/**
@class   PropotionalMotionAxis
@date    2012.10.17
@author  임태웅
@brief   PropotionalMotionAxis Motor 관련 Class
@note    MotionAxis Class 상속
*/
class PropotionalMotionAxis : public MotionAxis
{
//1	변수(Member)
protected:
	PropotionalConverter *m_pPropotionalConverter;		///< 축정보 단위 표준화 : 비례 제어 방식
	PROPOTINAL_GEAR_RATIO m_GearRatio;

//1	생성자/소멸자 (Constructor/Destructor)	
public:
	PropotionalMotionAxis(HWCommon *pHWCommon, int nAxisNum, bool bAbsoluteEncoder, double dGearRatio);	///< 생성자
	virtual ~PropotionalMotionAxis();	///< 소멸자 
	

//1	함수(Method)
public:
	bool	SetGearRatio(PROPOTINAL_GEAR_RATIO GearRatio);	///< 기어비 설정
	PROPOTINAL_GEAR_RATIO	GetGearRatio();									///< 기어비 확인

	bool	EnableUnitConverter(void);			///< 기어비 사용하도록

private:

};

#endif  //PropotionalMotionAxisH

