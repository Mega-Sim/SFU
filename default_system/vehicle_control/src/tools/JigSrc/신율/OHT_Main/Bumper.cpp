//---------------------------------------------------------------------------
/**
@file    Bumper.cpp
@version OHT 7.0
@brief   Bumper Class 소스 File
*/

#include <windows.h>
#include "Bumper.h"


/**
@brief   Bumper Class 생성자
@author  zzang9un
@date    2012.10.19
@param   pHWCommon : HWCommon 클래스 포인터
*/
Bumper::Bumper(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   Bumper Class 소멸자
@author  zzang9un
@date    2012.10.19
*/
Bumper::~Bumper()
{

}

/**
@brief	 Bumper Sensor가 On 되어 있는지 체크하는 함수
@author	 LSJ
@date	 2012.10.23
@return	 Bumper가 Press되어 있으면 True, 아니면 False 리턴.
*/
bool Bumper::IsBumperPress()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.BUMPER_Press == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief	 Bumper Press로 인해 Latch 된 신호를 풀어주기 위해 Safety Clear 신호를 출력 시키는 함수
@author	 LSJ
@date	 2012.10.23
@return	 SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
*/
int Bumper::ResetBumperPress()
{
   int nError = NO_ERR;

   nError = m_pHWCommon->SetBit(OUT_BUMPER_CLEAR, ON);

   Sleep(200);

   if(nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_BUMPER_CLEAR, OFF);

   return nError;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)

