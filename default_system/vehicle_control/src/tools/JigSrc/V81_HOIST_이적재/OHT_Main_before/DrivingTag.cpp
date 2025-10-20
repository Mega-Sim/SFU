/**
@file    DrivingTag.cpp
@version OHT 7.0
@brief   DrivingTag Class CPP File
*/


#include "DrivingTag.h"

/**
@brief   DrivingTag 생성자
@author  LSJ
@date    2012.10.27
@param   pHWCommon : HWCommon Class의 포인터
@note    Input을 사용하기 위해 HWSet의 Pointer를 가져옴
*/
DrivingTag::DrivingTag(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   DrivingTag 소멸자
@author  LSJ
@date    2012.10.22
*/

 DrivingTag::~DrivingTag()
{
}

/**
@brief   DrivingTag Front Sensor On/Off 유무를 체크하는 함수
@author  LSJ
@date    2012.10.27
@return  Driving Stop Sensor가 On인 경우 True, Off인 경우 False를 Return
*/

bool DrivingTag::IsOn()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.STOP_Driving_Front == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

bool DrivingTag::IsPreCheckOn()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.STOP_Driving_PreCheck== ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}