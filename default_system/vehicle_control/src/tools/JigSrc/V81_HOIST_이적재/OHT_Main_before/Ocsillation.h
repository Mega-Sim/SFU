/**
@file    Ocsillation.h
@version OHT 7.0
@brief   Ocsillation Class Header File
*/

#ifndef OcsillationH
#define OcsillationH


#include "HWCommon.h"

/**
@class   Ocsillation
@date    2012.10.22
@author
@brief   Ocsillation ฐüทร Class
*/
class Ocsillation
{

private:

	HWCommon	*m_pHWCommon;
	//Input Oscillation;

public:
	Ocsillation(HWCommon *pHWCommon);
	~Ocsillation();

	bool IsDetect();

};


#endif //OcsillationH
