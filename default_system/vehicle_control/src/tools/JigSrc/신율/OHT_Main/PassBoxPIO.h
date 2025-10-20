//---------------------------------------------------------------------------

#ifndef PassBoxPIOH
#define PassBoxPIOH
//---------------------------------------------------------------------------

#include "HWCommon.h"

class PassBoxPIO
{
private:
	HWCommon *m_pHWCommon;

public:

	PassBoxPIO(HWCommon *pHWCommon);
	~PassBoxPIO();

	int PIOEnable();
	int AllPIOOff();
	int PIODisable(bool bOutClear);

	bool IsPBAVBLOn();
	bool IsESOn();
	bool IsGoSignalOn();
	bool IsCloseOn();
	bool IsOpenOn();
	bool IsBusyOn();

	int OHTStatusOnOff(bool bOnOff);
	int DoorSelectOnOff(bool bOnOff);
	int OpenReqOnOff(bool bOnOff);
	int ReadyOnOff(bool bOnOff);

	int Check_PB_AVBL_OffTime(int nPeriodTime);
	int Check_ESOffTime(int nPeriodTime);
	int Check_GoOffTime(int nPeriodTime);
};

#define PASSBOX_PIO_CHECK_GO_SIGNAL_WAIT_TIME				60000		
#define PASSBOX_PIO_CHECK_ES_SIGNAL_WAIT_TIME				60000		
#define PASSBOX_PIO_CHECK_PB_AVBL_SIGNAL_WAIT_TIME			60000		
#define PASSBOX_PIO_CHECK_CLOSE_ON_SIGNAL_TA1				60000		
#define PASSBOX_PIO_CHECK_BUSY_ON_SIGNAL_TA2				60000		
#define PASSBOX_PIO_CHECK_CLOSE_OFF_SIGNAL_TA3				60000		
#define PASSBOX_PIO_CHECK_BUSY_OFF_SIGNAL_TA4				60000		
#define PASSBOX_PIO_CHECK_OPEN_ON_WAIT_TIME					60000		
#define PASSBOX_PIO_CHECK_ABNORMAL_SIGNAL_WAIT_TIME			60000		

#define PASSBOX_PIO_CHECK_GO_SIGNAL_CHECK_TIME				200
#define PASSBOX_PIO_CHECK_ES_SIGNAL_CHECK_TIME				200
#define PASSBOX_PIO_CHECK_PB_AVBL_SIGNAL_CHECK_TIME			200

#endif

