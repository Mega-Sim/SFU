/**
@file    Lamp.h
@version OHT 7.0
@brief   Lamp Class Header File
*/

#ifndef LampH
#define LampH

#include <ExtCtrls.hpp>
#include "HWCommon.h"

/**
@class   Lamp
@date
@author
@brief   Lamp ฐüทร Class
*/
class Lamp
{

private:
    HWCommon *m_pHWCommon;

public:
	Lamp(HWCommon *pHWCommon);
	~Lamp();

	int LampOn(int nLampStatus);
	int LampOff();
	int LampOff_Except_Red();
	int LampOff_Yellow();
	bool IsLampOn(int nColor);
	int LampControl(int nLampStatus);
	int LampYellowSteerOn(int nLampStatus);


};

#endif  //LampH

