//---------------------------------------------------------------------------


#pragma hdrstop

#include "VMSThread.h"
#include "OHTMain.h"


//---------------------------------------------------------------------------
VMSThread::VMSThread():ThreadCtrl()
{
	m_pVMS = OHTMainForm->GetVMS();
	m_defualtparam = OHTMainForm->GetDefaultParam();
}
//---------------------------------------------------------------------------
VMSThread::~VMSThread()
{

}

//---------------------------------------------------------------------------
void VMSThread::RealThreadProc(void)
{
	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker == true)  break;

        if(m_defualtparam->bUseVMS == true)
		{
			if(m_pVMS != NULL)
			{
				m_pVMS->SuperviseSendCmdDataOHT2VMS();
			}
			else
			{
			    m_bThreadBreaker = true;	
			}

			//AOHC-204 Thread 별 수행 시간 감시 기능
			static TimeChk timeChk;
			static ThreadTimeMonitor tmon;
			timeChk.End(0);

			tmon.Execute("VMST", timeChk.Get(0));

			timeChk.Start(0);
		}

		Sleep(100);
	}
}

//---------------------------------------------------------------------------
#pragma package(smart_init)


