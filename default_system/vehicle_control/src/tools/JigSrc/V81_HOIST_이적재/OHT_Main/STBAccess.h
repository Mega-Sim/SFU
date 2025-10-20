/**
@file    STBAccess.h
@version OHT 7.0
@brief   STBAccess Class Header File
*/

#ifndef STBAccessH
#define STBAccessH

#include "HWCommon.h"

/**
@class   STBAccess
@date    2012.10.27
@author  LSJ
@brief   STBAccess ฐüทร Class
*/
class STBAccess
{
private:
	HWCommon *m_pHWCommon;

public:
    STBAccess(HWCommon *pHWCommon);
    ~STBAccess();
    
    bool IsSTBLOHTDetect();
	bool IsSTBROHTDetect();
	bool IsSTBLWorkOn();
	bool IsSTBRWorkOn();
	int STBLWorkOn();
	int STBRWorkOn();
	int STBLWorkOff();
	int STBRWorkOff();
};

#endif  // STBAccessH