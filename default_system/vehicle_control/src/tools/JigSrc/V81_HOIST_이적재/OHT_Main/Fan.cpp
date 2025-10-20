/**
@file    Fan.cpp
@version OHT 7.0
@brief   Fan Class CPP File
*/

#include "Fan.h"

/**
@brief	 Fan 생성자
@author  zzang9un
@date    2012.10.13
@param   pHWCommon : HWCommon Class의 포인터
@note    Input, Ouput을 사용하기 위해 HWSet의 Pointer를 가져옴
*/
Fan::Fan(HWCommon *pHWCommon)
{
    m_pHWCommon = pHWCommon;
}

/**
@brief   Fan 소멸자
@author  zzang9un
@date    2012.10.13
*/
Fan::~Fan()
{
}

/**
@brief   Fan의 고장 유무를 체크하는 함수
@author  zzang9un
@date    2012.10.13
@return  Fan이 고장인 경우 True, 정상인 경우 False를 Return
*/
bool Fan::IsFanFail()
{
    int nResult = false;

    if (m_pHWCommon->m_Input.FAN_Fail == ON)
        nResult = true;
    else
        nResult = false;

    return nResult;
}
