/**
@file    DrivingTag.h
@version OHT 7.0
@brief   DrivingTag Class Header File
*/

#ifndef DrivingTagH
#define DrivingTagH


#include "HWCommon.h"

/**
@class   DrivingTag
@date    2012.10.22
@author  LSJ
@brief   DrivingTag ฐüทร Class
*/
class DrivingTag
{

private:
	HWCommon *m_pHWCommon;

public:
	DrivingTag(HWCommon *pHWCommon);
	~DrivingTag();

	bool IsOn();
	bool IsPreCheckOn();

};


#endif //DrivingTagH
