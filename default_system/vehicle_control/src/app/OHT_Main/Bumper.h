/**
@file    Bumper.h
@version OHT 7.0
@brief   Bumper Class Header File
*/

#ifndef BumperH
#define BumperH

#include "HWCommon.h"

/**
@class   Bumper
@date    2012.10.15
@author  zzang9un
@brief   Bumper 관련 Class
*/
class Bumper
{
//friend 하드웨어 쓰레드 클래스
private:
	HWCommon *m_pHWCommon;

public:
    Bumper(HWCommon *pHWCommon);
	~Bumper();

	bool IsBumperPress();
	int ResetBumperPress();
	int SetBitBit(int index, bool OnOff);
};

#endif  //BumperH

