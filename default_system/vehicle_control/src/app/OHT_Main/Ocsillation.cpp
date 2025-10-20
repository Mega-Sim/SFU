/**
@file    Ocsillation.cpp
@version OHT 7.0
@brief   Ocsillation Class CPP File
*/


#include "Ocsillation.h"

/**
@brief   Ocsillation 생성자
@author	 LSJ
@date    2012.10.27
@param   pHWCommon : HWCommon Class의 포인터
*/
Ocsillation::Ocsillation(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   Ocsillation 소멸자
@author  LSJ
@date    2012.10.27
*/

 Ocsillation::~Ocsillation()
{
}

/**
@brief   Ocsillation의 감지 유무를 체크하는 함수
@author  LSJ
@date    2012.10.27
@return  Hand의 진동이 감지된 경우 True, 그렇지 않은 경우 false
*/

bool Ocsillation::IsDetect()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.OSCILLATION == ON)
		bResult = false;
	else
		bResult = true;

	return bResult;
}