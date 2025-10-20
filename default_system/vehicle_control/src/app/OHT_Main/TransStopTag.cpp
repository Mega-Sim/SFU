//---------------------------------------------------------------------------
/**
@file    TransStopTag.cpp
@version OHT 7.0
@brief   TransStopTag Class CPP File
*/

#pragma hdrstop

#include "TransStopTag.h"

/**
@brief   TransStopTag Class 생성자
@date
@author
@param   pHWCommon : HWCommon 클래스 포인터
*/
TransStopTag::TransStopTag(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   TransStopTag Class 소멸자
@date
@author
*/
TransStopTag::~TransStopTag()
{
}

/**
@brief	 이적재 StopTag의 Front Sensor가 On 되어 있는지 체크하는 함수
@author	 LSJ
@date	 2012.10.22
@return	 이적재 Front Stop Sensor가 On이면 True, Off 면 False
*/
bool TransStopTag::IsOnFrontSensor()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.STOP_Trans_Front == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief	 이적재 StopTag의 Rear Sensor가 On 되어 있는지 체크하는 함수
@author	 LSJ
@date	 2012.10.22
@return	 이적재 Rear Stop Sensor가 On이면 True, Off 면 False
*/
bool TransStopTag::IsOnRearSensor()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.STOP_Trans_Rear == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief	 이적재 StopTag의 Front, Rear Sensor가 모두 On 되어 있는지 체크하는 함수
@author	 LSJ
@date	 2012.10.22
@return	 이적재 Front, Rear Stop Sensor가 모두 On이면 True, Off 면 False

bool TransStopTag::IsTransStopPosition()
{
	bool bResult = false;

	if(IsOnFrontSensor() && IsOnRearSensor())
		bResult = true;
	else
		bResult = false;

	return bResult;
}
*/
//---------------------------------------------------------------------------

#pragma package(smart_init)
