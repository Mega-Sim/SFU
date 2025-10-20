/**
@file    STBAccess.cpp
@version OHT 7.0
@brief   STBAccess Class CPP File
*/

#include "STBAccess.h"

/**
@brief   STBAccess Class 생성자
@author	 LSJ
@date	 2012.10.27
@param   pHWCommon : HWCommon Class의 포인터
*/
STBAccess::STBAccess(HWCommon * pHWCommon)
{
    m_pHWCommon = pHWCommon;
}

/**
@brief   STBAccess Class 소멸자
@author	 LSJ
@date	 2012.10.27
*/
STBAccess::~STBAccess()
{

}

/**
@brief   Left STB 작업 전 Left STB OHT Detect Sensor가 On/Off 인지 확인하는 함수.
@author	 LSJ
@date	 2012.10.27
@return  Left STB OHT Detect Sensor가 On인 경우 True, Off인 경우 False를 Return
*/
bool STBAccess::IsSTBLOHTDetect()
{
    bool bResult = false;

    if (m_pHWCommon->m_Input.STB_OHT_Detect_Left == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief   Right STB 작업 전 Left STB OHT Detect Sensor가 On/Off 인지 확인하는 함수.
@author	 LSJ
@date	 2012.10.27
@return  Right STB OHT Detect Sensor가 On인 경우 True, Off인 경우 False를 Return
*/
bool STBAccess::IsSTBROHTDetect()
{
    bool bResult = false;

    if (m_pHWCommon->m_Input.STB_OHT_Detect_Right == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief   Left STB에 작업 할 때 켜 놓은 발광부 센서가 On 되어 있는지 확인하는 함수
@author	 LSJ
@date	 2012.10.27
@return	 Left STB Work 출력 값이 On인 경우 True, Off인 경우 False를 Return
*/
bool STBAccess::IsSTBLWorkOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Output.STB_Left_Work == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief	 Right STB에 작업 할 때 켜 놓은 발광부 센서가 On 되어 있는지 확인하는 함수
@author	 LSJ
@date	 2012.10.27
@return  Right STB Work 출력 값이 On인 경우 True, Off인 경우 False를 Return
*/
bool STBAccess::IsSTBRWorkOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Output.STB_Right_Work == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief	 Left STB 이적재 작업 시 발광부 센서를 On 시키는 함수
@author	 LSJ
@date	 2012.10.29
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int STBAccess::STBLWorkOn()
{
    int nError = NO_ERR;

    nError = m_pHWCommon->SetBit(OUT_STB_L_WORK, ON);

    return nError;
}

/**
@brief	 Right STB 이적재 작업 시 발광부 센서를 On 시키는 함수
@author	 LSJ
@date	 2012.10.29
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int STBAccess::STBRWorkOn()
{
    int nError = NO_ERR;

    nError = m_pHWCommon->SetBit(OUT_STB_R_WORK, ON);

    return nError;
}

/**
@brief	 Left STB 이적재 작업이 끝난 후 발광부 센서를 Off 시키는 함수
@author	 LSJ
@date	 2012.10.29
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int STBAccess::STBLWorkOff()
{
    int nError = NO_ERR;

    if (IsSTBLWorkOn() == true)
        nError = m_pHWCommon->SetBit(OUT_STB_L_WORK, OFF);

    return nError;
}

/**
@brief	 Left STB 이적재 작업이 끝난 후 발광부 센서를 Off 시키는 함수
@author	 LSJ
@date	 2012.10.29
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int STBAccess::STBRWorkOff()
{
    int nError = NO_ERR;

    if (IsSTBRWorkOn() == true)
        nError = m_pHWCommon->SetBit(OUT_STB_R_WORK, OFF);

    return nError;
}
