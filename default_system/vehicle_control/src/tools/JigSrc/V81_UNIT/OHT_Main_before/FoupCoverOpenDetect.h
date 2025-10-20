//---------------------------------------------------------------------------

#ifndef FoupCoverOpenDetectH
#define FoupCoverOpenDetectH
//---------------------------------------------------------------------------


#include "HWCommon.h"

/**
@class   Ocsillation
@date    2012.10.22
@author
@brief   Ocsillation ฐüทร Class
*/
class FoupCoverOpenDetect
{

private:

	HWCommon	*m_pHWCommon;

public:
	FoupCoverOpenDetect(HWCommon *pHWCommon);
	~FoupCoverOpenDetect();

	bool IsDetect();

};


#endif
