//---------------------------------------------------------------------------

#ifndef BlackBoxThreadH
#define BlackBoxThreadH
//---------------------------------------------------------------------------

#include "ThreadCtrl.h"
#include "BlackBox.h"
#include <time.h>
//---------------------------------------------------------------------------
class BlackBoxThread : public ThreadCtrl
{
private:
	void RealThreadProc(void);
	BlackBox* m_pBlackBox;
	DEFAULT_PARAM_SET *m_defualtparam;

public:
	BlackBoxThread();
	~BlackBoxThread();
};
//---------------------------------------------------------------------------
#endif


