//---------------------------------------------------------------------------


#pragma hdrstop

#include "OHTMain.h"
#include "MonitorControlThread.h"

#define LOG_MONITORING(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\Monitoring.log", __VA_ARGS__)

//---------------------------------------------------------------------------
/**
@class   MonitorControl 생성자
@date    2013.06.03
@author  puting
*/
MonitorControlThread::MonitorControlThread(OHTMainStatus *pOHTMainStatus, Virtual_HID *pHID, NetworkSinalLib* pnetWorkDll, IPCLibrary *pIPCLibrary, CPUandMemInfo *pCPUandMemInfo, MONITORING *pMonitoring) : ThreadCtrl()
{
	m_pOHTMainStatus = pOHTMainStatus;
	m_pHID = pHID;         ///의문점 1
	m_pnetWorkDll = pnetWorkDll;
	m_pIPCLibrary =  pIPCLibrary;
	m_pCPUandMemInfo = pCPUandMemInfo;
	//m_pSigmaMonitor = pSigmaMonitor;
	m_pMonitoring = pMonitoring;

	IsInitDevice();

}
//--------------------------------------------------------------------------
/**
@class   MonitorControl 소멸자
@date    2013.06.03
@author  puting
*/
MonitorControlThread::~MonitorControlThread()
{
	m_pOHTMainStatus = NULL;
	m_pHID = NULL;
	m_pnetWorkDll = NULL;
	m_pIPCLibrary = NULL;
	m_pCPUandMemInfo = NULL;
	m_pMonitoring = NULL;

}
//--------------------------------------------------------------------------
/**
@class   MonitorControl Thread 함수
@date    2013.06.03
@author  puting
@brief   HID/Network/IPC 정보 갱신 및 Socket/Monitoring(Motor 정보) Updata
*/
void MonitorControlThread::RealThreadProc(void)
{

//	DWORD	dwHIDTrigerTime	= timeGetTime();     //함수 진입 시간.
//	DWORD	dwServoTrigerTime	= timeGetTime();     //함수 진입 시간.
//	DWORD   dwOHTTrigerTime    = timeGetTime();
//	int nError = NO_ERR;

	AnsiString strLog;
	//static AnsiString strPreLog = "";

	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker==true)	break;	// Thread 종료를 위해 필수로 추가



		//1. HID 정보 Updata  및 HID 정보 얻기
		if(m_pHID != NULL)
		{
			if(m_pHID->UpdateData() == NO_ERR)
			{
				m_pOHTMainStatus->m_cpMonitorData.HIDData = m_pHID->GetHIDData();
			}
		}

		//2. NetWork Data 얻기
		if(m_pnetWorkDll!=NULL)
		{
			m_pnetWorkDll->Get_WLanInfo(&m_pOHTMainStatus->m_cpMonitorData.APSignalData);

		}
		//3. IPC 정보 얻기
		if(m_pIPCLibrary !=NULL)
		{
			m_pIPCLibrary->GetIPCData(&m_pOHTMainStatus->m_cpMonitorData.IPCData);
			Get_IPCSysInfo(&m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.IPCSysInfo);

		}
		//4. 메모리 및 CPU 점유률 정보 얻기


		//5. 기타 모니터링 정보 얻기
		if(m_pMonitoring !=NULL)
		{
				m_pMonitoring->MonitorUpdate(&m_pOHTMainStatus->m_cpMonitorData); //값을 계속 읽음.

				//초기에 한번만 수행되는 부분
				if(m_pMonitoring->IsWinSetupLoaded() == false)
				{
					m_pMonitoring->MonitorInitData(&m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus);
                }
		}

		Sleep(500);
	}

}
//----------------------------------------------------------------------------
/**
@class   IsInitDevice 함수
@date    2013.06.03
@author  puting
@brief   Device 초기화 Check
*/
void MonitorControlThread::IsInitDevice()
{

	if(m_pHID->IsHIDOpen() != true )
	{
		m_pHID = NULL;
	}
	if(m_pnetWorkDll->NETWORKInit()!= 0)
	{
		m_pnetWorkDll = NULL;
	}
	if(m_pIPCLibrary->IsIPCReady() != true)
	{
		m_pIPCLibrary = NULL;
	}
// if(m_MonitorControl->gm_pSigmaMonitor->IsCommOpen() != true)
// {
//	m_MonitorControl->gm_pSigmaMonitor = NULL;
// }

}



#pragma package(smart_init)
