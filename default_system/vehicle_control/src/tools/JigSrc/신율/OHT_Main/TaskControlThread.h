//---------------------------------------------------------------------------

#ifndef TaskControlThreadH
#define TaskControlThreadH
//---------------------------------------------------------------------------

#include "Def_OHT.h"
#include "TaskControl.h"
#include "ThreadCtrl.h"
#include <time.h>

/**
@class   TaskControlThread
@date    2013.05.20
@author  puting
@brief   TaskControlThread Class
*/
class TaskControlThread : public ThreadCtrl
{
private:
	void RealThreadProc(void);
	TaskControl* m_pControlTask;

public:
	TaskControlThread();
	~TaskControlThread();

};

#endif
