/**
@file    FFU.cpp
@version MASK OHT
@brief   FFU Class CPP File
*/

#include "Ffu.h"
#if((OHT_NAME == OHT_NAME_P4_MASK) || (OHT_NAME == OHT_NAME_MASK_JIG))
/**
@brief	 FFU 생성자
@author
@date    2015.10.27
@param   pHWCommon : HWCommon Class의 포인터
@note    Input, Ouput을 사용하기 위해 HWSet의 Pointer를 가져옴
*/
Ffu::Ffu(HWCommon *pHWCommon)
{
    m_pHWCommon = pHWCommon;
}

/**
@brief   Ffu 소멸자
@author
@date    2015.10.27
*/
Ffu::~Ffu()
{
}

/**
@brief   P4 Mask OHT의 Hoist FFU의 고장을 체크하는 함수
@author  mgchoi
@date    2023.01.31
@return  FFU Hoist가 고장인 경우 True, 정상인 경우 False를 Return
*/
bool Ffu::IsFfuHoistFail()
{
	int nResult = false;

	if (m_pHWCommon->m_Input.FFU_Fail_Hoist == ON)
		nResult = true;
	else
		nResult = false;

	return nResult;
}

/**
@brief   P4 Mask OHT의 Frame FFU의 고장을 체크하는 함수
@author  mgchoi
@date    2023.01.31
@return  FFU Frame이 고장인 경우 True, 정상인 경우 False를 Return
*/
bool Ffu::IsFfuFrameFail()
{
	int nResult = false;
	if (m_pHWCommon->m_Input.FFU_Fail_Frame == ON)
		nResult = true;
	else
		nResult = false;

	return nResult;
}
#endif
