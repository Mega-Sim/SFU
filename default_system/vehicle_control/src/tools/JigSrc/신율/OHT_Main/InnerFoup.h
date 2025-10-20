/**
@file    InnerFoup.h
@version OHT 7.0
@brief   InnerFoup Class Header File
*/

#ifndef InnerFoupH
#define InnerFoupH

#include "HWCommon.h"

/**
@class   InnerFoup
@date    
@author  
@brief   InnerFoup ฐüทร Class
*/
class InnerFoup
{
private:
	HWCommon *m_pHWCommon;
	
public:
    InnerFoup(HWCommon *pHWCommon);
    ~InnerFoup();
    
	bool IsFoupDetect();
	bool IsEuvDetect();


};

#endif  // InnerFoupH

