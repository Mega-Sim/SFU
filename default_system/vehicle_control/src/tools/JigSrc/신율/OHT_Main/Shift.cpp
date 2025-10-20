/**
@file    Shift.cpp
@version OHT 7.0
@brief   Shift Class CPP File
*/

#include "Shift.h"


/**
@brief   Shift Class 생성자
@author  //
@date    //
*/
Shift::Shift(HWCommon *pHWCommon, int nAxisNum):PropotionalMotionAxis(pHWCommon, nAxisNum, true, 1.0)	//MotionAxis
{
	m_pHWCommon = pHWCommon;
}													

/**
@brief   Shift Class 생성자
@author  //
@date    //
*/
Shift::Shift(HWCommon *pHWCommon, int nAxisNum, double dGearRatio):PropotionalMotionAxis(pHWCommon, nAxisNum, true, dGearRatio)	//MotionAxis
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   Shift Class 소멸자
@author  //
@date    //
*/
Shift::~Shift()
{

}

/**
@brief   // 입력
@date    // 입력
@author  // 입력
@param   // 입력(없을 경우 삭제)
@return  // 입력(없을 경우 삭제)
*/
bool Shift::IsShiftHome()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.SHIFT_Home == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

