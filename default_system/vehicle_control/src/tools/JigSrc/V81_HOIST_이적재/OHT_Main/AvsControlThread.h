//---------------------------------------------------------------------------

#ifndef AvsControlThreadH
#define AvsControlThreadH
//---------------------------------------------------------------------------

#include "ThreadCtrl.h"
#include "AvsControl.h"
#include <time.h>
//---------------------------------------------------------------------------
class AvsControlThread : public ThreadCtrl
{
private:
	void RealThreadProc(void);
	AvsControl* m_pAvsControl;
public:
	AvsControlThread();
	~AvsControlThread();
};
//---------------------------------------------------------------------------
#endif


