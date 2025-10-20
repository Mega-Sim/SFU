		   /**
@file    Ffu.h
@version OHT 7.0
@brief   FFU Class Header File
*/

#ifndef FfuH
#define FfuH

#include "HWCommon.h"

/**
@class   FFU
@date    2015.10.27
@author
@brief   FFU ฐüทร Class
*/
class Ffu
{
private:
    HWCommon *m_pHWCommon;

public:
	Ffu(HWCommon *pHWCommon);
    ~Ffu();

	bool IsFfuHoistFail();
	bool IsFfuFrameFail();
};

#endif  // FanH
