/**
@file    Shift.h
@version OHT 7.0
@brief   Shift Class Header File
*/

#ifndef ShiftH
#define ShiftH

#include "HWCommon.h"			// HW 정보 사용
#include "PropotionalMotionAxis.h"	// 비례 제어 축 상속 


/**
@class   Shift
@date    2012.10.17
@author  임태웅
@brief   Shift Motor 관련 Class
*/
class Shift : public PropotionalMotionAxis	//MotionAxis
{
private:
	HWCommon *m_pHWCommon;		///< HW 의 정보를 사용함

public:
	Shift(HWCommon *pHWCommon, int nAxisNum);						///< 생성자, 기어비 없음 
	Shift(HWCommon *pHWCommon, int nAxisNum, double dGearRatio);	///< 생성자, 기어비 있음 
	~Shift();

	bool IsShiftHome();	///< IO 정보 : Home 센서 정보 확인

};

#endif  // ShiftH

