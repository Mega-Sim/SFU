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
@brief   InnerFoup 관련 Class
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

#if(OHT_NAME == OHT_NAME_P4_MASK)
	/// mgchoi 2022-08-18. P4 MaskOHT용 추가함수 ///
	bool IsPODDetect();
	bool IsEUVPodDetect();
	bool IsRSP150Detect();
	bool IsRSP200Detect();
	////////////////////////////////////////////////
#endif
};

#endif  // InnerFoupH

