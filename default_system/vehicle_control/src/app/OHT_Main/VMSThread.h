//---------------------------------------------------------------------------

#ifndef VMSThreadH
#define VMSThreadH
//---------------------------------------------------------------------------

#include "ThreadCtrl.h"
#include "VMS.h"
#include <time.h>
//---------------------------------------------------------------------------
class VMSThread : public ThreadCtrl
{
private:
	void RealThreadProc(void);
	VMS* m_pVMS;
	DEFAULT_PARAM_SET *m_defualtparam;

public:
	VMSThread();
	~VMSThread();
};
//---------------------------------------------------------------------------
#endif


