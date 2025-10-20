//---------------------------------------------------------------------------
/**
@file    PowerControl.cpp
@version OHT 7.0
@brief   PowerControl Class CPP File
*/

#pragma hdrstop

#include "PowerControl.h"

/**
@brief   PowerControl 생성자
@author  LSJ
@date    2012.10.13
@param   pHWCommon : HWCommon Class의 포인터
@note    Input, Ouput을 사용하기 위해 HWCommon의 Pointer를 가져옴
*/
PowerControl::PowerControl(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   PowerControl 소멸자
@author  LSJ
@date    2012.10.13
*/
PowerControl::~PowerControl()
{



}

/**
@brief	 SafetyController를 Reset 할 수 있는 함수
@author	 LSJ
@date	 2014.09.02
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int PowerControl::ResetController(void)
{
	int nError = 0;
	nError = m_pHWCommon->SetBit(OUT_SAFETY_RESET, ON);
	Sleep(1000);
	nError = m_pHWCommon->SetBit(OUT_SAFETY_RESET, OFF);

	return nError;
}


/**
@brief	 MC(Magnetic Contactor)를 On, Off 할 수 있는 함수
@author	 LSJ
@date	 2012.10.27
@param   MC를 On 시키려면 ON, Off 시키려면 OFF
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int PowerControl::MCControl(bool bOnOff)
{
	int nError = 0;
	if (bOnOff == ON)
		nError = m_pHWCommon->SetBit(OUT_SAFETY_RESET, ON);
	else
		nError = m_pHWCommon->SetBit(OUT_SAFETY_RESET, OFF);

	return nError;
}

/**
@brief   EMO가 눌린 상태인지 풀린(정상) 상태인지 확인하는 함수
@author	 LSJ
@date	 2012.10.27
@return	 EMO가 눌린 상태이면 true, 풀린 상태이면 false 리턴.
*/
bool PowerControl::IsOnEMO()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.EMO_Press == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   SMPS Short 신호가 ON인지 확인하는 함수
@author	 JSH
@date	 2014.11.13
@return	 EMO가 눌린 상태이면 true, 풀린 상태이면 false 리턴.
*/
bool PowerControl::IsOnSMPSFault()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.SMPS_Short == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
