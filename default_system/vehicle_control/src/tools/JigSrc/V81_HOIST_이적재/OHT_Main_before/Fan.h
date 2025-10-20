/**
@file    Fan.h
@version OHT 7.0
@brief   Fan Class Header File
*/

#ifndef FanH
#define FanH

#include "HWCommon.h"

/**
@class   Fan
@date    2012.10.13
@author  zzang9un
@brief   Fan ฐüทร Class
*/
class Fan
{
private:
    HWCommon *m_pHWCommon;
	
public:
    Fan(HWCommon *pHWCommon);
    ~Fan();
    
	bool IsFanFail();
};

#endif  // FanH