//---------------------------------------------------------------------------


#pragma hdrstop

#include "BlackBoxThread.h"
#include "OHTMain.h"


//---------------------------------------------------------------------------
BlackBoxThread::BlackBoxThread():ThreadCtrl()
{
	m_pBlackBox = OHTMainForm->GetBlackBox();
	m_defualtparam = OHTMainForm->GetDefaultParam();
}
//---------------------------------------------------------------------------
BlackBoxThread::~BlackBoxThread()
{

}

//---------------------------------------------------------------------------
void BlackBoxThread::RealThreadProc(void)
{
	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker == true)  break;

        if(m_defualtparam->bUse4ChBlackBox == true)
		{
			if(m_pBlackBox != NULL)
			{
				m_pBlackBox->ExcuteTimeCheck();

				m_pBlackBox->BlackBoxSocketUpdate();

				m_pBlackBox->SuperviseSendDataOHT2BLACKBOX();
			}
			else
			{
			    m_bThreadBreaker = true;	
			}

			//AOHC-204 Thread 별 수행 시간 감시 기능
			static TimeChk timeChk;
			static ThreadTimeMonitor tmon;
			timeChk.End(0);

			tmon.Execute("BlackBoxT", timeChk.Get(0));

			timeChk.Start(0);
		}

		Sleep(100);
	}
}

//---------------------------------------------------------------------------
#pragma package(smart_init)


