//---------------------------------------------------------------------------


#pragma hdrstop

#include "AvsControlThread.h"
#include "OHTMain.h"


//---------------------------------------------------------------------------
AvsControlThread::AvsControlThread():ThreadCtrl()
{
	m_pAvsControl = NULL;
//	m_pAvsControl = OHTMainForm->GetAvsControl();
}
//---------------------------------------------------------------------------
AvsControlThread::~AvsControlThread()
{

}

//---------------------------------------------------------------------------
void AvsControlThread::RealThreadProc(void)
{
	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker==true)  break;
		if(m_pAvsControl->ReceiveVibData() != NO_ERR)
		{
			m_bThreadBreaker = true;
		}

		Sleep(25);
	}
}

//---------------------------------------------------------------------------
#pragma package(smart_init)


