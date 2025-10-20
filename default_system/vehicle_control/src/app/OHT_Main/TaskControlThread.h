//---------------------------------------------------------------------------

#ifndef TaskControlThreadH
#define TaskControlThreadH
//---------------------------------------------------------------------------

#include "Def_OHT.h"
#include "TaskControl.h"
#include "ThreadCtrl.h"
#include <time.h>

//#define OHTDETECT_HOKUYO   1
//#define OHTDETECT_FINE   	2
//#define OHTDETECT_SOSLAB   3

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
	DEFAULT_PARAM_SET *m_defualtparam;


public:
	TaskControlThread();
	~TaskControlThread();

};

#endif
