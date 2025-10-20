//---------------------------------------------------------------------------
/**
@file    TransStopTag.h
@version OHT 7.0
@brief   TransStopTag Class Header File
*/

#ifndef TransStopTagH
#define TransStopTagH


#include "HWCommon.h"

/**
@class   TransStopTag
@date    2012.10.22
@author  LSJ
@brief   TransStopTag 관련 Class
@note    TransStopTag 관련된 센서 상태를 확인하기 위한 함수 구현
*/
class TransStopTag
{
private:
	HWCommon *m_pHWCommon;

public:

	TransStopTag(HWCommon *pHWCommon);
	~TransStopTag();

	bool IsOnFrontSensor();
	bool IsOnRearSensor();
	bool IsTransStopPosition();

};
//---------------------------------------------------------------------------
#endif  //TransStopTagH
