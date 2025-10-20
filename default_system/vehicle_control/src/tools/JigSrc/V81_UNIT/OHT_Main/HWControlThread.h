//---------------------------------------------------------------------------

#ifndef HWControlThreadH
#define HWControlThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "ThreadCtrl.h"
#include "HWCommon.h"
#include "HWSet.h"
//---------------------------------------------------------------------------
class HWControlThread:public ThreadCtrl
{
protected:
	void RealThreadProc(void);	// Execute 와 같은 함수
public:
	HWControlThread();
	~HWControlThread();

private:
	HWCommon *m_pHWCommon;
    HWSet *m_pHWSet;
};
//---------------------------------------------------------------------------
#endif
